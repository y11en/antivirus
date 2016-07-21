#ifndef __LLIO_H_8812312312
#define __LLIO_H_8812312312

#include <windows.h>
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Extract/plugin/p_windiskio.h>

typedef struct tag_LLDISKIO
{
	hIO m_hIo;
} LLDISKIO;

LLDISKIO* LLDiskIOInit(LPCWSTR pDiskName);
void LLDiskIODone(LLDISKIO* pLLDiskIO);
BOOL LLDiskIORead(LLDISKIO* pLLDiskIO, unsigned __int64 Offset, LPVOID pBuffer, DWORD dwBytesToRead, DWORD* pdwBytesRead);

#endif // __LLIO_H_8812312312
