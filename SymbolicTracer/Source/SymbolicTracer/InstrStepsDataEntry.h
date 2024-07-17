// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InstrStepsDataEntry.generated.h"

class ARTSPlayerPawnST;
/**
 * just for passing data to the 
 */
UCLASS(Blueprintable)
class SYMBOLICTRACER_API UInstrStepsDataEntry : public UObject
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// a reference to the playerCharacter (for clicking on a step)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;

	// is this instructionStep marked
	UPROPERTY(BlueprintReadOnly)
	bool Marked = false;

	// Id of this step
	UPROPERTY(BlueprintReadOnly)
	int StepId = -1;
	
	// the behaviour to show
	UPROPERTY(BlueprintReadOnly)
	FString Behaviour;

	// depth of this step
	UPROPERTY(BlueprintReadOnly)
	int Depth = -1;

	// depth of this step
	UPROPERTY(BlueprintReadOnly)
	bool Condition = true;

	// target of this step
	UPROPERTY(BlueprintReadOnly)
	int Target = -1;
	
	// Ind of run this step is part of
	UPROPERTY(BlueprintReadOnly)
	int RunInd = -1;

	// Ind of instruction this step is part of
	UPROPERTY(BlueprintReadOnly)
	int InstructionInd = -1;

	// Ind of this step
	UPROPERTY(BlueprintReadOnly)
	int StepInd = -1;

	// Ind of this step for clicking on it -> might be different from StepInd if this step is a duplicate (has same depth)
	UPROPERTY(BlueprintReadOnly)
	int StepClickInd = -1;

	
	

	

protected:

private:

	// ---METHODS---

public:

	void Init(ARTSPlayerPawnST* playerCharacter, int runInd, int instructionInd, int stepInd, int clickInd);

protected:

private:
	
};
