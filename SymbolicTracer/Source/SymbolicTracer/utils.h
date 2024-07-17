#pragma once

/*
 * example for working scales:
 *
constexpr float BlockDistance = 30.0f;
constexpr float BlockScale = 0.07f;
constexpr float RunDistance = -100.0f;
constexpr float InstrLineStart = 7.0f;
 */


// some variables used for spawning the actors in the scene
constexpr float BlockDistance = 30.0f;
constexpr float BlockScale = 0.07f;
constexpr float InstrScale = 0.07f; 
constexpr float RunDistance = 250.0f;
constexpr float InstrLineStart = 7.0f;
constexpr float BlockSize = 200.0f;
constexpr float NewBlockSize = BlockSize * BlockScale;
constexpr float BlockScaleY = 1.0f;
constexpr float NewBlockSizeY = BlockSize * BlockScaleY;
constexpr float MiddleOfRunY = NewBlockSizeY / 2;


// covnerts int to hexString
inline FString ToHexString(int32 number)
{
	if (number < 0)
	{
		FString result = "";
		return result;
	}
	std::ostringstream ss;
	ss << "0x" << std::hex << number;
	std::string result = ss.str();

	FString fResult(result.c_str());

	return fResult;
}
