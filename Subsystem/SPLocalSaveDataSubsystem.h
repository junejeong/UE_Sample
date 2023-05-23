// @Jung June

#pragma once

#include "CoreMinimal.h"

#include <Subsystems/GameInstanceSubsystem.h> 
#include <kismet/GameplayStatics.h>

#include "System/CSPomethingLocalSaveData.h"

#include "SPLocalSaveDataSubsystem.generated.h"

/**
 *
 */
UCLASS()
class SAMPLECODE_API USPLocalSaveDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USPLocalSaveDataSubsystem* Get()
	{
		return Instance;
	}
private:
	inline static USPLocalSaveDataSubsystem* Instance = nullptr;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override; 
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	class USPSomethingLocalSaveData* GetSomethingSaveData() { return SomethingSaveData; }

private:
	template<typename T>
	T* LoadOrCreateSaveData(FString&& FileName)
	{
		T* SaveData = Cast<T>(UGameplayStatics::LoadGameFromSlot(FileName, 0));

		if (SaveData == nullptr)
		{
			SaveData = NewObject<T>();

		}

		SaveData->InitSave FileName(Move Temp(FileName));
		return SaveData;
	}

	void LoadSomthingSaveData();

private:
	UPROPERTY()
	class USPSomethingLocalSaveData* SomethingSaveData;
};
