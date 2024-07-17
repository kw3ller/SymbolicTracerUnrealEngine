// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InstrStepBase.generated.h"

class UBorder;
class UTextBlock;
/**
 * Widget base for displaying individual instructionSteps (e.g. inside instructionWidget)
 * for all instructions except for jump and branch
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UInstrStepBase : public UWidgetSTBase, public IUserObjectListEntry
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// shows the step id
	UPROPERTY(BlueprintReadOnly, Category="instruction step", meta=(BindWidget))
	TObjectPtr<UTextBlock> StepId = nullptr;

	// shows the step depth
	UPROPERTY(BlueprintReadOnly, Category="instruction step", meta=(BindWidget))
	TObjectPtr<UTextBlock> Depth = nullptr;

	// shows the behaviour of the step
	UPROPERTY(BlueprintReadOnly, Category="instruction step", meta=(BindWidget))
	TObjectPtr<UTextBlock> Behaviour = nullptr;

	// marks this instruction step
	UPROPERTY(BlueprintReadOnly, Category="instruction step", meta=(BindWidget))
	TObjectPtr<UBorder> MarkBorder = nullptr;

	// the run index of this step
	UPROPERTY(BlueprintReadOnly)
	int RunIndex = -1;
	
	// the instruction index of this step
	UPROPERTY(BlueprintReadOnly)
	int InstructionIndex = -1;
	
	// the step index of this step
	UPROPERTY(BlueprintReadOnly)
	int StepIndex = -1;

	// the click index of this step -> TODO: delete: should not be necessary here -> will only be used for jump or branch instructions
	UPROPERTY(BlueprintReadOnly)
	int StepClickIndex = -1;


protected:

	

private:


	// ---METHODS---

public:

protected:

	// gets called when initializing (e.g. set content to show)
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	
	
};
