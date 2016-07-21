#ifndef MUTEX_H_INCLUDED_CCF6DF09_7FBA_4179_8908_F1F41A7E2373
#define MUTEX_H_INCLUDED_CCF6DF09_7FBA_4179_8908_F1F41A7E2373

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "commonHeaders.h"

namespace ProxyDetectorNamespace {

// a C++ wrapper class for WinAPI for convenience
class Mutex
{
public:
    // helper class which implements scoped lock idiom
    class Lock
    {
    public:
        Lock(Mutex &);
        ~Lock();

    private:
        Mutex &m_mutex;
    };

    Mutex(const std::string &name, const bool global);
    ~Mutex();

    void lock();
    void release();

private:
    HANDLE m_mutex;
};

}	// namespace ProxyDetectorNamespace


#endif  // MUTEX_H_INCLUDED_CCF6DF09_7FBA_4179_8908_F1F41A7E2373
