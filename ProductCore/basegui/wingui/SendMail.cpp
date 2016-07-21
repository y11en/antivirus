/////////////////////////////////////////////////////////////////////////////
// SendMail.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SendMail.h"
#include <mapi.h>

#include <Prague/pr_remote.h>

#undef send 

#ifndef WINSOCK_VERSION
#define WINSOCK_VERSION MAKEWORD(2,2)
#endif

#pragma comment(lib,"ws2_32.lib")

bool b64encodeString(const char *inbuf, char *outbuf, int size);

// cStringObj не корректно конвертирует строки из cCP_UNICODE в cCP_ANSI
// на некоторых MultiByte кодировках, например, в японском языке.
// Поэтому необходимо напрямую использовть WideCharToMultiByte
bool Uni2Str(LPCWSTR strW, DWORD lenW, tString& str)
{
	std::vector<char> strBuf;
	int size = ::WideCharToMultiByte(CP_ACP, 0, strW, lenW, NULL, 0, NULL, NULL);
	if( size == 0 )
		return false;

	strBuf.resize(size);
	if( ::WideCharToMultiByte(CP_ACP, 0, strW, lenW, &strBuf[0], size, NULL, NULL) == 0 )
		return false;

	str = tString(&strBuf[0], size);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMTPSender::CSMTPSender() : m_bHasError(FALSE)
{
	// Create socket
	if( WSAStartup(WINSOCK_VERSION, &m_wsaData) )
	{
		DecodeError();
		return;
	}
	
	m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( m_sock == INVALID_SOCKET )
		DecodeError();
}

CSMTPSender::~CSMTPSender()
{
	Disconnect();
	closesocket( m_sock );
	WSACleanup();
}

BOOL CSMTPSender::Connect(LPCSTR szServer, u_short usServerPort, LPCSTR szFrom, LPCSTR szUserName, LPCSTR szPassword)
{
	if( m_bHasError )
		return FALSE;

	unsigned long ulAddress = inet_addr (szServer); 
	struct hostent *pHost; 

	if( pHost = gethostbyname (szServer) )
	{
		memcpy((char FAR *)&ulAddress, pHost->h_addr, pHost->h_length); 
	}
	else
	{
		PR_TRACE((g_root,prtERROR,"Unable to resolve host name."));
		return DecodeError();
	} 

	m_sin.sin_family = AF_INET;
	m_sin.sin_addr.s_addr = ulAddress;   
	m_sin.sin_port = htons(usServerPort);
	
	int err = connect(m_sock , (PSOCKADDR)&m_sin, sizeof(m_sin) );
	if( !CheckResponse(err, check_Connection) )
		return FALSE;

	m_szFrom = szFrom;

	cStrObj strDomain;
	tDWORD pos = m_szFrom.find("@");
	if( pos != cStrObj::npos )
		strDomain.assign(m_szFrom, pos+1);

	m_szCommand.format(cCP_ANSI, "HELO %S\r\n", strDomain.data());
	SendCommand(check_Hello);

	if( szUserName && *szUserName && szPassword && *szPassword )
	{
		char base64tmp[512];

		m_szCommand.format(cCP_ANSI, "AUTH LOGIN\r\n");
		SendCommand(check_Auth);

		if( !m_bHasError )
		{
			b64encodeString(szUserName, base64tmp, sizeof(base64tmp));
			m_szCommand.format(cCP_ANSI, "%s\r\n", base64tmp);
			SendCommand(check_Auth);

			b64encodeString(szPassword, base64tmp, sizeof(base64tmp));
			m_szCommand.format(cCP_ANSI, "%s\r\n", base64tmp);
			SendCommand(check_Auth);
		}
		else
			m_bHasError = FALSE;
	}

	return !m_bHasError;
}

BOOL CSMTPSender::SendData(LPCSTR szTo, LPCSTR szSubject, LPCWSTR szBody, LPCSTR szCharset, CRootItem * pRoot)
{
	if( m_bHasError )
		return FALSE;

	m_szTO = szTo;

	m_szCommand.format(cCP_ANSI, "MAIL FROM:<%S>\r\n", m_szFrom.data());
	SendCommand(check_MailFrom);

	m_szCommand.format(cCP_ANSI,"RCPT TO:<%S>\r\n", m_szTO.data());
	SendCommand(check_Rcpt);

	m_szCommand.format(cCP_ANSI,"DATA\r\n");
	SendCommand(check_DataStart);

	m_szCommand.format(cCP_ANSI,"Return-Path:<>\r\n");
	SendCommand(check_None);

	m_szCommand.format(cCP_ANSI,"SUBJECT:%s\r\n", szSubject);
	SendCommand(check_None);

	m_szCommand.format(cCP_ANSI,"TO: <%S>\r\n", m_szTO.data());
	SendCommand(check_None);

	m_szCommand.format(cCP_ANSI,"FROM: <%S>\r\n", m_szFrom.data());
	SendCommand(check_None);

	m_szCommand.format(cCP_ANSI,"MIME-Version: 1.0\r\n");
	SendCommand(check_None);

	m_szCommand.format(cCP_ANSI,"Content-Type: text/plain; charset=UTF-8\r\n", szCharset);
	SendCommand(check_None);
			
	m_szCommand.format(cCP_ANSI,"Content-Transfer-Encoding: base64\r\n");
	SendCommand(check_None);

	// Необходимо обходить ситуацию с единственной точкой в строке,
	// т.к. это преждевременно завершит письмо
	cStrObj szBody1 = szBody;
	szBody1.replace_all("\r\n.\r\n", 0, "\r\n..\r\n");
	szBody1.replace_all("\n.\n", 0, "\r\n..\r\n");
	tDWORD size = 5*szBody1.size();

	cStrBuff strBase64;
	b64encodeString(pRoot->LocalizeStr(TOR(tString,()), szBody1.data(), szBody1.size()), strBase64.get(size, true), size);

	m_szCommand.format(cCP_ANSI,"\r\n%s", (LPCSTR)strBase64.ptr());
	SendCommand(check_None);

	m_szCommand.format(cCP_ANSI,"\r\n.\r\n");
	SendCommand(check_DataEnd);
	
	return !m_bHasError;
}

BOOL CSMTPSender::SendCommand(SMTPCheckedType Type)
{
	if( m_bHasError )
		return FALSE;

	PR_TRACE((g_root,prtIMPORTANT,"CSMTPSender::SendCommand => %S\n", m_szCommand.data()));

	tString strCommand;
	Uni2Str(m_szCommand.data(), m_szCommand.size(), strCommand);
	int err = send( m_sock, strCommand.c_str(), (int)strCommand.size(), 0); 
	return CheckResponse(err, Type);
}

BOOL CSMTPSender::Disconnect()
{
	if( m_bHasError )
		return FALSE;

	m_szCommand.format(cCP_ANSI,"QUIT \r\n");
	SendCommand(check_Quit);
	return !m_bHasError;
}

////////////////////////////////////////////////////
// Заимствованный код

BOOL CSMTPSender::CheckResponse(DWORD error, SMTPCheckedType Type)
{
	if( error == SOCKET_ERROR )
	{
		DecodeError();
		return FALSE;
	}

	if( Type == check_None )
		return TRUE;

	char buf [1000];
	memset(buf, 0, sizeof(buf));
	
	// Receive the data from Server
	int err = recv(m_sock, buf, sizeof(buf), 0);
	if ( err == SOCKET_ERROR ) 
	{
		PR_TRACE((g_root,prtERROR,"SOCKET_ERROR\n"));
		closesocket( m_sock );
		return FALSE;
	}
	
	char temp[4] = {0};
	strncpy(temp,buf,3);
	
	PR_TRACE((g_root, prtIMPORTANT, "CSMTPSender::CheckResponse => %s", (LPCTSTR)buf));

	int temp2 = atoi(temp);
	switch (Type)
	{
	case check_Connection: if (temp2 == 220) return TRUE; break;
	case check_Hello:      if (temp2 == 250) return TRUE; break;
	case check_MailFrom:   if (temp2 == 250) return TRUE; break;
	case check_Rcpt:       if (temp2 == 250) return TRUE; break;
	case check_DataStart:  if (temp2 == 354) return TRUE; break;
	case check_DataEnd:    if (temp2 == 250) return TRUE; break;
	case check_Quit:       if (temp2 == 221) return TRUE; break;
	case check_Data:       if (temp2 == 354) return TRUE; break;
	case check_Auth:       if (temp2 == 235 || temp2 == 334) return TRUE; break;
	default: return TRUE;
	}

	PR_TRACE((g_root, prtERROR, "CSMTPSender::CheckResponse => %s ERROR!", (LPCTSTR)buf));
	m_bHasError = TRUE;
	return FALSE;
}

BOOL CSMTPSender::DecodeError()
{
	DWORD ErrorCode = WSAGetLastError();

    TCHAR *pBuf;
    DWORD dwStatus = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&pBuf,
		0,
		NULL); 

	cStrObj strError;
    if (dwStatus != 0) 
	{
		strError = pBuf;
		strError.replace_all("\r\n", 0, "");

        LocalFree(pBuf);
    } 

	PR_TRACE((g_root,prtERROR,"SMTP => SOCKET_ERROR: [%d] %S\n", ErrorCode, strError.data()));
	m_bHasError = TRUE;
	return FALSE;
}

