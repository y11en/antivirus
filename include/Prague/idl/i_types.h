#ifndef idl_types_h
#define idl_types_h

#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_idl.h>
#include <Prague/pr_types.h>

#define IDLN(n) __##n
#define PR_IDL_LEGACY

typedef tIID   InterfaceId;
typedef tIID   PluginId;
typedef tIID   VendorId;
typedef tIID   ImplementationId;
typedef tIID   MessageId;
typedef tIID   MessageClassId;
typedef tIID   PropertyId;
typedef tCHAR* tSTRING;
//static const tDWORD cTRUE = 1;
//static const tDWORD cFALSE = 0;

struct IDLN(cRegistry);
#if defined(PR_IDL_TYPEDEF)
	typedef IDLN(cRegistry)* hREGISTRY;
#endif

struct IDLN(cReport);
#if defined(PR_IDL_TYPEDEF)
	typedef IDLN(cReport)* hREPORT;
#endif

struct IDLN(cString);
#if defined(PR_IDL_TYPEDEF)
	typedef IDLN(cString)* hSTRING;
#endif

#endif //idl_types_h

