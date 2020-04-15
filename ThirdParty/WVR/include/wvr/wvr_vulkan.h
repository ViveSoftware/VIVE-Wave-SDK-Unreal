// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_vulkan_h_
#define wvr_vulkan_h_

#include "wvr_render.h"
#include "wvr_stdinc.h"
#include "wvr_types.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

// From <vulkan/vulkan.h>:
#if !defined(VK_VERSION_1_0)
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || \
    defined(_M_X64) || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) ||       \
    defined(__powerpc64__)
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T* object;
#else
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#endif
VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_HANDLE(VkPhysicalDevice)
VK_DEFINE_HANDLE(VkDevice)
VK_DEFINE_HANDLE(VkQueue)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImage)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDeviceMemory)
#define VK_NULL_HANDLE 0
#endif

/**
 * @brief Vulkan initialization parameters
 *
 * Aggregate necessary information for Vulkan support.
 */
typedef struct WVR_VulkanSystemInfo {
    VkInstance          instance;
    VkPhysicalDevice    physicalDevice;
    VkDevice            device;
} WVR_VulkanSystemInfo_t;

/**
* @brief Initializes the API for Vulkan support.
*
* This must be called after @ref WVR_RenderInit and before @ref WVR_ObtainTextureQueue.
*
* @prarm vulkanInfo pointer of struct `WVR_VulkanSystemInfo_t`, to aggregate necessary information for Vulkan support.
* @retval true Success to set the necessary information.
* @retval false Fail to set the necessary information.
* @version API Level 3
*/
extern WVR_EXPORT bool WVR_VulkanInit(const WVR_VulkanSystemInfo_t* vulkanInfo);

/**
* @brief Get the VkImage of given index in the given texture queue.
*
* The index number must not be larger than the value returned from @ref WVR_GetTextureQueueLength
*
* @param handle The texture queue handle returned by @ref WVR_ObtainTextureQueue
* @param index The index of the VkImage in the queue.
* @return The desired VkImage.
* @version API Level 3
*/
extern WVR_EXPORT VkImage WVR_GetVkImage(WVR_TextureQueueHandle_t handle, int32_t index);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_vulkan_h_ */
