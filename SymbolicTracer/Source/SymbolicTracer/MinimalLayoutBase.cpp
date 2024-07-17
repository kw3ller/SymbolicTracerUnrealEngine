// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimalLayoutBase.h"

#include "GeneralAnalysisBase.h"
#include "PathAnalysisBase.h"
#include "RTSPlayerPawnST.h"


void UMinimalLayoutBase::Init(ARTSPlayerPawnST* PlayerChar)
{
	PlayerCharacter = PlayerChar;
	PathAnalysis->Init(PlayerCharacter);
	GeneralAnalysis->Init(PlayerCharacter);
}
