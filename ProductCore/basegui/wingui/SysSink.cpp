//////////////////////////////////////////////////////////////////////
// SysSink.cpp : implementation file

#include "stdafx.h"
#include "SysSink.h"

//////////////////////////////////////////////////////////////////////
// CLocaleRegionsCombo

void CLocaleRegionsCombo::OnInit()
{
	tKeys aKeys;
	Root()->GetKeys(PROFILE_LOCALIZE, "ISO3166-1", aKeys, false, Item());

	size_t nCount = aKeys.size();
	cVar v;
	for(size_t i = 0; i < nCount; i++)
	{
		tIniKey &strKey = aKeys[i];

		swap_objmem(v.MakeString(), (tString&)strKey);
		Item()->AddItem(Root()->LocalizeStr(TOR(cStrObj,()), strKey.val.c_str(), strKey.val.size()), v);
	}
}

//////////////////////////////////////////////////////////////////////
// CStdinRedirected

HANDLE CStdinRedirected::m_hProxyProcess = NULL;
HANDLE CStdinRedirected::m_hProxyRd = NULL;
HANDLE CStdinRedirected::m_hProxyWr = NULL;
HANDLE CStdinRedirected::m_hClientRd = NULL;
HANDLE CStdinRedirected::m_hClientWr = NULL;

static HANDLE hex2handle(LPCWSTR szwHandle)
{
	int len = lstrlenW(szwHandle);
	if (!szwHandle && !len)
		return NULL;

	int start = 0;
	if(len>2 && (szwHandle[1]==L'x' || szwHandle[1]==L'X'))
		start = 2;

	DWORD_PTR ret = 0;
	for (int i = len-1, bit_offset = 0; i>=start && bit_offset<=sizeof(DWORD_PTR)*8-4; --i, bit_offset+=4)
	{
		if (szwHandle[i] >= L'0' && szwHandle[i] <= L'9')
			ret |= (((DWORD_PTR)(szwHandle[i]-L'0'))<<bit_offset);
		else if(szwHandle[i] >= L'a' && szwHandle[i] <= L'f')
			ret |= (((DWORD_PTR)(szwHandle[i]-L'a'+0xA))<<bit_offset);
		else if(szwHandle[i] >= L'A' && szwHandle[i] <= L'F')
			ret |= (((DWORD_PTR)(szwHandle[i]-L'A'+0xA))<<bit_offset);
		else
			return NULL;
	}

	return (HANDLE)ret;
}

CStdinRedirected::CStdinRedirected() :
	m_hChildStdoutRd(NULL),
	m_hChildStdoutWr(NULL),
	m_hChildProcess(NULL),
	m_hChildThread(NULL),
	m_nEvent(0)
{
}

void CStdinRedirected::OnCreate()
{
	TBase::OnCreate();
	
	if( m_pItem->m_strItemType == GUI_ITEMT_EDIT )
	{
		m_pItem->SetText(m_strText.c_str());
		m_strText.erase();
	}
}

void CStdinRedirected::OnDestroy()
{
	if( m_pItem->m_strItemType == GUI_ITEMT_EDIT )
		m_strText = m_pItem->m_strText;
	
	TBase::OnDestroy();
}

void CStdinRedirected::OnTimerRefresh(tDWORD nTimerSpin)
{
	CheckProcessOutput();
	
	TBase::OnTimerRefresh(nTimerSpin);
}

bool CStdinRedirected::is_connection_available()
{
	return m_hProxyProcess && WaitForSingleObject(m_hProxyProcess, 0) != WAIT_OBJECT_0;
}

void CStdinRedirected::clear_connection_handles()
{
	if(m_hProxyRd) CloseHandle(m_hProxyRd);
	if(m_hProxyWr) CloseHandle(m_hProxyWr);
	if(m_hClientRd) CloseHandle(m_hClientRd);
	if(m_hClientWr) CloseHandle(m_hClientWr);

	if(m_hProxyProcess) CloseHandle(m_hProxyProcess);

	m_hProxyRd=NULL;
	m_hProxyWr=NULL;
	m_hClientRd = NULL;
	m_hClientWr = NULL;

	m_hProxyProcess = NULL;
}

DWORD CStdinRedirected::send_recv(LPCWSTR szwSend, LPVOID lpBuffer, DWORD dwBuffSize)
{
	if (!dwBuffSize || !lpBuffer ||!m_hProxyProcess)
		return 0;

	if (szwSend)
		if (PR_FAIL(send_command(szwSend)))
			return 0;

	return peek_pipe(m_hClientRd, lpBuffer, dwBuffSize);
}

