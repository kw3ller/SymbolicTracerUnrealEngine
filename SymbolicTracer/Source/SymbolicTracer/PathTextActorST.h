// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "PathTextActorST.generated.h"

class UTextRenderComponent;
class UClickableComponentST;
/*
 * this actor will be at top of each path
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API APathTextActorST : public ASceneActorBase
{
	GENERATED_BODY()


	// ---VARIABLES---

public:

	// index of the run this is part of (in runs)
	UPROPERTY(BlueprintReadOnly)
	int RunIndex = -1;

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// shall show the @ProgramCounterHex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instruction Text")
	TObjectPtr<UTextRenderComponent> TextRender = nullptr;

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
	APathTextActorST();

	void Init(const int PathText);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	
};
