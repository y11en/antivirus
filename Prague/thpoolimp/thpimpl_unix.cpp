#if defined (__unix__)

#include <new>
#include <errno.h>
#include <sys/time.h>
#include "thpimpl.h"

using namespace std;

/*!
 *		
 *		
 *		(C) 2004 Kaspersky Lab 
 *		
 *		\file	thpimpl_unix.cpp
 *		\brief	threads pool implementation
 *		
 *            
 *		\author Andrew Sobko ( remade by Alexander Sychev )
 *		\date	07.04.2004 18:12
 *		
 *		
 *		
 */		

// Internal types

typedef void (*cleanupFunc) ( void* );

void cleanupMutex ( pthread_mutex_t* aMutex )
{
  pthread_mutex_unlock ( aMutex );
}

class Lock {
public:

  Lock ( pthread_mutex_t* aLock )
    : theLock ( aLock )
  {
    lock ();
  }

  void lock ()
  {
    pthread_mutex_lock ( theLock );
  }

  void unlock ()
  {
    pthread_mutex_unlock ( theLock );
  }

  ~Lock () 
  { 
    unlock ();
  }

 private:
  pthread_mutex_t* theLock;
};

struct ThreadContext {
  NESTED_NODE_LIST_ENTRY(ThreadContext);
  pthread_t                             theThread;
  
  pthread_mutex_t                       theThreadLock;
  pthread_cond_t                        theThreadCondition;
  tThreadState                          theState;
  THPoolData*                           thePoolData;
  unsigned int                          theYieldTimeout;
  unsigned int				theEmptyCirclesCount;
  unsigned int                          theReferenceCount;

  ThreadContext () 
    : theState ( 0 ),
      thePoolData ( 0 ),
      theYieldTimeout ( 0 ),
      theEmptyCirclesCount ( 0 ),
      theReferenceCount ( 0 )
  {
    pthread_mutex_init ( &theThreadLock, 0 );
    pthread_cond_init ( &theThreadCondition, 0 );
  }

  ~ThreadContext ()
  {
    pthread_mutex_destroy ( &theThreadLock );
    pthread_cond_destroy ( &theThreadCondition );
  }
};

void* THP_Alloc ( tDWORD size );
void THP_Free ( void* ptr );
Task*  getFreeTask ( THPoolData* aPoolData );

struct Task
{
  NESTED_NODE_LIST_ENTRY(Task);
  tTaskId			  	theTaskId;
  tThreadTaskCallback	          	theCallbackFunc;
  tThreadState		          	theState;
  unsigned int                          thePriority;
  void*				  	theTaskContext;
  unsigned int			  	theTaskDataLen;
  char*				  	theTaskData;
  pthread_mutex_t                       theTaskLock;
  pthread_cond_t                        theTaskCondition; 
  bool                                  theFreeTask;
  unsigned int                          theReferenceCount;

  Task ()
    : theTaskId ( 0 ),
      theCallbackFunc ( 0 ),
      theState ( 0 ),
      theTaskContext ( 0 ),
      theTaskDataLen ( 0 ),
      theTaskData ( 0 ),
      theFreeTask ( true ),
      theReferenceCount ( 0 )
  {
    pthread_mutex_init ( &theTaskLock, 0 );
    pthread_cond_init ( &theTaskCondition, 0 );
  }

  ~Task ()
  {
    if ( theTaskData )
      THP_Free ( theTaskData );
    pthread_mutex_destroy ( &theTaskLock );
    pthread_cond_destroy ( &theTaskCondition );
  }
};

typedef void* (*threadProcType) ( void* );

void* threadProc ( ThreadContext* aThreadContext );
void* threadSeparateProc ( _tTHPoolSeparateData* aThreadSeparateData );

// functions

void* THP_Alloc ( tDWORD size )
{
  return malloc ( size );
}

void THP_Free ( void* ptr )
{
  free ( ptr );
}

inline ThreadContext* newThreadContext ()
{
  void* aMemory = THP_Alloc ( sizeof ( ThreadContext ) );
  if ( aMemory )
    return new ( aMemory ) ThreadContext;
  else
    return 0;
}

