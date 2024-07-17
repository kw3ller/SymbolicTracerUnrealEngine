// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "EAbstractionLevel.h"
#include "InstructionLineActorST.generated.h"

class UTextRenderComponent;
class ATextRenderActor;
/*
 * this is an actor that we spawn in the scene that symbolises on line between min and max pc in the program
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API AInstructionLineActorST : public ASceneActorBase
{
	GENERATED_BODY()


	// ---VARIABLES---

public:
	
	// hide in game -> for different abstraction levels
	UPROPERTY(BlueprintReadOnly)
	bool HideInGame = true;

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// shall show the @ProgramCounterHex
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instruction Text")
	TObjectPtr<UTextRenderComponent> TextRender = nullptr;

	// shall show the mainLine (see through)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instruction Text")
	TObjectPtr<UStaticMeshComponent> MiddleLine = nullptr;

	// shall show the upperLine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instruction Text")
	TObjectPtr<UStaticMeshComponent> UpperLine = nullptr;

	// shall show the lowerLine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instruction Text")
	TObjectPtr<UStaticMeshComponent> LowerLine = nullptr;

protected:

private:

	// ---METHODS---

public:
	// Sets default values for this actor's properties
	AInstructionLineActorST();

	void Init(const FString& ProgramCounterHex, const FVector& ScaleMiddle, const FVector& ScaleUpperLower);

	// hides or shows this actor
	void Hide(bool Hide);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:


	

};
