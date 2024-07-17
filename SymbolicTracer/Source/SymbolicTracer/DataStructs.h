#pragma once

#include "CoreMinimal.h"
#include "ECodeLineState.h"

#include "DataStructs.generated.h"


// forward declarations

class ACodeBlkCountActorST;
class APathSplineActorST;
class ARunAnimationActorST;
class AInstructionSplineActorST;
class AInstructionTextActorST;
class AInstructionLineMarkActorST;
enum class ECodeLineState : uint8;
class APathTextActorST;
class ACodeBlkTextActorST;
class AFunctionTextActorST;
class AFunctionBlockActorST;
class AInstructionLineActorST;
class ACodeBlockActorST;
class AInstructionActorST;
class UCodeLinesDataEntry;
class UCodeFilesDataEntry;


/*
 * structure for holding steps -> for instructions (form ptrace)
 */
USTRUCT(BlueprintType, Blueprintable)
struct FStepInstr
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 id = -1;

	UPROPERTY(BlueprintReadOnly)
	FString beh = "none";

	UPROPERTY(BlueprintReadOnly)
	int32 depth = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 target = -1;

	UPROPERTY(BlueprintReadOnly)
	bool condition = true;

	// actor to represent the instruction in 3D
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AInstructionActorST> InstructionActor3D = nullptr;

	// actor to represent control flow for jump and branch instructions -> will only be set for them
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AInstructionSplineActorST> InstructionSplineActor3D = nullptr;
};


/*
 * structure for holding instructions
 */
USTRUCT(BlueprintType, Blueprintable)
struct FInstruction
{
	GENERATED_BODY()


	// all the properties from the json trace
	UPROPERTY(BlueprintReadOnly)
	int32 pc = -1;

	UPROPERTY(BlueprintReadOnly)
	FString pc_hex;

	UPROPERTY(BlueprintReadOnly)
	int32 run_id = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 code_line = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 code_file_id = -1;

	UPROPERTY(BlueprintReadOnly)
	FString opcode;

	UPROPERTY(BlueprintReadOnly)
	FString type;

	UPROPERTY(BlueprintReadOnly)
	FString rs1;

	UPROPERTY(BlueprintReadOnly)
	FString rs2;

	UPROPERTY(BlueprintReadOnly)
	FString rs2d;

	UPROPERTY(BlueprintReadOnly)
	FString rd;

	UPROPERTY(BlueprintReadOnly)
	FString link;

	UPROPERTY(BlueprintReadOnly)
	int32 link_address = -1;

	UPROPERTY(BlueprintReadOnly)
	FString link_address_hex;

	UPROPERTY(BlueprintReadOnly)
	FString imm1;

	UPROPERTY(BlueprintReadOnly)
	FString imm2;

	UPROPERTY(BlueprintReadOnly)
	TArray<FStepInstr> step;

	// actor to represent the instruction in 2D
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AInstructionActorST> InstructionActor2D = nullptr;

	// actor to represent control flow for jump and branch instructions -> will only be set for them
	// this will be set for jumps and branches
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AInstructionSplineActorST> InstructionSplineActorTrue2D = nullptr;

	// actor to represent control flow for jump and branch instructions -> will only be set for them
	// this will not be set for jumps, but for branches
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AInstructionSplineActorST> InstructionSplineActorFalse2D = nullptr;
};


/*
 * structure for holding individual runs
 */
USTRUCT(BlueprintType)
struct FRunInstr
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadOnly)
	int32 run_id = -1;

	UPROPERTY()
	int32 parent = -1;

	UPROPERTY()
	FString start_hex;

	UPROPERTY()
	FString end_hex;

	UPROPERTY()
	int32 steps = -1;

	// holds all the instructions of this path
	UPROPERTY(BlueprintReadOnly)
	TArray<FInstruction> instruction;

	// holds all instructionTextActors of this run (the opCode) 
	UPROPERTY()
	TArray<AInstructionTextActorST*> InstructionTextActors;

	// holds the pathSplineActor (is held in child) -> when parentPath -1 this is nullptr
	UPROPERTY()
	TObjectPtr<APathSplineActorST> PathSplineActor = nullptr;
	
};

/*
 * structure for holding runs -> from ptrace.json
 */
