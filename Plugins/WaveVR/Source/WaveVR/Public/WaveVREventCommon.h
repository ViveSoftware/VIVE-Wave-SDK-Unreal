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
#include "WaveVRBlueprintFunctionLibrary.h"
#include "WaveVREventCommon.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVREventCommon, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsControllerRoleChangeNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsControllerRoleChangeBp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResumeNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResumeBp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTrackingModeChangeNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTrackingModeChangeBp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOEMConfigChangeNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOEMConfigChangeBp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAllCommonEventNative, int32, event_num);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAllCommonEventBp, int32, event_num);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTouchpadPressNative, bool, down);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTouchpadPressBp, bool, down);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAllButtonPressNative, uint8, button, bool, down);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAllButtonPressBp, uint8, button, bool, down);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAllEventPressNative, uint8, button, bool, down);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAllEventPressBp, uint8, button, bool, down);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAllEventTouchNative, uint8, button, bool, down);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAllEventTouchBp, uint8, button, bool, down);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHandChanging, bool, changing);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConnectionChangeNative, uint8, device, bool, connection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConnectionChangeBp, uint8, device, bool, connection);

#pragma region
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicGestureNative, uint8, type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicGestureBp, uint8, type);
#pragma endregion Gesture


static const int EventCommonButtonCount = 32;

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UWaveVREventCommon : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWaveVREventCommon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	static bool bInitialized;
	bool isFocusCapturedBySystem;

#pragma region
/// Call flow:
/// 1. Register handling function(s) to OnControllerRoleChangeNative
///		- function 1: OnControllerRoleChangeHandling
/// 2. WaveVRHMD calls OnControllerRoleChangeNative
/// 3. OnControllerRoleChangeNative calls handling function(s)
///		- function 1: OnControllerRoleChangeHandling
/// 4. OnControllerRoleChangeHandling is called, then call to OnControllerRoleChangeBp
public:
	static FSettingsControllerRoleChangeNative OnControllerRoleChangeNative;

	// OnControllerRoleChangeNative -> OnControllerRoleChangeHandling
	UFUNCTION()
	void OnControllerRoleChangeHandling();

	// OnControllerRoleChangeHandling -> OnControllerRoleChangeBp
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FSettingsControllerRoleChangeBp OnControllerRoleChangeBp;

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Event",
		meta = (ToolTip = "To check if the VR environment is left-handed mode."))
	static bool IsLeftHandedMode();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Event",
		meta = (ToolTip = "To notify the engine to check the left-handed mode on resume."))
	static void ShouldCheckLeftHandedOnResume(bool check);

	// To notify the engine to reset all button states on resume.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Event")
	static void ShouldResetButtonStatesOnResume(bool reset);

private:
	static bool bIsLeftHanded;
	static bool bCheckLeftHanded;
	static bool bResetButtonStates;
#pragma endregion WVR_EventType_DeviceRoleChanged

#pragma region
public:
	static FOnResumeNative OnResumeNative;

	UFUNCTION()
	void OnResumeHandling();

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Application")
	FOnResumeBp OnResumeBp;
#pragma endregion On Application Resume

#pragma region
public:
	static FTrackingModeChangeNative OnTrackingModeChangeNative;

	UFUNCTION()
	void OnTrackingModeChangeHandling();

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FTrackingModeChangeBp OnTrackingModeChangeBp;

	static FOEMConfigChangeNative OnOEMConfigChangeNative;

	UFUNCTION()
	void OnOEMConfigChangeHandling();

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FOEMConfigChangeBp OnOEMConfigChangeBp;

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Event",
		meta = (ToolTip = "To notify the engine to check the Degree-Of-Freedom on resume."))
	static void ShouldCheckDoFOnResume(bool check);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Event",
		meta = (ToolTip = "To get the Degree-Of-Freedom."))
	static EWVR_DOF GetDegreeOfFreedom();

private:
	static bool bCheckTrackingMode;
	static EWVR_DOF currentDoF;
#pragma endregion Tracking Mode Change

#pragma region
public:
	static FAllCommonEventNative OnAllEventNative;

	UFUNCTION()
	void OnAllEventHandling(int32 event_num);

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FAllCommonEventBp OnAllEventBp;
#pragma endregion All Common Event

