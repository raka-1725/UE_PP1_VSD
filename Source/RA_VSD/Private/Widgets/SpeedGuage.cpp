// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SpeedGuage.h"

#include "Components/TextBlock.h"

void USpeedGuage::NativeConstruct()
{
	Super::NativeConstruct();
}

void USpeedGuage::SetValue(float NewValue)
{
	static FNumberFormattingOptions FormattingOptions = FNumberFormattingOptions().SetMaximumFractionalDigits(0);
	ValueText->SetText(FText::AsNumber(NewValue));
}
