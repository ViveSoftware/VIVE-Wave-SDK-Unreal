// "WaveVR SDK 
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef WVR_overlay_h_
#define WVR_overlay_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Overlay errors
 *
 * Return for which error that overlay occurs.
 */
typedef enum {
    WVR_OverlayError_None                = 0,
    WVR_OverlayError_UnknownOverlay      = 10,
    WVR_OverlayError_OverlayUnavailable  = 11,
    WVR_OverlayError_InvalidParameter    = 20
} WVR_OverlayError;

/**
 * @brief Overlay type
 *
 */
typedef enum {
    WVR_OverlayType_None              = 0,
    WVR_OverlayType_CustomizeOverlay  = 2,
} WVR_OverlayType;

/**
 * @brief Overlay transformation type
 *
 */
typedef enum {
    WVR_OverlayTransformType_None,
    WVR_OverlayTransformType_Absolute,
    WVR_OverlayTransformType_Fixed
} WVR_OverlayTransformType;

/**
 * @brief Overlay flags
 *
 */
typedef enum {
    WVR_OverlayFlag_EyeLeft                 = 0x00000010,   /**< Only render this overlay on left eye */
    WVR_OverlayFlag_EyeRight                = 0x00000020,   /**< Only render this overlay on right eye */

    WVR_OverlayFlag_Reprojection            = 0x00010000,   /**< This overlay does time-warp */
} WVR_OverlayFlag;

/**
 * @brief The properties of positions for overlay
 */
typedef struct WVR_OverlayPosition {
    float x;
    float y;
    float z;
} WVR_OverlayPosition_t;

/**
 * @brief The properties of blend for overlay
 */
typedef struct WVR_OverlayBlendColor {
    float r;
    float g;
    float b;
} WVR_OverlayBlendColor_t;

/**
 * @brief The properties of texture for overlay
 */
typedef struct WVR_OverlayTexture {
    uint32_t textureId;
    uint32_t width;
    uint32_t height;
} WVR_OverlayTexture_t;

/**
 * @brief The callback of input event for overlay.
 */
typedef void (*WVR_OverlayInputEventCallback)(int32_t overlayId, WVR_EventType type, WVR_InputId inputId);

/**
 * @brief The properties of input callback for overlay
 */
typedef struct WVR_OverlayInputEvent {
    int32_t overlayId;
    WVR_OverlayInputEventCallback callback;
} WVR_OverlayInputEvent_t;

