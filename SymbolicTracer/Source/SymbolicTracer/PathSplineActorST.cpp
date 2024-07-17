// Fill out your copyright notice in the Description page of Project Settings.


#include "PathSplineActorST.h"

#include "Components/SplineComponent.h"


// Sets default values
APathSplineActorST::APathSplineActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	SplineComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void APathSplineActorST::Init(const FVector& Start, const FVector& End, const FVector& Scale)
{
	SplineComponent->AddSplineWorldPoint(Start);
	SplineComponent->AddSplineWorldPoint(End);

	SplineComponent->RemoveSplinePoint(0);
	SplineComponent->RemoveSplinePoint(0);

	SplineComponent->SetScaleAtSplinePoint(0, Scale, true);
	SplineComponent->SetScaleAtSplinePoint(1, FVector(0.01f), true);
}

void APathSplineActorST::Hide(bool Hide)
{
	HideInGame = Hide;

	for (int i = 0; i < SplineMeshComponents.Num(); i++)
	{
		SplineMeshComponents[i]->SetVisibility(!HideInGame);
	}
}

void APathSplineActorST::OnConstruction(const FTransform& Transform)
{
	if (!DefaultMesh)
	{
		return;
	}

	Super::OnConstruction(Transform);

	for (int SplineCount = 0; SplineCount < SplineComponent->GetNumberOfSplinePoints() - 1; SplineCount++)
	{
		SplineMeshComponents.Emplace(NewObject<USplineMeshComponent>(
			this, USplineMeshComponent::StaticClass()));


		SplineMeshComponents.Last()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SplineMeshComponents.Last()->SetMobility(EComponentMobility::Movable); // NOTE: probably can set this to static
		SplineMeshComponents.Last()->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMeshComponents.Last()->RegisterComponentWithWorld(GetWorld());


		SplineMeshComponents.Last()->AttachToComponent(SplineComponent,
		                                               FAttachmentTransformRules::KeepRelativeTransform);
		//SplineMeshComponent->SetMobility(EComponentMobility::Movable); // NOTE: probably can set this to static
		//SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		// SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		// AddInstanceComponent(SplineMeshComponent);


		const FVector StartPoint = SplineComponent->
			GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(
			SplineCount, ESplineCoordinateSpace::Local);
		const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(
			SplineCount + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(
			SplineCount + 1, ESplineCoordinateSpace::Local);

		const FVector StartScale = SplineComponent->GetScaleAtSplinePoint(SplineCount);
		const FVector EndScale = SplineComponent->GetScaleAtSplinePoint(SplineCount + 1);

		SplineMeshComponents.Last()->SetStartScale(FVector2d(StartScale.Y, StartScale.Z));
		SplineMeshComponents.Last()->SetEndScale(FVector2d(EndScale.Y, EndScale.Z));

		SplineMeshComponents.Last()->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);
		SplineMeshComponents.Last()->SetForwardAxis(ForwardAxis);

		
		SplineMeshComponents.Last()->SetStaticMesh(DefaultMesh);
		


		SplineMeshComponents.Last()->SetMaterial(0, DefaultMaterial);
		SplineMeshComponents.Last()->SetCastShadow(false);

		// SplineMeshComponent->SetVisibility(false);

		// SplineMeshComponent->SetRelativeScale3D(FVector(1.0f, 0.25f, 0.25f));
	}
}

// Called when the game starts or when spawned
void APathSplineActorST::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APathSplineActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
