#ifndef __KL_THREAD_H_
#define __KL_THREAD_H_

#include "kldef.h"
#include "klstatus.h"
#include "kl_object.h"
#include "kl_list.h"

HANDLE	
StartThread(PVOID	FuncAddr, PVOID	Context);

class CKl_Thread  :  public CKl_Object
{
public:
    QUEUE_ENTRY;
    CKl_Thread(KL_EVENT*	Event = NULL);
    virtual ~CKl_Thread();

    HANDLE      Start(PKSTART_ROUTINE   ThreadMain, PVOID   Parameter = NULL);
    NTSTATUS    ExitStatus(void);    
    
    // Called by thread
    PVOID       GetParameter(void);
    VOID		Terminate(KLSTATUS status);
    void        WaitForExit();      // Ждет, и завершается тогда, когда поток уничтожен.
    
    // Data members
    PVOID       m_parameter;
    NTSTATUS    m_ExitStatus;
    BOOLEAN     m_started;
    BOOLEAN     m_terminated;

    PVOID       m_Handle;
    PVOID       m_pObject;          // Объект синхронизации. Сигналится, когда поток завершается.
protected:
private:
};

extern CKl_List<CKl_Thread>*   pThreadList;

#endif // __KL_THREAD_H