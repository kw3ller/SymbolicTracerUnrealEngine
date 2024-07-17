// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "CodeBlkCountActorST.generated.h"


class UTextRenderComponent;
/*
 * this actor gets spawned at top of codeBlock and shows count how often this codeBlock got executed
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API ACodeBlkCountActorST : public ASceneActorBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// shall show the @ProgramCounterHex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Text")
	TObjectPtr<UTextRenderComponent> CountTextRender = nullptr;

protected:

private:

	// ---METHODS---

public:

	// Sets default values for this actor's properties
	ACodeBlkCountActorST();

	void Init(const int Count);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
};
