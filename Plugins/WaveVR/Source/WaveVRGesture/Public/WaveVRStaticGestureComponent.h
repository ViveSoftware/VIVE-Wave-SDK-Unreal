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
#include "WaveVRGestureEnums.h"
#include "WaveVRStaticGestureComponent.generated.h"


DEFINE_LOG_CATEGORY_STATIC(LogWaveVRStaticGestureComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStaticGestureNative, EWaveVRStaticGesture, type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStaticGestureBp, EWaveVRStaticGesture, type);

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVRGESTURE_API UWaveVRStaticGestureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWaveVRStaticGestureComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region
public:
	static FStaticGestureNative OnStaticGestureNative_Left;
	static FStaticGestureNative OnStaticGestureNative_Right;

	UFUNCTION()
	void OnStaticGestureHandling_Left(EWaveVRStaticGesture type);
	UFUNCTION()
	void OnStaticGestureHandling_Right(EWaveVRStaticGesture type);

	UPROPERTY(BlueprintAssignable, Category = "WaveVR | Gesture")
	FStaticGestureBp OnStaticGestureBp_Left;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR | Gesture")
	FStaticGestureBp OnStaticGestureBp_Right;
#pragma endregion Gesture events.


};
