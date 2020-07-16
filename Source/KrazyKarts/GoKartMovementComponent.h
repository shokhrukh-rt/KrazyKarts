// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"



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




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();


	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Setters
	void SetThrottle(float Throttle);
	void SetSteeringThrow(float SteeringThrow);
	void SetVelocity(FVector Velocity);

	// Getters
	float GetThrottel();
	float GetSteeringThrow();
	FVector GetVelocity();

	void SimulateMove(const FGoKartMove& Move);
	
	FGoKartMove GetLastMove() { return LastMove; }

private:	
	
	FGoKartMove CreateMove(float DeltaTime);
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


	float Throttle;

	float SteeringThrow;

	FVector Velocity;
	FGoKartMove LastMove;
};
