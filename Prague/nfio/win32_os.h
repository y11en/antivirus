// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  12 February 2003,  17:49 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_os.h
//  File input/output for Win32
// Additional info
//   File input/output for Win32



// AVP Prague stamp begin( OS, Interface constants )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __win32_os_h__68606d54_de6f_4349_a111_1f7d27a57c00 )
#define __win32_os_h__68606d54_de6f_4349_a111_1f7d27a57c00
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/plugin/p_win32_nfio.h>

#include "win32_objptr.h"
#include "win32_io.h"
#include <wincompat.h>

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define PID_WIN32_NFIO  ((tPID)(3))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// --- 213d8b50_e06c_11d3_bf0e_00d0b7161fb8 ---
// OS interface implementation
// Extended comment -
// Short comments -- object system interface
// Extended comment
//   Interface defines behavior of an object system. It responds for
//     - enumerate IO and Folder objects by creating Folder objects.
//     - create and delete IO and Folder objects
//   There are two possible ways to create or open IO object on OS. First is three steps protocol:
//     -- call ObjectCreate system method and get new IO (Folder) object
//     -- dictate properties of new object
//     -- call ObjectCreateDone system method and have working object
//   Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//   Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
tERROR pr_call OS_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure OS_Data is declared in O:\prague\nfio\win32_os.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
//! typedef struct tag_hi_OS {
//! 	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
//! 	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
//! } *hi_OS;
// AVP Prague stamp end



