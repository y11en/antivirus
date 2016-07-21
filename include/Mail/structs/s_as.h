#ifndef __S_AS_H
#define __S_AS_H

#include <ProductCore/structs/s_taskmanager.h>

#define PID_AS 421

//-----------------------------------------------------------------------------
// Anti-Spam Settings
struct cASSettings : public cTaskSettings
{
	cASSettings() :
		m_nPossibleSpamLimit(75),
		m_nSpamLimit(95),
		m_bScanOutlook(cTRUE),
		m_bCheckPOP3Traffic(cTRUE),
		m_nPOP3Port(110)
	{}
	
	DECLARE_IID( cASSettings, cTaskSettings, PID_AS, 1 );
			
	
	tINT				m_nPossibleSpamLimit;
	tINT				m_nSpamLimit;
	cStringObjVector	m_vecBlackList;
	cStringObjVector	m_vecWhiteList;
	
	// возможно, настройки m_bScanOutlook, m_bCheckPOP3Traffic и m_nPOP3Port
	// должны наследоваться от cMailSettings
	// так же поступить и с настройками MC
	tBOOL				m_bScanOutlook;
	tBOOL				m_bCheckPOP3Traffic;
	tWORD				m_nPOP3Port;

};

IMPLEMENT_SERIALIZABLE_BEGIN( cASSettings, 0 )
	SER_BASE( cTaskSettings,  0 )

	SER_VALUE( m_nPossibleSpamLimit,      tid_INT,          "PossibleSpamLimit" )
	SER_VALUE( m_nSpamLimit,              tid_INT,          "SpamLimit" )
	SER_VECTOR( m_vecBlackList,           tid_STRING_OBJ,   "BlackList" )
	SER_VECTOR( m_vecWhiteList,           tid_STRING_OBJ,   "WhiteList" )

	SER_VALUE( m_bScanOutlook,            tid_BOOL,          "ScanOutlook" )
	SER_VALUE( m_bCheckPOP3Traffic,       tid_BOOL,          "CheckPOP3Traffic" )
	SER_VALUE( m_nPOP3Port,               tid_WORD,          "POP3Port" )
IMPLEMENT_SERIALIZABLE_END()


#endif //__S_AS_H
