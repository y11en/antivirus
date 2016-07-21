#include <windows.h>
#include "signchk.h"

// -----------------------------------------------------------------------------------------
void* m_malloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

void m_free(void* pVoid)
{
	HeapFree(GetProcessHeap(), 0, pVoid);
}

static int sign_check_cb(HANDLE hFile, void* buffer, int buff_len )
{
	DWORD dw = 0;
	if (ReadFile(hFile, buffer, buff_len, &dw, NULL) == FALSE)
		return -1;

	return dw;
}

#if !defined(_DEBUG)
//int _sign_check_file(TCHAR* file)
//{
//	int sign_check_res;
//	HANDLE hFile;
//	BYTE buff[0x400];
//	hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE)
//		return SIGN_FILE_NOT_FOUND;
//
//	sign_init_mem(m_malloc, m_free);
//	sign_check_res = sign_check_buffer_callback((sign_check_call_back) sign_check_cb, hFile, buff, sizeof(buff), NULL, 1, NULL, 0, 0);
//	CloseHandle(hFile);
//
//	return sign_check_res;
//}
#endif