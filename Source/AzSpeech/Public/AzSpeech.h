// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEAzSpeech

#pragma once

#include "Modules/ModuleInterface.h"

/**
 *
 */

DECLARE_LOG_CATEGORY_EXTERN(LogAzSpeech, Display, Verbose);

class FAzSpeechModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void* CoreRuntimeLib = nullptr;
	void* AudioRuntimeLib = nullptr;
	void* KwsRuntimeLib = nullptr;
	void* LuRuntimeLib = nullptr;
	void* MasRuntimeLib = nullptr;
	void* SilkRuntimeLib = nullptr;
	void* CodecRuntimeLib = nullptr;

	static void FreeDependency(void*& Handle);
	static void LoadDependency(const FString& Path, void*& Handle);
};
