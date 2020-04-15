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

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"
#include "Runtime/UMG/Public/Components/Widget.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/CheckBox.h"
#include "WaveVRPrivatePCH.h"
#include "WaveVRBlueprintFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWaveVRBPFunLib, Log, All);

static const unsigned int EWVR_DeviceType_Count = 4;
/** Defines the type of tracked devices*/
UENUM(BlueprintType)
enum class EWVR_DeviceType : uint8
{
	DeviceType_Invalid              = WVR_DeviceType::WVR_DeviceType_Invalid,
	DeviceType_HMD                  = WVR_DeviceType::WVR_DeviceType_HMD,
	DeviceType_Controller_Right     = WVR_DeviceType::WVR_DeviceType_Controller_Right,
	DeviceType_Controller_Left      = WVR_DeviceType::WVR_DeviceType_Controller_Left,
};

UENUM(BlueprintType)
enum class EWVR_InputId : uint8
{
	NoUse = WVR_InputId::WVR_InputId_Alias1_System,
	Menu = WVR_InputId::WVR_InputId_Alias1_Menu,
	Grip = WVR_InputId::WVR_InputId_Alias1_Grip,
	DPad_Left = WVR_InputId::WVR_InputId_Alias1_DPad_Left,
	DPad_Up = WVR_InputId::WVR_InputId_Alias1_DPad_Up,
	DPad_Right = WVR_InputId::WVR_InputId_Alias1_DPad_Right,
	DPad_Down = WVR_InputId::WVR_InputId_Alias1_DPad_Down,
	Volume_Up = WVR_InputId::WVR_InputId_Alias1_Volume_Up,
	Volume_Down = WVR_InputId::WVR_InputId_Alias1_Volume_Down,
	Digital_Trigger = WVR_InputId::WVR_InputId_Alias1_Digital_Trigger,
	Back = WVR_InputId::WVR_InputId_Alias1_Back,
	Enter = WVR_InputId::WVR_InputId_Alias1_Enter,
	Touchpad = WVR_InputId::WVR_InputId_Alias1_Touchpad,
	Trigger = WVR_InputId::WVR_InputId_Alias1_Trigger,
	Thumbstick = WVR_InputId::WVR_InputId_Alias1_Thumbstick,

	//Max = 32,
};

/// Note: if this value were changed, WaveVRInput would also be updated.
/// The input Ids listed here are Unreal needs, so no Digital Trigger
static const int InputButtonCount = 13;
static const EWVR_InputId InputButton[InputButtonCount] =
{
	EWVR_InputId::Menu,
	EWVR_InputId::Grip,
	EWVR_InputId::DPad_Left,
	EWVR_InputId::DPad_Up,
	EWVR_InputId::DPad_Right,	// 5
	EWVR_InputId::DPad_Down,
	EWVR_InputId::Volume_Up,
	EWVR_InputId::Volume_Down,
	EWVR_InputId::Back,
	EWVR_InputId::Enter,    // 10
	EWVR_InputId::Touchpad,
	EWVR_InputId::Trigger,
	EWVR_InputId::Thumbstick,
	//EWVR_InputId::Joystick
};

UENUM(BlueprintType)
enum class EWVR_TouchId : uint8
{
	NoUse = WVR_InputId::WVR_InputId_Alias1_System,
	Touchpad = WVR_InputId::WVR_InputId_Alias1_Touchpad,
	Trigger = WVR_InputId::WVR_InputId_Alias1_Trigger,
	Thumbstick = WVR_InputId::WVR_InputId_Alias1_Thumbstick,
};

/// Note: if this value were changed, WaveVRInput would also be updated.
static const int TouchButtonCount = 3;
static const EWVR_TouchId TouchButton[TouchButtonCount] =
{
	EWVR_TouchId::Touchpad,
	EWVR_TouchId::Trigger,
	EWVR_TouchId::Thumbstick,
	//EWVR_TouchId::Joystick
};

UENUM(BlueprintType)
enum class EWVR_Hand : uint8
{
	Hand_Controller_Right = 2,  // WVR_DeviceType::WVR_DeviceType_Controller_Right
	Hand_Controller_Left = 3    // WVR_DeviceType::WVR_DeviceType_Controller_Left
};

UENUM(BlueprintType)
enum class EWVR_DOF : uint8
{
	DOF_3,
	DOF_6,
	DOF_SYSTEM
};

UENUM(BlueprintType)
enum class EWVR_ControllerMode : uint8
{
	FixedBeam_Mode,
	FlexibleBeam_Mode,
	Mouse_Mode
};

