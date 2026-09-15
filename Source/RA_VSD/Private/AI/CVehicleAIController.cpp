// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CVehicleAIController.h"

#include "AssetTypeCategories.h"
#include "AI/SplinePathActor.h"
#include "Vehicle/CVehiclePawn.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"

ACVehicleAIController::ACVehicleAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACVehicleAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledPawn = InPawn;
	ControlledVehicle = Cast<ACVehiclePawn>(InPawn);
	VehicleInput = Cast<IVehicleInputInterface>(InPawn);
	
	if (VehicleInput)
	{
		VehicleInput->OnAIControl();
	}
	
	if (USplineComponent* SplineComp = GetSpline())
	{
		CurrentSplineDistance = SplineComp->
		GetDistanceAlongSplineAtLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
	}
	
	bIsActive = true;
	UE_LOG(LogTemp, Warning, TEXT("ACVehicleAIController::OnPossess()"));
}

void ACVehicleAIController::OnUnPossess()
{
	ZeroInputs();
	
	if (VehicleInput)
	{
		ZeroInputs();
		VehicleInput->OnControlReleased();
	}

	VehicleInput = nullptr;
	ControlledPawn = nullptr;
	ControlledVehicle = nullptr;

	Super::OnUnPossess();
}

void ACVehicleAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsActive) return;
	
	FollowSpline(DeltaTime);
}

void ACVehicleAIController::TakeOverFromPlayer()
{
	bIsActive = true;
	UE_LOG(LogTemp, Warning, TEXT("taking over from player"));

	if (USplineComponent* SplineComp = GetSpline())
	{
		CurrentSplineDistance = SplineComp->
		GetDistanceAlongSplineAtLocation(ControlledPawn->GetActorLocation()
			, ESplineCoordinateSpace::World);
	}
}


void ACVehicleAIController::FollowSpline(float DeltaTime)
{
	USplineComponent* SplineComp = GetSpline();
	if (!SplineComp) return;
	
	const float SplineLength = SplineComp->GetSplineLength();
	
	const FVector VehicleVelocity = ControlledPawn->GetVelocity();
	const float SpdCmS = VehicleVelocity.Size();
	const float SpdKmh = SpdCmS * 0.036f;
	//modular - clamp/wrap for looping spline
	CurrentSplineDistance = FMath::Fmod(CurrentSplineDistance + SpdCmS * DeltaTime,SplineLength);
	
	//LookAhead
	const float Lookahead = FMath::Fmod(CurrentSplineDistance + LookaheadDist, SplineLength);
	const FVector TargetLocation = SplineComp->GetLocationAtDistanceAlongSpline(Lookahead, ESplineCoordinateSpace::World);
	
	//Obstacle check
	const float ObstacleBias = CheckObstacles();
	const bool bObstacleAhead = FMath::Abs(ObstacleBias) > 0.1f;
	
	//Steer
	float SteerVal = CalcSteer(TargetLocation);
	SteerVal = FMath::Clamp(SteerVal + ObstacleBias, -1.0f, 1.0f);
	
	//Throttle
	float Throttle = CalcThrottle(SpdKmh, SteerVal);
	if (bObstacleAhead)
	{
		const float DistToObst = ObstacleTraceDist *(1.0f - FMath::Abs(ObstacleBias));
		if (DistToObst < BrakeOnObstacleDist){ Throttle = 0.0f;}
	}
	
	//Brake
	//check dist to obstacle, if too close, brake
	const float Brake = (
		bObstacleAhead && ObstacleTraceDist * (1.0f - FMath::Abs(ObstacleBias)) 
		< BrakeOnObstacleDist * 0.5f) ? 0.6f : 0.0f;
	
	//Vehicle Input
	VehicleInput->ApplySteer(SteerVal);
	VehicleInput->ApplyBrake(Brake);
	VehicleInput->ApplyThrottle(Throttle);
	
	//Debug draw
	DrawDebugSphere(GetWorld(), TargetLocation, 30.f, 8, FColor::Green, false, -1.f);
	DrawDebugLine(GetWorld(), ControlledPawn->GetActorLocation(), TargetLocation, FColor::Magenta, false, -1.0f);
}

float ACVehicleAIController::CalcSteer(const FVector& TargetLocation) const
{
	const FVector ToTarget = (TargetLocation - ControlledPawn->GetActorLocation().GetSafeNormal());
	const float ForwardDot = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), ToTarget);
	const float RightDot = FVector::DotProduct(ControlledPawn->GetActorRightVector(), ToTarget);
	float Steer = FMath::Clamp(RightDot * SteeringSensitivity, -1.0f, 1.0f);
	
	// Multiply for turn around 
	if (ForwardDot < 0.0f)
		Steer = FMath::Clamp(Steer * 1.5f, -1.f, 1.f);
	return Steer;
}

float ACVehicleAIController::CalcThrottle(float CurrentSpeedKmh, float SteeringValue)
{
	const float TurnFactor = 1.0f - FMath::Abs(SteeringValue) * 0.5f;
	const float SPDFactor = CurrentSpeedKmh > ThrottleReduceSPD ? MaxThrottleHighSpeed : MaxThrottle;
	return FMath::Clamp(TurnFactor * SPDFactor, 0.0f, MaxThrottle);
}

float ACVehicleAIController::CheckObstacles() const
{
	return 0;
}


USplineComponent* ACVehicleAIController::GetSpline() const
{
	if (!AISplinePath) return nullptr;
	return AISplinePath->FindComponentByClass<USplineComponent>();
}

void ACVehicleAIController::ZeroInputs() const
{
	if (!VehicleInput) return;

	VehicleInput->ApplySteer(0.f);
	VehicleInput->ApplyThrottle(0.f);
	VehicleInput->ApplyBrake(0.f);
}

