// Fill out your copyright notice in the Description page of Project Settings.


#include "InstructionTextActorST.h"

#include "CustomLogging.h"
#include "Components/TextRenderComponent.h"


// Sets default values
AInstructionTextActorST::AInstructionTextActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	// bind the textRenderComponent
	OpCodeTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OpCodeTextRender"));
	OpCodeTextRender->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void AInstructionTextActorST::Init(const FString& OpCodeText)
{

	if(OpCodeText == "")
	{
		OpCodeTextRender->DestroyComponent();
	} else
	{
		OpCodeTextRender->SetText(FText::FromString(OpCodeText));
	}

	UE_LOG(STLog, Display, TEXT("Testing init of InstructionTextActor: %s"), *OpCodeText);
}

void AInstructionTextActorST::Hide(bool hide)
{
	HideInGame = hide;

	if(HideInGame)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	} else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
}

// Called when the game starts or when spawned
void AInstructionTextActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInstructionTextActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

