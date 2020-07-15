// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


USTRUCT()
struct FGoKartMove {

	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float TurnValue;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};


USTRUCT()
struct FGoKartState {

	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FGoKartMove LastMove;
};


UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	//MoveForward - called when W key is pressed
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	// ServerState ToBeReplicated
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

private:

	// Functions

	void SimulateMove(FGoKartMove Move);

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime, float TurnValue);
	FVector GetRollingResistance();
	FVector GetAirResistance();

	// UPROPERTY Variables
	
	// Mass of the car in kgs
	UPROPERTY(EditAnywhere)
	float Mass = 1000;
	
	// The force applied when throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// Drag Coefficient (kg/m)
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// Rolling Resistance Coefficient (N)
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	// Minimum Turning Radius of car turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;


	// Variables

	float Throttle;
	
	float TurnValue;	

	FVector Velocity;

};
