/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	s_spamtest.h
*		\brief	Здесь задаются типы настроек сервиса спамтеста
*		
*		\author Vitaly DVi Denisov
*		\date	07.09.2005
*		
*		
*		
*		
*/		

#ifndef _S_SPAMTEST_H_
#define _S_SPAMTEST_H_

#include <ProductCore/structs/s_taskmanager.h>
#include <Mail/plugin/p_spamtest.h>


//-----------------------------------------------------------------------------
//

struct cSpamtestSettings : public cTaskSettings
{
	cSpamtestSettings() :
		cTaskSettings(),
		m_szBaseFolder("%Bases%"),
		m_bUseGSG(cTRUE),
		m_bUsePDB(cTRUE),
		m_bUseRecentTerms(cTRUE)
	{}
		
	DECLARE_IID_SETTINGS( cSpamtestSettings, cTaskSettings, PID_SPAMTEST );
	cStringObj m_szBaseFolder;
	tBOOL      m_bUseGSG;
	tBOOL      m_bUsePDB;
	tBOOL      m_bUseRecentTerms;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cSpamtestSettings, 0 )
	SER_BASE( cTaskSettings, 0 )
	SER_VALUE(  m_szBaseFolder, tid_STRING_OBJ, "BaseFolder" )
	SER_VALUE(  m_bUseGSG,      tid_BOOL,       "UseGSG" )
	SER_VALUE(  m_bUsePDB,      tid_BOOL,       "UsePDB" )
	SER_VALUE(  m_bUseRecentTerms, tid_BOOL,    "UseRecentTerms" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cSpamtestParams : public cSerializable
{
	cSpamtestParams():
		cSerializable(),
		m_os(NULL),
		m_result(0),
		m_facility(0),
		m_bUseGSG(cTRUE),
		m_bUsePDB(cTRUE),
		m_bUseRecentTerms(cTRUE)
	{}
	
	DECLARE_IID( cSpamtestParams, cSerializable, PID_SPAMTEST, 1 );
	hOBJECT m_os;
	tUINT   m_result;
	tDWORD  m_facility;
	tBOOL   m_bUseGSG;
	tBOOL   m_bUsePDB;
	tBOOL	m_bUseRecentTerms;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cSpamtestParams, 0 )
//	SER_VALUE( m_os,       tid_OBJECT, "object" ) // будем передавать через контекст для возможности работы spamtest в отдельном процессе
	SER_VALUE( m_result,   tid_UINT,   "result" )
	SER_VALUE( m_facility, tid_DWORD,  "facility" )
	SER_VALUE( m_bUseGSG,  tid_BOOL,   "UseGSG" )
	SER_VALUE( m_bUsePDB,  tid_BOOL,   "UsePDB" )
	SER_VALUE( m_bUseRecentTerms, tid_BOOL, "UseRecentTerms" )
IMPLEMENT_SERIALIZABLE_END( );



#endif//_S_SPAMTEST_H_
