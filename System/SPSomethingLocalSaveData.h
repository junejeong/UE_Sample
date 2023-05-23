// @Jung June

#pragma once

#include "CoreMinimal.h"
#include "SPBaseLocalSaveData.h"
#include "SPSomethingLocalSaveData.generated.h"

/**
 *
 */
UCLASS()
class SAMPLECODE_API USPSomethingLocalSaveData : public USPBaseLocalSaveData
{
	GENERATED_BODY()

public:
	virtual ESaveDataType GetLocalSaveDataType() { return ESaveDataType::Something; }
	
	UPROPERTY()
	FString SomethingString;

};
