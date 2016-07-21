/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Sync.h
 * \author Андрей Казачков, Дамир Шияфетдинов
 * \date 2002
 * \brief Объекты синхронизации.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KL_SYNC_H_6A0778F4_B132_45c7_AFEB_AC689542E38D
#define KL_SYNC_H_6A0778F4_B132_45c7_AFEB_AC689542E38D

#include "std/base/klbase.h"

namespace KLSTD{

#define KLSTD_INFINITE (-1L)

	/*!
		\brief Critical section

	*/
	class KLSTD_NOVTABLE CriticalSection: public KLBase
    {
	public:
		virtual unsigned long AddRef()=0;
		virtual unsigned long Release()=0;
		virtual void Enter()=0;
		virtual void Leave()=0;
	};	

	/*!
		\brief Класс для автоматиеского выхода из критической секции

	*/
	template<class T>
		class AutoCriticalSectionT{
	public:
		AutoCriticalSectionT(CriticalSection* pCritSec){
			m_pCritSec=pCritSec;
			if(m_pCritSec)
				m_pCritSec->Enter();
		};
		~AutoCriticalSectionT(){
			if(m_pCritSec)
				m_pCritSec->Leave();
		};
	private:
		T m_pCritSec;
	};

	typedef AutoCriticalSectionT< CAutoPtr<CriticalSection> > AutoCriticalSection;
	typedef AutoCriticalSectionT< CriticalSection* > AutoCriticalSectionST;

    /*KLCSAK_BEGIN_PRIVATE*/
	
    /*!
		\brief Семафор без блокировки доступа read-only

	*/
	class KLSTD_NOVTABLE FileSemaphoreNoReadLock: public KLBase
    {
	public:
		virtual void Enter(bool bReadOnly, long lTimeout)=0;
		virtual void Leave()=0;
        virtual void EnterForDelete(long lTimeout) = 0;
	};


	class AutoFileSemaphoreNoReadLock{
	public:
		AutoFileSemaphoreNoReadLock(FileSemaphoreNoReadLock* pSem, bool bReadOnly, long lTimeout=KLSTD_INFINITE)
		{
			m_pSem=pSem;
			m_pSem->Enter(bReadOnly, lTimeout);
		};

		~AutoFileSemaphoreNoReadLock()
		{
			m_pSem->Leave();
		};
	private:
		CAutoPtr<FileSemaphoreNoReadLock>	m_pSem;
	};

    /*KLCSAK_END_PRIVATE*/

	/*!
		\brief Класс объекта синхронизации семафор.
	*/

	class KLSTD_NOVTABLE Semaphore : public KLBase
    {
	public:
		/*!
            \brief Уменьшает значение семафора.

            \param msec [in] Время ожидания в милисекундах. Для бесконечного ожидания значени <0.
            \return true  - значение семафора бело успешно уменьшено
					false - истекло время ожидания

        */
		virtual bool Wait( int msec = (-1) )=0;
		/*!
			\brief Увеличивает значение семафора.
		*/
		virtual void Post()=0;
	};	

    /*KLCSAK_BEGIN_PRIVATE*/

	extern volatile bool g_bFailIfSignMismatch;
    extern volatile long g_cfSignMismatchModules;

    //! Read-write lock, recursion isn't supported. Very expensive !!!
    class ReadWriteLock : public KLSTD::KLBase
    {
    public:
        virtual void Lock(bool bWriter) = 0;
        virtual void Unlock(bool bWriter) = 0;
    };

    class AutoReadWriteLock
    {
    public:
        AutoReadWriteLock(ReadWriteLock* pLock, bool bExclusive = false)
            :   m_bExclusive(bExclusive)
            ,   m_pLock(pLock)
        {
            if(m_pLock)
                m_pLock->Lock(m_bExclusive);
        };

        ~AutoReadWriteLock()
        {
            if(m_pLock)
                m_pLock->Unlock(m_bExclusive);
        };

    protected:
        KLSTD::CAutoPtr<ReadWriteLock>  m_pLock;
        bool                            m_bExclusive;
    };
    /*KLCSAK_END_PRIVATE*/
} // namespace KLSTD

KLCSC_DECL bool KLSTD_CreateCriticalSection(KLSTD::CriticalSection** pCritSec);

/*KLCSAK_BEGIN_PRIVATE*/
KLCSC_DECL bool KLSTD_CreateCriticalSectionST(KLSTD::CriticalSection** ppCritSec);
/*KLCSAK_END_PRIVATE*/

/*KLCSAK_BEGIN_PRIVATE*/
KLCSC_DECL void KLSTD_CreateFileSemaphoreNoReadLock(
							const std::wstring& wsName,
							KLSTD::FileSemaphoreNoReadLock** ppSem);
/*KLCSAK_END_PRIVATE*/

//!\brief Функции созданания объекта типа семафор
KLCSC_DECL bool KLSTD_CreateSemaphore( KLSTD::Semaphore** pCritSec, int initValue );

/*!
	\brief Нитебезопасный инкремент переменной типа long.
	Данная функция не должна вызываться до вызова main.
*/
KLCSC_DECL long KLSTD_FASTCALL KLSTD_InterlockedIncrement(long volatile* x);

/*!
	\brief Нитебезопасный декремент переменной типа long.
	Данная функция не должна вызываться до вызова main.
*/
KLCSC_DECL long KLSTD_FASTCALL KLSTD_InterlockedDecrement(long volatile* x);

/*KLCSAK_BEGIN_PRIVATE*/
KLCSC_DECL void KLSTD_CreateReadWriteLock(KLSTD::ReadWriteLock** ppLock);
/*KLCSAK_END_PRIVATE*/

/*!
  \brief	Sleeps for lTimeout mlliseconds

  \param	lTimeout time to sleep, ms
*/
KLSTD_NOTHROW KLCSC_DECL void KLSTD_Sleep(long lTimeout) throw();

/*KLCSAK_BEGIN_PRIVATE*/
KLCSC_DECL void KLSTD_GetModuleLock(KLSTD::CriticalSection** ppCS);
/*KLCSAK_END_PRIVATE*/

#endif //KL_SYNC_H_6A0778F4_B132_45c7_AFEB_AC689542E38D

// Local Variables:
// mode: C++
// End:
