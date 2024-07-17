// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ECodeLineState.h"
#include "CodeLinesDataEntry.generated.h"

// forward declarations

class ARTSPlayerPawnST;
class ASpectatorPawnST;


/**
 * just for passing data to the CodeLineWidget
 */
UCLASS(Blueprintable)
class SYMBOLICTRACER_API UCodeLinesDataEntry : public UObject
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// a reference to the playerCharacter (for clicking on a codeLine)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;

	// the content of the line (the code itself)
	UPROPERTY(BlueprintReadOnly)
	FString Content;

	// the text of the button in the codeLine
	UPROPERTY(BlueprintReadOnly)
	FString LineButtonText;

	// the text of the branchCount
	UPROPERTY(BlueprintReadOnly)
	int BranchCount;
	
	// the text of the branchCoverage
	UPROPERTY(BlueprintReadOnly)
	int BranchCov;

	// the text of the lineCoverage
	UPROPERTY(BlueprintReadOnly)
	int HitCount;

	// was this line symbolized ?!
	UPROPERTY(BlueprintReadOnly)
	bool Symbolized = false;

	// was this line concretized ?!
	UPROPERTY(BlueprintReadOnly)
	bool Concretized = false;

	// is this codeLine the beginning of a codeBlock
	UPROPERTY(BlueprintReadOnly)
	bool UpperClamp = false;

	// is this codeLine the beginning of a codeBlock
	UPROPERTY(BlueprintReadOnly)
	bool LowerClamp = false;

	// is this codeLine marked
	UPROPERTY(BlueprintReadOnly)
	bool Marked = false;

	// the pc of this codeLine
	UPROPERTY(BlueprintReadOnly)
	int32 Pc = -1;
	
	// did this codeLine get hit by symbolic run (is it part of a codeBlock)
	UPROPERTY(BlueprintReadOnly)
	ECodeLineState CodeLineState = ECodeLineState::None;

	// index of codeFile
	UPROPERTY(BlueprintReadOnly)
	int CodeFileIndex = -1;

	// index of codeLine
	UPROPERTY(BlueprintReadOnly)
	int CodeLineIndex = -1;
	
protected:

private:

	
	
	// ---METHODS---

public:

	
	void Init(ARTSPlayerPawnST* playerCharacter, int codeFileInd, int codeLineInd);

protected:

private:
};
