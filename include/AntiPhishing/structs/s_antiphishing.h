#ifndef _antiphishing_task_structs_
#define _antiphishing_task_structs_

#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

#include <AntiPhishing/plugin/p_antiphishing.h>
#include <AV/structs/s_avs.h>
#include <ParentalControl/structs/s_urlflt.h>
#include <ProductCore/structs/s_taskmanager.h>

typedef enum
{
	e_cAntiPhishingRequest = 1,
	e_cAntiBannerRequest = 2,
} _antiphishing_structs_ids;


// URL Filtering Mode flags constants
#define UFM_USE_DATABASE      0x01 // use blacklist database (like anti-phishing does)
#define UFM_USE_WHITELIST     0x02 // use user-defined whitelist
#define UFM_USE_BLACKLIST     0x04 // use user-defined blacklist
#define UFM_DENY_UNKNOWN      0x08 // deny any URL not included in whitelist
#define UFM_DONT_DENY_DOMAIN  0x10 // dont't deny domain root request like "http://www.google.com/"

//-----------------------------------------------------------------------------
//! Settings

struct cAntiPhishingSettings : public cTaskSettings
{
        cAntiPhishingSettings(): cTaskSettings(),
			m_nFilterMsgClass(0),
			m_nUrlFilteringMode(0),
			m_bAskAction(cTRUE),
			m_bLoadHTTP(cFALSE)
        {}

        DECLARE_IID_SETTINGS(cAntiPhishingSettings, cTaskSettings, PID_ANTIPHISHING);
        cStringObj	m_sDataBase;
        cStringObj	m_sTextBlDB;
		cVector<cEnabledStrItem>  m_vWhiteList;
		cVector<cEnabledStrItem>  m_vBlackList;
		cVector<cEnabledStrItem>  m_vUserBlackList;
		tDWORD      m_nUrlFilteringMode; // see UFM_xxx flags
		tDWORD      m_nFilterMsgClass  ; // use this class with msg_URL_FILTERING_CHECK_URL 
		tBOOL       m_bAskAction;        // ask user action
		cAskObjectAction m_AskActionDef; // ask action defaults
		tBOOL       m_bLoadHTTP;         // load traffic monitor
        cStringObj	m_sDatabaseUpdCat;
		cVector<cEnabledStrItem>  m_vMutexes;
		tBOOL       m_bBlockEmbeddedingScripts;
        cStringObj	m_sGlobalTaskName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiPhishingSettings, 0 )
        SER_BASE(   cTaskSettings,       0 )
        SER_VALUE(  m_sDataBase,         tid_STRING_OBJ, "bdb" )
        SER_VALUE(  m_sTextBlDB,         tid_STRING_OBJ, "bdbt" )
		SER_VECTOR_STREAM( m_vWhiteList,        cEnabledStrItem::eIID, "wlist" )
		SER_VECTOR_STREAM( m_vBlackList,        cEnabledStrItem::eIID, "blist" )
		SER_VECTOR_STREAM( m_vUserBlackList,    cEnabledStrItem::eIID, "ublist" )
		SER_VALUE(  m_nUrlFilteringMode, tid_DWORD , "ufmode"  )
		SER_VALUE(  m_nFilterMsgClass,   tid_DWORD , "ufpmc" )
		SER_VALUE(  m_bAskAction,        tid_BOOL  , "ask" )
		SER_VALUE(  m_AskActionDef,      cAskObjectAction::eIID ,  "askdef")
		SER_VALUE(  m_bLoadHTTP,         tid_BOOL  , "loadhttp" )
		SER_VALUE(  m_sDatabaseUpdCat,   tid_STRING_OBJ , "updcat")
		SER_VECTOR( m_vMutexes,          cEnabledStrItem::eIID, "mutexes" )
		SER_VALUE(  m_bBlockEmbeddedingScripts, tid_BOOL  , "embscr" )
		SER_VALUE(  m_sGlobalTaskName,   tid_STRING_OBJ , "gtname")
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
//! Statistics

struct cAntiPhishingStatistics : public cTaskStatistics
{
        cAntiPhishingStatistics(): cTaskStatistics(),
                m_nURLsChecked(0),
                m_nURLsDetected(0),
				m_nURLsInDatabase(0)
        {}

        DECLARE_IID_STATISTICS(cAntiPhishingStatistics, cTaskStatistics, PID_ANTIPHISHING);

        tUINT             m_nURLsChecked;
        tUINT             m_nURLsDetected;
        tUINT             m_nURLsInDatabase;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiPhishingStatistics, 0 )
        SER_BASE( cTaskStatistics, 0 )
        SER_VALUE_M( nURLsChecked,     tid_UINT )
        SER_VALUE_M( nURLsDetected,    tid_UINT )
        SER_VALUE_M( nURLsInDatabase,  tid_UINT )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//! Check Request


struct cAntiPhishingCheckRequest : public cSerializable
{
        cAntiPhishingCheckRequest(): cSerializable(),
                m_sProcessName(),
                m_sURL(),
                m_sTemplate(),
				m_nPID(0),
				m_bInteractive(cTRUE),
				m_nTID(0)
        {}

        DECLARE_IID(cAntiPhishingCheckRequest, cSerializable, PID_ANTIPHISHING, e_cAntiPhishingRequest);

        cStringObj        m_sProcessName;
        cStringObj        m_sURL;
		cStringObj        m_sTemplate;     // template which caused errACCESS_DENIED
		tDWORD            m_nPID;
		tDWORD            m_nTID;
		tBOOL             m_bInteractive;
		cStringObj        m_sReferrer;     // url referring requiesting object
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiPhishingCheckRequest, 0 )
        SER_VALUE_M( sProcessName,       tid_STRING_OBJ)
        SER_VALUE_M( sURL,               tid_STRING_OBJ)
        SER_VALUE_M( sTemplate,          tid_STRING_OBJ)
        SER_VALUE_M( nPID,               tid_DWORD)
        SER_VALUE_M( bInteractive,       tid_BOOL)
        SER_VALUE_M( sReferrer,          tid_STRING_OBJ)
		SER_VALUE_M( nTID,               tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

/*
struct cAntiBannerRequest : public cSerializable
{
        cAntiBannerRequest(): cSerializable(),
                m_sProcessName(),
                m_sURL(),
                m_sTemplate(),
				m_nPID(0),
				m_bInteractive(cTRUE)
        {}

        DECLARE_IID(cAntiPhishingCheckRequest, cSerializable, PID_ANTIPHISHING, e_cAntiBannerRequest);

        cStringObj        m_sProcessName;
        cStringObj        m_sURL;
		cStringObj        m_sTemplate;     // template which caused errACCESS_DENIED
		tDWORD            m_nPID;
		tBOOL             m_bInteractive;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiPhishingCheckRequest, 0 )
        SER_VALUE_M( sProcessName,       tid_STRING_OBJ)
        SER_VALUE_M( sURL,               tid_STRING_OBJ)
        SER_VALUE_M( sTemplate,          tid_STRING_OBJ)
        SER_VALUE_M( nPID,               tid_DWORD)
        SER_VALUE_M( bInteractive,       tid_BOOL)
IMPLEMENT_SERIALIZABLE_END()
*/

#endif // _antiphishing_task_structs_

