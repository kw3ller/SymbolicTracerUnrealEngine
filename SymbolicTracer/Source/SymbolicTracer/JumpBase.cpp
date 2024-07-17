// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpBase.h"

#include "CustomLogging.h"
#include "InstrStepsListBase.h"
#include "RTSPlayerPawnST.h"
#include "Components/TextBlock.h"


void UJumpBase::OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
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

		text = "Link: ";
		text.Append(Instruction.link);
		CurrentLinkText = FText::FromString(text);
		
		text = "Link Address: ";
		text.Append(FString::FromInt(Instruction.link_address));
		CurrentLinkAddressText = FText::FromString(text);

		text = "Link Address (Hex): ";
		text.Append(Instruction.link_address_hex);
		CurrentLinkAddressHexText = FText::FromString(text);
	}

	InstrStepsList->OnRunInstrStepIndexChanged(NewRunInd, NewInstrInd, NewStepInd, NewScrollStepId);

	UpdateWidget();
	
}


void UJumpBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
	InstrStepsList->Init(PlayerChar);
}


void UJumpBase::UpdateWidget()
{
	
	RunIndText->SetText(CurrentRunIndText);
	OpcodeText->SetText(CurrentOpcodeText);
	PcText->SetText(CurrentPcText);
	PcHexText->SetText(CurrentPcHexText);
	LinkText->SetText(CurrentLinkText);
	LinkAddressText->SetText(CurrentLinkAddressText);
	LinkAddressHexText->SetText(CurrentLinkAddressHexText);
	
}


void UJumpBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UE_LOG(STLog, Display, TEXT("NativeOnInitialized() in JumpBase"));
	UpdateWidget();
}

#if WITH_EDITOR
void UJumpBase::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	// Update the widget, after editor changes due to layout,
	// for example, resizing the widget, or a container that the widget is in.
	UpdateWidget();
}

void UJumpBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}
#endif
