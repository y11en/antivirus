#include "std/base/kldefs.h"

#include "testmodule.h"
#include "helpers.h"

#include "par/partest.h"
#include "sch/schtest.h"
#include "err/errtest.h"
#include "thr/thrtest.h"
#include "tp/tptest.h"
#include "tp/queuetest.h"

TESTMAP_2_DECL;

_BEGIN_TESTMAP_2()

//	SCH
  	ENTRY_TEST_2(L"TaskFunctionality", CTaskFunctionality)
	ENTRY_TEST_2(L"Execution", CExecution)
	ENTRY_TEST_2(L"Iterator", CIterator)
	ENTRY_TEST_2(L"SuspendResume", CSuspendResume)
	ENTRY_TEST_2(L"Performance", CPerformance)	
//	THR
	ENTRY_TEST_2(L"Thread", CThreadTest)
	ENTRY_TEST_2(L"CriticalSection", CCriticalSectionTest)
	ENTRY_TEST_2(L"Semaphore", CSemaphoreTest)
	ENTRY_TEST_2(L"ThreadStore", CThreadStoreTest)
//	ERR
	ENTRY_TEST_2(L"ERR_General", CErrorTest)
    ENTRY_TEST_2(L"STDCONV", CStdTest)
    ENTRY_TEST_2(L"CIOTest", CIOTest)
    ENTRY_TEST_2(L"CStlTest", CStlTest)
    ENTRY_TEST_2(L"CRWLockTest", CRWLockTest)
//	TP
	ENTRY_TEST_2(L"ThreadsPool", CThreadPoolTest)
    ENTRY_TEST_2(L"QueueTest", CQueueTest)
//	PAR
	ENTRY_TEST_2(L"PAR_General", CGeneralTest2)
	ENTRY_TEST_2(L"PAR_Cloning", CCloningTest2)
	ENTRY_TEST_2(L"PAR_Duplicating", CDuplicatingTest2)  
	ENTRY_TEST_2(L"PAR_Timing", CTimingTest2)
	ENTRY_TEST_2(L"PAR_Ser", CSerTest)
	
_END_TESTMAP_2()