// @Jung June


#include "SPBaseLocalSaveData.h"

#include <Kismet/GameplayStatics.h>

void USPBaseLocalSaveData::InitSavefileName(FString&& FileName)
{
	SaveFileName = FileName;
}

void USPBaseLocalSaveData::Save()
{
	UGameplayStatics::SaveGameToSlot(this, SaveFileName, 0);
}