USTRUCT(BlueprintType)
struct FRunsInstr
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadOnly)
	FString name;
	UPROPERTY(BlueprintReadOnly)
	TArray<FRunInstr> run;
};


/*
 * structure for holding analysis data about each run (e.g. code coverage)
 * -> will all be computed and does not come from json
 */
USTRUCT(BlueprintType)
struct FAnalysisPath
{
	GENERATED_BODY()


	// run id of this specific path
	UPROPERTY(BlueprintReadOnly)
	int32 RunInd = -1;

	// start pc of this run
	UPROPERTY()
	FString StartPcHex;

	// end pc of this run
	UPROPERTY()
	FString EndPcHex;
	
	// number of steps (from start not split)
	UPROPERTY()
	int32 StepsNum = 0;

	// run id of the parent run
	UPROPERTY()
	int32 ParentRunInd = -1;

	// split start of this run (the first animationStep number of this run)
	UPROPERTY()
	int32 SplitStartStepNumber = -1;

	// split start of this run (the first animationStep number of this run)
	UPROPERTY()
	FString SplitStartPcHex;

	// codeCoverage percentage of this run
	UPROPERTY()
	float CodeCoveragePercentage = 0.0f;

	// number of codeBlocks in this run
	UPROPERTY(BlueprintReadOnly)
	int32 CodeBlocksNum = 0;

};


/*
 * structure for holding the analysis data -> from ptrace.json with some extras
 */
USTRUCT(BlueprintType)
struct FAnalysisInstr
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 global_start = -1;

	UPROPERTY(BlueprintReadOnly)
	FString global_start_hex;

	UPROPERTY(BlueprintReadOnly)
	int32 min_pc = -1;

	UPROPERTY(BlueprintReadOnly)
	FString min_pc_hex;

	UPROPERTY(BlueprintReadOnly)
	int32 max_pc = -1;

	UPROPERTY(BlueprintReadOnly)
	FString max_pc_hex;

	UPROPERTY(BlueprintReadOnly)
	int32 num_runs = -1;

	// TODO: add some general analysis data that might be interesting

	// interesting for scaling (e.g. runAnimationActor)
	UPROPERTY(BlueprintReadOnly)
	int32 max_depth = 0;

	// holds code coverage percentage of this complete trace (gets computed)
	UPROPERTY()
	float CodeCovPercentage = 0.0f; 

	// some analysis data about each run
	UPROPERTY(BlueprintReadOnly)
	TArray<FAnalysisPath> analyis_paths;
};


/*
 * structure for holding an animation step-> from ptrace.json with some extras
 */
USTRUCT(BlueprintType)
struct FAnimationStep
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadOnly)
	int32 step_id = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 run_index = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 instruction_index = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 step_index = -1;
};


/*
 * structure for holding the individual animations for runs-> from ptrace.json with some extras
 */
USTRUCT(BlueprintType)
struct FRunAnimation
{
	GENERATED_BODY()


	// will probably never be used
	UPROPERTY(BlueprintReadOnly)
	int32 run_index = -1;

	// the animation steps for this run
	UPROPERTY(BlueprintReadOnly)
	TArray<FAnimationStep> animation_steps;

	// to keep track of where this animation is atm
	UPROPERTY(BlueprintReadOnly)
	int CurrentStepAnimationIndex = 0;

	// the instructionActor that gets marked (the current instructionStep)
	TObjectPtr<AInstructionActorST> CurrentInstructionActor = nullptr;

	// the animationActor for this run
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ARunAnimationActorST> RunAnimationActor = nullptr;

};


/*
 * structure for holding the animation data -> from ptrace.json with some extras
 */
USTRUCT(BlueprintType)
struct FTraceAnimation
{
	GENERATED_BODY()

	// animation data for each individual run
	UPROPERTY(BlueprintReadOnly)
	TArray<FRunAnimation> run_animations;

	// TODO: maybe add an run animation actor for going through all runs
};


/*
 * structure for holding the complete trace -> from ptrace.json
 */
USTRUCT(BlueprintType)
struct FTraceInstr
{
	GENERATED_BODY()

	// the ptrace version -> from ptrace.json
	UPROPERTY(BlueprintReadOnly)
	FString ptrace_version;

