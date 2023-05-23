// @Jung June

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/SaveGame.h>
#include "SPBaseLocalSaveData.generated.h"

/**
 * 
 */
enum class ESaveDataType : uint8
{
	None = 0,
	Something = 1
};

UCLASS()
class SAMPLECODE_API USPBaseLocalSaveData : public USaveGame
{
	GENERATED_BODY()

public:
	virtual ESaveDataType GetLocalSaveDataType() { return ESaveDataType::None; }
	virtual void Reset() {}

	void InitSavefileName(FString&& FileName);
	void Save();

private:
	FString SaveFileName;
	
};
