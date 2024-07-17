// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "PathAnalysisBase.generated.h"

class ARTSPlayerPawnST;
class UTextBlock;
/**
 * HUD widget to show analysis data for individual paths
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UPathAnalysisBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

protected:

	// will hold the int for indicating which run was clicked by user -> might be used in future for buttons
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices")
	int RunInd = -1;

	// TODO: add meta bind widget
		
	// will hold the text for indicating which run was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> RunIndText = nullptr;

	// will hold the text for indicating which run was the parent of this run
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> ParentRunIndText = nullptr;

	// will hold the text for indicating where the start is
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> StartPcText = nullptr;

	// will hold the text for indicating where the end is
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> EndPcText = nullptr;

	// will hold the text for indicating how many steps this run has
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> StepsNumText = nullptr;

	// will hold the text for indicating the step number at which this run was split 
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> SplitStartStepNumText = nullptr;

	// will hold the text for indicating the step pc at which this run was split 
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> SplitStartPcText = nullptr;
	
	// will hold the text for indicating the number of codeBlocks for this run
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> CodeBlocksNumText = nullptr;

	// will hold the text for indicating the code coverage for this run
	UPROPERTY(BlueprintReadOnly, Category="Data", meta=(BindWidget))
	TObjectPtr<UTextBlock> CodeCoveragePercentageText = nullptr;

	
	

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentRunIndText = FText::FromString("Path Index: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentParentRunIndText = FText::FromString("Parent Path Index: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentStartPcText = FText::FromString("StartPc: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentEndPcText = FText::FromString("End Pc: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentStepsNumText = FText::FromString("Number of Steps: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentSplitStartStepNumText = FText::FromString("Split Step Number: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentSplitStartPcText = FText::FromString("Split Pc: -1");
	
	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentCodeBlkNumText = FText::FromString("Number of CodeBlocks: 0");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentCodeCoverageText = FText::FromString("Code coverage: 0%");

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

#if WITH_EDITOR
	// allows us to test how the widget looks, when we change the percentage and value fields
	// when we inside the editor
	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:

private:

	// for visually updating the widget
	void UpdateWidget();
	
};
