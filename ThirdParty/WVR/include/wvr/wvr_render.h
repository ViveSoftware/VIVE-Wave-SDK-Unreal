// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_render_h_
#define wvr_render_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef void* WVR_Texture_t;                                /**< **WVR_Texture_t**: type define the name of texture. */
typedef void* WVR_TextureQueueHandle_t;                     /**< **WVR_TextureQueueHandle_t**: type define the name of texture queue handler instance in runtime. */

/**
 * @brief Graphics API for render support.
 *
 * Currently, only OpenGL texture type is supported.
 * Other graphics APIs will be supported in future releases.
 */
typedef enum {
    WVR_GraphicsApiType_OpenGL                  = 1,        /**< **WVR_GraphicsApiType_OpenGL**: Specify OpenGL as the graphics API during render runtime initialization. */
} WVR_GraphicsApiType;

/**
 * @brief Render runtime initialization configuration.
 *
 * Determine whether render runtime uses the additional method to improve the user experience after runtime initialization.
 * The render configuration is a bit mask that is able to combine with these methods.
 * This bit mask should be set to the member renderConfig of **WVR_RenderInitParams_t**.
 * As a necessary part of the input parameter of the @ref WVR_RenderInit interface, the render configurations are passed to the render runtime initialization stage.
 */
typedef enum {
    WVR_RenderConfig_Default                    = 0,             /**< **WVR_RenderConfig_Default**: Runtime initialization reflects certain properties in the device service such as the single buffer mode and reprojection mechanism. The default settings are determined by the device service or runtime config file on a specific platform. The default color space is set as the linear domain. */
    WVR_RenderConfig_Disable_SingleBuffer       = ( 1 << 0 ),    /**< **WVR_RenderConfig_Disable_SingleBuffer**: Disable the single buffer mode in runtime. */
    WVR_RenderConfig_Disable_Reprojection       = ( 1 << 1 ),    /**< **WVR_RenderConfig_Disable_Reprojection**: Disable the reprojection mechanism in runtime. */
    WVR_RenderConfig_sRGB                       = ( 1 << 2 ),    /**< **WVR_RenderConfig_sRGB**: Determine whether the color space is set as a sRGB domain. */
} WVR_RenderConfig;

/**
 * @brief Error for render runtime initialization.
 *
 * The return value for the initialization interface @ref WVR_RenderInit.
 */
typedef enum {
    WVR_RenderError_None                        = 0,        /**< **WVR_RenderError_None**: No error. */
    WVR_RenderError_RuntimeInitFailed           = 410,      /**< **WVR_RenderError_RuntimeInitFailed**: The dependent component failed to block the render runtime initialization.*/
    WVR_RenderError_ContextSetupFailed          = 411,      /**< **WVR_RenderError_ContextSetupFailed**: The necessary context to set up runtime failed.*/
    WVR_RenderError_DisplaySetupFailed          = 412,      /**< **WVR_RenderError_DisplaySetupFailed**: The display configuration for the display was not set up. */
    WVR_RenderError_LibNotSupported             = 413,      /**< **WVR_RenderError_LibNotSupported**: The provided graphics API type is not supported by runtime. */
    WVR_RenderError_NullPtr                     = 414,      /**< **WVR_RenderError_NullPtr**: Did not pass Null check. */
    WVR_RenderError_Max                         = 65535,    /**< **WVR_RenderError_Max**: Maximum error code reserve. The actual value may change depending on the compiler word length. */
} WVR_RenderError;

/**
 * @brief Extend the add-on functionality for WVR_SubmitFrame.
 *
 * WVR_SubmitFrame need this flag to specify invoking path.
 * Currently, the default funcationality is available for the runtime submit texture target process.
 * If the developers have the intention of doing distortion correction by themselves,
 * it is suitable for applying the flag WVR_SubmitExtend_DisableDistortion in this invoking path.
 * The flag WVR_SubmitExtend_PartialTexture is to be applied when presenting a cropped texture.
 * The cropped range of the texture is determined by the WVR_TextureLayout struct in WVR_TextureParams.
 */
