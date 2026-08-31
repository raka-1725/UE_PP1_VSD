// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "WheeledVehiclePawn.h"
#include "CVehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class ACVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()
public:
	ACVehiclePawn();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
//control input | player
	void Steer(const FInputActionValue& val);
	void Throttle(const FInputActionValue& val);
	void Brake(const FInputActionValue& val);
	
	

private:
	//Camera
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class UCameraComponent* ViewCam;
	
	//Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SteerInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ThrottleInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* BrakeInputAction;
	
	//AI Controller
	UPROPERTY(VisibleDefaultsOnly, Category = "AIController")
	TSubclassOf<class ACVehicleAIController> VehicleAIController;
	
	
	
};
