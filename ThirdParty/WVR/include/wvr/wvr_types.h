// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_types_h_
#define wvr_types_h_

#include "wvr_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The type of VR device
 *
 * Describes the role of VR devices
 */
typedef enum {
    WVR_DeviceType_Invalid                      = 0,    /**< The device is unknown or not existing. */
    WVR_DeviceType_HMD                          = 1,    /**< Head-Mounted Display. */
    WVR_DeviceType_Controller_Right             = 2,    /**< Right hand tracked controller. */
    WVR_DeviceType_Controller_Left              = 3,    /**< Left hand tracked controller. */
} WVR_DeviceType;

/**
 * @brief The sides of target scene.
 *
 * Specify the side of eye for targe scene.
 */
typedef enum {
    WVR_Eye_Left = 0,                                          /**< WVR_Eye_Left: Left eye id. */
    WVR_Eye_Right = 1,                                         /**< WVR_Eye_Right: Right eye id. */
} WVR_Eye;

/**
 * @brief The input id of device
 */
typedef enum {
    WVR_InputId_0     = 0,
    WVR_InputId_1     = 1,
    WVR_InputId_2     = 2,
    WVR_InputId_3     = 3,
    WVR_InputId_4     = 4,
    WVR_InputId_5     = 5,
    WVR_InputId_6     = 6,
    WVR_InputId_7     = 7,
    WVR_InputId_8     = 8,
    WVR_InputId_9     = 9,
    WVR_InputId_10    = 10,
    WVR_InputId_11    = 11,
    WVR_InputId_14    = 14,
    WVR_InputId_15    = 15,
    WVR_InputId_16    = 16,
    WVR_InputId_17    = 17,
    WVR_InputId_18    = 18,

    //alias group mapping
    WVR_InputId_Alias1_System          = WVR_InputId_0,  /**< System Button. */
    WVR_InputId_Alias1_Menu            = WVR_InputId_1,  /**< Menu Button. */
    WVR_InputId_Alias1_Grip            = WVR_InputId_2,  /**< Grip Button. */
    WVR_InputId_Alias1_DPad_Left       = WVR_InputId_3,  /**< DPad_Left Button in physical, or simulated by Touchpad Left pressed event. */
    WVR_InputId_Alias1_DPad_Up         = WVR_InputId_4,  /**< DPad_Up Button in physical, or  simulated by Touchpad Up pressed event. */
    WVR_InputId_Alias1_DPad_Right      = WVR_InputId_5,  /**< DPad_Right Button in physical, or simulated by Touchpad Right pressed event. */
    WVR_InputId_Alias1_DPad_Down       = WVR_InputId_6,  /**< DPad_Down Button in physical, or simulated by Touchpad Down pressed event. */
    WVR_InputId_Alias1_Volume_Up       = WVR_InputId_7,  /**< Volume_Up Button. */
    WVR_InputId_Alias1_Volume_Down     = WVR_InputId_8,  /**< Volume_Down Button. */
    WVR_InputId_Alias1_Digital_Trigger = WVR_InputId_9,  /**< Digital Trigger Button. */
    WVR_InputId_Alias1_A               = WVR_InputId_10, /**< Button A. */
    WVR_InputId_Alias1_B               = WVR_InputId_11, /**< Button B. */
    WVR_InputId_Alias1_Back            = WVR_InputId_14, /**< Hmd Back Button */
    WVR_InputId_Alias1_Enter           = WVR_InputId_15, /**< Hmd Enter Button */
    WVR_InputId_Alias1_Touchpad        = WVR_InputId_16, /**< Touchpad input device. */
    WVR_InputId_Alias1_Trigger         = WVR_InputId_17, /**< Trigger input device. */
    WVR_InputId_Alias1_Thumbstick      = WVR_InputId_18, /**< Thumbstick input device. */

    WVR_InputId_Max   = 32,
} WVR_InputId;

/**
 * @brief The style of tracking origin
 *
 * Identifies which style of tracking origin the application wants to use
 * for the poses it is requesting
 */
