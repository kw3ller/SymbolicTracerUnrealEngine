// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "CodeBlockActorST.generated.h"

// forward declarations

class UClickableComponentST;


/*
 * this is an actor that we spawn in the scene that will symbolise the instructions of the symbolic run
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API ACodeBlockActorST : public ASceneActorBase
{
	GENERATED_BODY()

	// ---VARIABLES---


public:
	// index of the run this is part of (in run_blocks)
	UPROPERTY(BlueprintReadOnly)
	int RunIndex = -1;

	// index of the block inside the run (in run_blocks) -> not index from uniqueBlock in cfgBlocks!
	UPROPERTY(BlueprintReadOnly)
	int BlockIndex = -1;

	// position of this actor
	UPROPERTY(BlueprintReadOnly)
	FVector Position;

	
	// Material that gets changed, to show how often this codeBlock got executed
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MarkMaterialInterface = nullptr;

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// the normal mesh component to which that mesh and material will be added
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

	// the clickable component (instance will be set in Blueprint)
	// TODO: add crash when not set?!
	// -> gets set in blueprint construction script!!!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UClickableComponentST> ClickableComponent = nullptr;

protected:

private:
	// ---METHODS---

public:
	// Sets default values for this actor's properties
	ACodeBlockActorST();

	// to initialize this codeBlock
	void Init(FVector Pos, FVector Scale,int Count, int MyRunIndex, int MyBlockIndex);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
