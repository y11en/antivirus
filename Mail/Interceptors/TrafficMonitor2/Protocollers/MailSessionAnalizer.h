// MailSessionAnalyzer.h: interface for the CMailSessionProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MAILSESSIONANALYZER_H_)
#define _MAILSESSIONANALYZER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ThreadImp.h>
#include <TrafficProtocoller.h>
using namespace TrafficProtocoller;
#include "ProtocolAnalyzer.h"

#define COMMAND_DATA_SIZE	16

class CMailSessionProcessor : public CThreadImp
{
public:
	virtual ~CMailSessionProcessor()
	{
	}

	virtual detect_code_t Detect(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
	{
		return dcNeedMoreInfo;
	}

	virtual tERROR Process(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize) = 0;

	// параметры инициализации
	struct InitParams_t
	{
		hOBJECT hParent;
		session_t *pSession;         // Сессия, в рамках которой осуществляется публикация
		hOBJECT hTrafficMonitor;  // Указатель на объект ТраффикМонитора
	};

	virtual tERROR Init(InitParams_t* pInitParams) = 0;

protected:
	InitParams_t m_InitParams;
};


#endif // !defined(_MAILSESSIONANALYZER_H_)