UENUM(BlueprintType)
enum class EWVR_InputModule : uint8
{
	Controller,
	Gaze
};

UENUM(BlueprintType)
enum class SimulatePosition : uint8
{
	WhenNoPosition = 0,   // simulate when 3DoF.
	ForceSimulation = 1, // force using simulation pose.
	NoSimulation = 2
};

/**
* @brief Recenter effect of WVR_InAppRecenter
*/
UENUM(BlueprintType)
enum class ERecenterType: uint8
{
	Disabled            = 0,    /**< Make everything back to system coordinate instantly */
	YawOnly             = 1,    /**< Only adjust the Yaw angle */
	YawAndPosition      = 2,    /**< Adjust the Yaw angle, and also reset user's position to Virtual World's center */
	RotationAndPosition = 3     /**< Affect all the XYZ and Pitch Yaw Roll. It is sensitive to user's head gesture at recetnering. */
};

/**
* @brief UTexture2D format
*/
UENUM(BlueprintType)
enum class EUTex2DFmtType : uint8
{
	BMP     = 0,    /** Note: BMP Image format */
	PNG     = 1,    /** Note: PNG Image format */
	JPEG    = 2     /** Note: JPEG Image format */
};

/**
 * @brief Peripheral Quality which using in @ref WVR_RenderFoveationParams_t.
 */
UENUM(BlueprintType)
enum class EWVR_PeripheralQuality : uint8
{
	Low    = 0,    /**< **WVR_PeripheralQuality_High**: Peripheral quity is high and power saving is low. */
	Medium = 1,    /**< **WVR_PeripheralQuality_Medium**: Peripheral quity is medium and power saving is medium. */
	High   = 2     /**< **WVR_PeripheralQuality_Low**: Peripheral quity is low and power saving is high. */
};

UENUM(BlueprintType)
enum class EEye : uint8
{
	LEFT = 0,
	RIGHT = 1
};


/**
 * UE4 WaveVR_Screenshot
 */
UENUM(BlueprintType)
enum class EScreenshotMode : uint8
{
	DEFAULT = 0,
	RAW,
	DISTORTED
};

