// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "AbstractionButtonsBase.generated.h"

class UEditableTextBox;
class UButton;
/**
 * Widget Base: these are the buttons to load trace data and switch the different abstraction levels of the visualization 
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UAbstractionButtonsBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// load trace data
	UPROPERTY(BlueprintReadOnly, Category="Abstraction Level", meta=(BindWidget))
	TObjectPtr<UButton> LoadTraceData;

	// change abstraction level to 1
	UPROPERTY(BlueprintReadOnly, Category="Abstraction Level", meta=(BindWidget))
	TObjectPtr<UButton> Level1;

	// change abstraction level to 2
	UPROPERTY(BlueprintReadOnly, Category="Abstraction Level", meta=(BindWidget))
	TObjectPtr<UButton> Level2;

	// change abstraction level to 3
	UPROPERTY(BlueprintReadOnly, Category="Abstraction Level", meta=(BindWidget))
	TObjectPtr<UButton> Level3;

	// change abstraction level to 4
	UPROPERTY(BlueprintReadOnly, Category="Abstraction Level", meta=(BindWidget))
	TObjectPtr<UButton> Level4;

	// the input box for the filePath
	UPROPERTY(BlueprintReadOnly, Category="Load", meta=(BindWidget))
	TObjectPtr<UEditableTextBox> PathInput;

	// the OK button to finally load the trace data
	UPROPERTY(BlueprintReadOnly, Category="Load", meta=(BindWidget))
	TObjectPtr<UButton> Load;

	// needed to set visibility 
	UPROPERTY(BlueprintReadWrite, Category="Load")
	bool LoadingVisibility = false;

	// needed to set the loading path
	UPROPERTY(BlueprintReadWrite, Category="Load")
	FString PathString = "";
	

protected:

private:

	// ---METHODS---

public:

protected:

private:
	
	
};
