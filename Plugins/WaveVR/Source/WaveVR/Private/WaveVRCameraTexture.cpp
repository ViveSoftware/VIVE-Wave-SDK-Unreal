// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRCameraTexture.h"
#include "WaveVRPrivatePCH.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(WVR_Camera, Display, All);

// Sets default values for this component's properties
UWaveVRCameraTexture::UWaveVRCameraTexture()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	reset();

	// ...
}

// Called when the game starts
void UWaveVRCameraTexture::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UWaveVRCameraTexture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UWaveVRCameraTexture::isCameraStarted()
{
	return bActive;
}

bool UWaveVRCameraTexture::isCameraUpdated()
{
	return bUpdated;
}

UTexture2D*  UWaveVRCameraTexture::getCameraTexture()
{
	if (!bActive)
	{
		WVR_CameraInfo cameraInfo;

		bActive = FWaveVRAPIWrapper::GetInstance()->StartCamera(&cameraInfo);
		if (bActive == true) {
			if (cameraInfo.imgType == WVR_CameraImageType_SingleEye) {
				mSize = cameraInfo.size;
				mWidth = cameraInfo.width;
				mHeight = cameraInfo.height;
				nativeRawData = (uint8_t *)malloc(mSize);
				mImgType = (EWVR_CameraImageType)WVR_CameraImageType_SingleEye;
				mImgFormat = (EWVR_CameraImageFormat)cameraInfo.imgFormat;
			}
			else if (cameraInfo.imgType == WVR_CameraImageType_DualEye) {
				mSize = cameraInfo.size;
				mWidth = cameraInfo.width;
				mHeight = cameraInfo.height;
				nativeRawData = (uint8_t *)malloc(mSize);
				mImgType = (EWVR_CameraImageType)WVR_CameraImageType_DualEye;
				mImgFormat = (EWVR_CameraImageFormat)cameraInfo.imgFormat;
			}
			else {
				LOGE(WVR_Camera, "No support these image type");
				mWidth = 640;
				mHeight = 480;
			}
			LOGE(WVR_Camera, "start camera success, getCameraTexture, width = %d, height = %d", mWidth, mHeight);
		}
		else {
			LOGE(WVR_Camera, "start camera fail");
			mWidth = 640;
			mHeight = 480;
		}

		//create texture
		CameraTexture = UTexture2D::CreateTransient(mWidth, mHeight, mPixelFormat);
		echoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, mWidth, mHeight);
		CameraTexture->UpdateResource();
		rawData.Init(FColor(0, 0, 0, 255), mWidth*mHeight);

		PrimaryComponentTick.bCanEverTick = true;
		PrimaryComponentTick.TickGroup = TG_PostPhysics;
	}
	return CameraTexture;
}

void UWaveVRCameraTexture::shutdownCamera()
{
	if (bActive)
	{
		bActive = false;
		FWaveVRAPIWrapper::GetInstance()->StopCamera();

		mWidth = 0;
		mHeight = 0;
	}

	if (echoUpdateTextureRegion != nullptr) {
		delete echoUpdateTextureRegion;
	}
}

void UWaveVRCameraTexture::updateCamera(float delta)
{
	timer += delta;
	if (timer >= (1.f / frameRate))
	{
		timer = 0;
		if (CameraTexture && bActive)
		{
			bool ret = FWaveVRAPIWrapper::GetInstance()->GetCameraFrameBuffer(nativeRawData, mSize);
			if (nativeRawData != nullptr) {
				// updateTexture(nativeRawData);
				//LOGE(WVR_Camera, "start SW codec");
				updateTextureG(nativeRawData);
				//LOGE(WVR_Camera, "end SW codec");
			}
		}
	}
}
/*
void UWaveVRCameraTexture::updateTexture(uint8_t* data)
{
	if (data == NULL) return;

#if WAVEVR_SUPPORTED_PLATFORMS
	// fill my Texture Region data
	FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
	RegionData->Texture2DResource = (FTexture2DResource*)CameraTexture->Resource;
	RegionData->MipIndex = 0;
	RegionData->NumRegions = 1;
	RegionData->Regions = echoUpdateTextureRegion;
	RegionData->SrcPitch = (uint32)(4 * mWidth);
	RegionData->SrcBpp = 4;

	uint8_t* yarray = (uint8_t*)data;
	uint8_t* uarray = yarray + (mWidth * mHeight);
	uint8_t* varray = yarray + (mWidth * mHeight) + (mWidth * mHeight) / 4;
	int ptr = 0;
	uint8_t* rgb = new uint8_t[mWidth * mHeight * 4];

	for (int y = 0; y < mHeight; y++) {
		for (int x = 0; x < mWidth; x++) {
			int yy = *(yarray + (y * mWidth) + x);
			int uu = *(uarray + ((y / 2) * (mWidth / 2)) + (x / 2));
			int vv = *(varray + ((y / 2) * (mWidth / 2)) + (x / 2));

			int r = 1.164 * (yy - 16) + 1.596 * (vv - 128);
			int g = 1.164 * (yy - 16) - 0.813 * (vv - 128) - 0.391 * (uu - 128);
			int b = 1.164 * (yy - 16) + 2.018 * (uu - 128);
			rgb[ptr++] = r > 255 ? 255 : r < 0 ? 0 : r;
			rgb[ptr++] = g > 255 ? 255 : g < 0 ? 0 : g;
			rgb[ptr++] = b > 255 ? 255 : b < 0 ? 0 : b;
			rgb[ptr++] = 255;
		}
	}
	RegionData->SrcData = (uint8*)rgb;

	FTexture2DMipMap& Mip = CameraTexture->PlatformData->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, RegionData->SrcData, mWidth * mHeight * 4);
	Mip.BulkData.Unlock();
	CameraTexture->UpdateResource();
	delete[] rgb;
	delete RegionData;
#endif
}
*/