UCLASS()
class WAVEVR_API UWaveVRBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	// To get the position and rotation of the device with the specified type.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static bool GetDevicePose(FVector& OutPosition, FRotator& OutOrientation, EWVR_DeviceType Type = EWVR_DeviceType::DeviceType_HMD);

	// To enable or disable position and rotation prediction of the device. HMD always apply rotation prediction and cannot be disabled. HMD position prediction and controller pose prediction are disabled by default.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static void SetPosePredictEnabled(EWVR_DeviceType Type, bool enabled_position_predict, bool enabled_rotation_predict);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To get the velocity of the device with the specified type."))
	static FVector GetDeviceVelocity(EWVR_DeviceType type = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To get the angular velocity of the device with the specified type."))
	static FVector GetDeviceAngularVelocity(EWVR_DeviceType type = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To check if the device with the specified type is connected."))
	static bool IsDeviceConnected(EWVR_DeviceType Type);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To check if the pose of the device with the specified type is valid."))
	static bool IsDevicePoseValid(EWVR_DeviceType Type);

	// To get the supported degree of freedom of the device with the specified type.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static EWVR_DOF GetSupportedNumOfDoF(EWVR_DeviceType Type);

	// To get the current degree of freedom of the device with the specified type.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager", meta=(DeprecatedFunction, DeprecationMessage = "Use new IsTrackingHMDPosition instead, we don't Set/Get Dof of controller any more."))
	static EWVR_DOF GetNumOfDoF(EWVR_DeviceType Type);

	// To set the current degree of freedom of the device with the specified type.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager", meta=(DeprecatedFunction, DeprecationMessage = "Use new SetTrackingHMDPosition instead, we don't Set/Get Dof of controller any more."))
	static bool SetNumOfDoF(EWVR_DeviceType Type, EWVR_DOF DOF = EWVR_DOF::DOF_3);

	// The position of HMD is (0,0,0) if IsTrackingPosition is false.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static void SetTrackingHMDPosition(bool IsTrackingPosition);

	// To get the status the last time do SetTrackingHMDPosition.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static bool IsTrackingHMDPosition();

	// The rotation of HMD is (0,0,0) if IsTrackingRotation is false.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static void SetTrackingHMDRotation(bool IsTrackingRotation);

	//Means the position of the HMD and the position of the Controllers are (0,0,0). Execute SetTrackingOrigin will cancel this effect.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static void SetTrackingOrigin3Dof();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To check whether the application runs in foreground or background, false for foreground."))
	static bool IsInputFocusCapturedBySystem();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To re-center the HMD position. Please refer to ERecenterType for the re-center type."))
	static void InAppRecenter(ERecenterType type);

	// To show if the device supports the foveated render feature or not.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Foveation")
	static bool IsRenderFoveationSupport();

	// To enable or disable the foveated render feature.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Foveation")
	static void EnableFoveation(bool bEnable);

	// To config the foveated render feature. EEye {LEFT, Right} means to which eye the parameter will be applied. Focal_X/Focal_Y means the X/Y coordinate of the assigned eye. The original point (0,0) resides on the center of the eye. The domain value is between {-1, 1}. FOV represents the angle of the clear region. EWVR_PeripheralQuality {Low, Medium, High} represents the resolution of the peripheral region.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Foveation")
	static void SetFoveationParams(EEye Eye = EEye::LEFT, float Focal_X = 0.0f, float Focal_Y = 0.0f, float FOV = 90.0f,
					EWVR_PeripheralQuality PeripheralQuality = EWVR_PeripheralQuality::High);

	// To show if the AdaptiveQuality feature is enabled or not.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|AdaptiveQuality")
	static bool IsAdaptiveQualityEnabled();

	// To enable or disable the AdaptiveQuality feature.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|AdaptiveQuality")
	static bool EnableAdaptiveQuality(bool enable);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To set the pose simulation type of the rotation-only controller. The simulation means the controller will use the arm model fake pose."))
	static void SetPoseSimulationOption(SimulatePosition Option = SimulatePosition::WhenNoPosition);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|PoseManager",
		meta = (ToolTip = "To make the rotation-only controller which uses the simulation pose follows the head's movement."))
	static void SetFollowHead(bool follow = false);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert X-Y-Z dimension values to a Unity coordinate FVector."))
	static FVector ConvertToUnityVector(float x, float y, float z, float WorldToMetersScale);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert a Unreal coordinate FVector to a Unity coordinate FVector."))
	static FVector ConvertToUnityVectorInVector(FVector vec, float WorldToMetersScale);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert X-Y-Z dimension values to a Unreal coordinate FVector."))
	static FVector ConvertToUnrealVector(float x, float y, float z, float WorldToMetersScale);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert a Unity coordinate FVector to a Unreal coordinate FVector."))
	static FVector ConvertToUnrealVectorInVector(FVector vec, float WorldToMetersScale);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert W-X-Y-Z quaternion values to a Unity coordinate FQuat."))
	static FQuat ConvertToUnityQuaternion(float w, float x, float y, float z);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert a Unreal coordinate FQuat to a Unity coordinate FQuat."))
	static FQuat ConvertToUnityQuaternionInQuat(FQuat quat);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert a Unreal coordinate FRotator to a Unity coordinate FQuat."))
	static FQuat ConvertToUnityQuaternionInRotator(FRotator rotator);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert W-X-Y-Z quaternion values to a Unreal coordinate FRotator."))
	static FRotator ConvertToUnrealRotator(float w, float x, float y, float z);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert a Unity coordinate FQuat to a Unreal coordinate FRotator."))
	static FRotator ConvertToUnrealRotatorInQuat(FQuat quat);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Coordinate|Unity",
		meta = (ToolTip = "To convert a Unity coordinate FRotator to a Unreal coordinate FRotator."))
	static FRotator ConvertToUnrealRotatorInRotator(FRotator rotator);

	UFUNCTION(BlueprintCallable, Category = "WaveVR", meta = (
		ToolTip = "Get connected device's battery percentage"))
	static float getDeviceBatteryPercentage(EWVR_DeviceType type);

	UFUNCTION(BlueprintCallable, Category = "WaveVR", meta = (
		ToolTip = "Get connected device's render model name"))
	static FString GetRenderModelName(EWVR_Hand hand);

	// Get the HMD's eye buffer Resolution size
	UFUNCTION(BlueprintCallable, Category = "WaveVR")
	static bool GetRenderTargetSize(FIntPoint& OutSize);

	UFUNCTION(BlueprintCallable, Category = "WaveVR", meta = (
		ToolTip = "This is an internal API, please do not call directly"))
	static AActor * LoadCustomControllerModel(EWVR_DeviceType device, EWVR_DOF dof, FTransform transform);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To check if the VR environment is left-handed mode."))
	static bool IsLeftHandedMode();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To enable or disable the neck model of a rotation-only head mount device. If being enabled, the HMD will apply the neck model fake pose."))
	static void EnableNeckModel(bool enable);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To find the children of ParentWidget. The children widgets will be stored in ChildWidgets."))
	static int GetHoveredWidgetSeqId(UUserWidget* ParentWidget, TArray<UWidget*>& ChildWidgets);

	// To get texture2D image from image file path and file name.
	UFUNCTION(BlueprintCallable, Category = "WaveVR")
	static UTexture2D* GetTexture2DFromImageFile(FString imageFileName, FString imagePath, EUTex2DFmtType type);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To get the API level of WaveVR SDK, e.g. 5"))
	static int GetWaveVRRuntimeVersion();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To get the key mapping pair of the specified button on the device with the specified type. E.g. Dominant Touchpad can be mapping from Left controller Thumbstick button."))
	static EWVR_InputId GetInputMappingPair(EWVR_DeviceType type, EWVR_InputId button);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To check if the specified button of the device with the specified type is available. This API has high performance."))
	static bool IsButtonAvailable(EWVR_DeviceType type, EWVR_InputId button);

	UFUNCTION(BlueprintCallable, Category = "WaveVR", meta = (
		ToolTip = "Update which controller is used to interact with objects"))
	static void SetFocusController(EWVR_DeviceType focusedController);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To check if the specified button of the device with the specified type is pressed."))
	static bool GetInputButtonState(EWVR_DeviceType type, EWVR_InputId id);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To check if the specified button of the device with the specified type is touched."))
	static bool GetInputTouchState(EWVR_DeviceType type, EWVR_InputId id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR", meta = (
		ToolTip = "Get which controller is used to interact with objects"))
	static EWVR_DeviceType GetFocusController();

	// To config the splash feature. InSplashTexture is the desired Texture which will show. BackGroundColor means the background color while showing a background removal texture (texture which has alpha channel). ScaleFactor means the amplification of the desired texture size, 1 is the original size. Shift means the shift offset by pixel applied to the texture in screen space. (0,0) means put the texture in the center. EnableAutoLoading means if the splash texture will show while switching between maps or not.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|SplashScreen")
	static void SetSplashParam(UTexture2D* InSplashTexture, FLinearColor BackGroundColor, float ScaleFactor, FVector2D Shift, bool EnableAutoLoading);

	// Manually force show splash texture.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|SplashScreen")
	static void ShowSplashScreen();

	// Manually force hide splash texture.
	UFUNCTION(BlueprintCallable, Category = "WaveVR|SplashScreen")
	static void HideSplashScreen();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|DirectPreview", meta = (
		ToolTip = "Check if Direct Preview is running"))
	static bool IsDirectPreview();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR",
		meta = (ToolTip = "To send a particular string as parameters to the device with the specified type."))
	static void SetParameters(EWVR_DeviceType type, FString pchValue);

