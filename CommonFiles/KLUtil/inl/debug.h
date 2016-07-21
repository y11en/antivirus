#ifndef __KLUTIL_DEBUG_H__
#define __KLUTIL_DEBUG_H__

#include <assert.h>

///////////////////////////////////////////////////////////////////////////
// Compile-time report macros
///////////////////////////////////////////////////////////////////////////

#define KLUTIL_DBG_MAKESTR(x)   #x
#define KLUTIL_DBG_MAKESTR2(x)  KLUTIL_DBG_MAKESTR(x)

// Usage:
// #pragma BUILD_MSG("Hi Mom")
// #pragma TODO("Finish this Off")
#define BUILD_MSG(desc)    message(__FILE__ "(" KLUTIL_DBG_MAKESTR2(__LINE__) "): " desc)
#define TODO(desc)         message(__FILE__ "(" KLUTIL_DBG_MAKESTR2(__LINE__) "): TODO: " desc)

///////////////////////////////////////////////////////////////////////////
// Assertion macros
///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#define verify(f)          assert(f)
#else
	#define verify(f)          ((void)(f))
#endif

#endif