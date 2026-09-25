// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

#include "CPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Vehicle/VehicleInputInterface.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Vehicle/CVehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Widgets/PlayerWidget.h"


APlayerCharacter::APlayerCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	
	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	

}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Sub =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (CharacterMappingContext)
					Sub->AddMappingContext(CharacterMappingContext, 0);
			}
		}
	}
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentVehicle == nullptr) CheckNearbyVehicle();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	if (MoveAction)
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_Move);

	if (LookAction)
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Input_Look);

	if (VehicleInteractAction)
		EIC->BindAction(VehicleInteractAction, ETriggerEvent::Started, this, &APlayerCharacter::TryEnterVehicle);
}



void APlayerCharacter::Input_Move(const struct FInputActionValue& value)
{
	if (!Controller) return;
	
	FVector2D Axis = value.Get<FVector2D>();
	const FRotator Yaw(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Axis.X);
}

void APlayerCharacter::Input_Look(const struct FInputActionValue& value)
{
	FVector2D Axis = value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void APlayerCharacter::CheckNearbyVehicle()
{
	if (CurrentVehicle) return;
	
	NearbyVehiclePawn = nullptr;
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	World->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params);
	
	for (const FOverlapResult& Hit : Overlaps)
	{
		ACVehiclePawn* Vehicle = Cast<ACVehiclePawn>(Hit.GetActor());
		if (!Vehicle) continue;
		
		if (!Vehicle->CanEnterVehicle(this)) continue;
		
		UE_LOG(LogTemp, Warning, TEXT("Vehicle Found"));
		NearbyVehiclePawn = Vehicle;
		break;
	}	
	
	if (NearbyVehiclePawn)
	{
		UPlayerWidget* PW = Cast<ACPlayerController>(GetController())->PlayerWidget;
		PW->ShowCanInteract();
	}
	else
	{
		UPlayerWidget* PW = Cast<ACPlayerController>(GetController())->PlayerWidget;
		PW->HideCanInteract();
	}
	
	//DrawDebugSphere(GetWorld(), GetActorLocation(), InteractRadius, 16, FColor::Emerald);
	
}
void APlayerCharacter::TryEnterVehicle()
{
	if (!NearbyVehiclePawn) return;

	AController* MyController = GetController();
	if (!MyController) return;
	
	CurrentVehicle = NearbyVehiclePawn;
	NearbyVehiclePawn = nullptr;

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetCharacterMovement()->DisableMovement();
    
	UPlayerWidget* PW = Cast<ACPlayerController>(GetController())->PlayerWidget;
	PW->HideCanInteract();
	
	CurrentVehicle->EnterVehicle(MyController);
	
}

void APlayerCharacter::OnExitVehicle(FTransform ExitVehicleTransform)
{
	UE_LOG(LogTemp, Warning, TEXT("Exiting Vehicle"));
	if (ACPlayerController* PC = Cast<ACPlayerController>(GetController()))
	{
		PC->Possess(this);
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTransform(ExitVehicleTransform, false, nullptr, ETeleportType::TeleportPhysics);
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	NearbyVehiclePawn = nullptr;
	CurrentVehicle = nullptr;
	
	
	CheckNearbyVehicle();
}

void APlayerCharacter::EnableCharacterInput(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;
	
	UEnhancedInputLocalPlayerSubsystem* Sub =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	if (!Sub) return;
	if (CharacterMappingContext)
	{
		Sub->AddMappingContext(CharacterMappingContext, 0);
	}
}

void APlayerCharacter::DisableCharacterInput(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;
	
	UEnhancedInputLocalPlayerSubsystem* Sub =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	if (!Sub) return;
	if (CharacterMappingContext)
	{
		Sub->RemoveMappingContext(CharacterMappingContext);
	}
}



