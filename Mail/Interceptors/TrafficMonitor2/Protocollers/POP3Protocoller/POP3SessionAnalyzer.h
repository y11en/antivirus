// SessionData.h: interface for the CPOP3SessionProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_POP3SESSIONANALIZER_H_)
#define _POP3SESSIONANALIZER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../MailSessionAnalizer.h"
#include "../../SimpleBufferWindow.h"
#include "NOOPThread.h"
#include "../MIMETransport.h"

#define COMMAND_DATA_SIZE	16

class CMailContent : public IContentItem
{
private:
    data_source_t   m_dsDataSource;
    content_t       m_ctContentType;
    hIO             m_hData;
    hIO             m_hOriginalData;
    tBOOL           m_bEncoded;

public:
	CMailContent( data_source_t dsDataSource, content_t ctContentType, hIO hOriginalData, tLONG dwSizeToCopy = -1 );
	virtual ~CMailContent();

	virtual data_source_t Source() const { return m_dsDataSource;  }
	virtual content_t Type()       const { return m_ctContentType; }
	virtual hIO DataIO()           const { return m_bEncoded ? m_hOriginalData : m_hData; }

	inline tERROR Encode()
	{
		if ( m_ctContentType != contentProtocolSpecific )
		{
			if ( m_bEncoded )
				return errOK;
			m_bEncoded = cTRUE;
			return ReplaceCRLFDotToCRLFDotDot( m_hData, m_hOriginalData );
		}
		return errOK;
	}
	
	inline tBOOL IsOK()
	{
        char data[1025] = {0};
        tDWORD dwRead = 0;

		if ( !m_hData )
			return cFALSE;
		if ( PR_FAIL( m_hData->SeekRead(&dwRead, 0, data, 1024) ) )
			return cFALSE;
		if ( !dwRead )
			return cFALSE;
		if ( 0 != memcmp(data, "+OK", 3) )
			return cFALSE;
		return cTRUE;
	}
};

class CPOP3SessionAnalyzer : public IProtocolAnalyzer
{
public:
	CPOP3SessionAnalyzer( hOBJECT hParent, session_t* pSession, hOBJECT hTrafficMonitor );
	virtual ~CPOP3SessionAnalyzer();

	virtual detect_code_t Detect(
		data_source_t   dsDataSource, 
		tBYTE*          pData, 
		tDWORD          dwDataSize
		);
	
	virtual tERROR EnqueueData(
		data_source_t   dsDataSource, 
		tBYTE*          pData, 
		tDWORD          dwDataSize
		);

	virtual tBOOL  HasContent() const;
	virtual tERROR ExtractContent( OUT IContentItem** ppMailContent, tBOOL bForce = cFALSE );
	virtual tERROR AssembleContent( IN IContentItem* pMailContent ) ;
	virtual tERROR ReleaseContent(  IN IContentItem* pMailContent ) ;

public:
	detect_code_t GetStatus();
	tBOOL IsInitedOK();

private:
	tERROR StartNOOPClentThread();
	tERROR StopNOOPClentThread();
	tERROR ClearData();
	
private:
	tERROR                  m_err;
	CMailContent*           m_pServerResponse;
	CMailContent*           m_pData;
	hIO                     m_Data;
	detect_code_t           m_dcResult;
	CSimpleBufferWindow*    m_pClientData;
	CSimpleBufferWindow*    m_pServerData;
	tQWORD                  m_pos;
	tBOOL CutServerResponse();

	tDWORD                  m_dwCommandsFound;
	CNOOPThread             m_hNOOPClientThread;
	hOBJECT                 m_hParent;
	session_t*              m_pSession;
	hOBJECT                 m_hTrafficMonitor;
};

class CNOOPServerThread : public CNOOPThread
{
public:
	CNOOPServerThread();
	virtual ~CNOOPServerThread();
	tERROR EnqueueData ( data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize );
	tERROR ExtractContent ( OUT IContentItem** ppMailContent );
	tERROR ReleaseContent(  IN IContentItem* pMailContent ) ;
private:
	hIO m_hData;
};

class CPOP3SessionProcessor : public CMailSessionProcessor
{
public:
	CPOP3SessionProcessor();
	virtual ~CPOP3SessionProcessor();

	detect_code_t Detect(
		data_source_t   dsDataSource, 
		tBYTE*          pData, 
		tDWORD          dwDataSize
		);
	tERROR  Process(
		data_source_t   dsDataSource, 
		tBYTE*          pData, 
		tDWORD          dwDataSize
		);

	tERROR  Init(InitParams_t* pInitParams);
	tERROR  _Run();
	void    SetTrafficMonitorHandle(hOBJECT hTrafficMonitor) { m_hTrafficMonitor = hTrafficMonitor; };
	tBOOL   HasTrafficMonitorHandle() { return m_hTrafficMonitor ? cTRUE:cFALSE; };

public:
	detect_code_t GetStatus();

private:
	void Flush();
	CPOP3SessionAnalyzer*   m_pAnalyzer;
	CMailContent*           m_pMailContent;
	hOBJECT                 m_hTrafficMonitor;
	CNOOPServerThread       m_hNOOPServerThread;

	tERROR Start();
	tBOOL m_bStarted;
};

#endif // !defined(_POP3SESSIONANALIZER_H_)
