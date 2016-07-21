#ifndef _THREAD_POOL__INCLUDE_
#define _THREAD_POOL__INCLUDE_

#include "defs.h"
#include <queue>
#include <vector>
#include <windows.h>
#include "inl/synchro.h"

namespace KLUTIL
{

//! Класс, в котором реализуется логика работы потока.
class KLUTIL_API IThreadJob
{
protected:
	virtual ~IThreadJob() {}
public:
	//! Освобождает данный объект.
	//! После окончания работы Process() вызывается Free().
	virtual void Free() { delete this; }
	//! В этой функции реализуется логика работы потока.
	virtual void Process(
		HANDLE evStop	//!< [in] Событие, при срабатывании которого поток должен досрочно выйти. Этот параметр может быть NULL.
		) throw() = 0;
};

template <class T, typename Param = void*>
class CMemberFuncThreadJobT: public IThreadJob
{
	T* m_pObj;
	Param m_param;
	typedef void (T::*ThreadFunc)(Param param, HANDLE evStop) throw();
	ThreadFunc m_pfnFunc;
public:
	CMemberFuncThreadJobT(T* pObj, ThreadFunc pfnFunc)
		: m_pObj(pObj), m_pfnFunc(pfnFunc) {}
	CMemberFuncThreadJobT(T* pObj, ThreadFunc pfnFunc, Param param)
		: m_pObj(pObj), m_pfnFunc(pfnFunc), m_param(param) {}
	void Process(HANDLE evStop) throw()
	{
		(m_pObj->*m_pfnFunc)(m_param, evStop);
	}
};

#pragma warning (push)
#pragma warning (disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'

//! Пул потоков.
class KLUTIL_API CThreadPool
{
public:
	//! Конструктор
	CThreadPool(
		unsigned nThreadCount = MAXIMUM_WAIT_OBJECTS	//!< [in] Максимальное колическтво потоков, запускаемых пулом. Не должно привышать более MAXIMUM_WAIT_OBJECTS.
		) throw();
	~CThreadPool() throw();
	//! Остановливает все потоки.
	void StopAllThreads() throw();
	//! Устанавливает количество потоков.
	//!  \attention Количество потоков можно только увеличить.
	void SetThreadCount(
		unsigned nThreadCount	//!< [in] Максимальное колическтво потоков, запускаемых пулом. Не должно привышать более MAXIMUM_WAIT_OBJECTS.
		) throw();
	//! Добавляет задание на обработку.
	void Process(
		IThreadJob* pJob	//!< [in] Задание на обработку. После окончания работы Process() вызывается Free().
		) throw();
	typedef void (*ThreadFunc)(void* pParam, HANDLE evStop);
	void Process(ThreadFunc pfnThreadFunc, void* pParam) throw();

private:
	static unsigned __stdcall WorkThread(void*);
	std::vector<HANDLE> m_hThreads;
	HANDLE m_evStop;
	typedef std::queue<IThreadJob*> CJobQueue;
	CJobQueue m_JobQueue;
	CCritSec m_cs;
	HANDLE m_hJobCount;
#if defined(_MSC_VER) && _MSC_VER>1300 // Борланду не нравится volatile
	LONG volatile m_nBusyThreadCount;
#else
	LONG          m_nBusyThreadCount;
#endif
	unsigned m_nMaxThreadCount;
};

#pragma warning (pop)


} // namespace KLUTIL

#endif