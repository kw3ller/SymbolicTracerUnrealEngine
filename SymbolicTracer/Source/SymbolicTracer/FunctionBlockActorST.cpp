// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionBlockActorST.h"


// Sets default values
AFunctionBlockActorST::AFunctionBlockActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the meshComponent
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultMeshComponent"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void AFunctionBlockActorST::Init(const FVector& Scale, bool Material)
{
	// NOTE: set mesh inside the bluePrint
	
	if(Material)
	{
		MeshComponent->SetMaterial(0, Material1);
		
	} else
	{
		MeshComponent->SetMaterial(0, Material2);
	}

	
	MeshComponent->SetRelativeScale3D(Scale);
}


// Called when the game starts or when spawned
void AFunctionBlockActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFunctionBlockActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