EWVR_CameraImageFormat UWaveVRCameraTexture::getCameraImageFormat() {
	return (bActive ? mImgFormat : EWVR_CameraImageFormat::WVR_CameraImageFormat_Invalid);
}

EWVR_CameraImageType UWaveVRCameraTexture::getCameraImageType() {
	return (bActive ? mImgType : EWVR_CameraImageType::WVR_CameraImageType_Invalid);
}

int UWaveVRCameraTexture::getCameraImageWidth() {
	return (bActive ? mWidth : 0);
}

int UWaveVRCameraTexture::getCameraImageHeight() {
	return (bActive ? mHeight : 0);
}

void UWaveVRCameraTexture::reset() {
	bActive = false;
	bUpdated = false;
	timer = 0.F;
	mPixelFormat = EPixelFormat::PF_R8G8B8A8;
	mSize = 0;
	mWidth = 0;
	mHeight = 0;
	mImgFormat = (EWVR_CameraImageFormat)WVR_CameraImageFormat_Invalid;
	mImgType = (EWVR_CameraImageType)WVR_CameraImageType_Invalid;
	if (nativeRawData != nullptr) {
		delete nativeRawData;
		nativeRawData = nullptr;
	}
}

void UWaveVRCameraTexture::updateTextureG(uint8_t* data)
{
	if (data == NULL) return;

	// fill my Texture Region data
	FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
	RegionData->Texture2DResource = (FTexture2DResource*)CameraTexture->Resource;
	RegionData->MipIndex = 0;
	RegionData->NumRegions = 1;
	RegionData->Regions = echoUpdateTextureRegion;
	RegionData->SrcPitch = (uint32)(4 * mWidth);
	RegionData->SrcBpp = 4;

	char* yuv420sp = (char*)data;
	int* rgb = new int[mWidth * mHeight];
	if (!nativeRawData) return;

	int size = mWidth*mHeight;
	int offset = size;

	int u, v, y1, y2, y3, y4;

//    for (int i = 0, k = 0; i < size; i += 2, k += 2) {
	for (int i = 0, k = 0; i < size; i += 2, k += 1) {
		y1 = yuv420sp[i] & 0xff;
		y2 = yuv420sp[i + 1] & 0xff;
		y3 = yuv420sp[mWidth + i] & 0xff;
		y4 = yuv420sp[mWidth + i + 1] & 0xff;

	   // u = yuv420sp[offset + k] & 0xff;
	   // v = yuv420sp[offset + (mWidth/2 * mHeight/2) + k] & 0xff;
		u = 0;//u - 128;
		v = 0;//v - 128;

		rgb[i] = YUVtoRGB(y1, u, v);
		rgb[i + 1] = YUVtoRGB(y2, u, v);
		rgb[mWidth + i] = YUVtoRGB(y3, u, v);
		rgb[mWidth + i + 1] = YUVtoRGB(y4, u, v);

		if (i != 0 && (i + 2) % mWidth == 0)
			i += mWidth;
	}

	RegionData->SrcData = (uint8*)rgb;

	FTexture2DMipMap& Mip = CameraTexture->PlatformData->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, RegionData->SrcData, mWidth * mHeight * 4);
	Mip.BulkData.Unlock();
	CameraTexture->UpdateResource();

	delete[] rgb;
	delete RegionData;
	bUpdated = true;
}

int UWaveVRCameraTexture::YUVtoRGB(int y, int u, int v)
{
	int r, g, b;
	r = y + (int)1.402f*v;
	g = y - (int)(0.344f*u + 0.714f*v);
	b = y + (int)1.772f*u;
	r = r>255 ? 255 : r<0 ? 0 : r;
	g = g>255 ? 255 : g<0 ? 0 : g;
	b = b>255 ? 255 : b<0 ? 0 : b;
	//return 0x000000ff | (r << 24) | (g << 16) | (b << 8);
	return 0xff000000 | (r << 16) | (g << 8) | b;
}
