// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VehicleWidget.generated.h"

/**
 * 
 */
UCLASS()
class UVehicleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class USpeedGuage* SpeedGuage;
	
	UPROPERTY(meta=(BindWidget))
	class UVehicleWidget* VehicleWidget;
	
	UPROPERTY()
	const class UChaosVehicleMovementComponent* VehicleMovementComponent;
	
};
