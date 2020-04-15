// "WaveVR SDK 
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#ifndef wvr_hand_h_
#define wvr_hand_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the gesture type
 */
typedef enum {
    WVR_HandGestureType_Invalid         = 0,    /**< The gesture is invalid. */
    WVR_HandGestureType_Unknown         = 1,    /**< Unknow gesture type. */
    WVR_HandGestureType_Fist            = 2,    /**< Represent fist gesture. */
    WVR_HandGestureType_Five            = 3,    /**< Represent five gesture. */
    WVR_HandGestureType_OK              = 4,    /**< Represent ok gesture. */
    WVR_HandGestureType_ThumbUp         = 5,    /**< Represent thumb up gesture. */
    WVR_HandGestureType_IndexUp         = 6,    /**< Represent index up gesture. */
    WVR_HandGestureType_Pinch           = 7,    /**< Reserved */
} WVR_HandGestureType;

/**
 * @brief the gesture information
 */
typedef struct WVR_HandGestureData {
    int64_t             timestamp;  /**< the current time in milliseconds */
    WVR_HandGestureType right;      /**< gesture type of right hand */
    WVR_HandGestureType left;       /**< gesture type of left hand */
}  WVR_HandGestureData_t;

/**
 * @brief one finger pose
 *
 * It separates a finger as four skeletion.
 */
typedef struct WVR_SingleFinger {
    WVR_Vector3f_t joint1;  /**< The position data of joint between trapezium and metracarpal in thumb, the position data of joint between metracarpal and proximal in other fingers */
    WVR_Vector3f_t joint2;  /**< The position data of joint between metracarpal and proximal in thumb, the position data of joint between proximal and intermediate in other fingers */
    WVR_Vector3f_t joint3;  /**< The position data of joint between proximal and distal in thumb, the position data of joint between intermediate and distal in other fingers */
    WVR_Vector3f_t tip;     /**< The position data of finger tip */
} WVR_SingleFinger_t;

/**
 * @brief the five fingers information
 */
typedef struct WVR_Fingers {
    WVR_SingleFinger_t    thumb;    /**< finger data of thumb */
    WVR_SingleFinger_t    index;    /**< finger data of index */
    WVR_SingleFinger_t    middle;   /**< finger data of middle */
    WVR_SingleFinger_t    ring;     /**< finger data of ring */
    WVR_SingleFinger_t    pinky;    /**< finger data of pinky */
}  WVR_Fingers_t;

/**
 * @brief the hand tracking information
 */
typedef struct WVR_HandTrackingData {
    WVR_PoseState_t         right;              /**< tracking data of right hand */
    WVR_Fingers_t           rightFingers;       /**< five fingers information of right hand */
    WVR_PoseState_t         left;               /**< tracking data of left hand */
    WVR_Fingers_t           leftFingers;        /**< five fingers information of left hand */
}  WVR_HandTrackingData_t;

/**
 * @brief Function to start hand gesture feature.
 *
 * This API will start hand gesture module
 * This API must be called by main thread.
 *
 * @retval WVR_Success start hand gesture feature successfully.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_StartHandGesture();

/**
 * @brief Function to stop hand gesture that you already started.
 *
 * This API will stop hand gesture that you already started,
 * and release related hand gesture source.
 * This API must be called by main thread.
 * @version API Level 5
*/
extern WVR_EXPORT void WVR_StopHandGesture();

/**
 * @brief Function to get gesture data.
 *
 * This API is used to get hand gesture data from the hand gesture module
 * This API must be called by main thread.
 *
 * @param data the hand gesture data @ref WVR_GestureData
 * @retval WVR_Success get data successfully.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_GetHandGestureData(WVR_HandGestureData_t *data);

/**
 * @brief Function to start hand tracking feature.
 *
 * This API will start hand tracking module
 * This API must be called by main thread.
 * @retval WVR_Success start hand tracking feature successfully.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_StartHandTracking();

/**
 * @brief Function to stop hand tracking that you already started.
 *
 * This API will stop hand tracking that you already started,
 * and release related hand tracking source.
 * This API must be called by main thread.
 * @version API Level 5
*/
extern WVR_EXPORT void WVR_StopHandTracking();

/**
 * @brief Function to get hand tracking for 21 key ponts data.
 *
 * This API is used to get hand tracking data from the hand tracking module
 * This API must be called by main thread.
 *
 * @param data the hand tracking data @ref WVR_HandTrackingData
 * @param originModel Tracking universe that returned poses should be relative to.  (refer to @ref WVR_PoseOriginModel)
 * @param predictedMilliSec Number of milliseconds from now to predict poses for. Positive numbers are in the future.
 * @retval WVR_Success get data successfully.
 * @retval others @ref WVR_Result mean failure.
 * @version API Level 5
*/
extern WVR_EXPORT WVR_Result WVR_GetHandTrackingData(WVR_HandTrackingData_t *data, WVR_PoseOriginModel originModel=WVR_PoseOriginModel_OriginOnHead, uint32_t predictedMilliSec=0);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_hand_h_ */

