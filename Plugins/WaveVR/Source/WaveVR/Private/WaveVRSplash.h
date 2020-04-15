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

#include "CoreMinimal.h"

class FWaveVRHMD;
class FWaveVRRender;

class FWaveVRSplash : public TSharedFromThis<FWaveVRSplash>
{
  public:

	FWaveVRSplash(FWaveVRRender* InRender);
	~FWaveVRSplash();

	void Init();
	void Show();
	void Hide();
	void SetSplashParam(UTexture2D* InSplashTexture, FLinearColor InBackGroundColor, float InScaleFactor, FVector2D Shift, bool EnableAutoLoading);
	bool IsShown() const { return bIsShown; }

  private:
	void OnPreLoadMap(const FString&);
	void OnPostLoadMap(UWorld*);

	bool ApplyTexture();
	void DisApplyTexture();
	void RenderStereo_RenderThread();
	bool AllocateSplashWrapper_RenderThread();
	void SubmitFrame_RenderThread();

  private:
	bool bAutoShow;
	UTexture2D* SplashTexture;
	IRendererModule* RendererModule;
	FWaveVRRender* mRender;
	FLinearColor BackGroundColor;
	float ScaleFactor;
	FVector2D PositionShift;


	bool bInitialized;
	bool bIsShown;
	bool bSplashScreenRendered;
	FTexture2DRHIRef SplashWrapperRHIRef;
	int32_t SplashWrapperId;
	int32_t SplashOverlayId;
};
