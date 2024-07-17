// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorBase.h"
#include "FunctionTextActorST.generated.h"
class UTextRenderComponent;
/*
 * this actor that we spawn in the scene 
 */
UCLASS(Abstract)
class SYMBOLICTRACER_API AFunctionTextActorST : public ASceneActorBase
{
	GENERATED_BODY()


	// ---VARIABLES---

	public:

	// the root component of this actor
	UPROPERTY()
	TObjectPtr<USceneComponent> Root = nullptr;

	// shall show the functionName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Function Text")
	TObjectPtr<UTextRenderComponent> TextRender = nullptr;

	// the default material
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FunctionText Color")
	FColor Color1;

	// the default material
	// TODO: must be set else crash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FuntcionText Color")
	FColor Color2;

protected:

private:

	// ---METHODS---

	public:
	// Sets default values for this actor's properties
	AFunctionTextActorST();

	void Init(const FString& FunctionName, bool Color);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

};
