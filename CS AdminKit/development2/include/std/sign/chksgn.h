#ifndef __KLSTDI_H__
#define __KLSTDI_H__

#include "std/base/kldefs.h"

/*!
  \brief	Checks file signature

  \params   szwFileName - имя файла
*/
KLCSC_DECL bool KLSTD_CheckFileSignature(const wchar_t* szwFileName, bool bThrowIfMismatch = true);

/*!
  \brief	Check module signature by known commandline

  \params   szwFileName - commandline
*/
KLCSC_DECL bool KLSTD_CheckModuleSignature(const wchar_t* szwCmdLine, bool bThrowIfMismatch = true);


#ifdef KLSTD_WINNT
    /*!
      \brief	Проверка подписи исполняемого модуля службы Windows NT
      \params   szwFileName - имя службы Windows NT
    */
    KLCSC_DECL bool KLSTD_CheckServiceSignature(const wchar_t* szwServiceNameName, bool bThrowIfMismatch = true);
#endif
    
KLCSC_DECL void KLSTD_CheckDllBeforeLoad(const wchar_t* szwDll);


/*!
  \brief	Checks already loaded modules

  \param	szwModules  pointer to array of module names, NULL or empty 
                        string means "main excutable module"
  \param	size_t      number of elements in array


  Sample code:
  <PRE>
        const wchar_t* c_arrModules[]= {L"", L"klcsstd.dll", L"klcskca.dll"};
        KLSTD_CheckLoadedModules(c_arrModules, KLSTD_COUNTOF(c_arrModules));
  </PRE>

*/
KLCSC_DECL void KLSTD_CheckLoadedModules(
                    const wchar_t * const * szwModules, 
                    size_t                  nModules);


KLCSC_DECL void KLSTD_SetShutdownFlag();

KLCSC_DECL bool KLSTD_GetShutdownFlag();

KLCSC_DECL void KLSTD_ClearShutdownFlag();

KLCSC_DECL void* KLSTD_GetModuleInstance();    

KLCSC_DECL void KLSTD_ParseCmdlineA(
					    const char* cmdstart,
					    char**      argv,
					    char*       args,
					    int*        numargs,
					    int*        numchars);

KLCSC_DECL void KLSTD_ParseCmdlineW(
					    const wchar_t*  cmdstart,
					    wchar_t**       argv,
					    wchar_t*        args,
					    int*            numargs,
					    int*            numchars);

#if defined(_UNICODE) || defined(UNICODE)
    #define KLSTD_ParseCmdline KLSTD_ParseCmdlineW
#else
    #define KLSTD_ParseCmdline KLSTD_ParseCmdlineW
#endif

#endif //__KLSTDI_H__
