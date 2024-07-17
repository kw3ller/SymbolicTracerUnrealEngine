// Copyright Epic Games, Inc. All Rights Reserved.

#include "SymbolicTracer.h"
#include "CustomLogging.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, SymbolicTracer, "SymbolicTracer");

// needed for logging declared in CustomLogging.h
DEFINE_LOG_CATEGORY(STLog);
