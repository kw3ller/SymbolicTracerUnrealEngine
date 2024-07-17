// Fill out your copyright notice in the Description page of Project Settings.


#include "InstructionLineMarkActorST.h"


// Sets default values
AInstructionLineMarkActorST::AInstructionLineMarkActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// TODO: check if needed
	// PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind marker mesh
	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	MarkerMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void AInstructionLineMarkActorST::Init(const FVector& Scale)
{
	MarkerMesh->SetRelativeScale3D(Scale);
}

// Called when the game starts or when spawned
void AInstructionLineMarkActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInstructionLineMarkActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

