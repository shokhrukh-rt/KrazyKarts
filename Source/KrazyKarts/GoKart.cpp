// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	NetUpdateFrequency = 1;
	
}

// Get Actor Roles
FString GetActorRole(ENetRole Role) 
{	
	switch (Role)
	{
	case ROLE_None:
		return "Role_None";
	case ROLE_SimulatedProxy:
		return "Simulated Proxy";
	case ROLE_AutonomousProxy:
		return "Autonomous Proxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}


// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled()) 
	{
		FGoKartMove Move;
		Move.DeltaTime = DeltaTime;
		Move.Throttle = Throttle;
		Move.TurnValue = TurnValue;
		// TODO: Set time
		Server_SendMove(Move);
		SimulateMove(Move);
	}	

	DrawDebugString
	(
		GetWorld(),
		FVector(0,0,160),
		GetActorRole(Role),
		this,
		FColor::Red,
		DeltaTime
	);
}

// OnRep_ReplicatedLocation
void AGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;
}


// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);

}


// GetLifetimeReplicatedProps for Location and Rotation
void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);

}


// SimulateMove
void AGoKart::SimulateMove(FGoKartMove Move) 
{
	FVector Force = MaxDrivingForce * Move.Throttle * GetActorForwardVector();
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;
	Velocity = Velocity + Acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.TurnValue);
	UpdateLocationFromVelocity(Move.DeltaTime);

}


// GetAirResistance
FVector AGoKart::GetAirResistance() 
{
	return -Velocity.GetSafeNormal()*Velocity.SizeSquared()*DragCoefficient;
}

// Apply Rotation
void AGoKart::ApplyRotation(float DeltaTime, float TurnValue)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * TurnValue; 
	FQuat DeltaRotation(GetActorUpVector(), RotationAngle);
	AddActorWorldRotation(DeltaRotation);

	Velocity = DeltaRotation.RotateVector(Velocity);
}

// Returns Rolling Resistance 
FVector AGoKart::GetRollingResistance()
{
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


// MoveForward
void AGoKart::MoveForward(float Value)
{
	Throttle = Value;	
}


// MoveRight
void AGoKart::MoveRight(float Value)
{
	TurnValue = Value;	
}


// MoveForward_Implementation
void AGoKart::Server_SendMove_Implementation(FGoKartMove Move) 
{	
	SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;

}

// Validate MoveForward
bool AGoKart::Server_SendMove_Validate(FGoKartMove Move) {

	return true; //TODO: Make Validation
}




