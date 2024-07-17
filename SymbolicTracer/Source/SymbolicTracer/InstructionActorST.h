// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "Math/Vector2D.h"
#include "DataStructs.h"
#include "EInstructionType.h"
#include "InstructionActorST.generated.h"


// forward declarations

enum class EBranchTypes : uint8;
enum class EAbstractionLevel : uint8;
class UTextRenderComponent;
class UClickableComponentST;


/*
 * this is an actor that we spawn in the scene that will symbolise the instructions of the symbolic run
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API AInstructionActorST : public ASceneActorBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:
	// index of the run this is part of (in runs)
	UPROPERTY(BlueprintReadOnly)
	int RunIndex = -1;

	// index of the instruction inside the run (in runs)
	UPROPERTY(BlueprintReadOnly)
	int InstructionIndex = -1;

	// index of the step inside the instruction inside the run (in runs)
	UPROPERTY(BlueprintReadOnly)
	int StepIndex = -1;

	// id of the step inside the instruction inside the run (in runs)
	UPROPERTY(BlueprintReadOnly)
	int StepId = -1;

	// the instructionType of this actor
	UPROPERTY()
	EInstructionType InstructionType = EInstructionType::Default;

	// position of this actor -> TODO: can probably be deleted
	UPROPERTY(BlueprintReadOnly)
	FVector Position;

	// hide in game -> for different abstraction levels
	UPROPERTY(BlueprintReadOnly)
	bool HideInGame = true;

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// the normal mesh component to which that mesh and material will be added
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

	// the default mesh for when instructionType is Default
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> DefaultMesh = nullptr;

	// the mesh for when instructionType is Branch (with true and false)
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> BranchMeshBoth = nullptr;

	// the mesh for when instructionType is Branch (with true)
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> BranchMeshTrue = nullptr;

	// the mesh for when instructionType is Branch (with false)
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> BranchMeshFalse = nullptr;

	// the mesh for when instructionType is Jump
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> JumpMesh = nullptr;

	// the mesh for when instructionType is an ecall
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> ECallMesh = nullptr;
	
	// the mesh for when instructionType is Arithmetic
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> ArithmeticMesh = nullptr;

	// the mesh for when instructionType is Load or Store
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Instruction Meshes")
	TObjectPtr<UStaticMesh> LoadStoreMesh = nullptr;

	// the clickable component (instance will be se in Blueprint)
	// TODO: add crash when not set?!
	// -> gets set in blueprint construction script!!!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UClickableComponentST> ClickableComponent = nullptr;

protected:


private:
	// ---METHODS---

public:
	// Sets default values for this actor's properties
	AInstructionActorST();

	// to initialize this instruction
	void Init(const EInstructionType& InstrType, UMaterialInterface* SymBehMaterial, const FVector& Pos, const FVector& Scale, int MyRunIndex, int MyInstrIndex, int MyStepIndex, int MyStepId, const EBranchTypes& BranchType);

	// change material of meshComponent
	void ChangeMaterial(UMaterialInterface* Material);
	
	// hides or shows this actor this act
	void Hide(bool hide);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
};
