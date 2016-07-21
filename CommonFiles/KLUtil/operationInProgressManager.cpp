#include "stdafx.h"
#include "operationInProgressManager.h"


namespace KLUTIL {

/////////////////////////////////////////////////////////////////////
///// OperationInProgressManager::Locker
OperationInProgressManager::Locker::Locker(OperationInProgressManager &operationInProgressManager)
 : m_operationInProgressManager(operationInProgressManager)
{
    m_operationInProgressManager.add();

}
OperationInProgressManager::Locker::~Locker()
{
    m_operationInProgressManager.release();
}


/////////////////////////////////////////////////////////////////////
///// OperationInProgressManager

const DWORD OperationInProgressManager::s_infinity = 0xFFFFFFFF;

OperationInProgressManager::OperationInProgressManager()
 : m_counter(0)
{
    m_noOperationInProgressEvent.Set();
}
OperationInProgressManager::~OperationInProgressManager()
{
}

void OperationInProgressManager::add()
{
    CCritSec::CLock lock(m_counterCriticalSection);
    ++m_counter;
    m_noOperationInProgressEvent.Reset();
}
void OperationInProgressManager::release()
{
    CCritSec::CLock lock(m_counterCriticalSection);

    if(m_counter)
        --m_counter;

    // no operation is in progress, set event
    if(!m_counter)
        m_noOperationInProgressEvent.Set();
}
bool OperationInProgressManager::wait(const DWORD &timeoutMilliseconds)const
{
    return WaitForSingleObject(m_noOperationInProgressEvent, timeoutMilliseconds) == WAIT_OBJECT_0;
}




}   // namespace KLUTIL