inline Task* newTask ()
{
  void* aMemory = THP_Alloc ( sizeof ( Task ) );
  if ( aMemory )
    return new ( aMemory ) Task;
  else
    return 0;
}

inline void freeThreadContext (  ThreadContext* aThreadContext ) 
{
  {
    Lock aLock  ( &aThreadContext->theThreadLock );
    if ( aThreadContext->theReferenceCount ) {
      aThreadContext->theReferenceCount--;
      return;
    }
  }
  aThreadContext->~ThreadContext ();
  THP_Free ( aThreadContext );
}

inline void freeTask ( Task* aTask )
{
  {
    Lock aLock  ( &aTask->theTaskLock );
    if ( aTask->theReferenceCount ) {
      aTask->theReferenceCount--;
      return;
    }
  }
  aTask->~Task ();
  THP_Free ( aTask );
}

void THP_InitData ( PTHPoolData pTHPdata )
{}

void THP_DoneData ( PTHPoolData pTHPdata )
{
  // deleting all tasks
  Lock aLock ( &pTHPdata->m_Lock );
  Tasks_T::iterator anIterator1 =  pTHPdata->theTasks.begin ();
  while ( anIterator1 != pTHPdata->theTasks.end () )
    freeTask ( &(*anIterator1++) );
  // deleting all threads context
  Threads_T::iterator anIterator2 =  pTHPdata->theThreads.begin ();
  while ( anIterator2 != pTHPdata->theThreads.end () ) {
    aLock.unlock ();
    pthread_join ( anIterator2->theThread, 0 );
    aLock.lock ();
    freeThreadContext ( &(*anIterator2++) );
  }
}

bool startNewThread ( PTHPoolData pTHPdata )
{
  ThreadContext* aThreadContext = newThreadContext ();
  if ( !aThreadContext )
    return false;
  aThreadContext->theState = TP_THREADSTATE_THREAD_STARTING;
  aThreadContext->thePoolData = pTHPdata;
  aThreadContext->theYieldTimeout = pTHPdata->m_YieldTimeout;
  bool aResult = pthread_create ( &aThreadContext->theThread, 0, (threadProcType)threadProc, aThreadContext ) == 0;
  if ( aResult ) {
    // awaiting thread state
    Lock aLock ( &aThreadContext->theThreadLock );
    while ( ( aThreadContext->theState != TP_THREADSTATE_THREAD_AFTER_INIT ) && 
            ( aThreadContext->theState != TP_THREADSTATE_TASK_INITFAILED ) )
      pthread_cond_wait ( &aThreadContext->theThreadCondition, &aThreadContext->theThreadLock );
  }
  // if thread has started successfully, add it to the list
  Lock aLock ( &pTHPdata->m_Lock );
  if ( aResult ) 
    pTHPdata->theThreads.push_back ( *aThreadContext );
  else 
    freeThreadContext ( aThreadContext );
  return aResult;
}

tBOOL THP_Start ( PTHPoolData pTHPdata )
{
  bool aResult = true;
  for ( unsigned int i = 0; ( i < pTHPdata->m_MinThreads) && aResult; i++) 
    aResult  = startNewThread ( pTHPdata );
  return aResult;
}