tERROR CStdinRedirected::make_connection()
{
	tERROR err = errNOT_OK;

	clear_connection_handles();

	cStringObj szRescuePath(L"%ProductRoot%\\rescue32.exe");
	((CRootSink*)(Root()->m_pSink))->ExpandEnvironmentString(szRescuePath);

	SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), 0,};
	if( CreatePipe(&m_hClientRd, &m_hProxyWr, &saAttr, 0) &&
		CreatePipe(&m_hProxyRd, &m_hClientWr, &saAttr, 0) )
	{

		cStringObj szwParams;
		szwParams.format( cCP_UNICODE, L"%08X %08X %08X", GetCurrentProcessId(), m_hProxyRd, m_hProxyWr);

		SHELLEXECUTEINFOW _info = {0};
		_info.cbSize = sizeof(_info);
		_info.fMask = SEE_MASK_NOCLOSEPROCESS;
		_info.nShow = SW_MINIMIZE;
		_info.lpFile = szRescuePath.data();
		_info.lpParameters = szwParams.data();

		if( ::ShellExecuteExW(&_info) )
		{
			WCHAR ret[9];
			m_hProxyProcess = _info.hProcess;

			//handshake
			if (send_recv(NULL, ret, sizeof(ret))==sizeof(ret))
			{
				ret[8] = L'\0';
				if(hex2handle(ret)==NULL)
					err = errOK;
			}
			
			if(PR_FAIL(err))
			{
				GetExitCodeProcess(m_hProxyProcess, (PDWORD)&m_nProcessExitCode);
				if( m_nProcessExitCode == STILL_ACTIVE ) 
					TerminateProcess(m_hProxyProcess, 0);

				clear_connection_handles();
			}
		}
	}
	else
		err = errOBJECT_CANNOT_BE_INITIALIZED;

	return err;
}


tERROR CStdinRedirected::start_process(bool bCreateProcess, HWND hWnd, LPCWSTR strExe, LPCWSTR strParameters, LPCWSTR strBaseFolder)
{
	m_strText.erase();
	
	SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), 0, TRUE};

	if (bCreateProcess)
	{
		if(!CreatePipe(&m_hChildStdoutRd, &m_hChildStdoutWr, &saAttr, 0))
		{
			m_err = errOBJECT_CANNOT_BE_INITIALIZED;
			return m_err;
		}
	}

	if (is_connection_available())
	{
		m_err = errNOT_OK;
		cStringObj strCommand;

		if (bCreateProcess)
			strCommand.format(cCP_UNICODE, L"process %08X \"%S\" %S", m_hChildStdoutWr, strBaseFolder, strParameters);
		else
			strCommand.format(cCP_UNICODE, L"shellexec %08X \"%S\" \"%S\" %S", hWnd, strExe, strBaseFolder, strParameters);

		WCHAR ret[9];
		if (send_recv(strCommand.data(), ret, sizeof(ret))==sizeof(ret))
		{
			ret[8] = L'\0';
			m_hChildProcess = hex2handle(ret);
			if(m_hChildProcess)
				m_err = errOK;
		}
	}
	else if(bCreateProcess)
	{
		PROCESS_INFORMATION pi = {0, };
		STARTUPINFOW si = {sizeof(STARTUPINFOW), 0, };
		si.hStdOutput = m_hChildStdoutWr;
		si.hStdError = m_hChildStdoutWr;
		si.dwFlags |= STARTF_USESTDHANDLES;

		if( ::CreateProcessW(NULL, (LPWSTR)strParameters, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, strBaseFolder, &si, &pi) )
			m_hChildProcess = pi.hProcess;
		else
			m_err = errNOT_OK;
	}
	else
	{
		SHELLEXECUTEINFOW _info; memset(&_info, 0, sizeof(_info));
		_info.cbSize = sizeof(_info);
		_info.fMask = SEE_MASK_NOCLOSEPROCESS;
		_info.nShow = SW_MINIMIZE;
		_info.lpFile = strExe;
		_info.hwnd = hWnd;
		_info.lpParameters = strParameters;
		_info.lpDirectory = strBaseFolder;

		if( ::ShellExecuteExW(&_info) )
			m_hChildProcess = _info.hProcess;
		else
			m_err = errUNEXPECTED;
	}

	return m_err;
}

tERROR CStdinRedirected::CreateProcess(LPCWSTR strParameters, LPCWSTR strBaseFolder, tDWORD nEvent)
{
	m_err = errOK;
	m_nEvent = nEvent;
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::stdin redirect: CreateProcess() nEvent: 0x%08x", m_nEvent));

//Vista
	if (/*(tBYTE)GetVersion()>5 && */!is_connection_available())
		make_connection();

	if( PR_FAIL(start_process(true, NULL, NULL, strParameters, strBaseFolder)) )
		DoneProcess();

	return m_err;
}

tERROR CStdinRedirected::ShellExec(HWND hWnd, LPCWSTR strExe, LPCWSTR strParameters, LPCWSTR strBaseFolder, tDWORD nEvent)
{
	m_err = errOK;
	m_nEvent = nEvent;
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::stdin redirect: ShellExecute() nEvent: 0x%08x", m_nEvent));

	//Vista
	if (/*(tBYTE)GetVersion()>5 && */!is_connection_available())
		make_connection();

	if( PR_FAIL(start_process(false, hWnd, strExe, strParameters, strBaseFolder)) )
		DoneProcess();

	return m_err;
}

void CStdinRedirected::DoneProxyProcess()
{
	send_command(L"stop");
	clear_connection_handles();
}

