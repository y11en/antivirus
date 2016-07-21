#ifndef AVP_DEBUG_H_included
#define AVP_DEBUG_H_included

//\\//\\//\\\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// assertion & trace macros support

#ifdef _DEBUG

//#define TRACE_LEVEL SomeNumber
// must be declared before including AvpDebug.h

#include "tchar.h"

#ifdef _UNICODE
#define KLabDbgTrace KLabDbgTraceW
#else
#define KLabDbgTrace KLabDbgTraceA
#endif

void KLabDbgTraceA(const char *pszFileName, int nLine, BOOL bAssert, const CHAR *pszFormat, ...);
void KLabDbgTraceW(const char *pszFileName, int nLine, BOOL bAssert, const WCHAR *pszFormat, ...);

#define KL_ASSERT(exp)			\
	if ( !(exp) )				\
		KLabDbgTraceA(__FILE__, __LINE__, TRUE, #exp)	

#define KL_VERIFY(exp) KL_ASSERT(exp)			

#define KL_TRACEL(level, msg)	\
	if((level) <= TRACE_LEVEL)	\
		KLabDbgTrace(__FILE__, __LINE__, FALSE, (msg))

#define KL_TRACEL1(level, msg, arg1) \
	if((level) <= TRACE_LEVEL)		\
		KLabDbgTrace(__FILE__, __LINE__, FALSE, (msg), (arg1))

#define KL_TRACEL2(level, msg, arg1, arg2)	\
	if((level) <= TRACE_LEVEL)				\
		KLabDbgTrace(__FILE__, __LINE__, FALSE, (msg), (arg1), (arg2))

#define KL_TRACEL3(level, msg, arg1, arg2, arg3)	\
	if((level) <= TRACE_LEVEL)				\
		KLabDbgTrace(__FILE__, __LINE__, FALSE, (msg), (arg1), (arg2), (arg3))

#define KL_TRACE(msg)						KL_TRACEL(1, msg)
#define KL_TRACE1(msg, arg1)				KL_TRACEL1(1, msg, arg1)
#define KL_TRACE2(msg, arg1, arg2)			KL_TRACEL2(1, msg, arg1, arg2)
#define KL_TRACE3(msg, arg1, arg2, arg3)	KL_TRACEL3(1, msg, arg1, arg2, arg3)

#else   //... _DEBUG not defined

#define KL_ASSERT(exp)
#define KL_VERIFY(exp) (exp)	

#define KL_TRACE(msg) 
#define KL_TRACE1(msg, arg1) 
#define KL_TRACE2(msg, arg1, arg2) 
#define KL_TRACE3(msg, arg1, arg2, arg3) 

#define KL_TRACEL(level, msg) 
#define KL_TRACEL1(level, msg, arg1) 
#define KL_TRACEL2(level, msg, arg1, arg2) 
#define KL_TRACEL3(level, msg, arg1, arg2, arg3) 

#endif
//\\//\\//\\\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//


#endif