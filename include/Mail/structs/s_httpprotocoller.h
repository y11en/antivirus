#ifndef _HTTPPROTOCOLLER_H_
#define _HTTPPROTOCOLLER_H_

#include <Prague/pr_serializable.h>

#include <Mail/plugin/p_httpprotocoller.h>
#include <ProductCore/structs/s_settings.h>


//-----------------------------------------------------------------------------
// cHTTPScanSettings structure
struct cHTTPSettings : public cSerializable
{
	cHTTPSettings():
		m_bUseTimeout(cTRUE),
		m_dwTimeout(1),
		m_dwTimeoutPercent_NotUsed(100),
		m_bUseStreamProcessor(cFALSE),
		m_szStreamProcessorDB("%Bases%\\stream.kfb")
	{
	}

	DECLARE_IID_SETTINGS( cHTTPSettings, cSerializable, PID_HTTPPROTOCOLLER );

	tBOOL  m_bUseTimeout;
	tDWORD m_dwTimeout;
	tDWORD m_dwTimeoutPercent_NotUsed;
	tBOOL  m_bUseStreamProcessor;
	cStringObj m_szStreamProcessorDB;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cHTTPSettings, 0 )
	SER_VALUE( m_bUseTimeout,         tid_BOOL,       "UseTimeLimit" )
	SER_VALUE( m_dwTimeout,           tid_DWORD,      "TimeLimit" )
	SER_VALUE( m_dwTimeoutPercent_NotUsed,    tid_DWORD,      "TimeoutPercent" )
	SER_VALUE( m_bUseStreamProcessor, tid_BOOL,       "UseStreamProcessor" )
	SER_VALUE( m_szStreamProcessorDB, tid_STRING_OBJ, "StreamProcessorDB" )
IMPLEMENT_SERIALIZABLE_END()



#endif//_HTTPPROTOCOLLER_H_
