// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	MovementComponent = CreateDefaultSubobject <UGoKartMovementComponent>(TEXT("MovementComponent"));
	MovementReplicator = CreateDefaultSubobject <UGoKartMovementReplicator>(TEXT("MovementReplicator"));

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


void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);

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




