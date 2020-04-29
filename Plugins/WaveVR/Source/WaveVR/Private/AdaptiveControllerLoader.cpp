// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "AdaptiveControllerLoader.h"
#include "WaveVRPrivatePCH.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/Json/Public/Json.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"

TSharedPtr<FJsonObject> UAdaptiveControllerLoader::TouchJsonParsed = nullptr;
FString UAdaptiveControllerLoader::mRenderModelPath = FString(TEXT(""));
bool UAdaptiveControllerLoader::mSpawnControllerFlag = false;
bool UAdaptiveControllerLoader::mLoadingTextureRet = false;
bool UAdaptiveControllerLoader::mSpawnActorRet = false;
bool UAdaptiveControllerLoader::mUseNewModel = false;
bool UAdaptiveControllerLoader::isReadTouchPadJson = false;
FString UAdaptiveControllerLoader::TouchPadContent = FString(TEXT(""));
bool UAdaptiveControllerLoader::isReadBatteryJson = false;
int UAdaptiveControllerLoader::mLevels = 0;
FString UAdaptiveControllerLoader::BatteryContent = FString(TEXT(""));
UTexture2D* UAdaptiveControllerLoader::mTexture = NULL;

DEFINE_LOG_CATEGORY(LogAdapCtrLoader);

UAdaptiveControllerLoader::UAdaptiveControllerLoader() {
}

bool UAdaptiveControllerLoader::doDeployControllerModel(EWVR_DeviceType type) {
	bool ret = false;
	mRenderModelPath = FString(TEXT(""));

	if (FWaveVRAPIWrapper::GetInstance()->GetWaveRuntimeVersion() < 2) {
		LOGE(LogAdapCtrLoader, "API Level(2) is larger than Runtime Version(%d).", FWaveVRAPIWrapper::GetInstance()->GetWaveRuntimeVersion());
		mSpawnActorRet = false;
		return false;
	}

	bool bIsLeftHanded = UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode();
	FString str_name = FString(TEXT("GetRenderModelName")); //Controller getParameter
	char *str = new char[64];
	FString index_name = FString(TEXT("backdoor_get_device_index"));
	char *deviceIndex = new char[2];

	if (bIsLeftHanded) {
		if (type == EWVR_DeviceType::DeviceType_Controller_Right) {
			if (!UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(EWVR_DeviceType::DeviceType_Controller_Left)) {
				LOGE(LogAdapCtrLoader, "%s", "left device is not connected!");
				return false;
			}
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_UTF8(*str_name), str, 64);
			FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_UTF8(*index_name), deviceIndex, 2);
		}
		else if (type == EWVR_DeviceType::DeviceType_Controller_Left) {
			if (!UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(EWVR_DeviceType::DeviceType_Controller_Right)) {
				LOGE(LogAdapCtrLoader, "%s", "right device is not connected!");
				return false;
			}
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_UTF8(*str_name), str, 64);
			FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_UTF8(*index_name), deviceIndex, 2);
		}
	}
	else {
		if (type == EWVR_DeviceType::DeviceType_Controller_Right) {
			if (!UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(EWVR_DeviceType::DeviceType_Controller_Right)) {
				LOGE(LogAdapCtrLoader, "%s", "right device is not connected!");
				return false;
			}
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_UTF8(*str_name), str, 64);
			FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_UTF8(*index_name), deviceIndex, 2);
		}
		else if (type == EWVR_DeviceType::DeviceType_Controller_Left) {
			if (!UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(EWVR_DeviceType::DeviceType_Controller_Left)) {
				LOGE(LogAdapCtrLoader, "%s", "left device is not connected!");
				return false;
			}
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_UTF8(*str_name), str, 64);
			FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_UTF8(*index_name), deviceIndex, 2);
		}
	}

	int devIndex = atoi(deviceIndex);
	LOGI(LogAdapCtrLoader, "doDeployControllerModel model: %s, index: %d", str, devIndex);

	if (devIndex == -1) {
		LOGE(LogAdapCtrLoader, "%s", "doDeployControllerModel, device index not found");
		return false;
	}

	FString renderModelUnzipFolder = FString(UTF8_TO_TCHAR(str)) + "/";
	LOGI(LogAdapCtrLoader, "renderModelUnzipFolder = %s", PLATFORM_CHAR(*renderModelUnzipFolder));
	std::string inputStr(TCHAR_TO_UTF8(*renderModelUnzipFolder));
	std::string retStr = FWaveVRAPIWrapper::GetInstance()->DeployRenderModelAssets(devIndex, inputStr);

	if (retStr == "") {
		ret = false;
	}
	else {
		ret = true;
		mRenderModelPath = FString(UTF8_TO_TCHAR(retStr.c_str()));
	}
	LOGI(LogAdapCtrLoader, "renderModelPath = %s", PLATFORM_CHAR(*mRenderModelPath));
	delete[] deviceIndex;
	return ret;
}

