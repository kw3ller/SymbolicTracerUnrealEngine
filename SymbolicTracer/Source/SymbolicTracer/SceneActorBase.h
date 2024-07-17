// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneActorBase.generated.h"

/*
 * from this class all our actors in the scene (representing the symbolic run) should inherit
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API ASceneActorBase : public AActor
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

protected:

private:
	// ---METHODS---	

public:
	// Sets default values for this actor's properties
	ASceneActorBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
};
