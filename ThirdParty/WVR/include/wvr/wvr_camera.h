// "WaveVR SDK 
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_camera_h_
#define wvr_camera_h_

#include "wvr_types.h"
#include "wvr_stdinc.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the image type
 *
 * Specify camera image type.
 */
typedef enum {
    WVR_CameraImageType_Invalid   = 0,     /**< The image type is invalid. */
    WVR_CameraImageType_SingleEye = 1,     /**< The image is comprised of one camera. */
    WVR_CameraImageType_DualEye   = 2,     /**< The image is comprised of dual camera. */
} WVR_CameraImageType;

/**
 * @brief the image format
 *
 * Specify camera image format.
 */
typedef enum {
    WVR_CameraImageFormat_Invalid     = 0,  /**< The image format is invalid. */
    WVR_CameraImageFormat_YUV_420     = 1,  /**< The image format is YUV420. */
    WVR_CameraImageFormat_Grayscale   = 2,  /**< The image format is 8-bit gray-scale. */
} WVR_CameraImageFormat;

/**
 * @brief the camera position on HMD
 *
 * Sperify camera position.
 */
typedef enum {
    WVR_CameraPosition_Invalid    = 0,    /**< The camera position is invalid. */
    WVR_CameraPosition_Left       = 1,    /**< Left camera on HMD. */
    WVR_CameraPosition_Right      = 2,    /**< Right camera on HMD. */
}  WVR_CameraPosition;

/**
 * @brief the camera intrinsics parameters
 *
 * Camera intrinsics parameters are comprised of focal length and principal point.
 */
typedef struct WVR_CameraIntrinsic {
    WVR_Vector2f_t    focalLength;      /**< focal length of the camera*/
    WVR_Vector2f_t    principalPoint;   /**< principal point of the camera*/
}  WVR_CameraIntrinsic_t;

/**
 * @brief the camera image information
 *
 * the camera image information form WVR_StartCamera.
 */
typedef struct WVR_CameraInfo {
    WVR_CameraImageType   imgType;      /**< Single or stereo image */
    WVR_CameraImageFormat imgFormat;    /**< The image format */
    uint32_t              width;        /**< The image width */
    uint32_t              height;       /**< The image height */
    uint32_t              size;         /**< The buffer size for raw image data */
} WVR_CameraInfo_t;

/**
 * @brief Function to start camera and get the basic camera information.
 *
 * This API will get supporting and suitable frame size and camera information from DeviceService.
 * This API must be called by main thread.
 *
 * @param WVR_CameraInfo_t comprised of camera image information
 * @retval true starting camera is success.
 * @retval false starting camera fail.
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_StartCamera(WVR_CameraInfo_t *info);

/**
 * @brief Function to stop camera that you already started.
 *
 * This API will stop camera that you already started, and release related camera source.
 * This API must be called by main thread.
 * @version API Level 1
*/
extern WVR_EXPORT void WVR_StopCamera();

/**
 * @brief Function to get raw image relate to camera.
 *
 * This API will copy the raw image from camera. The buffer size should be got from the return size
 * from WVR_StartCamera API. This API's performance will have worse performance due to memory copy.
 * This API should be called when WVR_StartCamera success, and stopped once WVR_StopCamera is called.
 *
 * @param uint8_t* memory that user would like to copy to
 * @param  uint32_t the buffer size that pframebuffer allocated
 * @retval true update pframebuffer success.
 * @retval false update pframebuffer fail.
 * @version API Level 1
*/

extern WVR_EXPORT bool WVR_GetCameraFrameBuffer(uint8_t *pframebuffer, uint32_t frameBufferSize);

/**
 * @brief Function to get camera intrinsic parameters. These parameters depends on camera hardware.
 *
 * This API will return the hardware related camera parameters, this should by supported by your
 * DeviceService.
 *
 * @param WVR_CameraPosition If you have dual camera on DeviceService, get the left/right camera intrinsic.
 * @param WVR_CameraIntrinsic_t Return camera intrinsic.
 * @retval true The camera parameter is valid.
 * @retval false The camera parameter is not supported on your device.
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_GetCameraIntrinsic(WVR_CameraPosition position, WVR_CameraIntrinsic_t *params);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* WVR_camera_h_ */

