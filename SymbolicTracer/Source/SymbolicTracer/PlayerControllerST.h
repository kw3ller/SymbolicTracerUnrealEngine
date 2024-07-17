// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControllerST.generated.h"


// forward declarations
class ASpectatorPawnST;
class AHudST;
class UEnhancedInputComponent;
class UInputMappingContext;
class UInputAction;

/**
 * this is the playerController which handles PlayerActions (e.g. jumping)
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API APlayerControllerST : public APlayerController
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// inputAction to map to loading data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Data")
	TObjectPtr<UInputAction> ActionLoadData = nullptr;

	// inputAction to map to cycle the UI Modes (e.g. minimal UI, moderate UI etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|UI")
	TObjectPtr<UInputAction> ActionCycleUIMode = nullptr;

	// inputAction to map to toggle if the UI is clickable or if the World is clickable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|UI")
	TObjectPtr<UInputAction> ActionClickableUI = nullptr;

	// inputMappingContext to use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Player Input")
	TObjectPtr<UInputMappingContext> InputMappingContent = nullptr;

protected:

private:
	// store reference to the inputComponent (cast to enhancedInputComponent)
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;

	// store reference to pawn we are controlling
	UPROPERTY()
	TObjectPtr<ASpectatorPawnST> PlayerPawn = nullptr;

	// store reference to the HUD we are using
	UPROPERTY()
	TObjectPtr<AHudST> PlayerHud = nullptr;

	// ---METHODS---

public:


protected:
	// handles loading the trace data
	void HandleLoadData();
	// handles cycling the UI mode
	void HandleCycleUIMode();
	// handles toggling of UI clickable Mode
	void HandleClickableUI();

	// when it gets possessed by a pawn 
	virtual void OnPossess(APawn* aPawn) override;
	// when it is no longer possessed
	virtual void OnUnPossess() override;

private:
};
