// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "Vehicle/CVehiclePawn.h"

ACAIController::ACAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledVehicle = Cast<ACVehiclePawn>(InPawn);
}

void ACAIController::OnUnPossess()
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

void ACAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DriveTowardTarget(DeltaTime);
}

void ACAIController::DriveTowardTarget(float DeltaTime)
{
	const APawn* Pawn = ControlledVehicle;
	const FVector PawnLocation = Pawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	
	const FVector ToTarget = TargetLocation - PawnLocation;
	const float DistToTarget = ToTarget.Size();
	
	if (DistToTarget <= StoppingDistance)
	{
		ControlledVehicle->Steer(0.f);
		ControlledVehicle->Throttle(0.f);
		ControlledVehicle->Brake(1.f);
	}
	
	const FVector ForwardVector = Pawn->GetActorForwardVector();
	const FVector ToTargetNormalized = ToTarget.GetSafeNormal();

	const float ForwardDot = FVector::DotProduct(ForwardVector, ToTargetNormalized);
	const float RightDot = FVector::DotProduct(Pawn->GetActorRightVector(), ToTargetNormalized);

	float SteeringValue = FMath::Clamp(RightDot * SteeringSensitivity, -1.f, 1.f);
	
	if (ForwardDot < 0.f)
	{
		SteeringValue = FMath::Clamp(SteeringValue * 1.5f, -1.f, 1.f);
	}

	ControlledVehicle->Steer(SteeringValue);
	ControlledVehicle->Throttle(MaxThrottle);
	ControlledVehicle->Brake(0.f);
}


