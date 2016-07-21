#include "custview.h"
#include <iface/i_os.h>

typedef union tag_QWord {
  tLONGLONG v;
  struct {
    tDWORD l;
    tLONG  h;
  };
} tQWord;



// ---
typedef struct tag_IOData {
  k_old_OBJECT obj;
  tCHAR*   full_name;          // full name buffer
  tDWORD   full_name_buff_len; // length of the buffer
  tSTRING  ppath;              // pointer to the full path
  tSTRING  pname;              // pointer to the file name
  HANDLE   handle;             // windows file handle
  tQWord   cp;                 // current file position
  tDWORD   open_mode;          // open mode
  tDWORD   access_mode;        // file attributes
  tQWord   m_size;             // file size
  FILETIME open_time;          // open modification time and date
  tDWORD   open_attr;          // open time file attributes
  tBOOL    del_on_close;       // must be deleted on close
  hOS      os;                 // owner os
  tDWORD   win32err;           // last win32 error
  tDWORD   origin;             // object origin
  tBOOL    resore_attribs;     // file attributes has been changed on InitDone and need to be resore
} IOData;


// ---
HRESULT WINAPI phNFIO( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {
	IOData  nfio;
  HRESULT hr;
  DWORD   dwErr;
  const char* fmt;
  const char* acc;
	tBOOL ansi_set = cFALSE;

  __try {
    if ( ((DWORD)address) == 0 ) {
      lstrcpyn( pOutputStr, "null pointer", dwMaxSize );
      return S_OK;
    }
    if ( ((DWORD)address) == 0xccccccccl ) {
      lstrcpyn( pOutputStr, "not initialized", dwMaxSize );
      return S_OK;
    }
  
		hr = pHelper->ReadDebuggeeMemory( pHelper, address, sizeof(IOData), &nfio, &dwErr );
    if ( FAILED(hr) ) {
      lstrcpyn( pOutputStr, "??? cannot read debuggee memory", dwMaxSize );
      return S_OK;
    }


		if ( !nfio.full_name || !nfio.full_name_buff_len ) {
			ansi_set = cTRUE;
			lstrcpy( local_buff, "file not set" );
		}
		else
			hr = pHelper->ReadDebuggeeMemory( pHelper, (DWORD)nfio.full_name, nfio.full_name_buff_len, local_buff,  &dwErr );
    if ( FAILED(hr) ) {
			ansi_set = cTRUE;
			lstrcpy( local_buff, "file ???" );
		}

    switch( nfio.access_mode & fACCESS_RW ) {
      case fACCESS_RW : acc = "rw"; break;
      case fACCESS_READ : acc = "r"; break;
      case fACCESS_WRITE : acc = "w"; break;
      default:   acc = "??"; break;
    }

    if ( ansi_set || (0x80000000l & GetVersion()) ) // 98
      fmt = "NFIO(%s,\"%s\")";
    else
      fmt = "NFIO(%s,\"%S\")";
    wsprintf( src, fmt, acc, local_buff );
    lstrcpyn( pOutputStr, src, dwMaxSize );
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
    lstrcpyn( pOutputStr, "cannot parse address", dwMaxSize );
  }
  return S_OK;
}


