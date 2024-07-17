// Fill out your copyright notice in the Description page of Project Settings.


#include "InstrStepsDataEntry.h"

#include "CustomLogging.h"
#include "DataStructs.h"
#include "RTSPlayerPawnST.h"

void UInstrStepsDataEntry::Init(ARTSPlayerPawnST* playerCharacter, int runInd, int instructionInd, int stepInd, int clickInd)
{
	PlayerCharacter = playerCharacter;
	RunInd = runInd;
	InstructionInd = instructionInd;
	StepInd = stepInd;
	StepClickInd = clickInd;
	Marked = false;

	// TODO: dont compute all this when scolling -> hold that data and get it from dataEntry 
	UE_LOG(STLog, Warning, TEXT("InstrStepsDataEntry Init called:"));

	// TODO: maybe we need to check if indices are bigger than -1 or are in general correct -> no do that when instantiating the corresponding dataThing 

	const FStepInstr& Step = PlayerCharacter->TraceInstrStructure.runs.run[RunInd].instruction[InstructionInd].step[StepInd];

	StepId = Step.id;
	Behaviour = Step.beh;
	Depth = Step.depth;
	Condition = Step.condition;
	Target = Step.target;
	
	
}
