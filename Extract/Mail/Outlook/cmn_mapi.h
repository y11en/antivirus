//////////////////////////////////////////////////////////////////////////
// Header file for MAPI routine

#if !defined( CMN_MAPI_H )
#define CMN_MAPI_H

#include <Prague/prague.h>
#include <Prague/pr_oid.h>

#include <stdio.h>
#include <mapix.h>
#include <mapiutil.h>

//////////////////////////////////////////////////////////////////////////
// buffer methods

typedef struct tag_PRProp_Buff {
	tCHAR*			buffer;
	tDWORD			size;
	tTYPE_ID		type;
	tDWORD			out_size;
} PRProp_Buff;

EXTERN_C tERROR	PRBuffSetNULL(PRProp_Buff *p_buff);
EXTERN_C tERROR	PRBuffSetCHAR(PRProp_Buff *p_buff, tCHAR p_ch);
EXTERN_C tERROR	PRBuffSetSTRING(PRProp_Buff *p_buff, tSTRING p_val);
EXTERN_C tERROR	PRBuffSetDWORD(PRProp_Buff *p_buff, tDWORD p_val);
EXTERN_C tERROR	PRBuffSetQWORD(PRProp_Buff *p_buff, tQWORD p_val);
EXTERN_C tERROR	PRBuffSetDT(PRProp_Buff *p_buff, FILETIME *p_val);
EXTERN_C tERROR	PRBuffSetBINARY(PRProp_Buff *p_buff, tBYTE *p_val, tDWORD p_size);

#define PRBUFF_ROOT			".\\"
#define PRBUFF_DIRSLASH		'\\'
#define PRBUFF_SDATEFMT		"%4d/%02d/%02d %02d:%02d:%02d"
#define PRBUFF_SDATELEN		20

#define OBJMSG_FROM			"[From:"
#define OBJMSG_TO			"[To:"
#define OBJMSG_NONE			"None"
#define OBJMSG_SUBJECT		"][Subject:"
#define OBJMSG_TIME			"][Time:"

//////////////////////////////////////////////////////////////////////////
// common routine

EXTERN_C tERROR	ObjectGetSProp(IMAPIProp *p_obj, ULONG p_prop, LPSPropValue *p_entry);
EXTERN_C tERROR	ObjectGetProp(IMAPIProp *p_obj, ULONG p_prop, tTYPE_ID p_type, PRProp_Buff *p_buff);
EXTERN_C tERROR	ObjectSetProp(IMAPIProp *p_obj, ULONG p_prop, tTYPE_ID p_type, PRProp_Buff *p_buff);
EXTERN_C tDWORD	ObjectGetPropDWORD(IMAPIProp *p_obj, ULONG p_prop);
EXTERN_C tERROR	ObjectSetPropDWORD(IMAPIProp *p_obj, ULONG p_prop, tDWORD p_value);

EXTERN_C tERROR	MessageGetName(IMAPIProp *p_obj, PRProp_Buff *p_buff);

//////////////////////////////////////////////////////////////////////////

#ifdef CHECK_REFERENCE

#define MAPI_IFACE_CREATE(obj, iface) \
	if( obj ){ g_c_##iface++; }

#define MAPI_IFACE_ADDREF(obj, iface) \
	{ (obj)->AddRef(); g_c_##iface++; }

#define MAPI_IFACE_COPY(dst, src, iface) \
	if( dst != src ) { if(src){ (src)->AddRef(); g_c_##iface++; } \
		if(dst){ (dst)->Release(); g_c_##iface--; } dst = src; }

#define MAPI_IFACE_RELEASE(obj, iface) \
	if( obj ) {(obj)->Release(); obj = NULL; g_c_##iface--; }

#else

#define MAPI_IFACE_CREATE(obj, iface)
#define MAPI_IFACE_ADDREF(obj, iface) (obj)->AddRef();
#define MAPI_IFACE_COPY(dst, src, iface) \
	if( dst != src ) { if(src)(src)->AddRef(); if(dst)(dst)->Release(); dst = src; }
#define MAPI_IFACE_RELEASE(obj, iface) \
	if( obj ) {__try{ (obj)->Release(); } __except(1){} obj = NULL; }

#endif

#define MAPI_BUFFER_FREE(obj) \
	if( obj ) { MAPIFreeBuffer(obj); obj = NULL; }

#define MAPI_BUFFER_COPY(dst, src) \
	if( dst != src ) { MAPI_BUFFER_FREE(dst) if( src ){ \
		size_t sz = strlen(src)+1; MAPIAllocateBuffer(sz, (LPVOID*)&dst); if( dst ) strcpy_s(dst, sz, src); } }

#undef PR_TRACE_A0
#undef PR_TRACE_A1
#undef PR_TRACE_A2

#define PR_TRACE_A0(t,s)     __try {
#define PR_TRACE_A1(t,s,e)   }__except(1) { error = errUNEXPECTED; PR_TRACE((g_root, prtFATAL, "MDB\t !!!EXCEPTION!!!: %s", s)); }
#define PR_TRACE_A2(t,s,p,e) }__except(1) { error = errUNEXPECTED; PR_TRACE((g_root, prtFATAL, "MDB\t !!!EXCEPTION!!!: %s", s)); }

//////////////////////////////////////////////////////////////////////////

#endif // CMN_MAPI_H
