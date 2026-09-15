// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SplinePathActor.generated.h"

UCLASS()
class ASplinePathActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplinePathActor();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SplinePath")
	USplineComponent* AISplineComp;
};
