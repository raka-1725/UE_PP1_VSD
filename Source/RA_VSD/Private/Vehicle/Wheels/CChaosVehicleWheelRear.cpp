// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/Wheels/CChaosVehicleWheelRear.h"

UCChaosVehicleWheelRear::UCChaosVehicleWheelRear()
{
	WheelRadius = 30.0f;
	WheelWidth = 20.0f;

	bAffectedByEngine = true;
	bAffectedByBrake = true;
	bAffectedByHandbrake = true;
	
	MaxSteerAngle = 0.0f; //0 for rear
	FrictionForceMultiplier = 3.5;
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionDampingRatio = 1.0f;
}
