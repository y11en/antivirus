//#include "Structures.h"
#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <Winspool.h>
#include <Winsock2.h>
#include "SubDefines.h"

enum SI_INSIDE_OS_NUMBER
{
	OS_WIN32=0, //VER_PLATFORM_WIN32s
	OS_WIN_UNKNOWN, //VER_PLATFORM_WIN32_WINDOWS unknown minor version
	OS_WIN95,
	OS_WIN98,
	OS_WINME,
	RESERVED5,
	RESERVED6,
	RESERVED7,
	RESERVED8,
	RESERVED9,
	OS_UNKNOWN, //unknown dwPlatformId or error
	OS_WINNT351,
	OS_WINNT40,
	OS_WIN2000,
	OS_WINXP,
	OS_WINXPNET,
};

//CPU vendors number
enum e_CPUVENDORS 
{ 
	VENDOR_UNKNOWN = 0, 
	VENDOR_INTEL, 
	VENDOR_AMD, 
	VENDOR_CYRIX, 
	VENDOR_CENTAUR, 
};

////CPU vendors strings
#define VENDOR_INTEL_STR		"GenuineIntel"
#define VENDOR_AMD_STR			"AuthenticAMD"
#define VENDOR_CYRIX_STR		"CyrixInstead"
#define VENDOR_CENTAUR_STR		"CentaurHauls"

#define BRANDTABLESIZE			4
#define NAMESTRING_FEATURE		0x80000004     // this is the namestring feature; goes from 0x80000002 to 0x80000004
#define MMX_FLAG				0x00800000     // [Bit 23] MMX supported                               

#define INFO_TYPE_COUNT					24
#define PROCESSOR_STEP_COUNT			8
#define HARD_NT_STEP_COUNT				8
#define HARD_NT_STEP2_STEP_COUNT		4
#define HARD_9X_STEP_COUNT				300515
#define FREEDISK_STEP_COUNT				78

typedef struct _PPTI4{
	PRINTER_INFO_4	Ppti4[1];
}PPTI4, *pPPTI4;

typedef struct _PPTI2{
	PRINTER_INFO_2	Ppti2[1];
}PPTI2, *pPPTI2;

typedef struct _PROTOCOL_INFOW_ARRAY{
	WSAPROTOCOL_INFOW	lpBuf[1];
}PROTOCOL_INFOW_ARRAY, *pPROTOCOL_INFOW_ARRAY;

typedef LANGID (WINAPI *pfnGetUserDefaultUILanguageType)(void);
typedef BOOL (WINAPI *pfnGetDiskFreeSpaceExType)(LPCTSTR lpDirectoryName,PULARGE_INTEGER lpFreeBytesAvailable,PULARGE_INTEGER lpTotalNumberOfBytes,PULARGE_INTEGER lpTotalNumberOfFreeBytes);
////typedef BOOL (WINAPI *pfnEnumPrintersPtr)(DWORD Flags,LPTSTR Name,DWORD Level,LPBYTE pPrinterEnum,DWORD cbBuf,LPDWORD pcbNeeded,LPDWORD pcReturned);
////typedef BOOL (WINAPI *pfnGetDefaultPrinterPtr)(LPTSTR pszBuffer,LPDWORD pcchBuffer);
typedef int (WINAPI *WSCEnumProtocolsPtr)(LPINT lpiProtocols,LPWSAPROTOCOL_INFOW lpProtocolBuffer,LPDWORD lpdwBufferLength,LPINT lpErrno);
typedef BOOL (WINAPI *GetCPInfoExPtr)(UINT CodePage,DWORD dwFlags,LPCPINFOEX lpCPInfoEx);

#endif //_DEFINES_H_
