// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
	
	ACAIController();
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float MaxThrottle = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float SteeringSensitivity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float StoppingDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "AI Driving")
	AActor* TargetActor = nullptr;
	
private:
	UPROPERTY()
	ACVehiclePawn* ControlledVehicle = nullptr;

	void DriveTowardTarget(float DeltaTime);
};
