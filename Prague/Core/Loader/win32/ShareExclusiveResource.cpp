#if defined (_WIN32)

#include "shareexclusiveresource.h"

HANDLE g_RWMHeap;

//#define DBG_BREAK
#if defined( _DEBUG ) && defined( DBG_BREAK )
	#define RWM_DebugBreak {OutputDebugString("\n\n\nRWM break!!!\n\n\n");_asm int 3}
#else
	#define RWM_DebugBreak {}
#endif

// ---
tVOID* g_allocator(tDWORD size)
{
	return HeapAlloc(g_RWMHeap, 0, size);
}

// ---
tVOID g_liberator(tVOID* mem)
{
	HeapFree(g_RWMHeap, 0, mem);
}

//+ ----------------------------------------------------------------------------------------
tBOOL RWM_IsSubQueueItemOnly(PRWM_SubQueue pQueueRoot, tDWORD accessor)
{
	PRWM_SubQueue pSubItem = pQueueRoot;
	while (pSubItem != NULL)
	{
		if (pSubItem->m_bFree == cFALSE)
		{
			if (pSubItem->m_Accessor != accessor)
				return cFALSE;
		}
		
		pSubItem = pSubItem->m_pNext;
	}
	
	return cTRUE;
}

inline tBOOL RWM_IsAllSubQueueItemFree(PRWM_SubQueue pQueueRoot)
{
	PRWM_SubQueue pSubItem = pQueueRoot;
	while (pSubItem != NULL)
	{
		if (pSubItem->m_bFree == cFALSE)
			return cFALSE;
		
		pSubItem = pSubItem->m_pNext;
	}
	
	return cTRUE;
}

inline PRWM_SubQueue RWM_IsSubQueueItem(PRWM_SubQueue pQueueRoot, tDWORD accessor) 
{
	PRWM_SubQueue pSubItem = pQueueRoot;
	while (pSubItem != NULL)
	{
		if (pSubItem->m_bFree == cFALSE)
		{
			if (pSubItem->m_Accessor == accessor)
				return pSubItem;
		}
		
		pSubItem = pSubItem->m_pNext;
	}
	
	return NULL;
}

inline PRWM_SubQueue RWM_GetFreeSubItem(PRWM_SubQueue pQueueRoot)
{
	PRWM_SubQueue pSubItem = pQueueRoot;
	while (pSubItem != NULL)
	{
		if (pSubItem->m_bFree == cTRUE)
			return pSubItem;
		pSubItem = pSubItem->m_pNext;
	}
	return NULL;
}

PRWM_SubQueue RWM_AddSubQueueItem(PRWM_SubQueue* ppQueueRoot, tDWORD accessor, RWManager* prwm) 
{
	PRWM_SubQueue pSubItem = NULL;

	if (*ppQueueRoot != NULL)
		pSubItem = RWM_GetFreeSubItem(*ppQueueRoot);

	if (pSubItem == NULL)
	{
		pSubItem  = (PRWM_SubQueue) prwm->allocator(sizeof(RWM_SubQueue));
		if (pSubItem == NULL)
			return NULL;

		pSubItem->m_pNext = *ppQueueRoot;
		*ppQueueRoot = pSubItem;

	}

	pSubItem->m_Accessor = accessor;
	pSubItem->m_EntersCount = 1;
	pSubItem->m_WriterCount = 0;
	pSubItem->m_bFree = cFALSE;
	
	return pSubItem;
}

RWM_PROC void  RWM_FreeSubQueue(PRWM_SubQueue pQueueRoot, RWManager* prwm)
{
	PRWM_SubQueue pSubItem = pQueueRoot;
	PRWM_SubQueue pSubItemTmp;
	while (pSubItem != NULL)
	{
		pSubItemTmp = pSubItem->m_pNext;
		prwm->liberator(pSubItem);
		
		pSubItem = pSubItemTmp;
	}
}


