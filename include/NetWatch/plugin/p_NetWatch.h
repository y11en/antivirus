#ifndef idl_p_NetWatch_h
#define idl_p_NetWatch_h

#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_idl.h>
#include "Prague/idl/std_idl.h"
#include "ProductCore/idl/i_task.h"



#if !defined(__cplusplus) || defined(_USE_VTBL)


typedef struct tag_hNetWatch tag_hNetWatch;
typedef tag_hNetWatch* hNETWATCH;
	//	interface tag_hNetWatch definition
	#define MACRO_NETWATCH_ID 0x3ae0
	static const InterfaceId NetWatch_IID = MACRO_NETWATCH_ID;
	struct iNetWatchVtbl;
	typedef struct iNetWatchVtbl iNetWatchVtbl;
	
	typedef struct tag_hNetWatch {
		const iNetWatchVtbl* vtbl;
		const iSYSTEMVtbl* sys;
	} *hNetWatch;
	typedef tERROR (pr_call *fnpNetWatch_SetSettings) (hNetWatch _this, const cSerializable* settings);
	typedef tERROR (pr_call *fnpNetWatch_GetSettings) (hNetWatch _this, cSerializable* settings);
	typedef tERROR (pr_call *fnpNetWatch_AskAction) (hNetWatch _this, Task_tActionId actionId, cSerializable* actionInfo);
	typedef tERROR (pr_call *fnpNetWatch_SetState) (hNetWatch _this, TaskManager_tTaskRequestState state);
	typedef tERROR (pr_call *fnpNetWatch_GetStatistics) (hNetWatch _this, cSerializable* statistics);
	typedef tERROR (pr_call *fnpNetWatch_Dummy) (hNetWatch _this, tDWORD param1);
	struct iNetWatchVtbl {
		fnpNetWatch_SetSettings	SetSettings;
		fnpNetWatch_GetSettings	GetSettings;
		fnpNetWatch_AskAction	AskAction;
		fnpNetWatch_SetState	SetState;
		fnpNetWatch_GetStatistics	GetStatistics;
		fnpNetWatch_Dummy	Dummy;
	};
	#define CALL_NetWatch_SetSettings(_this, settings)	((_this)->vtbl->SetSettings((_this), settings))
	#define CALL_NetWatch_GetSettings(_this, settings)	((_this)->vtbl->GetSettings((_this), settings))
	#define CALL_NetWatch_AskAction(_this, actionId, actionInfo)	((_this)->vtbl->AskAction((_this), actionId, actionInfo))
	#define CALL_NetWatch_SetState(_this, state)	((_this)->vtbl->SetState((_this), state))
	#define CALL_NetWatch_GetStatistics(_this, statistics)	((_this)->vtbl->GetStatistics((_this), statistics))
	#define CALL_NetWatch_Dummy(_this, param1)	((_this)->vtbl->Dummy((_this), param1))
	//	end of interface tag_hNetWatch definition
	



#else  // !defined(__cplusplus) || defined(_USE_VTBL)


struct IDL_NAME(cNetWatch);
typedef IDL_NAME(cNetWatch)* hNETWATCH;
//	interface IDL_NAME(cNetWatch) definition
struct pr_abstract NetWatch : public Task {
	static const InterfaceId IID = 0x3ae0;
	virtual tERROR pr_call SetSettings(const cSerializable* settings) = 0;
	virtual tERROR pr_call GetSettings(cSerializable* settings) = 0;
	virtual tERROR pr_call AskAction(Task::tActionId actionId, cSerializable* actionInfo) = 0;
	virtual tERROR pr_call SetState(TaskManager::tTaskRequestState state) = 0;
	virtual tERROR pr_call GetStatistics(cSerializable* statistics) = 0;
	virtual tERROR pr_call Dummy(tDWORD param1) = 0;
};


#if !defined(PR_IDL_LEGACY)

	#define CALL_NetWatch_SetSettings(_this, settings)	((_this)->SetSettings(settings))
	#define CALL_NetWatch_GetSettings(_this, settings)	((_this)->GetSettings(settings))
	#define CALL_NetWatch_AskAction(_this, actionId, actionInfo)	((_this)->AskAction(actionId, actionInfo))
	#define CALL_NetWatch_SetState(_this, state)	((_this)->SetState(state))
	#define CALL_NetWatch_GetStatistics(_this, statistics)	((_this)->GetStatistics(statistics))
	#define CALL_NetWatch_Dummy(_this, param1)	((_this)->Dummy(param1))


#endif // !defined(PR_IDL_LEGACY)


struct pr_abstract IDL_NAME(cNetWatch) : public NetWatch, public iObj {
	OBJ_IMPL( IDL_NAME(cNetWatch) );
};
//	end of interface IDL_NAME(cNetWatch) definition




#endif  // !defined(__cplusplus) || defined(_USE_VTBL)


//	plugin NetWatchImpl definition
	#define MACRO_NETWATCHIMPL_ID 0x5d1
	static const PluginId NetWatchImpl_PLUGINID = MACRO_NETWATCHIMPL_ID;
	static const tVERSION NetWatchImpl_Version = 1;
//	end of plugin NetWatchImpl definition





#endif //idl_p_NetWatch_h



#ifdef IMPEX_TABLE_CONTENT




#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)


	#include <iface/impexhlp.h>
	
	


#endif




