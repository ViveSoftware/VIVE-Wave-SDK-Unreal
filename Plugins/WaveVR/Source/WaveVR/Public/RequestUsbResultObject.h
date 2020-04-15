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
#include "UObject/NoExportTypes.h"
#include "RequestUsbResultObject.generated.h"
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelToBP, TArray<FString>, requestPerArr, TArray<bool>, resultArr);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelNative, TArray<FString>, requestPerArr, TArray<bool>, resultArr);
//void receiveRequestResultFromNative(TArray<FString> requestPerArr, TArray<bool> resultArr);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestUsbResultDelNative, const bool, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestUsbResultDelToBP,  const bool, result);

/**
 *
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API URequestUsbResultObject : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URequestUsbResultObject();
	virtual ~URequestUsbResultObject();

	UFUNCTION()
	void receiveRequestUsbResultFromNative(const bool result);

	UFUNCTION()
	void receiveSignal();
	UFUNCTION()
	void removeSignal();

	static FRequestUsbResultDelNative onRequestUsbResultNative;

	void setResult(bool result);

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Permission")
	FRequestUsbResultDelToBP onRequestUsbResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WaveVR|Permission")
	bool Result;
};
