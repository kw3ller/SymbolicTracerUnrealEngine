// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "Components/SplineMeshComponent.h"
#include "InstructionSplineActorST.generated.h"

enum class EBranchTypes : uint8;
class USplineComponent;
/*
 * this spline will represent the control flow (e.g. between jumps and branches)
 */ 
UCLASS(Abstract)
class SYMBOLICTRACER_API AInstructionSplineActorST : public ASceneActorBase
{


private:
	GENERATED_BODY()

	// ---VARIABLES

public:

	// hide in game -> for different abstraction levels
	UPROPERTY(BlueprintReadOnly)
	bool HideInGame = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default Material")
	TObjectPtr<UMaterial> DefaultMaterial = nullptr;

	// for jumps
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Normal Material")
	TObjectPtr<UMaterial> NormalMaterial = nullptr;

	// for branches
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Branch True Material")
	TObjectPtr<UMaterial> BranchTrueMaterial = nullptr;

	// for branches
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Branch False Material")
	TObjectPtr<UMaterial> BranchFalseMaterial = nullptr;
	
	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Spline")
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	
	
	
protected:

private:

	// the meshComponents of the spline we use to hide/show in scene
	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshComponents;

private:

	// ---METHODS---

public:

	// Sets default values for this actor's properties
	AInstructionSplineActorST();

	// ExecutionNr -> number of times this spline got executed -> for thickness
	void Init(const FVector& Start, const FVector& Middle, const FVector& End, int ExecutionNr, const EBranchTypes& BranchType);

	// hides or shows this actor
	void Hide(bool hide);

	// change material of this spline
	void SetMaterial(UMaterial* Material);

	// change material of this spline to defaultMaterial
	void SetDefaultMaterial();

	virtual void OnConstruction(const FTransform& Transform) override;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:


};
