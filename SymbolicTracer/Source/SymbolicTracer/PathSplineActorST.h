// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "Components/SplineMeshComponent.h"
#include "PathSplineActorST.generated.h"

class USplineComponent;
/*
 * this spline will represent the control flow between paths (e.g. an arrow between a parent path and a child path) 
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API APathSplineActorST : public ASceneActorBase
{
	GENERATED_BODY()

	// ---Variables---


public:

	// hide in game -> for different abstraction levels
	UPROPERTY(BlueprintReadOnly)
	bool HideInGame = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default Material")
	TObjectPtr<UMaterial> DefaultMaterial = nullptr;

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Spline")
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
	TObjectPtr<UStaticMesh> DefaultMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

protected:

private:

	// the meshComponents of the spline we use to hide/show in scene
	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshComponents;

	// ---METHODS---

public:

	// Sets default values for this actor's properties
	APathSplineActorST();

	void Init(const FVector& Start, const FVector& End, const FVector& Scale);

	// hides or shows this actor
	void Hide(bool hide);

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	
};
