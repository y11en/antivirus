/////////////////////////////////////////////////////////////////////////////
// SendMail.h : implementation file
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVI_SMTP_H__86B424B3_A211_11D5_851C_006052067816__INCLUDED_)
#define AFX_DVI_SMTP_H__86B424B3_A211_11D5_851C_006052067816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>
#include <stdio.h>

#define DEFAULT_SERVER	NULL
#define DEFAULT_FROM	NULL

enum SMTPCheckedType{
	check_Connection = 0, 
	check_Hello, 
	check_MailFrom,
	check_Rcpt,
	check_DataStart,
	check_DataEnd,
	check_Quit,
	check_Data,
	check_Auth,
	check_None
};

/////////////////////////////////////////////////////////////////////////////

class CSMTPSender  
{
public:
	CSMTPSender();
	virtual ~CSMTPSender();

	BOOL Connect(LPCSTR szServer, u_short usServerPort, LPCSTR szFrom, LPCSTR szUserName, LPCSTR szPassword);
	BOOL SendData(LPCSTR szTo, LPCSTR szSubject, LPCWSTR szBody, LPCSTR szCharset, CRootItem * pRoot);
	BOOL SendCommand(SMTPCheckedType Type);
	BOOL Disconnect();

	BOOL CheckResponse(DWORD error, SMTPCheckedType Type);
	BOOL DecodeError();
	
private:
	SOCKADDR_IN	m_sin;  
	SOCKET	    m_sock;
	WSADATA	    m_wsaData;
	cStrObj	    m_szCommand;
	cStrObj	    m_szFrom;
	cStrObj	    m_szTO;
	BOOL        m_bHasError;
};

/////////////////////////////////////////////////////////////////////////////

class CMailSender : public CRootTaskBase
{
public:
	CMailSender(CRootItem * pRoot) : CRootTaskBase(pRoot, 0xbf1dd4ec), m_work(1), m_loop(NULL), m_err(warnFALSE) {}
	~CMailSender();
	
	void do_work();
	void on_exit() { if( !m_loop ) CRootTaskBase::on_exit(); }

	tERROR CreateMailSenderProcess();

	CSendMailInfo m_info;
	CItemBase *   m_loop;
	tERROR        m_err;
	volatile unsigned      m_work : 1;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DVI_SMTP_H__86B424B3_A211_11D5_851C_006052067816__INCLUDED_)
