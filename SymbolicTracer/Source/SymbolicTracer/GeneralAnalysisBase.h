// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "GeneralAnalysisBase.generated.h"

class ARTSPlayerPawnST;
class UTextBlock;
/**
 * HUD widget to show the general analysis data about the whole symbolic run
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UGeneralAnalysisBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// will hold the text for indicating the global start
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> GlobalStartHexText = nullptr;

	// will hold the text for indicating the number of runs of the whole trace
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> NumRunsText = nullptr;

	// will hold the text for indicating the min pc of the whole trace
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> MinPcHexText = nullptr;

	// will hold the text for indicating the max pc of the whole trace
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> MaxPcHexText = nullptr;

	// will hold the text for indicating the maximum callstack depth
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> MaxCallStackDepthText = nullptr;

	// will hold the text for indicating the general code coverage percentage of the whole trace
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> CodeCovPerText = nullptr;

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentGlobalStartHexText = FText::FromString("Global Start: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentNumRunsText = FText::FromString("Number of Paths: 0");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentMinPcHexText = FText::FromString("Min PC: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentMaxPcHexText = FText::FromString("Max PC: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentMaxCallStackDepthText = FText::FromString("Max Callstack depth: 0");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentCodeCovPerText = FText::FromString("Code Coverage: 100%");

protected:

private:

	// reference to the player character (for reading the traceData)
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;

	// ---METHODS---

public:

	// Function that can be called (e.g. by player) to update shown information
	UFUNCTION(BlueprintCallable)
	void OnRunIndexChange(int NewRunInd);

	// initialize (e.g. set PlayerCharacter from TestingLayoutBase)
	void Init(ARTSPlayerPawnST* PlayerChar);

protected:

private:

	// for visually updating the widget
	void UpdateWidget();
	
	
};
