	// AVP Prague stamp begin( Interface header,  )
// -------- Sunday,  22 April 2007,  16:34 --------
// File Name   -- (null)i_threadpool.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_threadpool__99aa762b_2ca5_4f7a_a0ef_e0e9f1d87e2c )
#define __i_threadpool__99aa762b_2ca5_4f7a_a0ef_e0e9f1d87e2c
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <string.h>
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// ThreadPool interface implementation
// Short comments -- пул потоков/очередь заданий
//    объект создающий и управляющий пулом потоков и назначаемых заданий
//    доступные варианты использования:
//    1. добавление задний на основном потоке,  обработка в контексте потока
//    2. добавление задний на основном потоке,  обработка в контексте потока, завершение задания на основнов потоке
//    3. выполнение с таймаутом для каждого существующего потока
//
//    есть механизм создания контекста потоков и контекста заданий
//
//    прим: возможно расширение - использование пула как контейнера заданий
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_THREADPOOL  ((tIID)(61010))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tTaskId; // тип уникального идентификатора задания
typedef tDWORD                         tThreadPriority; // тип приоритетов потоков
typedef tDWORD                         tThreadState; // тип состояний потоков
typedef tDWORD                         tThreadCallType; // тип вызова callback-функции
typedef tERROR (pr_call* tThreadCallback)(tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext);
typedef tERROR (pr_call* tThreadTaskCallback)(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen);
typedef tDWORD                         tThreadTaskEnum; //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// - состояния потоков и их задач
#define TP_THREADSTATE_THREAD_NOTFOUND ((tThreadState)(0)) // задание или поток не найдены
#define TP_THREADSTATE_THREAD_STARTING ((tThreadState)(1)) // состояние потока - старт (базовая инициализация)
#define TP_THREADSTATE_THREAD_BEFORE_INIT_ ((tThreadState)(2)) // состояние потока - перед вызовом callback-функции (TP_CBTYPE_THREAD_INIT_CONTEXT)
#define TP_THREADSTATE_THREAD_INITING  ((tThreadState)(3)) // состояние потока - поток создает/иницилизирует потоковый контекст (в обработке TP_CBTYPE_THREAD_INIT_CONTEXT)
#define TP_THREADSTATE_THREAD_AFTER_INIT ((tThreadState)(4)) // состояние потока - после вызовал callback-функцию (TP_CBTYPE_THREAD_INIT_CONTEXT)
#define TP_THREADSTATE_TASK_WAITING_THREAD ((tThreadState)(5)) // состояние потока - ожидание задания
#define TP_THREADSTATE_TASK_BEFORE_INIT ((tThreadState)(6)) // состояние потока - поток получил задание, перед вызовом callback-функции (TP_CBTYPE_TASK_INIT_CONTEXT)
#define TP_THREADSTATE_TASK_INITFAILED ((tThreadState)(7)) // состояние потока - инициализация заданием своего контектста завершилась с ошибкой. поток пропускает обработку задания
#define TP_THREADSTATE_TASK_PROCESSING_INIT ((tThreadState)(8)) // состояние потока - задание инициализирует свой контекст (в обработке TP_CBTYPE_TASK_INIT_CONTEXT)
#define TP_THREADSTATE_TASK_AFTER_INIT ((tThreadState)(9)) // состояние потока - перед выполнением задания, после вызова callback-функции (TP_CBTYPE_TASK_INIT_CONTEXT)
#define TP_THREADSTATE_TASK_PROCESSING ((tThreadState)(10)) // состояние потока - поток выполняет задание (TP_CBTYPE_TASK_PROCESS)
#define TP_THREADSTATE_TASK_BEFORE_DONE ((tThreadState)(11)) // состояние потока - поток выполнил задание. перед очисткой контекста и задания (TP_CBTYPE_TASK_DONE_CONTEXT)
#define TP_THREADSTATE_TASK_AFTER_DONE ((tThreadState)(12)) // состояние потока - поток освободил контекст и задание (TP_CBTYPE_TASK_DONE_CONTEXT). перед ожиданием нового задания
#define TP_THREADSTATE_THREAD_BEFORE_STOP ((tThreadState)(13)) // состояние потока - завершение работы (остановка пула или освобождение не используемых ресурсов)
#define TP_THREADSTATE_THREAD_STOPPING ((tThreadState)(14)) // состояние потока - вызов callback-функции для освобождения поточного контекста (TP_CBTYPE_THREAD_DONE_CONTEXT)
#define TP_THREADSTATE_THREAD_STOPPED  ((tThreadState)(15)) // состояние потока - поток остановлен

// - приоритеты потоков
#define TP_THREADPRIORITY_IDLE         ((tThreadPriority)(0)) //  --
#define TP_THREADPRIORITY_LOWEST       ((tThreadPriority)(1)) //  --
#define TP_THREADPRIORITY_BELOW_NORMAL ((tThreadPriority)(2)) //  --
#define TP_THREADPRIORITY_NORMAL       ((tThreadPriority)(3)) //  --
#define TP_THREADPRIORITY_ABOVE_NORMAL ((tThreadPriority)(4)) //  --
#define TP_THREADPRIORITY_TIME_CRITICAL ((tThreadPriority)(5)) //  --

// - типы вызовов callback-ов
#define TP_CBTYPE_THREAD_INIT_CONTEXT  ((tThreadCallType)(0)) // вызов инициализация контекста для потока. При возврате ошибки из инициализации базового потока пул считается не созданным
#define TP_CBTYPE_THREAD_DONE_CONTEXT  ((tThreadCallType)(1)) // освобождение контекста потока
#define TP_CBTYPE_TASK_INIT_CONTEXT    ((tThreadCallType)(2)) // инициализация контекста задания для потока
#define TP_CBTYPE_TASK_PROCESS         ((tThreadCallType)(3)) // выполнение задания
#define TP_CBTYPE_TASK_DONE_CONTEXT    ((tThreadCallType)(4)) // освобождение контекста задания
#define TP_CBTYPE_THREAD_YIELD         ((tThreadCallType)(5)) // пустой цикл поточной функции

// -- перебор заданий в очереди
#define TP_ENUM_START                  ((tThreadTaskEnum)(0)) // начать перебор
#define TP_ENUM_NEXT                   ((tThreadTaskEnum)(1)) // следующая задача
#define TP_ENUM_ABORT                  ((tThreadTaskEnum)(2)) // прекратить перебор
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTHREADPOOL;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iThreadPoolVtbl;
typedef struct iThreadPoolVtbl iThreadPoolVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cThreadPool;
	typedef cThreadPool* hTHREADPOOL;
#else
	typedef struct tag_hTHREADPOOL 
	{
		const iThreadPoolVtbl* vtbl; // pointer to the "ThreadPool" virtual table
		const iSYSTEMVtbl*     sys;  // pointer to the "SYSTEM" virtual table
	} *hTHREADPOOL;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( ThreadPool_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpThreadPool_AddTask)                 ( hTHREADPOOL _this, tTaskId* pTaskId, tThreadTaskCallback cbTaskCallback, tPTR pTaskData, tDWORD DataLen, tDWORD TaskPriority ); // -- Добавление задачи в очередь;
	typedef   tERROR (pr_call *fnpThreadPool_GetThreadState)          ( hTHREADPOOL _this, tThreadState* pState, tTaskId TaskId ); // -- возвращает статус потока/задачи;
	typedef   tERROR (pr_call *fnpThreadPool_GetThreadContext)        ( hTHREADPOOL _this, tTaskId TaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDataLen, tThreadState* pState ); // -- получить контекст задания;
	typedef   tERROR (pr_call *fnpThreadPool_ReleaseThreadContext)    ( hTHREADPOOL _this, tTaskId TaskId ); // -- освободить контекст задачи;
	typedef   tERROR (pr_call *fnpThreadPool_WaitForTaskDone)         ( hTHREADPOOL _this, tTaskId TaskId ); // -- дождаться выполнения указанного задания;
	typedef   tERROR (pr_call *fnpThreadPool_SetThreadPriorityByTask) ( hTHREADPOOL _this, tTaskId TaskId, tThreadPriority ThreadPriority ); // -- изменить приоритет потока для указанной задачи;
	typedef   tERROR (pr_call *fnpThreadPool_RemoveTaskFromQueue)     ( hTHREADPOOL _this, tTaskId TaskId ); // -- удалить задачу из очереди;
	typedef   tERROR (pr_call *fnpThreadPool_WaitTaskDonenotify)      ( hTHREADPOOL _this, tDWORD Timeout ); // -- дождатся выполнения задачи;
	typedef   tERROR (pr_call *fnpThreadPool_CreateSeparateThread)    ( hTHREADPOOL _this, tThreadTaskCallback cbSeparateFunc, tDWORD SeparateData ); // -- создание потока для индивидуального задания;
	typedef   tERROR (pr_call *fnpThreadPool_EnumTaskContext)         ( hTHREADPOOL _this, tThreadTaskEnum EnumMode, tTaskId* pTaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDatalen, tThreadState* pState ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iThreadPoolVtbl 
{
	fnpThreadPool_AddTask                  AddTask;
	fnpThreadPool_GetThreadState           GetThreadState;
	fnpThreadPool_GetThreadContext         GetThreadContext;
	fnpThreadPool_ReleaseThreadContext     ReleaseThreadContext;
	fnpThreadPool_WaitForTaskDone          WaitForTaskDone;
	fnpThreadPool_SetThreadPriorityByTask  SetThreadPriorityByTask;
	fnpThreadPool_RemoveTaskFromQueue      RemoveTaskFromQueue;
	fnpThreadPool_WaitTaskDonenotify       WaitTaskDonenotify;
	fnpThreadPool_CreateSeparateThread     CreateSeparateThread;
	fnpThreadPool_EnumTaskContext          EnumTaskContext;
}; // "ThreadPool" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( ThreadPool_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION          mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT          mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTP_MIN_THREADS             mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgTP_MAX_THREADS             mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgTP_DEFAULT_THREAD_PRIORITY mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgTP_ALLOW_TERMINATING       mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001003 )
#define pgTP_TERMINATE_TIMEOUT       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgTP_YIELD_TIMEOUT           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgTP_TASK_QUEUE_MAX_LEN      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001006 )
#define pgTP_MAX_IDLE_COUNT          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001007 )
#define pgTP_THREAD_CALLBACK         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001008 )
#define pgTP_THREAD_CALLBACK_CONTEXT mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001009 )
#define pgTP_INVISIBLE_FLAG          mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100a )
#define pgTP_DELAYED_DONE            mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100b )
#define pgTP_QUICK_DONE              mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100c )
#define pgTP_QUICK_EXECUTE           mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100d )
#define pgTP_NAME                    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x0000100e )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_ThreadPool_AddTask( _this, pTaskId, cbTaskCallback, pTaskData, DataLen, TaskPriority )                                 ((_this)->vtbl->AddTask( (_this), pTaskId, cbTaskCallback, pTaskData, DataLen, TaskPriority ))
	#define CALL_ThreadPool_GetThreadState( _this, pState, TaskId )                                                                     ((_this)->vtbl->GetThreadState( (_this), pState, TaskId ))
	#define CALL_ThreadPool_GetThreadContext( _this, TaskId, ppTaskContext, ppTaskData, pTaskDataLen, pState )                          ((_this)->vtbl->GetThreadContext( (_this), TaskId, ppTaskContext, ppTaskData, pTaskDataLen, pState ))
	#define CALL_ThreadPool_ReleaseThreadContext( _this, TaskId )                                                                       ((_this)->vtbl->ReleaseThreadContext( (_this), TaskId ))
	#define CALL_ThreadPool_WaitForTaskDone( _this, TaskId )                                                                            ((_this)->vtbl->WaitForTaskDone( (_this), TaskId ))
	#define CALL_ThreadPool_SetThreadPriorityByTask( _this, TaskId, ThreadPriority )                                                    ((_this)->vtbl->SetThreadPriorityByTask( (_this), TaskId, ThreadPriority ))
	#define CALL_ThreadPool_RemoveTaskFromQueue( _this, TaskId )                                                                        ((_this)->vtbl->RemoveTaskFromQueue( (_this), TaskId ))
	#define CALL_ThreadPool_WaitTaskDonenotify( _this, Timeout )                                                                        ((_this)->vtbl->WaitTaskDonenotify( (_this), Timeout ))
	#define CALL_ThreadPool_CreateSeparateThread( _this, cbSeparateFunc, SeparateData )                                                 ((_this)->vtbl->CreateSeparateThread( (_this), cbSeparateFunc, SeparateData ))
	#define CALL_ThreadPool_EnumTaskContext( _this, EnumMode, pTaskId, ppTaskContext, ppTaskData, pTaskDatalen, pState )                ((_this)->vtbl->EnumTaskContext( (_this), EnumMode, pTaskId, ppTaskContext, ppTaskData, pTaskDatalen, pState ))
#else // if !defined( __cplusplus )
	#define CALL_ThreadPool_AddTask( _this, pTaskId, cbTaskCallback, pTaskData, DataLen, TaskPriority )                                 ((_this)->AddTask( pTaskId, cbTaskCallback, pTaskData, DataLen, TaskPriority ))
	#define CALL_ThreadPool_GetThreadState( _this, pState, TaskId )                                                                     ((_this)->GetThreadState( pState, TaskId ))
	#define CALL_ThreadPool_GetThreadContext( _this, TaskId, ppTaskContext, ppTaskData, pTaskDataLen, pState )                          ((_this)->GetThreadContext( TaskId, ppTaskContext, ppTaskData, pTaskDataLen, pState ))
	#define CALL_ThreadPool_ReleaseThreadContext( _this, TaskId )                                                                       ((_this)->ReleaseThreadContext( TaskId ))
	#define CALL_ThreadPool_WaitForTaskDone( _this, TaskId )                                                                            ((_this)->WaitForTaskDone( TaskId ))
	#define CALL_ThreadPool_SetThreadPriorityByTask( _this, TaskId, ThreadPriority )                                                    ((_this)->SetThreadPriorityByTask( TaskId, ThreadPriority ))
	#define CALL_ThreadPool_RemoveTaskFromQueue( _this, TaskId )                                                                        ((_this)->RemoveTaskFromQueue( TaskId ))
	#define CALL_ThreadPool_WaitTaskDonenotify( _this, Timeout )                                                                        ((_this)->WaitTaskDonenotify( Timeout ))
	#define CALL_ThreadPool_CreateSeparateThread( _this, cbSeparateFunc, SeparateData )                                                 ((_this)->CreateSeparateThread( cbSeparateFunc, SeparateData ))
	#define CALL_ThreadPool_EnumTaskContext( _this, EnumMode, pTaskId, ppTaskContext, ppTaskData, pTaskDatalen, pState )                ((_this)->EnumTaskContext( EnumMode, pTaskId, ppTaskContext, ppTaskData, pTaskDatalen, pState ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iThreadPool 
	{
		virtual tERROR pr_call AddTask( tTaskId* pTaskId, tThreadTaskCallback cbTaskCallback, tPTR pTaskData, tDWORD DataLen, tDWORD TaskPriority ) = 0; // -- Добавление задачи в очередь
		virtual tERROR pr_call GetThreadState( tThreadState* pState, tTaskId TaskId ) = 0; // -- возвращает статус потока/задачи
		virtual tERROR pr_call GetThreadContext( tTaskId TaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDataLen, tThreadState* pState ) = 0; // -- получить контекст задания
		virtual tERROR pr_call ReleaseThreadContext( tTaskId TaskId ) = 0; // -- освободить контекст задачи
		virtual tERROR pr_call WaitForTaskDone( tTaskId TaskId ) = 0; // -- дождаться выполнения указанного задания
		virtual tERROR pr_call SetThreadPriorityByTask( tTaskId TaskId, tThreadPriority ThreadPriority ) = 0; // -- изменить приоритет потока для указанной задачи
		virtual tERROR pr_call RemoveTaskFromQueue( tTaskId TaskId ) = 0; // -- удалить задачу из очереди
		virtual tERROR pr_call WaitTaskDonenotify( tDWORD Timeout ) = 0; // -- дождатся выполнения задачи
		virtual tERROR pr_call CreateSeparateThread( tThreadTaskCallback cbSeparateFunc, tDWORD SeparateData ) = 0; // -- создание потока для индивидуального задания
		virtual tERROR pr_call EnumTaskContext( tThreadTaskEnum EnumMode, tTaskId* pTaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDatalen, tThreadState* pState ) = 0;
	};

	struct pr_abstract cThreadPool : public iThreadPool, public iObj 
	{

		OBJ_IMPL( cThreadPool );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTHREADPOOL()   { return (hTHREADPOOL)this; }

		tDWORD pr_call get_pgTP_MIN_THREADS() { return (tDWORD)getDWord(pgTP_MIN_THREADS); }
		tERROR pr_call set_pgTP_MIN_THREADS( tDWORD value ) { return setDWord(pgTP_MIN_THREADS,(tDWORD)value); }

		tDWORD pr_call get_pgTP_MAX_THREADS() { return (tDWORD)getDWord(pgTP_MAX_THREADS); }
		tERROR pr_call set_pgTP_MAX_THREADS( tDWORD value ) { return setDWord(pgTP_MAX_THREADS,(tDWORD)value); }

		tThreadPriority pr_call get_pgTP_DEFAULT_THREAD_PRIORITY() { tThreadPriority value = {0}; get(pgTP_DEFAULT_THREAD_PRIORITY,&value,sizeof(value)); return value; }
		tERROR pr_call set_pgTP_DEFAULT_THREAD_PRIORITY( tThreadPriority value ) { return set(pgTP_DEFAULT_THREAD_PRIORITY,&value,sizeof(value)); }

		tBOOL pr_call get_pgTP_ALLOW_TERMINATING() { return (tBOOL)getBool(pgTP_ALLOW_TERMINATING); }
		tERROR pr_call set_pgTP_ALLOW_TERMINATING( tBOOL value ) { return setBool(pgTP_ALLOW_TERMINATING,(tBOOL)value); }

		tDWORD pr_call get_pgTP_TERMINATE_TIMEOUT() { return (tDWORD)getDWord(pgTP_TERMINATE_TIMEOUT); }
		tERROR pr_call set_pgTP_TERMINATE_TIMEOUT( tDWORD value ) { return setDWord(pgTP_TERMINATE_TIMEOUT,(tDWORD)value); }

		tDWORD pr_call get_pgTP_YIELD_TIMEOUT() { return (tDWORD)getDWord(pgTP_YIELD_TIMEOUT); }
		tERROR pr_call set_pgTP_YIELD_TIMEOUT( tDWORD value ) { return setDWord(pgTP_YIELD_TIMEOUT,(tDWORD)value); }

		tDWORD pr_call get_pgTP_TASK_QUEUE_MAX_LEN() { return (tDWORD)getDWord(pgTP_TASK_QUEUE_MAX_LEN); }
		tERROR pr_call set_pgTP_TASK_QUEUE_MAX_LEN( tDWORD value ) { return setDWord(pgTP_TASK_QUEUE_MAX_LEN,(tDWORD)value); }

		tDWORD pr_call get_pgTP_MAX_IDLE_COUNT() { return (tDWORD)getDWord(pgTP_MAX_IDLE_COUNT); }
		tERROR pr_call set_pgTP_MAX_IDLE_COUNT( tDWORD value ) { return setDWord(pgTP_MAX_IDLE_COUNT,(tDWORD)value); }

		tThreadCallback pr_call get_pgTP_THREAD_CALLBACK() { tThreadCallback value = {0}; get(pgTP_THREAD_CALLBACK,&value,sizeof(value)); return value; }
		tERROR pr_call set_pgTP_THREAD_CALLBACK( tThreadCallback value ) { return set(pgTP_THREAD_CALLBACK,&value,sizeof(value)); }

		tPTR pr_call get_pgTP_THREAD_CALLBACK_CONTEXT() { return (tPTR)getPtr(pgTP_THREAD_CALLBACK_CONTEXT); }
		tERROR pr_call set_pgTP_THREAD_CALLBACK_CONTEXT( tPTR value ) { return setPtr(pgTP_THREAD_CALLBACK_CONTEXT,(tPTR)value); }

		tBOOL pr_call get_pgTP_INVISIBLE_FLAG() { return (tBOOL)getBool(pgTP_INVISIBLE_FLAG); }
		tERROR pr_call set_pgTP_INVISIBLE_FLAG( tBOOL value ) { return setBool(pgTP_INVISIBLE_FLAG,(tBOOL)value); }

		tBOOL pr_call get_pgTP_DELAYED_DONE() { return (tBOOL)getBool(pgTP_DELAYED_DONE); }
		tERROR pr_call set_pgTP_DELAYED_DONE( tBOOL value ) { return setBool(pgTP_DELAYED_DONE,(tBOOL)value); }

		tBOOL pr_call get_pgTP_QUICK_DONE() { return (tBOOL)getBool(pgTP_QUICK_DONE); }
		tERROR pr_call set_pgTP_QUICK_DONE( tBOOL value ) { return setBool(pgTP_QUICK_DONE,(tBOOL)value); }

		tBOOL pr_call get_pgTP_QUICK_EXECUTE() { return (tBOOL)getBool(pgTP_QUICK_EXECUTE); }
		tERROR pr_call set_pgTP_QUICK_EXECUTE( tBOOL value ) { return setBool(pgTP_QUICK_EXECUTE,(tBOOL)value); }

		tERROR pr_call get_pgTP_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgTP_NAME,buff,size,cp); }
		tERROR pr_call set_pgTP_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgTP_NAME,buff,size,cp); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



#if defined (__cplusplus)

#define THREAD_POOL_TASKID_UNK ((tTaskId)(-1))

class cThreadTaskBase
{
public:
	cThreadTaskBase() : m_pTP(NULL), m_nTaskId(THREAD_POOL_TASKID_UNK){}
	~cThreadTaskBase() {}
	
	tERROR start(cThreadPool *pTP, tDWORD priority = TP_THREADPRIORITY_NORMAL) {
		m_pTP = pTP;
		cThreadTaskBase *_this = this;
		return m_pTP ? m_pTP->AddTask(&m_nTaskId, ThreadTaskCallback, &_this, sizeof(cThreadTaskBase*), priority) : errUNEXPECTED;
	}
	
	tERROR wait_for_stop() {
		if( m_pTP && m_nTaskId != THREAD_POOL_TASKID_UNK )
			m_pTP->WaitForTaskDone(m_nTaskId);
		m_nTaskId = THREAD_POOL_TASKID_UNK;
		return errOK;
	}
	
	virtual void on_init() {}
	virtual void do_work() {}
	virtual void on_exit() {}
	virtual bool is_equal(cThreadTaskBase* pTask) { return false; }
	
protected:
	static tERROR ThreadTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen) {
		cThreadTaskBase* _this = *(cThreadTaskBase**)pTaskData;
		switch( CallType )
		{
		case TP_CBTYPE_TASK_INIT_CONTEXT: _this->on_init(); break;
		case TP_CBTYPE_TASK_PROCESS:      _this->do_work(); break;
		case TP_CBTYPE_TASK_DONE_CONTEXT: _this->on_exit(); break;
		}
		return errOK;
	}
	
	cThreadPool* m_pTP;
	tTaskId      m_nTaskId;
};

struct cThreadPoolBase {
	cThreadPoolBase(const char* name) : m_TP(0), m_Name(name) {}
	~cThreadPoolBase() { de_init(); }

	operator cThreadPool* () { return m_TP; }
	cThreadPool* operator -> () { return m_TP; }

	tERROR init( const iObj* parent, tDWORD max_thread_count=1, tDWORD idle_timeout = 0, tThreadPriority priority = TP_THREADPRIORITY_NORMAL, tDWORD max_idle_count = 1 ) {
		cThreadPool* tp = 0;
		tERROR error = const_cast<iObj*>(parent)->sysCreateObject( (hOBJECT*)&tp, IID_THREADPOOL );
		if ( PR_FAIL(error) )
			return error;
		if (m_Name)
			tp->set_pgTP_NAME((char*)m_Name, 0, cCP_ANSI);
		tp->set_pgTP_MAX_THREADS( max_thread_count );
        tp->set_pgTP_DEFAULT_THREAD_PRIORITY( priority );
		if ( idle_timeout ) {
			tp->set_pgTP_THREAD_CALLBACK(ThreadCallback);
			tp->set_pgTP_THREAD_CALLBACK_CONTEXT(this);
			tp->set_pgTP_MIN_THREADS( 1 );
			tp->set_pgTP_MAX_IDLE_COUNT( max_idle_count );
			tp->set_pgTP_YIELD_TIMEOUT( idle_timeout );
		}
		error = tp->sysCreateObjectDone();
		if ( PR_SUCC(error) )
			m_TP = tp;
		else
			tp->sysCloseObject();
		return error;
	}

