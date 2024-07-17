// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerST.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CustomLogging.h"
#include "HudST.h"
#include "SpectatorPawnST.h"


void APlayerControllerST::HandleLoadData()
{
	// TODO: should handle loading data (maybe filePicker or give path through UI or stuff) 
	UE_LOG(STLog, Warning, TEXT("Loading Data pressed"));
	if (PlayerPawn)
	{
		PlayerPawn->LoadTraceInstrData();
		PlayerPawn->LoadTraceBlkData();
	}
}

void APlayerControllerST::HandleCycleUIMode()
{
	if (PlayerHud)
	{
		PlayerHud->CycleToNextViewMode();
	}
}

void APlayerControllerST::HandleClickableUI()
{
	UE_LOG(STLog, Warning, TEXT("HandleClickableUI was pressed"));
	if (PlayerHud)
	{
		PlayerHud->ToggleInteractionMode();
	}
}


void APlayerControllerST::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// store a reference to the spectator Pawn
	PlayerPawn = Cast<ASpectatorPawnST>(aPawn);
	// check if Spectator was set, else crash
	checkf(PlayerPawn, TEXT("APlayerControllerST derived classes should only posess ASpectatorPawnST derived pawns."));

	// Get a reference to the Hud
	PlayerHud = Cast<AHudST>(GetHUD());
	checkf(PlayerHud, TEXT("Unable to get the HUD."));

	// get reference to enhancedInputComponent
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent."));

	// Get the local player subSystem
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem."));

	// Wipe existing mappings and add our mapping
	checkf(InputMappingContent, TEXT("InputMappingContent was not specified."));
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContent, 0);

	// Bind all the input actions -> only attempt to bind if valid value was provided
	if (ActionLoadData)
	{
		EnhancedInputComponent->BindAction(ActionLoadData, ETriggerEvent::Triggered, this,
		                                   &APlayerControllerST::HandleLoadData);
	}
	if (ActionCycleUIMode)
	{
		EnhancedInputComponent->BindAction(ActionCycleUIMode, ETriggerEvent::Triggered, this,
		                                   &APlayerControllerST::HandleCycleUIMode);
	}
	if (ActionClickableUI)
	{
		EnhancedInputComponent->BindAction(ActionClickableUI, ETriggerEvent::Triggered, this,
		                                   &APlayerControllerST::HandleClickableUI);
	}
}

void APlayerControllerST::OnUnPossess()
{
	// Unbind things here
	EnhancedInputComponent->ClearActionBindings();

	// call the parent method
	Super::OnUnPossess();
}
