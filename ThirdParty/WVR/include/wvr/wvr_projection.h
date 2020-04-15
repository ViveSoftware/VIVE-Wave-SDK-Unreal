// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_projection_h_
#define wvr_projection_h_
#include "wvr_types.h"
#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function to get the projection matrix for the specified eye.
 *
 * @param eye Determines which eye the function should return the projection for.
 * @param near Distance to the near clip plane in meters
 * @param far Distance to the far clip plane in meters
 * @return The projection matrix with type @ref WVR_Matrix4f_t to use for the specified eye.
 * @version API Level 1
 */
extern WVR_EXPORT WVR_Matrix4f_t WVR_GetProjection(WVR_Eye eye, float near, float far);

/**
 * @brief Function to get the boundary of the clipping plane.
 *
 * Returns the coordinate of the margin of clipping plane for the specified eye.
 * The necessary information can be used to compose a custom matrix, though most contents should use @ref WVR_GetProjection
 * instead of this method, but sometimes a content needs to do something fancy with its projection and can use
 * these values to compute one.
 *
 * @param eye determines which eye the function should return for corresponding clipping plane.
 * @param left coordinate for the left margin of the clipping plane.
 * @param right coordinate for the right margin of the clipping plane.
 * @param top coordinate for the top margin of the clipping plane.
 * @param bottom coordinate for the bottom margin of the clipping plane.
 * @version API Level 1
 */
extern WVR_EXPORT void WVR_GetClippingPlaneBoundary(WVR_Eye eye, float *left, float *right, float *top, float *bottom);

/**
 * @brief Function to return the transform from eye space to the head space.
 *
 * Eye space is the per-eye flavor of view space that provides stereo disparity.
 * Instead of Model * View * Projection the model is Model * View * Eye * Projection.
 * Normally View and Eye will be multiplied together and treated as View in your application.
 *
 * This matrix incorporates the user's interpupillary distance (IPD).
 *
 * @param eye Determines which eye the function should return the eye matrix for.
 * @param dof Specify the DoF of current content. 6DoF transform considers the depth of eye to head but 3DoF not.
 * @return The transform between the view space and eye space.
 * @version API Level 1
*/
extern WVR_EXPORT WVR_Matrix4f_t WVR_GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof =  WVR_NumDoF_6DoF);

/**
 * @brief Function to get the suggested size of render target.
 *
 * This API provides render target size to fit display.
 *
 * @param width suggested width for the offscreen render target
 * @param height suggested height for the offscreen render target
 * @version API Level 1
 */
extern WVR_EXPORT void WVR_GetRenderTargetSize(uint32_t* width, uint32_t * height);

/**
 * @brief Function to set the ratio for overfill.
 *
 * Texture and mesh size may be larger than actual display size, this may decrease the
 * black area during the processing of ATW.
 * The ratio of width and height will be the same and apply the smaller one.
 * This function should be invoked after VR runtime initialization and before render runtime initialization.
 * This sequence help to prepare the prerequisite data in a suitable enviornment.
 *
 * @param ratioX the scale ratio of width. (must greater than or equal to 1.0)
 * @param ratioY the scale ratio of height (must greater than or equal to 1.0)
 * @version API Level 3
 */
extern WVR_EXPORT void WVR_SetOverfillRatio(float ratioX, float ratioY);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* wvr_projection_h_ */
