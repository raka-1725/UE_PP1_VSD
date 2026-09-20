// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CVehicleAIController.h"

#include "AI/SplinePathActor.h"
#include "Vehicle/CVehiclePawn.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"

ACVehicleAIController::ACVehicleAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACVehicleAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledPawn = InPawn;
	ControlledVehicle = Cast<ACVehiclePawn>(InPawn);
	VehicleInput = Cast<IVehicleInputInterface>(InPawn);
	
	if (VehicleInput)
	{
		VehicleInput->OnAIControl();
	}
	
	if (USplineComponent* SplineComp = GetSpline())
	{
		CurrentSplineDistance = SplineComp->
		GetDistanceAlongSplineAtLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
	}
	
	bIsActive = true;
	UE_LOG(LogTemp, Warning, TEXT("ACVehicleAIController::OnPossess()"));
}

void ACVehicleAIController::OnUnPossess()
{
	bIsActive = false;
	ZeroInputs();
	
	if (VehicleInput)
	{
		ZeroInputs();
		VehicleInput->OnControlReleased();
	}

	VehicleInput = nullptr;
	ControlledPawn = nullptr;
	ControlledVehicle = nullptr;

	Super::OnUnPossess();
}

void ACVehicleAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsActive || !ControlledPawn) return;
	
	FollowSpline(DeltaTime);
}

void ACVehicleAIController::TakeOverFromPlayer()
{
	bIsActive = true;
	UE_LOG(LogTemp, Warning, TEXT("taking over from player"));

	if (USplineComponent* SplineComp = GetSpline())
	{
		CurrentSplineDistance = SplineComp->
		GetDistanceAlongSplineAtLocation(ControlledPawn->GetActorLocation()
			, ESplineCoordinateSpace::World);
	}
}


void ACVehicleAIController::FollowSpline(float DeltaTime)
{
	USplineComponent* SplineComp = GetSpline();
	if (!SplineComp) return;
	
	const float SplineLength = SplineComp->GetSplineLength();
	
	const FVector VehicleVelocity = ControlledPawn->GetVelocity();
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const float SpdCmS = VehicleVelocity.Size();
	const float SpdKmh = SpdCmS * 0.036f;
	
	
	//Projecting Tangent
	const FVector SplineTangent = SplineComp->GetTangentAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World).GetSafeNormal();
	
	const float TangentSpeed = FVector::DotProduct(VehicleVelocity, SplineTangent);
	
	const float MinAdvanceSpeed = 50.f;
	const float AdvanceSpeed = FMath::Max(TangentSpeed, MinAdvanceSpeed);
	
	CurrentSplineDistance = FMath::Fmod(CurrentSplineDistance + AdvanceSpeed * DeltaTime, SplineLength);
	
	//Getting Closest point - Prevent from going off too far
	const FVector CurrentSplinePoint = SplineComp->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
	const float DistFromSpline = FVector::Dist(PawnLocation, CurrentSplinePoint);
	
	if (DistFromSpline > ResyncThreshold)
	{
		float BestDist   = CurrentSplineDistance;
		float BestSqDist = FVector::DistSquared(PawnLocation, CurrentSplinePoint);

		for (float Offset = 0.f; Offset < ResyncSearchWindow; Offset += 100.f)
		{
			const float TestDist = FMath::Fmod(
				CurrentSplineDistance + Offset, SplineLength);
			const FVector TestPt = SplineComp->GetLocationAtDistanceAlongSpline(
				TestDist, ESplineCoordinateSpace::World);
			const float SqD = FVector::DistSquared(PawnLocation, TestPt);

			if (SqD < BestSqDist)
			{
				BestSqDist = SqD;
				BestDist   = TestDist;
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("Resyncing spline | Drift: %.0f"), DistFromSpline);
		CurrentSplineDistance = BestDist;
	}
	
	//LookAhead
	const float Lookahead = FMath::Clamp(LookaheadDist + SpdCmS * LookaheadSpeedScale, LookaheadMin, LookaheadMax);
	const float LookaheadPos = FMath::Fmod(CurrentSplineDistance + Lookahead, SplineLength);
	FVector ValidTarget = SplineComp->GetLocationAtDistanceAlongSpline(LookaheadPos, ESplineCoordinateSpace::World);
	
	//Check target is in front of pawn
	const FVector ToTarget = (ValidTarget - PawnLocation).GetSafeNormal();
	const float ForwardDot = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), ToTarget);
	if (ForwardDot < 0.0f)
	{
		bool bFoundForward = false;
		for (float ExtraLookahead = Lookahead + 300.0f ; ExtraLookahead < SplineLength * 0.5f; ExtraLookahead += 200.0f)
		{
			const float TestPos = FMath::Fmod(CurrentSplineDistance + ExtraLookahead, SplineLength);
			const FVector TestTarget = SplineComp->GetLocationAtDistanceAlongSpline(TestPos, ESplineCoordinateSpace::World);
			const FVector TestDir = (TestTarget - PawnLocation).GetSafeNormal();
			
			if (FVector::DotProduct(ControlledPawn->GetActorForwardVector(), TestDir) > 0.1f)
			{
				ValidTarget = TestTarget;
				bFoundForward = true;
				break;
			}
		}
		if (!bFoundForward)
		{
			ValidTarget = PawnLocation + SplineTangent * Lookahead;
		}
	}
	
	//curvature
	const float Curvature = GetSplineCurvature(SplineComp, CurrentSplineDistance);
	const float AheadCurvature = GetSplineCurvature(SplineComp, FMath::Fmod(CurrentSplineDistance + Lookahead * 0.5f, SplineLength));
	
	//Obstacle check
	const float ObstacleBias = CheckObstacles();
	const bool bObstacleAhead = FMath::Abs(ObstacleBias) > 0.1f;
	
	//Steer
	float RawSteerVal = CalcSteer(ValidTarget);
	RawSteerVal = FMath::Clamp(RawSteerVal + ObstacleBias, -1.0f, 1.0f);
	
	smoothsteerValue = FMath::FInterpTo(smoothsteerValue, RawSteerVal, DeltaTime, 3.0f);
	
	const float SteerSpeed = bObstacleAhead ? 8.f : 4.f;
	steerValue = FMath::FInterpTo(steerValue, smoothsteerValue, DeltaTime, SteerSpeed);
	
	
	//Throttle
	float Throttle = CalcThrottle(SpdKmh, RawSteerVal, Curvature);
	float Brake = 0.0f;
	if (AheadCurvature > CornerBrakeThreshold)
	{
		Brake = CornerBrakeStrength * (AheadCurvature - CornerBrakeThreshold);
		Throttle = 0.0f;
	}
	if (bObstacleAhead)
	{
		const float DistToObst = ObstacleTraceDist *(1.0f - FMath::Abs(ObstacleBias));
		if (DistToObst < BrakeOnObstacleDist){ Throttle = 0.0f; Brake = 1.0f; }
	}
	//Speed clamp
	if (SpdKmh > MaxSPD)
	{
		Throttle = 0.0f;
	}
	
	//Vehicle Input
	VehicleInput->ApplySteer(RawSteerVal);
	VehicleInput->ApplyBrake(Brake);
	VehicleInput->ApplyThrottle(Throttle);
	
	//Debug draw
	const FVector ClosestPoint = SplineComp->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
	
	DrawDebugSphere(GetWorld(), CurrentSplinePoint, 20.f, 8, FColor::Blue,  false, -1.f);
	DrawDebugSphere(GetWorld(), ValidTarget,        40.f, 8, FColor::Green, false, -1.f);
	DrawDebugLine(GetWorld(), PawnLocation, ValidTarget, FColor::Magenta, false, -1.f);
	DrawDebugLine(GetWorld(), PawnLocation,
		PawnLocation + SplineTangent * 300.f, FColor::Cyan, false, -1.f);

	DrawDebugString(GetWorld(),
		PawnLocation + FVector(0, 0, 200),
		FString::Printf(TEXT("%.0fkm/h Curv:%.2f Str:%.2f Thr:%.2f Dot:%.2f Drift:%.0f"),
			SpdKmh, AheadCurvature, steerValue, Throttle, ForwardDot, DistFromSpline),
		nullptr, FColor::White, -1.f);
}

