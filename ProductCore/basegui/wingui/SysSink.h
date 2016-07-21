// SysSink.h: interface for the system depended sinks.
//

#if !defined(AFX_SYSSINK_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
#define AFX_SYSSINK_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_

//////////////////////////////////////////////////////////////////////
// CLocaleRegionsCombo

class CLocaleRegionsCombo : public CItemSinkT<CComboItem>
{
protected:
	void OnInit();
};

//////////////////////////////////////////////////////////////////////
// CStdinRedirected

class CStdinRedirected : public CStdinRedirectedSinkBase
{
public:
	typedef CStdinRedirectedSinkBase TBase;
	CStdinRedirected();

protected:
	void OnCreate();
	void OnDestroy();
	void OnTimerRefresh(tDWORD nTimerSpin);
	tERROR make_connection();
	tERROR start_process(bool bCreateProcess, HWND hWnd, LPCWSTR strExe, LPCWSTR strParameters, LPCWSTR strBaseFolder);
	bool is_connection_available();
	void clear_connection_handles();
	tERROR send_command(LPCWSTR strCommand);
	DWORD peek_pipe(HANDLE hPipeRd, LPVOID buf, DWORD buf_size);
	DWORD send_recv(LPCWSTR szwSend, LPVOID lpBuffer, DWORD dwBuffSize);

	
public:
	tERROR CreateProcess(LPCWSTR strParameters, LPCWSTR strBaseFolder, tDWORD nEvent);
	tERROR ShellExec(HWND hWnd, LPCWSTR strExe, LPCWSTR strParameters, LPCWSTR strBaseFolder, tDWORD nEvent);
	tERROR DoneProcess();
	tERROR CheckProcessOutput();
	void   GetResultText(tString& str) { str = m_strText; };

	void _ProcessOutputMsg(LPCSTR strMsg);
	void DoneProxyProcess();
	HANDLE GetChildHandle();

protected:
	HANDLE      m_hChildProcess;
	HANDLE      m_hChildStdoutRd;
	HANDLE      m_hChildStdoutWr;
	HANDLE      m_hChildThread;
	tDWORD      m_nEvent;
	static HANDLE		m_hProxyProcess;
	static HANDLE		m_hProxyRd;
	static HANDLE		m_hProxyWr;
	static HANDLE		m_hClientRd;
	static HANDLE		m_hClientWr;
	
	tString     m_strText;
};

//////////////////////////////////////////////////////////////////////
// CProcessorList

class CProcessorList : public CListItemSink
{
public:
	CProcessorList();

	void     OnInit();
	bool     OnSetValue(const cVariant &pValue);
	bool     OnGetValue(cVariant &pValue);
	bool     OnIsVector(){ return false; }
	tTYPE_ID OnGetType(){ return tid_QWORD; }
	bool     OnDataChanged(int nItem, int nColumn, cVariant& pData);

protected:
//	tERROR   OnGetRecord(cRowData& pData);
//	tDWORD   OnGetCount(){ return m_nNumber; }

private:
	tDWORD   m_nNumber;
	tDWORD   m_nMask;
	bool     m_bInited;
	tString  m_strDescr;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SYSSINK_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
