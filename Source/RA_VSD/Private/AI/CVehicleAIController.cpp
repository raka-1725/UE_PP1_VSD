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
	
	ControlledVehicle = Cast<ACVehiclePawn>(InPawn);
}

void ACVehicleAIController::OnUnPossess()
{
	Super::OnUnPossess();
	
	if (ControlledVehicle)
	{
		ControlledVehicle->Steer(0.f);
		ControlledVehicle->Throttle(0.f);
		ControlledVehicle->Brake(0.f);
	}

	ControlledVehicle = nullptr;

}

void ACVehicleAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DriveTowardTarget(DeltaTime);
}

void ACVehicleAIController::DriveTowardTarget(float DeltaTime) const
{
	const APawn* VehiclePawn = ControlledVehicle;
	const FVector PawnLocation = VehiclePawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	
	const FVector ToTarget = TargetLocation - PawnLocation;
	const float DistToTarget = ToTarget.Size();
	
	if (DistToTarget <= StoppingDistance)
	{
		ControlledVehicle->Steer(0.f);
		ControlledVehicle->Throttle(0.f);
		ControlledVehicle->Brake(1.f);
	}
	
	const FVector ForwardVector = VehiclePawn->GetActorForwardVector();
	const FVector ToTargetNormalized = ToTarget.GetSafeNormal();

	const float ForwardDot = FVector::DotProduct(ForwardVector, ToTargetNormalized);
	const float RightDot = FVector::DotProduct(VehiclePawn->GetActorRightVector(), ToTargetNormalized);

	float SteeringValue = FMath::Clamp(RightDot * SteeringSensitivity, -1.f, 1.f);
	
	if (ForwardDot < 0.f)
	{
		SteeringValue = FMath::Clamp(SteeringValue * 1.5f, -1.f, 1.f);
	}

	ControlledVehicle->Steer(SteeringValue);
	ControlledVehicle->Throttle(MaxThrottle);
	ControlledVehicle->Brake(0.f);
}


