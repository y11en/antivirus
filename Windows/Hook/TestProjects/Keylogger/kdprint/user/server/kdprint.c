#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "kdprint.h"
#include "..\..\kdshared.h"
#include "..\..\kdctl.h"

static BOOL g_bKDPrint_Inited = FALSE;

static HANDLE g_hKDPrint_Mutex = NULL;
static PKDPRINT_SECTION g_pKDPrint_Section = NULL;

BOOL KDPrintInit()
{	
	HANDLE hMutex;
	HANDLE hFileMapping;
	PKDPRINT_SECTION pSection;

	if (g_bKDPrint_Inited)
		return TRUE;

// объекты открываем просто по имени
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, U_MUTEX_NAME);
	if (!hMutex)
		return FALSE;

	hFileMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, U_FILEMAPPING_NAME);
	if (!hFileMapping)
	{
		CloseHandle(hMutex);
		return FALSE;
	}
	
	pSection = MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(KDPRINT_SECTION));
	if (!pSection)
	{
		CloseHandle(hFileMapping);
		CloseHandle(hMutex);
		return FALSE;
	}

	CloseHandle(hFileMapping);

	g_hKDPrint_Mutex = hMutex;
	g_pKDPrint_Section = pSection;

	g_bKDPrint_Inited = TRUE;

	return TRUE;
}

void KDPrintUnInit()
{
	if (!g_bKDPrint_Inited)
		return;
	
	g_bKDPrint_Inited = FALSE;

	UnmapViewOfFile(g_pKDPrint_Section);
	CloseHandle(g_hKDPrint_Mutex);
}

void _KDPrint(IN PCHAR Str2Output)
{
	ULONG WaitResult;
	ULONG cbStr2Output;
	ULONG Count;
	
	if (!g_bKDPrint_Inited)
		return;

// ждем мьютекса
	WaitResult = WaitForSingleObject(g_hKDPrint_Mutex, 2000);
	if (WaitResult != WAIT_OBJECT_0)
	{
		CHAR Str[64];

		sprintf(Str, "_KDPrint: failed to acquire mutex, timeouted, PID:%d\n", GetCurrentProcessId());
		OutputDebugString(Str);

		return;
	}
	cbStr2Output = strlen(Str2Output)+1;

// ждем освобождения буфера
	Count = 0;
	while (g_pKDPrint_Section->m_BufferTop+cbStr2Output > KD_BUFFER_LEN && Count < 10)
	{
// даем клиенту забрать данные
		ReleaseMutex(g_hKDPrint_Mutex);

		Sleep(20);
		
		WaitResult = WaitForSingleObject(g_hKDPrint_Mutex, 200);
		if (WaitResult != WAIT_OBJECT_0)
		{
			CHAR Str[64];

			sprintf(Str, "_KDPrint: failed to acquire mutex, timeouted, PID:%d\n", GetCurrentProcessId());
			OutputDebugString(Str);

			return;
		}

		Count++;
	}
	if (Count >= 10)
	{
		ReleaseMutex(g_hKDPrint_Mutex);
		return;
	}

	strcpy(&g_pKDPrint_Section->m_Buffer[g_pKDPrint_Section->m_BufferTop], Str2Output);
	g_pKDPrint_Section->m_BufferTop += cbStr2Output;
	
	ReleaseMutex(g_hKDPrint_Mutex);
}

void KDPrint(PCHAR FormatStr, ...)
{
	PCHAR OutputStr;

	OutputStr = (PCHAR)HeapAlloc(GetProcessHeap(), 0, KD_MAX_STR_SIZE);
	if (OutputStr)
	{
		va_list Mark;

		va_start(Mark, FormatStr);
		_vsnprintf(OutputStr, KD_MAX_STR_SIZE, FormatStr, Mark);
		va_end(Mark);

		_KDPrint(OutputStr);

		HeapFree(GetProcessHeap(), 0, OutputStr);
	}
}
