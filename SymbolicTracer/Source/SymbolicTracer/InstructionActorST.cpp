// Fill out your copyright notice in the Description page of Project Settings.


#include "InstructionActorST.h"

#include "CustomLogging.h"
#include "EBranchTypes.h"


// Sets default values
AInstructionActorST::AInstructionActorST()
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

void AInstructionActorST::Init(const EInstructionType& InstrType, UMaterialInterface* SymBehMaterial,
                               const FVector& Pos, const FVector& Scale,
                               int MyRunIndex, int MyInstrIndex, int MyStepIndex, int MyStepId,
                               const EBranchTypes& BranchType)
{
	RunIndex = MyRunIndex;
	InstructionIndex = MyInstrIndex;
	StepIndex = MyStepIndex;
	StepId = MyStepId;
	Position = Pos;
	InstructionType = InstrType;


	// TODO: if static mesh component is set set the other meshes you no longer need to null and also Materials
	// TODO: best to do this in another function -> or maybe all meshes/materials can be nulled?
	switch (InstrType)
	{
	case EInstructionType::Jump:
		{
			MeshComponent->SetStaticMesh(JumpMesh);
			MeshComponent->SetMaterial(1, SymBehMaterial);
			const FRotator Rotation(0, 90, 0);
			MeshComponent->AddRelativeRotation(Rotation);
			break;
		}
	case EInstructionType::Arith:
		{
			MeshComponent->SetStaticMesh(ArithmeticMesh);
			MeshComponent->SetMaterial(0, SymBehMaterial);
			break;
		}
	case EInstructionType::ECALL:
		{
			MeshComponent->SetStaticMesh(ECallMesh);
			break;
		}
	case EInstructionType::Load:
		{
			MeshComponent->SetStaticMesh(LoadStoreMesh);
			MeshComponent->SetMaterial(0, SymBehMaterial);
			const FRotator Rotation(0, 0, 90);
			MeshComponent->AddRelativeRotation(Rotation);
			break;
		}
	case EInstructionType::Store:
		{
			MeshComponent->SetStaticMesh(LoadStoreMesh);
			MeshComponent->SetMaterial(0, SymBehMaterial);
			const FRotator Rotation(0, 0, -90);
			MeshComponent->AddRelativeRotation(Rotation);
			break;
		}
	case EInstructionType::Branch:
		{
			switch (BranchType)
			{
			case EBranchTypes::BranchBoth:
				{
					MeshComponent->SetStaticMesh(BranchMeshBoth);
					MeshComponent->SetMaterial(2, SymBehMaterial);
					const FRotator Rotation(0, 90, 0);
					MeshComponent->AddRelativeRotation(Rotation);
					break;
				}
			case EBranchTypes::BranchTrue:
				{
					MeshComponent->SetStaticMesh(BranchMeshTrue);
					MeshComponent->SetMaterial(2, SymBehMaterial);
					const FRotator Rotation(0, 90, 0);
					MeshComponent->AddRelativeRotation(Rotation);
					break;
				}
			case EBranchTypes::BranchFalse:
				{
					MeshComponent->SetStaticMesh(BranchMeshFalse);
					MeshComponent->SetMaterial(2, SymBehMaterial);
					const FRotator Rotation(0, 90, 0);
					MeshComponent->AddRelativeRotation(Rotation);
					break;
				}
			default:
				{
					MeshComponent->SetStaticMesh(BranchMeshBoth);
					MeshComponent->SetMaterial(2, SymBehMaterial);
					const FRotator Rotation(0, 90, 0);
					MeshComponent->AddRelativeRotation(Rotation);
					break;
				}
			}
			break;
		}
	default:
		MeshComponent->SetStaticMesh(DefaultMesh);
		break;
	}

	MeshComponent->SetRelativeScale3D(Scale);


	UE_LOG(STLog, Display, TEXT("Testing init of InstructionActor: %d"), RunIndex);
}

void AInstructionActorST::ChangeMaterial(UMaterialInterface* Material)
{
	if (MeshComponent != nullptr)
	{
		// jump and branches don't change materials based on their symbolic behaviour
		if (InstructionType == EInstructionType::Branch)
		{
			MeshComponent->SetMaterial(2, Material);
		}
		else if (InstructionType == EInstructionType::Jump)
		{
			MeshComponent->SetMaterial(1, Material);
		}
		else
		{
			MeshComponent->SetMaterial(0, Material);
		}
	}
}

void AInstructionActorST::Hide(bool hide)
{
	HideInGame = hide;

	if (HideInGame)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
	else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
}

// Called when the game starts or when spawned
void AInstructionActorST::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInstructionActorST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
