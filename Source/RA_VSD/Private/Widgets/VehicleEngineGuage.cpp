// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleEngineGuage.h"


void UVehicleEngineGuage::NativeConstruct()
{
	Super::NativeConstruct();
}

void UVehicleEngineGuage::SetRPMValue(float currentRPM, float MaxRPM)
{
	if (!RPMText) return;
	
	int32 RPMInt = FMath::RoundToInt(currentRPM);
	RPMText->SetText(FText::AsNumber(RPMInt));
	
	float ClampedRPM = FMath::Clamp(currentRPM, 0.0f, MaxRPM);
	
	float TargetAngle = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, MaxRPM), FVector2D(-130.0f, 130.0f),
		ClampedRPM);
	
	RPMNeedle->SetRenderTransformAngle(TargetAngle);
}

void UVehicleEngineGuage::SetGear(int32 CurrentGear)
{
	if (!GearText) return;

	FString GearString;
	if (CurrentGear < 0)
	{
		GearString = TEXT("R");
	}
	else if (CurrentGear == 0)
	{
		GearString = TEXT("N");
	}
	else
	{
		GearString = FString::FromInt(CurrentGear);
	}

	GearText->SetText(FText::FromString(GearString));
}
