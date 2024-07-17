// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "InstructionLineMarkActorST.generated.h"

UCLASS(Abstract)
class SYMBOLICTRACER_API AInstructionLineMarkActorST : public ASceneActorBase
{
	GENERATED_BODY()
	// ---VARIABLES---

	public:

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// shall show the mainLine (see through)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Marker Mesh")
	TObjectPtr<UStaticMeshComponent> MarkerMesh = nullptr;

protected:

private:

	// ---METHODS---


	public:

	// Sets default values for this actor's properties
	AInstructionLineMarkActorST();

	void Init(const FVector& Scale);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
};
