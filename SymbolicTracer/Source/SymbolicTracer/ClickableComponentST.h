// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClickableComponentST.generated.h"

/*
 * this component makes the actor clickable (hover changes outline)
 * at this stage all the logic is in the blueprint derived from this class
 */
UCLASS(ClassGroup=(Custom), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class SYMBOLICTRACER_API UClickableComponentST : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UClickableComponentST();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
