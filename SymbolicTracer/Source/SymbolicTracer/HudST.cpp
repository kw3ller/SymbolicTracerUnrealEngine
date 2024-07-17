// Fill out your copyright notice in the Description page of Project Settings.


#include "HudST.h"

#include "CodeBase.h"
#include "CustomLogging.h"
#include "DetailedLayoutBase.h"
#include "GeneralAnalysisBase.h"
#include "MinimalLayoutBase.h"
#include "ModerateLayoutBase.h"
#include "PathAnalysisBase.h"
#include "RTSPlayerPawnST.h"
#include "TestingLayoutBase.h"
#include "TestingBase.h"


void AHudST::CycleToNextViewMode()
{
	// ++CurrentViewMode;

	// -> since 2 viewModes are not currently used (Moderate and Detailed) we skip them here -> not pretty, but quick solution
	if(CurrentViewMode == EHudViewMode::Clean)
	{
		CurrentViewMode = EHudViewMode::Minimal;
	} else if(CurrentViewMode == EHudViewMode::Minimal)
	{
		CurrentViewMode = EHudViewMode::Testing;
	} else if (CurrentViewMode == EHudViewMode::Testing)
	{
		CurrentViewMode = EHudViewMode::Clean;
	}

	
	UE_LOG(STLog, Warning, TEXT("CycleToNextViewMode %s"), *UEnum::GetValueAsString(CurrentViewMode));
	UpdateWidgets();
}

void AHudST::ToggleInteractionMode()
{
	switch (CurrentInteractionMode)
	{
	case ESlateVisibility::Visible:
		CurrentInteractionMode = ESlateVisibility::SelfHitTestInvisible;
		break;
	case ESlateVisibility::SelfHitTestInvisible:
		CurrentInteractionMode = ESlateVisibility::Visible;
		break;
	default:
		CurrentInteractionMode = ESlateVisibility::SelfHitTestInvisible;
		break;
	}

	UpdateWidgets();
}

void AHudST::BeginPlay()
{
	Super::BeginPlay();

	// Grab a reference to the current world
	World = GetWorld();
	checkf(World, TEXT("Failed to reference world."));


	// Get a reference to the character, and hook up the stat handlers
	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		PlayerCharacter = Cast<ARTSPlayerPawnST>(PlayerController->GetPawn());
	}
	checkf(PlayerCharacter, TEXT("Unable to get a reference to the player character"));


	// Ensure we have valid values for the 3 classes of widget used by the HUD
	checkf(MinimalLayoutClass, TEXT("Invalid MinimalLayoutClass reference."));
	checkf(ModerateLayoutClass, TEXT("Invalid ModerateLayoutClass reference."));
	checkf(DetailedLayoutClass, TEXT("Invalid DetailedLayoutClass reference."));
	checkf(TestingLayoutClass, TEXT("Invalid TestingLayoutClass reference."));


	// create the 3 types of layout widget, and add them to the viewport
	// We could have been 'clever' here, and had maybe a single widget which 'mutates'
	// based on the requirements, but this IS a tutorial after all, and we wanna keep it simple(er!)
	// When creating a widget, the first parameter (owning object) must be one of the following types:
	// UWidget, UWidgetTree, APlayerController, UGameInstance, or UWorld
	MinimalLayoutWidget = CreateWidget<UMinimalLayoutBase>(World, MinimalLayoutClass);
	MinimalLayoutWidget->AddToViewport();
	MinimalLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);
	MinimalLayoutWidget->Init(PlayerCharacter);

	ModerateLayoutWidget = CreateWidget<UModerateLayoutBase>(World, ModerateLayoutClass);
	ModerateLayoutWidget->AddToViewport();
	ModerateLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);

	DetailedLayoutWidget = CreateWidget<UDetailedLayoutBase>(World, DetailedLayoutClass);
	DetailedLayoutWidget->AddToViewport();
	DetailedLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);

	TestingLayoutWidget = CreateWidget<UTestingLayoutBase>(World, TestingLayoutClass);
	TestingLayoutWidget->AddToViewport();
	TestingLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);
	TestingLayoutWidget->Init(PlayerCharacter);


	UpdateWidgets();
}


