/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	trafficmonitor_types.h
*		\brief	Здесь задаются типы настроек перехватчика траффик-монитора
*		
*		\author Vitaly DVi Denisov
*		\date	11.03.2004 17:32:41
*		
*		Example:	
*		
*		
*		
*/		

#ifndef _S_MC_TRAFFICMONITOR_H_
#define _S_MC_TRAFFICMONITOR_H_

#include <Extract/plugin/p_memmodscan.h>
#include <Mail/plugin/p_trafficmonitor.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs/s_ip.h>

enum enTrafficMonitorStructTypes
{
	tmst_cTmPort = 0,
	tmst_cTmExclude,
	tmst_cTrafficMonitorAskBeforeStop,
	tmst_cTrafficAskAction,
	tmst_cTrafficSelfAskAction,
};

//-----------------------------------------------------------------------------

struct cTmPort : public cSerializable
{
	cTmPort() :
		m_bEnabled(cTRUE),
		m_nPort(0)
	{}
	
	DECLARE_IID( cTmPort, cSerializable, PID_TRAFFICMONITOR, tmst_cTmPort );

	tBOOL      m_bEnabled;
	tWORD      m_nPort;
	cStringObj m_sDescription;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTmPort, 0 )
	SER_VALUE( m_bEnabled,     tid_BOOL,       "Enabled" )
	SER_VALUE( m_nPort,        tid_WORD,       "Port" )
	SER_VALUE( m_sDescription, tid_STRING_OBJ, "Description" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cTmExclude : public cSerializable
{
	cTmExclude() :
		m_bEnabled(cTRUE),
		m_nHost_(0),
		m_nPort_(0),
		m_bSSLOnly(cFALSE)
	{}
	
	DECLARE_IID( cTmExclude, cSerializable, PID_TRAFFICMONITOR, tmst_cTmExclude );

    void pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 6) // pre-сIP version
        {
            if (m_nHost_)
                m_Hosts.push_back(cIP(m_nHost_));
            if (m_nPort_)
                m_Ports.push_back(m_nPort_);
        }
    }

    void pr_call finalizeRegDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 6) // pre-сIP version
        {
            if (m_nHost_)
                m_Hosts.push_back(cIP(m_nHost_));
            if (m_nPort_)
                m_Ports.push_back(m_nPort_);
        }
    }

	tBOOL      m_bEnabled;
	tDWORD     m_nHost_;       // deprecated
	tWORD      m_nPort_;       // deprecated
	cStringObj m_sImagePath;   // Полное имя файла
	cStringObj m_sDescription;
	tBOOL      m_bSSLOnly;     // Проверять, но не расшифровывать трафик
    cVector<cIP>    m_Hosts;   // IP-адреса сервера
    cVector<tWORD>  m_Ports;   // Порты сервера
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTmExclude, 0 )
	SER_VALUE( m_bEnabled,     tid_BOOL,       "Enabled" )
	SER_VALUE( m_nHost_,       tid_DWORD,      "Host" )
	SER_VALUE( m_nPort_,       tid_WORD,       "Port" )
	SER_VALUE( m_sImagePath,   tid_STRING_OBJ, "ImagePath" )
	SER_VALUE( m_sDescription, tid_STRING_OBJ, "Description" )
	SER_VALUE( m_bSSLOnly,     tid_BOOL,       "SSLOnly" )
    SER_VECTOR( m_Hosts,       cIP::eIID,      "Hosts" )
    SER_VECTOR( m_Ports,       tid_WORD,       "Ports" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

enum enDecodeSSL
{
	dssl_NO    = 0,
	dssl_YES   = 1,
	dssl_AskMe = 2,
};

struct cTrafficMonitorSettings : public cTaskSettings
{
	cTrafficMonitorSettings() :
		m_bScanTraffic(cTRUE),
		m_bAllPorts(cFALSE),
		m_nDecodeSSL(dssl_NO),
		m_szSSLCacheFolder("%Data%"),
        m_bUseKavsend( cFALSE )
	{};
	
	DECLARE_IID_SETTINGS( cTrafficMonitorSettings, cTaskSettings, PID_TRAFFICMONITOR );

	tBOOL               m_bScanTraffic;
	tBOOL               m_bAllPorts;		// Проверять ВСЕ порты
	cVector<cTmPort>    m_aPorts;			// Выбранные для проверки порты
	cVector<cTmExclude> m_aExcludes;		// Исключения из проверки
	tDWORD				m_nDecodeSSL;		// Расшифровывать SSL? (enDecodeSSL)
	cStringObj			m_szSSLCacheFolder;	// Папка для кеширования SSL
    tBOOL               m_bUseKavsend;      // Схема перехвата = KAVSEND
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTrafficMonitorSettings, 0 )
	SER_BASE( cTaskSettings,  0 )
	SER_VALUE( m_bScanTraffic,     tid_BOOL,         "ScanTraffic" )
	SER_VECTOR( m_aPorts,          cTmPort::eIID,    "Ports" )
	SER_VECTOR( m_aExcludes,       cTmExclude::eIID, "Excludes" )
	SER_VALUE( m_nDecodeSSL,       tid_DWORD,        "DecodeSSL" )
	SER_VALUE( m_szSSLCacheFolder, tid_STRING_OBJ,   "SSLCacheFolder" )
	SER_VALUE( m_bAllPorts,        tid_BOOL,         "AllPorts" )
    SER_VALUE( m_bUseKavsend,      tid_BOOL,         "UseKavsend" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cTrafficMonitorStatistics: public cTaskStatistics 
{
	cTrafficMonitorStatistics():
		m_dwSessionsOpened(0),
		m_dwSessionCount(0),
		cTaskStatistics()
		{};

	DECLARE_IID_STATISTICS( cTrafficMonitorStatistics, cTaskStatistics, PID_TRAFFICMONITOR );

	tDWORD	m_dwSessionsOpened;	// 
	tDWORD	m_dwSessionCount;	// 
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTrafficMonitorStatistics, 0 )
	SER_BASE( cTaskStatistics,  0 )
	SER_VALUE( m_dwSessionsOpened, tid_DWORD, "SessionsOpened" )
	SER_VALUE( m_dwSessionCount,   tid_DWORD, "SessionCount"   )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cTrafficMonitorAskBeforeStop : public cTaskHeader
{
	cTrafficMonitorAskBeforeStop():
		cTaskHeader(),
		m_nTimeout(10)
		{};

	DECLARE_IID( cTrafficMonitorAskBeforeStop, cTaskHeader, PID_TRAFFICMONITOR, tmst_cTrafficMonitorAskBeforeStop );
	
	tDWORD                      m_nTimeout;
	cTrafficMonitorStatistics	m_statistics;	// 
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTrafficMonitorAskBeforeStop, 0 )
	SER_BASE( cTaskHeader,  0 )
	SER_VALUE( m_nTimeout,   tid_DWORD,                       "Timeout" )
	SER_VALUE( m_statistics, cTrafficMonitorStatistics::eIID, "statistics" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//
// Команды AskAction для общения трафикМонитора и GUI
//
#define pmSESSION_UNACCESS		0xb3acc48b   //!< Невозможно установить соединение - оповестить пользователя
#define pmSESSION_SWITCH_TO_SSL	0x203f2960   //!< В сессии обнаружен SSL. Расшифровать?
#define pmSESSION_DISCONNECT	0x9aac7ff2   //!< Соединение, установленное программой с сервером, не будет обрабатываться

//////////////////////////////////////////////////////////////////////////

struct cTrafficAskAction : public cTaskHeader
{
	cTrafficAskAction():
		cTaskHeader(),
		m_nTimeout(10000),
		m_nServerPort(0),
		m_dwClientPID(0),
		m_nAction(0)
		{};

	DECLARE_IID( cTrafficAskAction, cTaskHeader, PID_TRAFFICMONITOR, tmst_cTrafficAskAction );
	
	tDWORD     m_nTimeout;        //!< Сколько времени держать AskAction?
	cStringObj m_szServerName;    //!< Имя сервера
	tINT       m_nServerPort;     //!< Порт сервера
	tDWORD     m_dwClientPID;     //!< PID процесса, инициировавшего соединение
	tDWORD     m_nAction;         //!< ID действия
	cStringObj m_szImagePath;     //!< Полное имя запускаемого файла процесса
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTrafficAskAction, 0 )
	SER_BASE( cTaskHeader,  0 )
	SER_VALUE( m_nTimeout,     tid_DWORD,      "Timeout" )
	SER_VALUE( m_szServerName, tid_STRING_OBJ, "ServerName" )
	SER_VALUE( m_nServerPort,  tid_INT,        "ServerPort" )
	SER_VALUE( m_dwClientPID,  tid_DWORD,      "ClientPID" )
	SER_VALUE( m_nAction,      tid_DWORD,      "ActionId" )
	SER_VALUE( m_szImagePath,  tid_STRING_OBJ, "ImagePath" )
IMPLEMENT_SERIALIZABLE_END( );

enum enUseSSLRule
{
	usr_NO    = 0,
	usr_YES   = 1,
	usr_Temporary = 2,
};

typedef cTrafficAskAction cProtocollerAskBeforeStop;

struct cTrafficSelfAskAction : public cTrafficAskAction
{
	cTrafficSelfAskAction() :
		cTrafficAskAction(),
		m_bShowUseRule(cTRUE),
		m_nUseRule(usr_NO),
		m_bDecodeSSL(cFALSE),
		m_bValid(cTRUE),
		m_bSSLEstablished(cTRUE),
		m_bNeedToInform(cTRUE)
		{};

	DECLARE_IID( cTrafficSelfAskAction, cTrafficAskAction, PID_TRAFFICMONITOR, tmst_cTrafficSelfAskAction );

	cStringObj m_szSSLCacheFolder;	// Папка для кеширования SSL
	tBOOL      m_bShowUseRule;		// Показывать "создать правило"
	tDWORD     m_nUseRule;			// Информация хранится в правиле (enUseSSLRule)
	tBOOL      m_bDecodeSSL;		// Расшифровывать SSL?
	tBOOL      m_bValid;			// Валидный ли сертификат выдал сервер (если принято решение расшифровывать SSL)?
	tBOOL      m_bSSLEstablished;	// Удалось ли установить SSL-коннект (если принято решение расшифровывать SSL)?
	cStringObj m_szCertName;		// Имя сертификата (если удалось получить его с сервера)
	cStringObj m_szOrigFileName;	// Имя файла оригинального сертификата в кеше (если удалось получить его с сервера)
	cStringObj m_szFakeFileName;	// Имя файла фейкового сертификата в кеше (если удалось его сгенерировать)
	tBOOL      m_bNeedToInform;		// Нужно ли обо всем этом информировать GUI?
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTrafficSelfAskAction, 0 )
	SER_BASE( cTrafficAskAction,  0 )
	SER_VALUE( m_szSSLCacheFolder, tid_STRING_OBJ,   "SSLCacheFolder" )
	SER_VALUE( m_bShowUseRule,     tid_BOOL,         "ShowUseRule" )
	SER_VALUE( m_nUseRule,         tid_DWORD,        "UseRule" )
	SER_VALUE( m_bDecodeSSL,       tid_BOOL,         "DecodeSSL" )
	SER_VALUE( m_bValid,           tid_BOOL,         "Valid" )
	SER_VALUE( m_bSSLEstablished,  tid_BOOL,         "SSLEstablished" )
	SER_VALUE( m_bNeedToInform,    tid_BOOL,         "NeedToInform" )
	SER_VALUE( m_szCertName,       tid_STRING_OBJ,   "CertName" )
	SER_VALUE( m_szOrigFileName,   tid_STRING_OBJ,   "OrigFileName" )
	SER_VALUE( m_szFakeFileName,   tid_STRING_OBJ,   "FakeFileName" )
IMPLEMENT_SERIALIZABLE_END( );


//-----------------------------------------------------------------------------

#endif//_S_MC_TRAFFICMONITOR_H_
