// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "WheeledVehiclePawn.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

#include "Vehicle/VehicleInputInterface.h"

#include "CVehiclePawn.generated.h"

class APlayerCharacter;
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
	
	
//Movementcomponent
	
	void CacheMovementComponent();
	UChaosVehicleMovementComponent* MovementComponent;
	UChaosWheeledVehicleMovementComponent* WheelMovement;
	UChaosWheeledVehicleMovementComponent* GetWMC() const;

	//Possession
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	
// VehicleInputinterface

	virtual void ApplySteer(float Value) override;
	virtual void ApplyThrottle(float Value) override;
	virtual void ApplyBrake(float Value) override;
	virtual void OnPlayerControl() override;
	virtual void OnAIControl() override;
	virtual void OnControlReleased() override;
	virtual bool IsPlayerDriving() const override;
	virtual bool CanEnterVehicle(APlayerCharacter* PlayerCharacter) const override;
	virtual void EnterVehicle(AController* NewDriver) override;
	virtual void ExitVehicle(AController* Exit) override;
	
	
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
	class UInputAction* VehicleInteractAction;
	
	//priority higher than player
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 MappingPriority = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle")
	FVector ExitOffset = FVector(0.f, 200.f, 100.f);
	

private:
	//Only player
	void Input_Steer(const FInputActionValue& value);
	void Input_Throttle(const FInputActionValue& Val);
	void Input_Brake(const FInputActionValue& Val);
	void Input_InteractVehicle();
	
	void AddMappingContext(APlayerController* PlayerController);
	void RemoveMappingContext(APlayerController* PlayerController);
	
	
	//Camera
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class UCameraComponent* ViewCam;
	
	UPROPERTY()
	bool bIsPlayerDriving = false;
	
	bool bCanExitVehicle = false;
	
	bool bIgnoreInteractInput = false;

//for mp
	UPROPERTY()
	APlayerCharacter* StoredDriver = nullptr;
	
	UPROPERTY()
	TArray<AController*> Passengers;
	
};