	// the runs -> from ptrace.json 
	UPROPERTY(BlueprintReadOnly)
	FRunsInstr runs;

	// animation -> from ptrace.json
	UPROPERTY(BlueprintReadOnly)
	FTraceAnimation trace_animation;

	// analysis -> from ptrace.json
	UPROPERTY(BlueprintReadOnly)
	FAnalysisInstr analysis;

	// actors to represent the instruction lines in the scene
	UPROPERTY(BlueprintReadWrite)
	TArray<AInstructionLineActorST*> InstructionLineActors;

	// actors to represent the pathTexts in the scene
	UPROPERTY(BlueprintReadWrite)
	TArray<APathTextActorST*> PathTextActors;

	// actor to represent the instructionLineMarker
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AInstructionLineMarkActorST> InstructionLineMarkActor = nullptr;
};


/*
 * structure for holding a codeLine
 */
USTRUCT(BlueprintType)
struct FCodeLine
{
	GENERATED_BODY()

	// holds the content of the code line (the code itself) 
	UPROPERTY(BlueprintReadOnly)
	FString Content;

	// indicates if this line is at the beginning of a codeBlock -> will be computed
	UPROPERTY(BlueprintReadOnly)
	bool CodeBlockBeginning = false;

	// indicates if this line is at the end of a codeBlock -> will be computed 
	UPROPERTY(BlueprintReadOnly)
	bool CodeBlockEnding = false;

	// indicates which pc this codeLine starts at
	UPROPERTY(BlueprintReadOnly)
	int32 pc = -1;

	// indicates how many branches this codeLine has
	UPROPERTY(BlueprintReadOnly)
	int32 BranchCount = 0;

	// indicates how many branches in this codeLine got hit
	UPROPERTY(BlueprintReadOnly)
	int32 BranchCoverage = 0;

	// indicates how many times this codeLine got hit
	UPROPERTY(BlueprintReadOnly)
	int32 HitCount = 0;

	// was this line symbolized ?!
	UPROPERTY(BlueprintReadOnly)
	bool Symbolized = false;

	// was this line concretized ?!
	UPROPERTY(BlueprintReadOnly)
	bool Concretized = false;

	// indicates if this line got hit by symbolic execution (same as part of codeBlock)
	// TODO: -> but instructions can hit it even if not part of codeBlock
	UPROPERTY(BlueprintReadOnly)
	ECodeLineState CodeLineState = ECodeLineState::None;


	// TODO: maybe add a run and block index -> so we can click on codeLine and jump with camera to codeBlockActor in scene
};


/*
 * structure for holding a function (from blk.json)
 */
USTRUCT(BlueprintType)
struct FaFunction
{
	GENERATED_BODY()

	// holds the name of this function
	UPROPERTY(BlueprintReadOnly)
	FString name;

	// start of function (in pc)
	UPROPERTY(BlueprintReadOnly)
	int32 start = -1;

	// end of function (in pc)
	UPROPERTY(BlueprintReadOnly)
	int32 end = -1;

	bool operator<(const FaFunction& other) const
	{
		return start < other.start;
	}
};


/*
 * structure for holding a compiledCodeLine (from blk.json)
 */
USTRUCT(BlueprintType)
struct FCompiledCodeLine
{
	GENERATED_BODY()

	// holds the line
	UPROPERTY(BlueprintReadOnly)
	int32 line = -1;

	// start of this codeLine (in pc)
	UPROPERTY(BlueprintReadOnly)
	int32 pc = -1;

	UPROPERTY()
	int32 hit_count = 0;
};


/*
 * structure for holding a codeFile (from blk.json)
 */
USTRUCT(BlueprintType)
struct FCodeFile
{
	GENERATED_BODY()

	// holds the fileID (should be the same as index)
	UPROPERTY(BlueprintReadOnly)
	int32 file_id = -1;

	// holds the filename (e.g. with .cpp)
	UPROPERTY(BlueprintReadOnly)
	FString name;

	// holds the absolute path of the file (without name)
	UPROPERTY(BlueprintReadOnly)
	FString path;

	// holds the absolute path with name of file
	UPROPERTY(BlueprintReadOnly)
	FString path_name;

