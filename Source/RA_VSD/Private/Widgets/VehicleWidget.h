// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeedGuage.h"
#include "VehicleEngineGuage.h"
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
	USpeedGuage* GetSpeedWidgdet();
	UVehicleEngineGuage* GetVehicleEngineWidgdet();
private:
	UPROPERTY(meta=(BindWidget))
	class USpeedGuage* SpeedGuage;
	
	UPROPERTY(meta=(BindWidget))
	class UVehicleEngineGuage* VehicleEngWidget;
	
};
