// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunAnimationActorST.generated.h"



/*
 * this actor will be used to animate the runs
 * -> this actor will go through the animation steps
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API ARunAnimationActorST : public AActor
{
	GENERATED_BODY()

	// ---VARIABLES---

public:


	

	// movement speed of the animation
	UPROPERTY()
	float Speed = 1.0f;

	// indicates if the movement is finished
	UPROPERTY()
	bool Finished = true;

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// the normal mesh component to which that mesh and material will be added
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

protected:

	UPROPERTY()
	FVector Direction;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	float TotalDistance;

	UPROPERTY()
	float CurrentDistance;

private:

	// ---METHODS---

public:
	// Sets default values for this actor's properties
	ARunAnimationActorST();

	// initialize (e.g. scale)
	void Init(const int32 MaxDepth);

	void TeleportToTarget(const FVector& Target);

	// moves this actor to a target with the given speed 
	void MoveToTarget(const FVector& Target, const float NewSpeed);

	// Called every frame -> used for moving this actor
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

};
