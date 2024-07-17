// Fill out your copyright notice in the Description page of Project Settings.


#include "InstrStepsListBase.h"

#include "CustomLogging.h"
#include "InstrStepsDataEntry.h"
#include "RTSPlayerPawnST.h"
#include "Components/ListView.h"

void UInstrStepsListBase::NativeConstruct()
{
	Super::NativeConstruct();

	StepsList->SetListItems(StepsData);
}

void UInstrStepsListBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
}

void UInstrStepsListBase::OnRunInstrStepIndexChanged(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
{
	UE_LOG(STLog, Warning,
	       TEXT("InsrListBase OnRunInstrStepIndexChanged: NewRunInd %d, NewInstrInd: %d, NewStepInd: %d: "), NewRunInd,
	       NewInstrInd, NewStepInd);

	if (NewRunInd < 0 || NewInstrInd < 0 || NewRunInd >= PlayerCharacter->TraceInstrStructure.runs.run.Num() ||
		NewInstrInd >= PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction.Num())
	{
		return;
	}

	// destroy every Object inside StepsData and empty it
	for (int i = 0; i < StepsData.Num(); i++)
	{
		StepsData[i]->ConditionalBeginDestroy();
		StepsData[i] = nullptr;
	}
	StepsData.Empty();


	// show all steps from this instruction -> 2D
	if (NewStepInd < 0)
	{
		// holds spawned depth with index -> will be used to set clickInd, when there are multiple steps with same depth 
		TMap<int, int> DepthsSpawned;
		for (int i = 0; i < PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].step.Num()
		     ; i++)
		{
			FStepInstr& NewStep = PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].step
				[i];
			int ClickedInd = i;
			if (DepthsSpawned.Contains(NewStep.depth))
			{
				ClickedInd = *DepthsSpawned.Find(NewStep.depth);
			}
			else
			{
				DepthsSpawned.Add(NewStep.depth, i);
			}


			UInstrStepsDataEntry* NewStepToShow = NewObject<UInstrStepsDataEntry>();
			NewStepToShow->Init(PlayerCharacter, NewRunInd, NewInstrInd, i, ClickedInd);
			StepsData.Add(NewStepToShow);
		}
	} // show specific step and all steps with same depth
	else if (NewStepInd >= 0 && NewStepInd < PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[
		NewInstrInd].step.Num())
	{
		FStepInstr& FirstRightStep = PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].step[
			NewStepInd];

		//UInstrStepsDataEntry* NewStepToShow = NewObject<UInstrStepsDataEntry>();
		//NewStepToShow->Init(PlayerCharacter, NewRunInd, NewInstrInd, NewStepInd, NewStepInd);
		//StepsData.Add(NewStepToShow);

		const int StepDepth = FirstRightStep.depth;

		for (int i = 0; i < PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].step.Num()
		     ; i++)
		{
			FStepInstr& Step = PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].step
				[i];

			if (Step.depth == StepDepth)
			{
				UInstrStepsDataEntry* NewStepToShow2 = NewObject<UInstrStepsDataEntry>();
				NewStepToShow2->Init(PlayerCharacter, NewRunInd, NewInstrInd, i, NewStepInd);
				StepsData.Add(NewStepToShow2);
			}
		}
	}

	// set NewScrollStepIndex
	NewScrollStepIndex = -1;
	for (int i = 0; i < StepsData.Num(); i++)
	{
		if(StepsData[i]->StepId == NewScrollStepId)
		{
			NewScrollStepIndex = i;
		}
	}

	UpdateWidget();
}

void UInstrStepsListBase::UpdateWidget()
{

	// unmark instructionStep
	if(MarkedInstructionStepDataEntry != nullptr)
	{
		MarkedInstructionStepDataEntry->Marked = false;
	}

	// mark new instructionStep
	if(NewScrollStepIndex > -1 && NewScrollStepIndex < StepsData.Num())
	{
		MarkedInstructionStepDataEntry = StepsData[NewScrollStepIndex];
		if(MarkedInstructionStepDataEntry != nullptr)
		{
			MarkedInstructionStepDataEntry->Marked = true;
		}
	}

	
	StepsList->SetListItems(StepsData);

	// has to be done otherwise the list entries don't change -> no marking
	StepsList->RegenerateAllEntries();

	// scroll to index (seemingly works fine even with -1 etc...)
	// StepsList->NavigateToIndex(NewScrollStepIndex);
	// StepsList->SetSelectedIndex(NewScrollStepIndex);
	StepsList->ScrollIndexIntoView(NewScrollStepIndex);
}