typedef enum {
    WVR_SubmitExtend_Default = 0x0000,                      /**< **WVR_SubmitExtend_Default**: The default process to submit texture to display. */
    WVR_SubmitExtend_DisableDistortion = 0x0001,            /**< **WVR_SubmitExtend_DisableDistortion**: The render runtime doesn't make distortion correction for this submit. */
    WVR_SubmitExtend_PartialTexture = 0x0010,               /**< **WVR_SubmitExtend_Partial**: Present the partial texture.*/
    WVR_SubmitExtend_SystemReserved1 = 1 << 30               /**< **System reserved.*/
} WVR_SubmitExtend;

/**
 * @brief The return type of API WVR_SubmitFrame
 *
 * Enumerate the error type of submitting frame.
 */
typedef enum {
    WVR_SubmitError_None                        = 0,        /**< **WVR_SubmitError_None**: No error */
    WVR_SubmitError_InvalidTexture              = 400,      /**< **WVR_SubmitError_InvalidTexture**: The submitted texture is invalid. Make sure texture is correct and not in use.*/
    WVR_SubmitError_ThreadStop                  = 401,      /**< **WVR_SubmitError_ThreadStop**: The submitted thread stops in the runtime. */
    WVR_SubmitError_BufferSubmitFailed          = 402,      /**< **WVR_SubmitError_BufferSubmitFailed**: The submitted buffer encounters error  in the runtime. */
    WVR_SubmitError_Max                         = 65535,    /**< **WVR_SubmitError_Max**: Maxium value to reserve bit word among compilers . */
} WVR_SubmitError;

/**
 * @brief Target texture
 *
 * Enumerate the specific target texture to generate texture.
 */
typedef enum {
    WVR_TextureTarget_2D,                   /**< **WVR_TextureTarget_2D**: Correspond to OpenGL target texture GL_TEXTURE_2D*/
    WVR_TextureTarget_2D_ARRAY,             /**< **WVR_TextureTarget_2D_ARRAY**: Correspond to OpenGL target texture GL_TEXTURE_2D_ARRAY*/
    WVR_TextureTarget_2D_EXTERNAL,          /**< **WVR_TextureTarget_2D_EXTERNAL**: Correspond to OpenGL target texture GL_TEXTURE_EXTERNAL_OES*/
    WVR_TextureTarget_2D_DUAL,              /**< **WVR_TextureTarget_2D_DUAL**: Correspond to OpenGL target texture GL_TEXTURE_2D. Both two eyes are rendered in the same texture, and therefore the texture width should be double large.*/
    WVR_TextureTarget_2D_EXT_DUAL,          /**< **WVR_TextureTarget_2D_EXT_DUAL**: Correspond to OpenGL target texture GL_TEXTURE_EXTERNAL_OES. Both two eyes are rendered in the same texture, and therefore the texture width should be double large.*/
    WVR_TextureTarget_VULKAN                /**< **WVR_TextureTarget_VULKAN**: Correspond to Vulkan target image.*/
} WVR_TextureTarget;

/**
 * @brief Texture format
 *
 * Enumerate the specific texture format to generate texture.
 */
typedef enum {
    WVR_TextureFormat_RGBA                                  /**< **WVR_TextureFormat_RGBA**: Correspond to OpenGL format of the pixel data GL_RGBA*/
} WVR_TextureFormat;

/**
 * @brief Texture type
 *
 * Enumerate the specific texture type to generate texture.
 */
typedef enum {
    WVR_TextureType_UnsignedByte                            /**< **WVR_TextureType_UnsignedByte**: Correspond to OpenGL data type of the pixel data GL_UNSIGNED_BYTE*/
} WVR_TextureType;

/**
 * @brief Screenshot mode
 *
 * Enumerate screenshot mode to select stereo scene or single eye image.
 */
typedef enum {
    WVR_ScreenshotMode_Default,                             /**< **WVR_ScreenshotMode_Default**: Screenshot image is stereo. Just as what is shown on screen*/
    WVR_ScreenshotMode_Raw,                                 /**< **WVR_ScreenshotMode_Raw**: Screenshot image has only single eye, and without distortion correction*/
    WVR_ScreenshotMode_Distorted,                           /**< **WVR_ScreenshotMode_Distorted**: Screenshot image has only single eye, and with distortion correction*/
} WVR_ScreenshotMode;

