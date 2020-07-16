// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
//#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	MovementComponent = CreateDefaultSubobject <UGoKartMovementComponent>(TEXT("MovementComponent"));

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

	if (!ensure(MovementComponent != nullptr)) { return; }

	if (Role == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = MovementComponent->SetMove(DeltaTime);
		UnacknowledgedMoves.Add(Move);
		MovementComponent->SimulateMove(Move);
	}

	// THis is the server
	if (Role == ROLE_Authority && GetRemoteRole() == ROLE_AutonomousProxy) 
	{
		FGoKartMove Move = MovementComponent->SetMove(DeltaTime);
		Server_SendMove(Move);
	}

	if (Role == ROLE_AutonomousProxy) 
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
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


// ClearAcknowledgedMoves
void AGoKart::ClearAcknowledgedMoves(FGoKartMove LastMove) 
{
	TArray< FGoKartMove> NewUnacknoledgedMoves;

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{			
				NewUnacknoledgedMoves.Add(Move);			
		}
	}
	UnacknowledgedMoves = NewUnacknoledgedMoves;
}

// OnRep_ReplicatedLocation
void AGoKart::OnRep_ServerState()
{
	if (!ensure(MovementComponent != nullptr)) { return; }

	SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);
	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
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


// MoveForward
void AGoKart::MoveForward(float Value)
{
	if (!ensure(MovementComponent != nullptr)) { return; }
	MovementComponent->SetThrottle(Value);	
}


// MoveRight
void AGoKart::MoveRight(float Value)
{
	if (!ensure(MovementComponent != nullptr)) { return; }
	MovementComponent->SetSteeringThrow(Value);	
}


// MoveForward_Implementation
void AGoKart::Server_SendMove_Implementation(FGoKartMove Move) 
{	
	if (!ensure(MovementComponent != nullptr)) { return; }

	MovementComponent->SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();

}

// Validate MoveForward
bool AGoKart::Server_SendMove_Validate(FGoKartMove Move) {

	return true; //TODO: Make Validation
}




