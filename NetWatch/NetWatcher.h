#ifndef idl_impl_NetWatcher_h
#define idl_impl_NetWatcher_h

#include <NetWatch/plugin/p_NetWatch.h>
#if !defined(__cplusplus) || defined(_USE_VTBL)
#define MACRO_NETWATCHER_ID 0xbd7a
static const ImplementationId NetWatcher_IMPLID = MACRO_NETWATCHER_ID;
#define MACRO_NETWATCHER_PGINTERFACE_COMPATIBILITY mPROPERTY_MAKE_LOCAL(pTYPE_DWORD, 0x69090)
static const PropertyId NetWatcher_pgINTERFACE_COMPATIBILITY = MACRO_NETWATCHER_PGINTERFACE_COMPATIBILITY;
static const tVERSION NetWatcher_Version = 1;
struct iNetWatcherVtbl;
typedef struct iNetWatcherVtbl iNetWatcherVtbl;

typedef struct tag_hNetWatcher {
	const iNetWatcherVtbl* vtbl;
	const iSYSTEMVtbl* sys;
} *hNetWatcher;
typedef tERROR (pr_call *fnpNetWatcher_SetSettings) (hNetWatcher _this, const cSerializable* settings);
typedef tERROR (pr_call *fnpNetWatcher_GetSettings) (hNetWatcher _this, cSerializable* settings);
typedef tERROR (pr_call *fnpNetWatcher_AskAction) (hNetWatcher _this, Task_tActionId actionId, cSerializable* actionInfo);
typedef tERROR (pr_call *fnpNetWatcher_SetState) (hNetWatcher _this, TaskManager_tTaskRequestState state);
typedef tERROR (pr_call *fnpNetWatcher_GetStatistics) (hNetWatcher _this, cSerializable* statistics);
typedef tERROR (pr_call *fnpNetWatcher_Dummy) (hNetWatcher _this, tDWORD param1);
struct iNetWatcherVtbl {
	fnpNetWatcher_SetSettings	SetSettings;
	fnpNetWatcher_GetSettings	GetSettings;
	fnpNetWatcher_AskAction	AskAction;
	fnpNetWatcher_SetState	SetState;
	fnpNetWatcher_GetStatistics	GetStatistics;
	fnpNetWatcher_Dummy	Dummy;
};
#define CALL_NetWatcher_SetSettings(_this, settings)	((_this)->vtbl->SetSettings((_this), settings))
#define CALL_NetWatcher_GetSettings(_this, settings)	((_this)->vtbl->GetSettings((_this), settings))
#define CALL_NetWatcher_AskAction(_this, actionId, actionInfo)	((_this)->vtbl->AskAction((_this), actionId, actionInfo))
#define CALL_NetWatcher_SetState(_this, state)	((_this)->vtbl->SetState((_this), state))
#define CALL_NetWatcher_GetStatistics(_this, statistics)	((_this)->vtbl->GetStatistics((_this), statistics))
#define CALL_NetWatcher_Dummy(_this, param1)	((_this)->vtbl->Dummy((_this), param1))
extern tERROR pr_call NetWatcher_ObjectInitDone(hNetWatcher _this );
extern tERROR pr_call NetWatcher_ObjectPreClose(hNetWatcher _this );
extern tERROR pr_call NetWatcher_ObjectClose(hNetWatcher _this );
extern tERROR pr_call NetWatcher_MsgReceive(hNetWatcher _this );
extern tERROR pr_call NetWatcher_Register(hROOT root);
typedef struct tag_tNetWatcher_Data tNetWatcher_Data;
#define DATA_STRUCT_NetWatcher_FIELDS \
	hOBJECT m_tm;\
	tDWORD m_task_state;\
	tDWORD m_task_session_id;\
	tDWORD m_task_id;\
	hOBJECT m_task_persistent_storage;\
	tDWORD m_task_parent_id;\

typedef struct tag_hi_NetWatcher {	
	const iNetWatcherVtbl* 	vtbl;
	const iSYSTEMVTBL*  sys;
	tNetWatcher_Data*          data;
} *hi_NetWatcher;
extern tERROR pr_call NetWatcher_SetSettings (hi_NetWatcher _this, const cSerializable* settings);
extern tERROR pr_call NetWatcher_GetSettings (hi_NetWatcher _this, cSerializable* settings);
extern tERROR pr_call NetWatcher_AskAction (hi_NetWatcher _this, Task_tActionId actionId, cSerializable* actionInfo);
extern tERROR pr_call NetWatcher_SetState (hi_NetWatcher _this, TaskManager_tTaskRequestState state);
extern tERROR pr_call NetWatcher_GetStatistics (hi_NetWatcher _this, cSerializable* statistics);
extern tERROR pr_call NetWatcher_Dummy (hi_NetWatcher _this, tDWORD param1);



