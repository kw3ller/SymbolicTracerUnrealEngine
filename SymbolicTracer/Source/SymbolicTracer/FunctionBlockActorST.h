// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "FunctionBlockActorST.generated.h"

/*
 * this actor will symbolise a function of the code
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API AFunctionBlockActorST : public ASceneActorBase
{
	GENERATED_BODY()

	// ---VARIABLES---

public:

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// the normal mesh component to which that mesh and material will be added
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

	// the default material
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FunctionBlock Materials")
	TObjectPtr<UMaterial> Material1 = nullptr;

	// the default material
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FuntcionBlock Materials")
	TObjectPtr<UMaterial> Material2 = nullptr;

	// TODO: add clickable component -> so we jump to function inside
	// but for that codeLine data has to be added to the json trace data -> so we know where to jump to


protected:

private:

	// ---METHODS---

public:
	// Sets default values for this actor's properties
	AFunctionBlockActorST();

	void Init(const FVector& Scale, bool Material);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	


	
};
