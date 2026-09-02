// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/CVehiclePawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AI/CVehicleAIController.h"

#include "ChaosWheeledVehicleMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ACVehiclePawn::ACVehiclePawn()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	
	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	
	//disable for player, enable for ai
	AIControllerClass = ACVehicleAIController::StaticClass();
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
//Possesion
void ACVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		OnPlayerControl();
		AddMappingContext(PlayerController);
	}
	else if (AAIController* AIController = Cast<AAIController>(NewController))
	{
		OnAIControl();
	}
}

void ACVehiclePawn::UnPossessed()
{
	Super::UnPossessed();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		RemoveMappingContext(PlayerController);
	}
	
	OnControlReleased();
}


//Input
void ACVehiclePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SteerInputAction)
		{
			EnhancedInputComponent->BindAction(SteerInputAction, ETriggerEvent::Triggered,this, &ACVehiclePawn::Input_Steer);
		}
		
		if (ThrottleInputAction)
		{
			EnhancedInputComponent->BindAction(ThrottleInputAction, ETriggerEvent::Triggered, this, &ACVehiclePawn::Input_Throttle);
		}
		
		if (BrakeInputAction)
		{
			EnhancedInputComponent->BindAction(BrakeInputAction, ETriggerEvent::Triggered,this, &ACVehiclePawn::Input_Brake);
		}
	}
}

//Interface
void ACVehiclePawn::ApplySteer(float Value)
{
	if (UChaosWheeledVehicleMovementComponent* MovementComponent =
		Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		MovementComponent->SetSteeringInput(Value);
	}
}

void ACVehiclePawn::ApplyThrottle(float Value)
{
	if (UChaosWheeledVehicleMovementComponent* MovementComponent =
	Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		MovementComponent->SetThrottleInput(Value);
	}
}

void ACVehiclePawn::ApplyBrake(float Value)
{
	if (UChaosWheeledVehicleMovementComponent* MovementComponent =
	Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		MovementComponent->SetBrakeInput(Value);
	}
}

void ACVehiclePawn::OnPlayerControl()
{
	bIsPlayerDriving = true;
}

void ACVehiclePawn::OnAIControl()
{
	bIsPlayerDriving = false;
}

void ACVehiclePawn::OnControlReleased()
{
	ApplySteer(0.f);
	ApplyThrottle(0.f);
	ApplyBrake(0.f);
}

bool ACVehiclePawn::IsPlayerDriving() const
{
	return bIsPlayerDriving;
}


//Helper

void ACVehiclePawn::AddMappingContext(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (VehicleMappingContext)
				Sub->AddMappingContext(VehicleMappingContext, MappingPriority);
		}
	}
}

void ACVehiclePawn::RemoveMappingContext(APlayerController* PlayerController)
{
	if (ULocalPlayer* LP = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub =
			LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (VehicleMappingContext)
				Sub->RemoveMappingContext(VehicleMappingContext);
		}
	}
}


//Input handle
void ACVehiclePawn::Input_Steer(const FInputActionValue& value)
{
	ApplySteer(value.Get<float>());
}

void ACVehiclePawn::Input_Throttle(const FInputActionValue& value)
{
	ApplyThrottle(value.Get<float>());
}

void ACVehiclePawn::Input_Brake(const FInputActionValue& value)
{
	ApplyBrake(value.Get<float>());
}


