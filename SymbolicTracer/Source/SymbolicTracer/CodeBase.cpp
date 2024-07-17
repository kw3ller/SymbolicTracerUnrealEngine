// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeBase.h"

#include "CodeFilesDataEntry.h"
#include "CodeLinesDataEntry.h"
#include "RTSPlayerPawnST.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"


void UCodeBase::NativeConstruct()
{
	Super::NativeConstruct();

	CodeLinesList->SetListItems(CodeLinesData);
	CodeFilesList->SetListItems(CodeFilesData);
}

void UCodeBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
}

void UCodeBase::OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId)
{
	if (NewRunInd < 0 || NewInstrInd < 0 || NewRunInd >= PlayerCharacter->TraceInstrStructure.runs.run.Num() ||
		NewInstrInd >= PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction.Num())
	{
		return;
	}
	CodeFileInd = PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].code_file_id;
	CodeLineInd = PlayerCharacter->TraceInstrStructure.runs.run[NewRunInd].instruction[NewInstrInd].code_line - 1;


	if (CodeFileInd > -1 && CodeFileInd < PlayerCharacter->TraceBlkStructure.code_files.Num())
	{
		// destroy every Object inside CodeLinesData and empty it
		for (int i = 0; i < CodeLinesData.Num(); i++)
		{
			CodeLinesData[i]->ConditionalBeginDestroy();
			CodeLinesData[i] = nullptr;
		}
		CodeLinesData.Empty();


		// destroy every Object inside CodeFilesData and empty it
		// NOTE: this does not have to happen every click (unless we have something like search files implemented) 
		for (int i = 0; i < CodeFilesData.Num(); i++)
		{
			CodeFilesData[i]->ConditionalBeginDestroy();
			CodeFilesData[i] = nullptr;
		}
		CodeFilesData.Empty();


		// load the file data into codeFilesData
		// NOTE: this does not have to happen every click (unless we have something like search files implemented) 
		for (int i = 0; i < PlayerCharacter->TraceBlkStructure.code_files.Num(); i++)
		{
			UCodeFilesDataEntry* NewFileToShow = NewObject<UCodeFilesDataEntry>();
			NewFileToShow->Init(PlayerCharacter->TraceBlkStructure.code_files[i].name,
			                    PlayerCharacter->TraceBlkStructure.code_files[i].CodeCovPercentage, i
			);
			CodeFilesData.Add(NewFileToShow);
		}


		// load the line data into codeLinesData (gets copied!)
		CodeLinesData = PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].CodeLinesDataEntries;

		CurrentFileNameText = PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].name;


		/*
		for (int i = 0; i < PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].code_lines.Num(); i++)
		{
			UCodeLinesDataEntry* newLineToShow = NewObject<UCodeLinesDataEntry>();
			newLineToShow->Init(PlayerCharacter, CodeFileInd, i);
			CodeLinesData.Add(newLineToShow);
		}
		*/
	}
	UpdateWidget();
}

void UCodeBase::OnCodeFileLineIndexChange(int NewCodeFileInd, int NewCodeLineInd)
{
	CodeFileInd = NewCodeFileInd;
	CodeLineInd = NewCodeLineInd;


	if (CodeFileInd > -1 && CodeFileInd < PlayerCharacter->TraceBlkStructure.code_files.Num())
	{
		// destroy every Object inside CodeLinesData and empty it
		for (int i = 0; i < CodeLinesData.Num(); i++)
		{
			CodeLinesData[i]->ConditionalBeginDestroy();
			CodeLinesData[i] = nullptr;
		}
		CodeLinesData.Empty();

		// destroy every Object inside CodeFilesData and empty it
		// NOTE: this does not have to happen every click (unless we have something like search files implemented) 
		for (int i = 0; i < CodeFilesData.Num(); i++)
		{
			CodeFilesData[i]->ConditionalBeginDestroy();
			CodeFilesData[i] = nullptr;
		}
		CodeFilesData.Empty();

		// NOTE: this does not have to happen every click (unless we have something like search files implemented) (gets copied!)
		CodeFilesData = PlayerCharacter->TraceBlkStructure.CodeFilesDataEntries;


		// load the line data into codeLinesData (gets copied!)
		CodeLinesData = PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].CodeLinesDataEntries;

		CurrentFileNameText = PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].name;
	}
	UpdateWidget();
}

