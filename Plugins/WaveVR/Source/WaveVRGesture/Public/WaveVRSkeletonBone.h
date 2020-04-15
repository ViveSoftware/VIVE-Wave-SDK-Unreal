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
#include "WaveVRGestureBPLibrary.h"
#include "WaveVRGestureEnums.h"
#include "WaveVRSkeletonBone.generated.h"


DEFINE_LOG_CATEGORY_STATIC(LogWaveVRSkeletonBone, Log, All);

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVRGESTURE_API UWaveVRSkeletonBone : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWaveVRSkeletonBone();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "WaveVR|Gesture|Bone")
	bool Enabled;

	UPROPERTY(EditAnywhere, Category = "WaveVR|Gesture|Bone")
	EWaveVRGestureBoneType BoneId;

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture|Bone",
		meta = (ToolTip = "To get the bone location."))
	FVector GetLocation();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture|Bone",
		meta = (ToolTip = "To get the bone rotation. (only the Wrist is available)"))
	FRotator GetRotation();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture|Bone",
		meta = (ToolTip = "To check if the bone pose is valid."))
	bool IsValidPose();

private:
	FVector location;
	FRotator rotation;
	bool validPose;
	int frameCount;
};
