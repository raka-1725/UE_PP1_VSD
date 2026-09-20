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
	float MaxThrottle = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float SteeringSensitivity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float StoppingDistance = 300.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float ThrottleReduceSPD = 120.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Driving")
	float MaxThrottleHighSpeed = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "AI Driving")
	float MaxSPD = 100.0f;
	
	// AI Path
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float LookaheadDist = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float LookaheadSpeedScale = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float LookaheadMin = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float LookaheadMax = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float WaypointReachRadius = 300.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Driving")
	float CornerBrakeThreshold = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Driving")
	float CornerBrakeStrength = 0.4f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float ResyncThreshold = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Path")
	float ResyncSearchWindow = 3000.f;
	
	// Obstacle avoidance
	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float ObstacleTraceDist = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float ObstacleTraceHalfWidth = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float AvoidanceSteerStrength = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Avoidance")
	float BrakeOnObstacleDist = 400.f;
	

	
private:
	IVehicleInputInterface* VehicleInput = nullptr;
	
	UPROPERTY()
	APawn* ControlledPawn = nullptr;
	
	UPROPERTY()
	ACVehiclePawn* ControlledVehicle = nullptr;
	
	float CurrentSplineDistance = 0.0f;
	bool bIsActive = false;
	//steer val
	float steerValue = 0.0f;
	float smoothsteerValue = 0.0f;
	//Driving Func 
	void FollowSpline(float DeltaTime);
	float CalcSteer(const FVector& TargetLocation) const; 
	float CalcThrottle(float CurrentSpeedKmh, float SteeringValue, float SplineCurvature);
	float GetSplineCurvature(USplineComponent* SplineComp, float Distance) const;
	
	//obstacle
	float CheckObstacles() const;
	
	USplineComponent* GetSpline() const;
	
	void ZeroInputs() const;
};
