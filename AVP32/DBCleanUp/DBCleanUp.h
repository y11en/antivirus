/**************************************************************************** 
  FILE..........: DBCleanUp.h
  COPYRIGHT.....: Copyright (c) 2000-01 F-Secure Ltd. 
  VERSION.......: 1.00 
  DESCRIPTION...: Declarations for Clean UP
  NOTE..........: Must be compliled for 
				  Win32 
				  WinNT Kernel
				  Win95 Vxd
				  

  HISTORY.......: DATE     COMMENT 
                  -------- ------------------------------------------------ 
                  2001-04-11 Created. 
****************************************************************************/

#ifndef __DBCLEANUP_H_
#define __DBCLEANUP_H_



#if defined( VTOOLSD )        // Windows 95 (vxd)
	#pragma message("VXD")
	
	#define AVP_CLEANUP_DB_UNDER_VXD
	#define CALL_CONV __stdcall

#elif defined( _WIN32_WINNT ) //WinNT Kernel
	#pragma message("driver")
	#define AVP_CLEANUP_DB_UNDER_WINNTKERNEL
	#define CALL_CONV __stdcall

#elif defined( _USRDLL )     // Windows 32 (FPIAPI DLL)
	#pragma message("FPI API")
	#define AVP_CLEANUP_DB_UNDER_DLL
	#define CALL_CONV __stdcall

#else
	#error "unknown platform"
#endif


enum AVP_CLEANUP_DB_RETURN_VALUE
{
	AVP_CLEANUP_DB_OK = 1,
	AVP_CLEANUP_DB_FAILURE ,
	AVP_CLEANUP_DB_INVALID_PARAMETER ,
	AVP_CLEANUP_DB_INTERNAL_ERROR
};


#define avc_mask				 "*.*"
#define database_extension_list  "avc"

AVP_CLEANUP_DB_RETURN_VALUE CALL_CONV 
	AVPCleanUpUnUsedFiles( const char *strAVPSetFile );



#endif //__DBCLEANUP_H_