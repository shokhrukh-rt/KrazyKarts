// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy) 
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
	
}


// SimulateMove
void UGoKartMovementComponent::SimulateMove(const FGoKartMove& Move)
{
	FVector Force = MaxDrivingForce * Move.Throttle * GetOwner()->GetActorForwardVector();
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;
	Velocity = Velocity + Acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.TurnValue);
	UpdateLocationFromVelocity(Move.DeltaTime);

}


// Create the Move
FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Throttle = Throttle;
	Move.TurnValue = SteeringThrow;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}


// GetAirResistance
FVector UGoKartMovementComponent::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}


// Apply Rotation
void UGoKartMovementComponent::ApplyRotation(float DeltaTime, float TurnValue)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * TurnValue;
	FQuat DeltaRotation(GetOwner()->GetActorUpVector(), RotationAngle);
	GetOwner()->AddActorWorldRotation(DeltaRotation);

	Velocity = DeltaRotation.RotateVector(Velocity);
}

// Returns Rolling Resistance 
FVector UGoKartMovementComponent::GetRollingResistance()
{
	// NormalForce = Mass*g(AccelerationDueToGravity) --(N)
	// g = -GetWorld()->GetGravityZ() / 100 --(Float)
	// RollingResistance = RollingResistanceCoefficient * NormalForce -- (N)

	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * Mass * (-GetWorld()->GetGravityZ() / 100);
}


// UpdateLocationFromVelocity
void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * DeltaTime * 100;

	FHitResult OutSweepHitResult;
	GetOwner()->AddActorWorldOffset(Translation, true, &OutSweepHitResult);

	if (OutSweepHitResult.IsValidBlockingHit()) {
		Velocity = FVector::ZeroVector;
	}
}


// Setters
void UGoKartMovementComponent::SetThrottle(float Throt)
{
	this->Throttle = Throt;
}

void UGoKartMovementComponent::SetSteeringThrow(float Throw)
{
	this->SteeringThrow = Throw;
}

void UGoKartMovementComponent::SetVelocity(FVector Velo) 
{
	this->Velocity = Velo;
}

// Getters
float UGoKartMovementComponent::GetThrottle()
{
	return Throttle;
}

float UGoKartMovementComponent::GetSteeringThrow()
{
	return SteeringThrow;
}

FVector UGoKartMovementComponent::GetVelocity()
{
	return Velocity;
}