void base64(BYTE *t, BYTE *q)
{
	BYTE alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	q[0] = alpha[t[0] >> 2];
	q[1] = alpha[((t[0] & 03) << 4) | (t[1] >> 4)];
	q[2] = alpha[((t[1] & 017) << 2) | (t[2] >> 6)];
	q[3] = alpha[t[2] & 077];
}

bool b64encodeString(const char *inbuf, char *outbuf, int size)
{
	BYTE iinbuf[1025], t[3], q[4];
	DWORD tp, inp, outp, i;

	memset(outbuf, 0, size);

	size_t inlen = strlen(inbuf);
	strncpy((char *)iinbuf, inbuf, sizeof(iinbuf)-1);

	for(tp=0, inp=0, outp=0;inp < inlen;)
	{
		t[tp++] = inbuf[inp++];

		if (tp == 3)
		{
			base64(t, q);
			for (i=0; i<4; i++)
				outbuf[outp++] = q[i];

			memset(t, 0, sizeof(t));
			tp = 0;
		}
	}

	if (tp)
	{
		base64(t, q);
		if (tp < 3) q[3] = '=';
		if (tp < 2) q[2] = '=';
		for (i=0; i<4; i++)
			outbuf[outp++] = q[i];
	}

	outbuf[outp] = 0;
	return true;
}

/////////////////////////////////////////////////////////////

//#include <plugin\p_dtreg.h>
//
//tERROR SaveStructAsDtReg(cStringObj &strFile, cSerializable *pStruct)
//{
//	tERROR err = errOK;
//	
//	cAutoObj<cRegistry> reg;
//	if( PR_SUCC(err) ) err = ::g_root->sysCreateObject((hOBJECT*)&reg, IID_REGISTRY, PID_DTREG);
//	if( PR_SUCC(err) ) err = strFile.copy(reg, pgOBJECT_NAME);
//	if( PR_SUCC(err) ) err = reg->propSetDWord(pgCLEAN, cTRUE);
//	if( PR_SUCC(err) ) err = reg->propSetDWord(pgOBJECT_RO, cFALSE);
//	if( PR_SUCC(err) ) err = reg->propSetDWord(pgSAVE_RESULTS_ON_CLOSE, cTRUE);
//	if( PR_SUCC(err) ) err = reg->sysCreateObjectDone();
//	if( PR_SUCC(err) ) err = g_root->RegSerialize(pStruct, SERID_UNKNOWN, reg, NULL);
//	return err;
//}

/////////////////////////////////////////////////////////////

CMailSender::~CMailSender()
{
	m_work = 0;
	if( m_loop )
		wait_for_stop();
}