void THP_Stop ( PTHPoolData pTHPdata )
{
  Lock aLock ( &pTHPdata->m_Lock );
  pTHPdata->theStop = true;
  // send broadcast signal to all threads to finish work
  if ( pTHPdata->m_bQuickDone ) {
    // free delayed tasks
    pTHPdata->m_bDelayedDone = cFALSE;              // switch to synchronize mode
    while ( true ) {
      Task* aTask = getFreeTask ( pTHPdata );
      aLock.unlock ();
      if ( !aTask ) {
        aLock.lock ();
        break;
      }
      Lock aTaskLock ( &aTask->theTaskLock ); 
      aTask->theCallbackFunc ( TP_CBTYPE_TASK_DONE_CONTEXT, 
                               0, 
                               &aTask->theTaskContext, 
                               aTask->theTaskData, 
                               aTask->theTaskDataLen );
      // signaling about end of task processing
      aTask->theState = TP_THREADSTATE_TASK_AFTER_DONE;
      pthread_cond_broadcast ( &aTask->theTaskCondition );
      aTaskLock.unlock ();
      sched_yield ();
      aLock.lock ();
      freeTask ( aTask );
    }
  }
  
  //increase the reference counts of tasks and threads to prevent erasing of them
  for ( Tasks_T::iterator anIterator =  pTHPdata->theTasks.begin ();
        anIterator != pTHPdata->theTasks.end ();
        ++anIterator ) {
    Lock aTaskLock  ( &anIterator->theTaskLock );
    anIterator->theReferenceCount++;
  }

  for ( Threads_T::iterator anIterator =  pTHPdata->theThreads.begin ();
        anIterator != pTHPdata->theThreads.end ();
        ++anIterator ) {
    Lock aThreadLock  ( &anIterator->theThreadLock );
    anIterator->theReferenceCount++;
  }
  aLock.unlock ();  

  // awaiting of finalization all tasks
  for ( Tasks_T::iterator anIterator =  pTHPdata->theTasks.begin ();
        anIterator != pTHPdata->theTasks.end ();
        ++anIterator ) {
    Lock aTaskLock  ( &anIterator->theTaskLock );
    while ( anIterator->theState < TP_THREADSTATE_TASK_AFTER_DONE )
      pthread_cond_wait ( &anIterator->theTaskCondition, &anIterator->theTaskLock );
  }

  aLock.lock (); 
  pTHPdata->theOnExit = true;
  pthread_cond_broadcast ( &pTHPdata->theFreeTasksCondition );
  aLock.unlock ();
  sched_yield ();

  // awaiting of finalization all threads
  for ( Threads_T::iterator anIterator =  pTHPdata->theThreads.begin ();
        anIterator != pTHPdata->theThreads.end ();
        ++anIterator ) {
    //    aLock.unlock ();
    // if thread termination is allowed, awaiting with timeout
    if ( pTHPdata->m_bAllowTerminating && (pTHPdata->m_TerminateTimeout != INFINITE ) ) {
      struct timeval aNow;
      gettimeofday(&aNow, 0);
      struct timespec aTimeOut;
      aTimeOut.tv_sec = aNow.tv_sec + 
	      pTHPdata->m_TerminateTimeout / 1000 + 
	      ( aNow.tv_usec + pTHPdata->m_TerminateTimeout % 1000 * 1000 ) / 1000000;
      aTimeOut.tv_nsec = ( aNow.tv_usec + pTHPdata->m_TerminateTimeout % 1000 * 1000 ) % 1000000 * 1000;
      Lock aThreadLock ( &anIterator->theThreadLock );
      int aResult = 0;
      while ( ( anIterator->theState < TP_THREADSTATE_THREAD_STOPPED ) && ( aResult != ETIMEDOUT ) )
        aResult = pthread_cond_timedwait ( &anIterator->theThreadCondition, &anIterator->theThreadLock, &aTimeOut );
      if ( aResult == ETIMEDOUT )
        pthread_cancel ( anIterator->theThread );
    }
    else {
      Lock aThreadLock ( &anIterator->theThreadLock );
      while ( anIterator->theState < TP_THREADSTATE_THREAD_STOPPED )
        pthread_cond_wait ( &anIterator->theThreadCondition, &anIterator->theThreadLock );
    }
    //    aLock.lock ();
  }
}

