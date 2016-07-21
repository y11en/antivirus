#ifndef __e_clsid_h
#define __e_clsid_h

#define ECLSID_KERNEL  0x00000001L
#define ECLSID_STRING  0x00000002L
#define ECLSID_LOADER  0x00000003L
#define ECLSID_L_LLIO  0x00000004L

#endif

#ifdef PR_EXPORT_LIB
#	undef EXPORT_FUNC_NAME
#	define EXPORT_FUNC_NAME(name) pr_call name
#else
#	undef EXPORT_FUNC_NAME
#	define EXPORT_FUNC_NAME(name) (pr_call* name)
#endif


#if defined(EXTERN_TABLE_CONTENT) || defined(PR_EXPORT_LIB)
#pragma message("EXTERN_TABLE_CONTENT macro should be replaced with IMPEX_TABLE_CONTENT")
#pragma message("PR_EXPORT_LIB macro should be replaced with IMPEX_EXPORT_LIB")
#pragma message("EXPORT_FUNC_NAME macro should be replaced with IMPEX_NAME or IMPEX_NAME_ID")
#pragma message("See details in <IFACE\\IMPEXHLP.H>")
#error  Compilation impossible :(
#endif