//+ ----------------------------------------------------------------------------------------
#define RWM_Lock(x)	EnterCriticalSection(&prwm->m_protector)
#define RWM_Unlock	LeaveCriticalSection(&prwm->m_protector)

/*
void _enter_prot( RWManager* prwm, DWORD timeout ) {
  DWORD thread_id = GetCurrentThreadId();
  WaitForSingleObject(prwm->m_protector, timeout);
  prwm->m_threads[prwm->m_count++] = thread_id;
}

void _leave_prot( RWManager* prwm ) {
  prwm->m_threads[--prwm->m_count] = 0;
  ReleaseMutex(prwm->m_protector);
}

#define RWM_Lock(x)	_enter_prot(prwm, x)
#define RWM_Unlock	_leave_prot(prwm)
*/

//+ ----------------------------------------------------------------------------------------

// ---
RWM_PROC tBOOL RWM_Initialize(RWManager* prwm, tVOID* (*allocator)(tDWORD), tVOID(*liberator)(tVOID*))
{
	// Initialize all data members to NULL so that we can
	// accurately check whether an error has occurred.
	if (!allocator != !liberator)
		return cFALSE;
	
	memset(prwm, 0, sizeof(RWManager));
	
	//prwm->m_protector = CreateMutex(0, FALSE, 0);
  InitializeCriticalSection( &prwm->m_protector );
	
	if (!allocator)
	{
		g_RWMHeap = GetProcessHeap();
		prwm->allocator = g_allocator;
		prwm->liberator = g_liberator;
	}
	else 
	{
		prwm->allocator = allocator;
		prwm->liberator = liberator;
	}
	
	return cTRUE;
}

//+ ----------------------------------------------------------------------------------------

RWM_PROC tBOOL RWM_AllocNewAccessorItem(RWManager* prwm, PRWM_Queue* ppAccessorItem)
{
	*ppAccessorItem = (PRWM_Queue) prwm->allocator(sizeof(RWM_Queue));
	if (*ppAccessorItem == NULL)
		return cFALSE;
	
	memset(*ppAccessorItem, 0, sizeof(RWM_Queue));
	(*ppAccessorItem)->m_bFree = cFALSE;
	(*ppAccessorItem)->m_SyncEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if ((*ppAccessorItem)->m_SyncEvent == NULL)
		RWM_DebugBreak;
	return cTRUE;
}

RWM_PROC PRWM_Queue RWM_GetFreeAccessorItem(RWManager* prwm)
{
	PRWM_Queue pAccessorItem = NULL;
	if (prwm->m_pCirceQueueFree == NULL)
	{
		if (!RWM_AllocNewAccessorItem(prwm, &pAccessorItem))
			return NULL;
		
		return pAccessorItem;
	}

	pAccessorItem = prwm->m_pCirceQueueFree;
	prwm->m_pCirceQueueFree = prwm->m_pCirceQueueFree->m_pNext;

	pAccessorItem->m_bFree = cFALSE;
	ResetEvent(pAccessorItem->m_SyncEvent);
	
	return pAccessorItem;
}

RWM_PROC void RWM_Delete(RWManager* prwm)
{
	PRWM_Queue pAccessorItem;
	PRWM_Queue pAccessorItemTmp;
	RWM_Lock(INFINITE);
	
	pAccessorItem = prwm->m_pCirceQueueRoot;
	while (pAccessorItem != NULL)
	{
		pAccessorItemTmp = pAccessorItem->m_pNext;
		
		RWM_FreeSubQueue(pAccessorItem->m_pSubList, prwm);
		CloseHandle(pAccessorItem->m_SyncEvent);
		
		prwm->liberator(pAccessorItem);
		
		pAccessorItem = pAccessorItemTmp;
	}
		
  pAccessorItem = prwm->m_pCirceQueueFree;
  while (pAccessorItem != NULL)
  {
    pAccessorItemTmp = pAccessorItem->m_pNext;
    
    RWM_FreeSubQueue(pAccessorItem->m_pSubList, prwm);
    CloseHandle(pAccessorItem->m_SyncEvent);
    
    prwm->liberator(pAccessorItem);
    
    pAccessorItem = pAccessorItemTmp;
  }
		
  RWM_Unlock;
	DeleteCriticalSection(&prwm->m_protector);
	
	memset(prwm, 0, sizeof(RWManager));
}


