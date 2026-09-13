// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/Wheels/CChaosVehicleWheel.h"

UCChaosVehicleWheel::UCChaosVehicleWheel()
{
	WheelRadius = 30.0f;
	WheelWidth = 20.0f;
	
	bAffectedByEngine = true; 
	
	MaxSteerAngle = 0.0f; //0 for rear
	FrictionForceMultiplier = 3.0f;
	SuspensionMaxRaise = 10.0f;
	SuspensionMaxDrop  = 15.0f;
	SuspensionDampingRatio     = 0.5f;
}