void UCodeBase::OnRunCodeBlockIndexChange(int NewRunInd, int NewCodeBlockInd)
{
	if (NewRunInd < 0 || NewCodeBlockInd < 0 || NewRunInd >= PlayerCharacter->TraceBlkStructure.run_blocks.Num() ||
		NewCodeBlockInd >= PlayerCharacter->TraceBlkStructure.run_blocks[NewRunInd].block.Num())
	{
		return;
	}

	const int blockId = PlayerCharacter->TraceBlkStructure.run_blocks[NewRunInd].block[NewCodeBlockInd].block_id;

	if (PlayerCharacter->TraceBlkStructure.cfg_blocks[blockId].code_file_id < 0 || PlayerCharacter->TraceBlkStructure.
		cfg_blocks[blockId].line_start - 1 < 0)
	{
		return;
	}

	CodeFileInd = PlayerCharacter->TraceBlkStructure.cfg_blocks[blockId].code_file_id;
	CodeLineInd = PlayerCharacter->TraceBlkStructure.cfg_blocks[blockId].line_start - 1;


	if (CodeFileInd > -1 && CodeFileInd < PlayerCharacter->TraceBlkStructure.code_files.Num())
	{
		// destroy every Object inside CodeLinesData and empty it
		for (int i = 0; i < CodeLinesData.Num(); i++)
		{
			CodeLinesData[i]->ConditionalBeginDestroy();
			CodeLinesData[i] = nullptr;
		}
		CodeLinesData.Empty();


		// destroy every Object inside CodeFilesData and empty it
		// NOTE: this does not have to happen every click (unless we have something like search files implemented) 
		for (int i = 0; i < CodeFilesData.Num(); i++)
		{
			CodeFilesData[i]->ConditionalBeginDestroy();
			CodeFilesData[i] = nullptr;
		}
		CodeFilesData.Empty();


		// load the file data into codeFilesData
		// NOTE: this does not have to happen every click (unless we have something like search files implemented) 
		for (int i = 0; i < PlayerCharacter->TraceBlkStructure.code_files.Num(); i++)
		{
			UCodeFilesDataEntry* newFileToShow = NewObject<UCodeFilesDataEntry>();
			newFileToShow->Init(PlayerCharacter->TraceBlkStructure.code_files[i].name,
			                    PlayerCharacter->TraceBlkStructure.code_files[i].CodeCovPercentage, i
			);
			CodeFilesData.Add(newFileToShow);
		}

		// load the line data into codeLinesData (gets copied!)
		CodeLinesData = PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].CodeLinesDataEntries;

		CurrentFileNameText = PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].name;
		/*
		for (int i = 0; i < PlayerCharacter->TraceBlkStructure.code_files[CodeFileInd].code_lines.Num(); i++)
		{
			UCodeLinesDataEntry* newLineToShow = NewObject<UCodeLinesDataEntry>();
			newLineToShow->Init(PlayerCharacter, CodeFileInd, i);
			CodeLinesData.Add(newLineToShow);
		}
		*/
	}
	UpdateWidget();
}

void UCodeBase::UpdateWidget()
{
	// TODO: mark the right codeLine like bellow -> keep reference and unmark when something changes!
	// unmark previously marked codeLine 
	if (MarkedCodeLineDataEntry != nullptr)
	{
		MarkedCodeLineDataEntry->Marked = false;
	}

	// mark new codeLine
	if (CodeLineInd > -1 && CodeLineInd < CodeLinesData.Num())
	{
		MarkedCodeLineDataEntry = CodeLinesData[CodeLineInd];
		if (MarkedCodeLineDataEntry != nullptr)
		{
			MarkedCodeLineDataEntry->Marked = true;
		}
	}


	CodeLinesList->SetListItems(CodeLinesData);
	CodeFilesList->SetListItems(CodeFilesData);

	FileNameText->SetText(FText::FromString(CurrentFileNameText));

	// has to be done otherwise the list entries don't change -> no marking
	CodeLinesList->RegenerateAllEntries();

	// scroll to index (seemingly works fine even with -1 etc...)
	CodeLinesList->ScrollIndexIntoView(CodeLineInd);
}
