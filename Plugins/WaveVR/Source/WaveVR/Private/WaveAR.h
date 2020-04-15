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
#include "WaveVRPrivatePCH.h"
#include "ARTypes.h"

class UARBasicLightEstimate;
class UARTrackedGeometry;

class FWaveAR
{
public:
	static FWaveAR* GetInstance();
	~FWaveAR();

public:
	void UpdateData();
	UARBasicLightEstimate* GetLightEstimate();
	TArray<UARTrackedGeometry*> GetCachedAllTrackedGeometry() const;
	EARTrackingQuality GetTrackingQuality() const;

private:
	FWaveAR();
	void CreateFakePlane();

private:
	bool bHasValidPose;
	FVector CachedPosition;
	FQuat CachedOrientation;

	UARBasicLightEstimate* LightEstimate;
	TArray<UARTrackedGeometry*> CachedAllTrackedGeometry;

	friend class FWaveVRHMD;
};
