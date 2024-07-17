// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "TestingLayoutBase.generated.h"


// forward declarations

class UBranchBase;
class ULoadBase;
class UStoreBase;
class UJumpBase;
class UArithBase;
class ARTSPlayerPawnST;
class UTestingBase;
class ASpectatorPawnST;
class UCodeBase;
class UCodeLinesDataEntry;

/**
 * for a layout for testing all kinds of stuff
 */
UCLASS()
class SYMBOLICTRACER_API UTestingLayoutBase : public UWidgetSTBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// for showing clicked runs and indices etc.
	// UPROPERTY(BlueprintReadOnly, Category = "Clicked Indices", meta = (BindWidget))
	// TObjectPtr<UTestingBase> Testing = nullptr;

	// for showing the code
	UPROPERTY(BlueprintReadOnly, Category = "Clicked Indices", meta = (BindWidget))
	TObjectPtr<UCodeBase> CodeInfo = nullptr;

	// for showing the arithmeticInstructionInfo
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UArithBase> ArithInstrInfo = nullptr;

	// for showing the jumpInstructionInfo
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UJumpBase> JumpInstrInfo = nullptr;

	// for showing the storeInstructionInfo
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UStoreBase> StoreInstrInfo = nullptr;

	// for showing the loadInstructionInfo
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<ULoadBase> LoadInstrInfo = nullptr;

	// for showing the branchInstructionInfo
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UBranchBase> BranchInstrInfo = nullptr;

protected:

private:
	// reference to the player character
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;


	// ---METHODS---

public:
	void Init(ARTSPlayerPawnST* PlayerChar);

	// Function that can be called (e.g. by player) to update shown information
	UFUNCTION(BlueprintCallable)
	void OnRunInstrStepIndexChange(int NewRunInd, int NewInstrInd, int NewStepInd, int NewScrollStepId);

	UFUNCTION(BlueprintCallable)
	void OnCodeFileLineIndexChange(int NewCodeFileInd, int NewCodeLineInd);

	UFUNCTION(BlueprintCallable)
	void OnRunCodeBlockIndexChange(int NewRunInd, int NewCodeBlockInd);
	

protected:

private:
};
