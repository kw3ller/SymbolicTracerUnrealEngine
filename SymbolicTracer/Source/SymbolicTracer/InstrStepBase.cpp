// Fill out your copyright notice in the Description page of Project Settings.


#include "InstrStepBase.h"

#include "CustomLogging.h"
#include "InstrStepsDataEntry.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UInstrStepBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UE_LOG(STLog, Warning, TEXT("InstrStepBase NativeOnListItemObjectSet called:"));
	
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UInstrStepsDataEntry* Item = Cast<UInstrStepsDataEntry>(ListItemObject);

	RunIndex = Item->RunInd;
	InstructionIndex = Item->InstructionInd;
	StepIndex = Item->StepInd;
	StepClickIndex = Item->StepClickInd;

	if(Item->Marked)
	{
		MarkBorder->SetBrushColor(FColor(50, 140, 170, 255)); // blue
	} else
	{
		MarkBorder->SetBrushColor(FColor(255, 255, 255, 255)); // white
	}

	
	StepId->SetText(FText::FromString(FString::FromInt(Item->StepId)));
	Depth->SetText(FText::FromString(FString::FromInt(Item->Depth)));
	Behaviour->SetText(FText::FromString(Item->Behaviour));
	

	
}