//+ ----------------------------------------------------------------------------------------

RWM_PROC tBOOL RWM_Wait(RWManager* prwm, tSHARE_LEVEL access_level)
{

  RWM_Lock(INFINITE);

  tDWORD accessor = GetCurrentThreadId();
	PRWM_Queue pAccessorItem = NULL;
	PRWM_SubQueue pSubItem;
	tDWORD EnterCount = 1;



	if (prwm->m_pCirceQueueRoot == NULL)
	{
		// create first item
		if (!RWM_AllocNewAccessorItem(prwm, &pAccessorItem))
		{
			RWM_Unlock;
			return cFALSE;
		}
		
		prwm->m_pCirceQueueRoot = pAccessorItem;
		prwm->m_pCirceQueueLast = pAccessorItem;
	}
	else
	{
		if (prwm->m_pCirceQueueRoot->m_bFree)
			pAccessorItem = prwm->m_pCirceQueueRoot;
	}

	if (pAccessorItem != NULL)
	{
		// new or first item in queue
		pAccessorItem->m_AccessLevel = access_level;
		pAccessorItem->m_bFree = cFALSE;

		pSubItem = RWM_AddSubQueueItem(&pAccessorItem->m_pSubList, accessor, prwm);
		if (pSubItem == NULL)
		{
			pAccessorItem->m_bFree = cTRUE;
			RWM_Unlock;
			return cFALSE;
		}
		else
		{
      
      #ifdef _DEBUG
        prwm->m_enters++;
      #endif

      if (access_level == SHARE_LEVEL_WRITE) {
				pSubItem->m_WriterCount++;
        #ifdef _DEBUG
          prwm->m_writers++;
        #endif
      }
		}
		
		RWM_Unlock;
		return cTRUE;
	}

	pSubItem = RWM_IsSubQueueItem(prwm->m_pCirceQueueRoot->m_pSubList, accessor);
	if (pSubItem != NULL)
	{
		// this is reeentering
    if (access_level == SHARE_LEVEL_WRITE) {
			pSubItem->m_WriterCount++;
      #ifdef _DEBUG
        prwm->m_writers++;
      #endif
    }

		if (prwm->m_pCirceQueueRoot->m_AccessLevel != access_level)
		{
			if (access_level == SHARE_LEVEL_READ)
			{
				// writer want to be a reader.. will be writer :)

				pSubItem->m_EntersCount++;
				pSubItem->m_WriterCount++;

        #ifdef _DEBUG
          prwm->m_enters++;
          prwm->m_writers++;
        #endif
				
				RWM_Unlock;
				return cTRUE;
			}
			else
			{
				// reader want to be a writer :)
				// если он единственный - просто меняем статус
				// otherwise it will be placed on last point (with decrement)
				if (RWM_IsSubQueueItemOnly(prwm->m_pCirceQueueRoot->m_pSubList, accessor))
				{
					prwm->m_pCirceQueueRoot->m_AccessLevel = access_level;
					pSubItem->m_EntersCount++;
          #ifdef _DEBUG
            prwm->m_enters++;
          #endif
          
					RWM_Unlock;
					return cTRUE;
				}
				else
				{
					// не единственный
          #ifdef _DEBUG
            prwm->m_enters -= pSubItem->m_EntersCount;
            prwm->m_writers -= pSubItem->m_WriterCount;
          #endif

          EnterCount += pSubItem->m_EntersCount;
					pSubItem->m_EntersCount = 0;
					pSubItem->m_WriterCount = 0;
          pSubItem->m_bFree = cTRUE;
				}
			}
		}
		else
		{
			pSubItem->m_EntersCount++;
      #ifdef _DEBUG
        prwm->m_enters++;
      #endif
      RWM_Unlock;
			return cTRUE;
		}
	}

	if (access_level == SHARE_LEVEL_READ)
	{
		// check - pissible to allow enter for several readers
		if (prwm->m_pCirceQueueRoot->m_pNext == NULL)
		{
			if (!prwm->m_pCirceQueueRoot->m_AccessLevel)
			{
				if (RWM_AddSubQueueItem(&prwm->m_pCirceQueueRoot->m_pSubList, accessor, prwm) != NULL)
				{
          
          #ifdef _DEBUG
            prwm->m_enters++;
          #endif
          
          RWM_Unlock;
					return cTRUE;
				}
			}
		}

		if (!prwm->m_pCirceQueueLast->m_AccessLevel)
			pAccessorItem = prwm->m_pCirceQueueLast;
	}

	if (pAccessorItem == NULL)
	{
		// new accessor is going to lock 
		pAccessorItem = RWM_GetFreeAccessorItem(prwm);
		if (pAccessorItem == NULL)
		{
			RWM_Unlock;
			return cFALSE;
		}
		pAccessorItem->m_AccessLevel = access_level;
		
		pSubItem = RWM_AddSubQueueItem(&pAccessorItem->m_pSubList, accessor, prwm);
		if (pSubItem == NULL)
		{
			pAccessorItem->m_bFree = cTRUE;
			pAccessorItem->m_pNext = prwm->m_pCirceQueueFree;
			prwm->m_pCirceQueueFree = pAccessorItem;

			RWM_Unlock;
			return cFALSE;
		}
    
    #ifdef _DEBUG
      prwm->m_enters++;
      prwm->m_enters -= pSubItem->m_EntersCount;
      prwm->m_enters += EnterCount;
    #endif

    pSubItem->m_EntersCount = EnterCount;
    if (access_level == SHARE_LEVEL_WRITE) {
      #ifdef _DEBUG
        prwm->m_writers -= pSubItem->m_WriterCount;
        prwm->m_writers++;
      #endif
      pSubItem->m_WriterCount = 1;
    }

		if (EnterCount != 1)
		{
			// встроиться после первого
			pAccessorItem->m_pNext = prwm->m_pCirceQueueRoot->m_pNext;
			if (prwm->m_pCirceQueueRoot == prwm->m_pCirceQueueLast)
				prwm->m_pCirceQueueLast = pAccessorItem;

			prwm->m_pCirceQueueRoot->m_pNext = pAccessorItem;
		}
		else
		{
			pAccessorItem->m_pNext = NULL;
			prwm->m_pCirceQueueLast->m_pNext = pAccessorItem;
			prwm->m_pCirceQueueLast = pAccessorItem;
		}
	}
	else
	{
		if (RWM_AddSubQueueItem(&pAccessorItem->m_pSubList, accessor, prwm) == NULL)
		{
			RWM_DebugBreak;
			RWM_Unlock;
			
			return cFALSE;
		}
    
    #ifdef _DEBUG
      prwm->m_enters++;
    #endif
    
  }


	RWM_Unlock;
	
	WaitForSingleObject(pAccessorItem->m_SyncEvent, INFINITE);

	return cTRUE;
}

