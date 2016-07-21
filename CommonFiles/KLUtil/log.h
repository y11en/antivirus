#ifndef __LOG_H
#define __LOG_H

#include "defs.h"
#include <windows.h>
#include <string>
#include <vector>
#include <stdarg.h>
#include <tchar.h>
#include "inl/smart_ptr.hpp"
#include "inl/synchro.h"
//#include "../Stuff/thread.h"

#pragma warning (disable: 4251) // need dll-linkage

namespace KLUTIL
{
	//! Типы сообщений.
	enum ELogMessageType
	{
		eLM_Error,		//!< "Ошибка" - произошёл сбой в работе, дальнейшее выполнение текущего действия невозможно.
		eLM_Warning,	//!< "Предупреждение" - произошёл сбой в работе, который удалось исправить.
		eLM_Info,		//!< "Информация" - сообщение о выполнении некоторого действия программой.
		eLM_Debug,		//!< "Отладка" - сообщение, содержащее отладочную информацию.
	};

	//! Уровень важности сообщений.
	enum ELogMessageLevel
	{
		L0 = 0,	//!< Наиболее важный.
		L1 = 1,
		L2 = 2,
		L3 = 3,
		L4 = 4,
		L5 = 5,
		LMAX = 255
	};

	//! Интерфейс "физического" лога. Осуществляет запись на носитель.
	class KLUTIL_API ILogMedia
	{
	public:
		virtual ~ILogMedia() {}
		//! Записывает сообщение.
		virtual void Write(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCTSTR pszDate,		//!< [in] Дата записи сообщения.
			LPCTSTR pszTime,		//!< [in] Время записи сообщения.
			LPCTSTR pszThreadId,	//!< [in] Идентификатор потока, из которого записано сообщение.
			LPCTSTR pszThreadName,	//!< [in] Имя потока, из которого записано сообщение.
			LPCTSTR pszModule,		//!< [in] Модуль, из которого записано сообщение.
			LPCTSTR pszMessage		//!< [in] Сообщение.
			) = 0;
		//! Разрешает, исключает или модифицирует сообщение.
		//!   \return false - Сообщение игнорируется.
		virtual bool Check(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCTSTR pszModule		//!< [in] Модуль, из которого записано сообщения.
			)
		{
			type; nLevel; pszModule;
			return true;
		};
		//! Возвращает, работает ли лог.
		//! Если лог был неудачно создан (например, кривое имя файла), узнать об этом можно здесь.
		virtual bool IsWorking() const { return true; }
	};

	typedef boost::shared_ptr<ILogMedia> TLogMediaPtr;

