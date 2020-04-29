// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVROEMConfig.h"
#include "WaveVRPrivatePCH.h"

#include "Runtime/Json/Public/Json.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"

DEFINE_LOG_CATEGORY(LogOEMConfig);

#define CONTROLLER_PROPERTY_KEY "controller_property"
#define BATTERY_INDICATOR_KEY "battery_indicator"
#define SINGLE_BEAM_KEY "controller_singleBeam"

WaveVROEMConfigImpl* WaveVROEMConfigImpl::mInstance = nullptr;

WaveVROEMConfigImpl::WaveVROEMConfigImpl() {
	jsonString = FString(TEXT(""));
	jsonUpdate = false;

	updateJsonStr();
}

WaveVROEMConfigImpl* WaveVROEMConfigImpl::getInstance() {
	if (mInstance == nullptr) {
		mInstance = new WaveVROEMConfigImpl();
	}
	return mInstance;
}

FString WaveVROEMConfigImpl::GetConfig(FString category, FString key) {
	FString ret = FString(TEXT(""));
	if (category == "" || key == "") {
		return ret;
	}

	if (!jsonUpdate) {
		LOGE(LogOEMConfig, "JSON string didn't update");
		updateJsonStr();
	}


	if (JsonParsed.IsValid()) {
		//category field
		bool tmpCat = JsonParsed->HasField(category);
		if (tmpCat == false) {
			LOGE(LogOEMConfig, "Category %s not found.", PLATFORM_CHAR(*category));
			return ret;
		}
		TSharedPtr<FJsonObject> categoryObject = JsonParsed->GetObjectField(category);
		//key field
		bool tmpKey = categoryObject->HasField(key);
		if (tmpKey == false) {
			LOGE(LogOEMConfig, "Key %s not found.", PLATFORM_CHAR(*key));
			return ret;
		}
		TSharedPtr<FJsonValue> tmp = categoryObject->TryGetField(key);
		if (tmp->IsNull())
			return ret;
		else
			ret = categoryObject->GetStringField(key);
	}
	else {
		LOGE(LogOEMConfig, "JSON Parse is not valid.");
	}
	LOGI(LogOEMConfig, "GetConfig, category = %s, key = %s, value = %s", PLATFORM_CHAR(*category), PLATFORM_CHAR(*key), PLATFORM_CHAR(*ret));

	return ret;
}

bool WaveVROEMConfigImpl::GetVector(FString category, FString key, FVector& vec) {
	if (category == "" || key == "") {
		return false;
	}
	vec = FVector(0.0, 0.0, 0.0);
	if (!jsonUpdate) {
		LOGE(LogOEMConfig, "JSON string didn't update");
		updateJsonStr();
	}

	if (JsonParsed.IsValid()) {
		//category field
		bool tmpCat = JsonParsed->HasField(category);
		if (tmpCat == false) {
			LOGE(LogOEMConfig, "Category %s not found.", PLATFORM_CHAR(*category));
			return false;
		}
		TSharedPtr<FJsonObject> categoryObject = JsonParsed->GetObjectField(category);
		//key field
		bool tmpKey = categoryObject->HasField(key);
		if (tmpKey == false) {
			LOGE(LogOEMConfig, "Key %s not found.", PLATFORM_CHAR(*key));
			return false;
		}
		TArray<FString> ArrayJson;
		categoryObject->TryGetStringArrayField(key, ArrayJson);
		if (categoryObject->TryGetStringArrayField(key, ArrayJson) != false) {
			FVector tVector = FVector(
				FCString::Atof(*ArrayJson[0]),
				FCString::Atof(*ArrayJson[1]),
				FCString::Atof(*ArrayJson[2])
			);
			vec = tVector;
			LOGI(LogOEMConfig, "GetVector, category = %s, key = %s, value = %f %f %f", PLATFORM_CHAR(*category), PLATFORM_CHAR(*key), vec.X, vec.Y, vec.Z);
			return true;
		}
		else {
			LOGE(LogOEMConfig, "Value format is wrong.");
			return false;
		}
	}
	else {
		LOGE(LogOEMConfig, "JSON Parse is not valid.");
	}

	return false;
}

