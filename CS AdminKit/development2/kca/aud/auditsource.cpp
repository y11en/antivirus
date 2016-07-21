/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file auditsource.cpp
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Реализация класса аудирования действий пользователя
 */

#include <string>
#include <map>

#include <avp/klids.h>

#include <std/trc/trace.h>
#include <std/thrstore/threadstore.h>
#include <std/err/klerrors.h>
#include <std/conv/klconv.h>

#include <common/locevents.h>

#include <transport/wat/authserver.h>
#include <transport/ev/eventsource.h>
#include <transport/tr/transport.h>
#include <transport/spl/securitypolicy.h>

#include <kca/aud/auditsource.h>
#include <kca/aud/audit_consts.h>


#ifdef _WIN32
#include <windows.h>
#endif

#define KLCS_MODULENAME L"KLAUD"

namespace KLAUD {
	
#ifdef _WIN32
	#define KLAUD_START_BRACKET {
	#define KLAUD_DOUBLE_BRACKET },{
	#define KLAUD_END_BRACKET }
#else
	#define KLAUD_START_BRACKET 
	#define KLAUD_DOUBLE_BRACKET ,
	#define KLAUD_END_BRACKET 
#endif

	static KLTRAP::Transport::StatusNotifyCallback g_pOldCallBack=NULL;

	class EventTemplate
	{		
	public:
		EventTemplate( AuditObjectType ot, ActionType at, 
				long eId, std::wstring	eName, 
				int	locDN, std::wstring	defDN,
				int	locFormat, std::wstring	defFormat,
				bool blocalizeProductNameFlag )
		{
			Init( ot, at, eId, eName, locDN, 
				defDN, locFormat, defFormat,
				blocalizeProductNameFlag );
		}

		EventTemplate( AuditObjectType ot, ActionType at, 
				long eId, std::wstring	eName, 
				int	locDN, std::wstring	defDN,
				int	locFormat, std::wstring	defFormat )
		{
			Init( ot, at, eId, eName, locDN, 
				defDN, locFormat, defFormat, 
				false );
		}

		void Init( AuditObjectType ot, ActionType at, 
				long eId, std::wstring	eName, 
				int	locDN, std::wstring	defDN,
				int	locFormat, std::wstring	defFormat,
				bool blocalizeProductNameFlag )
		{
			objectType = ot;
			actionType = at;		
			eventId = eId;
			eventName = eName;
			locEventDisplayName = locDN;
			defLocEventDisplayName = defDN;
			locFormatString = locFormat;
			defLocFormatString = defFormat;
			localizeProductNameFlag = blocalizeProductNameFlag;
		}		

		AuditObjectType objectType;
		ActionType		actionType;
		
		long			eventId;
		std::wstring	eventName;

		int				locEventDisplayName;
		std::wstring	defLocEventDisplayName;

		int				locFormatString;
		std::wstring	defLocFormatString;

		bool			localizeProductNameFlag;
	};

