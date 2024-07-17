// Fill out your copyright notice in the Description page of Project Settings.


#include "PathTextActorST.h"

#include "Components/TextRenderComponent.h"


// Sets default values
APathTextActorST::APathTextActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the textRenderComponent
	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	TextRender->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void APathTextActorST::Init(const int PathInd)
{
	RunIndex = PathInd;
	FString PathText = "Path " + FString::FromInt(RunIndex);
	TextRender->SetText(FText::FromString(PathText));
}

// Called when the game starts or when spawned
void APathTextActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathTextActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

