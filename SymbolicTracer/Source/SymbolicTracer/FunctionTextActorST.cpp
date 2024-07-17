// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionTextActorST.h"

#include "Components/TextRenderComponent.h"


// Sets default values
AFunctionTextActorST::AFunctionTextActorST()
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

void AFunctionTextActorST::Init(const FString& FunctionName, bool Color)
{
	if(Color)
	{
		TextRender->SetTextRenderColor(Color1);
		
	} else
	{
		TextRender->SetTextRenderColor(Color2);
	}
	TextRender->SetText(FText::FromString(FunctionName));
}

// Called when the game starts or when spawned
void AFunctionTextActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFunctionTextActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
