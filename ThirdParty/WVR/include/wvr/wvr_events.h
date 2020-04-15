// "WaveVR SDK
// © 2017 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."


#ifndef wvr_events_h_
#define wvr_events_h_

#include "wvr_stdinc.h"
#include "wvr_types.h"

#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Common Event, including WVR_EventType, timestamp
 */
typedef struct WVR_CommonEvent {
    WVR_EventType     type;             /**< The types of event */
    int64_t           timestamp;        /**< Delivered time in nanoseconds */
} WVR_CommonEvent_t;

/**
 * @brief Device Event, including WVR_EventType, timestamp, WVR_DeviceType
 */
typedef struct WVR_DeviceEvent {
    WVR_CommonEvent_t common;
    WVR_DeviceType    deviceType;       /**< Post event by @ref WVR_DeviceType */
} WVR_DeviceEvent_t;

/**
 * @brief Input Event, including WVR_EventType, timestamp, WVR_DeviceType, WVR_InputId
 */
typedef struct WVR_InputEvent {
    WVR_DeviceEvent_t device;
    WVR_InputId       inputId;          /**< Post event by @ref WVR_InputId */
} WVR_InputEvent_t;

/**
 * @brief WVR_Event, event type has been designed as union type to provide an efficient way of using the same
 * memory location for diffent event type. Developer can get the same member type to know the gotten event belongs
 * to which WVR_Event type, then get the specific memory region.
 */
typedef union WVR_Event {
    WVR_CommonEvent_t       common;   /**< @ref WVR_CommonEvent_t*/
    WVR_DeviceEvent_t       device;   /**< @ref WVR_DeviceEvent_t*/
    WVR_InputEvent_t        input;    /**< @ref WVR_InputEvent_t*/
} WVR_Event_t;

/**
 * @brief Function to get next event in event queue

 * @param event a struct of Event information
 * @return true means fill event struct with the next event in the queue, false means no event in the queue
 * @version API Level 1
 */
extern WVR_EXPORT bool WVR_PollEventQueue(WVR_Event_t *event);

#ifdef __cplusplus
} /* extern "C" */
#endif
#include "close_code.h"

#endif /* wvr_events_h_ */


