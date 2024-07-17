// Fill out your copyright notice in the Description page of Project Settings.


#include "TestingBase.h"

#include "CustomLogging.h"
#include "RTSPlayerPawnST.h"
#include "Components/TextBlock.h"


void UTestingBase::OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
{
	RunInd = NewRunInd;
	InstrInd = NewInstrInd;

	FString text = "Run Index: ";
	text.Append(FString::FromInt(RunInd));
	CurrentRunIndText = FText::FromString(text);

	text = "Instruction Index: ";
	text.Append(FString::FromInt(InstrInd));
	CurrentInstrIndText = FText::FromString(text);

	if (RunInd > -1 && InstrInd > -1 && RunInd < PlayerCharacter->TraceInstrStructure.runs.run.Num() && InstrInd < PlayerCharacter->TraceInstrStructure.runs.run[RunInd].instruction.Num())
	{
		text = "Instruction Type: ";
		text.Append(PlayerCharacter->TraceInstrStructure.runs.run[RunInd].instruction[InstrInd].type);
		CurrentInstrTypeText = FText::FromString(text);
	}


	UpdateWidget();
}

void UTestingBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
}


void UTestingBase::UpdateWidget()
{
	InstrIndText->SetText(CurrentInstrIndText);
	RunIndText->SetText(CurrentRunIndText);
	InstrTypeText->SetText(CurrentInstrTypeText);
}


void UTestingBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UE_LOG(STLog, Display, TEXT("NativeOnInitialized() in TestingBase"));
	UpdateWidget();
}

#if WITH_EDITOR
void UTestingBase::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	// Update the widget, after editor changes due to layout,
	// for example, resizing the widget, or a container that the widget is in.
	UpdateWidget();
}

void UTestingBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}
#endif