FString UAdaptiveControllerLoader::GetRenderModelPath() {
	return mRenderModelPath;
}

bool UAdaptiveControllerLoader::GetSpawnAdaptiveControllerFlag() {
	return mSpawnControllerFlag;
}

void UAdaptiveControllerLoader::SetSpawnAdaptiveControllerFlag(bool flag) {
	mSpawnControllerFlag = flag;
}

bool UAdaptiveControllerLoader::GetSpawnActorResult() {
	return mSpawnActorRet;
}

void UAdaptiveControllerLoader::SetSpawnActorResult(bool ret) {
	mSpawnActorRet = ret;
}

bool UAdaptiveControllerLoader::GetUseNewModel() {
	return mUseNewModel;
}

void UAdaptiveControllerLoader::SetUseNewModel(bool ret) {
	mUseNewModel = ret;
}

bool UAdaptiveControllerLoader::GetLoadingTexture2DResult() {
	return mLoadingTextureRet;
}

UTexture2D* UAdaptiveControllerLoader::GetTexture2DFromFile() {
	TArray<uint8> RawFileData;
	mTexture = NULL;

	FString PathStr = "";
	mLoadingTextureRet = false;

	std::string retStr = FWaveVRAPIWrapper::GetInstance()->GetRootRelativePath();
	PathStr = UTF8_TO_TCHAR(retStr.c_str());

	LOGI(LogAdapCtrLoader, "GetRootRelativePath PathStr: %s", retStr.c_str());

	FString FilePath = "";
	if(mRenderModelPath.Contains(FString(TEXT("Unreal")), ESearchCase::CaseSensitive, ESearchDir::FromEnd)) {
		FilePath = PathStr + mRenderModelPath + "Unreal.png";
	} else {
		FilePath = PathStr + mRenderModelPath + "controller00.png";
	}
	LOGI(LogAdapCtrLoader, "GetTexture2DFromFile FilePath: %s", PLATFORM_CHAR(*FilePath));
	if (FFileHelper::LoadFileToArray(RawFileData, *FilePath /*"<path to file>"*/))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		// Note: PNG format.  Other formats are supported
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			const TArray<uint8>* UncompressedBGRA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				// Create the UTexture for rendering
				mTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

				// Fill in the source data from the file
				void* TextureData = mTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				mTexture->PlatformData->Mips[0].BulkData.Unlock();

				// Update the rendering resource from data.
				mTexture->UpdateResource();
				mLoadingTextureRet = true;
			}
			else {
				LOGE(LogAdapCtrLoader, "ImageWrapper get raw fail: %s", PLATFORM_CHAR(*FilePath));
			}
		}
		else {
			LOGE(LogAdapCtrLoader, "ImageWrapper is either invalid or SetCompressed: %s", PLATFORM_CHAR(*FilePath));
		}
	}
	else {
		LOGE(LogAdapCtrLoader, "cant load PNG file, path = %s", PLATFORM_CHAR(*FilePath));
	}
	return mTexture;
}

float UAdaptiveControllerLoader::GetTouchPadFloatingHeight() {
	float height = 0.0f;

	if (!isReadTouchPadJson) {
		isReadTouchPadJson = GetTouchPadJson();
	}

	if (TouchJsonParsed.IsValid()) {
		bool tmpCat = TouchJsonParsed->HasField("FloatingDistance");
		if (tmpCat) {
			double x = TouchJsonParsed->GetNumberField("FloatingDistance");
			LOGI(LogAdapCtrLoader, "GetTouchPadFloatingHeight -> %f", x);
			height = x;
		}
	}

	return height;
}

