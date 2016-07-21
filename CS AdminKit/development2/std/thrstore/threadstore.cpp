/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	threadstore.cpp
 * \author	Mikhail Karmazine
 * \date	12.03.2003 16:06:19
 * \brief	
 * 
 */

#ifdef WIN32
#pragma warning( disable : 4786 )
#endif

#include "std/thrstore/threadstore.h"
#include "std/thr/sync.h"
#include "std/thr/thread.h"
#include "std/err/klerrors.h"

#include <map>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __unix
#include <pthread.h>
#endif

using namespace KLSTD;
using namespace std;

CAutoPtr<CriticalSection> g_pCS;
CAutoPtr<ThreadStore> g_pThreadStore;

struct ThreadAndName
{
    ThreadAndName() : dwThreadId(0) {};

    ThreadAndName( int _dwThreadId, const wstring & _wstrObjName ) :
        dwThreadId(_dwThreadId), wstrObjName(_wstrObjName) {};

    int dwThreadId;
    wstring wstrObjName;
};

struct ThreadAndName_Less : public std::binary_function <ThreadAndName, ThreadAndName, bool> 
{
    bool operator()( const ThreadAndName& _Left, const ThreadAndName& _Right ) const
    {
      
      if ( _Left.dwThreadId != _Right.dwThreadId ) {
        return _Left.dwThreadId < _Right.dwThreadId;
      }

      // in original was case insensitive comparison
      return  _Left.wstrObjName < _Right.wstrObjName;
    }
};

typedef CAutoPtr<KLBase> KLBaseAutoPtr;
typedef std::map<ThreadAndName, KLAdapt<KLBaseAutoPtr>, ThreadAndName_Less> ThreadObjects;

class ThreadStoreImpl : public KLBaseImpl<ThreadStore>
{
public:
    void AddObject( const wstring & wstrObjName, KLBase* pObj );

    void GetStoredObject( const std::wstring & wstrObjName, KLSTD::KLBase** ppObj );

    void RemoveObject( const wstring & wstrObjName, KLBase** ppObj = NULL );

    bool IsObjectHere( const wstring & wstrObjName );

    void CleanupThreadObjects();

    void Initialize();

private:
    ThreadObjects m_threadObjects;
    CAutoPtr<CriticalSection> m_pCriticalSection;
};

void ThreadStoreImpl::Initialize()
{
    if( m_pCriticalSection == NULL )
    {
        KLSTD_CreateCriticalSection( & m_pCriticalSection );
    }
}

void ThreadStoreImpl::AddObject( const std::wstring & wstrObjName, KLSTD::KLBase* pObj )
{
    AutoCriticalSection acs( m_pCriticalSection );

	int dwThreadId = KLSTD_GetCurrentThreadId();

    ThreadAndName threadAndName( dwThreadId, wstrObjName );

    ThreadObjects::iterator it = m_threadObjects.find( threadAndName );
    
    if( it != m_threadObjects.end() )
    {
        KLSTD_THROW2( STDE_THREAD_PARAM_EXISTS, wstrObjName.c_str(), dwThreadId );
    }

    m_threadObjects[threadAndName] = CAutoPtr<KLBase>(pObj);
}

void ThreadStoreImpl::GetStoredObject( const std::wstring & wstrObjName, KLSTD::KLBase** ppObj )
{
    AutoCriticalSection acs( m_pCriticalSection );

	int dwThreadId = KLSTD_GetCurrentThreadId();

    ThreadObjects::iterator it = m_threadObjects.find( ThreadAndName( dwThreadId, wstrObjName ) );
    
    if( it == m_threadObjects.end() )
    {
        KLSTD_THROW2( STDE_THREAD_PARAM_NOT_EXISTS, wstrObjName.c_str(), dwThreadId );
    }

    CAutoPtr<KLBase> pTemp = it->second.m_T;
    *ppObj = pTemp.Detach();
}

void ThreadStoreImpl::RemoveObject( const std::wstring & wstrObjName, KLSTD::KLBase** ppObj /* = NULL */ )
{
    AutoCriticalSection acs( m_pCriticalSection );

	int dwThreadId = KLSTD_GetCurrentThreadId();

    ThreadObjects::iterator it = m_threadObjects.find( ThreadAndName( dwThreadId, wstrObjName ) );
    
    if( it == m_threadObjects.end() )
    {
        KLSTD_THROW2( STDE_THREAD_PARAM_NOT_EXISTS, wstrObjName.c_str(), dwThreadId );
    }

    CAutoPtr<KLBase> pTemp = it->second.m_T; // prevent final release inside map:
    if( ppObj )
    {
        *ppObj = pTemp.Detach();
    }

    m_threadObjects.erase( it );
}

bool ThreadStoreImpl::IsObjectHere( const std::wstring & wstrObjName )
{
    AutoCriticalSection acs( m_pCriticalSection );

	int dwThreadId = KLSTD_GetCurrentThreadId();

    ThreadObjects::iterator it = m_threadObjects.find( ThreadAndName( dwThreadId, wstrObjName ) );
    
    return it != m_threadObjects.end();
}

void ThreadStoreImpl::CleanupThreadObjects()
{
    AutoCriticalSection acs( m_pCriticalSection );

    vector< KLAdapt<KLBaseAutoPtr> > vectRemovedObjects;
    ThreadObjects::iterator it = m_threadObjects.begin();
    while( it != m_threadObjects.end() )
    {
        if( it->first.dwThreadId != KLSTD_GetCurrentThreadId() )
        {
            it++;
        } else
        {
            // prevent final release inside map:
            vectRemovedObjects.push_back( it->second );
            m_threadObjects.erase( it++ );
        }
    }
}

KLCSC_DECL CAutoPtr<ThreadStore> KLSTD_GetGlobalThreadStore()
{
    AutoCriticalSection acs( g_pCS );

    if( g_pThreadStore == NULL )
    {
        CAutoPtr<ThreadStoreImpl> pThreadStoreImpl;
        pThreadStoreImpl.Attach( new ThreadStoreImpl );
        KLSTD_CHKMEM( pThreadStoreImpl );
        pThreadStoreImpl->Initialize();
        g_pThreadStore = pThreadStoreImpl;
    }

    return g_pThreadStore;
}

void KLSTD_InitializeGlobalThreadStore()
{
    KLSTD_CreateCriticalSection( & g_pCS );
}

void KLSTD_DeinitializeGlobalThreadStore()
{
    g_pCS = NULL;
    g_pThreadStore = NULL;
}
