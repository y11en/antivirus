#ifndef __custview_h
#define __custview_h


#include <prague.h>
#include <windows.h>
#include "../../kernel/kernel.h"


#ifdef __cplusplus
 extern "C" {
#endif
	

typedef struct tag_DEBUGHELPER {
	WORD wVersionLo;
	WORD wVersionHi;
	HRESULT (__stdcall* ReadDebuggeeMemory)( struct tag_DEBUGHELPER* helper, DWORD dwRemoteAddress, DWORD dwSize, PVOID pBuffer, DWORD* dwDebuggerError );
} DEBUGHELPER;


typedef struct tag_Name {
	DWORD id;
	char*	name;
} tName;



typedef struct tag_debOBJECT {
	const tVOID*       guard;        // windows will save some free list pointer here (on deallocate)
	struct sHANDLE*    saved_handle; // i will save back pointer to handle here (on deallocate)
  struct sHANDLE*    handle; // back pointer to handle
  const tVOID*       vtbl;   // interface defined function table - "Default VTBL" for generic object
  const iSYSTEMVtbl* sys;    // system defined function table
  const tVOID*       data;   //data;   // internal object data
} tDebObj;


typedef struct tag_relOBJECT {
  struct sHANDLE*    handle; // back pointer to handle
  const tVOID*       vtbl;   // interface defined function table - "Default VTBL" for generic object
  const iSYSTEMVtbl* sys;    // system defined function table
  const tVOID*       data;   //data;   // internal object data
} tRelObj;



extern tName iids[];
extern tUINT iid_count;
	
extern tName pids[];
extern tUINT pid_count;
	
tName* find( tName* arr, tUINT count, DWORD id );
char* find_name( tName* arr, tUINT count, DWORD id, char* def );

extern char src[0x4000];
extern char local_buff[0x4000];

typedef HRESULT (WINAPI* fnCustomViewer)(
	DWORD dwAddress,       // low 32-bits of address
	DEBUGHELPER *pHelper,  // callback pointer to access helper functions
	int nBase,             // decimal or hex
	BOOL bIgnore,          // not used
	char *pResult,         // where the result needs to go
	size_t max,            // how large the above buffer is
	DWORD dwReserved       // always pass zero
);


HRESULT WINAPI hr_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI prop_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI err_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI iid_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI pid_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI cp_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI dt_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI local_time_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI gmt_time_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );

HRESULT WINAPI str_range_format_val( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI phOBJECT( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI phString( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI phandle( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI phNFIO( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI serializableId( DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI serializable( DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI phHeap( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );
HRESULT WINAPI dt_format( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );


// ---
// old object
typedef struct sk_OBJECT {
  const tPTR          vtbl;   // interface defined function table - "Default VTBL" for generic object
  const iSYSTEMVtbl*  sys;    // system defined function table
  const tPTR          data;   // internal object data
} k_old_OBJECT;


#if defined(__i_root__37b137ba_ae7b_11d4_b757_00d0b7170e50)

// ---
//typedef struct s_old_HANDLE {
//  
//  tIID                iid;              // 00(00);  interface id for this handle
//  
//  // handle hierarchy related data              
//  struct s_old_HANDLE* parent;           // 01(04)  -112 parent handle
//  struct s_old_HANDLE* next;             // 02(08)  -108 next handle at the same level
//  struct s_old_HANDLE* prev;             // 03(12)  -104 prev handle at the same level
//  struct s_old_HANDLE* child;            // 04(16)  -100 first child
//  
//  tDWORD               flags;            // 05(20)  -96 some flags
//  tDWORD               init_props;       // 06(24)  -92 initialization property marks
//  tDWORD               memsize;          // 07(28)  -88 size of allocated memory
//  tINTERFACE*          iface;            // 08(32)  -84 pointer to interface decription
//  tCODEPAGE            cp;               // 09(36)  -80 objects code page, if there is no pgOBJECT_CODEPAGE property defined for the object
//  //tERROR              error;            // 09(36)  -80 last object error
//  tCustProp*           custom_prop;      // 10(40)  -76 custom (extermal) properties
//  tDWORD               lock_count;       // 11(44)  -72 memory lock count
//  tDWORD               wrappers;         // 12(48)  -68 wrapper list
//  tDWORD               wrap_data;        // 13(52)  -64 data passed to wrapper method
//  tDWORD               wrap_data2;       // 14(56)  -60 data passed to wrapper method 
//  hTRACER              tracer;           // 15(60)  -56 tracer handle if tracing turned on
//  tTRACE_LEVEL         min_trace_level;  // 16(64)  -52 minimum trace level for this handle
//  tTRACE_LEVEL         max_trace_level;  // 17(68)  -48 maximum trace level for this handle
//  tDWORD               find_data1;       // 18(72)  -44
//  tDWORD               find_data2;       // 19(76)  -40
//  tMHArray             mh_arr;           // 20(80)  -36 message handler array
//  tMHCacheArray        mh_c_arr;         // 22(88)  -28 cached message handlers
//  struct s_old_HANDLE* static_link;      // 24(96)  -20 real static object for faked substitution
//  hOBJECT              sync;             // 25(100) -16
//  tDWORD               user_data;        // 26(104) -12
//  hHEAP                mem;              // 27(108) -08 memory heap linked to the object
//  tDWORD               custom_prop_count;// 28(112) -04 custom (extermal) properties count
//  k_old_OBJECT         obj;              // 29(116)  00 interface defined part of handle
//  /*
//  const tPTR          vtbl;       // 29(116); interface defined function table - "Default VTBL" for generic object
//  const iSYSTEMVtbl*  sys;        // 30(120); system defined function table
//  const tPTR          data;       // 31(124); internal object data
//  */
//} t_old_HANDLE;
#endif

#ifdef __cplusplus
	}
#endif
	

#endif