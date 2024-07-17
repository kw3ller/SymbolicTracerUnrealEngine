// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerPawnST.h"

#include "CodeBlkCountActorST.h"
#include "CodeBlkTextActorST.h"
#include "CodeBlockActorST.h"
#include "CodeFilesDataEntry.h"
#include "CodeLinesDataEntry.h"
#include "CustomLogging.h"
#include "EBranchTypes.h"
#include "FunctionBlockActorST.h"
#include "FunctionTextActorST.h"
#include "InstructionActorST.h"
#include "InstructionLineActorST.h"
#include "InstructionLineMarkActorST.h"
#include "InstructionSplineActorST.h"
#include "InstructionTextActorST.h"
#include "JsonObjectConverter.h"
#include "PathSplineActorST.h"
#include "PathTextActorST.h"
#include "RunAnimationActorST.h"
#include "utils.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


ARTSPlayerPawnST::ARTSPlayerPawnST()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.0f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// TODO: maybe add here orthographic view -> or in the BluePrint
}


void ARTSPlayerPawnST::BeginPlay()
{
	Super::BeginPlay();

	// set the initial desired zoom and location
	TargetLocation = GetActorLocation();
	TargetZoom = 3000.0f;

	// set initial rotation for the camera
	const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	TargetRotation = FRotator(Rotation.Pitch + -50, Rotation.Yaw, 0.0f);

	// initialize this table (since the materials are ll set in BP)
	SymBehMaterialTable = {
		{"none", SymBehNoneMaterial},
		{"create", SymBehCreateMaterial},
		{"destroy", SymBehDestroyMaterial},
		{"update", SymBehUpdateMaterial},
		{"overwrite", SymBehOverwriteMaterial},
		{"special", SymBehSpecialMaterial},
	};
}

void ARTSPlayerPawnST::Forward(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		return;
	}

	TargetLocation = SpringArmComponent->GetForwardVector() * AxisValue * MoveSpeed + TargetLocation;
}

void ARTSPlayerPawnST::Right(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		return;
	}

	TargetLocation = SpringArmComponent->GetRightVector() * AxisValue * MoveSpeed + TargetLocation;
}


void ARTSPlayerPawnST::TeleportToPc(int32 Pc, int RunInd, int InstrInd, int StepInd, bool Branch)
{
	if (Pc < 0)
	{
		return;
	}
	const int MinPc = TraceInstrStructure.analysis.min_pc;
	float PosX = (Pc - MinPc) * BlockDistance / 4;
	float PosY = -100.0f;
	if (RunInd > -1)
	{
		PosY = -(RunInd * RunDistance + MiddleOfRunY);
	}

	const FVector Position(PosX, PosY, 0.0f);
	TargetLocation = Position;
	TargetZoom = 400.0f;

	MarkPcInScene(Pc);
	MarkInstrSplineInScene(RunInd, InstrInd, StepInd, Branch);
	//FRotator Rotation(0.0f);
	//TeleportTo(Position, Rotation);
}

void ARTSPlayerPawnST::MarkPcInScene(int32 Pc)
{
	GetWorldTimerManager().ClearTimer(TimerHandleInstructionLineMark);
	if (TraceInstrStructure.InstructionLineMarkActor)
	{
		UnMarkPcInScene();
	}
	const int MinPc = TraceInstrStructure.analysis.min_pc;
	const int NumRuns = TraceInstrStructure.analysis.num_runs;
	const float PosX = (Pc - MinPc) * BlockDistance / 4 + InstrLineStart;
	const FVector Position(PosX, 108.0f, 21.0f);
	const float ScaleY = (NumRuns * RunDistance - (RunDistance - NewBlockSizeY)) / NewBlockSizeY;
	const FRotator Rotation(0.0);


	FTransform SpawnTransform(Rotation, Position);
	TraceInstrStructure.InstructionLineMarkActor = Cast<AInstructionLineMarkActorST>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionLineMarkActor, SpawnTransform));

	if (TraceInstrStructure.InstructionLineMarkActor != nullptr)
	{
		FVector Scale(ScaleY + 0.55f, 0.15f, 1.0f);
		TraceInstrStructure.InstructionLineMarkActor->Init(Scale);

		UGameplayStatics::FinishSpawningActor(TraceInstrStructure.InstructionLineMarkActor,
		                                      SpawnTransform);
	}

	// set timer for destroy actor

	GetWorldTimerManager().SetTimer(TimerHandleInstructionLineMark, this, &ARTSPlayerPawnST::UnMarkPcInScene, 2.0f,
	                                false, 15.0f);
}

void ARTSPlayerPawnST::UnMarkPcInScene()
{
	if (TraceInstrStructure.InstructionLineMarkActor)
	{
		TraceInstrStructure.InstructionLineMarkActor->Destroy();
		TraceInstrStructure.InstructionLineMarkActor = nullptr;
	}
}

void ARTSPlayerPawnST::MarkInstrSplineInScene(int RunInd, int InstrInd, int StepInd, bool Branch)
{
	GetWorldTimerManager().ClearTimer(TimerHandleInstructionSplineMark);

	UnMarkInstrSplineInScene();
	MarkedInstructionSplineActor = nullptr;

	if (RunInd > -1 && InstrInd > -1 && RunInd < TraceInstrStructure.runs.run.Num() && InstrInd <
		TraceInstrStructure.runs.run[RunInd].instruction.Num())
	{
		// 3D spline
		if (AbstractionLevel == EAbstractionLevel::Level4 && StepInd > -1 && StepInd < TraceInstrStructure.runs.run[
			RunInd].instruction[InstrInd].step.Num())
		{
			MarkedInstructionSplineActor = TraceInstrStructure.runs.run[RunInd].instruction[InstrInd].step[StepInd].
				InstructionSplineActor3D;
		}
		else if (AbstractionLevel == EAbstractionLevel::Level3) // 2D spline
		{
			if (Branch)
			{
				MarkedInstructionSplineActor = TraceInstrStructure.runs.run[RunInd].instruction[InstrInd].
					InstructionSplineActorTrue2D;
			}
			else
			{
				MarkedInstructionSplineActor = TraceInstrStructure.runs.run[RunInd].instruction[InstrInd].
					InstructionSplineActorFalse2D;
			}
		}
		if (MarkedInstructionSplineActor != nullptr)
		{
			MarkedInstructionSplineActor->SetMaterial(InstrSplineMarkMaterial);
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandleInstructionSplineMark, this, &ARTSPlayerPawnST::UnMarkInstrSplineInScene,
	                                2.0f,
	                                false, 15.0f);
}

void ARTSPlayerPawnST::UnMarkInstrSplineInScene()
{
	if (MarkedInstructionSplineActor != nullptr)
	{
		MarkedInstructionSplineActor->SetDefaultMaterial();
	}
}


void ARTSPlayerPawnST::Zoom(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		return;
	}

	// the 100 changes how much the zoom changes per tick mouse
	const float Zoom = AxisValue * 100.0f;
	TargetZoom = FMath::Clamp(Zoom + TargetZoom, MinZoom, MaxZoom);
}

void ARTSPlayerPawnST::RotateRight()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f, -45.0f, 0.0f));
}

void ARTSPlayerPawnST::RotateLeft()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f, 45.0f, 0.0f));
}

void ARTSPlayerPawnST::EnableRotate()
{
	CanRotate = true;
}

void ARTSPlayerPawnST::DisableRotate()
{
	CanRotate = false;
}

void ARTSPlayerPawnST::RotateHorizontal(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		return;
	}
	if (CanRotate)
	{
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f, AxisValue, 0.0f));
	}
}

void ARTSPlayerPawnST::RotateVertical(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		return;
	}
	if (CanRotate)
	{
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(AxisValue, 0.0f, 0.0f));
	}
}

void ARTSPlayerPawnST::CameraBounds()
{
	float NewPitch = TargetRotation.Pitch;
	if (TargetRotation.Pitch < (RotatePitchMax * -1))
	{
		NewPitch = (RotatePitchMax * -1);
	}
	else if (TargetRotation.Pitch > (RotatePitchMin * -1))
	{
		NewPitch = (RotatePitchMin * -1);
	}

	// set the new pitch and clamp any roll
	TargetRotation = FRotator(NewPitch, TargetRotation.Yaw, 0.0f);

	// clamp desired location to within the bounds
	TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, 0.0f);
}

// Called every frame
void ARTSPlayerPawnST::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (RunAnimationRunning && CurrentRunAnimationActor != nullptr)
	{
		// TODO: animate whole run if CurrentRunAnimationIndex is invalid


		int& AnimationStepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			CurrentStepAnimationIndex;

		if (CurrentRunAnimationActor->Finished)
		{
			// We animate the current AnimationStepIndex and set it afterwards
			const int RunIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					AnimationStepIndex].run_index;
			const int InstructionIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					AnimationStepIndex].instruction_index;
			const int StepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					AnimationStepIndex].step_index;
			const int StepId = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					AnimationStepIndex].step_id;


			if (RunIndex > -1 && InstructionIndex > -1 && StepIndex > -1 && RunIndex < TraceInstrStructure.runs.run.
				Num() &&
				InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num() &&
				StepIndex < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num())
			{
				const AInstructionActorST* InstrActor = TraceInstrStructure.runs.run[RunIndex].instruction[
					InstructionIndex].InstructionActor2D;
				if (InstrActor != nullptr)
				{
					const FVector TargetPosition = InstrActor->GetActorLocation();
					CurrentRunAnimationActor->MoveToTarget(
						TargetPosition, 0.7);

					// mark the current instructionActor in Scene and HUD
					MarkInstructionActor(RunIndex, InstructionIndex, StepIndex, StepId);


					// set camera position
					if (FollowAnimationWithCamera)
					{
						const FVector NewCameraPosition(TargetPosition.X, TargetPosition.Y, 0.0f);
						TargetLocation = NewCameraPosition;
					}
				}
			}

			if (AnimationStepIndex + 1 > -1 && AnimationStepIndex + 1 < TraceInstrStructure.trace_animation.
				run_animations[CurrentRunAnimationIndex].animation_steps.Num())
			{
				AnimationStepIndex += 1;
			}
			else
			{
				AnimationStepIndex = 0;
				ResetSymMaterials(CurrentRunAnimationIndex, 0);
			}
		}
	}


	// Camera Movement

	CameraBounds();

	// move the pawn in the desired direction
	const FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime,
	                                                                   MoveSpeed);
	SetActorLocation(InterpolatedLocation);

	// zoom the camera in the desired location
	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom,
	                                                             DeltaTime, ZoomSpeed);
	SpringArmComponent->TargetArmLength = InterpolatedZoom;

	// rotate the camera to the desired location
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(),
	                                                                    TargetRotation, DeltaTime, RotateSpeed);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);
}

