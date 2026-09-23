// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/Wheels/CChaosVehicleWheelFront.h"

UCChaosVehicleWheelFront::UCChaosVehicleWheelFront()
{
	WheelRadius = 30.0f;
	WheelWidth = 25.0f;

	bAffectedByEngine = true;
	bAffectedByBrake = true;
	bAffectedBySteering = true;

	MaxSteerAngle = 35.0f; //0 for rear
	FrictionForceMultiplier = 3.0f;
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionDampingRatio = 0.9f;
}
