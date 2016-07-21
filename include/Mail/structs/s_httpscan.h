#ifndef _HTTPSCAN_H_
#define _HTTPSCAN_H_

#include <AV/structs/s_avs.h>

#include <Mail/plugin/p_httpscan.h>
#include <Mail/structs/s_antispam.h>
#include <Mail/structs/s_httpprotocoller.h>

#include <ProductCore/structs/s_taskmanager.h>

typedef enum 
{
	hss_PROCESS_PARAMS,
	hss_CANCEL_PARAMS,
	hss_ASK_ACTION,
	hss_LAST
} hss_t;

//-----------------------------------------------------------------------------
// cHTTPScanSettings structure
struct cHTTPScanSettings : public cProtectionSettings
{
	cHTTPScanSettings():
		cProtectionSettings(),
		m_bCheckTrafficMonitorMessages(cTRUE),
		m_bCheckExternalPluginMessages(cFALSE),
		m_bCheckIncomingMessagesOnly(cFALSE)
	{
	}

	DECLARE_IID_SETTINGS( cHTTPScanSettings, cProtectionSettings, PID_HTTPSCAN );

	tBOOL		  m_bCheckTrafficMonitorMessages;
	tBOOL		  m_bCheckExternalPluginMessages;
	tBOOL		  m_bCheckIncomingMessagesOnly;
	cHTTPSettings m_HTTPSettings;

	cVector<cAsBwListRule> m_vTrustedSites;
	cVector<cAsBwListRule> m_vTrustedTypes;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cHTTPScanSettings, 0 )
	SER_BASE( cProtectionSettings,  0 )
	SER_VALUE( m_bCheckTrafficMonitorMessages, tid_BOOL,            "CheckTrafficMonitorMessages"  )
	SER_VALUE( m_bCheckExternalPluginMessages, tid_BOOL,            "CheckExternalPluginMessages"  )
	SER_VALUE( m_bCheckIncomingMessagesOnly,   tid_BOOL,            "CheckIncomingDataMessages"    )
	SER_VALUE( m_HTTPSettings,                 cHTTPSettings::eIID, "HTTPSettings"                 )
	SER_VECTOR(m_vTrustedSites,			       cAsBwListRule::eIID, "TrustedSites")
	SER_VECTOR(m_vTrustedTypes,			       cAsBwListRule::eIID, "TrustedTypes")
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
// cHTTPScanProcessParams structure
typedef enum
{
	ss_DONT_USE = 0,
	ss_PACKET_BEGIN,
	ss_PACKET_PROCESS,
	ss_PACKET_END,
	ss_LAST
} stream_state_t;
struct cHTTPScanProcessParams : public cSerializable
{
	cHTTPScanProcessParams():
		cSerializable(),
		m_nCheckAsStream(ss_DONT_USE),
		m_bGZIPped(cFALSE),
		m_StreamCtx(NULL)
		{}
			
	DECLARE_IID( cHTTPScanProcessParams, cSerializable, PID_HTTPSCAN, hss_PROCESS_PARAMS );

	tINT              m_nCheckAsStream;   // [IN]     Проверять в потоковом движке (stream_state_t)
	tBOOL             m_bGZIPped;         // [IN]     Эти данные запакованы?
	tPTR              m_StreamCtx;        // [IN/OUT] Контекст пакета (используется потоковым движком)
	                                      //          Создается в состоянии m_nCheckAsStream == ss_PACKET_BEGIN
	                                      //          Удаляется в состоянии m_nCheckAsStream == ss_PACKET_END
	                                      //          Используется в состоянии m_nCheckAsStream == ss_PACKET_PROCESS
	cDetectObjectInfo m_DetectObjectInfo; // [OUT]    Результат детектирования (возвращается из AVS)
	cScanProcessInfo  m_ScanProcessInfo;  // [OUT]    Результат обработки (возвращается из AVS)
};

IMPLEMENT_SERIALIZABLE_BEGIN( cHTTPScanProcessParams, 0 )
	SER_VALUE( m_nCheckAsStream,   tid_INT,                 "CheckAsStream"    )
	SER_VALUE( m_StreamCtx,        tid_PTR,                 "StreamCtx"        )
	SER_VALUE( m_DetectObjectInfo, cDetectObjectInfo::eIID, "DetectObjectInfo" )
	SER_VALUE( m_ScanProcessInfo,  cScanProcessInfo::eIID,  "ScanProcessInfo"  )
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
// cHTTPCancelSettings structure
typedef enum
{
	cc_SSL,
	cc_LAST,
} cancel_cause_t;

struct cHTTPCancelSettings : public cSerializable
{
	cHTTPCancelSettings():
		cSerializable()
		{}
			
	DECLARE_IID( cHTTPCancelSettings, cSerializable, PID_HTTPSCAN, hss_CANCEL_PARAMS );

	cStringObj  m_sURL;          // Имя объекта
	tDWORD      m_dwCancelCause;  // Причина отказа (cancel_cause_t)
};

IMPLEMENT_SERIALIZABLE_BEGIN( cHTTPCancelSettings, 0 )
	SER_VALUE( m_sURL,          tid_STRING_OBJ, "URL" )
	SER_VALUE( m_dwCancelCause, tid_DWORD,      "CancelCause" )
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
// cHTTPAskObjectAction structure
struct cHTTPAskObjectAction : public cAskObjectAction
{
	cHTTPAskObjectAction():
		cAskObjectAction()
		{}
			
	DECLARE_IID( cHTTPAskObjectAction, cAskObjectAction, PID_HTTPSCAN, hss_ASK_ACTION );

};

IMPLEMENT_SERIALIZABLE_BEGIN( cHTTPAskObjectAction, 0 )
	SER_BASE(  cAskObjectAction,  0 )
IMPLEMENT_SERIALIZABLE_END()
#endif//_HTTPSCAN_H_