#pragma region ScreenshotMode
	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Screenshot",
		meta = (ToolTip = "To set the screenshot mode. Please refer to the SDK documents for detailed information."))
	static bool ScreenshotMode(EScreenshotMode ScreenshotMode);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Screenshot",
		meta = (ToolTip = "To get the file name and the saved path of the screenshot."))
	static void GetScreenshotFileInfo(FString &ImageFileName, FString &ImagePath);
#pragma endregion

	/**
	 * This blueprint function is only used for debug.  The unit of
	 * gameThreadLoading and renderThreadLoading are microsecond.  And the
	 * values are both limited in 100 milliseconds. Set them to simulate
	 * CPU loadings on both thread.
	**/
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Debug")
	static void SimulateCPULoading(int gameThreadLoading, int renderThreadLoading);

	/**
	 * LateUpdate is default enabled.  doUpdateInGameThread will work if
	 * LateUpdate is enabled. And predictTimeInGameThread will work if
	 * LateUpdate and doUpdateInGameThread are both enabled.
	 * The unit of predictTimeInGameThread is millisecond, and the time will
	 * be limited in 100ms. This blueprint function is for internal use only.
	**/
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static void EnableLateUpdate(bool enable, bool doUpdateInGameThread = false, float predictTimeInGameThread = 0.0f);

	/**
	 * LateUpdate is default enabled.  Use this function to check the
	 * status of LateUdate feature.
	**/
	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static bool IsLateUpdateEnabled();
};
