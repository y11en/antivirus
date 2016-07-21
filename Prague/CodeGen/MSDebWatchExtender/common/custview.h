#ifndef __custview_h
#define __custview_h


#include <windows.h>
#include "evalinfo.h"

typedef struct tag_Name {
  DWORD id;
  char*	name;
} tName;

extern tName iids[];
extern UINT  iid_count;

extern tName pids[];
extern UINT  pid_count;

extern char  src_buff[0x1000];
extern char  local_buff[0x1000];

tName* find( tName* arr, UINT count, DWORD id );
char* find_name( tName* arr, UINT count, DWORD id, char* def );


#ifndef countof
#define countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#ifdef __cplusplus
 extern "C" {
#endif
	

typedef struct tag_DEBUGHELPER {
	WORD wVersionLo;
	WORD wVersionHi;
	HRESULT (__stdcall* ReadDebuggeeMemory)( struct tag_DEBUGHELPER* helper, DWORD dwRemoteAddress, DWORD dwSize, PVOID pBuffer, DWORD* dwDebuggerError );
} DEBUGHELPER;


typedef HRESULT (WINAPI* fnCustomViewer)(
	DWORD dwAddress,       // low 32-bits of address
	DEBUGHELPER *pHelper,  // callback pointer to access helper functions
	int nBase,             // decimal or hex
	BOOL bIgnore,          // not used
	char *pResult,         // where the result needs to go
	size_t max,            // how large the above buffer is
	EVAL_INFO* pEvalInfo   // DWORD dwReserved       // always pass zero
);


#ifdef __cplusplus
	}
#endif
	

#endif