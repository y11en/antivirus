#ifndef _DISPATCH_PROCESS_H_
#define _DISPATCH_PROCESS_H_

#include "mailwasher.h"
#include <SimpleBufferWindow.h>
#include <vector>
using std::vector;

struct ClearCallbackData : public session_t
{
	ClearCallbackData():
		dsDataSource(dsUnknown),
		pdcDetectCode(0),
		phDetectorHandle(0),
		session_t()
	{;};
	IN data_source_t dsDataSource;   //!< Источник данных
	OUT detect_code_t* pdcDetectCode;//!< Результат детектирования протокола
	OUT hOBJECT* phDetectorHandle;   //!< Хендл протоколлера
};

class CMailWasher; //forward declaration
class CDispatchProcess
{
public:
	CDispatchProcess(
		CMailWasher* pMailWasher,
		hOBJECT&   hParent,
		session_t& hSessionCtx,
		recv_t&    p_recv,
		send_t&    p_send,
		detect_code_t& dcPOP3DetectCode,
		hOBJECT&   hPOP3Protocoller
		);
	virtual ~CDispatchProcess();
	tERROR DispatchProcess(process_t* pProcessParams);
	tERROR DispatchProtocollerMessage(process_msg_t* ppmCommand);
private:
	tDWORD	CollectMarkedLetters();
	bool	RemoveNextLetter();
private:
	detect_code_t&		 m_dcPOP3DetectCode;
	data_source_t        m_dsDataSource;
	ClearCallbackData    m_ccdClient; 
	ClearCallbackData    m_ccdServer; 
	CSimpleBufferWindow* m_pBuf_Client;
	CSimpleBufferWindow* m_pBuf_Server;
	hOBJECT&             m_hPOP3Protocoller;
	recv_t&              m_recv;
	send_t&              m_send;
	hOBJECT&             m_hParent;
	CMailWasher*         m_pMailWasher;
	vector<tDWORD>		 m_lettersIdToDelete; // список идентификаторов писем необходимых к удалению
	bool				 m_bWaitServerDeleResponse;
};

#endif//_DISPATCH_PROCESS_H_