tERROR THP_AddTask ( PTHPoolData pTHPdata, 
                     tTaskId* pTaskId, 
                     tThreadTaskCallback pTaskCallback, 
                     tPTR pTaskData,
                     tDWORD DataLen,
                     tDWORD TaskPriority )
{
  if ( !pTaskCallback )
    return errPARAMETER_INVALID;
  
  Lock aPoolLock ( &pTHPdata->m_Lock );
  
  if ( pTHPdata->theStop )
    return errOBJECT_ALREADY_FREED;
  
  if ( pTHPdata->m_TaskQueue_MaxLen && ( ( pTHPdata->theTasks.size () + 1 ) >= pTHPdata->m_TaskQueue_MaxLen ) )
    return errNOT_OK;
  
  // allocating and initialization of a new task
  Task* aTask = newTask ();
  if ( !aTask )
    return errNOT_ENOUGH_MEMORY;
  
  aTask->theTaskId = pTHPdata->theTaskIdNext++;
  
  if ( pTaskId )
    *pTaskId = aTask->theTaskId;
  
  aTask->thePriority = TaskPriority;
  aTask->theTaskDataLen = DataLen;
  aTask->theCallbackFunc = pTaskCallback;
  aTask->theState = TP_THREADSTATE_TASK_WAITING_THREAD;
  aTask->theTaskContext = 0;
  aTask->theTaskData = reinterpret_cast<char*> ( THP_Alloc ( DataLen ) );
  if ( !aTask->theTaskData ) {
    freeTask ( aTask );
    return errNOT_ENOUGH_MEMORY;    
  }
  memcpy ( aTask->theTaskData, pTaskData, DataLen );
  
  // inserting the new task into the tasks list according to its priority
  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) 
    if ( anIterator->thePriority >= TaskPriority )
      ++anIterator;
    else
      break;
  pTHPdata->theTasks.insert ( anIterator, *aTask );
  // starting a new thread if nessesary
  if ( !pTHPdata->m_MaxThreads || 
       ( ( pTHPdata->theThreads.size () < pTHPdata->m_MaxThreads ) && 
         ( pTHPdata->theThreads.size () <= pTHPdata->theTasks.size () ) ) ) {
    aPoolLock.unlock ();
    if ( !startNewThread ( pTHPdata ) && !pTHPdata->theThreads.size () ) {
      aPoolLock.lock ();
      freeTask ( aTask );
      return errNOT_OK;
    }
    aPoolLock.lock ();
  }
  // signal to all threads about new task
  pTHPdata->theFreeTasksNumber++;
  pthread_cond_broadcast ( &pTHPdata->theFreeTasksCondition );
  return errOK;
}

tThreadState THP_GetThreadState ( PTHPoolData pTHPdata, tTaskId TaskId )
{
  Lock aPoolLock ( &pTHPdata->m_Lock );
  
  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();

  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theTaskId == TaskId )
      return anIterator->theState;
    ++anIterator;
  }
  return TP_THREADSTATE_THREAD_NOTFOUND;
}

tERROR THP_WaitForTaskDone ( PTHPoolData pTHPdata, tTaskId TaskId )
{
  Lock aPoolLock ( &pTHPdata->m_Lock );
  // searching a task with required taskid
  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theTaskId == TaskId )
      break;
    ++anIterator;
  }

  if ( anIterator == pTHPdata->theTasks.end () ) 
    return errNOT_FOUND;
  
  // increasing the reference count of the found task - thread hasn't to destroy this task!
  Lock aLock  ( &anIterator->theTaskLock );
  anIterator->theReferenceCount++; 
  aLock.unlock ();

  aPoolLock.unlock ();
  // awaiting of end of task processing
  aLock.lock ();
  while ( anIterator->theState < TP_THREADSTATE_TASK_AFTER_DONE )
    pthread_cond_wait ( &anIterator->theTaskCondition, &anIterator->theTaskLock );
  aLock.unlock ();
  // destroying task 
  aPoolLock.lock ();
  freeTask ( &(*anIterator) );
  return errOK;
}

tERROR THP_GetThreadContext ( PTHPoolData pTHPdata, 
                              tTaskId TaskId, 
                              tPTR* ppTaskContext, 
                              tPTR* ppTaskData,
                              tDWORD* pTaskDatalen, 
                              tThreadState* pState )
{
  if (pTHPdata->m_bQuickExecute)
    return errOPERATION_CANCELED;

  if ( !ppTaskContext || !ppTaskData || !pState || !pTaskDatalen )
    return errPARAMETER_INVALID;

  Lock aPoolLock ( &pTHPdata->m_Lock );  

  if ( pTHPdata->theStop )
    return errOBJECT_ALREADY_FREED;
  
  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theTaskId == TaskId )
      break;
    ++anIterator;
  }

  if ( ( anIterator == pTHPdata->theTasks.end () ) || ( anIterator->theState >= TP_THREADSTATE_TASK_AFTER_DONE ) )
    return errNOT_FOUND;

  pthread_mutex_lock ( &anIterator->theTaskLock );

  *ppTaskContext = anIterator->theTaskContext;
  *ppTaskData = anIterator->theTaskData;
  *pTaskDatalen = anIterator->theTaskDataLen;
  *pState = anIterator->theState;
  return errOK;
}

