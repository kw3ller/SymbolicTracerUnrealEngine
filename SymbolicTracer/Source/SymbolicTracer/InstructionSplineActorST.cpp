// Fill out your copyright notice in the Description page of Project Settings.


#include "InstructionSplineActorST.h"

#include "CustomLogging.h"
#include "EBranchTypes.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"


// Sets default values
AInstructionSplineActorST::AInstructionSplineActorST()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// assign root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	SplineComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	//Mesh = CreateDefaultSubobject<UStaticMesh>("Mesh");
	//Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

void AInstructionSplineActorST::Init(const FVector& Start, const FVector& Middle, const FVector& End,
                                     int ExecutionNr, const EBranchTypes& BranchType)
{

	// NOTE: original scaling 0.01f
	float ClampedScale = 0.009f;
	ClampedScale = FMath::Clamp(ClampedScale + ExecutionNr * 0.005f, 0.009f, 0.07f);
	
	const FVector Scale(ClampedScale);
	
	switch (BranchType)
	{
	case EBranchTypes::BranchTrue:
		DefaultMaterial = BranchTrueMaterial;
		break;
	case EBranchTypes::BranchFalse:
		DefaultMaterial = BranchFalseMaterial;
		break;
	default:
		DefaultMaterial = NormalMaterial;
	}


	SplineComponent->AddSplineWorldPoint(Start);
	SplineComponent->AddSplineWorldPoint(Middle);
	SplineComponent->AddSplineWorldPoint(End);

	SplineComponent->RemoveSplinePoint(0);
	SplineComponent->RemoveSplinePoint(0);

	SplineComponent->SetScaleAtSplinePoint(0, Scale, true);
	SplineComponent->SetScaleAtSplinePoint(1, Scale, true);
	SplineComponent->SetScaleAtSplinePoint(2, Scale, true);

	SplineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// SplineComponent->SetRelativeScale3D(Scale);
}

void AInstructionSplineActorST::Hide(bool Hide)
{
	HideInGame = Hide;

	for (int i = 0; i < SplineMeshComponents.Num(); i++)
	{
		SplineMeshComponents[i]->SetVisibility(!HideInGame);
	}
}

void AInstructionSplineActorST::SetMaterial(UMaterial* Material)
{
	for (int i = 0; i < SplineMeshComponents.Num(); i++)
	{
		SplineMeshComponents[i]->SetMaterial(0, Material);
	}
}

void AInstructionSplineActorST::SetDefaultMaterial()
{
	for (int i = 0; i < SplineMeshComponents.Num(); i++)
	{
		SplineMeshComponents[i]->SetMaterial(0, DefaultMaterial);
	}
}

void AInstructionSplineActorST::OnConstruction(const FTransform& Transform)
{
	if (!Mesh)
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

		SplineMeshComponents.Last()->SetStaticMesh(Mesh);
		SplineMeshComponents.Last()->SetMaterial(0, DefaultMaterial);
		SplineMeshComponents.Last()->SetCastShadow(false);

		// SplineMeshComponent->SetVisibility(false);


		// SplineMeshComponent->SetRelativeScale3D(FVector(1.0f, 0.25f, 0.25f));
	}
}

// Called when the game starts or when spawned
void AInstructionSplineActorST::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInstructionSplineActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
