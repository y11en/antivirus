#ifndef _SYNCHRO_H__INCLUDE_
#define _SYNCHRO_H__INCLUDE_

#include <windows.h>

namespace KLUTIL
{

class CHndl
{
protected:
	HANDLE m_hHandle;
public:
	CHndl(HANDLE hHandle = NULL): m_hHandle(hHandle) {}
	~CHndl() { Close(); }
	operator HANDLE() { return m_hHandle; }
	CHndl& operator = (HANDLE hHandle) { Close(); m_hHandle = hHandle; return *this; }
	void Close() { if (m_hHandle != NULL) { CloseHandle(m_hHandle); m_hHandle = NULL; } }
};

class CFlHndl
{
protected:
	HANDLE m_hHandle;
public:
	CFlHndl(HANDLE hHandle = INVALID_HANDLE_VALUE): m_hHandle(hHandle) {}
	~CFlHndl() { Close(); }
	operator HANDLE() const { return m_hHandle; }
	CFlHndl& operator = (HANDLE hHandle) { Close(); m_hHandle = hHandle; return *this; }
	void Close() { if (m_hHandle != INVALID_HANDLE_VALUE) { CloseHandle(m_hHandle); m_hHandle = INVALID_HANDLE_VALUE; } }
};


class CEvnt: public CHndl
{
public:
	CEvnt(LPSECURITY_ATTRIBUTES pAttr = NULL, bool bManual = true, bool bState = false, LPCTSTR pName = NULL)
		: CHndl( CreateEvent(pAttr, bManual, bState, pName)) {}
	void Set() { SetEvent(m_hHandle); }
	void Reset() { ResetEvent(m_hHandle); }
};

template <class T>
class CLockT
{
public:
	CLockT(CLockT& lock): m_SyncObj(lock.m_SyncObj) { m_SyncObj.Lock(); }
	CLockT(T& SyncObj): m_SyncObj(SyncObj) { m_SyncObj.Lock(); }
	~CLockT() { m_SyncObj.Unlock(); }
private:
	CLockT &operator=(const CLockT& lock);
	T& m_SyncObj;
};

class CCritSec
{
public:
	CCritSec()    { InitializeCriticalSection(&m_cs); }
	~CCritSec()   { DeleteCriticalSection(&m_cs); }
	void Lock()   { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }

	typedef CLockT<CCritSec> CLock;
private:
	CRITICAL_SECTION m_cs;
};

/*
class CSemaphore: public CHndl
{
public:
	CSemaphore(long nInitCount, long nMax)
		: CHndl(CreateSemaphore(NULL, nInitCount, nMax, NULL)) {}
	void Lock()   { WaitForSingleObject(m_hHandle, INFINITE); }
	bool Lock(HANDLE evStop, unsigned nTimeout = INFINITE)
	{
		HANDLE h[] = { m_hHandle, evStop };
		return WaitForMultipleObjects(evStop!=NULL?2:1, h, FALSE, nTimeout)==WAIT_OBJECT_0;
	}
	void Unlock() { ReleaseSemaphore(m_hHandle, 1, NULL); }

	typedef CLockT<CSemaphore> CLock;
};
*/

//////////////////////////////////////////////////////////////

template <class Wrapper>
class CLockedPtrT: public CCritSec::CLock
{
public:
	typedef typename Wrapper::Type Type;
	CLockedPtrT(CLockedPtrT<Wrapper>& ptr): CCritSec::CLock(ptr), m_pObj(ptr.m_pObj) {}
	CLockedPtrT(Wrapper& obj): CCritSec::CLock(obj.m_cs), m_pObj(&(obj.m_obj)) {}
	Type* operator->() { return m_pObj; }
	const Type* operator->() const { return m_pObj; }
	Type& operator*() { return *m_pObj; }
	const Type& operator*() const { return *m_pObj; }
private:
	Type* m_pObj;
};

template <class T>
class CLockedClassT
{
public:
	typedef T                               Type;
	typedef CLockedPtrT< CLockedClassT<T> > Ptr;
	
	// конструкторы CLockedClassT для различных конструкторов класса T
	CLockedClassT(): m_obj() {}
	template <class T1> CLockedClassT(T1 p1): m_obj(p1) {}
	template <class T1, class T2> CLockedClassT(T1 p1, T2 p2): m_obj(p1, p2) {}
	
	Ptr operator->() { return Ptr(*this); } // реально вызов CLockedClassT-> вернёт T&, а не Ptr, как могло бы показаться
	const Ptr operator->() const { return Ptr(*this); }
private:
	T                m_obj;
	CCritSec m_cs;
	friend Ptr;
};

} // namespace KLUTIL

#endif