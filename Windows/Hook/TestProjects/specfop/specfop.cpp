// specfop.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "../../Hook/IDriver.h"

#ifdef _UNICODE
	#define _fn_1	_T("\\??\\d:\\reparse\\рус123")
	#define _fn_2	_T("\\??\\d:\\reparse\\123")
#pragma message ("_________ unicode _________")
#else
	#define _fn_1	_T("c:\\рус123")
	#define _fn_2	_T("c:\\123")
#endif

int main(int argc, char* argv[])
{
	BOOL bRet;

	ULONG BufferSize = 0;
	TCHAR reqbuf[1024];

	TCHAR msg[0x100];

	PSPECFILEFUNC_REQUEST pReq = (PSPECFILEFUNC_REQUEST) reqbuf;

//+ ------------------------------------------------------------------------------------------
	pReq->m_Request = _sfr_get;
	pReq->m_Idx = 0;
	pReq->m_Data.m_ContainerID = 0;

	ULONG querysize = 0;
	BufferSize = sizeof(querysize);
	bRet = IDriver_SpecFileRequest(pReq, sizeof(reqbuf), &querysize, &BufferSize);

	wsprintf(msg, _T("get %s with size %d last error %s\n"), 
		bRet ? _T("ok") : _T("failed"), 
		bRet ? BufferSize : querysize,
		GetLastError() == ERROR_INSUFFICIENT_BUFFER ? _T("buffer too small") : _T("system error"));
	
	OutputDebugString(msg);
//+ ------------------------------------------------------------------------------------------
	
	pReq->m_Request = _sfr_add;
	pReq->m_Data.m_FileOp = _sfop_rename;
	
	lstrcpy((TCHAR*)pReq->m_Data.m_FileNames, _fn_1);
	lstrcpy((TCHAR*)pReq->m_Data.m_FileNames + lstrlen(_fn_1) + 1, _fn_2);

	pReq->m_Data.m_ContainerID = 0;

	BufferSize = 0;
	bRet = IDriver_SpecFileRequest(pReq, sizeof(reqbuf), NULL, &BufferSize);
	
	if (bRet)
	{
		pReq->m_Request = _sfr_add;
		pReq->m_Data.m_FileOp = _sfop_delete;
		
		lstrcpy((TCHAR*)pReq->m_Data.m_FileNames, _fn_2);
		pReq->m_Data.m_ContainerID = 0;

		BufferSize = 0;
		bRet = IDriver_SpecFileRequest(pReq, sizeof(reqbuf), NULL, &BufferSize);
	}

	if (bRet)
		OutputDebugString(_T("request ok\n"));
	else
		OutputDebugString(_T("request failed\n"));
//+ ------------------------------------------------------------------------------------------

	pReq->m_Request = _sfr_flush;
	BufferSize = 0;
	bRet = IDriver_SpecFileRequest(pReq, sizeof(SPECFILEFUNC_REQUEST), NULL, &BufferSize);

	if (bRet)
		OutputDebugString(_T("flush ok\n"));
	else
		OutputDebugString(_T("flush failed\n"));
//+ ------------------------------------------------------------------------------------------

	int idx = 0;
	TCHAR data[1024];

	PSPECFILEFUNC_FILEBLOCK pBlock = (PSPECFILEFUNC_FILEBLOCK) data;
	do
	{
		pReq->m_Request = _sfr_get;
		pReq->m_Idx = idx++;
		pReq->m_Data.m_ContainerID = 0;

		BufferSize = sizeof(data);
		bRet = IDriver_SpecFileRequest(pReq, sizeof(reqbuf), data, &BufferSize);

		wsprintf(msg, _T("get %s with size %d\n"), bRet ? _T("ok") : _T("failed"), BufferSize);
		OutputDebugString(msg);

		if (bRet && BufferSize)
		{
			wsprintf(msg, _T("\tOp %d result: %d with %s\n"), pBlock->m_FileOp, pBlock->m_Result, 
				pBlock->m_FileNames);
			OutputDebugString(msg);
		}

		if (!BufferSize)
			bRet = FALSE;

	} while(bRet);

	return 0;
}
