// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "CodeBlkTextActorST.generated.h"

class UTextRenderComponent;
/*
 * this actor shows the lines and file of a codeBlock in the scene
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API ACodeBlkTextActorST : public ASceneActorBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// shall show the @ProgramCounterHex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Text")
	TObjectPtr<UTextRenderComponent> FileTextRender = nullptr;

	// shall show the @ProgramCounterHex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block Text")
	TObjectPtr<UTextRenderComponent> LineTextRender = nullptr;

protected:

private:

	// ---METHODS---

public:
	
	// Sets default values for this actor's properties
	ACodeBlkTextActorST();

	void Init(const FString& FileName, const int BlockStart, const int BlockEnd);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	


	

};
