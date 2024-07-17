// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "InstructionTextActorST.generated.h"

class UTextRenderComponent;
/*
 * this actor holds the opcode of each instruction
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API AInstructionTextActorST : public ASceneActorBase
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

	// shall show the @opCode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instruction Text")
	TObjectPtr<UTextRenderComponent> OpCodeTextRender = nullptr;

protected:

private:

	// ---METHODS---

public:

	// Sets default values for this actor's properties
	AInstructionTextActorST();

	// to initialize this instructionText
	void Init(const FString& OpCodeText);

	// hide or show actor
	void Hide(bool hide);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	

};
