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
	bIsActive = false;
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
	
	if (!bIsActive || !ControlledPawn) return;
	
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
	const float Lookahead = FMath::Clamp(LookaheadDist + SpdCmS * LookaheadSpeedScale, LookaheadMin, LookaheadMax);
	const float LookaheadPos = FMath::Fmod(CurrentSplineDistance + Lookahead, SplineLength);
	
	const FVector TargetLocation = SplineComp->GetLocationAtDistanceAlongSpline(LookaheadPos, ESplineCoordinateSpace::World);
	
	//curvature
	const float Curvature = GetSplineCurvature(SplineComp, CurrentSplineDistance);
	
	
	//Obstacle check
	const float ObstacleBias = CheckObstacles();
	const bool bObstacleAhead = FMath::Abs(ObstacleBias) > 0.1f;
	
	//Steer
	float RawSteerVal = CalcSteer(TargetLocation);
	RawSteerVal = FMath::Clamp(RawSteerVal + ObstacleBias, -1.0f, 1.0f);
	const float SteerSpeed = bObstacleAhead ? 8.f : 4.f;
	steerValue = FMath::FInterpTo(steerValue, RawSteerVal, DeltaTime, SteerSpeed);
	
	
	//Throttle
	float Throttle = CalcThrottle(SpdKmh, RawSteerVal, Curvature);
	if (bObstacleAhead)
	{
		const float DistToObst = ObstacleTraceDist *(1.0f - FMath::Abs(ObstacleBias));
		if (DistToObst < BrakeOnObstacleDist){ Throttle = 0.0f;}
	}
	
	//Brake
	//check dist to obstacle, if too close, brake
	 float Brake = 0.0f;
	//curvature
	const float AheadCurvature = GetSplineCurvature(SplineComp, FMath::Fmod(CurrentSplineDistance + Lookahead * 0.5f, SplineLength));
	
	if (AheadCurvature > CornerBrakeThreshold)
	{
		Brake = CornerBrakeStrength * (AheadCurvature - CornerBrakeThreshold);
		Throttle *= (1.0f - Brake * 0.5f);
	}
	
	if (bObstacleAhead)
	{
		const float DistToObst = ObstacleTraceDist * (1.0f - FMath::Abs(ObstacleBias));
		if (DistToObst < BrakeOnObstacleDist)
		{
			Throttle = FMath::Min(Throttle, 0.3f);
		}
		if (DistToObst < BrakeOnObstacleDist * 0.5f)
		{
			Brake = FMath::Max(Brake, 0.6f);
			Throttle = 0.0f;
		}
	}
	
	//Vehicle Input
	VehicleInput->ApplySteer(RawSteerVal);
	VehicleInput->ApplyBrake(Brake);
	VehicleInput->ApplyThrottle(Throttle);
	
	//Debug draw
	DrawDebugSphere(GetWorld(), TargetLocation, 30.f, 8, FColor::Green, false, -1.f);
	DrawDebugLine(GetWorld(), ControlledPawn->GetActorLocation(), TargetLocation, FColor::Magenta, false, -1.0f);
	

}

float ACVehicleAIController::GetSplineCurvature(USplineComponent* Spline, float Distance) const
{
	return 0;
}
float ACVehicleAIController::CalcSteer(const FVector& TargetLocation) const
{
	const FVector ToTarget = (TargetLocation - ControlledPawn->GetActorLocation()).GetSafeNormal();
	const float ForwardDot = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), ToTarget);
	const float RightDot = FVector::DotProduct(ControlledPawn->GetActorRightVector(), ToTarget);
	float Steer = FMath::Clamp(RightDot * SteeringSensitivity, -1.0f, 1.0f);
	
	// Multiply for turn around 
	if (ForwardDot < 0.0f)
		Steer = FMath::Clamp(Steer * 1.5f, -1.f, 1.f);
	return Steer;
}

float ACVehicleAIController::CalcThrottle(float CurrentSpeedKmh, float SteeringValue, float SplineCurvature)
{
	const float TurnFactor = 1.0f - FMath::Abs(SteeringValue) * 0.5f;
	const float SPDFactor = CurrentSpeedKmh > ThrottleReduceSPD ? MaxThrottleHighSpeed : MaxThrottle;
	return FMath::Clamp(TurnFactor * SPDFactor, 0.0f, MaxThrottle);
}


float ACVehicleAIController::CheckObstacles() const
{
	if (!ControlledPawn) return 0.0f;
	UWorld* World = GetWorld();
	if (!World) return 0.0f;
	
	const FVector Start = ControlledPawn->GetActorLocation();
	const FVector Foward = ControlledPawn->GetActorForwardVector();
	const FVector Right = ControlledPawn->GetActorRightVector();
	
	FCollisionQueryParams  Params;
	Params.AddIgnoredActor(ControlledPawn);
	
	FCollisionShape Box = FCollisionShape::MakeBox(FVector(ObstacleTraceDist * 0.5f, ObstacleTraceHalfWidth, 60.0f));
	
	//Center
	const FVector MiddleEnd = Start + Foward * ObstacleTraceDist;
	FHitResult MiddleHit;
	//sweep
	const bool bMiddleHit = World->SweepSingleByChannel(
		MiddleHit, Start, MiddleEnd, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(ObstacleTraceHalfWidth),
		Params);
	
	if (!bMiddleHit) return 0.0f;
	
	//left, steer to right
	const FVector LeftEnd = Start +(Foward - Right * 0.5f).GetSafeNormal() * ObstacleTraceDist;
	FHitResult LeftHit;
	const bool bLeftHit = World->SweepSingleByChannel(
		LeftHit, Start, LeftEnd, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(ObstacleTraceHalfWidth),
		Params);
	if (!bLeftHit) return 0.0f;
	
	//Rihgt, steer to left
	const FVector RightEnd = Start +(Foward + Right * 0.5f).GetSafeNormal() * ObstacleTraceDist;
	FHitResult RightHit;
	const bool bRightHit = World->SweepSingleByChannel(
		RightHit, Start, RightEnd, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(ObstacleTraceHalfWidth),
		Params);
	if (!bLeftHit) return 0.0f;
	
	
	//bias to clear
	const float HitDist = MiddleHit.Distance;
	const float Strength = 1.0f - (HitDist/ObstacleTraceDist);
	
	if (!bLeftHit) return -AvoidanceSteerStrength * Strength;
	if (!bRightHit) return AvoidanceSteerStrength * Strength;
	
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

