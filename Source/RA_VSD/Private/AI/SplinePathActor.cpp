// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SplinePathActor.h"
#include "Components/SplineComponent.h"

// Sets default values
ASplinePathActor::ASplinePathActor()
{
	AISplineComp = CreateDefaultSubobject<USplineComponent>("Spline");
	RootComponent = AISplineComp;
	
	AISplineComp->SetClosedLoop(true);
}


