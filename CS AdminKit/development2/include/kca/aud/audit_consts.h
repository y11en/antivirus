/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file audit_consts.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Константы модуля ауирования
 * 
 */

#ifndef KLAUD_AUDIT_CONSTS_H
#define KLAUD_AUDIT_CONSTS_H

#include <transport/ev/ev_general.h>

namespace KLAUD
{
	// Events

	const wchar_t c_evServerConnect[]=L"KLAUD_EV_SERVERCONNECT";
    
    #define KLAUD_SERVERCONNECT  5010L
    #define KLAUD_SERVERCONNECT_SEVERITY    KLEVP_EVENT_SEVERITY_INFO

	#define KLAUD_SERVERCONNECT_LOC_ID		4114L
	#define KLAUD_SERVERCONNECT_DEF_LOC		L"Audit (connect to server)"

    // %3 - username
	#define KLAUD_SERVERCONNECT_FORMAT_LOC_ID	4115L
    #define KLAUD_SERVERCONNECT_FORMAT_DEF		L"User '%3' connected to server."

	

	const wchar_t c_evObjectModified[]=L"KLAUD_EV_OBJECTMODIFY";
    
    #define KLAUD_EV_OBJECTMODIFY  5020L
    #define KLAUD_EV_OBJECTMODIFY_SEVERITY    KLEVP_EVENT_SEVERITY_INFO

	#define KLAUD_EV_OBJECTMODIFY_LOC_ID		4116L
	#define KLAUD_EV_OBJECTMODIFY_DEF_LOC		L"Audit (object modified)"


    // %1 - object name, %2 - product name, %4 - version name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_TASK_MODIFY_LOC_ID	4117L
    #define KLAUD_EV_OBJECTMODIFY_TASK_MODIFY_DEF	L"Task '%1' on product '%2' was modified by user '%3'."	
	#define KLAUD_EV_OBJECTMODIFY_TASK_ADDED_LOC_ID		4118L
    #define KLAUD_EV_OBJECTMODIFY_TASK_ADDED_DEF	L"Task '%1' for product '%2' was added by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_TASK_DELETED_LOC_ID	4119L
	#define KLAUD_EV_OBJECTMODIFY_TASK_DELETED_DEF	L"Task '%1' for product '%2' was deleted by user '%3'."

	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_GROUPTASK_MODIFY_LOC_ID	4124L
    #define KLAUD_EV_OBJECTMODIFY_GROUPTASK_MODIFY_DEF	L"Group task '%1' was modified by user '%3'."	
	#define KLAUD_EV_OBJECTMODIFY_GROUPTASK_ADDED_LOC_ID	4125L
    #define KLAUD_EV_OBJECTMODIFY_GROUPTASK_ADDED_DEF	L"Group task '%1' was added by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_GROUPTASK_DELETED_LOC_ID	4126L
	#define KLAUD_EV_OBJECTMODIFY_GROUPTASK_DELETED_DEF	L"Group task '%1' was deleted by user '%3'."
	
	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_POLICY_MODIFY_LOC_ID		4127L
    #define KLAUD_EV_OBJECTMODIFY_POLICY_MODIFY_DEF		L"Policy '%1' was modified by user '%3'."	
	#define KLAUD_EV_OBJECTMODIFY_POLICY_ADDED_LOC_ID		4128L
    #define KLAUD_EV_OBJECTMODIFY_POLICY_ADDED_DEF		L"Policy '%1' was added by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_POLICY_DELETED_LOC_ID		4129L
	#define KLAUD_EV_OBJECTMODIFY_POLICY_DELETED_DEF	L"Policy '%1' was deleted by user '%3'."

		// _LOC_ID		5003L
	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_PRODUCT_MODIFY_LOC_ID	4130L
    #define KLAUD_EV_OBJECTMODIFY_PRODUCT_MODIFY_DEF		L"Properties of product '%2' was modified by user '%3'."

	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_SERVER_MODIFY_LOC_ID		4131L
    #define KLAUD_EV_OBJECTMODIFY_SERVER_MODIFY_DEF			L"Properties of server was modified by user '%3'."
	
	
	const wchar_t c_evTaskStateChanged[]=L"KLAUD_EV_TASK_STATE_CHANGED";
    
    #define KLAUD_EV_OBJECT_STATE_CHANGED 5040L
    #define KLAUD_EV_OBJECT_STATE_CHANGED_SEVERITY    KLEVP_EVENT_SEVERITY_INFO

