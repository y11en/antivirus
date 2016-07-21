#ifndef __COMMON_NOTIFICATION_EVENT_IDS_H_
#define __COMMON_NOTIFICATION_EVENT_IDS_H_

// The event IDs from the range [CNE_COMPATIBLE_EVENTS_MIN...CNE_COMPATIBLE_EVENTS_MAX] are reserved
// and must not be used for new events

#define CNE_COMPATIBLE_EVENTS_MIN		0x00000000
#define CNE_COMPATIBLE_EVENTS_MAX		0x00000400

// The event IDs from the range [CNE_LOCAL_EVENTS_MIN...CNE_LOCAL_EVENTS_MAX] are reserved
// for new application local events

#define CNE_LOCAL_EVENTS_MIN			(CNE_COMPATIBLE_EVENTS_MAX + 1)
#define CNE_LOCAL_EVENTS_MAX			0x3FFFFFFF

// The event IDs from the range [CNE_COMMON_EVENTS_MIN...CNE_COMMON_EVENTS_MAX] are reserved
// for new events of commonly used objects

#define CNE_COMMON_EVENTS_MIN			(CNE_LOCAL_EVENTS_MAX + 1)
#define CNE_COMMON_EVENTS_MAX			0x7FFFFFFF

// The event IDs from the range [CNE_WINDOWS_EVENTS_MIN...CNE_WINDOWS_EVENTS_MAX] are used
// by Windows (Standard dispatch ID constants reserved for use by Automation)

#define CNE_WINDOWS_EVENTS_MIN			0x80000000
#define CNE_WINDOWS_EVENTS_MAX			0xFFFFFFFF

///////////////////////////////////////////////////////////////////////////////////
//
// Currently used common IDs
//
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// Range							Used by
// ________________________________________________________________________________
//
// 0x40000101..0x40000150			KLComm object

#endif