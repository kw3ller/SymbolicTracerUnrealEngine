// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeBlkTextActorST.h"

#include "Components/TextRenderComponent.h"


// Sets default values
ACodeBlkTextActorST::ACodeBlkTextActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the textRenderComponents
	FileTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("FileTextRender"));
	FileTextRender->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	LineTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LineTextRender"));
	LineTextRender->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACodeBlkTextActorST::Init(const FString& FileName, const int BlockStart, const int BlockEnd)
{
	FileTextRender->SetText(FText::FromString(FileName));
	const FString LineText = FString::FromInt(BlockStart) + " - " + FString::FromInt(BlockEnd);
	LineTextRender->SetText(FText::FromString(LineText));
}

// Called when the game starts or when spawned
void ACodeBlkTextActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACodeBlkTextActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

