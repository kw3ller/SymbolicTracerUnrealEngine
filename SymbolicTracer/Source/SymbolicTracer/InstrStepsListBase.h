// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "InstrStepsListBase.generated.h"

class ARTSPlayerPawnST;
class UListView;
class UInstrStepsDataEntry;
/**
 * Widget for displaying InstructionSteps in the UI
 */
UCLASS()
class SYMBOLICTRACER_API UInstrStepsListBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// data list for holding the steps
	UPROPERTY(EditAnywhere)
	TArray<UInstrStepsDataEntry*> StepsData;

	// marked instructionStep (used for marking/unmarking)
	UPROPERTY()
	UInstrStepsDataEntry* MarkedInstructionStepDataEntry = nullptr;

	// will visualize the steps 
	UPROPERTY(BlueprintReadOnly, Category = "Steps", meta = (BindWidget))
	TObjectPtr<UListView> StepsList;

	// will scroll to this index
	UPROPERTY()
	int NewScrollStepIndex = -1;

protected:

private:

	// reference to the player character (for reading the traceData)
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;


	// ---METHODS---

public:

	// gets called when building
	virtual void NativeConstruct() override;

	// initialize (e.g. set PlayerCharacter from TestingLayoutBase)
	void Init(ARTSPlayerPawnST* PlayerChar);

	// load into stepsList:
	// if stepInd = -1 -> load all steps in that instruction
	// else load that specific step and all steps with the same depth of that instruction
	// NOTE: should only be changed when clicking on instruction actor from scene, or when animating run(but animation has to check if indices are valid?)
	// scrollStepId: scroll to listItem with specified stepId 
	void OnRunInstrStepIndexChanged(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId);

protected:

private:

	// for visually updating the widget
	void UpdateWidget();
	
};
