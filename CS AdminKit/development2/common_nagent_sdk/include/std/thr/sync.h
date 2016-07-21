/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Sync.h
 * \author Андрей Казачков, Дамир Шияфетдинов
 * \date 2002
 * \brief Объекты синхронизации.
 *
 */

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

} // namespace KLSTD

KLCSC_DECL bool KLSTD_CreateCriticalSection(KLSTD::CriticalSection** pCritSec);



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


/*!
  \brief	Sleeps for lTimeout mlliseconds

  \param	lTimeout time to sleep, ms
*/
KLSTD_NOTHROW KLCSC_DECL void KLSTD_Sleep(long lTimeout) throw();


#endif //KL_SYNC_H_6A0778F4_B132_45c7_AFEB_AC689542E38D

// Local Variables:
// mode: C++
// End:
