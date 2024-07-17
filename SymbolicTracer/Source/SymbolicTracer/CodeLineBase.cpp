// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeLineBase.h"

#include "CodeLinesDataEntry.h"
#include "CustomLogging.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCodeLineBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UE_LOG(STLog, Warning, TEXT("CodeLineBase NativeOnListItemObjectSet called:"));

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UCodeLinesDataEntry* Item = Cast<UCodeLinesDataEntry>(ListItemObject);
	CodeFileIndex = Item->CodeFileIndex;
	CodeLineIndex = Item->CodeLineIndex;

	UE_LOG(STLog, Warning, TEXT("CodeLineBase NativeOnListItemObjectSet called: fileInd: %d, line Ind: %d"),
	       CodeFileIndex, CodeLineIndex);

	Content->SetText(FText::FromString(Item->Content));
	LineButtonText->SetText(FText::FromString(Item->LineButtonText));

	// set branchCount and branchCov
	BranchCountText->SetText(FText::FromString(FString::FromInt(Item->BranchCount)));
	BranchCovText->SetText(FText::FromString(FString::FromInt(Item->BranchCov)));

	// color branchCovBackground
	if (Item->CodeLineState == ECodeLineState::None)
	{
		BranchCountBackground->SetBrushColor(FColor(120, 120, 120, 255)); // grey
		BranchCovBackground->SetBrushColor(FColor(120, 120, 120, 255)); // grey
	}
	else
	{
		if (Item->BranchCount <= 0)
		{
			BranchCountBackground->SetBrushColor(FColor(150, 150, 150, 255)); // ligthtGrey
			BranchCovBackground->SetBrushColor(FColor(150, 150, 150, 255)); // lightGrey
		}
		else if (Item->BranchCov < Item->BranchCount)
		{
			BranchCountBackground->SetBrushColor(FColor(255, 255, 255, 255)); // white
			BranchCovBackground->SetBrushColor(FColor(252, 44, 3, 255)); // red
		}
		else
		{
			BranchCountBackground->SetBrushColor(FColor(255, 255, 255, 255)); // white
			BranchCovBackground->SetBrushColor(FColor(23, 255, 3, 255)); // green
		}
	}

	// set line coverageText and background
	HitCountText->SetText(FText::FromString(FString::FromInt(Item->HitCount)));

	if (Item->CodeLineState == ECodeLineState::None)
	{
		HitCountBackground->SetBrushColor(FColor(120, 120, 120, 255)); // grey
		SymConBackground->SetBrushColor(FColor(120, 120, 120, 255)); // grey
	}
	else
	{
		HitCountBackground->SetBrushColor(FColor(255, 255, 255, 255)); // white
		SymConBackground->SetBrushColor(FColor(255, 255, 255, 255)); // white
	}

	// set symConText
	FString SymConString = "";
	if (Item->Symbolized)
	{
		SymConString += "S";
	}
	if (Item->Concretized)
	{
		SymConString += "C";
	}
	SymConText->SetText(FText::FromString(SymConString));


	// set visibility of upper and lower clamps
	Item->UpperClamp
		? UpperClamp->SetVisibility(ESlateVisibility::SelfHitTestInvisible)
		: UpperClamp->SetVisibility(ESlateVisibility::Collapsed);
	Item->LowerClamp
		? LowerClamp->SetVisibility(ESlateVisibility::SelfHitTestInvisible)
		: LowerClamp->SetVisibility(ESlateVisibility::Collapsed);

	Pc = Item->Pc;

	// set codeBackgroundColor
	// NOTE: else case HAS to be specified. For some reason otherwise random coloring with default and hitColor
	// 
	switch (Item->CodeLineState)
	{
	case ECodeLineState::None:
		ContentBackground->SetBrushColor(FColor(120, 120, 120, 255)); // grey
		break;
	case ECodeLineState::Compiled:
		ContentBackground->SetBrushColor(FColor(252, 44, 3, 255)); // red
		break;
	case ECodeLineState::Hit:
		ContentBackground->SetBrushColor(FColor(23, 255, 3, 255)); // green
		break;
	}


	// mark/unmark codeLine and lineButton
	if (Item->Marked)
	{
		MarkBorder->SetBrushColor(FColor(50, 140, 170, 255)); // blue
		LineButton->SetBackgroundColor(FColor(50, 140, 170, 255));
	}
	else
	{
		MarkBorder->SetBrushColor(FColor(100, 100, 100, 100)); // grey
		LineButton->SetBackgroundColor(FColor(255, 255, 255, 255));
	}
}