#pragma region
public:
	static FHandChanging OnHandChangingNative;
	UFUNCTION()
	void OnHandChangingHandling(bool changing);		// true: changing, false: not changing
	UFUNCTION()
	bool IsHandChanging();
private:
	bool ChangingHand;
#pragma endregion Change Hand

#pragma region
public:
	static FConnectionChangeNative OnConnectionChangeNative;
	UFUNCTION()
	void OnConnectionChangeHandling(uint8 device, bool connection);

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Connection")
	FConnectionChangeBp OnConnectionChangeBp;
#pragma endregion Connection Change

#pragma region
public:
	static FTouchpadPressNative OnTouchpadPressNative_Right;
	static FTouchpadPressNative OnTouchpadPressNative_Left;
	static FAllButtonPressNative OnAllButtonPressNative_Right;
	static FAllButtonPressNative OnAllButtonPressNative_Left;

	static FAllEventPressNative OnAllEventPressNative_HMD;
	static FAllEventPressNative OnAllEventPressNative_Right;
	static FAllEventPressNative OnAllEventPressNative_Left;

	static FAllEventTouchNative OnAllEventTouchNative_HMD;
	static FAllEventTouchNative OnAllEventTouchNative_Right;
	static FAllEventTouchNative OnAllEventTouchNative_Left;

	UFUNCTION()
	void OnTouchpadPressHandling_Right(bool down);	// true: down, false: up
	UFUNCTION()
	void OnTouchpadPressHandling_Left(bool down);	// true: down, false: up
	UFUNCTION()
	void OnAllButtonPressHandling_Right(uint8 button, bool down);
	UFUNCTION()
	void OnAllButtonPressHandling_Left(uint8 button, bool down);

	UFUNCTION()
	void OnAllEventPressHandling_HMD(uint8 button, bool down);
	UFUNCTION()
	void OnAllEventPressHandling_Right(uint8 button, bool down);
	UFUNCTION()
	void OnAllEventPressHandling_Left(uint8 button, bool down);

	UFUNCTION()
	void OnAllEventTouchHandling_HMD(uint8 button, bool down);
	UFUNCTION()
	void OnAllEventTouchHandling_Right(uint8 button, bool down);
	UFUNCTION()
	void OnAllEventTouchHandling_Left(uint8 button, bool down);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Button",
		meta = (ToolTip = "To check if a button on a device is pressed."))
	static bool IsControllerButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Button",
		meta = (ToolTip = "To check if a button on a device is touched."))
	static bool IsControllerButtonTouched(EWVR_DeviceType device, EWVR_TouchId button_id);

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FTouchpadPressBp OnTouchpadPressBp_Right;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FTouchpadPressBp OnTouchpadPressBp_Left;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FAllButtonPressBp OnAllButtonPressBp_Right;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FAllButtonPressBp OnAllButtonPressBp_Left;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FAllButtonPressBp OnAllEventPressBp_HMD;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FAllButtonPressBp OnAllEventPressBp_Right;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Button")
	FAllButtonPressBp OnAllEventPressBp_Left;

private:
	static bool isBtnPress_HMD(EWVR_InputId button);
	static bool isBtnPress_right(EWVR_InputId button);
	static bool isBtnPress_left(EWVR_InputId button);

	static bool isBtnTouch_HMD(EWVR_TouchId button);
	static bool isBtnTouch_right(EWVR_TouchId button);
	static bool isBtnTouch_left(EWVR_TouchId button);

	static bool btnPress_HMD[EventCommonButtonCount];
	static bool btnPress_right[EventCommonButtonCount];
	static bool btnPress_left[EventCommonButtonCount];

	static bool btnTouch_HMD[EventCommonButtonCount];
	static bool btnTouch_right[EventCommonButtonCount];
	static bool btnTouch_left[EventCommonButtonCount];
#pragma endregion Button Event
#pragma region
public:
	static FDynamicGestureNative OnDynamicGestureNative_Left;
	static FDynamicGestureNative OnDynamicGestureNative_Right;
	UFUNCTION()
	void OnDynamicGestureHandling_Left(uint8 type);
	UFUNCTION()
	void OnDynamicGestureHandling_Right(uint8 type);
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Gesture")
	FDynamicGestureBp OnDynamicGestureBp_Left;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Gesture")
	FDynamicGestureBp OnDynamicGestureBp_Right;
#pragma endregion Gesture
};