void THP_ReleaseThreadContext ( PTHPoolData pTHPdata, tTaskId TaskId )
{
  Lock aPoolLock ( &pTHPdata->m_Lock );  
  
  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theTaskId == TaskId ) {
      pthread_mutex_unlock ( &anIterator->theTaskLock ); 
      return;
    }
    ++anIterator;
  }
}

tERROR THP_SetThreadPriorityByTask ( PTHPoolData pTHPdata, tTaskId TaskId, tThreadPriority ThreadPriority )
{
  return errNOT_SUPPORTED;
}

tERROR THP_RemoveTaskFromQueue(PTHPoolData pTHPdata, tTaskId TaskId)
{
  Task* aTask = 0;
  
  Lock aPoolLock ( &pTHPdata->m_Lock );
  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theTaskId == TaskId )
      break;
    ++anIterator;
  }
  
  if ( anIterator == pTHPdata->theTasks.end () )
    return errNOT_FOUND;
  
  if ( !anIterator->theFreeTask )
    return errLOCKED;
  anIterator->theFreeTask = false;
  pTHPdata->theFreeTasksNumber--;
  aTask = &(*anIterator);
  pTHPdata->theTasks.remove ( *aTask );
  aPoolLock.unlock ();

  aTask->theCallbackFunc ( TP_CBTYPE_TASK_DONE_CONTEXT, 
                           0, 
                           &aTask->theTaskContext, 
                           aTask->theTaskData, 
                           aTask->theTaskDataLen );
  
  freeTask ( aTask );
  return errOK;  
}

tERROR THP_WaitTaskDoneNotify ( PTHPoolData pTHPdata, tDWORD Timeout )
{
  if ( !pTHPdata->m_bDelayedDone )
    return errPARAMETER_INVALID;

  // awaiting of a finished task
  Lock aPoolLock ( &pTHPdata->m_Lock );
  if ( Timeout != INFINITE ) {
    struct timeval aNow;
    gettimeofday ( &aNow, 0 );
    struct timespec aTimeOut;
    aTimeOut.tv_sec = aNow.tv_sec + 
	    Timeout / 1000 + 
	    ( aNow.tv_usec + Timeout % 1000 * 1000 ) / 1000000;
    aTimeOut.tv_nsec = ( aNow.tv_usec + Timeout % 1000 * 1000 ) % 1000000 * 1000;

    aTimeOut.tv_sec = aNow.tv_sec + Timeout / 1000 ;
    aTimeOut.tv_nsec = aNow.tv_usec * 1000 + Timeout % 1000 * 1000000;
    int aResult = 0;
    while ( !pTHPdata->theTaskDone && aResult != ETIMEDOUT )
      aResult = pthread_cond_timedwait ( &pTHPdata->theTaskDoneCondition, 
                                         &pTHPdata->m_Lock, &aTimeOut );
    // if timeout has been expired, process to exit
    if ( aResult == ETIMEDOUT ) 
      return errTIMEOUT;
  }
  else {
    while ( !pTHPdata->theTaskDone )
      pthread_cond_wait ( &pTHPdata->theTaskDoneCondition, 
                          &pTHPdata->m_Lock );
  }

  Tasks_T::iterator anIterator = pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theState == TP_THREADSTATE_TASK_BEFORE_DONE )
      break;
    ++anIterator;	
  }
  
  if ( anIterator == pTHPdata->theTasks.end () )
    return errNOT_FOUND;

  pTHPdata->theTaskDone = false;
  
  Lock aTaskLock ( &anIterator->theTaskLock ); 
  anIterator->theCallbackFunc ( TP_CBTYPE_TASK_DONE_CONTEXT, 
                                0, 
                                &(anIterator->theTaskContext), 
                                anIterator->theTaskData, 
                                anIterator->theTaskDataLen);
  // signaling about end of task processing
  anIterator->theState = TP_THREADSTATE_TASK_AFTER_DONE;
  pthread_cond_broadcast ( &anIterator->theTaskCondition );
  aTaskLock.unlock ();
  aPoolLock.unlock ();
  sched_yield ();
  aPoolLock.lock ();
  freeTask ( &(*anIterator) );
  return errOK;
}