void CMailSender::do_work()
{
	if( m_info.m_strSMTPServer.empty() )
	{
		m_err = CreateMailSenderProcess();
		if( m_loop )
			m_loop->m_pRoot = NULL;
		return;
	}

	if( m_loop )
		m_loop->m_pRoot = NULL;
	
	CSMTPSender smtp;
	BOOL res = smtp.Connect(m_info.m_strSMTPServer.c_str(cCP_ANSI), m_info.m_nSMPTPort, m_info.m_strFrom.c_str(cCP_ANSI),
			m_info.m_strUserName.c_str(cCP_ANSI), m_info.m_strPassword.c_str(cCP_ANSI));

	if( m_info.m_aAddresses.size() != 1 )
		res = FALSE;

	tString strAddress, strSubject;
	Uni2Str(m_info.m_aAddresses[0].data(), m_info.m_aAddresses[0].size(), strAddress);
	Uni2Str(m_info.m_strSubject.data(), m_info.m_strSubject.size(), strSubject);
	if( res ) res = smtp.SendData(strAddress.c_str(), strSubject.c_str(),
		m_info.m_strBody.data(), "windows-1251", m_pRoot);

	m_err = res ? errOK : errUNEXPECTED;
}

tERROR CMailSender::CreateMailSenderProcess()
{
	tERROR err = errOK;

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE hChildStdinRd(NULL), hChildStdinWr(NULL), hChildStdoutRd(NULL), hChildStdoutWr(NULL);
	if( PR_SUCC(err) )
	{
		if( CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0) )
		{
			HANDLE hChildStdinWrDup;
			DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(), &hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
			CloseHandle(hChildStdinWr);
			hChildStdinWr = hChildStdinWrDup;
		}
		else
			err = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	if( PR_SUCC(err) )
	{
		if( CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) )
		{
			HANDLE hChildStdoutRdDup;
			DuplicateHandle(GetCurrentProcess(), hChildStdoutRd, GetCurrentProcess(), &hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
			CloseHandle(hChildStdoutRd);
			hChildStdoutRd = hChildStdoutRdDup;
		}
		else
			err = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	if( PR_SUCC(err) )
	{
		tDWORD nSize = 0x4000;
		cMemChunk buff(g_root);
		err = ::g_root->StreamSerialize(&m_info, SERID_UNKNOWN, (tBYTE *)buff.reserve(nSize, false), nSize, &nSize);
		if( err == errBUFFER_TOO_SMALL )
			err = ::g_root->StreamSerialize(&m_info, SERID_UNKNOWN, (tBYTE *)buff.reserve(nSize, false), nSize, &nSize);
		buff.m_used = nSize;

		if( PR_SUCC(err) )
		{
			char BaseguiPath[MAX_PATH];
			GetModuleFileName(GetModuleHandle("basegui.ppl"), BaseguiPath, countof(BaseguiPath));
			if( char *pLastSlash = strrchr(BaseguiPath, '\\') )
				*pLastSlash = 0;

			PROCESS_INFORMATION pi = {0, };
			STARTUPINFO si = {sizeof(STARTUPINFO), 0, };
			si.hStdInput = hChildStdinRd;
			si.hStdOutput = hChildStdoutWr;
			si.dwFlags |= STARTF_USESTDHANDLES;
			if( CreateProcess(NULL, "rundll32.exe basegui.ppl,MailSender", NULL, NULL, TRUE, 0, NULL, BaseguiPath, &si, &pi) )
			{
				tDWORD nMsgSize = buff.used();
				WriteFile(hChildStdinWr, &nMsgSize, sizeof(tDWORD), (LPDWORD)&nSize, NULL);
				WriteFile(hChildStdinWr, buff, nMsgSize, (LPDWORD)&nSize, NULL);
				
				bool bEnd = false;
				while( m_work )
				{
					if( WaitForSingleObject(pi.hProcess, 100) == WAIT_OBJECT_0 )
					{
						bEnd = true;
						break;
					}
				}
				
				if( bEnd )
					ReadFile(hChildStdoutRd, &err, sizeof(err), (LPDWORD)&nSize, NULL);
				else
					err = warnFALSE;

				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			else
				err = errNOT_OK;
		}
	}

	if( hChildStdinRd )
		CloseHandle(hChildStdinRd);
	if( hChildStdinWr )
		CloseHandle(hChildStdinWr);
	if( hChildStdoutRd )
		CloseHandle(hChildStdoutRd);
	if( hChildStdoutWr )
		CloseHandle(hChildStdoutWr);

	return err;
}

/////////////////////////////////////////////////////////////

tERROR MapiSendMail(CSendMailInfo &info);

extern "C" __declspec(dllexport) void _stdcall MailSender(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if( hInput == INVALID_HANDLE_VALUE || hOutput == INVALID_HANDLE_VALUE )
		return;

	CPRRemotePrague Prague;
	tERROR err = Prague.Init(hinst, 0);
	if( PR_SUCC(err) )
	{
		char *sPlugins[] = {"params.ppl"};
		for(int i = 0; i < countof(sPlugins) && PR_SUCC(err); i++)
			err = g_root->LoadModule(NULL, sPlugins[i], 0, cCP_ANSI);
	}
	tDWORD nSize;
	if( PR_SUCC(err) )
	{
		tDWORD nMsgSize;
		cMemChunk buff(g_root);
		ReadFile(hInput, &nMsgSize, sizeof(tDWORD), (LPDWORD)&nSize, NULL);
		ReadFile(hInput, buff.reserve(nMsgSize, false), nMsgSize, (LPDWORD)&nSize, NULL);
		buff.m_used = nSize;

		const cSerRegistrar * tmp;
		for(tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
			g_root->RegisterSerializableDescriptor(tmp->m_dsc);
		
		cSerializable *pStruct = NULL;
		if( PR_SUCC(err = ::g_root->StreamDeserialize(&pStruct, (tBYTE *)buff.ptr(), buff.used(), NULL)) )
		{
			if( pStruct && pStruct->isBasedOn(CSendMailInfo::eIID) )
			{
				err = MapiSendMail(*(CSendMailInfo *)pStruct);
				g_root->DestroySerializable(pStruct);
			}
			else
				err = errPARAMETER_INVALID;
		}
		
		for(tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
			g_root->UnregisterSerializableDescriptor(tmp->m_dsc);
	}
	WriteFile(hOutput, &err, sizeof(err), (LPDWORD)&nSize, NULL);
}

tERROR MapiSendMail(CSendMailInfo &info)
{
	CLibWrapper    hMapi32("mapi32.dll");
	LPMAPILOGON    pfnMAPILogon = NULL;    hMapi32.GetFunc((FARPROC *)&pfnMAPILogon,    "MAPILogon");
	LPMAPISENDMAIL pfnMAPISendMail = NULL; hMapi32.GetFunc((FARPROC *)&pfnMAPISendMail, "MAPISendMail");
	LPMAPILOGOFF   pfnMAPILogoff = NULL;   hMapi32.GetFunc((FARPROC *)&pfnMAPILogoff,   "MAPILogoff");
	hMapi32.Detach();
	if( !pfnMAPILogon || !pfnMAPISendMail || !pfnMAPILogoff )
		return errNOT_OK;
	
	std::vector<MapiRecipDesc> l_recipients;
	std::vector<MapiFileDesc>  l_attaches;
	std::vector<tString>       l_recipientsNames;
	std::vector<tString>       l_attachesObjs;
	std::vector<tString>       l_attachesNames;
	tString                    l_strSubject;
	tString                    l_strBody;
	MapiMessage                l_msg;
	memset(&l_msg, 0, sizeof(MapiMessage));

	size_t i;
	
	if(info.m_aAddresses.empty())
		return errNOT_OK;

	l_recipients.resize(info.m_aAddresses.size());
	l_recipientsNames.resize(info.m_aAddresses.size());
	for(i = 0; i < info.m_aAddresses.size(); i++)
	{
		Uni2Str(info.m_aAddresses[i].data(), info.m_aAddresses[i].size(), l_recipientsNames[i]);
		
		memset(&l_recipients[i], 0, sizeof(MapiRecipDesc));
		l_recipients[i].ulRecipClass = MAPI_TO;
		l_recipients[i].lpszAddress = (LPSTR)l_recipientsNames[i].c_str();
		l_recipients[i].lpszName = (LPSTR)l_recipientsNames[i].c_str();
	}

	l_attaches.resize(info.m_aAattaches.size());
	l_attachesObjs.resize(info.m_aAattaches.size());
	l_attachesNames.resize(info.m_aAattaches.size());
	for(i = 0; i < info.m_aAattaches.size(); i++)
	{
		const CAttachedFile& pAttachFile = info.m_aAattaches[i];
		MapiFileDesc& pMapiAttach = l_attaches[i];

		Uni2Str(pAttachFile.m_strObjPath.data(), pAttachFile.m_strObjPath.size(), l_attachesObjs[i]);
		Uni2Str(pAttachFile.m_strName.data(), pAttachFile.m_strName.size(), l_attachesNames[i]);
	
		memset(&pMapiAttach, 0, sizeof(MapiFileDesc));
		pMapiAttach.lpszPathName = (LPSTR)l_attachesObjs[i].c_str();
		if( !l_attachesNames[i].empty() )
			pMapiAttach.lpszFileName = (LPSTR)l_attachesNames[i].c_str();
		pMapiAttach.nPosition = -1;
	}

	Uni2Str(info.m_strSubject.data(), info.m_strSubject.size(), l_strSubject);
	Uni2Str(info.m_strBody.data(), info.m_strBody.size(), l_strBody);

	l_msg.lpszSubject  = (LPSTR)l_strSubject.c_str();
	l_msg.lpszNoteText = (LPSTR)l_strBody.c_str();
	l_msg.nRecipCount  = (ULONG)l_recipients.size();
	l_msg.lpRecips     = &l_recipients[0];
	l_msg.nFileCount   = (ULONG)l_attaches.size();
	l_msg.lpFiles      = l_attaches.size() ? &l_attaches[0] : NULL;

	LHANDLE hSession;
	ULONG   errMapi;

	errMapi = pfnMAPILogon(0, NULL, NULL, MAPI_LOGON_UI, 0, &hSession);
	if( errMapi != SUCCESS_SUCCESS )
	{
		PR_TRACE((g_root, prtERROR, "GUI(Win)::MAPILogon fails (HRESULT 0x%08x)\n", errMapi));
		switch( errMapi )
		{
		case MAPI_E_INSUFFICIENT_MEMORY: return errNOT_ENOUGH_MEMORY;
		case MAPI_E_USER_ABORT:          return warnFALSE;
		}

		return errNOT_OK;
	}

	PR_TRACE((g_root, prtERROR, "GUI(Win)::MAPISendMail attempt, Recip=%S", info.m_aAddresses.size() ? info.m_aAddresses[0].data() : L"none"));

	tERROR err = errOK;
	errMapi = pfnMAPISendMail(hSession, 0, &l_msg, MAPI_DIALOG, 0);
	if( errMapi != SUCCESS_SUCCESS )
	{
		PR_TRACE((g_root, prtERROR, "GUI(Win)::MAPISendMail fails (HRESULT 0x%08x)", errMapi));

		switch( errMapi )
		{
		case MAPI_E_INSUFFICIENT_MEMORY: err = errNOT_ENOUGH_MEMORY; break;
		case MAPI_E_USER_ABORT:          err = warnFALSE; break;
		default:                         err = errNOT_OK;
		}
	}

	errMapi = pfnMAPILogoff(hSession, NULL, 0, 0);
	if( errMapi != SUCCESS_SUCCESS )
	{
		PR_TRACE((g_root, prtERROR, "GUI(Win)::MAPILogoff fails (HRESULT 0x%08x)", errMapi));
	}

	return err;
}