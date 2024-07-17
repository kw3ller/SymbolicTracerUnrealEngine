// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeFilesDataEntry.h"

void UCodeFilesDataEntry::Init(FString FileName,int CodeCovPerc, int FileInd)
{
	Content = FileName;
	FileIndex = FileInd;
	CodeCovPercentage = CodeCovPerc;
}
