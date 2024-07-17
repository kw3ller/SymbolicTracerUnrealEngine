// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "AnimationButtonsBase.generated.h"

class USpinBox;
class UButton;
/**
 * Widget Base: these are the buttons used for controlling the animations in rtsPlayerPawn
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UAnimationButtonsBase : public UWidgetSTBase
{
	GENERATED_BODY()

public:

	// start/continue animation (implemented in BP)
	UPROPERTY(BlueprintReadOnly, Category="Play", meta=(BindWidget))
	TObjectPtr<UButton> PlayButton;

	// pause animation (implemented in BP)
	UPROPERTY(BlueprintReadOnly, Category="Play", meta=(BindWidget))
	TObjectPtr<UButton> PauseButton;

	// reset animation to initial start (implemented in BP)
	UPROPERTY(BlueprintReadOnly, Category="Play", meta=(BindWidget))
	TObjectPtr<UButton> ResetButton;

	// jump with animation to the last step (implemented in BP)
	UPROPERTY(BlueprintReadOnly, Category="Play", meta=(BindWidget))
	TObjectPtr<UButton> ToEndButton;

	// to animate selected run
	UPROPERTY(BlueprintReadOnly, Category="Index", meta=(BindWidget))
	TObjectPtr<USpinBox> RunIndexInput;

	// to input start stepId
	UPROPERTY(BlueprintReadOnly, Category="Index", meta=(BindWidget))
	TObjectPtr<USpinBox> StepIdInput;

	// to set the start stepId -> from input (implemented in BP)
	UPROPERTY(BlueprintReadOnly, Category="Play", meta=(BindWidget))
	TObjectPtr<UButton> SetStepIdButton;
	

	

	// TODO: some kind of visual indication if we play/pause etc.
	

	protected:

private:

	// ---METHODS---

	public:

protected:

private:
	
};
