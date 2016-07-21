#if !defined (_THPIMPL_UNIX_H_)
#define _THPIMPL_UNIX_H_


#include <pthread.h>
#include <semaphore.h>

#include <nestednodelist.h>

//- ------------------------------------------------------------------------------------------

struct ThreadContext;
struct Task;

typedef NestedNodeListNS::NestedNodeList<ThreadContext> Threads_T;
typedef NestedNodeListNS::NestedNodeList<Task> Tasks_T;

struct _tTHPoolData : public _tTHPoolPublicData
{
  //	internal data
  Threads_T				theThreads;
  Tasks_T				theTasks;
  tTaskId				theTaskIdNext;
  bool                                  theOnExit;
  bool                                  theStop;
  unsigned int                          theFreeTasksNumber;
  pthread_cond_t                        theFreeTasksCondition;
  bool                                  theTaskDone;
  pthread_cond_t                        theTaskDoneCondition;
  
  _tTHPoolData ()
    : theTaskIdNext ( 0 ),
       theOnExit ( false ),
       theStop ( false ),
       theFreeTasksNumber ( 0 ),
       theTaskDone ( false )
  {
    pthread_mutex_init ( &m_Lock, 0 );
    pthread_cond_init ( &theFreeTasksCondition, 0 );
    pthread_cond_init ( &theTaskDoneCondition, 0 );

  }

  ~_tTHPoolData ()
  {
    pthread_mutex_destroy ( &m_Lock );
    pthread_cond_destroy ( &theFreeTasksCondition );
    pthread_cond_destroy ( &theTaskDoneCondition );
  }

}; 

struct _tTHPoolSeparateData : public  _tTHPoolSeparate {
  bool                                  theStarted;
  pthread_mutex_t                       theStartedLock;
  pthread_cond_t                        theStartedCondition;

  _tTHPoolSeparateData ()
    : theStarted ( false )
  {
    pthread_mutex_init ( &theStartedLock, 0 );
    pthread_cond_init ( &theStartedCondition, 0 );
  }

  ~_tTHPoolSeparateData ()
  {
    pthread_mutex_destroy ( &theStartedLock );
    pthread_cond_destroy ( &theStartedCondition );
  }

};

#endif
