// @Jung June


#include "SPCheatManager.h"
#include <Engine/Console.h>
#include <AssetRegistry/AssetRegistryModule.h>

void USPCheatManager::InitCheatManager()
{
	ParseCommands();
	UConsole::RegisterConsoleAutoCompleteEntries.AddUObject(this, &USPCheatManager::RegisterAutoCompleteEntries);
}

bool USPCheatManager::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bForceExecute = false;
	FString Source = Cmd;
	FString Key;
	if (FParse::Token(Cmd, Key, true))
	{
		auto* FindItem = Commands.FindByPredicate([Key](const FSPCheatData& Item)
			{
				return Item.Command;
			});

		if (FindItem != nullptr)
		{
			bForceExecute = true;
			Source = FindItem->FunctionName + Cmd;
		}
	}

	return CallFunctionByNameWithArguments(*Source, Ar, Executor, bForceExecute);
}

void USPCheatManager::BeginDestroy()
{
	Super::BeginDestroy();
	UConsole::RegisterConsoleAutoCompleteEntries.RemoveAll(this);
}

void USPCheatManager::RegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteList)
{
	for (const auto& Item : Commands)
	{
		FAutoCompleteCommand Entry;
		Entry.Command = Item.Command;
		Entry.Desc = Item.Desc;
		Entry.Color = Item.Color;
		AutoCompleteList.Emplace(Entry);
	}
}

void USPCheatManager::ParseCommands()
{
	const UClass* CurrentClass = GetClass();

	TArray<FName> FunctionNames;
	CurrentClass->GenerateFunctionList(FunctionNames);

	for (const auto& Name : FunctionNames)
	{
		const UFunction* Func = CurrentClass->FindFunctionByName(Name);
		if (Func == nullptr)
		{
			continue;
		}

		FString CheatName = Prefix + Func->GetMetaData(TEXT("Cheat"));
		FString Desc = Func->GetMetaData(TEXT("Desc"));

		for (TFieldIterator<FProperty> PropIt(Func); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			FProperty* Prop = *PropIt;
			Desc += FString::Printf(TEXT(" %s [%s]"), *Prop->GetName(), *Prop->GetCPPType());
		}

		FSPCheatData Data;
		Data.Command = CheatName;
		Data.Desc = Desc;
		Data.Color = CommandColor;
		Data.FunctionName = Name.ToString();

		Commands.Emplace(Data);
	}
}



void USPCheatManager::CheatTestFunction(FString Argument)
{

}