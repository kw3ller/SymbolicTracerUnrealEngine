// Fill out your copyright notice in the Description page of Project Settings.


#include "InstrStepBaseBranch.h"
#include "InstrStepsDataEntry.h"
#include "CustomLogging.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

class UInstrStepsDataEntry;

void UInstrStepBaseBranch::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UE_LOG(STLog, Warning, TEXT("InstrStepBaseBranch NativeOnListItemObjectSet called:"));
	
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UInstrStepsDataEntry* Item = Cast<UInstrStepsDataEntry>(ListItemObject);

	RunIndex = Item->RunInd;
	InstructionIndex = Item->InstructionInd;
	StepIndex = Item->StepInd;
	StepClickIndex = Item->StepClickInd;
	Target = Item->Target;
	Condition = Item->Condition;

	if(Item->Marked)
	{
		MarkBorder->SetBrushColor(FColor(50, 140, 170, 255)); // blue
	} else
	{
		MarkBorder->SetBrushColor(FColor(255, 255, 255, 255)); // white
	}

	
	StepId->SetText(FText::FromString(FString::FromInt(Item->StepId)));
	Depth->SetText(FText::FromString(FString::FromInt(Item->Depth)));
	TargetText->SetText(FText::FromString(FString::FromInt(Item->Target)));
	Behaviour->SetText(FText::FromString(Item->Behaviour));

	Condition ? ConditionText->SetText(FText::FromString("True")) : ConditionText->SetText(FText::FromString("False")); 
}
