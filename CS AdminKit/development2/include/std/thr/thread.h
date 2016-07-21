/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Thread.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Абстрактный интерфейс класса поддержки потоков
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/


#ifndef KLSTD_THREAD_H
#define KLSTD_THREAD_H

#include "std/base/klbase.h"
#ifdef __unix__
 #ifndef _stdcall
  #define _stdcall
 #endif
#endif

namespace KLSTD {

#ifndef N_PLAT_NLM 		// Novell Netware
	#define KLSTD_THREADDECL	_stdcall
#else
	#define KLSTD_THREADDECL
#endif

	typedef unsigned long ( KLSTD_THREADDECL *ThreadFunctionPtr)(void *arpg);

	class Thread
	{
	public:
		
		/*!\enum Приоритеты потоков */
		enum ThreadPriority
		{
			PriorityLow,
			PriorityNormal,
			PriorityHigh
		};

		virtual ~Thread() {}
	/*!
      \brief Функция создания и запуска потока.

      \param name [in] Имя потока.
	  \param func [in] Функция потока.
      \exception TRERR_INVALID_PARAMETER 
    */
        virtual void Start( const wchar_t *name, ThreadFunctionPtr func, 
			void *argp = NULL ) = 0;

	/*!
      \brief Функция ожидания завершения потока.

      \param timeOut [in] Время ожидания в миллисекундах <0 - ожидание пока поток не закончиться  
	  \return Результат операции. true - поток закончил свою работу
    */
        virtual bool Join( int timeOut = (-1) ) = 0;

	};

} // namespace KLSTD

//!\brief Функция создания объекта типа Thread
KLCSC_DECL bool KLSTD_CreateThread(KLSTD::Thread** pThread);

//!\brief Function returns id of current thread
KLCSC_DECL int KLSTD_GetCurrentThreadId();

//!\brief Регестрирует текущий поток в драйвере
void RegisterThreadInDriver();

//!\brief Дерегестрирует текущий поток в драйвере
void UnregisterThreadInDriver();

//!\brief Function returns pid of current process

KLCSC_DECL int KLSTD_GetCurrentProcessId();

/*KLCSAK_BEGIN_PRIVATE*/

//!\brief Изменяет приоритет текущего потока
KLCSC_DECL void KLSTD_SetCurrentThreadPriority( KLSTD::Thread::ThreadPriority priority );

/*!
	\brief Инициализация подсистемы регистрации потоков в драйвере
*/
void KLSTD_InitThreadRegistration();
void KLSTD_DeInitThreadRegistration();

/*!
  \brief	KLSTD_InitializeThreads - должна быть вызвана при инициализации STD.
*/
void KLSTD_InitializeThreads();

/*!
  \brief	KLSTD_DeinitializeThreads - должна быть вызвана при деинициализации STD.
*/
void KLSTD_DeinitializeThreads();

/*KLCSAK_END_PRIVATE*/
#endif // KLSTD_THREAD_H
