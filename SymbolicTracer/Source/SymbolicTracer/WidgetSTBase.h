// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetSTBase.generated.h"

/**
 * this is a base widget from which our minimal and other layout bases will inherit
 * *BaseClass for a group of Widgets
 * pretty much *just* to group our widgets in a CustomPaletteCategory
 */
UCLASS()
class SYMBOLICTRACER_API UWidgetSTBase : public UUserWidget
{
	GENERATED_BODY()

	// ---Variables

public:

protected:

private:
	// ---METHODS---

public:
	// Because otherwise we get error when packing the game -> because this is only "used" in th editor
#if WITH_EDITOR
	// We create our own Widget Category "GGameDev", which can be seen when creating a UI Widget under "palette"
	virtual const FText GetPaletteCategory() override;
#endif

protected:

private:
};
