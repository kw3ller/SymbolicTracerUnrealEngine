// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerControllerST.h"

#include "CustomLogging.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HudST.h"
#include "RTSPlayerPawnST.h"

void ARTSPlayerControllerST::HandleForward(const FInputActionValue& InputActionValue)
{
	const float MovementValue  = InputActionValue.Get<float>();
	
	if(PlayerPawn)
		PlayerPawn->Forward(MovementValue);
}

void ARTSPlayerControllerST::HandleRight(const FInputActionValue& InputActionValue)
{
	const float MovementValue  = InputActionValue.Get<float>();
	
	if(PlayerPawn)
		PlayerPawn->Right(MovementValue);
}

void ARTSPlayerControllerST::HandleZoom(const FInputActionValue& InputActionValue)
{
	const float ZoomValue  = InputActionValue.Get<float>();
	
	if(PlayerPawn)
		PlayerPawn->Zoom(ZoomValue);
}

void ARTSPlayerControllerST::HandleRotate(const FInputActionValue& InputActionValue)
{
	const bool Rotate = InputActionValue.Get<bool>();
	
	if(PlayerPawn) {
		if(Rotate)
		{
			PlayerPawn->EnableRotate();
		}else {
			PlayerPawn->DisableRotate();
		}
	} 
}


void ARTSPlayerControllerST::HandleRotateRight()
{
	if(PlayerPawn)
		PlayerPawn->RotateRight();
}

void ARTSPlayerControllerST::HandleRotateLeft()
{
	if(PlayerPawn)
		PlayerPawn->RotateLeft();
}

void ARTSPlayerControllerST::HandleRotateHorizontal(const FInputActionValue& InputActionValue)
{
	const float RotateValue  = InputActionValue.Get<float>();
	
	if(PlayerPawn)
		PlayerPawn->RotateHorizontal(RotateValue);
}

void ARTSPlayerControllerST::HandleRotateVertical(const FInputActionValue& InputActionValue)
{
	const float RotateValue  = InputActionValue.Get<float>();
	
	if(PlayerPawn)
		PlayerPawn->RotateVertical(RotateValue);
}

void ARTSPlayerControllerST::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// store reference to the rtsPlayerPawn
	PlayerPawn = Cast<ARTSPlayerPawnST>(InPawn);
	// check if Spectator was set, else crash
	checkf(PlayerPawn, TEXT("ARTSPlayerControllerST derived classes should only posess ARTSPlayerpawn derived pawns."));

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
	if (ActionForward)
	{
		EnhancedInputComponent->BindAction(ActionForward, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleForward);
	}
	if (ActionRight)
	{
		EnhancedInputComponent->BindAction(ActionRight, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleRight);
	}
	if (ActionZoom)
	{
		EnhancedInputComponent->BindAction(ActionZoom, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleZoom);
	}
	if (ActionRotate)
	{
		EnhancedInputComponent->BindAction(ActionRotate, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleRotate);
	}
	if (ActionRotateRight)
	{
		EnhancedInputComponent->BindAction(ActionRotateRight, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleRotateRight);
	}
	if (ActionRotateLeft)
	{
		EnhancedInputComponent->BindAction(ActionRotateLeft, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleRotateLeft);
	}
	if (ActionRotateHorizontal)
	{
		EnhancedInputComponent->BindAction(ActionRotateHorizontal, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleRotateHorizontal);
	}
	if (ActionRotateVertical)
	{
		EnhancedInputComponent->BindAction(ActionRotateVertical, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleRotateVertical);
	}
	if (ActionLoadData)
	{
		EnhancedInputComponent->BindAction(ActionLoadData, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleLoadData);
	}
	if (ActionCycleUIMode)
	{
		EnhancedInputComponent->BindAction(ActionCycleUIMode, ETriggerEvent::Triggered, this, &ARTSPlayerControllerST::HandleCycleUIMode);
	}
}



void ARTSPlayerControllerST::OnUnPossess()
{
	// Unbind things here
	EnhancedInputComponent->ClearActionBindings();
	
	Super::OnUnPossess();
}

void ARTSPlayerControllerST::HandleLoadData()
{
	if (PlayerPawn)
	{
		const FString SelectedDataDir = PlayerPawn->SelectDataDir();

		// TODO: should probably check if the selected dir is valid... 
		
		PlayerPawn->LoadTraceInstrData(SelectedDataDir);
		PlayerPawn->LoadTraceBlkData(SelectedDataDir);
	}
}

void ARTSPlayerControllerST::HandleCycleUIMode()
{
	if (PlayerHud)
	{
		PlayerHud->CycleToNextViewMode();
	}
}
