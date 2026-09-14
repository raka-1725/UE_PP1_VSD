// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Vehicle/VehicleInputInterface.h"
#include "CVehicleAIController.generated.h"

class ACVehiclePawn;
class ASplinePathActor;
class USplineComponent;
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
	
	void TakeOverFromPlayer();
	
	UPROPERTY(EditAnywhere, Category = "AI Path")
	ASplinePathActor* AISplinePath = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float MaxThrottle = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float SteeringSensitivity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float StoppingDistance = 300.f;
	
	// AI Path
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float LookaheadDistance = 600.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float WaypointReachRadius = 300.f;
	
	// Obstacle avoidance
	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float ObstacleTraceDistance = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float ObstacleTraceHalfWidth = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float AvoidanceSteerStrength = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float BrakeOnObstacleDistance = 400.f;
	
	
	UPROPERTY(EditAnywhere, Category = "AI Driving")
	AActor* TargetActor = nullptr;
	
	
	
private:
	IVehicleInputInterface* VehicleInput = nullptr;
	
	UPROPERTY()
	APawn* ControlledPawn = nullptr;
	
	UPROPERTY()
	ACVehiclePawn* ControlledVehicle = nullptr;
	
	float CurrentSplineDistance = 0.f;
	
	bool bIActive = false;
	
	void DriveTowardTarget(float DeltaTime) const;
	
	void ZeroInputs() const;
};
