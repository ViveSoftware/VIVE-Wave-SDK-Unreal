// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

class WAVEVR_API FDllLoader {
public:
	FDllLoader(FString libName);
	~FDllLoader();
	bool LoadExternalDLL();
	void UnloadExternalDLL();
	void * GetHandle();

private:
	FDllLoader();
	void * mDllHandle = nullptr;
	FString DllName;
};
