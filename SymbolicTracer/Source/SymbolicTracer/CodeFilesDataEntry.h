// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CodeFilesDataEntry.generated.h"

/**
 * just for passing data to the CodeFileWidget
 */
UCLASS(Blueprintable)
class SYMBOLICTRACER_API UCodeFilesDataEntry : public UObject
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// shows the name of the codeFile
	UPROPERTY(BlueprintReadOnly)
	FString Content = "";

	// shows the coverage percentage of the codeFile
	UPROPERTY(BlueprintReadOnly)
	int CodeCovPercentage = 0;

	UPROPERTY(BlueprintReadOnly)
	int FileIndex = -1;

protected:

private:
	// ---METHODS---

public:
	void Init(FString FileName, int CodeCovPerc, int FileIndint);

protected:

private:
};
