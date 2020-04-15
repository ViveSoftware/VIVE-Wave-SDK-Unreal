// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


/**
 *  \file WVR.h
 *
 *  Main include header for the WVR library
 */


#ifndef wvr_h_
#define wvr_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief app type for initialize VR runtime
 *
 * Describes what kind of app is being initialized.
 */
typedef enum {
    WVR_AppType_VRContent       = 1,   /**< The app type of thiis content is VR content */
    WVR_AppType_ARContent       = 2,   /**< The app type of thiis content is AR content */
} WVR_AppType ;

/**
 * @brief returned error of initialize VR runtime
 *
 * Describes what kind of error is being returned by the WVR_Init function
 */
typedef enum {
    WVR_InitError_None           = 0,   /**< Init successed */
    WVR_InitError_Unknown        = 1,   /**< Unknown error */
    WVR_InitError_NotInitialized = 2,   /**< Init failed */
} WVR_InitError ;

/**
 * @brief supported feature for id
 */
typedef enum {
    WVR_SupportedFeature_PassthroughImage   = 1<<0,    /**< Passthrough image feature type */
    WVR_SupportedFeature_PassthroughOverlay = 1<<1,    /**< Passthrough overlay feature type */

    WVR_SupportedFeature_HandTracking       = 1<<4,    /**< Hand tracking feature type */
    WVR_SupportedFeature_HandGesture        = 1<<5,    /**< Hand gesture feature type */
} WVR_SupportedFeature;

/**
 * @brief The entry function pointer of native application.
 *
 * The pointer of the entry function is set to associate with runtime library.
 * @version API Level 1
 */
typedef int (* WVR_Main_t)(int argc, char *argv[]);
/**
 * @brief Interface to register the main function.
 *
 * When VM needs to load native library and call JNI_Onload, runtime expect the entry function of native application registed here via this API.
 *
 * @param WVR_Main_t the entry function pointer of native application.
 * @version API Level 1
 */
extern WVR_EXPORT void WVR_RegisterMain(WVR_Main_t main);

/**
 * @brief Init the VR runtime.
 *
 * @param type which indicate the application type for this rumtime initialization. **NOTE:** Now this param is ignored by system.
 * @return enum WVR_InitError to know what error occurs
 * @version API Level 1
 */
extern WVR_EXPORT WVR_InitError WVR_Init(WVR_AppType type = WVR_AppType_VRContent);

/**
 * @brief Quit the VR runtime.
 *
 * Close and release all the resource allocated form VR runtime.
 * @version API Level 1
 */
extern WVR_EXPORT void WVR_Quit();

/**
 * @brief Get the error description for the WVR_InitError
 *
 * @param error which indicate WVR_InitError type.
 * @return const char*, refer the string description to know what error occurs
 * @version API Level 1
 */
extern WVR_EXPORT const char* WVR_GetInitErrorString( WVR_InitError error );

/**
 * @brief Get the runtime version
 *
 * @return refer the runtime version, equal 0 is error
 * @version API Level 1
 */
extern WVR_EXPORT uint32_t WVR_GetWaveRuntimeVersion();

/**
 * @brief Get the sdk version
 *
 * @return refer the sdk version, equal 0 is error
 * @version API Level 1
 */
extern WVR_EXPORT uint32_t WVR_GetWaveSDKVersion();


/**
 * @brief Function to get if feature is supportted
 *
 * This API can work no matter before or after invoking WVR_Init
 * @return the bitmask of all supported features
 * @version API Level 5
 */
extern WVR_EXPORT uint64_t WVR_GetSupportedFeatures();

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_h_ */
