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

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"
#include "WaveVRCameraTexture.generated.h"

UENUM(BlueprintType)
enum class EWVR_CameraImageType : uint8
{
	WVR_CameraImageType_Invalid = 0,
	WVR_CameraImageType_SingleEye = 1,
	WVR_CameraImageType_DualEye = 2,
};

UENUM(BlueprintType)
enum class EWVR_CameraImageFormat : uint8
{
	WVR_CameraImageFormat_Invalid = 0,
	WVR_CameraImageFormat_YUV_420 = 1,
	WVR_CameraImageFormat_Grayscale = 2,
};

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UWaveVRCameraTexture : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWaveVRCameraTexture();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//void updateTexture(void* data);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "To update Texture2D by native camera raw data."))
	void updateCamera(float delta);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Start native camera component and return Texture2D pointer to update camera images."))
	UTexture2D* getCameraTexture();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Stop native camera component."))
	void shutdownCamera();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Return image format from native camera component."))
	EWVR_CameraImageFormat getCameraImageFormat();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Return image type from native camera component."))
	EWVR_CameraImageType getCameraImageType();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Return image width from native camera component."))
	int getCameraImageWidth();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Return image height from native camera component."))
	int getCameraImageHeight();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Return true if native camera component is started."))
	bool isCameraStarted();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture", meta = (
		ToolTip = "Return true if Texture2D is updated by image from native camera component."))
	bool isCameraUpdated();

private:
	bool bActive = false;
	bool bUpdated = false;
	UTexture2D* CameraTexture;
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mSize;
	EWVR_CameraImageType mImgType;
	EWVR_CameraImageFormat mImgFormat;
	TArray<FColor> rawData;
	FUpdateTextureRegion2D *echoUpdateTextureRegion;
	//void updateTexture(uint8_t* data);
	void updateTextureG(uint8_t* data);
	void reset();
	int YUVtoRGB(int y, int u, int v);
	const float frameRate = 30.f;
	float timer = 0;
	uint8_t* nativeRawData;
	EPixelFormat mPixelFormat;
};

// Region Data struct
struct FUpdateTextureRegionsData
{
	FTexture2DResource* Texture2DResource;
	int32 MipIndex;
	uint32 NumRegions;
	FUpdateTextureRegion2D* Regions;
	uint32 SrcPitch;
	uint32 SrcBpp;
	uint8* SrcData;
};
