// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/HUD.h"
#include "HudST.generated.h"

// forward declaration

class ARTSPlayerPawnST;
class UTestingLayoutBase;
class UMinimalLayoutBase;
class UModerateLayoutBase;
class UDetailedLayoutBase;
class ASpectatorPawnST;

/*
 * Enum for various viewMode types
 * The hud should show different levels of detail 
 */
UENUM(BlueprintType)
enum class EHudViewMode : uint8
{
	Clean UMETA(Tooltip="Show nothing"),
	Minimal UMETA(Tooltip="Show minimal information"),
	Moderate UMETA(Tooltip="Show moderate information"),
	Detailed UMETA(Tooltip="Show detailed information"),
	Testing UMETA(Tooltip="Show testing information"),
};

// simple operator overload to cycle the viewMode
inline EHudViewMode& operator++(EHudViewMode& ViewMode)
{
	switch (ViewMode)
	{
	case EHudViewMode::Clean:
		ViewMode = EHudViewMode::Minimal;
		break;
	case EHudViewMode::Minimal:
		ViewMode = EHudViewMode::Moderate;
		break;
	case EHudViewMode::Moderate:
		ViewMode = EHudViewMode::Detailed;
		break;
	case EHudViewMode::Detailed:
		ViewMode = EHudViewMode::Testing;
		break;
	case EHudViewMode::Testing:
		ViewMode = EHudViewMode::Clean;
		break;
	}
	return ViewMode;
}

// simple operator overload to cycle the viewMode
inline EHudViewMode& operator--(EHudViewMode& ViewMode)
{
	switch (ViewMode)
	{
	case EHudViewMode::Clean:
		ViewMode = EHudViewMode::Testing;
		break;
	case EHudViewMode::Minimal:
		ViewMode = EHudViewMode::Clean;
		break;
	case EHudViewMode::Moderate:
		ViewMode = EHudViewMode::Minimal;
		break;
	case EHudViewMode::Detailed:
		ViewMode = EHudViewMode::Moderate;
		break;
	case EHudViewMode::Testing:
		ViewMode = EHudViewMode::Detailed;
		break;
	}

	return ViewMode;
}

/**
 * this is the hud with different levels
 */
UCLASS()
class SYMBOLICTRACER_API AHudST : public AHUD
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// the minimalLayout
	// TSubclassOf means in editor we get drop dwn menu that gets filled with subclasses of that class
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMinimalLayoutBase> MinimalLayoutClass = nullptr;

	// the moderateLayout
	UPROPERTY(EditAnywhere)
	TSubclassOf<UModerateLayoutBase> ModerateLayoutClass = nullptr;

	// the detailedLayout
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDetailedLayoutBase> DetailedLayoutClass = nullptr;

	// the testingLayout
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTestingLayoutBase> TestingLayoutClass = nullptr;

protected:

private:
	// determine what UI levelOfDetail should be shown 
	UPROPERTY(EditAnywhere)
	EHudViewMode CurrentViewMode = EHudViewMode::Minimal;

	// determine if UI is interactable or if World is interactable (SelfHitTestInvisible -> not interactable, Visible -> interactable)
	UPROPERTY(EditAnywhere)
	ESlateVisibility CurrentInteractionMode = ESlateVisibility::SelfHitTestInvisible;

	// these will be made instances when the hud runs (in beginPlay)
	// store reference to minimal layout 
	UPROPERTY()
	TObjectPtr<UMinimalLayoutBase> MinimalLayoutWidget = nullptr;

	// store reference to moderate layout 
	UPROPERTY()
	TObjectPtr<UModerateLayoutBase> ModerateLayoutWidget = nullptr;

	// store reference to detailed layout 
	UPROPERTY()
	TObjectPtr<UDetailedLayoutBase> DetailedLayoutWidget = nullptr;

	// store reference to testing layout 
	UPROPERTY()
	TObjectPtr<UTestingLayoutBase> TestingLayoutWidget = nullptr;

	// reference to the world we play in
	UPROPERTY()
	TObjectPtr<UWorld> World = nullptr;

	// reference to the player character
	UPROPERTY()
	TObjectPtr<ARTSPlayerPawnST> PlayerCharacter = nullptr;

	// ---METHODS---

public:
	// gets called to change the @CurrentViewMode
	UFUNCTION(BlueprintCallable)
	void CycleToNextViewMode();

	// gets called to toggle the @CurrentInteractionMode
	UFUNCTION(BlueprintCallable)
	void ToggleInteractionMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// whenever we change the view mode, this private function is called to show the appropriate widgets.
	void UpdateWidgets();

	// Release any delegate bindings.
	void ClearAllHandlers();
};
