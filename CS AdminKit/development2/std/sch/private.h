 /*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Private.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Описание внутренних классов для модуля Scheduler.
 *
 */


#ifndef KLSCH_PRIVATE_H
#define KLSCH_PRIVATE_H

#include "std/base/klbase.h"
#include "std/thr/sync.h"


namespace KLSCH {

	/*!
    * \brief Класс LightMutex.
    *
    */ 

    class LightMutex
	{
	public:
		//!\ Класс автоматической заблокировки mutex
		class AutoUnlock 
		{
		public:
			AutoUnlock( LightMutex *mtx );
			~AutoUnlock();

		private:
			LightMutex *mtx;
		};

	public:
		LightMutex();
		~LightMutex();

		//!\brief Операция блокировки mutex
		void Lock();

		//!\brief Операция разблокировки mutex
		void Unlock();

	private:

		KLSTD::CAutoPtr<KLSTD::CriticalSection> criticalSection;
	};

	/*!
    * \brief Класс Semaphore.
    *
    */ 

    class Semaphore
	{
	public:
		Semaphore( int initCount = 1 );
		~Semaphore();

		/*!
            \brief Уменьшает значение семафора.

            \param msec [in] Время ожидания в милисекундах. Для бесконечного ожидания c_Infinite.
            \return true  - значение семафора бело успешно уменьшено
					false - истекло время ожидания

        */
		bool Wait( int msec );

		/*!
            \brief Увеличивает значение семафора.
		*/
		void Post();

	private:
		KLSTD::CAutoPtr<KLSTD::Semaphore> semHandle;
	};

	void Trace( int traceLevel, const char* format , ...);    

} // namespace KLSCH

#endif // KLSCH_PRIVATE_H

// Local Variables:
// mode: C++
// End:
