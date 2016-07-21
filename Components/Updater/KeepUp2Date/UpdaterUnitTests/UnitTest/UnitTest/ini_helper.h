#ifndef INI_HELPER_H__INCLUDED
#define INI_HELPER_H__INCLUDED

#include "include/unicode_common.h"

#define UPDATER_INI_FILENAME    _T("ss_storage.ini")
#define PRODINFO_SECTION        _T("ProductInfo")
#define UPDATER_SECTION         _T("Updater")
#define UPDATER_LISTS_FILENEME  _T("rollback.ini")
#define UPDATER_RESULT_FILENAME  ("iupdater.txt")

#define PAR_APPLICATION_ID  "ApplicationID"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <shlwapi.h>
#include <time.h>

#include <include/requests_ids.h>

#include <helper/stdinc.h>


#include "..\..\..\Client\ini_libs\ini_common.h"
#include "ini_updater.h"


bool getConfigurationFileName(STRING &, const bool useCurrentFolder, Log *);
bool getProductFolder(const bool useCurrentFolder, STRING &, Log *);
TCHAR *GetFirstItemFromString(const TCHAR *pszListString, TCHAR *pszItemBuf, int nBufSize);


#endif // INI_HELPER_H__INCLUDED