	//! Лог-заглушка.
	class KLUTIL_API CLogMediaProxy: public ILogMedia
	{
	public:
		CLogMediaProxy(const TLogMediaPtr& pLog = TLogMediaPtr());
		~CLogMediaProxy();
		void SetLog(const TLogMediaPtr& pLog);
		virtual void Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage);
		virtual bool Check(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszModule);
	private:
		TLogMediaPtr m_pLog;
		CCritSec m_cs;
	};

	//! Компоновщик "физических" логов.
	class KLUTIL_API CLogMediaColl: public ILogMedia
	{
		typedef std::vector<TLogMediaPtr> TMediaColl;
	public:
		CLogMediaColl();
		~CLogMediaColl();
		void Add(const TLogMediaPtr& pMedia);
		virtual void Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage);
		virtual bool Check(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszModule);
	private:
		TMediaColl m_MediaColl;
		CCritSec m_cs;
	};

	//! Интерфейс фильтрации сообщений лога.
	class KLUTIL_API IFilter
	{
	public:
		virtual ~IFilter() {}
		//! Разрешает, исключает или модифицирует сообщение.
		//!   \return false - Сообщение игнорируется.
		virtual bool Check(
			ELogMessageType& type,		//!< [in/out] Тип сообщения.
			ELogMessageLevel& nLevel,	//!< [in/out] Уровень важности (L0 - наиболее важный).
			LPCTSTR pszModule			//!< [in] Модуль, из которого записано сообщения.
			) = 0;
	};

	typedef boost::shared_ptr<IFilter> TFilterPtr;

	//! Фильтр по типу и уровню сообщения.
	//! Разрешает сообщения более приоритетного типа и сообщения указанного типа с важностью не ниже указанной.
	class KLUTIL_API CTypeLevelFilter: public IFilter
	{
		ELogMessageType m_type;
		ELogMessageLevel m_nLevel;
	public:
		//! Конструктор.
		CTypeLevelFilter(
			ELogMessageType type = eLM_Info,	//!< [in] Разрешает сообщения приоритетнee type.
			ELogMessageLevel nLevel = LMAX	//!< [in] Разрешает типа type с важностью не ниже nLevel.
			): m_type(type), m_nLevel(nLevel) {}
		virtual bool Check(ELogMessageType& type, ELogMessageLevel& nLevel, LPCTSTR)
		{
			return type < m_type || (type == m_type && nLevel <= m_nLevel);
		}
	};

	//! Фильтрующий лог.
	class KLUTIL_API CFilterLogMedia: public ILogMedia
	{
		typedef std::vector<TFilterPtr> TFilterColl;
	public:
		//! Конструктор.
		CFilterLogMedia(
			const TLogMediaPtr& pMedia	//!< [in] Лог, на который накладывается фильтр.
			);
		virtual ~CFilterLogMedia();
		//! Добавляет фильтр.
		void AddFilter(
			TFilterPtr pFilter	//!< [in] Фильтр.
			);
		virtual void Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage);
		virtual bool Check(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszModule);

	private:
		TLogMediaPtr m_pMedia;
		TFilterColl m_FilterColl;
		CCritSec m_cs;
	};

	//! Параметры создания лога.
	struct KLUTIL_API CLogParam
	{
		CLogParam(
			TLogMediaPtr pMedia = TLogMediaPtr(),	//!< [in]
			LPCTSTR pszModule = NULL	//!< [in]
			): m_pMedia(pMedia), m_pszModule(pszModule) {}
		TLogMediaPtr m_pMedia;
		LPCTSTR m_pszModule;
	};

	//! "Логический" лог.
	class KLUTIL_API CLogBase
	{
	public:
		virtual ~CLogBase();
		//! Возврашает "физический" лог данного лога.
		TLogMediaPtr GetMedia() const throw() { return m_pMedia; }

		//! Записывает форматированное сообщение.
		void Write(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCTSTR pszMessage,		//!< [in] Сообщение.
			...
			) throw()
		{
			va_list ap;
			va_start(ap, pszMessage);
			WriteVA(type, nLevel, NULL, pszMessage, ap);
			va_end(ap);
		}

		//! Записывает форматированное Unicode-сообщение.
		void WriteW(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCWSTR pszModule,		//!< [in] Имя модуля. (NULL - имя по умолчанию)
			LPCWSTR pszMessage,		//!< [in] Сообщение.
			...
			) throw()
		{
			va_list ap;
			va_start(ap, pszMessage);
			WriteVAW(type, nLevel, pszModule, pszMessage, ap);
			va_end(ap);
		}

		//! Записывает форматированное сообщение.
		virtual void WriteVA(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCTSTR pszModule,		//!< [in] Имя модуля. (NULL - имя по умолчанию)
			LPCTSTR pszMessage,		//!< [in] Сообщение.
			va_list args
			) throw();

		//! Записывает форматированное Unicode-сообщение.
		virtual void WriteVAW(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCWSTR pszModule,		//!< [in] Имя модуля. (NULL - имя по умолчанию)
			LPCWSTR pszMessage,		//!< [in] Сообщение.
			va_list args
			) throw();

		void Debug(LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Debug, L0, NULL, pszMessage, ap); va_end(ap); }
		void Debug(ELogMessageLevel nLevel, LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Debug, nLevel, NULL, pszMessage, ap); va_end(ap); }

		void Info(LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Info, L0, NULL, pszMessage, ap); va_end(ap); }
		void Info(ELogMessageLevel nLevel, LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Info, nLevel, NULL, pszMessage, ap); va_end(ap); }

		void Warning(LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Warning, L0, NULL, pszMessage, ap); va_end(ap); }
		void Warning(ELogMessageLevel nLevel, LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Warning, nLevel, NULL, pszMessage, ap); va_end(ap); }

		void Error(LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Error, L0, NULL, pszMessage, ap); va_end(ap); }
		void Error(ELogMessageLevel nLevel, LPCTSTR pszMessage, ...) throw() { va_list ap; va_start(ap, pszMessage); WriteVA(eLM_Error, nLevel, NULL, pszMessage, ap); va_end(ap); }
		
		//! Проверяет, попадёт ли это сообщение в лог.
		bool IsFiltered(ELogMessageType type, ELogMessageLevel nLevel);
		
		//! Устанавливает название потока.
		static void SetThreadName(
			LPCTSTR pszThreadName	//!< [in] Имя потока. Указатель должен быть валидным в течении работы потока.
			);
		
		//! Возращает устанавленное название потока.
		static LPCTSTR GetThreadName();
		
		//! Возвращает "физический" лог приложения. (Представляет собой CLogMediaProxy)
		static TLogMediaPtr GetAppLogMedia();
		
		//! Устанавливает "физический" лог приложения.
		static void SetAppLogMedia(TLogMediaPtr pLog);
		
		static TLogMediaPtr CreateConsoleMedia();
		
		static TLogMediaPtr CreateFileMedia(
			LPCTSTR pszFilename,	//!< [in] Имя файла.
			bool bAppend,	//!< [in] Флаг добавления в конец.
			bool bFlush	= false	//!< [in] Флаг сброса на диск после каждого Write.
			);
		
		enum EDebugMediaStartParams
		{
			DEBUGMEDIA_NORMAL = 0x1,
			DEBUGMEDIA_FORCE = 0x2,			//!< Создать даже если нет отладчика.
			DEBUGMEDIA_REPORTERRORS = 0x4,	//!< Выводить ошибки через _CrtDbgReport.
		};
		static TLogMediaPtr CreateDebugMedia(
			DWORD dwParam = DEBUGMEDIA_NORMAL	//!< [in] Создать даже если нет отладчика.
			);
		
		/*! Создаёт лог из описания в реестре.
		В указанном ключе могут находится следующие переменные:
		  REG_SZ	"TraceHeader" - Первой строкой в лог будет добавлен этот текст
		  REG_SZ	"TraceFile" - лог будет подключен к файлу, указанному в значении переменной
		  REG_DWORD	"TraceFileAppend" (def: 1) - флаг дописывать в конец файла
		  REG_DWORD	"TraceFileFlush" (def: 0) - флаг сброса на диск после каждой записи
		  REG_DWORD	"TraceToApp" = 1 - лог будет подключен к логу приложения.
		  REG_DWORD	"TraceToConsole" = 1 - лог будет подключен к окну консоли
		  REG_DWORD	"TraceToDebug" = 1 - лог будет подключен к логу отладчика (Debug Output).
		  На все эти логи можно наложить фильтр.
		    Возможны следующие фильтры:
				REG_SZ		"TraceType" = ["debug"|"info"|"warning"|"error"] - тип, сообщения ниже которого отображаться не будут.
				REG_DWORD	"TraceLevel" = [0|1|2|...] - уровень типа TraceType, сообщения ниже которого отображаться не будут.
		  Если необходимо накладывать фильтры индивидуально для каждого лога, или необходимо
		  несколько логов одного типа (например, писать в два файла), то в указанном ключе
		  можно завести подключи "Trace1" "Trace2" и т.д. (должны идти последовательно начиная с 1)
		  в которых можно указать дополнительные логи и фильтры.
		  Фильтры в ключе влияют и на подключи тоже!
		  Пример:
			HLKM\....\SomeComp
				REG_SZ		"TraceFile" = "c:\my.log"
			HLKM\....\SomeComp\Trace1
				REG_DWORD	"TraceToDebug" = 1
				REG_SZ		"TraceType" = "warning"
				REG_DWORD	"TraceLevel" = 5
			Означает: писать в файл всё, в дебаг все ошибки и предупреждения уровня 5 и ниже.


		   \return Лог, который можно передать в CLog.
		*/
		static TLogMediaPtr CreateMediaFromRegistry(
			HKEY hRoot,				//!< [in] Корневой элемент реестра.
			LPCTSTR pszPath,		//!< [in] Путь от корневого элемента, до раздела, содержащего описание лога.
			bool bAppLog = false	//!< [in] Для лога приложения должен быть true. Также устанавливает лог приложения (SetAppLogMedia()).
			);

	protected:
		//! Конструктор.
		CLogBase(
			TLogMediaPtr pMedia,	//!< [in] Лог.
			LPCTSTR pszModule		//!< [in] Имя модуля.
			);
		TLogMediaPtr m_pMedia;
		std::basic_string<TCHAR> m_szModule;
	};

	class KLUTIL_API CLocalLog: public CLogBase
	{
	public:
		//! Конструктор для под-компонента.
		CLocalLog(
			const CLogParam& param,	//!< [in] Параметры создания лога.
			LPCTSTR pszModule		//!< [in] Имя модуля по умолчанию.
			);
		//! Конструктор.
		CLocalLog(
			TLogMediaPtr pMedia,	//!< [in] Лог.
			LPCTSTR pszModule		//!< [in] Имя модуля.
			);
		//! Конструктор.
		CLocalLog(
			const CLogBase& log,	//!< [in] Лог.
			LPCTSTR pszModule		//!< [in] Имя модуля.
			);
		virtual ~CLocalLog();
	};

	class KLUTIL_API CLog: public CLogBase
	{
	public:
		//! Конструктор для под-компонента.
		CLog(
			const CLogParam& param,	//!< [in] Параметры создания лога.
			LPCTSTR pszModule		//!< [in] Имя модуля по умолчанию.
			);
		//! Конструктор для главного лога приложения или компонента.
		CLog(
			TLogMediaPtr pMedia,	//!< [in] Лог.
			LPCTSTR pszModule		//!< [in] Имя модуля.
			);
		//! Конструктор для под-компонента.
		CLog(
			const CLogBase& log,	//!< [in] Лог.
			LPCTSTR pszModule		//!< [in] Имя модуля.
			);
		virtual ~CLog();
		void SetParams(
			TLogMediaPtr pMedia,		//!< [in] Новый лог.
			LPCTSTR pszModule = NULL	//!< [in] Новое имя модуля. Может быть NULL.
			);
		//! Записывает форматированное сообщение.
		virtual void WriteVA(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCTSTR pszModule,		//!< [in] Имя модуля.
			LPCTSTR pszMessage,		//!< [in] Сообщение.
			va_list args
			) throw();
		//! Записывает форматированное Unicode-сообщение.
		virtual void WriteVAW(
			ELogMessageType type,	//!< [in] Тип сообщения.
			ELogMessageLevel nLevel,//!< [in] Уровень важности (L0 - наиболее важный).
			LPCWSTR pszModule,		//!< [in] Имя модуля.
			LPCWSTR pszMessage,		//!< [in] Сообщение.
			va_list args
			) throw();
	private:
		CCritSec m_cs;
	};


	//! "Логический" лог с фильтрацией.
	class KLUTIL_API CFilterLog: public CLog
	{
		typedef boost::shared_ptr<CFilterLogMedia> TFilterLogMediaPtr;
		TFilterLogMediaPtr GetFilterLogMedia() { return boost::static_pointer_cast<CFilterLogMedia>(GetMedia()); }
	public:
		CFilterLog(const CLogParam& param, LPCTSTR pszModule): CLog(param, pszModule)
		{
			TLogMediaPtr pMedia = GetMedia();
			if (pMedia)
				SetParams(TLogMediaPtr(new CFilterLogMedia(pMedia)));
		}
		virtual ~CFilterLog();
		void AddFilter(TFilterPtr pFilter) { if (GetFilterLogMedia()) GetFilterLogMedia()->AddFilter(pFilter); }
	};

} // namespace KLUTIL

#endif