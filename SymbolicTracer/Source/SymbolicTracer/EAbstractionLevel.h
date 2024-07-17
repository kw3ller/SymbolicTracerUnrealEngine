#pragma once


/*
 * enum for holding different abstraction levels in which the trace data is shown in
 * Level0: no trace loaded
 * Level1: show basic stuff (e.g. codeBlocks)
 * Level2: show more stuff (e.g. Instructions in 2D)
 * Level3: show more stuff (e.g. using 3rd dimension for instructions)
 * Level4: show more stuff (e.g. animations)
 */
UENUM(BlueprintType, Blueprintable)
enum class EAbstractionLevel : uint8
{
	Level0 UMETA(Tooltip="Level0"),
	Level1 UMETA(Tooltip="Level1"),
	Level2 UMETA(Tooltip="Level2"),
	Level3 UMETA(Tooltip="Level3"),
	Level4 UMETA(Tooltip="Level4"),
};