// Called to bind functionality to input
void ARTSPlayerPawnST::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

int ARTSPlayerPawnST::GetClickedRunIndex() const
{
	return ClickedRunIndex;
}

int ARTSPlayerPawnST::GetClickedInstructionIndex() const
{
	return ClickedInstructionIndex;
}

FString ARTSPlayerPawnST::SelectDataDir()
{
	FString SelectedDirectory = "";
	FString DefaultPath = "/home";
	const FString Message = "Select the directory where the trace data lies";
	// NOTE. can not be used in final Build
	// PromptUserForDirectory(SelectedDirectory, Message, DefaultPath);

	return SelectedDirectory;
}

void ARTSPlayerPawnST::LoadTraceInstrData(FString DataDir)
{
	UE_LOG(STLog, Warning, TEXT("SpectatorPawn: LoadingTraceInstrData"));

	FString PtraceJsonPath = DataDir + "out_json.ptrace";

	// TODO: remove -> testing atm
	//PtraceJsonPath = "/home/kristoffer/Desktop/masterarbeit/TestData/jsonTraces/example_trace_ptrace_simple.json";

	// We will use this FileManager to deal with the file.
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString fileContent;

	// Always first check if the file that you want to manipulate exist.
	if (FileManager.FileExists(*PtraceJsonPath))
	{
		// We use the LoadFileToString to load the file into
		if (FFileHelper::LoadFileToString(fileContent, *PtraceJsonPath))
		{
			// UE_LOG(STLog, Warning, TEXT("The File Content is: %s"), *fileContent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Did not load text from file"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *PtraceJsonPath);
	}

	FJsonObjectConverter::JsonObjectStringToUStruct(fileContent, &TraceInstrStructure, 0, 0);

	// TODO: check if data was successfully loaded

	// if successful loading of json spawn the different actors (atm TESTING)
	SpawnInstrObjects();

	SpawnPathSplineObjects();

	SpawnPathTextObjects();

	SpawnInstrLineObjects();

	SpawnRunAnimationObjects();
}

void ARTSPlayerPawnST::LoadTraceBlkData(FString DataDir)
{
	UE_LOG(STLog, Warning, TEXT("SpectatorPawn: LoadingTraceBlkData"));

	FString blkJsonPath = DataDir + "out_json.blk";

	// TODO: remove -> testing atm
	// blkJsonPath = "/home/kristoffer/Desktop/masterarbeit/TestData/jsonTraces/example_trace_blk_simple.json";

	// We will use this FileManager to deal with the file.
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString fileContent;

	// Always first check if the file that you want to manipulate exist.
	if (FileManager.FileExists(*blkJsonPath))
	{
		// We use the LoadFileToString to load the file into
		if (FFileHelper::LoadFileToString(fileContent, *blkJsonPath))
		{
			// UE_LOG(STLog, Warning, TEXT("The File Content is: %s"), *fileContent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Did not load text from file"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *blkJsonPath);
	}


	FJsonObjectConverter::JsonObjectStringToUStruct(fileContent, &TraceBlkStructure, 0, 0);

	// TODO: should probably test if file was loaded successfully

	LoadCodeFiles(DataDir);

	// if successful loading of json spawn the different actors 
	SpawnFunctionBlkTextObjects();
	// spawn codeBlockActors
	SpawnBlkObjects();
	// spawn codeBlkTextActors
	SpawnBlkTextObjects();
	// set initial abstraction level
	ChangeAbstractionLevel(EAbstractionLevel::Level1);
}


void ARTSPlayerPawnST::LoadCodeFiles(FString DataDir)
{
	// TODO: make this whole function safer/better structured/more performant?! 

	// the path where we search for the codeFiles via name
	FString basePath = DataDir;

	// TODO: remove -> testing atm
	// basePath = "/home/kristoffer/Desktop/masterarbeit/TestData/CodeFiles/";

	// We will use this FileManager to deal with the file.
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	for (int i = 0; i < TraceBlkStructure.code_files.Num(); i++)
	{
		FString FilePath = basePath + TraceBlkStructure.code_files[i].name;

		TArray<FString> FileContentArr;
		// Always first check if the file that you want to manipulate exist.
		if (FileManager.FileExists(*FilePath))
		{
			// We use the LoadFileToString to load the file into
			if (FFileHelper::LoadFileToStringArray(FileContentArr, *FilePath))
			{
				// UE_LOG(STLog, Warning, TEXT("The File Content is: %s"), *FileContentArr[0]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Did not load text from file"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *FilePath);
		}

		// write the codeLines into @TraceBlkStructure
		for (int j = 0; j < FileContentArr.Num(); j++)
		{
			FCodeLine newCodeLine;
			newCodeLine.Content = FileContentArr[j];
			TraceBlkStructure.code_files[i].code_lines.Add(newCodeLine);
		}
	}

	// TODO: should probably check if codeFiles isn't empty
	ClickedCodeFileIndex = 0;
	ClickedCodeLineIndex = -1;

	ComputeCompiledCodeLinesAndPc();
	ComputeBlocksCodeLines();
	ComputeHitCodeLinesSymCon();
	ComputeBranchCodeLines();
	ComputeCodeCovPercentage();
	ComputeCodeFilesLinesDataEntries();
	ComputeAnalysisData();

	OnCodeFileLineIndexChanged.Broadcast(ClickedCodeFileIndex, ClickedCodeLineIndex);
}

void ARTSPlayerPawnST::ComputeCodeFilesLinesDataEntries()
{
	UE_LOG(STLog, Warning, TEXT("ComputerCodeLinesDataEntries called:"))
	for (int i = 0; i < TraceBlkStructure.code_files.Num(); i++)
	{
		// add codeFilesDataEntries
		UCodeFilesDataEntry* newFileToShow = NewObject<UCodeFilesDataEntry>();
		newFileToShow->Init(TraceBlkStructure.code_files[i].name, TraceBlkStructure.code_files[i].CodeCovPercentage, i);
		TraceBlkStructure.CodeFilesDataEntries.Add(newFileToShow);

		// add codeLinesDataEntries
		for (int j = 0; j < TraceBlkStructure.code_files[i].code_lines.Num(); j++)
		{
			UCodeLinesDataEntry* newLineToShow = NewObject<UCodeLinesDataEntry>();
			newLineToShow->Init(this, i, j);

			TraceBlkStructure.code_files[i].CodeLinesDataEntries.Add(newLineToShow);
		}
	}
}

void ARTSPlayerPawnST::ComputeCompiledCodeLinesAndPc()
{
	for (int i = 0; i < TraceBlkStructure.code_files.Num(); i++)
	{
		for (int j = 0; j < TraceBlkStructure.code_files[i].compiled_code_lines.Num(); j++)
		{
			int CompiledLineInd = TraceBlkStructure.code_files[i].compiled_code_lines[j].line - 1;
			int CompiledLinePc = TraceBlkStructure.code_files[i].compiled_code_lines[j].pc;
			int CompiledLineHitCount = TraceBlkStructure.code_files[i].compiled_code_lines[j].hit_count;
			if (CompiledLineInd > -1 && CompiledLineInd < TraceBlkStructure.code_files[i].code_lines.Num())
			{
				TraceBlkStructure.code_files[i].code_lines[CompiledLineInd].CodeLineState = ECodeLineState::Compiled;
				TraceBlkStructure.code_files[i].code_lines[CompiledLineInd].pc = CompiledLinePc;
				TraceBlkStructure.code_files[i].code_lines[CompiledLineInd].HitCount = CompiledLineHitCount;
			}
		}
	}
}


void ARTSPlayerPawnST::ComputeBlocksCodeLines()
{
	for (int i = 0; i < TraceBlkStructure.run_blocks.Num(); i++)
	{
		for (int j = 0; j < TraceBlkStructure.run_blocks[i].block.Num(); j++)
		{
			const int blockId = TraceBlkStructure.run_blocks[i].block[j].block_id;
			const int codeFileId = TraceBlkStructure.cfg_blocks[blockId].code_file_id;
			const int lineStartInd = TraceBlkStructure.cfg_blocks[blockId].line_start - 1;
			const int lineEndInd = TraceBlkStructure.cfg_blocks[blockId].line_end - 1;

			if (codeFileId < 0 | lineStartInd < 0 | lineEndInd < 0)
			{
				continue;
			}

			if (codeFileId < TraceBlkStructure.code_files.Num() && lineStartInd < TraceBlkStructure.code_files[
				codeFileId].code_lines.Num() && lineEndInd < TraceBlkStructure.code_files[codeFileId].code_lines.Num())
			{
				TraceBlkStructure.code_files[codeFileId].code_lines[lineStartInd].CodeBlockBeginning = true;
				TraceBlkStructure.code_files[codeFileId].code_lines[lineEndInd].CodeBlockEnding = true;

				// compute all hit lines inside the codeBlock
				for (int k = lineStartInd; k <= lineEndInd; k++)
				{
					TraceBlkStructure.code_files[codeFileId].code_lines[k].CodeLineState = ECodeLineState::Hit;
				}
			}
		}
	}
}

void ARTSPlayerPawnST::ComputeHitCodeLinesSymCon()
{
	for (int i = 0; i < TraceInstrStructure.runs.run.Num(); i++)
	{
		for (int j = 0; j < TraceInstrStructure.runs.run[i].instruction.Num(); j++)
		{
			int CodeFileInd = TraceInstrStructure.runs.run[i].instruction[j].code_file_id;
			int CodeLineInd = TraceInstrStructure.runs.run[i].instruction[j].code_line - 1;
			bool Symbolized = false;
			bool Concretized = false;
			// check all steps for symbolized or conretized
			for (int k = 0; k < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); k++)
			{
				const FString Behaviour = TraceInstrStructure.runs.run[i].instruction[j].step[k].beh;
				if (Behaviour == "create" || Behaviour == "update" || Behaviour == "overwrite")
				{
					Symbolized = true;
				}
				if (Behaviour == "special")
				{
					Concretized = true;
				}
			}

			if (CodeFileInd > -1 && CodeLineInd > -1 && CodeFileInd < TraceBlkStructure.code_files.Num() && CodeLineInd
				< TraceBlkStructure.code_files[CodeFileInd].code_lines.Num())
			{
				TraceBlkStructure.code_files[CodeFileInd].code_lines[CodeLineInd].CodeLineState = ECodeLineState::Hit;
				if (Symbolized)
				{
					TraceBlkStructure.code_files[CodeFileInd].code_lines[CodeLineInd].Symbolized = true;
				}
				if (Concretized)
				{
					TraceBlkStructure.code_files[CodeFileInd].code_lines[CodeLineInd].Concretized = true;
				}
			}
		}
	}
}

// gets only used for ComputeBranchCodeLines
struct BranchCodeLine
{
	int Pc = -1;
	int CodeFileId = -1;
	int CodeLineInd = -1;
	bool HitTrue = false;
	bool HitFalse = false;

	BranchCodeLine(int Pc, int CodeFileId, int CodeLineInd, bool HitTrue, bool HitFalse)
		: Pc(Pc), CodeFileId(CodeFileId), CodeLineInd(CodeLineInd), HitTrue(HitTrue), HitFalse((HitFalse))
	{
	}
};

void ARTSPlayerPawnST::ComputeBranchCodeLines()
{
	TArray<BranchCodeLine> AllBranchCodeLines;

	// go through all instructions and collect branchCodeLines
	for (int i = 0; i < TraceInstrStructure.runs.run.Num(); i++)
	{
		for (int j = 0; j < TraceInstrStructure.runs.run[i].instruction.Num(); j++)
		{
			FString InstrType = TraceInstrStructure.runs.run[i].instruction[j].type;
			if (InstrType != "Branch")
			{
				continue;
			}
			const int Pc = TraceInstrStructure.runs.run[i].instruction[j].pc;
			int BranchCodeLineIndex = -1;
			// check if this pc was already added to AllBranchCodeLines
			for (int k = 0; k < AllBranchCodeLines.Num(); k++)
			{
				if (AllBranchCodeLines[k].Pc == Pc)
				{
					BranchCodeLineIndex = k;
					break;
				}
			}
			if (BranchCodeLineIndex != -1)
			{
				// go through all steps and check the condition
				for (int k = 0; k < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); k++)
				{
					if (TraceInstrStructure.runs.run[i].instruction[j].step[k].condition)
					{
						AllBranchCodeLines[BranchCodeLineIndex].HitTrue = true;
					}
					else
					{
						AllBranchCodeLines[BranchCodeLineIndex].HitFalse = true;
					}
				}
			}
			else
			{
				int CodeFileId = TraceInstrStructure.runs.run[i].instruction[j].code_file_id;
				int CodeLineInd = TraceInstrStructure.runs.run[i].instruction[j].code_line - 1;
				BranchCodeLine NewBranchCodeLine(Pc, CodeFileId, CodeLineInd, false, false);

				// go through all steps and check the condition
				for (int k = 0; k < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); k++)
				{
					if (TraceInstrStructure.runs.run[i].instruction[j].step[k].condition)
					{
						NewBranchCodeLine.HitTrue = true;
					}
					else
					{
						NewBranchCodeLine.HitFalse = true;
					}
				}

				AllBranchCodeLines.Emplace(NewBranchCodeLine);
			}
		}
	}

	// go through AllBranchCodeLines and write data into codeLines

	for (int i = 0; i < AllBranchCodeLines.Num(); i++)
	{
		const int CodeFileId = AllBranchCodeLines[i].CodeFileId;
		const int CodeLineInd = AllBranchCodeLines[i].CodeLineInd;

		if (CodeFileId > -1 && CodeLineInd > -1 && CodeFileId < TraceBlkStructure.code_files.Num() &&
			CodeLineInd < TraceBlkStructure.code_files[CodeFileId].code_lines.Num())
		{
			TraceBlkStructure.code_files[CodeFileId].code_lines[CodeLineInd].BranchCount += 2;
			if (AllBranchCodeLines[i].HitTrue)
			{
				TraceBlkStructure.code_files[CodeFileId].code_lines[CodeLineInd].BranchCoverage += 1;
			}
			if (AllBranchCodeLines[i].HitFalse)
			{
				TraceBlkStructure.code_files[CodeFileId].code_lines[CodeLineInd].BranchCoverage += 1;
			}
		}
	}
}

