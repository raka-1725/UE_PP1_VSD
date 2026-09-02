// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CVehicleAIController.h"

#include "Vehicle/CVehiclePawn.h"

ACVehicleAIController::ACVehicleAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACVehicleAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledPawn = InPawn;
	
	VehicleInput = Cast<IVehicleInputInterface>(InPawn);
	
	if (VehicleInput)
	{
		VehicleInput->OnAIControl();
	}
}

void ACVehicleAIController::OnUnPossess()
{
	Super::OnUnPossess();
	
	if (VehicleInput)
	{
		ZeroInputs();
		VehicleInput->OnControlReleased();
	}

	VehicleInput = nullptr;
	ControlledPawn = nullptr;


}

void ACVehicleAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DriveTowardTarget(DeltaTime);
}

void ACVehicleAIController::DriveTowardTarget(float DeltaTime) const
{
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	
	const FVector ToTarget = TargetLocation - PawnLocation;
	const float DistToTarget = ToTarget.Size();
	
	if (DistToTarget <= StoppingDistance)
	{
		ZeroInputs();
		VehicleInput->ApplyBrake(1.0f);
		return;
	}
	
	const FVector ForwardVector = ControlledPawn->GetActorForwardVector();
	const FVector ToTargetNormalized = ToTarget.GetSafeNormal();

	const float ForwardDot = FVector::DotProduct(ForwardVector, ToTargetNormalized);
	const float RightDot = FVector::DotProduct(ControlledPawn->GetActorRightVector(), ToTargetNormalized);

	float SteeringValue = FMath::Clamp(RightDot * SteeringSensitivity, -1.f, 1.f);
	
	if (ForwardDot < 0.f)
	{
		SteeringValue = FMath::Clamp(SteeringValue * 1.5f, -1.f, 1.f);
	}

	VehicleInput->ApplySteer(SteeringValue);
	VehicleInput->ApplyThrottle(MaxThrottle);
	VehicleInput->ApplyBrake(0.f);
	
}

void ACVehicleAIController::ZeroInputs() const
{
	if (!VehicleInput) return;

	VehicleInput->ApplySteer(0.f);
	VehicleInput->ApplyThrottle(0.f);
	VehicleInput->ApplyBrake(0.f);
}