	// holds code coverage percentage of this codeFile (gets computed)
	UPROPERTY()
	int32 CodeCovPercentage = 0; 

	// holds the functions of the file 
	UPROPERTY(BlueprintReadOnly)
	TArray<FaFunction> functions;
	
	// holds all the lines that where found in the dwarf information -> which where compiled 
	UPROPERTY(BlueprintReadOnly)
	TArray<FCompiledCodeLine> compiled_code_lines;

	// holds the codeLines of this codeFile -> not from json, gets loaded in later 
	UPROPERTY(BlueprintReadOnly)
	TArray<FCodeLine> code_lines;

	// holds the codeLines of this codeFile -> just for visualization in the HUD 
	UPROPERTY(BlueprintReadOnly)
	TArray<UCodeLinesDataEntry*> CodeLinesDataEntries;
};

/*
 * structure for holding one code block -> from ptrace.json
 */
USTRUCT(BlueprintType)
struct FCodeBlock
{
	GENERATED_BODY()

	// block_start in pc 
	UPROPERTY(BlueprintReadOnly)
	int32 block_start = -1;

	// block_start_hex in pc in hex
	UPROPERTY(BlueprintReadOnly)
	FString block_start_hex;

	// block_end in pc
	UPROPERTY(BlueprintReadOnly)
	int32 block_end = -1;

	// block_end in pc in hex
	UPROPERTY(BlueprintReadOnly)
	FString block_end_hex;

	// name of the file this block is part of
	UPROPERTY(BlueprintReadOnly)
	FString file_name;

	// id/index this file is part of
	UPROPERTY(BlueprintReadOnly)
	int32 code_file_id = -1;

	// start of block in codeFile
	UPROPERTY(BlueprintReadOnly)
	int32 line_start = -1;

	// end of block in codeFile
	UPROPERTY(BlueprintReadOnly)
	int32 line_end = -1;

	// name of function this block is part of
	UPROPERTY(BlueprintReadOnly)
	FString function_name;


	// TODO: maybe add a run id and block_id? -> to act as indices -> only when needed
};

/*
 * structure for holding the blocks inside run_blocks
 */
USTRUCT(BlueprintType)
struct FRunBlock
{
	GENERATED_BODY()

	// TODO: adding a run_id might come in handy

	// the blockId
	UPROPERTY()
	int32 block_id = -1;

	// actor to represent the codeBlock
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACodeBlockActorST> CodeBlockActor = nullptr;

	// actor to represent the codeBlockCount
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACodeBlkCountActorST> CodeBlkCountActor = nullptr;
};

/*
 * structure for holding one run with its blocks -> from blk.json
 */
USTRUCT(BlueprintType)
struct FRunsBlk
{
	GENERATED_BODY()

	// holds the run_id -> identical to index
	UPROPERTY(BlueprintReadOnly)
	int32 run_id = -1;

	// holds the list of blocks 
	UPROPERTY(BlueprintReadOnly)
	TArray<FRunBlock> block;
};


/*
 * structure for holding the complete trace -> from blk.json
 */
USTRUCT(BlueprintType)
struct FTraceBlk
{
	GENERATED_BODY()

	// holds the run_blocks from blk.json
	UPROPERTY(BlueprintReadOnly)
	TArray<FRunsBlk> run_blocks;

	// holds the cfg_blocks from blk.json -> these are unique
	UPROPERTY(BlueprintReadOnly)
	TArray<FCodeBlock> cfg_blocks;

	// holds the code_files from blk.json
	UPROPERTY(BlueprintReadOnly)
	TArray<FCodeFile> code_files;

	// holds all the functionBlockActors	
	UPROPERTY(BlueprintReadOnly)
	TArray<AFunctionBlockActorST*> FunctionBlockActors;

	// holds all the functionTextActors	
	UPROPERTY(BlueprintReadOnly)
	TArray<AFunctionTextActorST*> FunctionTextActors;

	// holds all the blockTextActors	
	UPROPERTY(BlueprintReadOnly)
	TArray<ACodeBlkTextActorST*> CodeBlockTextActors;

	// holds the codeFiles -> just for visualization in the HUD 
	UPROPERTY(BlueprintReadOnly)
	TArray<UCodeFilesDataEntry*> CodeFilesDataEntries;
};
