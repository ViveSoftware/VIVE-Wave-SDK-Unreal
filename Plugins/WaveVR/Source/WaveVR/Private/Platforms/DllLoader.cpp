// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "DllLoader.h"
#include "WaveVRPrivatePCH.h"

DEFINE_LOG_CATEGORY_STATIC(WVRDllLoader, Log, All);

FDllLoader::FDllLoader(FString libName)
  : DllName(libName) {
}

FDllLoader::~FDllLoader() {
	mDllHandle = nullptr;
}

void * FDllLoader::GetHandle() {
	return mDllHandle;
}

bool FDllLoader::LoadExternalDLL() {
#if PLATFORM_WINDOWS
	FString WaveVRDllDir = FPaths::EngineDir() / FString::Printf(TEXT("Source/ThirdParty/WVR/lib/Win64/"));

	if (mDllHandle != nullptr)
	{
		UE_LOG(WVRDllLoader, Error, TEXT("Error : mDllHandle != nullptr"));
		return false;
	}

	FPlatformProcess::PushDllDirectory(*WaveVRDllDir);
	mDllHandle = FPlatformProcess::GetDllHandle(*(WaveVRDllDir + DllName));
	FPlatformProcess::PopDllDirectory(*WaveVRDllDir);
#endif	//PLATFORM_WINDOWS

	if (mDllHandle == nullptr)
	{
		UE_LOG(WVRDllLoader, Log, TEXT("Failed to load %s."), *DllName);
		return false;
	}

	UE_LOG(WVRDllLoader, Log, TEXT("Load %s success."), *DllName);
	return true;
}

void FDllLoader::UnloadExternalDLL() {
	//LOG_FUNC();
	if (mDllHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(mDllHandle);
		mDllHandle = nullptr;
		UE_LOG(WVRDllLoader, Log, TEXT("Unload %s success."), *DllName);
	}
}
