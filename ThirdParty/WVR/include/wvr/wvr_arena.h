// "WaveVR SDK 
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_arena_h_
#define wvr_arena_h_

#include "wvr_types.h"
#include "wvr_stdinc.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief To decide whether vitual wall should be showed.
 */
typedef enum {
    WVR_ArenaVisible_Auto     = 0, /**< Show Arena while HMD is near boundaries */
    WVR_ArenaVisible_ForceOn  = 1, /**< Always show Arena */
    WVR_ArenaVisible_ForceOff = 2 /**< Never show Arena */
} WVR_ArenaVisible;

/**
 * @brief Supported play area shape
 */
typedef enum
{
    WVR_ArenaShape_None      = 0, /**< Arena shape is undefined */
    WVR_ArenaShape_Rectangle = 1, /**< Arena shape is rectangle */
    WVR_ArenaShape_Round     = 2 /**< Arena shape is round */
} WVR_ArenaShape;

/**
 * @brief Describes rectangle Arena area (play area) size parameters
 */
typedef struct WVR_ArenaRectangle {
    float width;  /**< The width of rectangle Arena area */
    float length; /**< The length of rectangle Arena area */
} WVR_ArenaRectangle_t;

/**
 * @brief Describes round Arena area (play area) size parameter
 */
typedef struct WVR_ArenaRound {
    float diameter; /**< The diameter of round Arena area */
} WVR_ArenaRound_t;

/**
 * @brief Describes 2 types of Arena area (play area) range parameters
 */
typedef union WVR_ArenaArea {
    WVR_ArenaRectangle_t rectangle; /**< The setting of rectangle Arena area (refer to @ref WVR_ArenaRectangle) */
    WVR_ArenaRound_t round; /**< The setting of round Arena area (refer to @ref WVR_ArenaRound) */
} WVR_ArenaArea_t;

/**
 * @brief Describes current effective Arena area (play area) shape and its related range parameters
 */
typedef struct WVR_Arena {
    WVR_ArenaShape shape; /**< The setting of Arena shape */
    WVR_ArenaArea_t area; /**< The setting of Arena area (refer to @ref WVR_ArenaArea) */
} WVR_Arena_t;

/**
 * @brief Function to return the current supported Play Area range info
 *
 * It depends on play area shape returned to check its range.
 * For Rectangle, check rectangle.
 * For Round, check round.
 *
 * @return  The supported shape and its related play area range. (refer to @ref WVR_Arena)
 * @version API Level 1
*/
extern WVR_EXPORT WVR_Arena_t WVR_GetArena();

/**
 * @brief Function to set play area size info.
 *
 * Uses this function to change play area range.
 * The maximum range is depended on device specific. It will make valid check for those settings.
 *
 * @param arena The new setting supported shape and its related play area range.  (refer to @ref WVR_Arena)
 * @retval true The setting is valid.
 * @retval false The setting is invalid.
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_SetArena(const WVR_Arena_t* arena);

/**
 * @brief Function to get virtual wall behavior.
 *
 * In auto mode, it will show virtual wall while HMD is out of bounds.
 * Force on mode will always show virtual wall.
 * Force off mode will always disable virtual wall.
 *
 * @return  The configuration of virtual wall behavior. (refer to @ref WVR_ArenaVisible)
 * @version API Level 1
*/
extern WVR_EXPORT WVR_ArenaVisible WVR_GetArenaVisible();

/**
 * @brief Function to set virtual wall behavior.
 *
 * In auto mode, it will show virtual wall while HMD is out of bounds.
 * Force on mode will always show virtual wall.
 * Force off mode will always disable virtual wall.
 * There are only AUTO and FORCE ON effective for general Apps. As to FORCE OFF, a Developer Mode bit must be set in Server DB in advance.
 *
 * @param config The configuration of virtual wall behavior. (refer to @ref WVR_ArenaVisible)
 * @version API Level 1
*/
extern WVR_EXPORT void WVR_SetArenaVisible(WVR_ArenaVisible config);

/**
 * @brief Function to tell whether user reached play area boundary.
 *
 * Uses this function to decide whether the virtual wall should be showed to player or not.
 *
 * @retval true User is reaching or outside play area boundary (The virtual wall should be showed to player.)
 * @retval false User is inside the play area.
 * @version API Level 1
*/
extern WVR_EXPORT bool WVR_IsOverArenaRange();

/**
 * @brief Function to show the passthrough overlay when a player passes through the virtual wall.
 *
 * Use this function to also decide whether the passthrough overlay should be shown or not when a player passes through the virtual wall.
 * This function must be called after calling @ref WVR_RenderInit().
 *
 * @param Enable or disable the passthrough overlay when a player passes through the virtual wall.
 * @version API Level 5
 * @note Supported from Runtime version 5 or higher (certain devices only). Make sure the target device supports passthrough overlay by calling @ref WVR_GetSupportedFeatures() and checking @ref WVR_SupportedFeature_PassthroughOverlay.
*/
extern WVR_EXPORT void WVR_EnableAutoPassthrough(bool enable);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* wvr_arena_h_ */