tERROR THP_CreateSeparateThread ( PTHPoolData pTHPdata, tThreadTaskCallback pSeparateCallback, tDWORD SeparateData)
{
  _tTHPoolSeparateData* aThreadContext;
  void* aMemory = THP_Alloc ( sizeof (  _tTHPoolSeparateData ) );
  if ( aMemory )
    aThreadContext = new ( aMemory ) _tTHPoolSeparateData;
  else
    return errNOT_ENOUGH_MEMORY;
  
  aThreadContext->m_pSeparateCallback = pSeparateCallback;
  aThreadContext->m_SeparateData = SeparateData;

  pthread_attr_t aThreadAttrs;
  pthread_t aThreadID;
  if ( pthread_attr_init ( &aThreadAttrs) || 
       pthread_attr_setdetachstate ( &aThreadAttrs, PTHREAD_CREATE_DETACHED ) ||
       pthread_create ( &aThreadID, &aThreadAttrs, (threadProcType)threadSeparateProc, aThreadContext ) )
    return  errOBJECT_CANNOT_BE_INITIALIZED;

  pthread_attr_destroy ( &aThreadAttrs );
  // awaiting thread state
  Lock aLock ( &aThreadContext->theStartedLock );
  while ( !aThreadContext->theStarted )
    pthread_cond_wait ( &aThreadContext->theStartedCondition, &aThreadContext->theStartedLock );
  return errOK;
}

tERROR THP_EnumTaskContext ( PTHPoolData pTHPdata, 
                             tThreadTaskEnum EnumMode, 
                             tTaskId* pTaskId, 
                             tPTR* ppTaskContext, 
                             tPTR* ppTaskData, 
                             tDWORD* pTaskDatalen, 
                             tThreadState* pState )
{
  if ( !pTaskId || !ppTaskContext || !ppTaskData || !pState || ! pTaskDatalen )
    return errPARAMETER_INVALID;

  if ( TP_ENUM_START == EnumMode )
    pthread_mutex_lock ( &pTHPdata->m_Lock );
  
  if ( pTHPdata->theStop ) {
    pthread_mutex_unlock ( &pTHPdata->m_Lock );
    return errOBJECT_ALREADY_FREED;
  }
  
  if ( TP_ENUM_ABORT == EnumMode ){
    pthread_mutex_unlock ( &pTHPdata->m_Lock );
    return errOK;
  }

  Tasks_T::iterator anIterator =  pTHPdata->theTasks.begin ();
  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( anIterator->theTaskId == *pTaskId )
      break;
    ++anIterator;
  }

  if ( TP_ENUM_START != EnumMode )
	  ++anIterator;

  while ( anIterator != pTHPdata->theTasks.end () ) {
    if ( TP_THREADSTATE_TASK_BEFORE_DONE > anIterator->theState ) 
      break;
    ++anIterator;    
  }

  if ( anIterator == pTHPdata->theTasks.end () ) {
    pthread_mutex_unlock ( &pTHPdata->m_Lock );
    return errNOT_FOUND;
  }
  
  *pTaskId = anIterator->theTaskId;
  *ppTaskContext = anIterator->theTaskContext;
  *ppTaskData = anIterator->theTaskData;
  *pTaskDatalen = anIterator->theTaskDataLen;
  *pState = anIterator->theState;

  return errOK;
}

Task*  getFreeTask ( THPoolData* aPoolData )
{
  Tasks_T::iterator anIterator = aPoolData->theTasks.begin ();
  while ( anIterator != aPoolData->theTasks.end () ) {
    if ( anIterator->theFreeTask ) {
      anIterator->theFreeTask = false;
      --aPoolData->theFreeTasksNumber;
      return &(*anIterator);
    }
    ++anIterator;
  }
  return 0;
}

void* threadProc ( ThreadContext* aThreadContext )
{
  {
    Lock aThreadContextLock ( &aThreadContext->theThreadLock );
    pthread_cleanup_push ( (cleanupFunc)cleanupMutex, &aThreadContext->theThreadLock );
    aThreadContext->theState = TP_THREADSTATE_THREAD_BEFORE_INIT_;
    tERROR anError = errOK;
    
    void* aContext = 0;
    if ( aThreadContext->thePoolData->m_cbThread ) {
      aThreadContext->theState = TP_THREADSTATE_THREAD_INITING;
      anError = aThreadContext->thePoolData->m_cbThread ( TP_CBTYPE_THREAD_INIT_CONTEXT, 
                                                          aThreadContext->thePoolData->m_pThreadData, 
                                                          &aContext );
    }
    // signaling to the thread creator about status of the thread
    aThreadContext->theState = PR_SUCC ( anError ) ? TP_THREADSTATE_THREAD_AFTER_INIT : TP_THREADSTATE_TASK_INITFAILED;
    pthread_cond_signal ( &aThreadContext->theThreadCondition );
    aThreadContextLock.unlock ();
    sched_yield ();
    if ( PR_FAIL ( anError ) ) {
      Lock aPoolLock ( &aThreadContext->thePoolData->m_Lock );
      freeThreadContext ( aThreadContext );
      return 0;
    }

    while ( true ) {
      Task* aTask = 0;
      Lock aLock ( &aThreadContext->thePoolData->m_Lock );
      // awaiting of a free task
      if ( aThreadContext->thePoolData->m_YieldTimeout != INFINITE ) {
        struct timeval aNow;
        gettimeofday ( &aNow, 0 );
        struct timespec aTimeOut;
 	aTimeOut.tv_sec = aNow.tv_sec + 
 		aThreadContext->thePoolData->m_YieldTimeout / 1000 + 
 		( aNow.tv_usec + aThreadContext->thePoolData->m_YieldTimeout % 1000 * 1000 ) / 1000000;
	aTimeOut.tv_nsec = ( aNow.tv_usec + aThreadContext->thePoolData->m_YieldTimeout % 1000 * 1000 ) % 1000000 * 1000;
        int aResult = 0;
        pthread_cleanup_push ( (cleanupFunc)cleanupMutex, &aThreadContext->thePoolData->m_Lock );
        while ( !aThreadContext->thePoolData->theFreeTasksNumber && 
                aResult != ETIMEDOUT && 
                !aThreadContext->thePoolData->theOnExit )
          aResult = pthread_cond_timedwait ( &aThreadContext->thePoolData->theFreeTasksCondition, 
                                             &aThreadContext->thePoolData->m_Lock, &aTimeOut );
        pthread_cleanup_pop ( 0 );
        // if timeout has been expired, process empty cycle or exit
        if ( aResult == ETIMEDOUT ) { 
          if ( ( ( ( aThreadContext->thePoolData->m_IdleCountMax > 0 ) && 
		   ( aThreadContext->theEmptyCirclesCount++ < aThreadContext->thePoolData->m_IdleCountMax )
		   ) || 
                 ( aThreadContext->thePoolData->m_IdleCountMax == static_cast<tDWORD> ( -1 ) ) ||
		 ( aThreadContext->thePoolData->theThreads.size () == aThreadContext->thePoolData->m_MinThreads ) ) && 
               aThreadContext->thePoolData->m_cbThread )  
            aThreadContext->thePoolData->m_cbThread ( TP_CBTYPE_THREAD_YIELD, aThreadContext->thePoolData->m_pThreadData, &aContext );
          else 
            break;
        }
      }
      else {
        pthread_cleanup_push ( (cleanupFunc)cleanupMutex, &aThreadContext->thePoolData->m_Lock );
        while ( !aThreadContext->thePoolData->theFreeTasksNumber && 
                !aThreadContext->thePoolData->theOnExit )
          pthread_cond_wait ( &aThreadContext->thePoolData->theFreeTasksCondition, 
                              &aThreadContext->thePoolData->m_Lock );
        pthread_cleanup_pop ( 0 );
      }
      if ( aThreadContext->thePoolData->theOnExit ) 
        break;
      // getting a free task
      aTask = getFreeTask ( aThreadContext->thePoolData );
      aLock.unlock ();
      // if a free task has beeg got
      if ( aTask ) {
        // process task
        Lock aTaskLock ( &aTask->theTaskLock );
        pthread_cleanup_push ( (cleanupFunc)cleanupMutex, &aTask->theTaskLock );
        aTask->theState = TP_THREADSTATE_TASK_PROCESSING_INIT;
      
        anError = aTask->theCallbackFunc ( TP_CBTYPE_TASK_INIT_CONTEXT, 
                                           aContext, 
                                           &aTask->theTaskContext, 
                                           aTask->theTaskData, 
                                           aTask->theTaskDataLen);
        if ( PR_FAIL ( anError ) )
          aTask->theState = TP_THREADSTATE_TASK_INITFAILED;
        else {
          aTask->theState = TP_THREADSTATE_TASK_AFTER_INIT;
          if ( !aThreadContext->thePoolData->m_bQuickExecute ) {
            aTaskLock.unlock ();
            // getting a chanse to lock task context
            sched_yield ();
            aTaskLock.lock ();
          }
          aTask->theState = TP_THREADSTATE_TASK_PROCESSING;
          aTask->theCallbackFunc ( TP_CBTYPE_TASK_PROCESS, 
                                   aContext, 
                                   &aTask->theTaskContext, 
                                   aTask->theTaskData, 
                                   aTask->theTaskDataLen);
          aTask->theState = TP_THREADSTATE_TASK_BEFORE_DONE;
          if ( !aThreadContext->thePoolData->m_bQuickExecute ) {
            aTaskLock.unlock ();
            // getting a chanse to lock thread context
            sched_yield ();        
            aTaskLock.lock ();
          }    
          if ( !aThreadContext->thePoolData->m_bDelayedDone ) {
            aTask->theCallbackFunc ( TP_CBTYPE_TASK_DONE_CONTEXT, 
                                     aContext, 
                                     &aTask->theTaskContext, 
                                     aTask->theTaskData, 
                                     aTask->theTaskDataLen );
            aTask->theState = TP_THREADSTATE_TASK_AFTER_DONE;
          }
          else {
            aTask->theReferenceCount++; 
            aLock.lock ();
            aThreadContext->thePoolData->theTaskDone = true;
            pthread_cond_broadcast ( &aThreadContext->thePoolData->theTaskDoneCondition ); 
            aLock.unlock ();
            sched_yield ();
          }
        }
        // signaling about end of task processing
        pthread_cond_broadcast ( &aTask->theTaskCondition );
        aTaskLock.unlock ();
        pthread_cleanup_pop ( 0 );
        sched_yield ();
      }
      // destroying the task
      if ( aTask ) {
        aLock.lock ();
        freeTask ( aTask );
        aLock.unlock ();
      }
    } 
    aThreadContextLock.lock (); 
    aThreadContext->theState = TP_THREADSTATE_THREAD_BEFORE_STOP;
    
    if ( aThreadContext->thePoolData->m_cbThread ) {
      aThreadContext->theState = TP_THREADSTATE_THREAD_STOPPING;
      anError = aThreadContext->thePoolData->m_cbThread ( TP_CBTYPE_THREAD_DONE_CONTEXT, 
                                                          aThreadContext->thePoolData->m_pThreadData, 
                                                          &aContext );
    }
    aThreadContext->theState = TP_THREADSTATE_THREAD_STOPPED;
    pthread_cond_broadcast ( &aThreadContext->theThreadCondition );
    aThreadContextLock.unlock ();
    pthread_cleanup_pop ( 0 );
    sched_yield ();
  }
  Lock aPoolLock ( &aThreadContext->thePoolData->m_Lock );
  freeThreadContext ( aThreadContext );
  return 0;
}

void* threadSeparateProc ( _tTHPoolSeparateData* aThreadContext )
{
  tThreadTaskCallback aSeparateCallback = aThreadContext->m_pSeparateCallback ;
  tDWORD              aSeparateData =  aThreadContext->m_SeparateData;

  {
    Lock aLock ( &aThreadContext->theStartedLock );
    aThreadContext->theStarted = true;
    pthread_cond_broadcast ( &aThreadContext->theStartedCondition );
  }
  aSeparateCallback (TP_CBTYPE_TASK_PROCESS, 0, 0, reinterpret_cast <tPTR> ( aSeparateData ), 0);
  aThreadContext->~_tTHPoolSeparateData ();
  THP_Free ( aThreadContext );
  return 0;
}

#endif // __unix__