	#define KLAUD_EV_OBJECT_STATE_CHANGED_LOC_ID		4132L
	#define KLAUD_EV_OBJECT_STATE_CHANGED_DEF_LOC		L"Audit (object's state changed)"

	// %1 - object name, %2 - product name, %4 - version name, %3 - username
	#define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RUN_LOC_ID	4133L
    #define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RUN_DEF		L"Task '%1' on product '%2' was started by user '%3'."	
	#define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_STOP_LOC_ID	4134L
    #define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_STOP_DEF		L"Task '%1' on product '%2' was stopped by user '%3'."
	#define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_SUSPEND_LOC_ID	4135L
	#define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_SUSPEND_DEF	L"Task '%1' on product '%2' was suspended by user '%3'."
	#define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RESUME_LOC_ID	4136L
	#define KLAUD_EV_OBJECT_TASK_STATE_CHANGED_RESUME_DEF	L"Task '%1' on product '%2' was resumed by user '%3'."

	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RUN_LOC_ID		4137L
    #define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RUN_DEF			L"Group task '%1' was started by user '%3'."	
	#define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_STOP_LOC_ID		4138L
    #define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_STOP_DEF		L"Group task '%1' was stopped by user '%3'."
	#define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_SUSPEND_LOC_ID	4139L
	#define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_SUSPEND_DEF		L"Group task '%1' was suspended by user '%3'."
	#define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RESUME_LOC_ID	4140L
	#define KLAUD_EV_OBJECT_GROUPTASK_STATE_CHANGED_RESUME_DEF		L"Group task '%1' was resumed by user '%3'."

	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_RUN_LOC_ID		5049L
    #define KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_RUN_DEF			L"Product '%2' was started by user '%3'."		
	#define KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_STOP_LOC_ID		5050L
    #define KLAUD_EV_OBJECT_PRODUCT_STATE_CHANGED_STOP_DEF			L"Product '%2' was stopped by user '%3'."

	const wchar_t c_evAdmGroup_Changed[]=L"KLAUD_EV_ADMGROUP_CHANGED";
    
    #define KLAUD_EV_ADMGROUP_CHANGED 5060L
    #define KLAUD_EV_ADMGROUP_CHANGED_SEVERITY    KLEVP_EVENT_SEVERITY_INFO

	#define KLAUD_EV_ADMGROUP_CHANGED_LOC_ID		4143L
	#define KLAUD_EV_ADMGROUP_CHANGED_DEF_LOC		L"Audit (admistration group)"

	// %1 - object name, %2 - host name, %3 - username
	#define KLAUD_EV_ADMGROUP_CHANGED_MODIFY_LOC_ID	4144L
    #define KLAUD_EV_ADMGROUP_CHANGED_MODIFY_DEF	L"Adminstration group '%1' was modified by user '%3'."	
	#define KLAUD_EV_ADMGROUP_CHANGED_ADD_LOC_ID	4145L
    #define KLAUD_EV_ADMGROUP_CHANGED_ADD_DEF		L"Adminstration group '%1' was added by user '%3'."	
	#define KLAUD_EV_ADMGROUP_CHANGED_DELETE_LOC_ID	4146L
    #define KLAUD_EV_ADMGROUP_CHANGED_DELETE_DEF	L"Adminstration group '%1' was deleted by user '%3'."			
	#define KLAUD_EV_ADMGROUP_CHANGED_HOST_ADDED_LOC_ID			4147L
	#define KLAUD_EV_ADMGROUP_CHANGED_HOST_ADDED_DEF			L"Host '%2' was added to adminstration group '%1' by user '%3'."
	#define KLAUD_EV_ADMGROUP_CHANGED_HOST_DELETED_LOC_ID		4148L
	#define KLAUD_EV_ADMGROUP_CHANGED_HOST_DELETED_DEF			L"Host '%2' was deleted from adminstration group '%1' by user '%3'."



	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_HOST_MODIFY_LOC_ID		4149L
    #define KLAUD_EV_OBJECTMODIFY_HOST_MODIFY_DEF			L"Properties of host '%1' was modified by user '%3'."


	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_GLOBALTASK_MODIFY_LOC_ID			4180L
    #define KLAUD_EV_OBJECTMODIFY_GLOBALTASK_MODIFY_DEF				L"Global task '%1' was modified by user '%3'."	
	#define KLAUD_EV_OBJECTMODIFY_GLOBALTASK_ADDED_LOC_ID			4181L
    #define KLAUD_EV_OBJECTMODIFY_GLOBALTASK_ADDED_DEF				L"Global task '%1' was added by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_GLOBALTASK_DELETED_LOC_ID			4182L
	#define KLAUD_EV_OBJECTMODIFY_GLOBALTASK_DELETED_DEF			L"Global task '%1' was deleted by user '%3'."

	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RUN_LOC_ID		4183L
    #define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RUN_DEF		L"Global task '%1' was started by user '%3'."	
	#define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_STOP_LOC_ID	4184L
    #define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_STOP_DEF		L"Global task '%1' was stopped by user '%3'."
	#define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_SUSPEND_LOC_ID	4185L
	#define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_SUSPEND_DEF	L"Global task '%1' was suspended by user '%3'."
	#define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RESUME_LOC_ID	4186L
	#define KLAUD_EV_OBJECT_GLOBALTASK_STATE_CHANGED_RESUME_DEF		L"Global task '%1' was resumed by user '%3'."
	
