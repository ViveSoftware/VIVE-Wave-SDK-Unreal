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
//#include "WaveVRHMD.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BatteryStatusEvent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatteryStatusDelOneParam, float, percent);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatteryStatusDelToBPOneParam, float, percent);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBatteryStatusDelTwoParams, float, percent, int, device);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBatteryStatusDelToBPTwoParams, float, percent, int, device);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBatteryStatusDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBatteryStatusDelToBP);

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UBatteryStatusEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBatteryStatusEvent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void receiveStatusUpdateFromNative();

	static FBatteryStatusDelNative onBatteryStatusUpdateNative;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FBatteryStatusDelToBP WaveVR_onBatteryStatusUpdate;
};

//#endif
