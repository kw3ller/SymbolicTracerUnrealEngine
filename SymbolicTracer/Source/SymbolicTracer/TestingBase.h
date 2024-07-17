// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "TestingBase.generated.h"

class ARTSPlayerPawnST;
// forward declaration
class UTextBlock;
class ASpectatorPawnST;


/**
 * for a testing base widget
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UTestingBase : public UWidgetSTBase
{
	GENERATED_BODY()


	//---VARIABLES---

public:

protected:
	// will hold the int for indicating which run was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices")
	int RunInd = -1;

	// will hold the int for indicating which instruction was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices")
	int InstrInd = -1;

	// will hold the text for indicating which run was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> RunIndText = nullptr;

	// will hold the text for indicating which instruction was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> InstrIndText = nullptr;

	// will hold the text for indicating which instruction type was clicked by user
	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> InstrTypeText = nullptr;

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentRunIndText = FText::FromString("Run Index: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentInstrIndText = FText::FromString("Instruction Index: -1");

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentInstrTypeText = FText::FromString("Instruction Type: None");

private:
	// reference to the player character (for reading the traceData)
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;


	//---METHODS---

public:
	// Function that can be called (e.g. by player) to update shown information
	UFUNCTION(BlueprintCallable)
	void OnRunInstrIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId);

	// initialize (e.g. set PlayerCharacter from TestingLayoutBase)
	void Init(ARTSPlayerPawnST* PlayerChar);


#if WITH_EDITOR
	// allows us to test how the widget looks, when we change the percentage and value fields
	// when we inside the editor
	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// gets called when the statBar get initialized (at runtime when the game is playing) 
	virtual void NativeOnInitialized() override;

private:
	// for visually updating the widget
	void UpdateWidget();
};
