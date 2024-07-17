// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InstrStepBaseJump.generated.h"

class UBorder;
class UButton;
class UTextBlock;
/**
 * Widget base for displaying individual instructionSteps (e.g. inside instructionWidget)
 * for jump instruction
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UInstrStepBaseJump : public UWidgetSTBase, public IUserObjectListEntry
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

	// shows the target of the step
	UPROPERTY(BlueprintReadOnly, Category="instruction step", meta=(BindWidget))
	TObjectPtr<UTextBlock> TargetText = nullptr;
	
	// shows the targetButton of the step -> behaviour will be set in bluePrint
	UPROPERTY(BlueprintReadOnly, Category="instruction step", meta=(BindWidget))
	TObjectPtr<UButton> TargetButton = nullptr;

	// the run index of this step
	UPROPERTY(BlueprintReadOnly)
	int RunIndex = -1;
	
	// the instruction index of this step
	UPROPERTY(BlueprintReadOnly)
	int InstructionIndex = -1;
	
	// the step index of this step
	UPROPERTY(BlueprintReadOnly)
	int StepIndex = -1;

	// the step index of this step
	UPROPERTY(BlueprintReadOnly)
	int StepClickIndex = -1;

	// the step index of this step
	UPROPERTY(BlueprintReadOnly)
	int Target = -1;

protected:

private:

	// ---METHODS---

	public:

protected:

	// gets called when initializing (e.g. set content to show)
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:

};
