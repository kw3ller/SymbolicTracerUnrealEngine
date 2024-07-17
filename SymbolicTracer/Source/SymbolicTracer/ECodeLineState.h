#pragma once


/*
 * enum for holding different states a codeLine can be in
 */
UENUM(BlueprintType)
enum class ECodeLineState : uint8
{
	None UMETA(Tooltip="None"),
	Compiled UMETA(Tooltip="Compiled"),
	Hit UMETA(Tooltip="Hit"),
};