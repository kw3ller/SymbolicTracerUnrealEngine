// Fill out your copyright notice in the Description page of Project Settings.


#include "PathAnalysisBase.h"

#include "RTSPlayerPawnST.h"
#include "Components/TextBlock.h"

void UPathAnalysisBase::OnRunIndexChange(int NewRunInd)
{
	RunInd = NewRunInd;

	FString text = "Path Index: ";
	text.Append(FString::FromInt(RunInd));
	CurrentRunIndText = FText::FromString(text);

	if(RunInd > -1 && RunInd < PlayerCharacter->TraceInstrStructure.analysis.analyis_paths.Num())
	{
		text = "Parent Path Index: ";
		text.Append(FString::FromInt(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].ParentRunInd));
		CurrentParentRunIndText = FText::FromString(text);

		text = "Start Pc: ";
		text.Append(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].StartPcHex);
		CurrentStartPcText = FText::FromString(text);

		text = "End Pc: ";
		text.Append(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].EndPcHex);
		CurrentEndPcText = FText::FromString(text);

		text = "Number of Steps: ";
		text.Append(FString::FromInt(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].StepsNum));
		CurrentStepsNumText = FText::FromString(text);

		text = "Split Step Number: ";
		text.Append(FString::FromInt(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].SplitStartStepNumber));
		CurrentSplitStartStepNumText = FText::FromString(text);

		text = "Split Pc: ";
		text.Append(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].SplitStartPcHex);
		CurrentSplitStartPcText = FText::FromString(text);

		text = "Number of CodeBlocks: ";
		text.Append(FString::FromInt(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].CodeBlocksNum));
		CurrentCodeBlkNumText = FText::FromString(text);

		text = "Code coverage: ";
		text.Append(FString::SanitizeFloat(PlayerCharacter->TraceInstrStructure.analysis.analyis_paths[RunInd].CodeCoveragePercentage) + "%");
		CurrentCodeCoverageText = FText::FromString(text);
		
	}

	UpdateWidget();
}

void UPathAnalysisBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
}

void UPathAnalysisBase::UpdateWidget()
{
	RunIndText->SetText(CurrentRunIndText);
	ParentRunIndText->SetText(CurrentParentRunIndText);
	StartPcText->SetText(CurrentStartPcText);
	EndPcText->SetText(CurrentEndPcText);
	StepsNumText->SetText(CurrentStepsNumText);
	SplitStartStepNumText->SetText(CurrentSplitStartStepNumText);
	SplitStartPcText->SetText(CurrentSplitStartPcText);
	CodeBlocksNumText->SetText(CurrentCodeBlkNumText);
	CodeCoveragePercentageText->SetText(CurrentCodeCoverageText);

}


#if WITH_EDITOR
void UPathAnalysisBase::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	// Update the widget, after editor changes due to layout,
	// for example, resizing the widget, or a container that the widget is in.
	UpdateWidget();
}

void UPathAnalysisBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}
#endif