void ARTSPlayerPawnST::ComputeCodeCovPercentage()
{
	float GeneralCompiledOrHit = 0.0f;
	float GeneralHit = 0.0f;

	for (int i = 0; i < TraceBlkStructure.code_files.Num(); i++)
	{
		float CompiledOrHit = 0.0f;
		float Hit = 0.0f;

		for (int j = 0; j < TraceBlkStructure.code_files[i].code_lines.Num(); j++)
		{
			if (TraceBlkStructure.code_files[i].code_lines[j].CodeLineState == ECodeLineState::Hit)
			{
				CompiledOrHit += 1.0f;
				Hit += 1.0f;
				GeneralCompiledOrHit += 1.0f;
				GeneralHit += 1.0f;
			}
			else if (TraceBlkStructure.code_files[i].code_lines[j].CodeLineState == ECodeLineState::Compiled)
			{
				CompiledOrHit += 1.0f;
				GeneralCompiledOrHit += 1.0f;
			}
		}
		float CoveragePercentage = 100.0f;
		if (CompiledOrHit != 0.0f)
		{
			CoveragePercentage = Hit / CompiledOrHit * 100.0f;
		}
		TraceBlkStructure.code_files[i].CodeCovPercentage = CoveragePercentage;
	}
	float GeneralCoveragePercentage = 100.0f;
	if (GeneralCompiledOrHit != 0.0f)
	{
		GeneralCoveragePercentage = GeneralHit / GeneralCompiledOrHit * 100.0f;
	}
	TraceInstrStructure.analysis.CodeCovPercentage = GeneralCoveragePercentage;
}

void ARTSPlayerPawnST::ComputeAnalysisData()
{
	int NumRuns = TraceInstrStructure.analysis.num_runs;

	// number of all compiled program lines
	int NumAllProgramLines = 0;
	for (int i = 0; i < TraceBlkStructure.code_files.Num(); i++)
	{
		NumAllProgramLines += TraceBlkStructure.code_files[i].compiled_code_lines.Num();
	}


	for (int i = 0; i < NumRuns; i++)
	{
		FAnalysisPath NewPathAnalysis;
		NewPathAnalysis.RunInd = i;
		NewPathAnalysis.ParentRunInd = TraceInstrStructure.runs.run[i].parent;
		NewPathAnalysis.StartPcHex = TraceInstrStructure.runs.run[i].start_hex;
		NewPathAnalysis.EndPcHex = TraceInstrStructure.runs.run[i].end_hex;
		NewPathAnalysis.SplitStartStepNumber = TraceInstrStructure.trace_animation.run_animations[i].animation_steps[0].
			step_id;
		NewPathAnalysis.StepsNum = TraceInstrStructure.runs.run[i].steps;
		NewPathAnalysis.SplitStartPcHex = "";


		// search for SplitStartStepPc
		for (int j = 0; j < TraceInstrStructure.runs.run[i].instruction.Num(); j++)
		{
			if (NewPathAnalysis.SplitStartPcHex != "")
			{
				break;
			}
			for (int k = 0; k < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); k++)
			{
				if (TraceInstrStructure.runs.run[i].instruction[j].step[k].id == NewPathAnalysis.SplitStartStepNumber)
				{
					NewPathAnalysis.SplitStartPcHex = TraceInstrStructure.runs.run[i].instruction[j].pc_hex;
					break;
				}
			}
		}

		// calculate number of unique blocks in run
		TArray<int> UniqueBlocksInRun;
		for (int j = 0; j < TraceBlkStructure.run_blocks[i].block.Num(); j++)
		{
			if (!UniqueBlocksInRun.Contains(TraceBlkStructure.run_blocks[i].block[j].block_id))
			{
				UniqueBlocksInRun.Emplace(TraceBlkStructure.run_blocks[i].block[j].block_id);
			}
		}
		NewPathAnalysis.CodeBlocksNum = UniqueBlocksInRun.Num();


		// calculate number of codeLines this run hit

		// all hit code lines: fileInd, codeLine 
		TArray<TTuple<int, int>> CodeFileLines;

		int RunIndex = i;
		// indicates to which instruction it shall ran through
		int MaxAnimationStepIndex = TraceInstrStructure.trace_animation.run_animations[i].animation_steps.Num() - 1;

		// go through all instruction and codeBlocks -> with parent run
		while (RunIndex > -1 && RunIndex < TraceInstrStructure.trace_animation.run_animations.Num())
		{
			UE_LOG(STLog, Warning, TEXT("Going through max animationSteps run: %i, maxanimationStepIndex: %i"),
			       RunIndex, MaxAnimationStepIndex);
			// go through all instructions up to the end
			for (int j = 0; j <= MaxAnimationStepIndex; j++)
			{
				int AnimRunIndex = TraceInstrStructure.trace_animation.run_animations[RunIndex].animation_steps[j].
					run_index;
				int AnimStepIndex = TraceInstrStructure.trace_animation.run_animations[RunIndex].animation_steps[j].
					instruction_index;

				if (AnimRunIndex < 0 || AnimStepIndex < 0 || AnimRunIndex > TraceInstrStructure.runs.run.Num() ||
					AnimStepIndex > TraceInstrStructure.runs.run[AnimRunIndex].instruction.Num())
				{
					continue;
				}

				int FileId = TraceInstrStructure.runs.run[AnimRunIndex].instruction[AnimStepIndex].code_file_id;
				int Line = TraceInstrStructure.runs.run[AnimRunIndex].instruction[AnimStepIndex].code_line;


				if (FileId > -1 && Line > 0)
				{
					TTuple<int, int> NewEntry(FileId, Line);

					// UE_LOG(STLog, Warning, TEXT("FileLine found run: %i, File: %i, Line: %i"), i, FileId, Line);
					CodeFileLines.AddUnique(NewEntry);
				}
			}


			int MaxAnimationStep = TraceInstrStructure.trace_animation.run_animations[RunIndex].animation_steps[0].
				step_id - 1;
			int ParentRunIndex = TraceInstrStructure.runs.run[RunIndex].parent;
			if (ParentRunIndex > TraceInstrStructure.runs.run.Num() || ParentRunIndex < 0)
			{
				break;
			}
			// got through parentRun to find MaxAnimationStepIndex
			bool AnimationStepFound = false;
			for (int y = 0; y < TraceInstrStructure.trace_animation.run_animations[ParentRunIndex].animation_steps.Num()
			     ; y++)
			{
				if (TraceInstrStructure.trace_animation.run_animations[ParentRunIndex].animation_steps[y].step_id ==
					MaxAnimationStep)
				{
					MaxAnimationStepIndex = y;
					AnimationStepFound = true;
					break;
				}
			}
			if (!AnimationStepFound)
			{
				break;
			}
			RunIndex = TraceInstrStructure.runs.run[RunIndex].parent;
		}

		// go through all codeBlocks
		if (i < TraceBlkStructure.run_blocks.Num())
		{
			for (int j = 0; j < TraceBlkStructure.run_blocks[i].block.Num(); j++)
			{
				int BlockId = TraceBlkStructure.run_blocks[i].block[j].block_id;
				int FileId = TraceBlkStructure.cfg_blocks[BlockId].code_file_id;
				int LineStart = TraceBlkStructure.cfg_blocks[BlockId].line_start;
				int LineEnd = TraceBlkStructure.cfg_blocks[BlockId].line_end;

				if (FileId < 0 || LineStart < 1 || LineEnd < 1)
				{
					continue;
				}
				if (LineEnd < LineStart)
				{
					int End = LineEnd;
					LineEnd = LineStart;
					LineStart = End;
				}

				for (int x = LineStart; x <= LineEnd; x++)
				{
					UE_LOG(STLog, Warning, TEXT("Found codeBlock run: %i, file: %i, line: %i"), i, FileId, x);

					TTuple<int, int> NewEntry(FileId, x);

					CodeFileLines.AddUnique(NewEntry);
				}
			}
		}


		NewPathAnalysis.CodeCoveragePercentage = (float)CodeFileLines.Num() / (float)NumAllProgramLines * 100.0f;

		UE_LOG(STLog, Warning, TEXT("CodeFileLinesFound: %f"), (float)CodeFileLines.Num());

		TraceInstrStructure.analysis.analyis_paths.Emplace(NewPathAnalysis);
	}

	ClickedOnlyRunIndex = 0;
	OnRunIndexChanged.Broadcast(ClickedOnlyRunIndex);
}

void ARTSPlayerPawnST::BroadcastCurrentStats() const
{
	OnRunInstrStepIndexChanged.Broadcast(ClickedRunIndex, ClickedInstructionIndex, -1, ClickedStepId);
	OnCodeFileLineIndexChanged.Broadcast(ClickedCodeFileIndex, ClickedCodeLineIndex);
	OnRunIndexChanged.Broadcast(ClickedOnlyRunIndex);
	// OnRunCodeBlockIndexChanged.Broadcast(ClickedRunIndex, ClickedCodeBlockIndex);
}

void ARTSPlayerPawnST::ClickInstructionActor(int RunInd, int InstrInd, int StepInd, int StepId)
{
	if (RunInd < 0 | InstrInd < 0)
	{
		return;
	}

	ClickedRunIndex = RunInd;
	ClickedInstructionIndex = InstrInd;
	ClickedStepIndex = StepInd;
	ClickedStepId = StepId;

	UE_LOG(STLog, Warning, TEXT("clickInstructionActor called RunInd: %d, InstrInd: %d, StepInd: %d, StepId: %d"),
	       RunInd, InstrInd,
	       StepInd, StepId);

	// let others know that clicked indices have changed (e.g. the Hud)
	OnRunInstrStepIndexChanged.Broadcast(ClickedRunIndex, ClickedInstructionIndex, -1, ClickedStepId);
}

void ARTSPlayerPawnST::ClickCodeBlockActor(int RunInd, int CodeBlockInd)
{
	if (RunInd < 0 | CodeBlockInd < 0)
	{
		return;
	}


	ClickedRunIndex = RunInd;
	ClickedCodeBlockIndex = CodeBlockInd;

	if (ClickedRunIndex >= 0 && ClickedRunIndex < TraceBlkStructure.run_blocks.Num())
	{
		int BlockInd = TraceBlkStructure.run_blocks[ClickedRunIndex].block[ClickedCodeBlockIndex].block_id;
		if (TraceBlkStructure.cfg_blocks[BlockInd].code_file_id > -1 && TraceBlkStructure.cfg_blocks[BlockInd].
			line_start - 1 > -1)
		{
			ClickedCodeFileIndex = TraceBlkStructure.cfg_blocks[BlockInd].code_file_id;
			ClickedCodeLineIndex = TraceBlkStructure.cfg_blocks[BlockInd].line_start - 1;
		}
	}

	UE_LOG(STLog, Warning, TEXT("clickCodeBlockActor called RunInd: %d, CodeBlockInd: %d"), RunInd, CodeBlockInd);

	// let others know that clicked indices have changed (e.g. the Hud)
	OnRunCodeBlockIndexChanged.Broadcast(ClickedRunIndex, ClickedCodeBlockIndex);
}

void ARTSPlayerPawnST::ClickPathTextActor(int RunInd)
{
	UE_LOG(STLog, Warning, TEXT("clickPathTextActor called RunInd: %d"), RunInd);
	ClickedOnlyRunIndex = RunInd;
	if (RunInd > -1 && RunInd < TraceInstrStructure.analysis.num_runs)
	{
		OnRunIndexChanged.Broadcast(ClickedOnlyRunIndex);
	}
}


void ARTSPlayerPawnST::ClickCodeFile(int FileInd)
{
	ClickedCodeFileIndex = FileInd;
	ClickedCodeLineIndex = -1;

	UE_LOG(STLog, Warning, TEXT("clickCodeFile called FileInd: %d"), FileInd);
	OnCodeFileLineIndexChanged.Broadcast(ClickedCodeFileIndex, ClickedCodeLineIndex);
}

void ARTSPlayerPawnST::ChangeAbstractionLevel(EAbstractionLevel Level)
{
	AbstractionLevel = Level;

	bool HideInstructionLineActors = true;
	bool Hide3DInstructionActors = true;
	bool Hide3DInstructionSplineActors = true;
	bool Hide2DInstructionActors = true;
	bool Hide2DInstructionSplineActors = true;
	// bool HidePathSplineActors = true;
	bool HideInstructionTextActors = true;

	if (AbstractionLevel > EAbstractionLevel::Level0)
	{
		// HidePathSplineActors = false;
	}
	if (AbstractionLevel > EAbstractionLevel::Level1)
	{
		HideInstructionLineActors = false;
		HideInstructionTextActors = false;
	}
	if (AbstractionLevel > EAbstractionLevel::Level2)
	{
		Hide2DInstructionActors = false;
		Hide2DInstructionSplineActors = false;
	}
	if (AbstractionLevel > EAbstractionLevel::Level3)
	{
		Hide2DInstructionActors = true;
		Hide2DInstructionSplineActors = true;

		Hide3DInstructionActors = false;
		Hide3DInstructionSplineActors = false;
	}


	// show or hide all InstructionLineActors
	for (int i = 0; i < TraceInstrStructure.InstructionLineActors.Num(); i++)
	{
		TraceInstrStructure.InstructionLineActors[i]->Hide(HideInstructionLineActors);
	}

	// show or hide all 3D InstructionActors and 3D SplineActors
	// show or hide all 2D InstructionActors and 2D SplineActors
	// show or hide all PathSplineActors
	for (int i = 0; i < TraceInstrStructure.runs.run.Num(); i++)
	{
		for (int j = 0; j < TraceInstrStructure.runs.run[i].InstructionTextActors.Num(); j++)
		{
			TraceInstrStructure.runs.run[i].InstructionTextActors[j]->Hide(HideInstructionTextActors);
		}

		/* // show or hide pathSplineActor -> atm show always
		if(TraceInstrStructure.runs.run[i].PathSplineActor != nullptr)
		{
			TraceInstrStructure.runs.run[i].PathSplineActor->Hide(HidePathSplineActors);
		}
		*/

		for (int j = 0; j < TraceInstrStructure.runs.run[i].instruction.Num(); j++)
		{
			if (TraceInstrStructure.runs.run[i].instruction[j].InstructionActor2D != nullptr)
			{
				TraceInstrStructure.runs.run[i].instruction[j].InstructionActor2D->
				                                               Hide(Hide2DInstructionActors);
			}
			if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D != nullptr)
			{
				TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D->
				                                               Hide(Hide2DInstructionSplineActors);
			}
			if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D != nullptr)
			{
				TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D->
				                                               Hide(Hide2DInstructionSplineActors);
			}

			for (int k = 0; k < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); k++)
			{
				if (TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionActor3D != nullptr)
				{
					TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionActor3D->
					                                                       Hide(Hide3DInstructionActors);
				}
				if (TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionSplineActor3D != nullptr)
				{
					TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionSplineActor3D->
					                                                       Hide(Hide3DInstructionSplineActors);
				}
			}
		}
	}
}


void ARTSPlayerPawnST::SpawnInstrObjects()
{
	const int MinPc = TraceInstrStructure.analysis.min_pc;
	const FVector Scale(InstrScale);
	// FVector Origin(InstrLineStart, -MiddleOfRunY, 20.0f);
	const FRotator Rotation(0.0f);


	for (int i = 0; i < TraceInstrStructure.runs.run.Num(); i++)
	{
		for (int j = 0; j < TraceInstrStructure.runs.run[i].instruction.Num(); j++)
		{
			EInstructionType* InstrTypeVal = InstructionTypeTable.Find(
				TraceInstrStructure.runs.run[i].instruction[j].type);
			EInstructionType InstrType = EInstructionType::Default;

			if (InstrTypeVal)
			{
				InstrType = *InstrTypeVal;
			}

			const float InstrPC = TraceInstrStructure.runs.run[i].instruction[j].pc;

			const float PosX = (InstrPC - MinPc) * BlockDistance / 4 + InstrLineStart;
			const float PosY = -(i * RunDistance + MiddleOfRunY);


			const float PosZ2D = 26.0f;

			// write all branchConditions of the steps inside here -> will be used to spawn branchInstr in 2D 
			TArray<bool> AllBranchConditions;

			// to avoid spawning e.g. jump instructions at the same depth (might be in loop where nothing changes with steps)
			TArray<int32> SpawnedDepths = {};
			for (int k = 0; k < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); k++)
			{
				const int32 Depth = TraceInstrStructure.runs.run[i].instruction[j].step[k].depth;
				const bool BranchCondition = TraceInstrStructure.runs.run[i].instruction[j].step[k].condition;

				// get symbolic behaviour materialInterface
				UMaterialInterface** SymBehMaterialVal = SymBehMaterialTable.Find(
					TraceInstrStructure.runs.run[i].instruction[j].step[k].beh);
				UMaterialInterface* SymBehMaterial = SymBehNoneMaterial;
				if (SymBehMaterialVal)
				{
					SymBehMaterial = *SymBehMaterialVal;
				}


				// calculate maxDepth (e.g. for scaling runAnimationActors)
				if (Depth > TraceInstrStructure.analysis.max_depth)
				{
					TraceInstrStructure.analysis.max_depth = Depth;
				}

				// to avoid spawning e.g. jump instructions at the same depth (might be in loop where nothing changes with steps)
				if (SpawnedDepths.Contains(Depth))
				{
					continue;
				}
				SpawnedDepths.Emplace(Depth);

				const float PosZ = 26.0f + 15.0f * Depth;
				FVector Position(PosX, PosY, PosZ);

				// try to create instruction actor in 3D
				FTransform SpawnTransform(Rotation, Position);
				TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionActor3D = Cast<AInstructionActorST>(
					UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionActor, SpawnTransform)
				);

				// init and spawn instruction actor in 3D
				if (TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionActor3D != nullptr)
				{
					EBranchTypes BranchType = BranchCondition ? EBranchTypes::BranchTrue : EBranchTypes::BranchFalse;
					int NewStepId = TraceInstrStructure.runs.run[i].instruction[j].step[k].id;
					TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionActor3D->Init(
						InstrType, SymBehMaterial, Position, Scale, i, j, k, NewStepId, BranchType);

					UGameplayStatics::FinishSpawningActor(
						TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionActor3D,
						SpawnTransform);
				}

				// create instruction textActor
				if (k == 0)
				{
					FVector TextPosition(PosX, PosY, 20.5f);
					FTransform TextSpawnTransform(Rotation, TextPosition);
					// try to create InstructionTextActor
					TraceInstrStructure.runs.run[i].InstructionTextActors.Emplace(Cast<AInstructionTextActorST>(
						UGameplayStatics::BeginDeferredActorSpawnFromClass(
							this, InstructionTextActor, TextSpawnTransform)
					));

					// init and spawn textActor
					if (TraceInstrStructure.runs.run[i].InstructionTextActors.Last() != nullptr)
					{
						FString OpCodeText = TraceInstrStructure.runs.run[i].instruction[j].opcode;

						TraceInstrStructure.runs.run[i].InstructionTextActors.Last()->Init(OpCodeText);

						UGameplayStatics::FinishSpawningActor(
							TraceInstrStructure.runs.run[i].InstructionTextActors.Last(),
							TextSpawnTransform);
					}
				}

				// create instructionSplineActor in 3D
				if (InstrType == EInstructionType::Branch || InstrType == EInstructionType::Jump)
				{
					// how often this spline got executed (step with same depth)
					int ExecutionNr = 0;
					for (int x = 0; x < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); x++)
					{
						if (InstrType == EInstructionType::Jump)
						{
							if (TraceInstrStructure.runs.run[i].instruction[j].step[k].depth == TraceInstrStructure.runs
								.run[i].instruction[j].step[x].depth)
							{
								ExecutionNr += 1;
							}
						}
						else if (InstrType == EInstructionType::Branch)
						{
							if (TraceInstrStructure.runs.run[i].instruction[j].step[k].depth == TraceInstrStructure.runs
								.run[i].instruction[j].step[x].depth
								&& TraceInstrStructure.runs.run[i].instruction[j].step[k].condition ==
								TraceInstrStructure.runs.run[i].instruction[j].step[x].condition)
							{
								ExecutionNr += 1;
							}
						}
					}

					const int Target = TraceInstrStructure.runs.run[i].instruction[j].step[k].target;
					const float TargetPosX = (Target - MinPc) * BlockDistance / 4 + InstrLineStart;

					const float MiddlePosX = (PosX + TargetPosX) / 2.0f;
					const float MiddlePosY = PosY - 0.3f * (Target - InstrPC);
					FVector StartPosition(PosX, PosY, PosZ);
					FVector EndPosition(TargetPosX, PosY, PosZ);
					FVector MiddlePosition(MiddlePosX, MiddlePosY, PosZ);

					// try to create instructionSplineActor 3D
					FTransform SplineSpawnTransform(Rotation, StartPosition);
					TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionSplineActor3D = Cast<
						AInstructionSplineActorST>(
						UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionSplineActor,
						                                                   SplineSpawnTransform)
					);

					// setting type of branch
					EBranchTypes BranchType = EBranchTypes::BranchNone;
					if (InstrType == EInstructionType::Branch)
					{
						BranchType = BranchCondition ? EBranchTypes::BranchTrue : EBranchTypes::BranchFalse;
					}
					// init and spawn instructionSplineActor 3D
					if (TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionSplineActor3D != nullptr)
					{
						TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionSplineActor3D->Init(
							StartPosition, MiddlePosition, EndPosition, ExecutionNr, BranchType);

						UGameplayStatics::FinishSpawningActor(
							TraceInstrStructure.runs.run[i].instruction[j].step[k].InstructionSplineActor3D,
							SplineSpawnTransform);
					}
				}


				// create instructionSplineActor in 2D -> JumpInstruction
				if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D == nullptr &&
					InstrType == EInstructionType::Jump)
				{
					// number of steps this jump is involved in -> for thickness of spline
					const int ExecutionNr = TraceInstrStructure.runs.run[i].instruction[j].step.Num();
					const int Target = TraceInstrStructure.runs.run[i].instruction[j].step[k].target;
					const float TargetPosX = (Target - MinPc) * BlockDistance / 4 + InstrLineStart;

					const float MiddlePosX = (PosX + TargetPosX) / 2.0f;
					const float MiddlePosY = PosY - 0.3f * (Target - InstrPC);
					FVector StartPosition(PosX, PosY, PosZ2D);
					FVector EndPosition(TargetPosX, PosY, PosZ2D);
					FVector MiddlePosition(MiddlePosX, MiddlePosY, PosZ2D);


					// try to create instructionSplineActor True 2D
					FTransform SplineSpawnTransform(Rotation, StartPosition);
					TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D = Cast<
						AInstructionSplineActorST>(
						UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionSplineActor,
						                                                   SplineSpawnTransform)
					);

					// init and spawn instructionSplineActor True 2D
					if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D != nullptr)
					{
						TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D->Init(
							StartPosition, MiddlePosition, EndPosition, ExecutionNr, EBranchTypes::BranchNone);

						UGameplayStatics::FinishSpawningActor(
							TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D,
							SplineSpawnTransform);
					}
				}

				// create instructionSplineActor in 2D -> BranchInstruction
				if (InstrType == EInstructionType::Branch)
				{
					AllBranchConditions.Emplace(BranchCondition);
					// true branch
					if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D == nullptr &&
						BranchCondition)
					{
						// number of steps this true branch got executed
						int ExecutionNr = 0;
						for (int x = 0; x < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); x++)
						{
							if (TraceInstrStructure.runs.run[i].instruction[j].step[x].condition == true)
							{
								ExecutionNr += 1;
							}
						}

						const int Target = TraceInstrStructure.runs.run[i].instruction[j].step[k].target;
						const float TargetPosX = (Target - MinPc) * BlockDistance / 4 + InstrLineStart;

						const float MiddlePosX = (PosX + TargetPosX) / 2.0f;
						const float MiddlePosY = PosY - 0.3f * (Target - InstrPC);
						FVector StartPosition(PosX, PosY, PosZ2D);
						FVector EndPosition(TargetPosX, PosY, PosZ2D);
						FVector MiddlePosition(MiddlePosX, MiddlePosY, PosZ2D);
						FVector SplineScale(0.01f);

						// try to create instructionSplineActor True 2D
						FTransform SplineSpawnTransform(Rotation, StartPosition);
						TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D = Cast<
							AInstructionSplineActorST>(
							UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionSplineActor,
							                                                   SplineSpawnTransform)
						);

						// init and spawn instructionSplineActor True 2D
						if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D != nullptr)
						{
							TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D->Init(
								StartPosition, MiddlePosition, EndPosition, ExecutionNr, EBranchTypes::BranchTrue);

							UGameplayStatics::FinishSpawningActor(
								TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorTrue2D,
								SplineSpawnTransform);
						}
					}

					// false branch
					if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D == nullptr && !
						BranchCondition)
					{
						// number of steps this true branch got executed
						int ExecutionNr = 0;
						for (int x = 0; x < TraceInstrStructure.runs.run[i].instruction[j].step.Num(); x++)
						{
							if (TraceInstrStructure.runs.run[i].instruction[j].step[x].condition == false)
							{
								ExecutionNr += 1;
							}
						}

						const int Target = TraceInstrStructure.runs.run[i].instruction[j].step[k].target;
						const float TargetPosX = (Target - MinPc) * BlockDistance / 4 + InstrLineStart;

						const float MiddlePosX = (PosX + TargetPosX) / 2.0f;
						const float MiddlePosY = PosY - 0.3f * (Target - InstrPC);
						FVector StartPosition(PosX, PosY, PosZ2D);
						FVector EndPosition(TargetPosX, PosY, PosZ2D);
						FVector MiddlePosition(MiddlePosX, MiddlePosY, PosZ2D);
						FVector SplineScale(0.01f);

						// try to create instructionSplineActor False 2D
						FTransform SplineSpawnTransform(Rotation, StartPosition);
						TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D = Cast<
							AInstructionSplineActorST>(
							UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionSplineActor,
							                                                   SplineSpawnTransform)
						);

						// init and spawn instructionSplineActor False 2D
						if (TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D != nullptr)
						{
							TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D->Init(
								StartPosition, MiddlePosition, EndPosition, ExecutionNr, EBranchTypes::BranchFalse);

							UGameplayStatics::FinishSpawningActor(
								TraceInstrStructure.runs.run[i].instruction[j].InstructionSplineActorFalse2D,
								SplineSpawnTransform);
						}
					}
				}
			}


			FVector Position2D(PosX, PosY, PosZ2D);
			// try to create instructionActor in 2D
			FTransform SpawnTransform2D(Rotation, Position2D);
			TraceInstrStructure.runs.run[i].instruction[j].InstructionActor2D = Cast<AInstructionActorST>(
				UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionActor, SpawnTransform2D)
			);

			// get symbolic behaviour materialInterface
			UMaterialInterface** SymBehMaterialVal = SymBehMaterialTable.Find(
				TraceInstrStructure.runs.run[i].instruction[j].step[0].beh);
			UMaterialInterface* SymBehMaterial = SymBehNoneMaterial;
			if (SymBehMaterialVal)
			{
				SymBehMaterial = *SymBehMaterialVal;
			}

			// init and spawn instruction actor in 2D
			if (TraceInstrStructure.runs.run[i].instruction[j].InstructionActor2D != nullptr)
			{
				EBranchTypes BranchType = EBranchTypes::BranchBoth;
				if (AllBranchConditions.Contains(true) && !AllBranchConditions.Contains(false))
				{
					BranchType = EBranchTypes::BranchTrue;
				}
				if (AllBranchConditions.Contains(false) && !AllBranchConditions.Contains(true))
				{
					BranchType = EBranchTypes::BranchFalse;
				}


				TraceInstrStructure.runs.run[i].instruction[j].InstructionActor2D->Init(
					InstrType, SymBehMaterial, Position2D, Scale, i, j, -1, -1, BranchType);

				UGameplayStatics::FinishSpawningActor(
					TraceInstrStructure.runs.run[i].instruction[j].InstructionActor2D,
					SpawnTransform2D);
			}
		}
	}
}

void ARTSPlayerPawnST::SpawnPathSplineObjects()
{
	const int MinPc = TraceInstrStructure.analysis.min_pc;
	const float PosZ2D = 26.0f;
	const FRotator Rotation(0.0f);
	const FVector SplineScale(0.12f);

	for (int i = 0; i < TraceInstrStructure.runs.run.Num(); i++)
	{
		const int ParentPathInd = TraceInstrStructure.runs.run[i].parent;
		if (ParentPathInd < 0 || TraceInstrStructure.runs.run[i].instruction.IsEmpty() || TraceInstrStructure.runs.run[
			i].instruction[0].step.IsEmpty())
		{
			continue;
		}

		const int StartStepPc = TraceInstrStructure.runs.run[i].instruction[0].pc;
		const int StartStepId = TraceInstrStructure.runs.run[i].instruction[0].step[0].id;

		int ParentStartStepPC = -1;

		if (ParentPathInd < TraceInstrStructure.runs.run.Num())
		{
			// search parentRun for startStepID -1
			for (int j = 0; j < TraceInstrStructure.runs.run[ParentPathInd].instruction.Num(); j++)
			{
				if (ParentStartStepPC != -1)
				{
					break;
				}
				for (int k = 0; k < TraceInstrStructure.runs.run[ParentPathInd].instruction[j].step.Num(); k++)
				{
					if (TraceInstrStructure.runs.run[ParentPathInd].instruction[j].step[k].id == StartStepId - 1)
					{
						ParentStartStepPC = TraceInstrStructure.runs.run[ParentPathInd].instruction[j].pc;
						break;
					}
				}
			}
		}

		// parentStep in parentRun could not be found
		if (ParentStartStepPC == -1)
		{
			return;
		}

		// calculate positions
		const int EndPositionX = (StartStepPc - MinPc) * BlockDistance / 4 + InstrLineStart;
		const float EndPositionY = -(i * RunDistance + MiddleOfRunY);

		const int StartPositionX = (ParentStartStepPC - MinPc) * BlockDistance / 4 + InstrLineStart;
		const float StartPositionY = -(ParentPathInd * RunDistance + MiddleOfRunY);

		FVector StartPosition(StartPositionX, StartPositionY - 15.0f, PosZ2D);
		FVector EndPosition(EndPositionX, EndPositionY + 60.0f, PosZ2D);

		// try to create pathSplineActor
		FTransform SplineSpawnTransform(Rotation, StartPosition);
		TraceInstrStructure.runs.run[i].PathSplineActor = Cast<
			APathSplineActorST>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, PathSplineActor,
			                                                   SplineSpawnTransform)
		);

		// init and spawn instructionSplineActor True 2D
		if (TraceInstrStructure.runs.run[i].PathSplineActor != nullptr)
		{
			TraceInstrStructure.runs.run[i].PathSplineActor->Init(
				StartPosition, EndPosition, SplineScale);

			UGameplayStatics::FinishSpawningActor(
				TraceInstrStructure.runs.run[i].PathSplineActor,
				SplineSpawnTransform);
		}
	}
}

void ARTSPlayerPawnST::SetFollowAnimationWithCamera(bool Follow)
{
	FollowAnimationWithCamera = Follow;
}

void ARTSPlayerPawnST::JumpAnimationToEnd(int AnimationRunIndex)
{
	if (RunAnimationRunning)
	{
		return;
	}

	CurrentRunAnimationIndex = AnimationRunIndex;

	if (CurrentRunAnimationIndex < 0)
	{
		// TODO: set variables to animate whole run
		return;
	}
	else if (CurrentRunAnimationIndex < TraceInstrStructure.trace_animation.run_animations.Num())
	{
		CurrentRunAnimationActor = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			RunAnimationActor;
		if (CurrentRunAnimationActor == nullptr)
		{
			return;
		}
		int AnimationStepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
		                         animation_steps.Num() - 1;
		TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].CurrentStepAnimationIndex =
			AnimationStepIndex;

		// get all the indices with current runAnimationIndex and AnimationStepIndex
		const int RunIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].run_index;
		const int InstructionIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].instruction_index;
		const int StepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].step_index;
		const int StepId = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].step_id;

		// setting all symbolic materials according to current step
		ResetSymMaterials(AnimationRunIndex, AnimationStepIndex);

		// now teleport animationActor to right position and mark instructionActor
		if (RunIndex > -1 && InstructionIndex > -1 && StepIndex > -1 && RunIndex < TraceInstrStructure.runs.run.
			Num() &&
			InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num() &&
			StepIndex < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num())
		{
			const AInstructionActorST* InstrActor = TraceInstrStructure.runs.run[RunIndex].instruction[
				InstructionIndex].InstructionActor2D;
			if (InstrActor != nullptr)
			{
				const FVector TargetPosition = InstrActor->GetActorLocation();
				CurrentRunAnimationActor->TeleportToTarget(
					FVector(TargetPosition.X, TargetPosition.Y, 20.0f));

				// mark the current instructionActor in Scene and HUD
				MarkInstructionActor(RunIndex, InstructionIndex, StepIndex, StepId);

				// set camera position
				if (FollowAnimationWithCamera)
				{
					const FVector NewCameraPosition(TargetPosition.X, TargetPosition.Y, 0.0f);
					TargetLocation = NewCameraPosition;
				}
			}
		}
	}
}


void ARTSPlayerPawnST::ResetAnimation(int AnimationRunIndex)
{
	if (RunAnimationRunning)
	{
		return;
	}

	CurrentRunAnimationIndex = AnimationRunIndex;

	if (CurrentRunAnimationIndex < 0)
	{
		// TODO: set variables to animate whole run
		return;
	}
	else if (CurrentRunAnimationIndex < TraceInstrStructure.trace_animation.run_animations.Num())
	{
		CurrentRunAnimationActor = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			RunAnimationActor;
		if (CurrentRunAnimationActor == nullptr)
		{
			return;
		}
		TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].CurrentStepAnimationIndex = 0;

		// get all the indices with current runAnimationIndex and AnimationStepIndex
		const int RunIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				0].run_index;
		const int InstructionIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				0].instruction_index;
		const int StepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				0].step_index;
		const int StepId = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				0].step_id;

		// setting all symbolic materials according to current step
		ResetSymMaterials(AnimationRunIndex, 0);

		// now teleport animationActor to right position and mark instructionActor
		if (RunIndex > -1 && InstructionIndex > -1 && StepIndex > -1 && RunIndex < TraceInstrStructure.runs.run.
			Num() &&
			InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num() &&
			StepIndex < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num())
		{
			const AInstructionActorST* InstrActor = TraceInstrStructure.runs.run[RunIndex].instruction[
				InstructionIndex].InstructionActor2D;
			if (InstrActor != nullptr)
			{
				const FVector TargetPosition = InstrActor->GetActorLocation();
				CurrentRunAnimationActor->TeleportToTarget(
					FVector(TargetPosition.X, TargetPosition.Y, 20.0f));

				// mark the current instructionActor in Scene and HUD
				MarkInstructionActor(RunIndex, InstructionIndex, StepIndex, StepId);

				// set camera position
				if (FollowAnimationWithCamera)
				{
					const FVector NewCameraPosition(TargetPosition.X, TargetPosition.Y, 0.0f);
					TargetLocation = NewCameraPosition;
				}
			}
		}
	}
}

void ARTSPlayerPawnST::ResetSymMaterials(int AnimationRunIndex, int AnimationStepIndex)
{
	if (AnimationRunIndex > -1 && AnimationRunIndex < TraceInstrStructure.trace_animation.run_animations.Num())
	{
		// completely reset all materials as if animation starts from 0
		TArray<int> OccuredPcs;

		for (int i = 0; i < TraceInstrStructure.trace_animation.run_animations[AnimationRunIndex].animation_steps.Num();
		     i++)
		{
			const int RunIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					i].run_index;
			const int InstructionIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					i].instruction_index;
			const int StepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					i].step_index;

			if (RunIndex > -1 && InstructionIndex > -1 && StepIndex > -1 && RunIndex < TraceInstrStructure.runs.run.
				Num() &&
				InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num() &&
				StepIndex < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num())
			{
				const int Pc = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].pc;
				AInstructionActorST* CurrentInstructionActor2D = nullptr;
				AInstructionActorST* CurrentInstructionActor3D = nullptr;
				if (OccuredPcs.Contains(Pc))
				{
					continue;
				}
				else
				{
					OccuredPcs.Emplace(Pc);
				}

				// get symbolic behaviour materialInterface for this step
				UMaterialInterface** SymBehMaterialVal = SymBehMaterialTable.Find(
					TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].beh);
				UMaterialInterface* SymBehMaterial = SymBehNoneMaterial;
				if (SymBehMaterialVal)
				{
					SymBehMaterial = *SymBehMaterialVal;
				}

				// get 2D instructionActor

				CurrentInstructionActor2D = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].
					InstructionActor2D;

				// get 3D instructionActor				
				// get the actor with same depth closest to listStart (that one actually got spawned)
				int Depth = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].depth;
				for (int j = 0; i < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num(); j
				     ++)
				{
					if (Depth == TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[j].depth)
					{
						CurrentInstructionActor3D = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex]
							.step[j].
							InstructionActor3D;
						break;
					}
				}


				if (CurrentInstructionActor2D != nullptr)
				{
					// set the symbolic behaviour material
					CurrentInstructionActor2D->ChangeMaterial(SymBehMaterial);
				}
				if (CurrentInstructionActor3D != nullptr)
				{
					// set the symbolic behaviour material
					CurrentInstructionActor3D->ChangeMaterial(SymBehMaterial);
				}
			}
		}

		// now set all materials that changed previous from the stepIndex
		if (AnimationStepIndex >= TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
		                          animation_steps.Num())
		{
			return;
		}

		OccuredPcs = {};
		for (int i = AnimationStepIndex; i > -1; i--)
		{
			const int RunIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					i].run_index;
			const int InstructionIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					i].instruction_index;
			const int StepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[
					i].step_index;

			if (RunIndex > -1 && InstructionIndex > -1 && StepIndex > -1 && RunIndex < TraceInstrStructure.runs.run.
				Num() &&
				InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num() &&
				StepIndex < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num())
			{
				const int Pc = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].pc;
				AInstructionActorST* CurrentInstructionActor2D = nullptr;
				AInstructionActorST* CurrentInstructionActor3D = nullptr;
				if (OccuredPcs.Contains(Pc))
				{
					continue;
				}
				else
				{
					OccuredPcs.Emplace(Pc);
				}

				// get symbolic behaviour materialInterface for this step
				UMaterialInterface** SymBehMaterialVal = SymBehMaterialTable.Find(
					TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].beh);
				UMaterialInterface* SymBehMaterial = SymBehNoneMaterial;
				if (SymBehMaterialVal)
				{
					SymBehMaterial = *SymBehMaterialVal;
				}

				// get 2D instructionActor
				CurrentInstructionActor2D = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].
					InstructionActor2D;

				// get 3D instructionActor
				// get the actor with same depth closest to listStart (that one actually got spawned)
				int Depth = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].depth;
				for (int j = 0; i < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num(); j
				     ++)
				{
					if (Depth == TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[j].depth)
					{
						CurrentInstructionActor3D = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex]
							.step[j].
							InstructionActor3D;
						break;
					}
				}


				if (CurrentInstructionActor2D != nullptr)
				{
					// set the symbolic behaviour material
					CurrentInstructionActor2D->ChangeMaterial(SymBehMaterial);
				}

				if (CurrentInstructionActor3D != nullptr)
				{
					// set the symbolic behaviour material
					CurrentInstructionActor3D->ChangeMaterial(SymBehMaterial);
				}
			}
		}
	}
}

void ARTSPlayerPawnST::SetAnimationStart(int AnimationRunIndex, int AnimationStepId)
{
	if (RunAnimationRunning)
	{
		return;
	}

	CurrentRunAnimationIndex = AnimationRunIndex;

	if (CurrentRunAnimationIndex < 0)
	{
		// TODO: set variables to animate whole run
		return;
	}
	else if (CurrentRunAnimationIndex < TraceInstrStructure.trace_animation.run_animations.Num())
	{
		CurrentRunAnimationActor = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			RunAnimationActor;
		if (CurrentRunAnimationActor == nullptr)
		{
			return;
		}

		// get animationStepIndex with stepId -> set to 0 if not found 
		int AnimationStepIndex = 0;
		for (int i = 0; i < TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].animation_steps
		     .Num(); i++)
		{
			if (AnimationStepId == TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
				animation_steps[i].step_id)
			{
				AnimationStepIndex = i;
				break;
			}
		}
		TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].CurrentStepAnimationIndex =
			AnimationStepIndex;


		// setting all symbolic materials according to current step
		ResetSymMaterials(AnimationRunIndex, AnimationStepIndex);


		// get all the indices with current runAnimationIndex and AnimationStepIndex
		const int RunIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].run_index;
		const int InstructionIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].instruction_index;
		const int StepIndex = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].step_index;
		const int StepId = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			animation_steps[
				AnimationStepIndex].step_id;

		// now teleport animationActor to right position and mark instructionActor
		if (RunIndex > -1 && InstructionIndex > -1 && StepIndex > -1 && RunIndex < TraceInstrStructure.runs.run.
			Num() &&
			InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num() &&
			StepIndex < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num())
		{
			const AInstructionActorST* InstrActor = TraceInstrStructure.runs.run[RunIndex].instruction[
				InstructionIndex].InstructionActor2D;
			if (InstrActor != nullptr)
			{
				const FVector TargetPosition = InstrActor->GetActorLocation();
				CurrentRunAnimationActor->TeleportToTarget(
					FVector(TargetPosition.X, TargetPosition.Y, 20.0f));

				// mark the current instructionActor in Scene and HUD
				MarkInstructionActor(RunIndex, InstructionIndex, StepIndex, StepId);


				// set camera position
				if (FollowAnimationWithCamera)
				{
					const FVector NewCameraPosition(TargetPosition.X, TargetPosition.Y, 0.0f);
					TargetLocation = NewCameraPosition;
				}
			}
		}
	}
}

void ARTSPlayerPawnST::MarkInstructionActor(int RunIndex, int InstructionIndex, int StepIndex, int ScrollStepId)
{
	// these have all been checked before
	ClickedRunIndex = RunIndex;
	ClickedInstructionIndex = InstructionIndex;
	ClickedStepIndex = -1;
	ClickedStepId = ScrollStepId;

	UE_LOG(STLog, Warning, TEXT("markInstructionActor called RunInd: %d, InstrInd: %d, StepInd: %d"), RunIndex,
	       InstructionIndex,
	       StepIndex);

	// unmark instructionActor
	if (MarkedInstructionActor != nullptr)
	{
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(
			MarkedInstructionActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		if (StaticMesh != nullptr)
		{
			StaticMesh->SetRenderCustomDepth(false);
		}
	}

	// get symbolic behaviour materialInterface for this step
	UMaterialInterface** SymBehMaterialVal = SymBehMaterialTable.Find(
		TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].beh);
	UMaterialInterface* SymBehMaterial = SymBehNoneMaterial;
	if (SymBehMaterialVal)
	{
		SymBehMaterial = *SymBehMaterialVal;
	}

	// get 2D instructionActor
	if (AbstractionLevel < EAbstractionLevel::Level4)
	{
		MarkedInstructionActor = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].
			InstructionActor2D;
	} // get 3D instructionActor
	else
	{
		// get the actor with same depth closest to listStart (that one actually got spawned)
		int Depth = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].depth;
		for (int i = 0; i < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num(); i++)
		{
			if (Depth == TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[i].depth)
			{
				MarkedInstructionActor = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[i].
					InstructionActor3D;
				break;
			}
		}
	}

	// just for changing symbolic behaviour material for both -> regardless of abstraction level
	AInstructionActorST* OtherMarkedInstructionActor = nullptr;
	if (AbstractionLevel >= EAbstractionLevel::Level4)
	{
		OtherMarkedInstructionActor = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].
			InstructionActor2D;
	} // get 3D instructionActor
	else
	{
		// get the actor with same depth closest to listStart (that one actually got spawned)
		int Depth = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[StepIndex].depth;
		for (int i = 0; i < TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step.Num(); i++)
		{
			if (Depth == TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[i].depth)
			{
				OtherMarkedInstructionActor = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].step[i].
					InstructionActor3D;
				break;
			}
		}
	}
	if(OtherMarkedInstructionActor != nullptr)
	{
		// set the symbolic behaviour material
		OtherMarkedInstructionActor->ChangeMaterial(SymBehMaterial);
	}
	

	if (MarkedInstructionActor != nullptr)
	{
		// set the symbolic behaviour material
		MarkedInstructionActor->ChangeMaterial(SymBehMaterial);

		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(
			MarkedInstructionActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		// outline the selected mesh
		if (StaticMesh != nullptr)
		{
			StaticMesh->SetRenderCustomDepth(true);
		}
	}


	// let others know that clicked indices have changed (e.g. the Hud)
	OnRunInstrStepIndexChanged.Broadcast(ClickedRunIndex, ClickedInstructionIndex, ClickedStepIndex, ClickedStepId);
}

void ARTSPlayerPawnST::StopRunAnimation()
{
	RunAnimationRunning = false;
}

void ARTSPlayerPawnST::AnimateRun(int AnimationRunIndex)
{
	CurrentRunAnimationIndex = AnimationRunIndex;

	if (CurrentRunAnimationIndex < 0)
	{
		// TODO: set variables to animate whole run
	}
	else if (CurrentRunAnimationIndex < TraceInstrStructure.trace_animation.run_animations.Num())
	{
		CurrentRunAnimationActor = TraceInstrStructure.trace_animation.run_animations[CurrentRunAnimationIndex].
			RunAnimationActor;
		if (CurrentRunAnimationActor != nullptr && TraceInstrStructure.trace_animation.run_animations[
			CurrentRunAnimationIndex].animation_steps.Num() != 0)
		{
			RunAnimationRunning = true;
			return;
		}
	}

	RunAnimationRunning = false;
}


void ARTSPlayerPawnST::SpawnRunAnimationObjects()
{
	const FRotator Rotation(0.0f);
	const int32 MinPc = TraceInstrStructure.analysis.min_pc;
	const float PosZ = 20.0f;
	const int32 MaxDepth = TraceInstrStructure.analysis.max_depth <= 0 ? 1 : TraceInstrStructure.analysis.max_depth;

	for (int i = 0; i < TraceInstrStructure.trace_animation.run_animations.Num(); i++)
	{
		if (TraceInstrStructure.trace_animation.run_animations[i].animation_steps.Num() > 0)
		{
			const int32 RunIndex = TraceInstrStructure.trace_animation.run_animations[i].animation_steps[0].run_index;
			const int32 InstructionIndex = TraceInstrStructure.trace_animation.run_animations[i].animation_steps[0].
				instruction_index;

			if (RunIndex > -1 && InstructionIndex > -1 && RunIndex < TraceInstrStructure.runs.run.Num() &&
				InstructionIndex < TraceInstrStructure.runs.run[RunIndex].instruction.Num())
			{
				const int Pc = TraceInstrStructure.runs.run[RunIndex].instruction[InstructionIndex].pc;
				if (Pc > -1)
				{
					const float PosX = (Pc - MinPc) * BlockDistance / 4 + InstrLineStart;
					const float PosY = -(RunIndex * RunDistance + MiddleOfRunY);


					FVector Position(PosX, PosY, PosZ);

					// try to create runAnimationObject
					FTransform SpawnTransform(Rotation, Position);
					TraceInstrStructure.trace_animation.run_animations[i].RunAnimationActor = Cast<
						ARunAnimationActorST>(
						UGameplayStatics::BeginDeferredActorSpawnFromClass(this, RunAnimationActor, SpawnTransform)
					);

					// init and spawn runAnimationObject
					if (TraceInstrStructure.trace_animation.run_animations[i].RunAnimationActor != nullptr)
					{
						TraceInstrStructure.trace_animation.run_animations[i].RunAnimationActor->Init(MaxDepth);

						UGameplayStatics::FinishSpawningActor(
							TraceInstrStructure.trace_animation.run_animations[i].RunAnimationActor,
							SpawnTransform);
					}
				}
			}
		}
	}
}


void ARTSPlayerPawnST::SpawnInstrLineObjects()
{
	UE_LOG(STLog, Warning, TEXT("In SpawnInstrLineObjects:"));
	FVector Origin(InstrLineStart, 100.0f, 20.0f);
	const FRotator Rotation(0.0);

	int minPc = TraceInstrStructure.analysis.min_pc;
	int maxPc = TraceInstrStructure.analysis.max_pc;
	const int NumRuns = TraceInstrStructure.analysis.num_runs;
	float ScaleY = (NumRuns * RunDistance - (RunDistance - NewBlockSizeY)) / NewBlockSizeY;
	FVector ScaleMiddleLine(0.068646f, ScaleY, 1.0f);
	FVector ScaleUpperLowerLine(0.007f, ScaleY, 1.0f);
	int traceInstrIndex = 0;


	UE_LOG(STLog, Warning, TEXT("SpawnInstrLineObjects: minPc %i, maxPc %i"), minPc, maxPc);

	for (int i = minPc; i <= maxPc; i += 4)
	{
		// create instruction line actor
		FTransform SpawnTransform(Rotation, Origin);
		TraceInstrStructure.InstructionLineActors.Emplace(Cast<AInstructionLineActorST>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InstructionLineActor, SpawnTransform)
		));

		// init and spawn actor
		if (TraceInstrStructure.InstructionLineActors[traceInstrIndex] != nullptr)
		{
			FString hexStr = ToHexString(i);
			TraceInstrStructure.InstructionLineActors[traceInstrIndex]->Init(
				hexStr, ScaleMiddleLine, ScaleUpperLowerLine);

			UGameplayStatics::FinishSpawningActor(TraceInstrStructure.InstructionLineActors[traceInstrIndex],
			                                      SpawnTransform);
		}
		traceInstrIndex += 1;
		Origin.X += BlockDistance;
	}
}

void ARTSPlayerPawnST::SpawnFunctionBlkTextObjects()
{
	UE_LOG(STLog, Warning, TEXT("In SpawnFunctionBlkTextObjects:"));

	const int NumRuns = TraceInstrStructure.analysis.num_runs;
	const int MinPc = TraceInstrStructure.analysis.min_pc;
	float FunctionScaleY = (NumRuns * RunDistance - (RunDistance - NewBlockSizeY)) / NewBlockSizeY;
	const FRotator Rotation(0.0);
	TArray<FaFunction> AllFunctions;

	for (int i = 0; i < TraceBlkStructure.code_files.Num(); i++)
	{
		for (int j = 0; j < TraceBlkStructure.code_files[i].functions.Num(); j++)
		{
			AllFunctions.Emplace(TraceBlkStructure.code_files[i].functions[j]);
		}
	}
	AllFunctions.Sort();

	bool Material = true;
	for (int i = 0; i < AllFunctions.Num(); i++)
	{
		// spawn functionBlocks
		const int FunctionStart = AllFunctions[i].start;
		const int FunctionEnd = AllFunctions[i].end;
		float PosX = (FunctionStart - MinPc) * BlockDistance / 4;
		float PosXEnd = (FunctionEnd - MinPc) * BlockDistance / 4 + NewBlockSize;
		float ScaleX = (PosXEnd - PosX) / NewBlockSize;

		FVector Position(PosX, 0, -10);
		FVector Scale(ScaleX * BlockScale, FunctionScaleY, BlockScale);

		// try to create functionBlockActor
		FTransform SpawnTransform(Rotation, Position);
		TraceBlkStructure.FunctionBlockActors.Emplace(Cast<AFunctionBlockActorST>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, FunctionBlockActor, SpawnTransform)
		));

		if (TraceBlkStructure.FunctionBlockActors[i] != nullptr)
		{
			TraceBlkStructure.FunctionBlockActors[i]->Init(Scale, Material);
			UGameplayStatics::FinishSpawningActor(TraceBlkStructure.FunctionBlockActors[i],
			                                      SpawnTransform);
			// TODO: what about finishspawningActor?
		}

		// spawn functionTexts

		const int FunctionMiddle = (FunctionStart + FunctionEnd) / 2;
		float PosTextX = (FunctionMiddle - MinPc) * BlockDistance / 4;
		FString FunctionName = AllFunctions[i].name;
		FVector PositionText(PosTextX, 150, 20);

		// try to create functionTextActor
		FTransform SpawnTransformText(Rotation, PositionText);
		TraceBlkStructure.FunctionTextActors.Emplace(Cast<AFunctionTextActorST>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, FunctionTextActor, SpawnTransformText)
		));

		if (TraceBlkStructure.FunctionTextActors[i] != nullptr)
		{
			TraceBlkStructure.FunctionTextActors[i]->Init(FunctionName, Material);
			UGameplayStatics::FinishSpawningActor(TraceBlkStructure.FunctionTextActors[i],
			                                      SpawnTransformText);
		}

		Material = !Material;
	}
}

void ARTSPlayerPawnST::SpawnPathTextObjects()
{
	UE_LOG(STLog, Warning, TEXT("In SpawnPathtextObjects:"));
	FVector Position(-30.0f, -MiddleOfRunY, 5);
	const FRotator Rotation(0.0);
	const int NumPaths = TraceInstrStructure.analysis.num_runs;

	for (int i = 0; i < NumPaths; i++)
	{
		Position.Y = -(i * RunDistance + MiddleOfRunY);
		// try to create pathTextActor
		FTransform SpawnTransform(Rotation, Position);
		TraceInstrStructure.PathTextActors.Emplace(Cast<APathTextActorST>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, PathTextActor, SpawnTransform)
		));

		// Init and spawn actor
		if (TraceInstrStructure.PathTextActors[i] != nullptr)
		{
			TraceInstrStructure.PathTextActors[i]->Init(i);

			UGameplayStatics::FinishSpawningActor(TraceInstrStructure.PathTextActors[i],
			                                      SpawnTransform);
		}
	}
}

void ARTSPlayerPawnST::SpawnBlkObjects()
{
	UE_LOG(STLog, Warning, TEXT("In SpawnBlkObjects:"));
	const FRotator Rotation(0.0);

	const int MinPc = TraceInstrStructure.analysis.min_pc;


	for (int i = 0; i < TraceBlkStructure.run_blocks.Num(); i++)
	{
		TArray<int32> SpawnedBlocks;
		for (int j = 0; j < TraceBlkStructure.run_blocks[i].block.Num(); j++)
		{
			int BlockId = TraceBlkStructure.run_blocks[i].block[j].block_id;
			if (SpawnedBlocks.Contains(BlockId))
			{
				continue;
			}
			SpawnedBlocks.Add(BlockId);
			const int BlockStart = TraceBlkStructure.cfg_blocks[BlockId].block_start;
			const int BlockEnd = TraceBlkStructure.cfg_blocks[BlockId].block_end;
			const float PosX = (BlockStart - MinPc) * BlockDistance / 4;
			const float PosXEnd = (BlockEnd - MinPc) * BlockDistance / 4 + NewBlockSize;
			const float ScaleX = (PosXEnd - PosX) / NewBlockSize;
			FVector Position(PosX, i * -RunDistance, 5);
			const FVector Scale(ScaleX * BlockScale, BlockScaleY, BlockScale);

			// we need to get the number of times this codeBlock got ran through
			int Count = -1;
			int RunIndex = i;

			while (Count == -1 && RunIndex > -1 && RunIndex < TraceInstrStructure.runs.run.Num())
			{
				for (int x = 0; x < TraceInstrStructure.runs.run[RunIndex].instruction.Num(); x++)
				{
					if (TraceInstrStructure.runs.run[RunIndex].instruction[x].pc >= BlockStart && TraceInstrStructure.
						runs.run[RunIndex].instruction[x].pc <= BlockEnd)
					{
						if (Count < TraceInstrStructure.runs.run[RunIndex].instruction[x].step.Num())
						{
							Count = TraceInstrStructure.runs.run[RunIndex].instruction[x].step.Num();
						}
					}
				}
				RunIndex = TraceInstrStructure.runs.run[RunIndex].parent;
			}


			// try to create codeBlock actor
			FTransform SpawnTransform(Rotation, Position);
			TraceBlkStructure.run_blocks[i].block[j].CodeBlockActor = Cast<ACodeBlockActorST>(
				UGameplayStatics::BeginDeferredActorSpawnFromClass(this, CodeBlockActor, SpawnTransform)
			);

			// init and spawn actor
			if (TraceBlkStructure.run_blocks[i].block[j].CodeBlockActor != nullptr)
			{
				TraceBlkStructure.run_blocks[i].block[j].CodeBlockActor->Init(Position, Scale, Count, i, j);

				UGameplayStatics::FinishSpawningActor(TraceBlkStructure.run_blocks[i].block[j].CodeBlockActor,
				                                      SpawnTransform);
			}


			// try to create codeBlockCountActor
			Position.Z += 16.0f;
			Position.Y -= NewBlockSizeY;

			FTransform CountSpawnTransform(Rotation, Position);
			TraceBlkStructure.run_blocks[i].block[j].CodeBlkCountActor = Cast<ACodeBlkCountActorST>(
				UGameplayStatics::BeginDeferredActorSpawnFromClass(this, CodeBlkCountActor, CountSpawnTransform)
			);

			// init and spawn actor
			if (TraceBlkStructure.run_blocks[i].block[j].CodeBlkCountActor != nullptr)
			{
				TraceBlkStructure.run_blocks[i].block[j].CodeBlkCountActor->Init(Count);

				UGameplayStatics::FinishSpawningActor(TraceBlkStructure.run_blocks[i].block[j].CodeBlkCountActor,
				                                      CountSpawnTransform);
			}
		}
	}
}

void ARTSPlayerPawnST::SpawnBlkTextObjects()
{
	UE_LOG(STLog, Warning, TEXT("In SpawnBlkTextObjects:"));
	const FRotator Rotation(0.0);

	const int MinPc = TraceInstrStructure.analysis.min_pc;

	for (int i = 0; i < TraceBlkStructure.cfg_blocks.Num(); i++)
	{
		int BlockStart = TraceBlkStructure.cfg_blocks[i].block_start;
		int BlockEnd = TraceBlkStructure.cfg_blocks[i].block_end;
		int BlockMiddle = (BlockStart + BlockEnd) / 2;
		float PosX = (BlockMiddle - MinPc) * BlockDistance / 4;

		FString FileName = TraceBlkStructure.cfg_blocks[i].file_name;
		int LineStart = TraceBlkStructure.cfg_blocks[i].line_start;
		int LineEnd = TraceBlkStructure.cfg_blocks[i].line_end;

		FVector Position(PosX, 200, 20);

		// try to create codeBlkTextActor
		FTransform SpawnTransformText(Rotation, Position);
		TraceBlkStructure.CodeBlockTextActors.Emplace(Cast<ACodeBlkTextActorST>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, CodeBlkTextActor, SpawnTransformText)
		));

		// init and spawn actor
		if (TraceBlkStructure.CodeBlockTextActors[i] != nullptr)
		{
			TraceBlkStructure.CodeBlockTextActors[i]->Init(FileName, LineStart, LineEnd);
			UGameplayStatics::FinishSpawningActor(TraceBlkStructure.CodeBlockTextActors[i],
			                                      SpawnTransformText);
		}
	}
}
