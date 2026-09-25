// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Vehicle/CVehiclePawn.h"
#include "CPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* NewPawn) override;
	
private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
	
	UPROPERTY()
	ACVehiclePawn* VehiclePawn;
	
	UPROPERTY()
	UChaosVehicleMovementComponent* VehicleMovementComponent;
	
	
	//widgets
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UVehicleWidget> VehicleWidgetClass;
	
	UPROPERTY()
	UVehicleWidget* VehicleWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UPlayerWidget> PlayerWidgetClass;
	
	UPROPERTY()
	UPlayerWidget* PlayerWidget;
	
	
	void SpawnPlayerWidget();
	void SpawnVehicleWidget();
	void RemoveWidget();
	
};
