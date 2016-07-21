#ifndef __TIMEOUTSTORE_H__
#define __TIMEOUTSTORE_H__
/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	timeoutstore.h
 * \author	Andrew Kazachkov
 * \date	21.11.2002 11:06:52
 * \brief	Временное хранилище объектов
 * 
 */

#include "std/tp/threadspool.h"

namespace KLTMSG
{

	/*!
	*
	*	\class TimeoutStore - хранилище объектов с тайм-аутом
	*
	*/

	class KLSTD_NOVTABLE TimeoutStore : public KLSTD::KLBase
	{
	public:

		/*!
		  \brief	Добавление объекта в хравнилище. При этом инкрементируется
			счётчик ссылок добавляемого объекта. По истечении указанного срока
			счётчик ссылок декрементируется.

		  \param	pItem - указатель на объект
		  \param	lTimeout - время жизни объекта в мс
		  \return	std::wstring - идентификатор объекта
		*/
		virtual std::wstring Insert(KLSTD::KLBase* pItem, long lTimeout)=0;

		virtual void Insert(
				KLSTD::KLBase* pItem,
				long lTimeout,
				const std::wstring& wstrKey)=0;


		/*!
		  \brief	Получение указателя на объект по идентификатору.

		  \param	wstrKey - Идентификатор объекта
		  \param	ppItem - указатель на переменную, в которую будет
					записан указатель на объект.

		  \exception STDE_NOTFOUND - объект с заданным идентификатором не найден.
		*/
		virtual void GetAt(const std::wstring& wstrKey, KLSTD::KLBase** ppItem)=0;


		/*!
		  \brief	Досрочно удаляет объект из хранлища.

		  \param	wstrKey - Идентификатор объекта
		  \param	ppItem - опциональный указатель на переменную, в которую
					будет записан указатель на объект.
		  \return	true - если объект с заданным идентификатором был
					действительно обнаружен в хранилище
		*/
		virtual bool Remove(const std::wstring& wstrKey, KLSTD::KLBase** ppItem=NULL)=0;


		/*!
		  \brief	Продлевает время жизни объекта.

		  \param	wstrKey - Идентификатор объекта

		  \exception STDE_NOTFOUND - объект с заданным идентификатором не найден.
		*/
		virtual void Prolong(const std::wstring& wstrKey)=0;
        
        virtual void Clean() = 0;
	};	


	/*!
	*
	*	\class TimeoutObject - объект с информацией о типе
	*
	*/
    typedef enum
    {
        KLTMSG_REMOVED_MANUALLY,
        KLTMSG_REMOVED_TIMEOUT,
        KLTMSG_REMOVED_DTOR
    }KLTMSG_REMOVE_REASON;

	class KLSTD_NOVTABLE TimeoutObject : public KLSTD::KLBase
	{
	public:
		virtual const char* typeId()=0;
        virtual void KLTMSG_OnRemoved(
                                const std::wstring&     wstrKey,
                                KLTMSG_REMOVE_REASON    nReason){;};
    };

	#define KLTMSG_DECLARE_TYPEID(_classname)	\
		const char* typeId()\
		{\
			static const char c_szaTypeId[]=#_classname;\
			return c_szaTypeId;\
		};

