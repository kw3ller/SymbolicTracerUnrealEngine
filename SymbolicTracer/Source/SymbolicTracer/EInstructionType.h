#pragma once


/*
 * enum for various instruction types (e.g. jump etc.)
 * the instructions will represent different kinds of instructions (e.g. via different meshes)
 * TODO: add the missing ones
 */
UENUM(BlueprintType)
enum class EInstructionType : uint8
{
	Default UMETA(Tooltip="No instruction type."),
	Jump UMETA(Tooltip="Jump instruction type."),
	Arith UMETA(Tooltip="Arithmetic instruction type."),
	Store UMETA(Tooltip="Store instruction type."),
	Load UMETA(Tooltip="Load instruction type."),
	Branch UMETA(Tooltip="Branch instruction type."),
	ECALL UMETA(Tooltip="ECall instruction type."),
};