/**
 * @brief The rendering context from the native application.
 *
 * Maintain the rendering context from the native application via the render runtime initialization interface.
 */
typedef struct WVR_GraphicsParams {
    void *renderContext;                                    /**< The rendering context from the native application is for binding the surface of the VR application. */
} WVR_GraphicsParams_t;

/**
 * @brief Render initialization parameters.
 *
 * Aggregate the necessary information to initialize the render runtime.
 */
typedef struct WVR_RenderInitParams {
    WVR_GraphicsApiType graphicsApi;                        /**< Select the supported graphics API library (currently only OpenGL is supportred.) */
    uint64_t renderConfig;                                  /**< The bit mask chooses the combination of the render initialization configuration. The corresponding enumeration is defined in @ref WVR_RenderConfig */
    WVR_GraphicsParams_t graphicsParams;                    /**< The rendering context from the native application is packed here @ref WVR_GraphicsParams. */
} WVR_RenderInitParams_t;

/**
 * @brief Render properties
 *
 * Aggregate the propertie values from runtime via API WVR_GetRenderProps.
 */
typedef struct WVR_RenderProps {
    float refreshRate;                                      /**< The refresh rate is the number of frame shown in a second.*/
    bool hasExternal;                                       /**< To probe whether the present display is an external device.*/
    float ipdMeter;                                         /**< The presumed interpupillary distance of HMD user, measured in meters.*/
} WVR_RenderProps_t;

/**
 * @brief Texture layout
 *
 * Store the lower and upper UV coordinates to specify the region of submitting texture.
 * The left-low coordinates are kept in lowerLeftUVs.
 * The right-up coordinates are kept in upperRightUVs.
 */
typedef struct WVR_TextureLayout {
    WVR_Vector2f leftLowUVs;                               /**< Component 0 and 1 are left-low UV coordinates of the texture. */
    WVR_Vector2f rightUpUVs;                               /**< Component 0 and 1 are right-up UV coordinates of the texture. */
} WVR_TextureLayout_t;

/**
 * @brief Texture parameters
 *
 * Aggregate the texture related information which needs to be updated to runtime.
 * The texture parameters can be obtained via WVR_GetTexture or assigned a valid texture directly.
 * The WVR_SubmitFrame need this struct as an argument to pass current texture identiy to runtime.
 */
typedef struct WVR_TextureParams {
    WVR_Texture_t id;                               /**< Correspond to the name of texture to identify texture entity in graphics library such as OpenGL. */
    WVR_TextureTarget target;                       /**< Specifies the target to which the texture is bound. The parameter corresponds to the texture targe in graphics library such as OpenGL. */
    WVR_TextureLayout_t layout;                      /**< The lower left and upper right UV coordinates pair specifies the presenting region of submitted texture. */
} WVR_TextureParams_t;

/**
 * @brief Peripheral Quality which using in @ref WVR_RenderFoveationParams_t.
 */
typedef enum {
    WVR_PeripheralQuality_Low    = 0x0000,           /**< **WVR_PeripheralQuality_Low**: Peripheral quity is low and power saving is high. */
    WVR_PeripheralQuality_Medium = 0x0001,           /**< **WVR_PeripheralQuality_Medium**: Peripheral quity is medium and power saving is medium. */
    WVR_PeripheralQuality_High   = 0x0002,           /**< **WVR_PeripheralQuality_High**:  Peripheral quity is high and power saving is low. */
} WVR_PeripheralQuality;

/**
 * @brief Foveation parameters
 *
 * Aggregate custom informations of foveation. Using in @ref WVR_PreRenderEye.
 */
typedef struct WVR_RenderFoveationParams {
    float focalX;                                    /**< **focalX**: The x coordinate of the focal point in normalized device coordinates. */
    float focalY;                                    /**< **focalY**: The y coordinate of the focal point in normalized device coordinates. */
    float fovealFov;                                 /**< **fovealFov**: The fov of the foveal.*/
    WVR_PeripheralQuality periQuality;               /**< **periQuality**: The peripheral quality of foveation. */
} WVR_RenderFoveationParams_t;

