// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeedGuage.generated.h"

/**
 * 
 */
UCLASS()
class USpeedGuage : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeConstruct() override;
	void SetValue(float NewValue);
	
private:
	UPROPERTY(meta =(BindWidget))
	class UTextBlock* ValueText;
	
	
};
