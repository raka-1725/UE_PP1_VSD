// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class ACVehiclePawn;

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TryEnterVehicle();
	void OnExitVehicle(FTransform ExitVehicleTransform);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* CharacterMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* VehicleInteractAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Vehicle")
	float InteractRadius = 300.0f;
	
private:
	void Input_Move(const struct FInputActionValue& value);
	void Input_Look(const struct FInputActionValue& value);
	
	UPROPERTY()
	ACVehiclePawn* CurrentVehicle = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class UCameraComponent* ViewCam;
};
