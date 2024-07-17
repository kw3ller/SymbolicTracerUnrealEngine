// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "MinimalLayoutBase.generated.h"

class UGeneralAnalysisBase;
class ARTSPlayerPawnST;
class UPathAnalysisBase;
/**
 * the base for our minimal layout from which our widget blueprint minimal layout inherits
 */
UCLASS()
class SYMBOLICTRACER_API UMinimalLayoutBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// for showing general analysis data of the whole symbolic run
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UGeneralAnalysisBase> GeneralAnalysis = nullptr;
	
	// for showing analysis data of individual runs
	UPROPERTY(BlueprintReadOnly, meta= (BindWidget))
	TObjectPtr<UPathAnalysisBase> PathAnalysis = nullptr;

	
	

protected:

private:
	// reference to the player character
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;

	// ---METHODS---

public:
	void Init(ARTSPlayerPawnST* PlayerChar);

protected:

private:
	
};
