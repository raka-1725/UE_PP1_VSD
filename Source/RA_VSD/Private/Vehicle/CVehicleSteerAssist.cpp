// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/CVehicleSteerAssist.h"

// Sets default values for this component's properties
UCVehicleSteerAssist::UCVehicleSteerAssist()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCVehicleSteerAssist::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCVehicleSteerAssist::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

