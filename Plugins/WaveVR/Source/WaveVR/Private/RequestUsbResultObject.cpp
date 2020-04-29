// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "RequestUsbResultObject.h"
#include "WaveVRPrivatePCH.h"

FRequestUsbResultDelNative URequestUsbResultObject::onRequestUsbResultNative;

URequestUsbResultObject::URequestUsbResultObject() {
	Result = false;
	LOGI(LogTemp,"URequestUsbResultObject constructor");
	//URequestResultObject::onRequestResultNative.AddDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

URequestUsbResultObject::~URequestUsbResultObject() {
	//URequestResultObject::onRequestResultNative.RemoveDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

void URequestUsbResultObject::receiveRequestUsbResultFromNative(const bool result) {
	onRequestUsbResult.Broadcast(result);
	//removeSignal();
}

void URequestUsbResultObject::setResult(bool result) {
	Result = result;
}

void URequestUsbResultObject::receiveSignal() {
	URequestUsbResultObject::onRequestUsbResultNative.AddDynamic(this, &URequestUsbResultObject::receiveRequestUsbResultFromNative);
}

void URequestUsbResultObject::removeSignal() {
	URequestUsbResultObject::onRequestUsbResultNative.RemoveDynamic(this, &URequestUsbResultObject::receiveRequestUsbResultFromNative);
}