	// %1 - object name, %3 - username
	#define KLAUD_EV_OBJECTMODIFY_REPORT_MODIFY_LOC_ID				4190L
	#define KLAUD_EV_OBJECTMODIFY_REPORT_MODIFY_DEF					L"Report '%1' was modified by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_REPORT_ADDED_LOC_ID				4191L
	#define KLAUD_EV_OBJECTMODIFY_REPORT_ADDED_DEF					L"Report '%1' was added by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_REPORT_DELETED_LOC_ID				4192L
	#define KLAUD_EV_OBJECTMODIFY_REPORT_DELETED_DEF				L"Report '%1' was deleted by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_REPORT_STATE_CHANGED_RUN_LOC_ID	4193L
	#define KLAUD_EV_OBJECTMODIFY_REPORT_STATE_CHANGED_RUN_DEF		L"Report '%1' was generated by user '%3'."

	// %1 - object name, %3 - username	
	#define KLAUD_EV_OBJECTMODIFY_IPDIAPASON_ADDED_LOC_ID			4194L
	#define KLAUD_EV_OBJECTMODIFY_IPDIAPASON_ADDED_DEF				L"Subnet '%1' was added by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_IPDIAPASON_MODIFY_LOC_ID			4195L
	#define KLAUD_EV_OBJECTMODIFY_IPDIAPASON_MODIFY_DEF				L"Subnet '%1' was modified by user '%3'."
	#define KLAUD_EV_OBJECTMODIFY_IPDIAPASON_DELETED_LOC_ID			4196L
	#define KLAUD_EV_OBJECTMODIFY_IPDIAPASON_DELETED_DEF			L"Subnet '%1' was deleted by user '%3'."

	// %1 - network list name, %2 - taskaname, %3 - username, %4 - host name
	#define KLAUD_EV_OBJECT_NETWORKLIST_STATE_CHANGED_LOC_ID		4197L
	#define KLAUD_EV_OBJECT_NETWORKLIST_STATE_CHANGED_DEF			L"Network list '%1': user '%3' added task '%2' for item(s) from host '%4'."
	
	// %1 - group name, %3 - username
	#define KLAUD_EV_OBJECT_SECURITYPOLICY_CHANGED_LOC_ID			4300L
	#define KLAUD_EV_OBJECT_SECURITYPOLICY_CHANGED_DEF				L"User '%3' changed the security rights for administration group '%1'."

		#define KLAUD_EV_NETWORK_LIST_TASK_RESTORE_LOC_ID			4198L
		#define KLAUD_EV_NETWORK_LIST_TASK_RESTORE_DEF				L"Restore"
		#define KLAUD_EV_NETWORK_LIST_TASK_DELETE_LOC_ID			4199L
		#define KLAUD_EV_NETWORK_LIST_TASK_DELETE_DEF				L"Delete"
		#define KLAUD_EV_NETWORK_LIST_TASK_SCAN_LOC_ID				4200L
		#define KLAUD_EV_NETWORK_LIST_TASK_SCAN_DEF					L"Scan"

		#define KLAUD_EV_NETWORK_LIST_QUARANTINE_LOC_ID				4201L
		#define KLAUD_EV_NETWORK_LIST_QUARANTINE_DEF				L"Quarantine"
		#define KLAUD_EV_NETWORK_LIST_BACKUP_LOC_ID					4202L
		#define KLAUD_EV_NETWORK_LIST_BACKUP_DEF					L"Backup"



	

} // end namespace KLAUD

#endif // KLAUD_AUDIT_CONSTS_H
