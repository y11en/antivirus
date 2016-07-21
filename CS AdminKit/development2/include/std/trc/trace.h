/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Trace.h
 * \author Шияфетдинов Дамир
 * \date 2002
 * \brief Функции вывода трассировачной информации.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KL_STD_TRACE_H
#define KL_STD_TRACE_H

#include <stdarg.h>
#include <std/base/kldefs.h>

#ifdef N_PLAT_NLM
#include <wchar_t.h>
#else
#include <wchar.h>
#endif

#define KLTRACE1 KLSTD::Trace1
#define KLTRACE2 KLSTD::Trace2
#define KLTRACE3 KLSTD::Trace3
#define KLTRACE4 KLSTD::Trace4
#define KLTRACE5 KLSTD::Trace5

#ifdef _DEBUG
# define KLTRACED  KLSTD::Trace5
# define KLTRACED3 KLSTD::Trace3
# define KLTRACED4 KLSTD::Trace4
# define KLTRACED5 KLSTD::Trace5
#else
# define KLTRACED
# define KLTRACED3
# define KLTRACED4
# define KLTRACED5
#endif

#ifndef KLASSERT

#ifdef  _WIN32

#ifdef _DEBUG

#include <crtdbg.h>
#define KLASSERT(expr)		_ASSERT(expr)   
#define KLASSERTD(expr)		_ASSERT(expr)   
#define KLASSERTEQ( A, B)	_ASSERT( A==B )   
#define KLASSERTEQD( A, B)	_ASSERT( A==B )
#define KLASSERTLT( A, B)	_ASSERT( A<B )
#define KLASSERTLTD( A, B)	_ASSERT( A<B )
#define KLASSERTGT( A, B)	_ASSERT( A>B )
#define KLASSERTGTD( A, B)	_ASSERT( A>B )
#define KLASSERTNE( A, B)	_ASSERT( A!=B )
#define KLASSERTNED( A, B)	_ASSERT( A!=B )

#else //!_DEBUG

#define KLASSERT(expr)
#define KLASSERTD(expr)
#define KLASSERTEQ( A, B)
#define KLASSERTEQD( A, B)
#define KLASSERTLT( A, B)
#define KLASSERTLTD( A, B)
#define KLASSERTGT( A, B)
#define KLASSERTGTD( A, B)
#define KLASSERTNE( A, B)
#define KLASSERTNED( A, B)

#endif //_DEBUG

#else //!_WIN32

#include <assert.h>

#define KLASSERT(expr)		assert(expr)
#define KLASSERTD(expr)		/*empty*/
#define KLASSERTEQ( A, B)	assert( A==B )   
#define KLASSERTEQD( A, B)	/*empty*/
#define KLASSERTLT( A, B)	assert( A<B )
#define KLASSERTLTD( A, B)	/*empty*/
#define KLASSERTGT( A, B)	assert( A>B )
#define KLASSERTGTD( A, B)	/*empty*/
#define KLASSERTNE( A, B)	assert( A!=B )
#define KLASSERTNED( A, B)	/*empty*/

#endif //_WIN32

#endif //KLASSERT


namespace KLSTD{

	enum {
		TF_PRINT_DATETIME	= 0x1,	//!< Флаг вывода в строку трассировки текущих даты и времени
		TF_PRINT_MODULE		= 0x2,	//!< Флаг вывода имени модуля
		TF_PRINT_THREAD_ID  = 0x4,	//!< Вывода идентификатора потока. Если данный поток 
									//   зарегестрирован в системе трасировки ( см. SetTraceThreadName )
									//	 то также будет выведено его имя
		TF_ENABLE_FILE_CACHE= 0x8,  //!< Включает режим кеширования ОС для файла трассировки
		TF_PRINT_MILLISECONDS = 0x10,	//!< Флаг вывода милисекунд в текущей секунде
		TF_NOT_SYNCHRONIZE	= 0x20,	//!< Выключаем синхронизацию при выводе трассировки
        TF_ENABLE_DEBUGOUT  = 0x40  //!< Turns on OutputdDebugString usage
	};		

/*KLCSAK_BEGIN_PRIVATE*/
	/*!
		\brief Функция инициализации системы Trace.

		Данная функция должна быть вызвана до вызова любой функции трассировки.
		Функция не является потоконезависемой и должна вызываться в самом начале работы
		программы.
		Для деинициализации системы Trace необходимо вызвать функцию DeinitTrace.
		Если данная функция не была вызвана система трасировки вывода не производит
	*/
	KLCSC_DECL void InitTrace();

	/*!
		\brief Функция запуска системы Trace. 
		
		\param defaultTraceLevel [in]	Уровень трассировки ( 1..5 )		
		\param logFileName [in] Имя log файла. Если равен NULL, то вывод будеть 
						производиться в stdout
		\param traceFlags [in] Битовые флаги системы трассировки
	*/
	KLCSC_DECL void StartTrace( int defaultTraceLevel, char *logFileName, 
		int traceFlags = 0 );