bool UAdaptiveControllerLoader::GetTouchPadInfo(EWVR_TouchpadIndex index, FVector& vec) {
	vec = FVector(0.0, 0.0, 0.0);
	bool ret = false;

	if (!isReadTouchPadJson)
		isReadTouchPadJson = GetTouchPadJson();

	if (TouchJsonParsed.IsValid()) {
		switch (index) {
		case EWVR_TouchpadIndex::EWVR_TouchpadIndex_Up:  // forward
		{
			bool tmpCat = TouchJsonParsed->HasField("up");
			if (tmpCat) {
				TSharedPtr<FJsonObject> categoryObject = TouchJsonParsed->GetObjectField("up");
				//key field
				bool xKey = categoryObject->HasField("x");
				bool yKey = categoryObject->HasField("y");
				bool zKey = categoryObject->HasField("z");
				if (xKey && yKey && zKey) {
					double x = categoryObject->GetNumberField("x");
					double y = categoryObject->GetNumberField("y");
					double z = categoryObject->GetNumberField("z");
					vec.X = x;
					vec.Y = y;
					vec.Z = z;
					ret = true;
					LOGI(LogAdapCtrLoader, "GetTouchPadInfo Up -> %f, %f, %f", vec.X, vec.Y, vec.Z);
				}
				else {
					LOGE(LogAdapCtrLoader, "%s", "up xyz not found.");
				}
			}
		}
			break;
		case EWVR_TouchpadIndex::EWVR_TouchpadIndex_Right: // right
		{
			bool tmpCat = TouchJsonParsed->HasField("right");
			if (tmpCat) {
				TSharedPtr<FJsonObject> categoryObject = TouchJsonParsed->GetObjectField("right");
				//key field
				bool xKey = categoryObject->HasField("x");
				bool yKey = categoryObject->HasField("y");
				bool zKey = categoryObject->HasField("z");
				if (xKey && yKey && zKey) {
					double x = categoryObject->GetNumberField("x");
					double y = categoryObject->GetNumberField("y");
					double z = categoryObject->GetNumberField("z");
					vec.X = x;
					vec.Y = y;
					vec.Z = z;
					ret = true;
					LOGI(LogAdapCtrLoader, "GetTouchPadInfo Right -> %f, %f, %f", vec.X, vec.Y, vec.Z);
				}
				else {
					LOGE(LogAdapCtrLoader, "%s", "right xyz not found.");
				}
			}
		}
			break;
		case EWVR_TouchpadIndex::EWVR_TouchpadIndex_Center: // center
		{
			bool tmpCat = TouchJsonParsed->HasField("center");
			if (tmpCat) {
				TSharedPtr<FJsonObject> categoryObject = TouchJsonParsed->GetObjectField("center");
				//key field
				bool xKey = categoryObject->HasField("x");
				bool yKey = categoryObject->HasField("y");
				bool zKey = categoryObject->HasField("z");
				if (xKey && yKey && zKey) {
					double x = categoryObject->GetNumberField("x");
					double y = categoryObject->GetNumberField("y");
					double z = categoryObject->GetNumberField("z");
					vec.X = x;
					vec.Y = y;
					vec.Z = z;
					ret = true;
					LOGI(LogAdapCtrLoader, "GetTouchPadInfo Center -> %f, %f, %f", vec.X, vec.Y, vec.Z);
				}
				else {
					LOGE(LogAdapCtrLoader, "%s", "center xyz not found.");
				}
			}
		}
			break;
		default:
			break;
		}
	}

	return ret;
}

bool UAdaptiveControllerLoader::GetTouchPadJson() {
	bool ret = false;
	FString PathStr = "";
	FString FilePath = "";
	std::string retStr = FWaveVRAPIWrapper::GetInstance()->GetRootRelativePath();
	PathStr = UTF8_TO_TCHAR(retStr.c_str());
	LOGI(LogAdapCtrLoader, "GetRootRelativePath PathStr: %s", retStr.c_str());
	if (mRenderModelPath.Contains(FString(TEXT("Model")), ESearchCase::CaseSensitive, ESearchDir::FromEnd)) {
		FilePath = PathStr + mRenderModelPath + "Touchpad.json";

		FFileHelper::LoadFileToString(TouchPadContent, *FilePath);

		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(TouchPadContent);;
		if (!FJsonSerializer::Deserialize(Reader, TouchJsonParsed))
		{
			LOGE(LogAdapCtrLoader, "%s", "JSON Parse error.");
		}
		else {
			LOGE(LogAdapCtrLoader, "%s", "JSON Parse success.");
		}
		LOGI(LogAdapCtrLoader, "GetTouchPadJson content: %s", PLATFORM_CHAR(*TouchPadContent));

		ret = true;
	}

	return ret;
}

bool UAdaptiveControllerLoader::IsBatteryInfo() {
	bool ret = false;

	return ret;
}

bool UAdaptiveControllerLoader::GetBatteryLevels(int& levels) {
	levels = mLevels;
	bool ret = false;

	if (!isReadBatteryJson)
		isReadBatteryJson = GetBatteryJson();

	if (isReadBatteryJson) {

		FBatteryInfo JsonData;
		bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct<FBatteryInfo>(
			BatteryContent,
			&JsonData,
			0, 0);

		if (parsed) {
			mLevels = JsonData.LevelCount;
			levels = mLevels;
			ret = true;
		}
		LOGI(LogAdapCtrLoader, "GetBatteryLevels levels %d", levels);
	}

	return ret;
}

