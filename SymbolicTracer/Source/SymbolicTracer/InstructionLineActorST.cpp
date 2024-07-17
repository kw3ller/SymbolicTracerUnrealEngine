// Fill out your copyright notice in the Description page of Project Settings.


#include "InstructionLineActorST.h"

#include "EAbstractionLevel.h"
#include "Components/TextRenderComponent.h"


// Sets default values
AInstructionLineActorST::AInstructionLineActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the textRenderComponent
	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	TextRender->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	// bind the lines
	MiddleLine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiddleLine"));
	MiddleLine->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	UpperLine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpperLine"));
	UpperLine->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	LowerLine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LowerLine"));
	LowerLine->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

}

void AInstructionLineActorST::Init(const FString& ProgramCounterHex, const FVector& ScaleMiddle, const FVector& ScaleUpperLower)
{
	
	TextRender->SetText(FText::FromString(ProgramCounterHex));

	MiddleLine->SetRelativeScale3D(ScaleMiddle);
	UpperLine->SetRelativeScale3D(ScaleUpperLower);
	LowerLine->SetRelativeScale3D(ScaleUpperLower);

}

void AInstructionLineActorST::Hide(bool Hide)
{
	HideInGame = Hide;
	
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
void AInstructionLineActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInstructionLineActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

