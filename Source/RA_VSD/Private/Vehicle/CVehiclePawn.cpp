// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/CVehiclePawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AI/CVehicleAIController.h"

ACVehiclePawn::ACVehiclePawn()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	
	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	
	VehicleAIController = ACVehicleAIController::StaticClass();
	AIControllerClass = VehicleAIController;
	
	//disable for player, enable for ai
	AutoPossessAI = EAutoPossessAI::Disabled;
}	


void ACVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACVehiclePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SteerInputAction)
		{
			EnhancedInputComponent->BindAction(SteerInputAction, ETriggerEvent::Triggered,this, &ACVehiclePawn::Steer);
		}
		
		if (ThrottleInputAction)
		{
			EnhancedInputComponent->BindAction(ThrottleInputAction, ETriggerEvent::Triggered, this, &ACVehiclePawn::Throttle);
		}
		
		if (BrakeInputAction)
		{
			EnhancedInputComponent->BindAction(BrakeInputAction, ETriggerEvent::Triggered,this, &ACVehiclePawn::Brake);
		}
	}
}

void ACVehiclePawn::Steer(const FInputActionValue& val)
{
}

void ACVehiclePawn::Throttle(const FInputActionValue& val)
{
}

void ACVehiclePawn::Brake(const FInputActionValue& val)
{
}
