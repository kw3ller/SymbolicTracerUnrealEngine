// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerControllerST.generated.h"

// forward declarations


class AHudST;
struct FInputActionValue;
class ARTSPlayerPawnST;
class UEnhancedInputComponent;
class UInputMappingContext;
class UInputAction;


/**
 * the rts player controller handling the inputs
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API ARTSPlayerControllerST : public APlayerController
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionForward = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionRight = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionZoom = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionRotate = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionRotateRight = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionRotateLeft = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionRotateHorizontal = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Movement")
	TObjectPtr<UInputAction> ActionRotateVertical = nullptr;

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
	TObjectPtr<ARTSPlayerPawnST> PlayerPawn = nullptr;

	// store reference to the HUD we are using
	UPROPERTY()
	TObjectPtr<AHudST> PlayerHud = nullptr;

	// ---METHODS---

public:

protected:


	// handle movement
	void HandleForward(const FInputActionValue& InputActionValue);
	void HandleRight(const FInputActionValue& InputActionValue);
	void HandleZoom(const FInputActionValue& InputActionValue);
	void HandleRotate(const FInputActionValue& InputActionValue);
	void HandleRotateRight();
	void HandleRotateLeft();
	void HandleRotateHorizontal(const FInputActionValue& InputActionValue);
	void HandleRotateVertical(const FInputActionValue& InputActionValue);

	// when it gets possessed by a pawn
	virtual void OnPossess(APawn* InPawn) override;
	// when it is no longer possessed
	virtual void OnUnPossess() override;

	// handles loading the trace data  -> NOTE: no longer in use! now loading data is called via blueprints
	void HandleLoadData();
	// handles cycling the UI mode
	void HandleCycleUIMode();

private:
	
};
