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

#include "WaveVRGestureEnums.generated.h"

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture")
enum class EWaveVRStaticGesture : uint8
{
	TYPE_INVALID	= 0,	/**< The gesture is invalid. */
	TYPE_UNKNOWN	= 1,    /**< Unknow gesture type. */
	TYPE_FIST		= 2,    /**< Represent fist gesture. */
	TYPE_FIVE		= 3,    /**< Represent five gesture. */
	TYPE_OK			= 4,    /**< Represent ok gesture. */
	TYPE_LIKE		= 5,    /**< Represent thumb up gesture. */
	TYPE_POINT		= 6,    /**< Represent index up gesture. */
	TYPE_PINCH		= 7,    /**< Represent pinch gesture. */
};

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture")
enum class EWaveVRDynamicGesture : uint8
{
	TYPE_INVALID = 0,
	TYPE_SwipingRight,
	TYPE_SwipingLeft,
	TYPE_SwipingUp,
	TYPE_SwipingDown
};

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture|Hand")
enum class EWaveVRGestureHandType : uint8
{
	LEFT,
	RIGHT,

	COUNT
};

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture|Finger")
enum class EWaveVRGestureFingerType : uint8
{
	THUMB_L,
	INDEX_L,
	MIDDLE_L,
	RING_L,
	PINKY_L,

	THUMB_R,
	INDEX_R,
	MIDDLE_R,
	RING_R,
	PINKY_R,

	COUNT
};

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture|Bone")
enum class EWaveVRGestureBoneType : uint8
{
	BONE_ROOT,

	// Left Arm, 20 finger bones + wrist / fore arm / upper arm.
	BONE_UPPERARM_L,
	BONE_FOREARM_L,
	BONE_HAND_WRIST_L,
	BONE_HAND_PALM_L,
	BONE_THUMB_JOINT1_L,	// 5
	BONE_THUMB_JOINT2_L,
	BONE_THUMB_JOINT3_L,
	BONE_THUMB_TIP_L,
	BONE_INDEX_JOINT1_L,
	BONE_INDEX_JOINT2_L,	// 10
	BONE_INDEX_JOINT3_L,
	BONE_INDEX_TIP_L,
	BONE_MIDDLE_JOINT1_L,
	BONE_MIDDLE_JOINT2_L,
	BONE_MIDDLE_JOINT3_L,	// 15
	BONE_MIDDLE_TIP_L,
	BONE_RING_JOINT1_L,
	BONE_RING_JOINT2_L,
	BONE_RING_JOINT3_L,
	BONE_RING_TIP_L,		// 20
	BONE_PINKY_JOINT1_L,
	BONE_PINKY_JOINT2_L,
	BONE_PINKY_JOINT3_L,
	BONE_PINKY_TIP_L,

	// Right Arm, 20 finger bones + wrist / fore arm / upper arm.
	BONE_UPPERARM_R,		// 25
	BONE_FOREARM_R,
	BONE_HAND_WRIST_R,
	BONE_HAND_PALM_R,
	BONE_THUMB_JOINT1_R,
	BONE_THUMB_JOINT2_R,	// 30
	BONE_THUMB_JOINT3_R,
	BONE_THUMB_TIP_R,
	BONE_INDEX_JOINT1_R,
	BONE_INDEX_JOINT2_R,
	BONE_INDEX_JOINT3_R,	// 35
	BONE_INDEX_TIP_R,
	BONE_MIDDLE_JOINT1_R,
	BONE_MIDDLE_JOINT2_R,
	BONE_MIDDLE_JOINT3_R,
	BONE_MIDDLE_TIP_R,		// 40
	BONE_RING_JOINT1_R,
	BONE_RING_JOINT2_R,
	BONE_RING_JOINT3_R,
	BONE_RING_TIP_R,
	BONE_PINKY_JOINT1_R,	// 45
	BONE_PINKY_JOINT2_R,
	BONE_PINKY_JOINT3_R,
	BONE_PINKY_TIP_R,

	BONES_COUNT
};

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture")
enum class EWaveVRHandGestureStatus : uint8
{
	// Initial, can call Start API in this state.
	NOT_START,
	START_FAILURE,

	// Processing, should NOT call API in this state.
	STARTING,
	STOPING,

	// Running, can call Stop API in this state.
	AVAILABLE,

	// Do nothing.
	UNSUPPORT
};

UENUM(BlueprintType, Category = Category = "WaveVR|Gesture")
enum class EWaveVRHandTrackingStatus : uint8
{
	// Initial, can call Start API in this state.
	NOT_START,
	START_FAILURE,

	// Processing, should NOT call API in this state.
	STARTING,
	STOPING,

	// Running, can call Stop API in this state.
	AVAILABLE,

	// Do nothing.
	UNSUPPORT
};
