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

#ifndef _S_MC_OE_H_
#define _S_MC_OE_H_

#include <Mail/plugin/p_outlookplugintask.h>
#include <ProductCore/structs/s_taskmanager.h>

//-----------------------------------------------------------------------------

struct cMCOESettings : public cTaskSettings
{
	cMCOESettings():
		cTaskSettings(),
		m_bCheckIncoming(cTRUE),
		m_bCheckOutgoing(cTRUE),
		m_bCheckOnOutlookStartup(cTRUE)
		{};
	
	DECLARE_IID_SETTINGS( cMCOESettings, cTaskSettings, PID_OUTLOOKPLUGINTASK );

	tBOOL	m_bCheckIncoming;
	tBOOL	m_bCheckOutgoing;
	tBOOL	m_bCheckOnOutlookStartup;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cMCOESettings, 0 )
	SER_BASE( cTaskSettings,  0 )
	SER_VALUE( m_bCheckIncoming,         tid_BOOL, "CheckIncoming"  )
	SER_VALUE( m_bCheckOutgoing,         tid_BOOL, "CheckOutgoing"  )
	SER_VALUE( m_bCheckOnOutlookStartup, tid_BOOL, "CheckOnStartup" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cMCOEStatistics: public cTaskStatistics 
{
	cMCOEStatistics():
		m_dwMailCount(0)
		{};

	DECLARE_IID_STATISTICS( cMCOEStatistics, cTaskStatistics, PID_OUTLOOKPLUGINTASK );

	tDWORD	m_dwMailCount;	// Количество перехваченных сообщений
};


IMPLEMENT_SERIALIZABLE_BEGIN( cMCOEStatistics, 0 )
	SER_BASE( cTaskStatistics,  0 )
	SER_VALUE( m_dwMailCount, tid_DWORD, "MailCount" )
IMPLEMENT_SERIALIZABLE_END( );


//-----------------------------------------------------------------------------

#endif//_S_MC_TRAFFICMONITOR_H_
