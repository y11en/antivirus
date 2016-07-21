#include "mutex.h"


ProxyDetectorNamespace::Mutex::Lock::Lock(Mutex &mutex)
 : m_mutex(mutex)
{
    m_mutex.lock();
}
ProxyDetectorNamespace::Mutex::Lock::~Lock()
{
    m_mutex.release();
}



ProxyDetectorNamespace::Mutex::Mutex(const std::string &nameIn, const bool global)
 : m_mutex(0)
{
    KLUPD::NoCaseString name = KLUPD::asciiToWideChar(nameIn);
    if(!name.empty() && global && KLUPD::isGlobalMutexNamespaceSupported())
        name = KLUPD::NoCaseString(L"Global\\") + KLUPD::asciiToWideChar(nameIn);

#if defined(UNICODE) || defined(_UNICODE)
    #ifdef WSTRING_SUPPORTED
        m_mutex = CreateMutex(0, FALSE, name.toWideChar());
    #else
        m_mutex = CreateMutexA(0, FALSE, name.toAscii().c_str());
    #endif
#else
    m_mutex = CreateMutex(0, FALSE, name.toAscii().c_str());
#endif
}
ProxyDetectorNamespace::Mutex::~Mutex()
{
    if(m_mutex)
        CloseHandle(m_mutex);
}

void ProxyDetectorNamespace::Mutex::lock()
{
    if(!m_mutex)
        return;
    WaitForSingleObject(m_mutex, INFINITE);
}
void ProxyDetectorNamespace::Mutex::release()
{
    if(!m_mutex)
        return;
    ReleaseMutex(m_mutex);
}