typedef enum {
    WVR_PoseOriginModel_OriginOnHead                = 0, /**< The origin of 6 DoF pose is on head. */
    WVR_PoseOriginModel_OriginOnGround              = 1, /**< The origin of 6 DoF pose is on ground. */
    WVR_PoseOriginModel_OriginOnTrackingObserver    = 2, /**< The raw pose from tracking system. */
    WVR_PoseOriginModel_OriginOnHead_3DoF           = 3, /**< The origin of 3 DoF pose is on head. */
} WVR_PoseOriginModel;

/**
 * @brief The error status of specified device from device service.
 *
 * Device Service uses bitmask to record device error, because it might occur many errors at the same time.
 * Each error status represents that device has hardware exception.
 * And restore status means that error status has been fixed.
 */
typedef enum : uint64_t {
    WVR_DeviceErrorStatus_None                       = 0,       /**< the status of device service is normal. */
    WVR_DeviceErrorStatus_BatteryOverheat            = 1,       /**< battery temperature is too hot. */
    WVR_DeviceErrorStatus_BatteryOverheatRestore     = 1 << 1,  /**< battery temperature has been cooled down. */
    WVR_DeviceErrorStatus_BatteryOvervoltage         = 1 << 2,  /**< battery is overvoltage charging. */
    WVR_DeviceErrorStatus_BatteryOvervoltageRestore  = 1 << 3,  /**< overvoltage has been fixed. */
    WVR_DeviceErrorStatus_DeviceConnectFail          = 1 << 4,  /**< it's failed to connect to USB device. */
    WVR_DeviceErrorStatus_DeviceConnectRestore       = 1 << 5,  /**< ConnectFail has been fixed. */
    WVR_DeviceErrorStatus_DeviceLostTracking         = 1 << 6,  /**< tracking device might be out of range. */
    WVR_DeviceErrorStatus_DeviceTrackingRestore      = 1 << 7,  /**< LostTracking has been fixed. */
    WVR_DeviceErrorStatus_ChargeFail                 = 1 << 8,  /**< when device is charging, consuming voltage is larger than charging. */
    WVR_DeviceErrorStatus_ChargeRestore              = 1 << 9,  /**< ChargeFail has been fixed. */
} WVR_DeviceErrorStatus;

/**
 * @brief The interaction mode of application.
 */
typedef enum {
    WVR_InteractionMode_SystemDefault   = 1,  /**< Reset @ref WVR_InteractionMode and @ref WVR_GazeTriggerType to sysetem default values. Refer to @ref WVR_SetInteractionMode. */
    WVR_InteractionMode_Gaze            = 2,  /**< Gaze mode only. */
    WVR_InteractionMode_Controller      = 3,  /**< Application uses controller if controller is connected; otherwise switches to gaze. */
} WVR_InteractionMode;

/**
 * @brief The trigger type of gaze of application.
 */
typedef enum {
    WVR_GazeTriggerType_Timeout          = 1,  /**< Use timeout to trigger gaze. */
    WVR_GazeTriggerType_Button           = 2,  /**< Use button to trigger gaze. */
    WVR_GazeTriggerType_TimeoutButton    = 3,  /**< Both timeout and button can trigger gaze. */
} WVR_GazeTriggerType;

/**
 * @brief event type
 */
