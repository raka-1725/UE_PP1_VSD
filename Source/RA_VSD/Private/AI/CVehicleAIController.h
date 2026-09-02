// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Vehicle/VehicleInputInterface.h"
#include "CVehicleAIController.generated.h"

class ACVehiclePawn;
/**
 * 
 */
UCLASS()
class ACVehicleAIController : public AAIController
{
	GENERATED_BODY()
	
	ACVehicleAIController();
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
	IVehicleInputInterface* VehicleInput = nullptr;
	
	UPROPERTY()
	APawn* ControlledPawn = nullptr;

	void DriveTowardTarget(float DeltaTime) const;
	
	void ZeroInputs() const;
};
