#ifndef __MCOUAS_TRACE_H__
#define __MCOUAS_TRACE_H__

#define _TRACEHR(_hr, _tracestr) \
{ \
	if(FAILED(_hr)) \
		PR_TRACE((0, prtERROR, _tracestr " returned 0x%08x (0x%08x)", _hr, GetLastError())); \
	else \
		PR_TRACE((0, prtIMPORTANT, _tracestr " succeeded")); \
}

#endif