bool UAdaptiveControllerLoader::GetBatteryLevelInfo(int level, float& min, float& max, UTexture2D*& levelTex) {
	bool ret = false;
	if (level < 0)
		return ret;

	if (!isReadBatteryJson)
		isReadBatteryJson = GetBatteryJson();

	if (isReadBatteryJson) {
		FBatteryInfo JsonData;
		bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct<FBatteryInfo>(
			BatteryContent,
			&JsonData,
			0, 0);

		if (parsed) {
			mLevels = JsonData.LevelCount;

			if (level >= mLevels) {
				LOGE(LogAdapCtrLoader, "level %d > mLevel %d", level, mLevels);
				return ret;
			}

			if (level > JsonData.BatteryLevel.Num()) {
				LOGE(LogAdapCtrLoader, "level %d > JsonData.BatteryLevel.Num() %d, return", level, JsonData.BatteryLevel.Num());
				return ret;
			}

			min = JsonData.BatteryLevel[level].min;
			max = JsonData.BatteryLevel[level].max;

			TArray<uint8> RawFileData;

			FString PathStr = "";
			std::string retStr = FWaveVRAPIWrapper::GetInstance()->GetRootRelativePath();
			PathStr = UTF8_TO_TCHAR(retStr.c_str());
			LOGD(LogAdapCtrLoader, "GetRootRelativePath PathStr: %s", retStr.c_str());
			FString FilePath = PathStr + mRenderModelPath + JsonData.BatteryLevel[level].path;
			LOGI(LogAdapCtrLoader, "GetBatteryLevelInfo FilePath: %s", PLATFORM_CHAR(*FilePath));
			if (FFileHelper::LoadFileToArray(RawFileData, *FilePath /*"<path to file>"*/))
			{
				IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
				// Note: PNG format.  Other formats are supported
				TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
				{
					const TArray<uint8>* UncompressedBGRA = NULL;
					if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
					{
						// Create the UTexture for rendering
						levelTex = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

						// Fill in the source data from the file
						void* TextureData = levelTex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
						FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
						levelTex->PlatformData->Mips[0].BulkData.Unlock();

						// Update the rendering resource from data.
						levelTex->UpdateResource();
						ret = true;
						LOGI(LogAdapCtrLoader, "GetBatteryLevels level: %d of levels %d", level, JsonData.LevelCount);
						LOGI(LogAdapCtrLoader, "GetBatteryLevels level: %d min: %d max %d", level, JsonData.BatteryLevel[level].min, JsonData.BatteryLevel[level].max);
						LOGI(LogAdapCtrLoader, "GetBatteryLevels level: %d path: %s", level, PLATFORM_CHAR(*JsonData.BatteryLevel[level].path));
						LOGI(LogAdapCtrLoader, "GetBatteryLevels level: %d texture height: %d, texture width: %d", level, levelTex->PlatformData->SizeY, levelTex->PlatformData->SizeX);
					}
					else {
						LOGE(LogAdapCtrLoader, "ImageWrapper get raw fail: %s", PLATFORM_CHAR(*FilePath));
					}
				}
				else {
					LOGE(LogAdapCtrLoader, "ImageWrapper is either invalid or SetCompressed: %s", PLATFORM_CHAR(*FilePath));
				}
			}
			else {
				LOGE(LogAdapCtrLoader, "cant load PNG file, path = %s", PLATFORM_CHAR(*FilePath));
			}
		}
	}

	return ret;
}

bool UAdaptiveControllerLoader::GetBatteryJson()
{
	bool ret = false;
	FString PathStr = "";
	FString FilePath = "";

	std::string retStr = FWaveVRAPIWrapper::GetInstance()->GetRootRelativePath();
	PathStr = UTF8_TO_TCHAR(retStr.c_str());
	LOGD(LogAdapCtrLoader, "GetRootRelativePath PathStr: %s", retStr.c_str());
	FilePath = PathStr + mRenderModelPath + "BatteryIndicator.json";
	LOGI(LogAdapCtrLoader, "GetBatteryJson FilePath: %s", PLATFORM_CHAR(*FilePath));

	if (mRenderModelPath.Contains(FString(TEXT("Model")), ESearchCase::CaseSensitive, ESearchDir::FromEnd)) {
		FilePath = PathStr + mRenderModelPath + "BatteryIndicator.json";

		ret = FFileHelper::LoadFileToString(BatteryContent, *FilePath);
	}

	return ret;
}