// ---
typedef WINBASEAPI BOOL   (WINAPI* tGetVolumeInformation)( const tVOID* lpRootPathName, PVOID lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, PVOID lpFileSystemNameBuffer, DWORD nFileSystemNameSize );
typedef WINBASEAPI HANDLE (WINAPI* tCreateFile          )( const tVOID* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
typedef WINBASEAPI BOOL   (WINAPI* tDeleteFile          )( const tVOID* lpFileName );
typedef WINBASEAPI DWORD  (WINAPI* tGetFileAttributes   )( const tVOID* lpFileName );
typedef WINBASEAPI DWORD  (WINAPI* tGetFileAttributesEx )( const tVOID* lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation );
typedef WINBASEAPI HANDLE (WINAPI* tFindFirstFile       )( const tVOID* lpFileName, PVOID lpFindFileData );
typedef WINBASEAPI BOOL   (WINAPI* tFindNextFile        )( HANDLE hFindFile, PVOID lpFindFileData );
typedef WINBASEAPI BOOL   (WINAPI* tFindClose           )( HANDLE hFindFile );
typedef WINBASEAPI BOOL   (WINAPI* tSetFileAttributes   )( const tVOID* lpFileName, DWORD dwFileAttributes );
typedef WINBASEAPI BOOL   (WINAPI* tCopyFile            )( const tVOID* lpExistingFileName, const tVOID* lpNewFileName, BOOL bFailIfExists );
typedef WINBASEAPI BOOL   (WINAPI* tMoveFile            )( const tVOID* lpExistingFileName, const tVOID* lpNewFileName );
typedef WINBASEAPI BOOL   (WINAPI* tMoveFileEx          )( const tVOID* lpExistingFileName, const tVOID* lpNewFileName, DWORD flags );
typedef WINBASEAPI BOOL   (WINAPI* tCreateDirectory     )( const tVOID* lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
typedef WINBASEAPI BOOL   (WINAPI* tRemoveDirectory     )( const tVOID* lpPathName );
typedef WINBASEAPI DWORD  (WINAPI* tGetTempPath         )( DWORD nBufferLength, const tVOID* lpBuffer );
typedef WINBASEAPI UINT   (WINAPI* tGetTempFileName     )( const tVOID* lpPathName, const tVOID* lpPrefixString, UINT uUnique, PVOID lpTempFileName);
typedef WINBASEAPI UINT   (WINAPI* tGetDriveType        )( LPCTSTR lpRootPathName );
typedef WINBASEAPI BOOL   (WINAPI* tGetFileSecurity     )(LPCTSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, DWORD nLength, LPDWORD lpnLengthNeeded);
typedef WINBASEAPI BOOL   (WINAPI* tSetFileSecurity     )(LPCTSTR lpFileName, SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor);
typedef WINBASEAPI DWORD  (WINAPI* tGetFullPathName     )( const tVOID* lpFileName, DWORD nBufferLength, tVOID* lpBuffer, const tVOID** lpFilePart );
typedef WINBASEAPI DWORD  (WINAPI* tGetLongPathName     )( const tVOID* lpszShortPath, tVOID* lpszLongPath,  DWORD cchBuffer );
typedef WINBASEAPI DWORD  (WINAPI* tGetShortPathName    )( const tVOID* lpszLongPath,  tVOID* lpszShortPath, DWORD cchBuffer );


#if defined(__cplusplus)
extern "C" {
#endif
  
  extern HMODULE               win_kernel;
  extern HMODULE               win_nt;
  extern tBOOL                 run_under_nt;
  extern tBOOL                 g_case_sensitive;
  extern tCODEPAGE             codepage;
  //extern tGetVolumeInformation fGetVolumeInformation;
  extern tCreateFile           fCreateFile;
  extern tDeleteFile           fDeleteFile;
  extern tGetFileAttributes    fGetFileAttributes;
  extern tGetFileAttributesEx  fGetFileAttributesEx;
  extern tFindFirstFile        fFindFirstFile;
  extern tFindNextFile         fFindNextFile;
  extern tFindClose            fFindClose;
  extern tSetFileAttributes    fSetFileAttributes;
  extern tCopyFile             fCopyFile;
  extern tMoveFile             fMoveFile;
  extern tMoveFileEx           fMoveFileEx;
  extern tCreateDirectory      fCreateDirectory;
  extern tRemoveDirectory      fRemoveDirectory;
  extern tGetDriveType         fGetDriveType;
  extern tGetFileSecurity      fGetFileSecurity;
  extern tSetFileSecurity      fSetFileSecurity;
	extern tGetFullPathName      fGetFullPathName;
	extern tGetLongPathName      fGetLongPathName;
	extern tGetShortPathName     fGetShortPathName;
  
  tSTRING       pr_call FindSeparator( tSTRING buff, tDWORD* size );
  tWSTRING      pr_call FindSeparatorW( tWSTRING buff, tDWORD* size );
  tERROR        pr_call _ExtractName( hOBJECT obj, tPTR* buffer, tDWORD len, tPROPID propid, tDWORD* out_len, tIID* iid_ptr );
  tERROR        pr_call _ConvertWin32Err( tDWORD win_err );
  const tWCHAR* pr_call get_prefix( tWSTRING buffer, tUINT size, tUINT* adv, tUINT* len );
  tUINT         pr_call calc_full_path_len( tWSTRING full, tWSTRING path, tUINT copy_len, tBOOL* need_prefix );
  tUINT         pr_call uni_len( tWSTRING str, tUINT size );
  tUINT         pr_call mb_len( tSTRING str, tUINT size );
  tBOOL         pr_call is_relative_path( tCHAR* buffer, tDWORD size );
  tERROR        pr_call is_folder( tCHAR* fname, tDWORD* win_err );
  tERROR        pr_call _ren_move( tPTR obj, tPTR existing, tPTR new_name, tBOOL overwrite, tDWORD* native_error );
	const tWCHAR* pr_call _find_w_prefix( const tWCHAR* buffer, tUINT size ); // returns symbol next to prefix if not zero
	const tWCHAR* pr_call _find_w_unc_prefix( const tWCHAR* buffer, tUINT size );  // returns symbol next to unc prefix if not zero
	const tWCHAR* pr_call _find_w_unc_path( const tWCHAR* buffer, tUINT size, tBOOL double_slash_passed );  // returns symbol next to unc path beginning ("\\server\share\some folder")
	const tCHAR*  pr_call _find_a_unc_path( const tCHAR*  buffer, tUINT size, tBOOL double_slash_passed );  // returns symbol next to unc path beginning ("\\server\share\some folder")
	                                                                                                        //                                                           ^
	#if defined(_WIN32) 
		tERROR pr_call _to_short_a( ObjPtrData* d, const tCHAR** ppath, tCHAR* dst );
		tERROR pr_call _to_short_w( ObjPtrData* d, const tWCHAR** ppath, tWCHAR* dst );
		tERROR pr_call _to_long_a( const tCHAR* src, tCHAR* dst, tUINT dst_len, tDWORD* out_len, tDWORD* win32err );
		tERROR pr_call _to_long_w( hOBJECT obj, const tWCHAR* src, tWCHAR** dst, tUINT* dst_len, tDWORD* out_len, tDWORD* win32err );
		tERROR pr_call _go_to_root( ObjPtrData* d );
	#else
		#define _to_short_a(o,p,d)       (errOK)
		#define _to_short_w(o,p,d)       (errOK)
		#define _to_long_a(s,d,l,ol,e)   (errOK)
		#define _to_long_w(o,s,d,l,ol,e) (errOK)
	#endif

	extern const tWCHAR  g_prefix[];
	extern const tUINT   g_prefix_size;
	extern const tUINT   g_prefix_count;
	extern const tWCHAR  g_prefix_unc[];
	extern const tUINT   g_prefix_unc_size;
	extern const tUINT   g_prefix_unc_count;
	
#if defined(__cplusplus)
}
#endif

/*
tERROR        pr_call is_exist( tCHAR* fname, tDWORD* win_err );
tBOOL         pr_call is_current_path( tCHAR* buffer, tDWORD size );
tBOOL         pr_call current_go( tCHAR* buffer, tDWORD* size );
tBOOL         pr_call relative_go_up( tCHAR* buffer, tDWORD* size );
tBOOL         pr_call can_go_up( tCHAR* path, tDWORD size );
tBOOL         pr_call go_up( tCHAR* path, tDWORD* size );
*/



#define UNICODE_MAX_PATH 32000
#define UNICODE_MAX_NAME (255 * sizeof(tWCHAR))
#define UNI(a)           ((tWCHAR*)(a))
#define MB(a)            ((tCHAR*)(a))
#define UNIS              sizeof(tWCHAR)
#define MBS               sizeof(tCHAR)


#if defined(_WIN32)
	#define IS_DELIMETER(symb)   (run_under_nt ? ((((tWCHAR)(symb))==PATH_DELIMETER_WIDECHAR) || (((tWCHAR)(symb))==L'/')) : ((((tCHAR)(symb))==PATH_DELIMETER_CHAR) || (((tWCHAR)(symb))=='/')))
	#define IS_W_DELIMETER(symb) ((((tWCHAR)(symb))==PATH_DELIMETER_WIDECHAR) || (((tWCHAR)(symb))==L'/'))
	#define IS_A_DELIMETER(symb) ((((tCHAR)(symb))==PATH_DELIMETER_CHAR) || (((tWCHAR)(symb))=='/'))
#else
	#define IS_DELIMETER(symb)   (((tCHAR)(symb))==PATH_DELIMETER_CHAR)
	#define IS_W_DELIMETER(symb) IS_DELIMETER(symb)
	#define IS_A_DELIMETER(symb) IS_DELIMETER(symb)
#endif
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// #define pgNATIVE_ERR mPROPERTY_MAKE_LOCAL( pTYPE_DWORD, 0x00000020 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // win32_os_h
// AVP Prague stamp end



