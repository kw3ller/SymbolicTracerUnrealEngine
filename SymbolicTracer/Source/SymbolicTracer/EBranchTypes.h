#pragma once


/*
 * enum for holding different types of branches
 */
UENUM(BlueprintType, Blueprintable)
enum class EBranchTypes : uint8
{
	BranchNone UMETA(Tooltip="BranchNone"),
	BranchBoth UMETA(Tooltip="BranchBoth"),
	BranchTrue UMETA(Tooltip="BranchTrue"),
	BranchFalse UMETA(Tooltip="BranchFalse"),
};