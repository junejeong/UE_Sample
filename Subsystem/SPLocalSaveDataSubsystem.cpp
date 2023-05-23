// @Jung June


#include "SPLocalSaveDataSubsystem.h"

void USPLocalSaveDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Instance =	this;

	LoadSomethingSaveData();
}

void USPLocalSaveDataSubsystem::Deinitialize()
{
	Instance =	nullptr;
	Super::Deinitialize();
}

bool USPLocalSaveDataSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void USPLocalSaveDataSubsystem::LoadSomethingSaveData()
{
	SomethingSaveData = LoadOrCreateSaveData<UCPSomethingLocalSaveData>(TEXT("Something"));
}