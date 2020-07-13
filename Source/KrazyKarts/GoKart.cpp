// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = MaxDrivingForce * Throttle * GetActorForwardVector();
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;

	ApplyRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);

}


// GetAirResistance
FVector AGoKart::GetAirResistance() {

	return -Velocity.GetSafeNormal()*Velocity.SizeSquared()*DragCoefficient;
}

// Apply Rotation
void AGoKart::ApplyRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * TurnValue; 
	FQuat DeltaRotation(GetActorUpVector(), RotationAngle);
	AddActorWorldRotation(DeltaRotation);

	Velocity = DeltaRotation.RotateVector(Velocity);
}

// Returns Rolling Resistance 
FVector AGoKart::GetRollingResistance(){

	// NormalForce = Mass*g(AccelerationDueToGravity) --(N)
	// g = -GetWorld()->GetGravityZ() / 100 --(Float)
	// RollingResistance = RollingResistanceCoefficient * NormalForce -- (N)

	return -Velocity.GetSafeNormal()*RollingResistanceCoefficient* Mass* (-GetWorld()->GetGravityZ() / 100);

}


// UpdateLocationFromVelocity
void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * DeltaTime * 100;

	FHitResult OutSweepHitResult;
	AddActorWorldOffset(Translation, true, &OutSweepHitResult);

	if (OutSweepHitResult.IsValidBlockingHit()) {
		Velocity = FVector::ZeroVector;
	}
}


// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);

}


// MoveForward
void AGoKart::MoveForward(float Value) {

	Throttle = Value;

}

void AGoKart::MoveRight(float Value) {

	TurnValue = Value;

}