/**
 * @param overlayId  which overlay should be control.
 * @return The error code of generate overlay.
 * @brief Generate a overlay, If success, return overlayId.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_GenOverlay(int32_t *overlayId);

/**
 * @param overlayId  which overlay should be control.
 * @return The error code of delete overlay.
 * @brief Delete current overlay.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_DelOverlay(int32_t overlayId);

/**
 * @param overlayId  which overlay should be control.
 * @param position which contained the overlay's translation(x,y,z) relative to HMD.
 * @return The error code of set transform position.
 * @brief Sets the transform relative to the HMD by position.x, position.y, position.z.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_SetOverlayFixedPosition(int32_t overlayId, const WVR_OverlayPosition_t *position);

/**
 * @param overlayId  which overlay should be control.
 * @param position   which contained the overlay's translation(x,y,z) relative to HMD.
 * @return The error code of get transform position.
 * @brief Gets the transform relative to the HMD by position.x, position.y, position.z.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_GetOverlayFixedPosition(int32_t overlayId, WVR_OverlayPosition_t *position);

/**
 * @param overlayId  which overlay should be control.
 * @param type       would be WVR_OverlayTransformType_None, WVR_OverlayTransformType_Absolute, WVR_OverlayTransformType_Fixed
 * @return The error code of get transform.
 * @brief Get overlay transform type
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_GetOverlayTransformType(int32_t overlayId, WVR_OverlayTransformType *type);

/**
 * @param overlayId   which overlay should be control.
 * @param color       which contained color value of red/green/blue, the value is between 0 and 1 to set.
 * @return The error code of blend overlay color.
 * @brief Sets the color ratio to WVR_SetBlendOverlayColor color. Use 0.0 to 1.0 to per color.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_SetBlendOverlayColor(int32_t overlayId, const WVR_OverlayBlendColor_t *color);

/**
 * @param overlayId  which overlay should be control.
 * @param color      Get color sets through WVR_OverlayBlendColor_t.
 * @return The error code of get overlay color.
 * @brief Gets the color ratio from WVR_GetBlendOverlayColor color. Get red/green/blue through members .r/.g/.b.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_GetBlendOverlayColor(int32_t overlayId, WVR_OverlayBlendColor_t *color);

/**
 * @param overlayId  which overlay should be control.
 * @param alpha Alpha value to set. This should be a value between 0 (fully transparent) and 1.0 (fully opaque).
 * @return The error code of blend overlay alpha.
 * @brief Sets the alpha ratio to the overlay. Use 0.0 to 1.0 to alpha.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_SetBlendOverlayAlpha(int32_t overlayId, float alpha);

/**
 * @param overlayId  which overlay should be control.
 * @param alpha Alpha value to get.
 * @return The error code of get overlay alpha.
 * @brief Gets the alpha ratio from the overlay.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_GetBlendOverlayAlpha(int32_t overlayId, float *alpha);

/**
 * @param overlayId  which overlay should be control.
 * @return The error code of show overlay.
 * @brief Shows the overlay.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_ShowOverlay(int32_t overlayId);

/**
 * @param overlayId  which overlay should be control.
 * @return The error code of hide overlay.
 * @brief Hides the overlay.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_HideOverlay(int32_t overlayId);

/**
 * @param overlayId  which overlay should be control.
 * @param texture    which contained Texture's infomation, textureId/width and height.
 * @return The error code of set texture id.
 * @brief Set texture id which generated by user.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_SetOverlayTextureId(int32_t overlayId, const WVR_OverlayTexture_t *texture);

/**
 * @param overlayId  which overlay should be control.
 * @return The result of overlay show or hide.
 * @brief Get overlay show or hide.
 * @version API Level 1
 */
extern WVR_EXPORT bool WVR_IsOverlayShow(int32_t overlayId);

/**
 * @param callback which contained overlayId(callback.overlayId) and event callback(callback.cb), This callback will be called if a input event trigger
 * @return The error code of input event for overlay
 * @brief Register a input event for overlay
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_RegisterOverlayInputEvent(WVR_OverlayInputEvent_t *callback);

/**
 * @param callback which contained overlayId(callback.overlayId) and event callback(callback.cb), This callback will be called if a input event trigger
 * @return The error code of input event for overlay
 * @brief Unregister a input event for overlay
 * @version API Level 1
 */
extern WVR_EXPORT WVR_OverlayError WVR_UnregisterOverlayInputEvent(WVR_OverlayInputEvent_t *callback);

/**
 * @param overlayId which overlay should be control.
 * @return This overlay id is valid or not
 * @brief Overlay system will release after android suspended, so check WVR_IsOverlayValid per frame in native APP.
 * If WVR_IsOverlayValid return false, re-call WVR_GenOverlay to re-generate new overlay id.
 * @version API Level 2
 */
extern WVR_EXPORT bool WVR_IsOverlayValid(int32_t overlayId);

/**
 * @param overlayId which overlay's flags should be set.
 * @return This overlay id is valid or not
 * @brief Set the flags to this overlay.
 * @version API Level 5
 */
extern WVR_EXPORT WVR_OverlayError WVR_SetOverlayFlags(int32_t overlayId, WVR_OverlayFlag flags);

/**
 * @param overlayId which overlay's flags should be returned.
 * @return This overlay id is valid or not
 * @brief Get the flags from this overlay.
 * @version API Level 5
 */
extern WVR_EXPORT uint32_t WVR_GetOverlayFlags(int32_t overlayId);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* WVR_overlay_h_ */