HANDLE CStdinRedirected::GetChildHandle()
{
	return m_hChildProcess;
}

tERROR CStdinRedirected::send_command(LPCWSTR strCommand)
{
	DWORD dwWrite = (wcslen(strCommand)+1)*sizeof(WCHAR), dwWritten = 0;
	if( m_hClientWr && (m_hClientWr!=INVALID_HANDLE_VALUE) && ::WriteFile(m_hClientWr, strCommand, dwWrite, &dwWritten, NULL) &&
		dwWritten == dwWrite )
		return errOK;

	return errNOT_OK;
}

DWORD CStdinRedirected::peek_pipe(HANDLE hPipeRd, LPVOID buf, DWORD buf_size)
{
	DWORD nSize = 0;
	if ( hPipeRd && (hPipeRd!=INVALID_HANDLE_VALUE) && buf && buf_size && ReadFile(hPipeRd, buf, buf_size, (LPDWORD)&nSize, NULL) )
		return nSize;
	
	return 0;
}

tERROR CStdinRedirected::DoneProcess()
{
	GetExitCodeProcess(m_hChildProcess, (PDWORD)&m_nProcessExitCode);
	if( m_nProcessExitCode == STILL_ACTIVE ) 
		TerminateProcess(m_hChildProcess, 0);

	if( m_hChildStdoutRd )
	{
		CloseHandle(m_hChildStdoutRd);
		m_hChildStdoutRd = NULL;
	}
	if( m_hChildStdoutWr )
	{
		CloseHandle(m_hChildStdoutWr);
		m_hChildStdoutWr = NULL;
	}
	if( m_hChildProcess )
	{
		CloseHandle(m_hChildProcess); 
		m_hChildProcess = NULL;
	}
	if( m_hChildThread )
	{
		CloseHandle(m_hChildThread);
		m_hChildThread = NULL;
	}

	Item()->SendEvent(m_nEvent, NULL, false);
	//PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::stdin redirect: DoneProcess() nEvent: 0x%08x", m_nEvent));

	return errOK;
}

tERROR CStdinRedirected::CheckProcessOutput()
{
	if( !m_hChildProcess )
		return errOBJECT_NOT_INITIALIZED;

	bool bProcessExited = WaitForSingleObject(m_hChildProcess, 0) == WAIT_OBJECT_0;
	//PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::stdin redirect: CheckProcessOutput() bProcessExited: %u", bProcessExited));
	
	DWORD nSize;
	while( PeekNamedPipe(m_hChildStdoutRd, NULL, 0, NULL, &nSize, NULL ) && nSize )
	{
		char Buffer[0x400];
		if( ReadFile(m_hChildStdoutRd, Buffer, sizeof(Buffer) - 1, (LPDWORD)&nSize, NULL) )
		{
			Buffer[nSize] = 0;

			_ProcessOutputMsg(Buffer);
		}
	}
	
	if( bProcessExited )
		return DoneProcess();

	return errOK;
}

void CStdinRedirected::_ProcessOutputMsg(LPCSTR strMsg)
{
	if( m_pItem->m_strItemType == GUI_ITEMT_EDIT )
		((CEditItem *)m_pItem)->AddText(strMsg, true);
	else if( strMsg )
		m_strText += strMsg;
}

/////////////////////////////////////////////////////////////////////////////
// CProcessorList

struct cAffinity
{
	tDWORD  m_nCpuNumber;
	tDWORD  m_nAffinityMask;
};

CProcessorList::CProcessorList() :
	m_nMask(0),
	m_nNumber(2)
{}

void CProcessorList::OnInit()
{
	LoadLocString(m_strDescr, "affinity.ProcDescr");
	
	CListItemSink::OnInit();
}

bool CProcessorList::OnSetValue(const cVariant &pValue)
{
	tQWORD nValue = pValue.ToDWORD();
	cAffinity &pAffinity = *(cAffinity *)nValue;
	m_nMask = pAffinity.m_nAffinityMask;
	m_nNumber = pAffinity.m_nCpuNumber;

	Item()->UpdateView();
	return true;
}

bool CProcessorList::OnGetValue(cVariant &pValue)
{
	cAffinity Affinity;
	Affinity.m_nAffinityMask = m_nMask;
	Affinity.m_nCpuNumber = m_nNumber;
	pValue = *(tQWORD *)&Affinity;
	return true;
}

bool CProcessorList::OnDataChanged(int nItem, int nColumn, cVariant& pData)
{
	bool bChecked = pData;
	if( bChecked )
		m_nMask |= (1 >> nItem);
	else
		m_nMask &= ~(1 >> nItem);

	return true;
}

/*
tERROR CProcessorList::OnGetRecord(cRowData& pData)
{
	tCHAR text[256];
	sprintf(text, "%s %d", m_strDescr.c_str(), pData.m_nIdx);

	pData.m_vCells[0].m_data = !!(m_nMask & (1 << pData.m_nIdx));
	pData.m_vCells[0].m_text = text;
	return errOK;
}
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
