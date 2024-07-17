// Fill out your copyright notice in the Description page of Project Settings.


#include "TestingLayoutBase.h"

#include "ArithBase.h"
#include "BranchBase.h"
#include "CodeBase.h"
#include "JumpBase.h"
#include "LoadBase.h"
#include "RTSPlayerPawnST.h"
#include "StoreBase.h"
#include "TestingBase.h"


void UTestingLayoutBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;

	// Testing->Init(PlayerCharacter);

	ArithInstrInfo->Init(PlayerCharacter);
	JumpInstrInfo->Init(PlayerCharacter);
	StoreInstrInfo->Init(PlayerCharacter);
	LoadInstrInfo->Init(PlayerCharacter);
	BranchInstrInfo->Init(PlayerCharacter);

	CodeInfo->Init(PlayerCharacter);

	ArithInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
	JumpInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
	StoreInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
	LoadInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
	BranchInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
}

void UTestingLayoutBase::OnRunInstrStepIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
{
	// TODO: show/hide instructionSpecific Widgets and change their indices

	if (NewRunInd > -1 && NewInstrInd > -1 && NewRunInd < PlayerCharacter->TraceInstrStructure.runs.run.Num() &&
		NewInstrInd < PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction.Num())
	{
		EInstructionType* InstrTypeVal = PlayerCharacter->InstructionTypeTable.Find(
			PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].type);
		EInstructionType InstrType = EInstructionType::Default;

		if (InstrTypeVal)
		{
			InstrType = *InstrTypeVal;
		}

		ArithInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
		JumpInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
		StoreInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
		LoadInstrInfo->SetVisibility(ESlateVisibility::Collapsed);
		BranchInstrInfo->SetVisibility(ESlateVisibility::Collapsed);

		switch (InstrType)
		{
		case EInstructionType::Default:
			break;
		case EInstructionType::Arith:
			ArithInstrInfo->SetVisibility(ESlateVisibility::Visible);
			ArithInstrInfo->OnRunInstrIndexChange(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);
			break;
		case EInstructionType::Jump:
			JumpInstrInfo->SetVisibility(ESlateVisibility::Visible);
			JumpInstrInfo->OnRunInstrIndexChange(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);
			break;
		case EInstructionType::Load:
			LoadInstrInfo->SetVisibility(ESlateVisibility::Visible);
			LoadInstrInfo->OnRunInstrIndexChange(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);
			break;
		case EInstructionType::Store:
			StoreInstrInfo->SetVisibility(ESlateVisibility::Visible);
			StoreInstrInfo->OnRunInstrIndexChange(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);
			break;
		case EInstructionType::Branch:
			BranchInstrInfo->SetVisibility(ESlateVisibility::Visible);
			BranchInstrInfo->OnRunInstrIndexChange(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);
			break;
		default:
			break;
		}
	}

	CodeInfo->OnRunInstrIndexChange( NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);
}

void UTestingLayoutBase::OnCodeFileLineIndexChange(int NewCodeFileInd, int NewCodeLineInd)
{
	CodeInfo->OnCodeFileLineIndexChange(NewCodeFileInd, NewCodeLineInd);
}

void UTestingLayoutBase::OnRunCodeBlockIndexChange(int NewRunInd, int NewCodeBlockInd)
{
	CodeInfo->OnRunCodeBlockIndexChange(NewRunInd, NewCodeBlockInd);
}
