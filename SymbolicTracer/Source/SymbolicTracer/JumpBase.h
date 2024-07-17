// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "JumpBase.generated.h"

class UButton;
class UTextBlock;
class ARTSPlayerPawnST;
class UInstrStepsListBase;
/**
 * widget for showing details of jump instructions
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UJumpBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

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

	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> OpcodeText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> PcText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> PcHexText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> LinkText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> LinkAddressText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Clicked Indices", meta=(BindWidget))
	TObjectPtr<UTextBlock> LinkAddressHexText = nullptr;

	

	// setting this text and then using its value in UpdateWidget
	UPROPERTY()
	FText CurrentRunIndText = FText::FromString("Run Index: -1");

	UPROPERTY()
	FText CurrentOpcodeText = FText::FromString("Opcode: none");

	UPROPERTY()
	FText CurrentPcText = FText::FromString("Pc: -1");

	UPROPERTY()
	FText CurrentPcHexText = FText::FromString("Pc (Hex): -1");

	UPROPERTY()
	FText CurrentLinkText = FText::FromString("Link: -1");

	UPROPERTY()
	FText CurrentLinkAddressText = FText::FromString("Link Address: -1");

	UPROPERTY()
	FText CurrentLinkAddressHexText = FText::FromString("Link Address (Hex): -1");

	// is the instructionList
	UPROPERTY()
	TObjectPtr<UInstrStepsListBase> InstrStepsList = nullptr;

	// reference to the player character (for reading the traceData)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;

private:

	

	// ---METHODS---

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
