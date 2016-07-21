#if defined (_WIN32)

#include "llio.h"

#ifndef countof
#define countof(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

#define _llio_alloc(size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
#define _llio_free(ptr) HeapFree(GetProcessHeap(), 0, ptr);

extern hROOT g_root;
LLDISKIO* LLDiskIOInit(LPCWSTR pDiskName)
{
	tERROR error;
	PWSTR pwstrName = NULL;
	hIO hIo = NULL;

	if (!pDiskName)
		return NULL;

/*	if(pDiskName[0] != L'C' && pDiskName[0] != L'c' || pDiskName[1])
		pwstrName = (PWSTR) pDiskName;
	else
		pwstrName = L"\\Device\\HarddiskVolume1";*/
	pwstrName = (PWSTR) pDiskName;

	error = CALL_SYS_ObjectCreate( (hOBJECT) g_root, &hIo, IID_IO, PID_WINDISKIO, 0 );
	if (PR_SUCC(error))
	{
		error = CALL_SYS_PropertySetStr( hIo, NULL, pgOBJECT_NAME, pwstrName, 0, cCP_UNICODE );
		if (PR_SUCC( error ))
			error = CALL_SYS_ObjectCreateDone( hIo );
		
		if (PR_SUCC( error ))
		{
			LLDISKIO* pLLDiskIO = (LLDISKIO*)_llio_alloc(sizeof(LLDISKIO));
			if (pLLDiskIO)
			{
				pLLDiskIO->m_hIo = hIo;
				return pLLDiskIO;
			}
		}

		CALL_SYS_ObjectClose( hIo );
	}

	return NULL;
}

void LLDiskIODone(LLDISKIO* pLLDiskIO)
{
	if (pLLDiskIO)
	{
		CALL_SYS_ObjectClose( pLLDiskIO->m_hIo );
		_llio_free(pLLDiskIO);
	}
}

BOOL LLDiskIORead(LLDISKIO* pLLDiskIO, unsigned __int64 Offset, LPVOID pBuffer, DWORD dwBytesToRead, DWORD* pdwBytesRead)
{
	tERROR error;
	if (!pLLDiskIO)
		return FALSE;

	error = CALL_IO_SeekRead( pLLDiskIO->m_hIo, pdwBytesRead, Offset, pBuffer, dwBytesToRead );
	if (PR_SUCC(error))
		return TRUE;
	
	return FALSE;
}

#endif //_WIN32