	#define KLTMSG_TYPECHECK(_p, _classname)	\
			(strcmp(_p->typeId(), #_classname)==0)
	
	#define KLTMSG_TYPECAST(_p, _classname)	\
			(( strcmp(_p -> typeId(), #_classname)==0  )?\
			(  (_classname*) (KLTMSG::TimeoutObject*) _p) :\
				(_classname*)( KLSTD_THROW(KLSTD::STDE_BADHANDLE) , NULL )) 


	/*!
	*
	*	\class TimeoutStore2 - хранилище объектов с тайм-аутом и RTTI
	*
	*/
	

	class KLSTD_NOVTABLE TimeoutStore2 : public KLSTD::KLBase
	{
	public:

		/*!
		  \brief	Добавление объекта в хравнилище. При этом инкрементируется
			счётчик ссылок добавляемого объекта. По истечении указанного срока
			счётчик ссылок декрементируется.

		  \param	pItem - указатель на объект
		  \param	lTimeout - время жизни объекта в мс
		  \return	std::wstring - идентификатор объекта
		*/
		virtual std::wstring Insert(TimeoutObject* pItem, long lTimeout)=0;

		virtual void Insert(
				TimeoutObject* pItem,
				long lTimeout,
				const std::wstring& wstrKey)=0;

		/*!
		  \brief	Получение указателя на объект по идентификатору.

		  \param	wstrKey - Идентификатор объекта
		  \param	ppItem - указатель на переменную, в которую будет
					записан указатель на объект.

		  \exception STDE_NOTFOUND - объект с заданным идентификатором не найден.
		*/
		virtual void GetAt(const std::wstring& wstrKey, TimeoutObject** ppItem)=0;


		/*!
		  \brief	Досрочно удаляет объект из хранлища.

		  \param	wstrKey - Идентификатор объекта
		  \param	ppItem - опциональный указатель на переменную, в которую
					будет записан указатель на объект.
		  \return	true - если объект с заданным идентификатором был
					действительно обнаружен в хранилище
		*/
		virtual bool Remove(const std::wstring& wstrKey, TimeoutObject** ppItem=NULL)=0;


		/*!
		  \brief	Продлевает время жизни объекта. МЕТОД ЕЩЁ НЕ РЕАЛИЗОВАН !!!.

		  \param	wstrKey - Идентификатор объекта

		  \exception STDE_NOTFOUND - объект с заданным идентификатором не найден.
		*/
		virtual void Prolong(const std::wstring& wstrKey)=0;

        virtual void Clean() = 0;
	};	

	class KLSTD_NOVTABLE TimerCallback : public KLSTD::KLBase
	{
	public:
		virtual long OnTimerCallback(long id) = 0;
	};
	
	typedef KLTP::ThreadsPool::Worker		TPWorker;
	typedef KLTP::ThreadsPool::WorkerId		TPWorkerId;

	//! Template class for async call of class' method with no parameters
	template<class T>
	class AsyncCall0T : public KLSTD::KLBaseImpl<KLTMSG::TPWorker>
	{
	public:
		typedef void (T::*func_t)();
		AsyncCall0T(T* pObject, func_t f)
			:	KLSTD::KLBaseImpl<KLTMSG::TPWorker>()
			,	m_pObject(pObject)
			,	m_pMethod(f)
		{
			KLSTD_ASSERT(m_pObject && f);
		};
		virtual ~AsyncCall0T()
		{
			m_pObject=NULL;
		};

		virtual int RunWorker( KLTMSG::TPWorkerId wId )
		{
#ifndef __MWERKS__
			(m_pObject->*m_pMethod)();
#else
            // (operator ->*(m_pObject,m_pMethod))();
            ((*m_pObject).*m_pMethod)();
            // (m_pObject->operator ->*(m_pMethod))();
#endif
			return 0;
		};
        static void Start(
                        T* pObject, 
                        func_t f, 
                        KLTP::ThreadsPool::WorkerPriority priority = 
                                        KLTP::ThreadsPool::PriorityNormal)
		{
			KLSTD::CAutoPtr<KLTMSG::TPWorker> pWorker;
			pWorker.Attach(new AsyncCall0T<T>(pObject, f));
			KLTMSG::TPWorkerId idWorker=0;			
			KLTP_GetThreadsPool()->AddWorker2(
										&idWorker,
										L"KLTMSG::AsyncCall0T",
										pWorker,
										KLTP::ThreadsPool::RunOnce,
                                        true,
                                        -1,
                                        priority);
		};
	protected:
		KLSTD::CAutoPtr<T>	m_pObject;
		func_t				m_pMethod;
	};

	//! Template class for async call of class' method with 1 parameter
	template<class T, class P1>
	class AsyncCall1T : public KLSTD::KLBaseImpl<KLTMSG::TPWorker>
	{
	public:
		typedef void (T::*func_t)(P1 p1);
		AsyncCall1T(T* pObject, func_t f, P1 p1)
			:	KLSTD::KLBaseImpl<KLTMSG::TPWorker>()
			,	m_pObject(pObject)
			,	m_pMethod(f)
			,	m_p1(p1)
		{
			KLSTD_ASSERT(m_pObject && f);
		};

		virtual ~AsyncCall1T()
		{
			m_pObject=NULL;
		};

		virtual int RunWorker( KLTMSG::TPWorkerId wId )
		{
#ifndef __MWERKS__
          (m_pObject->*m_pMethod)(m_p1);
#else
          ((*m_pObject).*m_pMethod)(m_p1);
#endif
          return 0;
		};

		static void Start(
                        T* pObject, 
                        func_t f, 
                        P1 p1,
                        KLTP::ThreadsPool::WorkerPriority priority = 
                                        KLTP::ThreadsPool::PriorityNormal)
		{
			KLSTD::CAutoPtr<KLTMSG::TPWorker> pWorker;
			pWorker.Attach(new AsyncCall1T<T, P1>(pObject, f, p1));
			KLTMSG::TPWorkerId idWorker=0;			
			KLTP_GetThreadsPool()->AddWorker2(
										&idWorker,
										L"KLTMSG::AsyncCall1T",
										pWorker,
										KLTP::ThreadsPool::RunOnce,
                                        true,
                                        -1,
                                        priority);
		};

	protected:
		KLSTD::CAutoPtr<T>	m_pObject;
		func_t				m_pMethod;
		
		P1					m_p1;
	};

	//! Template class for async call of class' method with 2 parameters
	template<class T, class P1, class P2>
	class AsyncCall2T : public KLSTD::KLBaseImpl<KLTMSG::TPWorker>
	{
	public:
		typedef void (T::*func_t)(P1 p1, P2 p2);
		AsyncCall2T(T* pObject, func_t f, P1 p1, P2 p2)
			:	KLSTD::KLBaseImpl<KLTMSG::TPWorker>()
			,	m_pObject(pObject)
			,	m_pMethod(f)
			,	m_p1(p1)
            ,	m_p2(p2)
		{
			KLSTD_ASSERT(m_pObject && f);
		};

		virtual ~AsyncCall2T()
		{
			m_pObject=NULL;
		};

		virtual int RunWorker( KLTMSG::TPWorkerId wId )
		{
#ifndef __MWERKS__
          (m_pObject->*m_pMethod)(m_p1, m_p2);
#else
          ((*m_pObject).*m_pMethod)(m_p1, m_p2);
#endif
          return 0;
		};

		static void Start(
                        T* pObject, 
                        func_t f, 
                        P1 p1, 
                        P2 p2,
                        KLTP::ThreadsPool::WorkerPriority priority = 
                                        KLTP::ThreadsPool::PriorityNormal)

		{
			KLSTD::CAutoPtr<KLTMSG::TPWorker> pWorker;
			pWorker.Attach(new AsyncCall2T<T, P1, P2>(pObject, f, p1, p2));
			KLTMSG::TPWorkerId idWorker=0;			
			KLTP_GetThreadsPool()->AddWorker2(
										&idWorker,
										L"KLTMSG::AsyncCall2T",
										pWorker,
										KLTP::ThreadsPool::RunOnce,
                                        true,
                                        -1,
                                        priority);
		};

	protected:
		KLSTD::CAutoPtr<T>	m_pObject;
		func_t				m_pMethod;
		
		P1					m_p1;
        P2					m_p2;
	};

};

void KLCSC_DECL KLTMSG_CreateTimeoutStore(KLTMSG::TimeoutStore** ppStore);


void KLCSC_DECL KLTMSG_CreateTimeoutStore2(KLTMSG::TimeoutStore2** ppStore);


void KLCSC_DECL KLTMSG_GetCommonTimeoutStore(KLTMSG::TimeoutStore2** ppStore);


KLCSC_DECL long KLTMSG_SetTimer(KLTMSG::TimerCallback* pCallback, long lTimeout, long* plTimer = NULL);

KLCSC_DECL void KLTMSG_KillTimer(long nTimer);

KLCSC_DECL void KLTMSG_CleanCommonTimeoutStore();

#endif //__TIMEOUTSTORE_H__

// Local Variables:
// mode: C++
// End:
