// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Vehicle/VehicleInputInterface.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Vehicle/CVehiclePawn.h"


APlayerCharacter::APlayerCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

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

void APlayerCharacter::TryEnterVehicle()
{
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
		
		AController* MyController = GetController();
		if (!MyController) return;
		
		CurrentVehicle = Vehicle;
		
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		GetCharacterMovement()->DisableMovement();
		
		Vehicle->EnterVehicle(MyController);
		return;
	}
}

void APlayerCharacter::OnExitVehicle(FTransform ExitVehicleTransform)
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	SetActorTransform(ExitVehicleTransform, false, nullptr, ETeleportType::TeleportPhysics);

	CurrentVehicle = nullptr;
}



