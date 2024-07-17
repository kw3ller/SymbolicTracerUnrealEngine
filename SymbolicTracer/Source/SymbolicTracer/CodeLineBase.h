// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CodeLineBase.generated.h"

// forward declaration

class UButton;
class UTextBlock;
class UImage;
class UBorder;


/**
 * Widget base for displaying individual CodeLines (e.g. inside CodeBase)
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API UCodeLineBase : public UWidgetSTBase, public IUserObjectListEntry
{
	GENERATED_BODY()


	// ---VARIABLES---

public:
	// shows the content of the codeLines (the Code)
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UTextBlock> Content = nullptr;

	// the background of the content -> colored to show which lines got hit
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UBorder> ContentBackground = nullptr;

	// shall be the line Button
	// TODO: atm behaviour in Blueprints -> better would be in code
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UButton> LineButton = nullptr;

	// shows the codeLine
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UTextBlock> LineButtonText = nullptr;

	// resembles the upper clamp of a CodeBlock shown in code
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UImage> UpperClamp = nullptr;

	// resembles the lower clamp of a CodeBlock shown in code
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UImage> LowerClamp = nullptr;

	// resembles the marking 
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UBorder> MarkBorder = nullptr;

	// the line coverage text
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UTextBlock> HitCountText = nullptr;

	// background of the line coverage
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UBorder> HitCountBackground = nullptr;
	
	// the branch count text
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UTextBlock> BranchCountText = nullptr;
	
	// the branch coverage text
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UTextBlock> BranchCovText = nullptr;

	// background of the branchCount
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UBorder> BranchCountBackground = nullptr;

	// background of the branchCoverage
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UBorder> BranchCovBackground = nullptr;

	// the symbolized/conretized text
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UTextBlock> SymConText = nullptr;

	// the symbolized/conretized text background
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UBorder> SymConBackground = nullptr;
	

	// the index of this codeFile
	UPROPERTY(BlueprintReadOnly)
	int CodeFileIndex = -1;
	
	// the index of this codeLine
	UPROPERTY(BlueprintReadOnly)
	int CodeLineIndex = -1;

	// the pc of this codeLine
	UPROPERTY(BlueprintReadOnly)
	int32 Pc = -1;

protected:

private:

	// ---METHODS---

public:
	 

protected:

	// gets called when initializing (e.g. set content to show)
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
};
