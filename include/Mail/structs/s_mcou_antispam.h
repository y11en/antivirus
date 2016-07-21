/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	s_mcou_antispam.h
*		\brief	Структуры настроек mcou_antispam
*		
*		\author Vitaly DVi Denisov
*		\date	12.10.2004 15:39:29
*		
*		
*		
*/		



#ifndef _S_MCOU_ANTISPAM_H_
#define _S_MCOU_ANTISPAM_H_

#include <Prague/pr_serializable.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/i_objptr.h>

#include <Mail/structs/s_antispam.h>
#include <Mail/structs/s_as_trainsupport.h>

#include <mapidefs.h>


enum AntispamPluginSettingsId
{
	apsid_PluginRule = asstruct_LAST,
	apsid_PluginSettings,
	apsid_Request,
	apsid_PluginStatus,
	apsid_LAST
};

// Возможное действие над письмом
enum AntispamPluginAction_t
{
	AntispamPluginAction_Move,
	AntispamPluginAction_Copy,
	AntispamPluginAction_Delete,
	AntispamPluginAction_KeepAsIs
};

//! Правило обращения с письмом
struct AntispamPluginRule_t : public cSerializable
{
	AntispamPluginRule_t():
		AntispamPluginAction(AntispamPluginAction_KeepAsIs),
		bMarkAsRead(cFALSE),
		cSerializable()
		{;};
	DECLARE_IID(AntispamPluginRule_t, cSerializable, PID_ANTISPAMTASK, apsid_PluginRule);
	AntispamPluginAction_t AntispamPluginAction;	//!< Действие с письмом
	tBOOL bMarkAsRead;								//!< Помечать как прочитанное?
	FolderInfo_t FolderForAction;					//!< Папка для AntispamPluginAction_t
};
IMPLEMENT_SERIALIZABLE_BEGIN( AntispamPluginRule_t, 0 )
	SER_VALUE( AntispamPluginAction, tid_DWORD,          "AntispamPluginAction" )
	SER_VALUE( bMarkAsRead,	         tid_BOOL,           "MarkAsRead" )
	SER_VALUE( FolderForAction,      FolderInfo_t::eIID, "FolderForAction" )
IMPLEMENT_SERIALIZABLE_END()

//! На все письма, распознанные как спам, мы посылаем письмо-request. 
//! Если на него приходит положительный ответ (в теме письма указан Code), 
//! то данный адресат заносится в белый список.
struct Request_t : public cSerializable
{
	Request_t():
		szCode(""),
		cSerializable()
		{
			Now(&DateToForget);
		};
	DECLARE_IID(Request_t, cSerializable, PID_ANTISPAMTASK, apsid_Request);
	cStringObj szCode;			//!< Код, посланный в request'е
	tDATETIME DateToForget;		//!< Дата, после которой следует "забыть" про request
};
IMPLEMENT_SERIALIZABLE_BEGIN( Request_t, 0 )
	SER_VALUE( szCode,         tid_STRING_OBJ, "Code" )
	SER_VALUE( DateToForget,   tid_DATETIME,   "DateToForget" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<Request_t> RequestList_t;

enum check_algorithm_t
{
	chal_UseBackgroundScan = 0,
	chal_UseRule,
};

//! Настройки для процессирования
struct AntispamPluginSettings_t : public cSerializable
{
	AntispamPluginSettings_t():
		bUseRequestLogic(cFALSE),
		dwRequestStoreDaysCount(7),
		dwCheckAlgorithm(chal_UseBackgroundScan),
		cSerializable()
		{;};
	DECLARE_IID(AntispamPluginSettings_t, cSerializable, PID_ANTISPAMTASK, apsid_PluginSettings);
	AntispamPluginRule_t CertainSpamRule;
	AntispamPluginRule_t PossibleSpamRule;
	tBOOL bUseRequestLogic;
	tDWORD dwRequestStoreDaysCount;
	RequestList_t RequestList;
	tDWORD dwCheckAlgorithm;    // check_algorithm_t
};
IMPLEMENT_SERIALIZABLE_BEGIN( AntispamPluginSettings_t, 0 )
	SER_VALUE( CertainSpamRule,         AntispamPluginRule_t::eIID, "CertainSpamRule" )
	SER_VALUE( PossibleSpamRule,        AntispamPluginRule_t::eIID, "PossibleSpamRule" )
	SER_VALUE( bUseRequestLogic,        tid_BOOL,                   "UseRequestLogic" )
	SER_VALUE( dwRequestStoreDaysCount, tid_DWORD,                  "RequestStoreDaysCount" )
	SER_VECTOR(RequestList,	            Request_t::eIID,            "RequestList")
	SER_VALUE( dwCheckAlgorithm,        tid_DWORD,                  "CheckAlgorithm" )
IMPLEMENT_SERIALIZABLE_END()


struct cAntispamPluginStatus : public cSerializable
{
	cAntispamPluginStatus() :
		m_bKavStarted(cFALSE),
		m_bAsEnabled(cFALSE),
		m_bMcEnabled(cFALSE)
		{}
	
	DECLARE_IID(cAntispamPluginStatus, cSerializable, PID_ANTISPAMTASK, apsid_PluginStatus);
	
	tBOOL         m_bKavStarted;
	tBOOL         m_bAsEnabled;
	tBOOL         m_bMcEnabled;
};
IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamPluginStatus, 0 )
	SER_VALUE( m_bKavStarted,           tid_BOOL,                   "KavStarted" )
	SER_VALUE( m_bAsEnabled,            tid_BOOL,                   "AsEnabled" )
	SER_VALUE( m_bMcEnabled,            tid_BOOL,                   "McEnabled" )
IMPLEMENT_SERIALIZABLE_END()

#endif//_S_MCOU_ANTISPAM_H_
