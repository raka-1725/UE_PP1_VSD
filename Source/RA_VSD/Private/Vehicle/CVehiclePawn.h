// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"

#include "Vehicle/VehicleInputInterface.h"

#include "CVehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class ACVehiclePawn : public AWheeledVehiclePawn, public IVehicleInputInterface
{
	GENERATED_BODY()
public:
	ACVehiclePawn();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
//Possession
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	
// VehicleInputinterface

	virtual void ApplySteering(float Value) override;
	virtual void ApplyThrottle(float Value) override;
	virtual void ApplyBrake(float Value) override;
	virtual void OnPlayerControl() override;
	virtual void OnAIControl() override;
	virtual void OnControlReleased() override;
	virtual bool IsPlayerDriving() const override;
	
	
//Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* VehicleMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SteerInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ThrottleInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* BrakeInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 MappingPriority = 0;
	

private:
	//Only player
	void Input_Steer(const FInputActionValue& value);
	void Input_Throttle(const FInputActionValue& Val);
	void Input_Brake(const FInputActionValue& Val);
	
	void AddMappingContext(APlayerController* PlayerController);
	void RemoveMappingContext(APlayerController* PlayerController);
	
	
	//Camera
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class UCameraComponent* ViewCam;
	
	UPROPERTY()
	bool bIsPlayerDriving = false;
	
};
