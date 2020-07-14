// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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
	void Server_MoveForward(float Value);

	// MoveRight or Left depending on the Value
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

private:

	// Functions

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
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

	float DeltaDegrees;
	float Throttle;
	float TurnValue;
	FVector Velocity;
	

	

};
