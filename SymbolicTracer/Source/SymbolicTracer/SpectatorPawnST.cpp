// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorPawnST.h"

#include "CodeBlockActorST.h"
#include "CodeFilesDataEntry.h"
#include "CodeLinesDataEntry.h"
#include "CustomLogging.h"
#include "DataStructs.h"
#include "InstructionActorST.h"
#include "InstructionLineActorST.h"
#include "JsonObjectConverter.h"
#include "utils.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpectatorPawnST::ASpectatorPawnST()
{
	// TODO: maybe turn this of?!
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpectatorPawnST::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpectatorPawnST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASpectatorPawnST::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

int ASpectatorPawnST::GetClickedRunIndex() const
{
	return ClickedRunIndex;
}

int ASpectatorPawnST::GetClickedInstructionIndex() const
{
	return ClickedInstructionIndex;
}

void ASpectatorPawnST::LoadTraceInstrData()
{
	
}

void ASpectatorPawnST::LoadTraceBlkData()
{
	
}



void ASpectatorPawnST::LoadCodeFiles()
{
	
}

void ASpectatorPawnST::ComputeCodeFilesLinesDataEntries()
{
	
}


void ASpectatorPawnST::ComputeBlocksCodeLines()
{
	
}

void ASpectatorPawnST::BroadcastCurrentStats()
{
	
}

void ASpectatorPawnST::ClickInstructionActor(int RunInd, int InstrInd)
{
	
}

void ASpectatorPawnST::ClickCodeBlockActor(int RunInd, int CodeBlockInd)
{
	
}

void ASpectatorPawnST::ClickCodeFile(int FileInd)
{
	
}

void ASpectatorPawnST::SpawnInstrObjects()
{
	
}


void ASpectatorPawnST::SpawnInstrLineObjects()
{
	
}

void ASpectatorPawnST::SpawnBlkObjects()
{
	
}