/**
 * @brief Aggregate the function pointers to support the callbacks of runtime.
 *
 * Runtime maintains a stereo renderer mode to handle the customized callback at the specific moment.
 * Native application developer implements all the callbacks and delegates the invoking timing to runtime.
 * The optimized algorithm improves the stereoscopic experience in render runtime with single buffer method.
 *
 */
typedef struct WVR_StereoRenderer {
    /**
     * @brief To initialize graphics library.
     *
     * Developer can enable server-side capabilities such as doing depth comparisons and update the depth buffer,
     * specifying the value used for depth buffer comparisons, or specifying whether depth buffer is enabled for writing.
     * Based on developer's requirement, to set up the scene objects, frame buffer for stereo render targets, or render models.
     *
     * @retval true succeed to initialize graphics library.
     * @retval false fail to initialize graphics library.
     */
    bool (*graphics_init)();
    /**
     * @brief Called when a new frame is about to be drawn.
     *
     * Any per-frame operations not specific to a single view should happen here.
     *
     * @retval true succeed to invoke this function.
     * @retval false fail to invoke this function.
     */
    bool (*pre_draw_frame)();
    /**
     * @brief Requests to draw the contents from the point of view of an eye.
     *
     * Implementor should draw the frame buffer corresponding to the different eye.
     * In order to submit the content of view as texture to runtime, binding the framebuffer, rendering the scene,
     * packing the texture id as the argument of @ref WVR_SubmitFrame.
     *
     * @param eye WVR_Eye, eye id.
     */
    void (*draw_eye)(WVR_Eye eye);
    /**
     * @brief Called before a frame is finished.
     *
     * By the time of this call, the frame contents have been already drawn.
     * Any rendering in this step is relative to the full surface, not to any single eye view,
     * such as polling the pose of devices.
     *
     */
    void (*post_draw_frame)();
} WVR_StereoRenderer_t;

/**
* @brief Use this interface to initiate the render runtime.
*
* Render runtime is in charge of pre-processing the rendered scene before updating to the display.
* To initiate render runtime, it is recommended to invoke this interface before the following interfaces in some platforms,
* such as @ref WVR_RenderMask, @ref WVR_SubmitFrame, @ref WVR_ObtainTextureQueue, @ref WVR_GetTextureQueueLength, @ref WVR_GetAvailableTextureIndex,
* @ref WVR_GetTexture, @ref WVR_ReleaseTextureQueue,
* @ref WVR_RequestScreenshot, @ref WVR_StartRenderer, @ref WVR_IsRendererRendering, and @ref WVR_GetSyncPose.
*
* Instead of the default configuration and selecting the OpenGL graphics API, Invoking @ref WVR_RenderInit first can provide
* the customized initiating configuration and select the supported graphics library via the argument **WVR_RenderInitParams_t**.
* Based on the return type @ref WVR_RenderError, the status of the render runtime initialization can be determined.
*
* @param param Pointer of struct **WVR_RenderInitParams_t** to aggregate the necessary information to initialize the render runtime.
* @return WVR_RenderError, this return type enumerates all the possible error statuses for this interface.
* @version API Level 1
*/
extern WVR_EXPORT WVR_RenderError WVR_RenderInit(const WVR_RenderInitParams_t* param);

/**
* @brief Requests the properties from render runtime.
*
* The interface fetch necessary properties from render runtime.
* The output argument WVR_RenderProps_t* carries the frame rate and the existing status of the external display.
* The corresponding member value can be access via dereferencing the interface parameter referring to @ref WVR_RenderProps_t.
* Return value is the validness of properties. If it is not true, it means the fetched value is not available.
* Please note that some platforms return false rather than fetching render properties.
*
* @param props pointer of struct WVR_RenderProps_t, to aggregate the properties fetched from render runtime.
* @retval true Success to get the render properties.
* @retval false Fail to get the render properties, the fetched properties value is invalid.
* @version API Level 1
*/
extern WVR_EXPORT bool WVR_GetRenderProps(WVR_RenderProps_t* props);

