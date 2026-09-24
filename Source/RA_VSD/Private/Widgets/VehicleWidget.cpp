// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/VehicleWidget.h"

#include "SpeedGuage.h"
#include "Player/PlayerCharacter.h"
#include "Vehicle/CVehiclePawn.h"

void UVehicleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ACVehiclePawn* VehiclePawn = Cast<APlayerCharacter>(GetOwningPlayer())->CurrentVehicle;
	VehicleMovementComponent = VehiclePawn->GetVehicleMovementComponent();
	
	SpeedGuage->SetValue(VehicleMovementComponent->GetForwardSpeedMPH());	
}