float ACVehicleAIController::GetSplineCurvature(USplineComponent* SplineComp, float Distance) const
{
	if (!SplineComp) return 0.0f;
	const float SplineLength = SplineComp->GetSplineLength();
	const float StepSize = 100.0f;
	
	const float D0 = FMath::Fmod(Distance, SplineLength);
	const float D1 = FMath::Fmod(Distance + StepSize,SplineLength);
	const float D2 = FMath::Fmod(Distance + StepSize * 2, SplineLength);

	const FVector T0 = SplineComp->GetTangentAtDistanceAlongSpline(D0, ESplineCoordinateSpace::World).GetSafeNormal();
	const FVector T1 = SplineComp->GetTangentAtDistanceAlongSpline(D1, ESplineCoordinateSpace::World).GetSafeNormal();
	const FVector T2 = SplineComp->GetTangentAtDistanceAlongSpline(D2, ESplineCoordinateSpace::World).GetSafeNormal();
	
	const float AngleChange0 = FMath::Acos(FMath::Clamp(FVector::DotProduct(T0, T1), -1.f, 1.f));
	const float AngleChange1 = FMath::Acos(FMath::Clamp(FVector::DotProduct(T1, T2), -1.f, 1.f));

	const float AvgAngle = (AngleChange0 + AngleChange1) * 0.5f;
	return FMath::Clamp(AvgAngle / FMath::DegreesToRadians(45.f), 0.f, 1.f);
	
}
float ACVehicleAIController::CalcSteer(const FVector& TargetLocation) const
{
	const FVector PawnLoc  = ControlledPawn->GetActorLocation();
	const FVector ToTarget = TargetLocation - PawnLoc;
	
	const float Lookahead = ToTarget.Size();
	
	const float LateralError  = FVector::DotProduct(ControlledPawn->GetActorRightVector(),ToTarget / Lookahead);
	
	const float NormalizedLookahead = Lookahead / 1000.f;
	const float Curvature = (2.0f * LateralError) / FMath::Max(NormalizedLookahead, 0.1f);
	
	return FMath::Clamp(Curvature * 0.7f, -1.0f, 1.0f);
}

float ACVehicleAIController::CalcThrottle(float CurrentSpeedKmh, float SteeringValue, float SplineCurvature)
{
	const float TurnFactor = 1.0f - FMath::Abs(SteeringValue) * 0.5f;
	const float SPDFactor = CurrentSpeedKmh > ThrottleReduceSPD ? MaxThrottleHighSpeed : MaxThrottle;
	const float CurvatureFactor = 1.0f - SplineCurvature * 0.6f;
	return FMath::Clamp(TurnFactor * SPDFactor, 0.0f, MaxThrottle);
}


float ACVehicleAIController::CheckObstacles() const
{
	if (!ControlledPawn) return 0.0f;
	UWorld* World = GetWorld();
	if (!World) return 0.0f;
	
	const FVector Start = ControlledPawn->GetActorLocation();
	const FVector Forward = ControlledPawn->GetActorForwardVector();
	const FVector Right = ControlledPawn->GetActorRightVector();
	
	FCollisionQueryParams  Params;
	Params.AddIgnoredActor(ControlledPawn);
	
	FCollisionShape TraceSphere = FCollisionShape::MakeSphere(ObstacleTraceHalfWidth);
	
	//Centre
	FHitResult MiddleHit;
	//sweep
	const bool bMiddleHit = World->SweepSingleByChannel(
		MiddleHit, Start,Start + Forward * ObstacleTraceDist, FQuat::Identity,
		ECC_Visibility, TraceSphere,
		Params);
	
	if (!bMiddleHit) return 0.0f;
	const float HitStrength = 1.f - (MiddleHit.Distance / ObstacleTraceDist);
	
	//left, steer to right
	FHitResult LeftHit;
	const bool bLeftHit = World->SweepSingleByChannel(
		LeftHit, Start, Start + (Forward - Right * 0.5f).GetSafeNormal() * ObstacleTraceDist, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(ObstacleTraceHalfWidth),
		Params);
	
	//Right, steer to left
	FHitResult RightHit;
	const bool bRightHit = World->SweepSingleByChannel(
		RightHit, Start, Start + (Forward + Right * 0.5f).GetSafeNormal() * ObstacleTraceDist, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(ObstacleTraceHalfWidth),
		Params);
	//avoid to right
	if (bLeftHit && !bRightHit) return AvoidanceSteerStrength * HitStrength;
	
	//avoid to left
	if (!bLeftHit && bRightHit) return -AvoidanceSteerStrength * HitStrength;
	
	if (!bLeftHit && bRightHit)
	{
		const float LeftDist = LeftHit.bBlockingHit ? LeftHit.Distance : ObstacleTraceDist;
		const float RightDist = RightHit.bBlockingHit ? RightHit.Distance : ObstacleTraceDist;
		
		return RightDist > LeftDist ? AvoidanceSteerStrength * HitStrength : -AvoidanceSteerStrength * HitStrength;
	}
	return 0;
}


USplineComponent* ACVehicleAIController::GetSpline() const
{
	if (!AISplinePath) return nullptr;
	return AISplinePath->FindComponentByClass<USplineComponent>();
}

void ACVehicleAIController::ZeroInputs() const
{
	if (!VehicleInput) return;

	VehicleInput->ApplySteer(0.f);
	VehicleInput->ApplyThrottle(0.f);
	VehicleInput->ApplyBrake(0.f);
}

