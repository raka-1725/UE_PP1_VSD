// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerCharacter.h"
#include "UObject/Interface.h"
#include "VehicleInputInterface.generated.h"

/**
 * 
 */
//Minimal API for less exposed method
UINTERFACE(MinimalAPI)
class UVehicleInputInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IVehicleInputInterface
{
	GENERATED_BODY()
	
public:
	virtual void ApplySteer(float val) = 0;
	virtual void ApplyThrottle(float val) = 0;
	virtual void ApplyBrake(float val) = 0;
	
	virtual void OnPlayerControl() = 0;
	virtual void OnAIControl() = 0;
	virtual void OnControlReleased() = 0;
	
	virtual bool IsPlayerDriving() const = 0;
	
	//Enter / Exit
	
	virtual void EnterVehicle(AController* NewDriver) = 0;
	virtual void ExitVehicle(AController* Exit) = 0;
	virtual bool CanEnterVehicle(APlayerCharacter* PlayerCharacter) const = 0;
};