// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRResourceWrapper.h"
#include "WaveVRPrivatePCH.h"

DEFINE_LOG_CATEGORY(LogResourceWrapper);

ResourceWrapperImpl* ResourceWrapperImpl::mInstance = nullptr;

ResourceWrapperImpl::ResourceWrapperImpl() {
	mPreferredLanguage = FString(TEXT("system"));
	mCountry = FString(TEXT("system"));
	useSystemLanguageFlag = true;
}

ResourceWrapperImpl* ResourceWrapperImpl::getInstance() {
	if (mInstance == nullptr) {
		mInstance = new ResourceWrapperImpl();
	}
	return mInstance;
}

FString ResourceWrapperImpl::getString(FString stringName) {
	LOG_FUNC();
	std::string inputStr(TCHAR_TO_UTF8(*stringName));
	std::string nativeStr;

	if (useSystemLanguageFlag == true)
	{
		nativeStr = FWaveVRAPIWrapper::GetInstance()->GetStringBySystemLanguage(inputStr);
	}
	else
	{
		std::string lang(TCHAR_TO_UTF8(*mPreferredLanguage));
		std::string country(TCHAR_TO_UTF8(*mCountry));

		nativeStr = FWaveVRAPIWrapper::GetInstance()->GetStringByLanguage(inputStr, lang, country);
	}

	FString ret(UTF8_TO_TCHAR(nativeStr.c_str()));
	LOGI(LogResourceWrapper, "Get string %s from native is %s", PLATFORM_CHAR(*stringName), PLATFORM_CHAR(*ret));

	return ret;
}

FString ResourceWrapperImpl::getStringByLanguage(FString stringName, FString lang, FString country) {
	LOG_FUNC();
	std::string inputStr(TCHAR_TO_UTF8(*stringName));
	std::string inLang(TCHAR_TO_UTF8(*lang));
	std::string inCountry(TCHAR_TO_UTF8(*country));
	std::string nativeStr;

	nativeStr = FWaveVRAPIWrapper::GetInstance()->GetStringByLanguage(inputStr, inLang, inCountry);
	FString ret(UTF8_TO_TCHAR(nativeStr.c_str()));
	LOGI(LogResourceWrapper, "Get string %s lang %s country %s from native is %s", PLATFORM_CHAR(*stringName), PLATFORM_CHAR(*lang), PLATFORM_CHAR(*country), nativeStr.c_str());
	return ret;
}

FString ResourceWrapperImpl::getSystemLanguage() {
	LOG_FUNC();
	FString retStr(UTF8_TO_TCHAR(FWaveVRAPIWrapper::GetInstance()->GetSystemLanguage().c_str()));
	LOGI(LogResourceWrapper, "SystemLanguage is %s", PLATFORM_CHAR(*retStr));
	return retStr;
}

FString ResourceWrapperImpl::getSystemCountry() {
	LOG_FUNC();
	FString retStr(UTF8_TO_TCHAR(FWaveVRAPIWrapper::GetInstance()->GetSystemCountry().c_str()));
	//LOGI(LogResourceWrapper, "SystemCountry is %s", PLATFORM_CHAR(*retStr));
	return retStr;
}

bool ResourceWrapperImpl::setPreferredLanguage(FString lang, FString country) {
	useSystemLanguageFlag = false;
	mPreferredLanguage = lang;
	mCountry = country;
	return true;
}

void ResourceWrapperImpl::useSystemLanguage() {
	mPreferredLanguage = FString(TEXT("system"));
	mCountry = FString(TEXT("system"));
	useSystemLanguageFlag = true;
}

// -----------------------------------------------------------------

FString UWaveVRResourceWrapper::WaveVR_GetString(FString stringName) {
	return ResourceWrapperImpl::getInstance()->getString(stringName);
}

FString UWaveVRResourceWrapper::WaveVR_GetStringByLanguage(FString stringName, FString lang, FString country) {
	return ResourceWrapperImpl::getInstance()->getStringByLanguage(stringName, lang, country);
}

FString UWaveVRResourceWrapper::WaveVR_GetSystemLanguage() {
	return ResourceWrapperImpl::getInstance()->getSystemLanguage();
}

FString UWaveVRResourceWrapper::WaveVR_GetSystemCountry() {
	return ResourceWrapperImpl::getInstance()->getSystemCountry();
}

bool UWaveVRResourceWrapper::WaveVR_SetPreferredLanguage(FString lang, FString country) {
	return ResourceWrapperImpl::getInstance()->setPreferredLanguage(lang, country);
}

void UWaveVRResourceWrapper::WaveVR_UseSystemLanguage() {
	ResourceWrapperImpl::getInstance()->useSystemLanguage();
}

