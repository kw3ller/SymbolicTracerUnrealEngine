// Fill out your copyright notice in the Description page of Project Settings.


#include "BranchBase.h"

#include "CustomLogging.h"
#include "InstrStepsListBase.h"
#include "RTSPlayerPawnST.h"
#include "Components/TextBlock.h"


void UBranchBase::OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
{

	
	RunInd = NewRunInd;
	InstrInd = NewInstrInd;

	FString text = "Run Index: ";
	text.Append(FString::FromInt(RunInd));
	CurrentRunIndText = FText::FromString(text);

	if (RunInd > -1 && InstrInd > -1 && RunInd < PlayerCharacter->TraceInstrStructure.runs.run.Num() && InstrInd <
		PlayerCharacter->TraceInstrStructure.runs.run[RunInd].instruction.Num())
	{
		const FInstruction& Instruction = PlayerCharacter->TraceInstrStructure.runs.run[RunInd].instruction[InstrInd];
		
		text = "Opcode: ";
		text.Append(Instruction.opcode);
		CurrentOpcodeText = FText::FromString(text);

		text = "Pc: ";
		text.Append(FString::FromInt(Instruction.pc));
		CurrentPcText = FText::FromString(text);

		text = "Pc (Hex): ";
		text.Append(Instruction.pc_hex);
		CurrentPcHexText = FText::FromString(text);

		text = "Rs1: ";
		text.Append(Instruction.rs1);
		CurrentRs1Text = FText::FromString(text);
		
		text = "Rs2: ";
		text.Append(Instruction.rs2);
		CurrentRs2Text = FText::FromString(text);

	}

	InstrStepsList->OnRunInstrStepIndexChanged(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);

	UpdateWidget();

	
}


void UBranchBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
	InstrStepsList->Init(PlayerChar);
}


void UBranchBase::UpdateWidget()
{
	
	RunIndText->SetText(CurrentRunIndText);
	OpcodeText->SetText(CurrentOpcodeText);
	PcText->SetText(CurrentPcText);
	PcHexText->SetText(CurrentPcHexText);
	Rs1Text->SetText(CurrentRs1Text);
	Rs2Text->SetText(CurrentRs2Text);
	
}


void UBranchBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UE_LOG(STLog, Display, TEXT("NativeOnInitialized() in BranchBase"));
	UpdateWidget();
}

#if WITH_EDITOR
void UBranchBase::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	// Update the widget, after editor changes due to layout,
	// for example, resizing the widget, or a container that the widget is in.
	UpdateWidget();
}

void UBranchBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}
#endif
