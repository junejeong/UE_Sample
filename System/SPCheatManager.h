// @Jung June

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/CheatManager.h>

#include "SPCheatManager.generated.h"


struct FAutoCompleteCommand;
struct FSPCheatData
{
	FString Command;
	FString Desc;
	FColor Color;
	FString FunctionName;
};
/**
 * 
 */
UCLASS()
class SAMPLECODE_API USPCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	virtual void InitCheatManager() override;
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
	virtual void BeginDestroy() override;
	void RegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& Commands);

private:
	inline static FString Prefix = "SP.";		
	inline static FColor CommandColor = FColor::Green;
	
	TArray<FSPCheatData> Commands;
	
	void ParseCommands();

public:
	UFUNCTION(Exec, meta = (Cheat = "test", Desc = -"this is reflection"))
	void CheatTestFunction(FString Argument);
	
};
