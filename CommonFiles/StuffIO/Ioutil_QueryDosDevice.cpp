#include <windows.h>
#include <tchar.h>
#include "../stuff/pathstr.h"
#include "../stuff/pathstra.h"
#include "../stuff/pathstrw.h"
#include "../stuff/csystem.h"
#include "xerrors.h"
#include "ioutil.h"


// ---
DWORD IOSQueryDosDevice( LPCTSTR lpDeviceName, LPTSTR lpTargetPath, DWORD ucchMax ) {
	return ::QueryDosDevice( lpDeviceName, lpTargetPath, ucchMax );
}

