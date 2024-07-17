// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "CodeBase.generated.h"


class UTextBlock;
class ARTSPlayerPawnST;
class UCodeFilesDataEntry;
// forward declaration
class UCodeLineBase;
class UListView;
class UCodeLinesDataEntry;
class ASpectatorPawnST;

/**
 * Widget base for displaying CodeFiles in the UI (imagine simple VSCode layout)
 */
UCLASS()
class SYMBOLICTRACER_API UCodeBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// data list for holding the codeLines
	UPROPERTY(EditAnywhere)
	TArray<UCodeLinesDataEntry*> CodeLinesData;

	// marked codeLine (used for marking/unmarking)
	UPROPERTY()
	UCodeLinesDataEntry* MarkedCodeLineDataEntry = nullptr;

	// will visualize the codeLines 
	UPROPERTY(BlueprintReadOnly, Category = "Code Lines", meta = (BindWidget))
	TObjectPtr<UListView> CodeLinesList;

	// data list for holding the codeFiles
	UPROPERTY(EditAnywhere)
	TArray<UCodeFilesDataEntry*> CodeFilesData;

	// will visualize the codeFiles 
	UPROPERTY(BlueprintReadOnly, Category = "Code Files", meta = (BindWidget))
	TObjectPtr<UListView> CodeFilesList;

	UPROPERTY()
	FString CurrentFileNameText = "";

	// will show which file is currently selected
	UPROPERTY(BlueprintReadOnly, Category = "Code Files", meta = (BindWidget))
	TObjectPtr<UTextBlock> FileNameText = nullptr;
	
protected:
	// will hold the int for indicating which CodeFile was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices")
	int CodeFileInd = -1;

	// will hold the int for indicating which CodeLine was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices")
	int CodeLineInd = -1;

private:
	// reference to the player character (for reading the traceData)
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;


	// ---METHODS---

public:
	// gets called when building
	virtual void NativeConstruct() override;

	// initialize (e.g. set PlayerCharacter from TestingLayoutBase)
	void Init(ARTSPlayerPawnST* PlayerChar);

	// Function that can be called (e.g. by player) to update shown information (e.g. when clicking on InstructionActor in Scene)
	UFUNCTION(BlueprintCallable)
	void OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId);

	// Function that can be called (e.g. by player) to update shown information (e.g. when clicking on codeFile in HUD)
	UFUNCTION(BlueprintCallable)
	void OnCodeFileLineIndexChange(int NewCodeFileInd, int NewCodeLineInd);

	// Function that can be called (e.g. by player) to update shown information (e.g. when clicking on CodeBlockActor in Scene)
	UFUNCTION(BlueprintCallable)
	void OnRunCodeBlockIndexChange(int NewRunInd, int NewCodeBlockInd);
	

protected:

private:
	// for visually updating the widget
	void UpdateWidget();
};
