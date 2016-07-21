#ifndef __KLDRV_DEBUG_H_
#define __KLDRV_DEBUG_H_

#define TRACE_LEVEL_NONE        0   // Tracing is not on
#define TRACE_LEVEL_FATAL       1   // Abnormal exit or termination
#define TRACE_LEVEL_ERROR       2   // Severe errors that need logging
#define TRACE_LEVEL_WARNING     3   // Warnings such as allocation failure
#define TRACE_LEVEL_INFORMATION 4   // Includes non-error cases(e.g.,Entry-Exit)
#define TRACE_LEVEL_VERBOSE     5   // Detailed traces from intermediate steps

#define WPP_CHECK_FOR_NULL_STRING  //to prevent exceptions due to NULL strings

#define WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID(KlifGuid,(DD2F61EC, 2B24, 4cff, 86C5, AD6461A51899),  \
	WPP_DEFINE_BIT(DC_DRIVER) \
	WPP_DEFINE_BIT(DC_COMM) \
	WPP_DEFINE_BIT(DC_LLDISKIO) \
	WPP_DEFINE_BIT(DC_ICHDRV) \
	WPP_DEFINE_BIT(DC_SYSTEM) \
	WPP_DEFINE_BIT(DC_FIDBOX) \
	WPP_DEFINE_BIT(DC_INVISIBLE) \
	WPP_DEFINE_BIT(DC_FILTER) \
	WPP_DEFINE_BIT(DC_CLIENT) \
	WPP_DEFINE_BIT(DC_EVQUEUE) \
	WPP_DEFINE_BIT(DC_CACHE) \
	WPP_DEFINE_BIT(DC_HIPS) \
	)

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level  >= lvl)

#if DBG == 0
	#define _dbg_break_
#else
	#define _dbg_break_		DbgBreakPoint()
#endif // dbg

#endif //__KLDRV_DEBUG_H_