void AHudST::UpdateWidgets()
{
	// Unhook any delegate handlers.
	ClearAllHandlers();

	// Set all the widgets so we see none of them
	MinimalLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);
	ModerateLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);
	DetailedLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);
	TestingLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);

	switch (CurrentViewMode)
	{
	case EHudViewMode::Clean:
		// Currently there isn't actually anything to do here!
		break;
	case EHudViewMode::Minimal:
		MinimalLayoutWidget->SetVisibility(CurrentInteractionMode);
		PlayerCharacter->OnRunIndexChanged.AddUniqueDynamic(MinimalLayoutWidget->PathAnalysis,
		                                                    &UPathAnalysisBase::OnRunIndexChange);
		PlayerCharacter->OnRunIndexChanged.AddUniqueDynamic(MinimalLayoutWidget->GeneralAnalysis,
		                                                    &UGeneralAnalysisBase::OnRunIndexChange);
		break;
	case EHudViewMode::Moderate:
		ModerateLayoutWidget->SetVisibility(CurrentInteractionMode);
		break;
	case EHudViewMode::Detailed:
		DetailedLayoutWidget->SetVisibility(CurrentInteractionMode);
		break;
	case EHudViewMode::Testing:
		TestingLayoutWidget->SetVisibility(CurrentInteractionMode);
	// bind the delegates for this layout
		PlayerCharacter->OnRunInstrStepIndexChanged.AddUniqueDynamic(TestingLayoutWidget,
																 &UTestingLayoutBase::OnRunInstrStepIndexChange);
	// changing run or instruction index // TODO: can be removed in future
		//PlayerCharacter->OnRunInstrStepIndexChanged.AddUniqueDynamic(TestingLayoutWidget->Testing,
		//                                                         &UTestingBase::OnRunInstrIndexChange);
	// changing instruction or codeFile	(when clicking on instructionActor in Scene
		//PlayerCharacter->OnRunInstrStepIndexChanged.AddUniqueDynamic(TestingLayoutWidget->CodeInfo,
		//                                                         &UCodeBase::OnRunInstrIndexChange);
	// changing codeFile or codeLine
		PlayerCharacter->OnCodeFileLineIndexChanged.AddUniqueDynamic(TestingLayoutWidget,
																 &UTestingLayoutBase::OnCodeFileLineIndexChange);
		//PlayerCharacter->OnCodeFileLineIndexChanged.AddUniqueDynamic(TestingLayoutWidget->CodeInfo,
		//                                                             &UCodeBase::OnCodeFileLineIndexChange);
	// changing Run or codeBlock (from blk.json)
		PlayerCharacter->OnRunCodeBlockIndexChanged.AddUniqueDynamic(TestingLayoutWidget,
																 &UTestingLayoutBase::OnRunCodeBlockIndexChange);
		//PlayerCharacter->OnRunCodeBlockIndexChanged.AddUniqueDynamic(TestingLayoutWidget->CodeInfo,
		//                                                             &UCodeBase::OnRunCodeBlockIndexChange);
		break;
	default: ;
	}

	// This ensures that even if something has not changed recently, the newly switched-to widget will get sent
	// the latest character stats, so it can update itself.
	PlayerCharacter->BroadcastCurrentStats();
}

void AHudST::ClearAllHandlers()
{
	// clear all the delegates of the character
	if (PlayerCharacter)
	{
		PlayerCharacter->OnRunInstrStepIndexChanged.Clear();
		PlayerCharacter->OnCodeFileLineIndexChanged.Clear();
		PlayerCharacter->OnRunCodeBlockIndexChanged.Clear();
		PlayerCharacter->OnRunIndexChanged.Clear();
	}
}


void AHudST::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// NOTE: Release any event handlers if any exist


	Super::EndPlay(EndPlayReason);
}