	/*!
		\brief Функция производит остановку системы трассировки

		Данная функция не вызывает деинициализацию системы трассировки.
	*/
	KLCSC_DECL void StopTrace();
/*KLCSAK_END_PRIVATE*/
	/*!
		\brief Функция возвращает текущий статус трассировки

		\param curTraceLevel [out] Текущий уровень трассировки. Можеть быть NULL.
	*/
	KLCSC_DECL bool IsTraceStarted( int *curTraceLevel = NULL );
/*KLCSAK_BEGIN_PRIVATE*/
	/*
		\brief Функция деинициализация подсистемы трассировки

		Функция не является потокобезопасной и должна быть вызванна после завершения 
		всех вызовов к системе трассировки.
	*/
	KLCSC_DECL void DeinitTrace();
/*KLCSAK_END_PRIVATE*/
/*KLCSAK_BEGIN_PRIVATE*/
	/*!
		\brief Функция спецификации модулей участвующих в трассировке

		Если данная функция ни разу не была вызванна, то производиться трассировка всех 
		модулей процесса. Для выбора нескольких модулей данную функцию необходимо вызвать
		для каждего модуля отдельно.

		\param moduleName [in] Имя модуля для трассировки
		\param traceLevel [in] Уровень трассировки для собщений от данного модуля ( если равен
								нулю - используется общий уровень )
		\param logFileName [in] Имя log файла ( если равен NULL то исользуется общий log файл )
	*/
	KLCSC_DECL void StartTraceModule( const wchar_t *moduleName, int traceLevel = 0, 
		char *logFileName = NULL );

	/*!
		\brief Функия определения потоков участвующих в трассировке.

		Если даннау функция ни разу не была вызванна, то производиться трассировка всех
		потоков в процессе. Для выбора нескольких потоков данную функцию необходимо вызвать
		для каждего потока отдельно.

		\param threadName [in] Имя потока для трассировки
		\param traceLevel [in] Уровень трассировки для собщений данного потока ( если равен
								нулю - используется общий уровень )
		\param logFileName [in] Имя log файла ( если равен NULL то исользуется общий log файл )
	*/
	KLCSC_DECL void StartTraceThread( const wchar_t *threadName, int traceLevel = 0, 
		char *logFileName = NULL );


	/*!
		\brief Функция производит соотвествие между текущим потоком и указанным именем

		Указанное для потока имя используется для возможности фильтрации по потоку выводимых 
		подсистемой трасировочных собщений.

		\param threadName [in] Имя текущего потока
	*/
	KLCSC_DECL void SetTraceThreadName( const wchar_t *threadName );

	/*!
		\brief Функия удаляет текущий поток из внутреннего списка ( см. SetTraceThreadName )
	*/
	KLCSC_DECL void UnregisterTraceThread();

	/*!
		\brief Функция возвращает файловый указатель для вывода Trace сообщений. 
		
		\param logFileName [out] Путь файла Log
	*/
	KLCSC_DECL void GetTraceLogFileName( char *logFileName, int *strSizeInChar );

	/*!
		\brief Общая функция вывода Trace информации

		\param traceLevel [in] Уровень трасировки для вывода.
		\param module [in] Имя модуля, который производить вывод
		\param format [in] Форматная строка для вывода
		\param ... [in] Набор параметров для подстановки в форматную строку
	*/
/*KLCSAK_END_PRIVATE*/
	KLCSC_DECL void Trace(	int traceLevel, const wchar_t *module, const wchar_t *format, ... );

	//!\brief Общая функция трассировачного вывода 
	KLCSC_DECL void TraceCommon( int traceLevel, const wchar_t *module, const wchar_t *format,
		va_list args );

	/** @name Функции вывода трассировачной информации с определенными уровнями вывода*/		
	//@{		
	/** Вывод с traceLevel равным 1 */
	KLCSC_DECL void Trace1( const wchar_t *module, const wchar_t *format, ... );
	/** Вывод с traceLevel равным 2 */
	KLCSC_DECL void Trace2( const wchar_t *module, const wchar_t *format, ... );
	/** Вывод с traceLevel равным 3 */
	KLCSC_DECL void Trace3( const wchar_t *module, const wchar_t *format, ... );
	/** Вывод с traceLevel равным 4 */
	KLCSC_DECL void Trace4( const wchar_t *module, const wchar_t *format, ... );
	/** Вывод с traceLevel равным 5 */
	KLCSC_DECL void Trace5( const wchar_t *module, const wchar_t *format, ... );
	//@}

} // end namespace KLSTD

#define KLSTD_TRACE0(_level, _tmlstr)							\
			KLSTD::Trace(_level, KLCS_MODULENAME, _tmlstr);

#define KLSTD_TRACE1(_level, _tmlstr, arg1)						\
			KLSTD::Trace(_level, KLCS_MODULENAME, _tmlstr, arg1);

#define KLSTD_TRACE2(_level, _tmlstr, arg1, arg2)				\
			KLSTD::Trace(_level, KLCS_MODULENAME, _tmlstr, arg1, arg2);

#define KLSTD_TRACE3(_level, _tmlstr, arg1, arg2, arg3)			\
			KLSTD::Trace(_level, KLCS_MODULENAME, _tmlstr, arg1, arg2, arg3);

#define KLSTD_TRACE4(_level, _tmlstr, arg1, arg2, arg3, arg4)	\
			KLSTD::Trace(_level, KLCS_MODULENAME, _tmlstr, arg1, arg2, arg3, arg4);

#endif // KL_STD_TRACE_H

// Local Variables:
// mode: C++
// End:
