#if !defined(AFX_OPERATIONINPROGRESSMANAGER_H__3236B000_9D04_4F8A_88AB_0D2D4AAFC3DB__INCLUDED_)
#define AFX_OPERATIONINPROGRESSMANAGER_H__3236B000_9D04_4F8A_88AB_0D2D4AAFC3DB__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "defs.h"
#include "inl/synchro.h"


// 4251 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning( disable : 4251 )

namespace KLUTIL {


/// The OperationInProgressManager thread-safe operation counter. Client can
///  initiate operations, and check whether any operation is in progress.
class KLUTIL_API OperationInProgressManager
{
public:
    /// The Locker class is helper implementation of scopped locking idiom
    ///   to simplify usage of OperationInProgressManager::release() operation,
    ///  which is called automatically by destructor
    class KLUTIL_API Locker
    {
    public:
        /// add operation to OperationInProgressManager object
        Locker(OperationInProgressManager &);
        /// automatically release operation from OperationInProgressManager object
        ~Locker();

    private:
        OperationInProgressManager &m_operationInProgressManager;
    };

	OperationInProgressManager();
	virtual ~OperationInProgressManager();

    /// call this function when you operation started
    void add();
    /// call this function when you operation finished
    void release();
    /// the wait() function blocks execution thread until all operation finishes or timeout expires
    /// @param timeoutMilliseconds [in] - see WaitForSingleObject() MSDN comment
    /// @return true in case no operation condition reached
    ///  WARNING: you can not be sure then until you get return value and use it no more operation was initiated
    bool wait(const DWORD &timeoutMilliseconds = s_infinity)const;

	/// infinity value for wait() function
	static const DWORD s_infinity;

private:
    /// critical section provides thread-safe operation with counter
    mutable CCritSec m_counterCriticalSection;
    /// operation in progress counter
    unsigned m_counter;

    /// the event is set when no operation is in progress
    mutable CEvnt m_noOperationInProgressEvent;
};


}   // namespace KLUTIL

#endif // !defined(AFX_OPERATIONINPROGRESSMANAGER_H__3236B000_9D04_4F8A_88AB_0D2D4AAFC3DB__INCLUDED_)