typedef enum {
    /** common event region */
    WVR_EventType_Quit                               = 1000,    /**< Application Quit. */
    WVR_EventType_SystemInteractionModeChanged       = 1001,    /**< @ref WVR_InteractionMode changed; using @ref WVR_GetInteractionMode to get interaction mode. */
    WVR_EventType_SystemGazeTriggerTypeChanged       = 1002,    /**< @ref WVR_GazeTriggerType changed; using @ref WVR_GetGazeTriggerType to get gaze trigger type. */
    WVR_EventType_TrackingModeChanged                = 1003,    /**< Notification of changing tracking mode (3 Dof/6 Dof); using @ref WVR_GetDegreeOfFreedom can get current tracking mode.*/
    WVR_EventType_RecommendedQuality_Lower           = 1004,    /**< Notification recommended quality to Lower from runtime. */
    WVR_EventType_RecommendedQuality_Higher          = 1005,    /**< Notification recommended quality to Higher from runtime. */
    WVR_EventType_HandGesture_Changed                = 1006,    /**< Notification gesture changed. */
    WVR_EventType_HandGesture_Abnormal               = 1007,    /**< Notification gesture abnormal. */
    WVR_EventType_HandTracking_Abnormal              = 1008,    /**< Notification hand tracking abnormal. */

    /** Device events region */
    WVR_EventType_DeviceConnected                    = 2000,    /**< @ref WVR_DeviceType connected. */
    WVR_EventType_DeviceDisconnected                 = 2001,    /**< @ref WVR_DeviceType disconnected. */
    WVR_EventType_DeviceStatusUpdate                 = 2002,    /**< @ref WVR_DeviceType configure changed. */
    WVR_EventType_DeviceSuspend                      = 2003,    /**< When user takes off HMD*/
    WVR_EventType_DeviceResume                       = 2004,    /**< When user puts on HMD*/
    WVR_EventType_IpdChanged                         = 2005,    /**< The interpupillary distance has been changed; using @ref WVR_GetRenderProps can get current ipd. */
    WVR_EventType_DeviceRoleChanged                  = 2006,    /**< @ref WVR_DeviceType controller roles are switched. */
    WVR_EventType_BatteryStatusUpdate                = 2007,    /**< @ref WVR_DeviceType the battery status of device has changed; using @ref WVR_GetBatteryStatus to check the current status of the battery. */
    WVR_EventType_ChargeStatusUpdate                 = 2008,    /**< @ref WVR_DeviceType the charged status of device has changed; using @ref WVR_GetChargeStatus to check the current status of the battery in use. */
    WVR_EventType_DeviceErrorStatusUpdate            = 2009,    /**< @ref WVR_DeviceType device occurs some warning; using @ref WVR_GetDeviceErrorState to get the current error status from device service. */
    WVR_EventType_BatteryTemperatureStatusUpdate     = 2010,    /**< @ref WVR_DevcieType battery temperature of device has changed; using @ref WVR_GetBatteryTemperatureStatus to get the current battery temperature. */
    WVR_EventType_RecenterSuccess                    = 2011,    /**< Notification of recenter success for 6 DoF device*/
    WVR_EventType_RecenterFail                       = 2012,    /**< Notification of recenter fail for 6 DoF device*/
    WVR_EventType_RecenterSuccess3DoF                = 2013,    /**< Notification of recenter success for 3 DoF device*/
    WVR_EventType_RecenterFail3DoF                   = 2014,    /**< Notification of recenter fail for 3 DoF device*/
    WVR_EventType_PassthroughOverlayShownBySystem    = 2100,    /**< Notification of passthrough overlay is shown by system.*/
    WVR_EventType_PassthroughOverlayHiddenBySystem   = 2101,    /**< Notification of passthrough overlay is hidden by system. */

    /** Input Event region */
    WVR_EventType_ButtonPressed                      = 3000,     /**< @ref WVR_InputId status change to pressed. */
    WVR_EventType_ButtonUnpressed                    = 3001,     /**< @ref WVR_InputId status change to unpressed */
    WVR_EventType_TouchTapped                        = 3002,     /**< @ref WVR_InputId status change to touched. */
    WVR_EventType_TouchUntapped                      = 3003,     /**< @ref WVR_InputId status change to untouched. */
    WVR_EventType_LeftToRightSwipe                   = 3004,     /**< Notification of swipe motion (move Left to Right) on touchpad */
    WVR_EventType_RightToLeftSwipe                   = 3005,     /**< Notification of swipe motion (move Right to Left) on touchpad */
    WVR_EventType_DownToUpSwipe                      = 3006,     /**< Notification of swipe motion (move Down to Up) on touchpad */
    WVR_EventType_UpToDownSwipe                      = 3007,     /**< Notification of swipe motion (move Up to Down) on touchpad */
} WVR_EventType;

/**
 * @brief The type of degree of freedom (DoF)
 */
typedef enum {
    WVR_NumDoF_3DoF = 0, /**< Position (x, y, z) on 3 coordinates, which contains (1) moving up and down, (2) moving left and right, (3) moving forward and backward. */
    WVR_NumDoF_6DoF = 1, /**< Position (x, y, z) on 3 coordinates and rotation (yaw, pitch, roll) on 3 coordinates. */
} WVR_NumDoF;


/**
 * @brief the returned result of a function call for providing the information of failure.
 */