#else
struct pr_abstract NetWatcher : public NetWatch {
	static const PluginId         PID    = NetWatchImpl_PLUGINID;
	static const ImplementationId ImplID = 0xbd7a;
	static const VendorId         VID    = 0xbd7a;
	static const PropertyId IDL_NAME(pgINTERFACE_COMPATIBILITY) = mPROPERTY_MAKE_LOCAL(pTYPE_DWORD, 0x69090);
	static const tVERSION Version = 1;
	virtual tERROR pr_call SetSettings(const cSerializable* settings) = 0;
	virtual tERROR pr_call GetSettings(cSerializable* settings) = 0;
	virtual tERROR pr_call AskAction(Task::tActionId actionId, cSerializable* actionInfo) = 0;
	virtual tERROR pr_call SetState(TaskManager::tTaskRequestState state) = 0;
	virtual tERROR pr_call GetStatistics(cSerializable* statistics) = 0;
	virtual tERROR pr_call Dummy(tDWORD param1) = 0;
};

struct pr_abstract IDL_NAME(cNetWatcher) : public NetWatcher, public iObj {
	typedef IDL_NAME(cNetWatcher) Base;
	OBJ_IMPL( IDL_NAME(cNetWatcher) );
	static iINTERNAL i_vtbl;
	static tDATA e_vtbl[6];
	inline void *__cdecl operator new(size_t, void *_)	{ return _; }
	inline void __cdecl operator delete(void *, void *) {return; }
	static const size_t ActualObjectSize;
	static tERROR pr_call ObjectNew( hOBJECT object, tBOOL construct );
	static tERROR pr_call Register(hROOT root);
	virtual tERROR pr_call ObjectInitDone();
	virtual tERROR pr_call ObjectPreClose();
	virtual tERROR pr_call ObjectClose();
	virtual tERROR pr_call MsgReceive();
	hOBJECT m_tm;
	tDWORD m_task_state;
	tDWORD m_task_session_id;
	tDWORD m_task_id;
	hOBJECT m_task_persistent_storage;
	tDWORD m_task_parent_id;
};

#define CPP_VTBL_NETWATCHER_DEFINE(Scope) \
	CPP_DEFINE_IMPLEMENTATION(Scope)\
	iINTERNAL IDL_NAME(cNetWatcher)::i_vtbl = {\
		(tIntFnRecognize)(NULL),\
		(tIntFnObjectNew)(Scope::ObjectNew),\
		(tIntFnObjectInit)(NULL),\
		(tIntFnObjectInitDone)CAST_FROM_MEMBER_FN(Scope::ObjectInitDone),\
		(tIntFnObjectCheck)(NULL),\
		(tIntFnObjectPreClose)CAST_FROM_MEMBER_FN(Scope::ObjectPreClose),\
		(tIntFnObjectClose)CAST_FROM_MEMBER_FN(Scope::ObjectClose),\
		(tIntFnChildNew)(NULL),\
		(tIntFnChildInitDone)(NULL),\
		(tIntFnChildClose)(NULL),\
		(tIntFnMsgReceive)CAST_FROM_MEMBER_FN(Scope::MsgReceive),\
		(tIntFnIFaceTransfer)(NULL),\
	};\
	tDATA IDL_NAME(cNetWatcher)::e_vtbl[] = {\
		CAST_FROM_MEMBER_FN(Scope::SetSettings),\
		CAST_FROM_MEMBER_FN(Scope::GetSettings),\
		CAST_FROM_MEMBER_FN(Scope::AskAction),\
		CAST_FROM_MEMBER_FN(Scope::SetState),\
		CAST_FROM_MEMBER_FN(Scope::GetStatistics),\
		CAST_FROM_MEMBER_FN(Scope::Dummy),\
	};\

extern tERROR pr_call NetWatcher_Register(hROOT root);



#endif

#endif //idl_impl_NetWatcher_h