	EventTemplate c_eventsTemplates[] =
	{
		KLAUD_START_BRACKET	
			EventTemplate(
				AuditObjectTask, ActionAdded, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_TASK_ADDED_LOC_ID, KLAUD_EV_OBJECTMODIFY_TASK_ADDED_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectTask, ActionDeleted, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_TASK_DELETED_LOC_ID, KLAUD_EV_OBJECTMODIFY_TASK_DELETED_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectTask, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_TASK_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_TASK_MODIFY_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectTask, ActionRun, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RUN_LOC_ID, KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RUN_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectTask, ActionStop, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_TASK_STATE_CHANGED_STOP_LOC_ID, KLAUD_EV_OBJECT_TASK_STATE_CHANGED_STOP_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectTask, ActionSuspend, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_TASK_STATE_CHANGED_SUSPEND_LOC_ID, KLAUD_EV_OBJECT_TASK_STATE_CHANGED_SUSPEND_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectTask, ActionResume, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RESUME_LOC_ID, KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RESUME_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionAdded, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_GROUPTASK_ADDED_LOC_ID, KLAUD_EV_OBJECTMODIFY_GROUPTASK_ADDED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionDeleted, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_GROUPTASK_DELETED_LOC_ID, KLAUD_EV_OBJECTMODIFY_GROUPTASK_DELETED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_GROUPTASK_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_GROUPTASK_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionRun, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RUN_LOC_ID, KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RUN_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionStop, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_STOP_LOC_ID, KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_STOP_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionSuspend, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_SUSPEND_LOC_ID, KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_SUSPEND_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGroupTask, ActionResume, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RESUME_LOC_ID, KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RESUME_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionAdded, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_GLOBALTASK_ADDED_LOC_ID, KLAUD_EV_OBJECTMODIFY_GLOBALTASK_ADDED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionDeleted, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_GLOBALTASK_DELETED_LOC_ID, KLAUD_EV_OBJECTMODIFY_GLOBALTASK_DELETED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_GLOBALTASK_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_GLOBALTASK_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionRun, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RUN_LOC_ID, KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RUN_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionStop, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_STOP_LOC_ID, KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_STOP_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionSuspend, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_SUSPEND_LOC_ID, KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_SUSPEND_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectGlobalTask, ActionResume, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RESUME_LOC_ID, KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RESUME_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectPolicy, ActionAdded, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_POLICY_ADDED_LOC_ID, KLAUD_EV_OBJECTMODIFY_POLICY_ADDED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectPolicy, ActionDeleted, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_POLICY_DELETED_LOC_ID, KLAUD_EV_OBJECTMODIFY_POLICY_DELETED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectPolicy, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_POLICY_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_POLICY_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectProduct, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_PRODUCT_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_PRODUCT_MODIFY_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectProduct, ActionRun, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_RUN_LOC_ID, KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_RUN_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectProduct, ActionStop, 
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,
				KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_STOP_LOC_ID, KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_STOP_DEF,
				true
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectServer, ActionConnect, 
				KLAUD_SERVERCONNECT, c_evServerConnect,
				KLAUD_SERVERCONNECT_LOC_ID, KLAUD_SERVERCONNECT_DEF_LOC,
				KLAUD_SERVERCONNECT_FORMAT_LOC_ID, KLAUD_SERVERCONNECT_FORMAT_DEF				
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectServer, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_SERVER_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_SERVER_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectAdministrationGroup, ActionGroupAdded, 
				KLAUD_EV_ADMGROUP_CHANGED, c_evAdmGroup_Changed,
				KLAUD_EV_ADMGROUP_CHANGED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_DEF_LOC,
				KLAUD_EV_ADMGROUP_CHANGED_ADD_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_ADD_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectAdministrationGroup, ActionGroupDeleted, 
				KLAUD_EV_ADMGROUP_CHANGED, c_evAdmGroup_Changed,
				KLAUD_EV_ADMGROUP_CHANGED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_DEF_LOC,
				KLAUD_EV_ADMGROUP_CHANGED_DELETE_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_DELETE_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectAdministrationGroup, ActionModified, 
				KLAUD_EV_ADMGROUP_CHANGED, c_evAdmGroup_Changed,
				KLAUD_EV_ADMGROUP_CHANGED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_DEF_LOC,
				KLAUD_EV_ADMGROUP_CHANGED_MODIFY_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectAdministrationGroup, ActionHostAdded, 
				KLAUD_EV_ADMGROUP_CHANGED, c_evAdmGroup_Changed,
				KLAUD_EV_ADMGROUP_CHANGED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_DEF_LOC,
				KLAUD_EV_ADMGROUP_CHANGED_HOST_ADDED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_HOST_ADDED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectAdministrationGroup, ActionHostDeleted, 
				KLAUD_EV_ADMGROUP_CHANGED, c_evAdmGroup_Changed,
				KLAUD_EV_ADMGROUP_CHANGED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_DEF_LOC,
				KLAUD_EV_ADMGROUP_CHANGED_HOST_DELETED_LOC_ID, KLAUD_EV_ADMGROUP_CHANGED_HOST_DELETED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectHost, ActionModified, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_HOST_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_HOST_MODIFY_DEF
			)		
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectReport, ActionAdded, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_REPORT_ADDED_LOC_ID, KLAUD_EV_OBJECTMODIFY_REPORT_ADDED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectReport, ActionModified,
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_REPORT_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_REPORT_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectReport, ActionDeleted,
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_REPORT_DELETED_LOC_ID, KLAUD_EV_OBJECTMODIFY_REPORT_DELETED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectReport, ActionRun,
				KLAUD_EV_OBJECT_STATE_CHANGED, c_evTaskStateChanged,
				KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC,				
				KLAUD_EV_OBJECTMODIFY_REPORT_STATE_CHANGED_RUN_LOC_ID, KLAUD_EV_OBJECTMODIFY_REPORT_STATE_CHANGED_RUN_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectIPDiapason, ActionAdded, 
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_IPDIAPASON_ADDED_LOC_ID, KLAUD_EV_OBJECTMODIFY_IPDIAPASON_ADDED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectIPDiapason, ActionModified,
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_IPDIAPASON_MODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_IPDIAPASON_MODIFY_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectIPDiapason, ActionDeleted,
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECTMODIFY_IPDIAPASON_DELETED_LOC_ID, KLAUD_EV_OBJECTMODIFY_IPDIAPASON_DELETED_DEF
			)		
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectNetworkLst, ActionRun,
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECT_NETWORKLIST_STATE_CHANGED_LOC_ID, KLAUD_EV_OBJECT_NETWORKLIST_STATE_CHANGED_DEF
			)
		KLAUD_DOUBLE_BRACKET
			EventTemplate(
				AuditObjectSecurityPolicy, ActionModified,
				KLAUD_EV_OBJECTMODIFY, c_evObjectModified,
				KLAUD_EV_OBJECTMODIFY_LOC_ID, KLAUD_EV_OBJECTMODIFY_DEF_LOC,
				KLAUD_EV_OBJECT_SECURITYPOLICY_CHANGED_LOC_ID, KLAUD_EV_OBJECT_SECURITYPOLICY_CHANGED_DEF
			)
		KLAUD_END_BRACKET		
	};

	bool IsThreadAuditStopped();	

	static const int							c_userConnsMapLimit = 100;

	class AuditSourceImp : public KLSTD::KLBaseImpl<AuditSource>
	{
	public:
		AuditSourceImp();

		//\brief Определяет активен ли аудит для данного типа объектов
		bool IsAuditActive( AuditObjectType objectType );

		//\brief Функция нотификации об произошедшем действии
		virtual void NotifyAction( AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1 =	L"", const std::wstring param2 = L"" );

		//\brief Функция нотификации об произошедшем действии
		virtual void NotifyAction( AuditObjectType objectType, ActionType actionType,
			const int &objectNameLocId, const std::wstring &defObjectName, 
			const int param1LocId =	(-1), const std::wstring defParam1 =	L"",
			const int param2LocId = (-1), const std::wstring defParam2 =	L"" );

		//\brief Функция нотификации об подключении нового пользователя
		virtual void NotifyUserConnectAction( struct soap* soap );

		//\brief Функция нотификации об произошедшем действии
		virtual void NotifyAction( KLWAT::ClientContext *pClientContext, AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1 =	L"", const std::wstring param2 = L"" );

		void NotifyAction( const std::wstring &userAccountName, 
			AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1, const std::wstring param2 );

		virtual void NotifySSChanged( 
			const std::wstring &szwProduct, 
			const std::wstring &szwVersion,
            const std::wstring &szwSection  );

		virtual void SetLocalization( ObjectLocalization *pLoc );

	protected:

		std::wstring GetUserAccountName();

		std::wstring LocalizeProductName( const std::wstring &productName );
		
		KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pCricSec;
				
		typedef std::map<std::wstring,KLTRAP::Transport::ConnectionFullName> UserConnectionsMap;
		UserConnectionsMap							m_userConnsMap;		

		KLSTD::CAutoPtr<ObjectLocalization>			m_pLoc;
	};

	AuditSourceImp::AuditSourceImp()
	{
		KLSTD_CreateCriticalSection( &m_pCricSec );
	}

	//\brief Определяет активен ли аудит для данного типа объектов
	bool AuditSourceImp::IsAuditActive( AuditObjectType objectType )
	{
		return true;
	}

	//\brief Функция нотификации об произошедшем действии
	void AuditSourceImp::NotifyAction( KLWAT::ClientContext *pClientContext, 
			AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1, const std::wstring param2 )
	{
		KLWAT::ClientContext *authClntContext = pClientContext;
		if ( authClntContext==NULL ) return;

		std::wstring userAccountName;

		if ( authClntContext->GetPermissions()!=NULL )
		{			
			KLSTD::CAutoPtr<KLSPL::Permissions> perms;
			perms = authClntContext->GetPermissions();

			userAccountName = perms->GetUserAccountName();

			NotifyAction( userAccountName, objectType, actionType,
				objectName, param1, param2 );

			return;
		}

		KLSTD::CAutoPtr<KLWAT::AccessTokenHolder> pAccessTokenHolder;
        {
			KLSTD::CAutoPtr<KLWAT::AuthServer> pServer;
	        KLWAT_GetGlobalAuthServer(&pServer);
            pAccessTokenHolder = pServer->GetAccessCheckToken(authClntContext->GetClientIdentity());
        };

		if ( pAccessTokenHolder->GetType()!=KLWAT::atOS ) 
		{
			return;
		}
		

#if defined(_WIN32) && !defined(KLSTD_WIN9X)

		DWORD dwBufSize = 0;
		
		::GetTokenInformation( pAccessTokenHolder->GetOSAccessToken(), TokenUser, NULL, 0, & dwBufSize );
		DWORD dwLastError = ::GetLastError();
		if( dwLastError != ERROR_INSUFFICIENT_BUFFER )
		{
			return;
		}

		KLSTD::CArrayPointer<BYTE> pUserTokenBuffer;
		PTOKEN_OWNER pTokenUser;

		pUserTokenBuffer = (BYTE*)malloc( dwBufSize * sizeof(BYTE) );
		pTokenUser = (PTOKEN_OWNER)((BYTE*)pUserTokenBuffer);

		BOOL bAPIRes =
			::GetTokenInformation(
                pAccessTokenHolder->GetOSAccessToken(),
                TokenUser, pUserTokenBuffer, dwBufSize, & dwBufSize );

		if( ! bAPIRes )
		{
			return;
		}

		KLSTD_USES_CONVERSION;

		BOOL RetBln = FALSE;
		DWORD usernameLength = 255;
		DWORD domainnameLength = 255;
		UINT  RetVal = 0;		
		SID_NAME_USE  snu;

		KLSTD::CArrayPointer<TCHAR> lpDomainName;
		lpDomainName = (LPTSTR)malloc(255);
		memset(lpDomainName,0,255);
		KLSTD::CArrayPointer<TCHAR> lpUserName;
		lpUserName = (LPTSTR)malloc(255);
		memset(lpUserName,0,255);
        
		RetBln = LookupAccountSid(NULL,
			pTokenUser->Owner,
			lpUserName,
			&usernameLength,
			lpDomainName,
			&domainnameLength,
			&snu);
		if(!RetBln)
		{
			return;
		}

		userAccountName = KLSTD_T2W( lpUserName );
		std::wstring domainName = KLSTD_T2W( lpDomainName );
		if ( userAccountName.find(domainName)==std::wstring::npos)
		{
			userAccountName = domainName + L"\\" + userAccountName;
		}
#endif
		NotifyAction( userAccountName, objectType, actionType,
			objectName, param1, param2 );
	}

	//\brief Функция нотификации об произошедшем действии
	void AuditSourceImp::NotifyAction( AuditObjectType objectType, ActionType actionType,
			const int &objectNameLocId, const std::wstring &defObjectName, 
			const int param1LocId, const std::wstring defParam1,
			const int param2LocId, const std::wstring defParam2 )
	{
		KLERR_BEGIN
		{
			std::wstring objName, param1, param2;

			objName = KLLOC::LoadLocString( objectNameLocId, defObjectName.c_str() );

			if ( param1LocId>=0 )
				param1 = KLLOC::LoadLocString( param1LocId, defParam1.c_str() );
			else param1 = defParam1;

			if ( param2LocId>=0 )
				param2 = KLLOC::LoadLocString( param2LocId, defParam2.c_str() );
			else param2 = defParam2;
	
			NotifyAction( objectType, actionType,
				objName, param1, param2 );
		}
		KLERR_ENDT(2);
	}

	void AuditSourceImp::NotifyAction( const std::wstring &userAccountName, 
			AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1, const std::wstring param2 )
	{
		if ( IsThreadAuditStopped() ) return;

		if ( userAccountName.empty() ) return;

		KLSTD::CAutoPtr<KLEV::Event> pEvent;
		KLSTD::CAutoPtr<KLPAR::Params> pEventBody;

		// находим подходящее событие
		EventTemplate *foundTemplate = NULL;
		for( int c = 0; c < KLSTD_COUNTOF(c_eventsTemplates); c++ )
		{
			if ( c_eventsTemplates[c].objectType==objectType && 
				c_eventsTemplates[c].actionType==actionType )
			{
				foundTemplate = &c_eventsTemplates[c];
				break;
			}
		}

		if ( foundTemplate==NULL ) return;

		std::wstring locParam2 = param2;
		if ( foundTemplate->localizeProductNameFlag )
		{
			locParam2 = LocalizeProductName( param2 );
		}

        KLLOC::PrepareEventBody(
							foundTemplate->eventName.c_str(),
                            foundTemplate->locEventDisplayName,
                            foundTemplate->defLocEventDisplayName.c_str(),
                            foundTemplate->locFormatString,
							foundTemplate->defLocFormatString.c_str(),
                            KLEVP_EVENT_SEVERITY_INFO,
                            &pEventBody,
                            objectName.c_str(),
                            param1.c_str(),
                            userAccountName.c_str(),
                            locParam2.c_str() );

		time_t now;
		time(&now);
		KLEV_CreateEvent( &pEvent, 
			KLPRCI::ComponentId( KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLCS_COMPONENT_SERVER, L"" ), 
			foundTemplate->eventName, pEventBody, now );

		KLEV_GetEventSource()->PublishEvent( pEvent );
	}

	void AuditSourceImp::NotifyAction( AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, const std::wstring param1, const std::wstring param2 )
	{
		if( KLSTD_GetGlobalThreadStore()->IsObjectHere(KLSPL_SEC_PERMISSIONS) )
		{
			KLSTD::CAutoPtr<KLSPL::Permissions> perms;

			KLSTD_GetGlobalThreadStore()->GetStoredObject(
					KLSPL_SEC_PERMISSIONS,
					(KLSTD::KLBase**)&perms);
			KLSTD_ASSERT_THROW(perms);


			NotifyAction( perms->GetUserAccountName(), objectType, actionType,
					objectName, param1, param2 );
		}
		else
		{
			if( KLSTD_GetGlobalThreadStore()->IsObjectHere(KLWAT_SEC_CONTEXT) )
			{	
				KLSTD::CAutoPtr<KLWAT::ClientContext> authClntContext;

				KLSTD_GetGlobalThreadStore()->GetStoredObject(
					KLWAT_SEC_CONTEXT,
					(KLSTD::KLBase**)&authClntContext);
				KLSTD_ASSERT_THROW(authClntContext);

				NotifyAction( authClntContext, objectType, actionType,
					objectName, param1, param2 );		
			}
		}
	}
	
	void AuditSourceImp::NotifyUserConnectAction( struct soap* soap )
	{
		if ( soap==NULL ) return;
		
		std::wstring userAccountName = GetUserAccountName();

		if ( userAccountName.empty() ) return;

		bool needAudit = true;
		{ 
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			KLTRAP::Transport::ConnectionFullName connName;
			KLTR_GetTransport()->GetConnectionName( soap, connName );

			UserConnectionsMap::iterator fit = m_userConnsMap.find( userAccountName );			
			if ( fit!=m_userConnsMap.end() )
			{
				KLTRAP::Transport::ConnectionFullName &userCconnName = fit->second;

				if ( KLTR_GetTransport()->IsConnectionActive( userCconnName.localComponentName.c_str(),
					userCconnName.remoteComponentName.c_str() ) ) needAudit = false;
			}
			if ( needAudit )
			{
				if ( m_userConnsMap.size()>c_userConnsMapLimit )
				{
					m_userConnsMap.erase(m_userConnsMap.begin());
				}

				m_userConnsMap[userAccountName] = connName;
				
			}
		}

		if ( needAudit )
		{
			NotifyAction( KLAUD::AuditObjectServer, KLAUD::ActionConnect, L"" );
		}
	}

	void AuditSourceImp::NotifySSChanged( 
			const std::wstring &szwProduct, 
			const std::wstring &szwVersion,
            const std::wstring &szwSection  )
	{
		
	}

	void AuditSourceImp::SetLocalization( ObjectLocalization *pLoc )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_pLoc = pLoc;
	}

	std::wstring AuditSourceImp::GetUserAccountName()
	{
		std::wstring userAccountName;
		
		if( KLSTD_GetGlobalThreadStore()->IsObjectHere(KLSPL_SEC_PERMISSIONS) )
		{
			KLSTD::CAutoPtr<KLSPL::Permissions> perms;

			KLSTD_GetGlobalThreadStore()->GetStoredObject(
					KLSPL_SEC_PERMISSIONS,
					(KLSTD::KLBase**)&perms);
			KLSTD_ASSERT_THROW(perms);

			userAccountName = perms->GetUserAccountName();
		}
		else
		{
			if( KLSTD_GetGlobalThreadStore()->IsObjectHere(KLWAT_SEC_CONTEXT) )
			{	
				KLSTD::CAutoPtr<KLWAT::ClientContext> authClntContext;

				KLSTD_GetGlobalThreadStore()->GetStoredObject(
					KLWAT_SEC_CONTEXT,
					(KLSTD::KLBase**)&authClntContext);
				KLSTD_ASSERT_THROW(authClntContext);

				KLSTD::CAutoPtr<KLWAT::AccessTokenHolder> pAccessTokenHolder;
				{
					KLSTD::CAutoPtr<KLWAT::AuthServer> pServer;
					KLWAT_GetGlobalAuthServer(&pServer);
					pAccessTokenHolder = pServer->GetAccessCheckToken(authClntContext->GetClientIdentity());
				};
				
				if ( pAccessTokenHolder->GetType()!=KLWAT::atOS ) 
				{
					if ( authClntContext->GetPermissions()!=NULL )
					{			
						KLSTD::CAutoPtr<KLSPL::Permissions> perms;
						perms = authClntContext->GetPermissions();

						userAccountName = perms->GetUserAccountName();
					}

					return userAccountName;
				}			

		#if defined(_WIN32) && !defined(KLSTD_WIN9X)

				DWORD dwBufSize = 0;
				
				::GetTokenInformation( pAccessTokenHolder->GetOSAccessToken(), TokenUser, NULL, 0, & dwBufSize );
				DWORD dwLastError = ::GetLastError();
				if( dwLastError != ERROR_INSUFFICIENT_BUFFER )
				{
					return userAccountName;
				}

				KLSTD::CArrayPointer<BYTE> pUserTokenBuffer;
				PTOKEN_OWNER pTokenUser;

				pUserTokenBuffer = (BYTE*)malloc( dwBufSize * sizeof(BYTE) );
				pTokenUser = (PTOKEN_OWNER)((BYTE*)pUserTokenBuffer);

				BOOL bAPIRes =
					::GetTokenInformation(
						pAccessTokenHolder->GetOSAccessToken(),
						TokenUser, pUserTokenBuffer, dwBufSize, & dwBufSize );

				if( ! bAPIRes )
				{
					return userAccountName;
				}

				KLSTD_USES_CONVERSION;

				BOOL RetBln = FALSE;
				DWORD usernameLength = 255;
				DWORD domainnameLength = 255;
				UINT  RetVal = 0;		
				SID_NAME_USE  snu;

				KLSTD::CArrayPointer<TCHAR> lpDomainName;
				lpDomainName = (LPTSTR)malloc(255);
				memset(lpDomainName,0,255);
				KLSTD::CArrayPointer<TCHAR> lpUserName;
				lpUserName = (LPTSTR)malloc(255);
				memset(lpUserName,0,255);
        
				RetBln = LookupAccountSid(NULL,
					pTokenUser->Owner,
					lpUserName,
					&usernameLength,
					lpDomainName,
					&domainnameLength,
					&snu);
				if(!RetBln)
				{
					return userAccountName;
				}

				userAccountName = KLSTD_T2W( lpUserName );
				std::wstring domainName = KLSTD_T2W( lpDomainName );
				if ( userAccountName.find(domainName)==std::wstring::npos)
				{
					userAccountName = domainName + L"\\" + userAccountName;
				}
		#endif
			}
		}

		return userAccountName;
	}

	std::wstring AuditSourceImp::LocalizeProductName( const std::wstring &productName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_pLoc==NULL ) return productName;
		
		std::wstring wstrProdLocName = productName;
		KLERR_IGNORE( wstrProdLocName = m_pLoc->LocalizeProductName( productName ) );

		return wstrProdLocName;		
	}

	int ThisStatusNotifyCallback(
			KLTRAP::Transport::ConnectionStatus status, 
			const wchar_t*              remoteComponentName,
            const wchar_t*              localComponentName,
			bool                        remoteConnFlag)
    {
		if ( status==KLTRAP::Transport::ComponentConnected )
		{
			KLPRCI::ComponentId trLocalNameComponentId;
			if ( KLTRAP::ConvertTransportNameToComponentId( trLocalNameComponentId,
				localComponentName ) )
			{
				if ( trLocalNameComponentId.componentName==KLCS_COMPONENT_SERVER &&
					remoteComponentName[0]!=0 )
				{
					if ( !KLTR_GetTransport()->IsLocalConnection(localComponentName,remoteComponentName) ) 
					{
						KLAUD_NOTIFY_SERVER_CONNECT(NULL);
					}
				}
			}			
		}
		
		if(g_pOldCallBack)
            return g_pOldCallBack(
                            status, 
                            remoteComponentName,
                            localComponentName,
                            remoteConnFlag);
        else
            return 0;
	}

	/*
		\brief Функции позволяет временно пропускать публикацию событий аудита
		на текущем потоке. Используется для выполнения служеюных действий 
	*/
	#define AUDIT_THREAD_STOPPER_SEED L"AUDIT_STOPPER"
	void StopThreadAudit()
	{
		KLERR_IGNORE(KLSTD_GetGlobalThreadStore()->AddObject( AUDIT_THREAD_STOPPER_SEED, NULL ));
	}

	void StartThreadAudit()
	{
		KLERR_IGNORE(KLSTD_GetGlobalThreadStore()->RemoveObject( AUDIT_THREAD_STOPPER_SEED ));
	}

	bool IsThreadAuditStopped()
	{
		bool res = false;
		KLERR_IGNORE(res=KLSTD_GetGlobalThreadStore()->IsObjectHere( AUDIT_THREAD_STOPPER_SEED ));
		return res;
	}
	
} // end namespace KLAUD

KLSTD::CAutoPtr<KLAUD::AuditSource> pGlobalAuditSource;

KLAUD::AuditSource *KLAUD_GetAuditSource()
{	
	KLASSERT(pGlobalAuditSource!=NULL);
	return pGlobalAuditSource;
}

IMPLEMENT_MODULE_INIT_DEINIT(KLAUD)

IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL, KLAUD)

	pGlobalAuditSource.Attach( new KLAUD::AuditSourceImp() );

//	KLAUD::g_pOldCallBack = KLTR_GetTransport()->SetStatusCallback(
//			KLAUD::ThisStatusNotifyCallback);

IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL, KLAUD)

	KLTR_GetTransport()->SetStatusCallback( KLAUD::g_pOldCallBack );
	pGlobalAuditSource = NULL;
		
IMPLEMENT_MODULE_DEINIT_END()