typedef enum {
    WVR_Success                              = 0,    /**< The result of the function call was successful. */
    WVR_Error_SystemInvalid                  = 1,    /**< The initialization was not finished or the feature was not started yet. */
    WVR_Error_InvalidArgument                = 2,    /**< One of the arguments was not appropriate for the function call. */
    WVR_Error_OutOfMemory                    = 3,    /**< A memory allocation has failed. */
    WVR_Error_FeatureNotSupport              = 4,    /**< The feature was not supported; either lack of some services or service does not support this feature. */
    WVR_Error_RuntimeVersionNotSupport       = 5,    /**< The runtime version is too old to support the function call. */
    WVR_Error_CameraNotAvailable             = 6,    /**< Camera was unavaliable, cannot query camera related information. */
    WVR_Error_CameraPermissionNotGranted     = 7,    /**< The Android camera permission was not granted yet. */
    //----- CtrlerModel API Use.(Start at 100)
    WVR_Error_CtrlerModel_WrongDeviceType    = 100,  /**< Input wrong device type for asking controller model. */
    WVR_Error_CtrlerModel_DeviceDisconnected = 101,  /**< The controller device you want to get its model is disconnected. */
    WVR_Error_CtrlerModel_InvalidModel       = 102,  /**< We can't get model that can be use. */
    WVR_Error_CtrlerModel_Unknown            = 103   /**< Unknown error. */
} WVR_Result;

/**
 * @brief The basic matrix (4x4 floats) struct to be used in pose
 */
typedef struct WVR_Matrix4f {
    float m[4][4]; /**< Matrix form (4X4) to represent rotation and position of pose data. */
} WVR_Matrix4f_t;

/**
 * @brief The basic vector (3 floats) struct to be used in pose
 */
typedef struct WVR_Vector3f {
    float v[3]; /**< Matrix form (3X1) to represent a set of data which contains 3 elements. */
} WVR_Vector3f_t;

/**
 * @brief The basic vector (2 floats) struct to be used in pose
 */
typedef struct WVR_Vector2f {
    float v[2]; /**< Matrix form (2X1) to represent a set of data which contains 2 elements. */
} WVR_Vector2f_t;

/**
 * @brief The basic quaternion (4 floats) struct to be used in pose
 */
typedef struct WVR_Quatf {
    float w, x, y, z; /**< Quaternion form to represent rotation of pose. */
} WVR_Quatf_t;

/**
 * @brief Describes a single pose in quaternion form for a tracked object
 */
typedef struct WVR_Pose {
    WVR_Vector3f_t position; /**< Position (x, y, z) part of Pose (refer to @ref WVR_Vector3f). */
    WVR_Quatf_t rotation;    /**< Rotation (quaternion form: w, x, y, z) part of Pose (refer to @ref WVR_Quatf). */
} WVR_Pose_t;

/**
 * @brief Describes a single pose in rotation matrix form for a tracked object
 */
typedef struct WVR_PoseState {
    bool isValidPose;                   /**< The label of valid(true)/invalid(false) pose. */
    WVR_Matrix4f_t poseMatrix;          /**< The pose data (ratation, position) in matrix form (refer to @ref WVR_Matrix4f). */
    WVR_Vector3f_t velocity;            /**< The velocity of pose  (refer to @ref WVR_Vector3f). */
    WVR_Vector3f_t angularVelocity;     /**< The angular velocity of pose. */
    bool is6DoFPose;                    /**< The label of 6 DoF(true)/3 DoF(false) pose. */
    int64_t timestamp;                  /**< Absolute time (in nanosecond) of pose. */
    WVR_Vector3f_t acceleration;        /**< The acceleration of pose. */
    WVR_Vector3f_t angularAcceleration; /**< The angular acceleration of pose. */
    float predictedMilliSec;            /**< Number of milliseconds from now to predict poses. */
    WVR_PoseOriginModel originModel;    /**< The origin model of pose. */
    WVR_Pose_t rawPose;                 /**< The raw data of pose from device */
} WVR_PoseState_t;

/**
 * @brief Aggregate the device type identity and device tracking pose.
 */
typedef struct WVR_DevicePosePair {
    WVR_DeviceType type;        /**< Type of VR device. */
    WVR_PoseState_t pose;       /**< Pose information of Tracked VR device (refer to @ref WVR_PoseState). */
} WVR_DevicePosePair_t;

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_types_h_ */


