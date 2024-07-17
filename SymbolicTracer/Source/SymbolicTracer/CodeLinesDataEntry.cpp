// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeLinesDataEntry.h"

#include "CustomLogging.h"
#include "RTSPlayerPawnST.h"




void UCodeLinesDataEntry::Init(ARTSPlayerPawnST* playerCharacter, int codeFileInd, int codeLineInd)
{
	PlayerCharacter = playerCharacter;
	CodeFileIndex = codeFileInd;
	CodeLineIndex = codeLineInd;
	Marked = false;


	// TODO: dont compute all this when scolling -> hold that data and get it from dataEntry 
	UE_LOG(STLog, Warning, TEXT("CodeLineBaseDataEntry Init called:"));


	// TODO: maybe we need to check if indices are bigger than -1 -> no do that when instantiating the corresponding dataThing 

	const FCodeLine& CodeLine = PlayerCharacter->TraceBlkStructure.code_files[CodeFileIndex].code_lines[CodeLineIndex];

	
	Content = CodeLine.Content;
	LineButtonText = FString::FromInt(CodeLineIndex+1);

	// set visibility of upper and lower clamps
	UpperClamp = CodeLine.CodeBlockBeginning;
	LowerClamp = CodeLine.CodeBlockEnding;

	// set symbolized/ concretized
	Symbolized = CodeLine.Symbolized;
	Concretized = CodeLine.Concretized;

	// color contentBackground if it got hit
	Pc = CodeLine.pc;
	CodeLineState = CodeLine.CodeLineState;
	
	// set BranchCount and BranchCov
	BranchCount = CodeLine.BranchCount;
	BranchCov = CodeLine.BranchCoverage;
	HitCount = CodeLine.HitCount;
}
