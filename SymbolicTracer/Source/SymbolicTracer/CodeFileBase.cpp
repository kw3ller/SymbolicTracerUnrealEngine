// Fill out your copyright notice in the Description page of Project Settings.


#include "CodeFileBase.h"

#include "CodeFilesDataEntry.h"
#include "Components/TextBlock.h"


void UCodeFileBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UCodeFilesDataEntry* Item = Cast<UCodeFilesDataEntry>(ListItemObject);


	CovText->SetText(FText::FromString(FString::FromInt(Item->CodeCovPercentage) + "%"));
	if (Item->CodeCovPercentage >= 90)
	{
		// color green
		CovText->SetColorAndOpacity(FColor(23, 255, 3, 255));
	}
	else if (Item->CodeCovPercentage >= 70)
	{
		// color orange
		CovText->SetColorAndOpacity(FColor(255, 60, 0, 255));
	}
	else
	{
		// color red
		CovText->SetColorAndOpacity(FColor(255, 0, 0, 255));
	}
	
	Content->SetText(FText::FromString(Item->Content));
	FileIndex = Item->FileIndex;
}
