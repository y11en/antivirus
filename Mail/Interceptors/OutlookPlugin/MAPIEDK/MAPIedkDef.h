#pragma once

#include "MAPIEDK.h"
extern CMAPIEDK g_MAPIEDK;

#ifdef HR_LOG
#undef HR_LOG
#endif

#define HR_LOG(_hr) g_MAPIEDK.HrLog(__FILE__, __LINE__, (_hr), GetLastError())


#ifdef RETURN
#undef RETURN
#endif

#define RETURN(_hr) return (g_MAPIEDK.HrLog(__FILE__, __LINE__, (_hr), GetLastError()))

#ifdef FAILED
#undef FAILED
#endif

#define FAILED(_hr) g_MAPIEDK.HrFailed(__FILE__, __LINE__, (_hr), GetLastError())

#ifdef SUCCEEDED
#undef SUCCEEDED
#endif

#define SUCCEEDED(_hr) (!g_MAPIEDK.HrFailed(__FILE__, __LINE__, (_hr), GetLastError()))

#ifdef DEBUGPUBLIC
#undef DEBUGPUBLIC
#endif

#define DEBUGPUBLIC(x) { (g_MAPIEDK.Assert("DEBUG", __FILE__, __LINE__, D_PUBLIC, TRUE, "%s\n", (x))); }

#ifdef DEBUGPRIVATE
#undef DEBUGPRIVATE
#endif

#define DEBUGPRIVATE(x) { g_MAPIEDK.Assert("DEBUG", __FILE__, __LINE__, D_PRIVATE, TRUE, "%s\n", (x)); }

#ifdef MODULE_WARNING
#undef MODULE_WARNING
#endif

#define MODULE_WARNING(x) \
{ g_MAPIEDK.Assert("ASSERT", __FILE__, __LINE__, D_WARNING, FALSE, "%s\n", (x)); }

#ifdef MODULE_WARNING1
#undef MODULE_WARNING1
#endif

#define MODULE_WARNING1(x, y) \
	{ g_MAPIEDK.Assert("ASSERT", __FILE__, __LINE__, D_WARNING, FALSE, (x"\n"), (y)); }


#ifdef ASSERTERROR
#undef ASSERTERROR
#endif

#define ASSERTERROR(x, y) \
{ g_MAPIEDK.Assert("ASSERT", __FILE__, __LINE__, D_ERROR, (x), (y"\n")); }

#ifdef MODULE_ERROR
#undef MODULE_ERROR
#endif

#define MODULE_ERROR(x) \
{ g_MAPIEDK.Assert("ASSERT", __FILE__, __LINE__, D_ERROR, FALSE, "%s\n", (x)); }


#ifdef MAPIFreeBuffer
#undef MAPIFreeBuffer
#endif

#define MAPIFreeBuffer(x) g_MAPIEDK.pMAPIFreeBuffer(x)

#ifdef MAPIAllocateBuffer
#undef MAPIAllocateBuffer
#endif

#define MAPIAllocateBuffer(x, y) g_MAPIEDK.pMAPIAllocateBuffer(x, y)

#ifdef ASSERTERROR
#undef ASSERTERROR
#endif

#define ASSERTERROR(x, y) \
{g_MAPIEDK.Assert("ASSERT", __FILE__, __LINE__, D_ERROR, (x), (y"\n")); }

#ifdef ASSERTWARNING
#undef ASSERTWARNING
#endif

#define ASSERTWARNING(x, y) \
{ g_MAPIEDK.Assert("ASSERT", __FILE__, __LINE__, D_WARNING, (x), (y"\n")); }

#ifdef ULRELEASE
#undef ULRELEASE
#endif

#define ULRELEASE(x) { g_MAPIEDK.pUlRelease((x)); (x) = NULL; }

#ifdef ScDupPropset
#undef ScDupPropset
#endif

#define ScDupPropset(a, b, c, d) g_MAPIEDK.pScDupPropset(a, b, g_MAPIEDK.pMAPIAllocateBuffer, d)

#ifdef FREEPROWS
#undef FREEPROWS
#endif

#define FREEPROWS(x) { g_MAPIEDK.pFreeProws((x)); (x) = NULL; }

#ifdef HrQueryAllRows
#undef HrQueryAllRows
#endif

#define HrQueryAllRows(a, b, c, d, e, f) g_MAPIEDK.pHrQueryAllRows(a, b, c, d, e, f)

#ifdef CreateIProp
#undef CreateIProp
#endif

#define CreateIProp(a, b, c, d, e, f) g_MAPIEDK.pCreateIProp(a, g_MAPIEDK.pMAPIAllocateBuffer, g_MAPIEDK.pMAPIAllocateMore, g_MAPIEDK.pMAPIFreeBuffer, e, f)

#ifdef FPropExists
#undef FPropExists
#endif

#define FPropExists(x, y) g_MAPIEDK.pFPropExists(x, y)

#ifdef FREEPADRLIST
#undef FREEPADRLIST
#endif

#define FREEPADRLIST(x)  { g_MAPIEDK.pFreePadrlist((x)); (x) = NULL; }


#ifdef GlobalAlloc
#undef GlobalAlloc
#endif

#define GlobalAlloc(x, y) ::GlobalAlloc(x, y)

#ifdef GlobalFree
#undef GlobalFree
#endif

#define GlobalFree(x) ::GlobalFree(x)