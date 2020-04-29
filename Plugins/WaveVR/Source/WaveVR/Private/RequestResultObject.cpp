// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "RequestResultObject.h"
#include "WaveVRPrivatePCH.h"

FRequestResultDelNative URequestResultObject::onRequestResultNative;

URequestResultObject::URequestResultObject() {
	Result = false;
	LOGI(LogTemp,"URequestResultObject constructor");
	//URequestResultObject::onRequestResultNative.AddDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

URequestResultObject::~URequestResultObject() {
	//URequestResultObject::onRequestResultNative.RemoveDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

void URequestResultObject::receiveRequestResultFromNative(const TArray<FString>& requestPerArr, const TArray<bool>& resultArr) {
	int size = requestPerArr.Num();
	LOGI(LogTemp, "receiveRequestResultFromNative in URequestResultObject ----- %d", size);

	for (int i = 0; i < size; i++) {
		LOGI(LogTemp, "receiveRequestResultFromNative in URequestResultObject ----- %s %d", (char*)*requestPerArr[i], resultArr[i]);
	}

	onRequestResult.Broadcast(requestPerArr, resultArr);
	removeSignal();
}

void URequestResultObject::setResult(bool result) {
	Result = result;
}

void URequestResultObject::receiveSignal() {
	URequestResultObject::onRequestResultNative.AddDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

void URequestResultObject::removeSignal() {
	URequestResultObject::onRequestResultNative.RemoveDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}
