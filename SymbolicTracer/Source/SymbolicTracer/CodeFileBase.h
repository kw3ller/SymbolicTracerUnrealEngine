// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetSTBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CodeFileBase.generated.h"


// forward declarations

class UButton;
class UTextBlock;


/**
 * the widget base for a codeFileEntry (List of codeFiles to click and see)
 */
UCLASS()
class SYMBOLICTRACER_API UCodeFileBase : public UWidgetSTBase, public IUserObjectListEntry
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// shows the codeFileName
	UPROPERTY(BlueprintReadOnly, Category="Code Files", meta=(BindWidget))
	TObjectPtr<UTextBlock> Content = nullptr;

	// shows the coverage percentage of this codeFile
	UPROPERTY(BlueprintReadOnly, Category="Code Files", meta=(BindWidget))
	TObjectPtr<UTextBlock> CovText = nullptr;

	// to change active codeLines on CLick
	// TODO: atm behaviour done in Blueprint (with dynamic casting to spectatorPawnST) -> better would be code with reference
	UPROPERTY(BlueprintReadOnly, Category="Code Lines", meta=(BindWidget))
	TObjectPtr<UButton> FileButton = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int FileIndex = -1;

protected:

private:
	// ---METHODS---

public:
	

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
};
