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
#include "Components/ActorComponent.h"
#include "CtrlrSwipeEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeLRDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeRLDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeUDDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeDUDelNative);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeLRDelToBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeRLDelToBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeUDDelToBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeDUDelToBP);

UCLASS(ClassGroup = (WaveVR), meta = (BlueprintSpawnableComponent))
class WAVEVR_API UCtrlrSwipeEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCtrlrSwipeEvent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void receiveCtrlrSwipeLRUpdateFromNative();
	UFUNCTION()
		void receiveCtrlrSwipeRLUpdateFromNative();
	UFUNCTION()
		void receiveCtrlrSwipeUDUpdateFromNative();
	UFUNCTION()
		void receiveCtrlrSwipeDUUpdateFromNative();

	static FCtrlrSwipeLRDelNative onCtrlrSwipeLtoRUpdateNative;
	static FCtrlrSwipeRLDelNative onCtrlrSwipeRtoLUpdateNative;
	static FCtrlrSwipeUDDelNative onCtrlrSwipeUtoDUpdateNative;
	static FCtrlrSwipeDUDelNative onCtrlrSwipeDtoUUpdateNative;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeLRDelToBP WaveVR_onCtrlrSwipeLRUpdate;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeRLDelToBP WaveVR_onCtrlrSwipeRLUpdate;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeUDDelToBP WaveVR_onCtrlrSwipeUDUpdate;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeDUDelToBP WaveVR_onCtrlrSwipeDUUpdate;
};
