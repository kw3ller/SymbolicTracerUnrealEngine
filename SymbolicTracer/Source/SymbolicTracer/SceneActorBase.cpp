// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneActorBase.h"


// Sets default values
ASceneActorBase::ASceneActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASceneActorBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASceneActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
