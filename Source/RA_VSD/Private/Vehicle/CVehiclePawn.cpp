// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/CVehiclePawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Player/PlayerCharacter.h"

#include "AI/CVehicleAIController.h"

#include "ChaosWheeledVehicleMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	
	if (bCanExitVehicle) bIgnoreInteractInput = false;
}
//Possesion
void ACVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy: %s"), *GetNameSafe(NewController));

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
		OnControlReleased();
	}
	
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
			UE_LOG(LogTemp,Warning, TEXT("Steering Input bind"));
		}
		else
		{
			UE_LOG(LogTemp,Error, TEXT("Steering Input not bind"));
		}
		
		if (ThrottleInputAction)
		{
			EnhancedInputComponent->BindAction(ThrottleInputAction, ETriggerEvent::Triggered, this, &ACVehiclePawn::Input_Throttle);
		}
		
		if (BrakeInputAction)
		{
			EnhancedInputComponent->BindAction(BrakeInputAction, ETriggerEvent::Triggered,this, &ACVehiclePawn::Input_Brake);
		}
		
		if (VehicleInteractAction)
		{
			EnhancedInputComponent->BindAction(VehicleInteractAction, ETriggerEvent::Started, this, &ACVehiclePawn::Input_InteractVehicle);
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
		UE_LOG(LogTemp, Warning, TEXT("Throttle Value: %f"), Value);
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
	bIsPlayerDriving = false;
}

bool ACVehiclePawn::IsPlayerDriving() const
{
	return bIsPlayerDriving;
}

bool ACVehiclePawn::CanEnterVehicle(APlayerCharacter* PlayerCharacter) const
{
	if (bIsPlayerDriving || StoredDriver != nullptr) return false;
	UE_LOG(LogTemp, Warning, TEXT("Can enter vehicle %d"), bIsPlayerDriving);
	return true;
}

void ACVehiclePawn::EnterVehicle(AController* NewDriver)
{
	if (!NewDriver) return;
	if (bIsPlayerDriving || StoredDriver != nullptr) return;
	
	StoredDriver = Cast<APlayerCharacter>(NewDriver->GetPawn());
	if (!StoredDriver)
	{
		UE_LOG(LogTemp, Error, TEXT("No Stored Driver"));
		return;
	};

	APlayerController* PlayerController = Cast<APlayerController>(NewDriver);
	if (!PlayerController) return;
	
	StoredDriver->DisableCharacterInput(PlayerController);
	
	NewDriver->Possess(this);
	
	
	PlayerController->SetViewTargetWithBlend(this,0.5f,EViewTargetBlendFunction::VTBlend_Cubic,0.f,true);;
	
	bCanExitVehicle = true;
	bIgnoreInteractInput = true;
	
	UE_LOG(LogTemp, Warning, TEXT("EnterVehicle: Possessed by %s"), *GetNameSafe(NewDriver));
}

void ACVehiclePawn::ExitVehicle(AController* Exit)
{
	if (!Exit || !StoredDriver ||!bCanExitVehicle) return;
	APlayerController* PlayerController = Cast<APlayerController>(Exit);
	
	RemoveMappingContext(PlayerController);
	OnControlReleased();
	
	const FTransform VehicleTransform = GetActorTransform();
	const FVector WorldOffset = VehicleTransform.TransformVector(ExitOffset);
	const FTransform ExitTransform = FTransform(
		FRotator(0.0f, GetActorRotation().Yaw, 0.0f).Quaternion(),
		GetActorLocation() + WorldOffset);
	
	StoredDriver->SetActorHiddenInGame(false);
	StoredDriver->SetActorEnableCollision(true);
	StoredDriver->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	StoredDriver->SetActorTransform(ExitTransform, false, nullptr, ETeleportType::TeleportPhysics);
	
	Exit->Possess(StoredDriver);
	
	StoredDriver->EnableCharacterInput(PlayerController);
	PlayerController->SetViewTargetWithBlend(StoredDriver,0.3f,EViewTargetBlendFunction::VTBlend_Linear,0.f,true);
	UE_LOG(LogTemp, Warning, TEXT("ExitVehicle: Returned control to %s"), *GetNameSafe(StoredDriver));
	
	StoredDriver = nullptr;
}


//Helper

void ACVehiclePawn::AddMappingContext(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;
	
	UEnhancedInputLocalPlayerSubsystem* Sub =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Sub) return;
	
	if (VehicleMappingContext)
	{
		Sub->AddMappingContext(VehicleMappingContext, MappingPriority);
		UE_LOG(LogTemp, Warning, TEXT("AddMappingContext for Vehicle"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AddMappingContext: VehicleMappingContext is null"));
	}
}

void ACVehiclePawn::RemoveMappingContext(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;
	
	UEnhancedInputLocalPlayerSubsystem* Sub =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (VehicleMappingContext)
		Sub->RemoveMappingContext(VehicleMappingContext);
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

void ACVehiclePawn::Input_InteractVehicle()
{
	if (!bCanExitVehicle || bIgnoreInteractInput) {UE_LOG(LogTemp, Warning, TEXT("Exit vehicle return")); return;}
	UE_LOG(LogTemp, Warning, TEXT("Exit vehicle"));
	ExitVehicle(GetController());
}


