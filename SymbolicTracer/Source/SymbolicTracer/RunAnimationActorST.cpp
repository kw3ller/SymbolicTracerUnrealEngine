// Fill out your copyright notice in the Description page of Project Settings.


#include "RunAnimationActorST.h"


// Sets default values
ARunAnimationActorST::ARunAnimationActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// bind the meshComponent
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	
}

void ARunAnimationActorST::Init(const int32 MaxDepth)
{
	const FVector Scale (0.08f, 0.08f, 0.09f * MaxDepth);
	MeshComponent->SetRelativeScale3D(Scale);
}

void ARunAnimationActorST::TeleportToTarget(const FVector& Target)
{
	SetActorLocation(Target);
	StartLocation = Target;
	TotalDistance = 0.0f;
	CurrentDistance = 0.0f;	
	Direction = FVector(0);
	Finished = true;
}


void ARunAnimationActorST::MoveToTarget(const FVector& Target, const float NewSpeed)
{
	StartLocation = GetActorLocation();
	Speed = NewSpeed;
	Direction = Target - GetActorLocation();
	TotalDistance = Direction.Size();
	CurrentDistance = 0.0f;
	Finished = false;
}

// Called when the game starts or when spawned
void ARunAnimationActorST::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARunAnimationActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(CurrentDistance < TotalDistance)
	{
		FVector Location = GetActorLocation();
		Location += Direction * Speed * DeltaTime;
		SetActorLocation(Location);
		CurrentDistance = (Location - StartLocation).Size();
		Finished = false;
	} else
	{
		Finished = true;
	}
}

