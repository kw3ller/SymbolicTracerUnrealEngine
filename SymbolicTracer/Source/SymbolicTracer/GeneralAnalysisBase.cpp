// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralAnalysisBase.h"

#include "RTSPlayerPawnST.h"
#include "Components/TextBlock.h"

void UGeneralAnalysisBase::OnRunIndexChange(int NewRunInd)
{
	FString Text = "Global Start: ";
	Text.Append(PlayerCharacter->TraceInstrStructure.analysis.global_start_hex);
	CurrentGlobalStartHexText = FText::FromString(Text);

	Text = "Number of Paths: ";
	Text.Append(FString::FromInt(PlayerCharacter->TraceInstrStructure.analysis.num_runs));
	CurrentNumRunsText = FText::FromString(Text);

	Text = "Min PC: ";
	Text.Append(PlayerCharacter->TraceInstrStructure.analysis.min_pc_hex);
	CurrentMinPcHexText = FText::FromString(Text);

	Text = "Max PC: ";
	Text.Append(PlayerCharacter->TraceInstrStructure.analysis.max_pc_hex);
	CurrentMaxPcHexText = FText::FromString(Text);

	Text = "Max Callstack depth: ";
	Text.Append(FString::FromInt(PlayerCharacter->TraceInstrStructure.analysis.max_depth));
	CurrentMaxCallStackDepthText = FText::FromString(Text);

	Text = "Code Coverage: ";
	Text.Append(FString::SanitizeFloat(PlayerCharacter->TraceInstrStructure.analysis.CodeCovPercentage) + "%");
	CurrentCodeCovPerText = FText::FromString(Text);


	UpdateWidget();
}


void UGeneralAnalysisBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
}

void UGeneralAnalysisBase::UpdateWidget()
{
	GlobalStartHexText->SetText(CurrentGlobalStartHexText);
	NumRunsText->SetText(CurrentNumRunsText);
	MinPcHexText->SetText(CurrentMinPcHexText);
	MaxPcHexText->SetText(CurrentMaxPcHexText);
	MaxCallStackDepthText->SetText(CurrentMaxCallStackDepthText);
	CodeCovPerText->SetText(CurrentCodeCovPerText);
}
