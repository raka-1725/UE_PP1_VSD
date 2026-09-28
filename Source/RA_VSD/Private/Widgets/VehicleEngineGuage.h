// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "VehicleEngineGuage.generated.h"
/**
 * 
 */
UCLASS()
class RA_VSD_API UVehicleEngineGuage : public UUserWidget
{
	GENERATED_BODY()
	
	//130 - -130 for needle rotation
public:
	virtual void NativeConstruct() override;
	void SetRPMValue(float currentRPM, float MaxRPM);
	void SetGear(int32 currentGear);
	void SetHandBrakeIcon(bool BrakeSet);
	
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RPMText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GearText;
	
	UPROPERTY(meta = (BindWidget))
	UImage* RPMNeedle;
	
	UPROPERTY(meta = (BindWidget))
	UImage* HandBrakeIcon;
	
	UPROPERTY(EditAnywhere, Category = "Indicators")
	FLinearColor HandBrakeColor;
	
	UPROPERTY(EditAnywhere, Category = "Indicators")
	FLinearColor IconIdleColor;
	
};
