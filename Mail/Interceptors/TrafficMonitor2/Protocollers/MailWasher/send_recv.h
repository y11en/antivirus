#ifndef _SEND_RECV_H_
#define _SEND_RECV_H

#include <windows.h>
#include <Prague/prague.h>
#include "lookahead.h"
#include "../../TrafficProtocoller.h"
using namespace TrafficProtocoller;
#include "mw_utils.h"
#include <Prague/pr_wrappers.h>

tERROR IOCopy(hIO src, hIO dst, tQWORD pos_begin = 0, tQWORD length = 0);

class send_t
{
public:
	send_t();
	virtual ~send_t();

public:
	tERROR Init(hOBJECT hParent);
	tERROR Clear();
	hIO Get(OUT data_source_t* pdsDataSource = NULL);
	void Release();
	
protected:
	volatile data_source_t m_dsDataSource;
	hIO m_io;
	PRLocker m_cs;
};

class recv_t : public send_t
{
public:
	recv_t();
	virtual ~recv_t();

public:
	tERROR Receive(data_source_t dsDataSource, tCHAR* buf, tINT len, HANDLE hStopEvent);
	tERROR Recv(data_source_t dsDataSource, tBOOL bRecvIsMultyLine, HANDLE hStopEvent, tBOOL bRecvOnce = cFALSE);

	tBOOL Is(tCHAR* szCommand1, tINT nCommandSize);
	tBOOL IsOK();
	tBOOL IsRETR();
	tBOOL IsRETR(OUT tDWORD& dwMessageNumber);
	tBOOL IsQUIT();
	tBOOL IsDELE(OUT tDWORD& dwMessageNumber);
	tBOOL IsLIST(OUT tDWORD& dwMessageNumber);
	tBOOL IsTOP (OUT tDWORD& dwMessageNumber);
	tBOOL NeedMore(tCHAR* end, tDWORD size);

public:
	enum retr_t
	{
		retr_no   = 0,
		retr_yes  = 1,
		retr_quit = 2,
	};
	tERROR WaitForRETR(
		tDWORD& dwRetrNum, 
		hIO& retr_io, 
		session_t hSessionCtx, 
		hOBJECT hTrafficMonitor, 
		hOBJECT& hPOP3Protocoller, 
		detect_code_t& dcPOP3DetectCode, 
		HANDLE hStopEvent, 
		retr_t& rCommand,
		tBOOL bWaitOnce = cFALSE);

	tERROR Translate(session_t hSessionCtx, hOBJECT hTrafficMonitor, hOBJECT& hPOP3Protocoller, detect_code_t& dcPOP3DetectCode, HANDLE hStopEvent);
};

class auto_io_t
{
public:
	auto_io_t(send_t& original) : m_original(original)
	{ m_io = m_original.Get(&m_dsDataSource);}
	virtual ~auto_io_t()
	{ m_original.Release(); }
	inline hIO io()
	{ return m_io; }
	inline data_source_t ds()
	{ return m_dsDataSource; }
protected:
	send_t& m_original;
	hIO m_io;
	data_source_t m_dsDataSource;

private:

};

#endif//_SEND_RECV_H_