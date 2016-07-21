// SessionData.h: interface for the CSMTPSessionAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SMTPSESSIONANALYZER_H_)
#define _SMTPSESSIONANALYZER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Mail/structs/s_mc.h>

#include "../mailsessionanalizer.h"
#include "../../SimpleBufferWindow.h"

#define COMMAND_DATA_SIZE	16

class CSMTPSessionAnalyzer : public CMailSessionProcessor
{
public:
	CSMTPSessionAnalyzer();
	virtual ~CSMTPSessionAnalyzer();

	detect_code_t Detect(
		data_source_t dsDataSource, 
		tBYTE* pData, 
		tDWORD dwDataSize
		);
	tERROR Process(
		data_source_t dsDataSource, 
		tBYTE* pData, 
		tDWORD dwDataSize
		);

	tERROR Init(InitParams_t* pInitParams);
	tERROR _Run();

private:
	tERROR Check(OUT cMCProcessParams& mcParams, OUT cStringObj& szMessageName);
	tERROR ClearData();
	hIO m_Data;
	detect_code_t m_dcResult;
	CSimpleBufferWindow* m_pClientData;
	CSimpleBufferWindow* m_pServerData;
	tQWORD m_pos;
	tDWORD m_dwCommandsFound;
	tDWORD m_dwLastCommand;
};

#endif // !defined(_SMTPSESSIONANALYZER_H_)
