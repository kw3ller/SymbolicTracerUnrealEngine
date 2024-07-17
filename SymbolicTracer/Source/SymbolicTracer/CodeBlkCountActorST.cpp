// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeBlkCountActorST.h"

#include "Components/TextRenderComponent.h"


// Sets default values
ACodeBlkCountActorST::ACodeBlkCountActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the textRenderComponents
	CountTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountTextRender"));
	CountTextRender->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACodeBlkCountActorST::Init(const int Count)
{
	CountTextRender->SetText(FText::FromString(FString::FromInt(Count)));
}

// Called when the game starts or when spawned
void ACodeBlkCountActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACodeBlkCountActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

