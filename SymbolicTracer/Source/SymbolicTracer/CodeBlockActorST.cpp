// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeBlockActorST.h"


// Sets default values
ACodeBlockActorST::ACodeBlockActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the meshComponent
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultMeshComponent"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACodeBlockActorST::Init(FVector Pos, FVector Scale, int Count, int MyRunIndex, int MyBlockIndex)
{
	RunIndex = MyRunIndex;
	BlockIndex = MyBlockIndex;
	Position = Pos;

	MeshComponent->SetRelativeScale3D(Scale);

	const UStaticMeshComponent* Cube = FindComponentByClass<UStaticMeshComponent>();
	UMaterialInterface* Material = Cube->GetMaterial(1);
	// UMaterialInterface* BaseMaterial = MeshComponent->GetMaterial(0);
	MarkMaterialInterface = UMaterialInstanceDynamic::Create(Material, NULL);
	MeshComponent->SetMaterial(1, MarkMaterialInterface);
	// MarkMaterialInterface->SetVectorParameterValue("HitColor", FLinearColor::Red);
	// Blends between Yellow (smaller) and red (bigger)
	// TODO: set Blend accordingly to number of executions (max number of steps in this codeBlock)

	const float BlendValue = Count / 10.0f; 
	
	MarkMaterialInterface->SetScalarParameterValue(TEXT("Blend"), BlendValue);


	//UMaterialInterface MaterialInterface = new Mater

	

	// set the mesh inside the blueprint
	// MeshComponent->SetStaticMesh(DefaultMesh);
	// MeshComponent->SetMaterial(0, DefaultMaterial);
}

// Called when the game starts or when spawned
void ACodeBlockActorST::BeginPlay()
{
	Super::BeginPlay();



	
}

// Called every frame
void ACodeBlockActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}
