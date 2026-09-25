// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/PlayerWidget.h"
#include "Widgets/VehicleWidget.h"

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::OnPossess()"))
	PlayerCharacter = Cast<APlayerCharacter>(NewPawn);
	if (!PlayerCharacter)
	{
		VehiclePawn = Cast<ACVehiclePawn>(NewPawn);
		VehicleMovementComponent = VehiclePawn->GetVehicleMovementComponent();
		RemoveWidget();
		SpawnVehicleWidget();
		return;	
	};
	RemoveWidget();
	SpawnPlayerWidget();
}

void ACPlayerController::SpawnPlayerWidget()
{
	if (!PlayerWidgetClass) return;
	PlayerWidget = CreateWidget<UPlayerWidget>(this,PlayerWidgetClass);
	PlayerWidget->AddToViewport();
}

void ACPlayerController::SpawnVehicleWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("Vehicle Widget"))
	if (!VehicleWidgetClass) return;
	VehicleWidget = CreateWidget<UVehicleWidget>(this,VehicleWidgetClass);
	VehicleWidget->AddToViewport();
}

void ACPlayerController::RemoveWidget()
{
	if (!VehicleWidget || !PlayerWidget) return;
	PlayerWidget->RemoveFromViewport();
	VehicleWidget->RemoveFromViewport();
}
