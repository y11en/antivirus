#ifndef __THREADCONTEXT_H
#define __THREADCONTEXT_H
//--------------------------------------------------------------------------
typedef PVOID PTHREAD_CONTEXT;

PTHREAD_CONTEXT AllocateThreadContext( 
									  __in POOL_TYPE PoolType,
									  __in ULONG cbContext
									  );

VOID ReleaseThreadContext( __in PTHREAD_CONTEXT pContext );

// retval:	TRUE if no thread with ThreadId Id existed in the cache
//			else FALSE
BOOLEAN SetThreadContext( 
						 __in PTHREAD_CONTEXT pContext,
						 __in HANDLE ThreadId 
						 );

PTHREAD_CONTEXT GetThreadContext( __in HANDLE ThreadId );
BOOLEAN InitThreadContexts();
VOID UnInitThreadContexts();
//--------------------------------------------------------------------------
#endif