	bool is_inited() { return !!m_TP; }
	
	bool check_task_in_queue(cThreadTaskBase* task) {
		if ( !m_TP )
			return false;
		tPTR pTaskData = NULL; tDWORD nDataLen; tTaskId pTaskId; tPTR ppTaskContext; tThreadState pState;
		tERROR err = m_TP->EnumTaskContext( TP_ENUM_START, &pTaskId, &ppTaskContext, &pTaskData, &nDataLen, &pState);
		while( PR_SUCC(err) ) {
			if( task->is_equal((*(cThreadTaskBase**)pTaskData)) ) {
				m_TP->EnumTaskContext( TP_ENUM_ABORT, NULL, NULL, NULL, NULL, NULL);
				return true;
			}

			err = m_TP->EnumTaskContext( TP_ENUM_NEXT, &pTaskId, &ppTaskContext, &pTaskData, &nDataLen, &pState);
		}
		return false;
	}

	tERROR do_command(tDWORD command, tPTR ctx=NULL) {
		cCommandTask* pTask = new cCommandTask();
		pTask->m_pPool = this;
		pTask->m_nCommand = command;
		pTask->m_pCtx = ctx;
		return pTask->start(*this);
	}

	void de_init() {
		if ( m_TP ) {
			cThreadPool* tp = m_TP;
			m_TP = 0;
			tp->sysCloseObject();
		}
	}

	virtual tERROR on_idle() { return errOK; }
	virtual tERROR on_command(tDWORD command, void* ctx) { return errOK; }

protected:
	cThreadPool* m_TP;
	const char* m_Name;
	static tERROR pr_call ThreadCallback( tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext ) {
		switch (CallType)
		{
			default                           :
			case TP_CBTYPE_THREAD_INIT_CONTEXT:
			case TP_CBTYPE_THREAD_DONE_CONTEXT: return errOK;
			case TP_CBTYPE_THREAD_YIELD:
				return ((cThreadPoolBase*)pCommonThreadContext)->on_idle();
		}
	}

	struct cCommandTask : public cThreadTaskBase
	{
		cThreadPoolBase* m_pPool;
		tDWORD           m_nCommand;
		tPTR             m_pCtx;

		virtual void do_work() { m_pPool->on_command(m_nCommand, m_pCtx); }
		virtual void on_exit() { delete this; }
	};
};


#endif // if defined (__cplusplus)

// AVP Prague stamp begin( Header endif,  )
#endif // __i_threadpool__99aa762b_2ca5_4f7a_a0ef_e0e9f1d87e2c
// AVP Prague stamp end



