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
#include "RequestResultObject.generated.h"
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelToBP, TArray<FString>, requestPerArr, TArray<bool>, resultArr);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelNative, TArray<FString>, requestPerArr, TArray<bool>, resultArr);
//void receiveRequestResultFromNative(TArray<FString> requestPerArr, TArray<bool> resultArr);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelNative, const TArray<FString>&, requestPerArr, const TArray<bool>&, resultArr);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelToBP, const TArray<FString>&, requestPerArr, const TArray<bool>&, resultArr);

/**
 *
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API URequestResultObject : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URequestResultObject();
	virtual ~URequestResultObject();

	UFUNCTION()
	void receiveRequestResultFromNative(const TArray<FString>& requestPerArr, const TArray<bool>& resultArr);

	UFUNCTION()
	void receiveSignal();
	UFUNCTION()
	void removeSignal();

	static FRequestResultDelNative onRequestResultNative;

	void setResult(bool result);

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Permission")
	FRequestResultDelToBP onRequestResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WaveVR|Permission")
	bool Result;
};
