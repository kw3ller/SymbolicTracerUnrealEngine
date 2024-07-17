// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreBase.h"

#include "CustomLogging.h"
#include "InstrStepsListBase.h"
#include "RTSPlayerPawnST.h"
#include "Components/TextBlock.h"


void UStoreBase::OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
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
		text.Append(Instruction.rs2d);
		CurrentRs2dText = FText::FromString(text);

		text = "Imm2: ";
		text.Append(Instruction.imm2);
		CurrentImm2Text = FText::FromString(text);
	}

	InstrStepsList->OnRunInstrStepIndexChanged(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);

	UpdateWidget();
	
}


void UStoreBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
	InstrStepsList->Init(PlayerChar);
}


void UStoreBase::UpdateWidget()
{
	
	RunIndText->SetText(CurrentRunIndText);
	OpcodeText->SetText(CurrentOpcodeText);
	PcText->SetText(CurrentPcText);
	PcHexText->SetText(CurrentPcHexText);
	Rs1Text->SetText(CurrentRs1Text);
	Rs2dText->SetText(CurrentRs2dText);
	Imm2Text->SetText(CurrentImm2Text);
	
}


void UStoreBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UE_LOG(STLog, Display, TEXT("NativeOnInitialized() in StoreBase"));
	UpdateWidget();
}

#if WITH_EDITOR
void UStoreBase::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	// Update the widget, after editor changes due to layout,
	// for example, resizing the widget, or a container that the widget is in.
	UpdateWidget();
}

void UStoreBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}
#endif