/**
* @brief Renders a mask to cover the pixels that will be hidden after distortion.
*
* The mask will be drawn into the depth buffer setting near clipping distance in the z-direction.
* This will improve performance by letting the GPU early-reject pixels the user
* will never see.
* Called before rendering the scene texture of each eye.
*
* @param eye WVR_Eye, eye id to specify the side of scene.
* @param target WVR_TextureTarget, Specifies the target texture.
* @version API Level 1
*/
extern WVR_EXPORT void WVR_RenderMask(WVR_Eye eye, WVR_TextureTarget target = WVR_TextureTarget_2D);

/**
* @brief Update scene image to display.
*
* The scene is rendered by developer with both sides of eye.
* The scene content is in form of index or pointer according to the supported graphics API, such as texture name in OpenGL.
* Developer can submit the image(s) of both eyes at most once within one Vsync duration to signal the runtime to process.
* The WVR_TextureParams_t contains the texture which specifies the current scene. This scene is already rendered beforehand.
* This texture is verified its validness in advance of updating it to display.
*
* A much more accurate polling pose approximating the moment that scene is rendered, provides much more stabilized performance of rendering.
* The fourth is a flag to specify the internal invoking path.
* The default process of handing scene fulfills all the basic requirement of updating scene to display.
* Add-on functionalities will be appended in the future.
*
* The update frequency is limited by the refresh rate of the display. The checkpoints which check if the application can start to render the next frame is set to every middle of two adjacent Vsyncs. Therefore, this API will not return until the next checkpoint is coming. Moreover, the caller will be blocked for one or more checkpoints if the previous frame has not completed the rendering yet.
*
* @param eye WVR_Eye, eye id to specify the side of scene.
* @param param pointer to struct WVR_TextureParams_t, to aggregate the name of scene texture.
* @param pose pointer to struct WVR_PoseState_t, the pose used for rendering this frame.
* @param extendMethod enum WVR_SubmitExtend, flag to specify invoking path.
* @return WVR_SubmitError, the error status of interface WVR_SubmitFrame, the possible status can refer to @ref WVR_RenderError.
* @version API Level 1
*/
extern WVR_EXPORT WVR_SubmitError WVR_SubmitFrame(WVR_Eye eye, const WVR_TextureParams_t *param, const WVR_PoseState_t* pose = NULL, WVR_SubmitExtend extendMethod = WVR_SubmitExtend_Default);

/**
* @brief Obtain a new texture queue.
*
* Several texture queue related methods are provided for maintaining textures.
* Call @ref WVR_ReleaseTextureQueue to release the queue when the queue is no longer used.
*
* @param target Specifies the target texture. For target **WVR_TextureTarget_2D_EXTERNAL**, @ref WVR_ObtainTextureQueue only create textures without EGLImage objects. Therefore, user are expected to bind EGLImage to textures by glEGLImageTargetTexture2DOES, Android SurfaceTexture etc..
* @param format Specifies the number of color components in the texture. Must be WVR_TextureFormat_RGBA.
* @param type Specifies the data type of the pixel data. The following symbolic values are accepted: WVR_TextureType_UnsignedByte.
* @param width Specifies the width of the texture image. For targets WVR_TextureTarget_2D_DUAL and WVR_TextureTarget_2D_EXT_DUAL, the width should be double large.
* @param height Specifies the height of the texture image.
* @param level Specifies the level-of-detail number.
* @return The handle of the queue @ref WVR_TextureQueueHandle_t.
* @version API Level 1
*/
extern WVR_EXPORT WVR_TextureQueueHandle_t WVR_ObtainTextureQueue(WVR_TextureTarget target, WVR_TextureFormat format, WVR_TextureType type, uint32_t width, uint32_t height, int32_t level);

/**
* @brief Get the length of a texture queue.
* @param handle The texture queue handle returned by @ref WVR_ObtainTextureQueue
* @return The length of the queue.
* @version API Level 1
*/
extern WVR_EXPORT uint32_t WVR_GetTextureQueueLength(WVR_TextureQueueHandle_t handle);

/**
* @brief Get the index of an available texture.
*
* Before drawing, applications should call this function to get the index of an available
* texture and call @ref WVR_GetTexture to get the real texture ID.
*
* @param handle The texture queue handle returned by @ref WVR_ObtainTextureQueue
* @return The ID of the texture. -1 means no available texture currently or invalid handle.
* @version API Level 1
*/
extern WVR_EXPORT int32_t WVR_GetAvailableTextureIndex(WVR_TextureQueueHandle_t handle);

