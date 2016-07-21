#pragma hdrstop
#include "tchar.h"
#include "windows.h"
#include "stdio.h"

static WCHAR s_strModuleName [1024] = {0};
		    
inline void MyGetModuleFileName()
{
	if ( s_strModuleName[0] || 
		!GetModuleFileNameW(NULL, s_strModuleName, sizeof(s_strModuleName))) 
		return;

	for(int i = lstrlenW(s_strModuleName); i>=0 ; i--) 
	{
		if(L'.'  == s_strModuleName[i])
			s_strModuleName[i] = 0;

		if(L'\\' == s_strModuleName[i]) 
		{
			lstrcpyW(s_strModuleName, s_strModuleName + i + 1);
			break;
		}
	}
}

void KLabDbgTraceA(const char *pszFileName, int nLine, BOOL bAssert, const CHAR *pszFormat, ...)
{
	MyGetModuleFileName();

	const int nCount = 1024;
	CHAR szBuf[nCount] = {'\0'};
	int nLen = 0;

	nLen += _snprintf(szBuf + nLen, nCount - nLen, "%S:%s:%d(%x) %s ", s_strModuleName, pszFileName, nLine, 
		GetCurrentThreadId(), bAssert ? "-assert failed-" : "");

	va_list ptr;						  
	va_start(ptr, pszFormat);
	nLen += _vsnprintf(szBuf + nLen, nCount - nLen, pszFormat, ptr);
	va_end(ptr);

	lstrcpynA(szBuf + nLen, "\n", min(2, nCount - nLen));

	OutputDebugStringA(szBuf);
}

void KLabDbgTraceW(const char *pszFileName, int nLine, BOOL bAssert, const WCHAR *pszFormat, ...)
{
	MyGetModuleFileName();

	const int nCount = 1024;
	WCHAR szBuf[nCount] = {'\0'};
	int nLen = 0;

	nLen += _snwprintf(szBuf + nLen, nCount - nLen, L"%s:%S:%d(%x) %s ", s_strModuleName, pszFileName, nLine, 
		GetCurrentThreadId(), bAssert ? L"-assert failed-" : L"");

	va_list ptr; 
	va_start(ptr, pszFormat);
	nLen += _vsnwprintf(szBuf + nLen, nCount - nLen, pszFormat, ptr);
	va_end(ptr);

	lstrcpynW(szBuf + nLen, L"\n", min(2, nCount - nLen));

	OutputDebugStringW(szBuf);
}