bool WaveVROEMConfigImpl::GetVector4(FString category, FString key, FVector4& vec4) {
	if (category == "" || key == "") {
		return false;
	}

	vec4 = FVector4(0.0, 0.0, 0.0, 0.0);
	if (!jsonUpdate) {
		LOGD(LogOEMConfig, "JSON string didn't update");
		updateJsonStr();
	}

	if (JsonParsed.IsValid()) {
		//category field
		bool tmpCat = JsonParsed->HasField(category);
		if (tmpCat == false) {
			LOGE(LogOEMConfig, "Category %s not found.", PLATFORM_CHAR(*category));
			return false;
		}
		TSharedPtr<FJsonObject> categoryObject = JsonParsed->GetObjectField(category);
		//key field
		bool tmpKey = categoryObject->HasField(key);
		if (tmpKey == false) {
			LOGE(LogOEMConfig, "Key %s not found.", PLATFORM_CHAR(*key));
			return false;
		}
		TArray<FString> ArrayJson;
		categoryObject->TryGetStringArrayField(key, ArrayJson);
		if (categoryObject->TryGetStringArrayField(key, ArrayJson) != false) {
			FVector4 Vector = FVector4(
				FCString::Atof(*ArrayJson[0]),
				FCString::Atof(*ArrayJson[1]),
				FCString::Atof(*ArrayJson[2]),
				FCString::Atof(*ArrayJson[3])
			);
			vec4 = Vector;
			LOGI(LogOEMConfig, "GetVector4, category = %s, key = %s, value = %f %f %f %f", PLATFORM_CHAR(*category), PLATFORM_CHAR(*key), vec4.X, vec4.Y, vec4.Z, vec4.W);
			return true;
		}
		else {
			LOGE(LogOEMConfig, "Value format is wrong.");
			return false;
		}
	}
	else {
		LOGE(LogOEMConfig, "JSON Parse is not valid.");
	}
	return false;
}

bool WaveVROEMConfigImpl::IsEnableSingleBeam() {
	bool ret = true;
	std::string JsonRawData = FWaveVRAPIWrapper::GetInstance()->GetOEMConfigRawData(SINGLE_BEAM_KEY);

	if (JsonRawData != "") {
		FOEnableSingleBeam JsonData;
		FString CurrJsonString = FString(UTF8_TO_TCHAR(JsonRawData.c_str()));
		bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct<FOEnableSingleBeam>(
			CurrJsonString,
			&JsonData,
			0, 0);

		if (parsed) {
			FString enable = JsonData.enable;
			ret = (enable.TrimStartAndEnd().Equals(TEXT("true"), ESearchCase::IgnoreCase)) ? true : false;
		}
	}

	LOGI(LogOEMConfig, "IsEnableSingleBeam = %d", ret);

	return ret;
}

bool WaveVROEMConfigImpl::IsBatteryInfo() {
	bool ret = false;
	std::string JsonRawData = FWaveVRAPIWrapper::GetInstance()->GetOEMConfigRawData(BATTERY_INDICATOR_KEY);

	if (JsonRawData != "") {
		FOBatterySetting JsonData;
		FString CurrJsonString = FString(UTF8_TO_TCHAR(JsonRawData.c_str()));
		bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct<FOBatterySetting>(
			CurrJsonString,
			&JsonData,
			0, 0);

		if (parsed) {
			int show = JsonData.show;

			ret = (show == 2) ? true : false;
		}
	}
	LOGI(LogOEMConfig, "IsBatteryInfo = %d", ret);

	return ret;
}

void WaveVROEMConfigImpl::updateJsonStr() {
	jsonUpdate = false;
	std::string JsonRawData = FWaveVRAPIWrapper::GetInstance()->GetOEMConfigRawData(CONTROLLER_PROPERTY_KEY);

	if (JsonRawData != "") {
		jsonString = UTF8_TO_TCHAR(JsonRawData.c_str());
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(jsonString);;

		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			jsonUpdate = true;
			LOGI(LogOEMConfig, "JSON Parse success.");
		}
		else {
			LOGE(LogOEMConfig, "JSON Parse failed.");
		}
	}
	else {
		LOGE(LogOEMConfig, "GetOEMConfigRawData is empty string.");
	}
}

//---------------------------------------------------------------------------
FString UWaveVROEMConfig::WaveVR_GetKeyValue(FString category, FString key) {
	return WaveVROEMConfigImpl::getInstance()->GetConfig(category, key);
}

bool UWaveVROEMConfig::WaveVR_GetKeyVector(FString category, FString key, FVector& vec) {
	return WaveVROEMConfigImpl::getInstance()->GetVector(category, key, vec);
}

bool UWaveVROEMConfig::WaveVR_GetKeyVector4(FString category, FString key, FVector4& vec4) {
	return WaveVROEMConfigImpl::getInstance()->GetVector4(category, key, vec4);
}

bool UWaveVROEMConfig::WaveVR_IsEnableSingleBeam() {
	return WaveVROEMConfigImpl::getInstance()->IsEnableSingleBeam();
}

bool UWaveVROEMConfig::WaveVR_IsBatteryInfo() {
	return WaveVROEMConfigImpl::getInstance()->IsBatteryInfo();
}