/**
* @brief Get texture of given index in the given texture queue.
*
* Use the index returned from @ref WVR_GetAvailableTextureIndex.
* The available index can specify the available texture in texture queue maintained in runtime.
*
* @param handle The texture queue handle returned by @ref WVR_ObtainTextureQueue
* @param index The index of the texture in the queue.
* @return The struct WVR_TextureParams_t. aggregate the name of texture as a member inside the struct.
* @version API Level 1
*/
extern WVR_EXPORT WVR_TextureParams_t WVR_GetTexture(WVR_TextureQueueHandle_t handle, int32_t index);

/**
* @brief Release a texture queue.
* @param handle The handle of the texture queue to be released.
* @version API Level 1
*/
extern WVR_EXPORT void WVR_ReleaseTextureQueue(WVR_TextureQueueHandle_t handle);

/**
* @brief Request a screenshot of requested mode.
* @param width Specifies the width of the texture image.
* @param height Specifies the height of the texture image.
* @param mode Specifies requested screenshot mode. The values should be one of @ref WVR_ScreenshotMode.
* @param filename Specifies preferred saving filename. (optional)
* @retval true Succeed to request screenshot.
* @retval false Fail to request screenshot.
* @version API Level 1
*/
extern WVR_EXPORT bool WVR_RequestScreenshot(uint32_t width, uint32_t height, WVR_ScreenshotMode mode = WVR_ScreenshotMode_Default, const char* filename = NULL);

/**
* @brief Send the callback function set to runtime.
*
* Runtime needs the callback function implemented by developer.
* Runtime hooks the member functions pointer of @ref WVR_StereoRenderer_t with function set from external.
* This functions also trigger runtime to achieve the algorithm of direct mode method.
*
* @param renderer pointer to struct WVR_StereoRenderer, aggregate the function pointers to support the callback of runtime.
* @version API Level 1
*/
extern WVR_EXPORT void WVR_StartRenderer(WVR_StereoRenderer_t* renderer);

/**
* @brief Check whether the direct mode is still working.
*
* Runtime will clear the callback set when it is going to terminate the invoking delegation.
* Check existence of callback function set in runtime. If the renderer is null then return false. If on the contrary then returning true.
* This function is used to handle the life cycle of the native activity in the main loop. The render thread is held in runtime.
* Activity can proceed to terminating state when the render thread is going to put an end to the process.
*
* @retval true callback set is still presence in runtime.
* @retval false callback set is no longer existing in runtime.
* @version API Level 1
*/
extern WVR_EXPORT bool WVR_IsRendererRendering();

/**
* @brief Check whether foveation is support.
*
* This function must be called after calling WVR_RenderInit.
*
* @retval true Foveation feature is support in runtime.
* @retval false Foveation feature is not support  in runtime.
* @version API Level 3
*/
extern WVR_EXPORT bool WVR_IsRenderFoveationSupport();

/**
* @brief Enable or disable foveation rendering (beta).
*
* Using to enable or disable foveation rendering.
* And developer need to call @ref WVR_PreRenderEye to take foveation effect on texture.
* This function is platform-depent, must call @ref WVR_IsRenderFoveationSupport to check does this platform support this.
*
* @param enable to enable or diable foveation.
* @version API Level 3
*/
extern WVR_EXPORT void WVR_RenderFoveation(bool enable);

/**
* @brief Runtime do extra working on texture.
*
* Need to be called before developer rendering any objects of scene onto texture.
* Failing to call this API will caused some functions are not working.
*
* @param eye WVR_Eye, eye id to specify the side of scene.
* @param textureParam pointer to struct WVR_TextureParams_t, to aggregate the name of scene texture.
* @param foveatParam pointer to struct WVR_RenderFoveationParams_t, to aggregate custom informations of foveation. (optional)
* @version API Level 3
*/
extern WVR_EXPORT void WVR_PreRenderEye(WVR_Eye eye, const WVR_TextureParams_t *textureParam, const WVR_RenderFoveationParams_t* foveatedParam = NULL);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_render_h_ */