//+ ----------------------------------------------------------------------------------------

RWM_PROC tBOOL RWM_Release(RWManager* prwm, tSHARE_LEVEL* prev_level)
{

        tDWORD accessor = GetCurrentThreadId();
	PRWM_SubQueue pSubItem = NULL;

	RWM_Lock(INFINITE);

	if (prwm->m_pCirceQueueRoot == NULL)
	{
		// who is going to release? nobody?
		RWM_DebugBreak;
		RWM_Unlock;
    if ( prev_level )
      *prev_level = SHARE_LEVEL_ERROR;
		return cFALSE;
	}


	pSubItem = RWM_IsSubQueueItem(prwm->m_pCirceQueueRoot->m_pSubList, accessor);
	if (pSubItem == NULL)
	{
		RWM_DebugBreak;
    if ( prev_level )
      *prev_level = SHARE_LEVEL_ERROR;
    RWM_Unlock;
    return cFALSE;
	}
	else
	{
    tSHARE_LEVEL prev_access;
    if (pSubItem->m_EntersCount > 0) {
			pSubItem->m_EntersCount--;
      #ifdef _DEBUG
        prwm->m_enters--;
      #endif
    }
		else
			RWM_DebugBreak;

    prev_access = prwm->m_pCirceQueueRoot->m_AccessLevel;

    if (prwm->m_pCirceQueueRoot->m_AccessLevel) {
			pSubItem->m_WriterCount--;
      #ifdef _DEBUG
        prwm->m_writers--;
      #endif
    }

		// is this last?
		if (pSubItem->m_EntersCount == 0)
		{
			// eee, last eccessor in item-e
			pSubItem->m_bFree = cTRUE;
			if ((prwm->m_pCirceQueueRoot->m_AccessLevel) || (RWM_IsAllSubQueueItemFree(prwm->m_pCirceQueueRoot->m_pSubList)))
			{
				// free current accessor item
				// allow to enter next accessor
				prwm->m_pCirceQueueRoot->m_bFree = cTRUE;

				if (prwm->m_pCirceQueueRoot->m_pNext != NULL)
				{
					PRWM_Queue pNext = prwm->m_pCirceQueueRoot->m_pNext;

					// root to free queue
					prwm->m_pCirceQueueRoot->m_pNext = prwm->m_pCirceQueueFree;
					prwm->m_pCirceQueueFree = prwm->m_pCirceQueueRoot;
					
					// new work item
					prwm->m_pCirceQueueRoot = pNext;

					// go-go-go
					SetEvent(prwm->m_pCirceQueueRoot->m_SyncEvent);
				}
			}
		}
		else
		{
			// is this last writer?
			if ((pSubItem->m_WriterCount == 0) && (prwm->m_pCirceQueueRoot->m_AccessLevel == cTRUE))
			{
				// last writer is exit - only reader enters exist for this accessor... switch to reader
				PRWM_Queue pNext = prwm->m_pCirceQueueRoot->m_pNext;
				prwm->m_pCirceQueueRoot->m_AccessLevel = cFALSE;
				if (pNext == NULL)
				{
					// единственный accessor... просто выход
				}
				else
				{
					// а следующий accessor не ридер случайно? если врайтер то выходим
					if (pNext->m_AccessLevel == cFALSE)
					{
						// всё таки ридер... пусть и они поработают...
						// переложить accessor в список следующего объекта
						// с текущим pSubItem->m_EntersCount
						// текущий рут переложить в свободные
						// отпусить event следующего обеъкта
						PRWM_SubQueue pSubItemTmp = RWM_AddSubQueueItem(&pNext->m_pSubList, accessor, prwm);
						if (pSubItemTmp != NULL)
						{
							tDWORD EnterCount = pSubItem->m_EntersCount;
							pSubItem->m_bFree = cTRUE;
							pSubItem->m_EntersCount = 0;

              #ifdef _DEBUG
                prwm->m_enters++;
                //prwm->m_enters -= EnterCount;
              #endif
              
							prwm->m_pCirceQueueRoot->m_pNext = prwm->m_pCirceQueueFree;
							prwm->m_pCirceQueueFree = prwm->m_pCirceQueueRoot;

							prwm->m_pCirceQueueRoot = pNext;

							pSubItemTmp->m_EntersCount = EnterCount;

              #ifdef _DEBUG
                //prwm->m_enters += EnterCount;
              #endif
              
							SetEvent(pNext->m_SyncEvent);
						}
					}
				}

			}
		}
    if ( prev_level )
      *prev_level = prev_access;
	}
	
	RWM_Unlock;
  return cTRUE;
}

#endif //_WIN32
