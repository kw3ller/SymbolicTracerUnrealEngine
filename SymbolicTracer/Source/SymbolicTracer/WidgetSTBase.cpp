// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSTBase.h"


#if WITH_EDITOR
const FText UWidgetSTBase::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "CustomPaletteCategory", "Symbolic Tracer");
}
#endif
