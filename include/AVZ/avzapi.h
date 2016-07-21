#ifndef _AVZAPI_H_
#define _AVZAPI_H_

#ifndef WINAPI
#	define WINAPI __stdcall
#endif
#ifndef IN
#	define IN 
#endif
#ifndef OUT
#	define OUT 
#endif
#if !defined(__IStream_INTERFACE_DEFINED__)
	struct IStream;
#endif

#ifndef PRAGUE_OLD_DEFINES
#include <Prague/pr_compl.h>
#include <Prague/pr_types.h>
#else
#include <pr_compl.h>
#include <pr_types.h>
#endif

// calling convention for AVZ functions
#define AVZCALL

// Коды ошибок AVZ
enum AVZERROR
{
	AVZ_OK                   = 0x00000000, // Ошибок нет
	AVZ_Error                = 0xC0000000, // Ошибка (широкого плана, без уточнения)
	AVZ_InvalidKernelHandle  = 0xC0000001, // Некорректный хендл ядра
	AVZ_InvalidDBPath        = 0xC0000002, // Некорректный путь к базам
	AVZ_LoadDBError          = 0xC0000003, // Ошибка загрузки баз
	AVZ_Cancel               = 0xC0000004, // Аварийный стоп
	AVZ_InvalidStream        = 0xC0000005, // Некорректный поток - ожидалось получить указатель на поток, а его нет
	AVZ_StreamReadError      = 0xC0000006, // Ошибка чтения данных из потока
	AVZ_XMLParseError        = 0xC0000007, // Ошибка парсинга XML
	AVZ_XMLTokenNotFound     = 0xC0000008, // XML тег не найден
	AVZ_InvalidFormatMask    = 0xC0000009, // Некорректная маска, задающая формат преобразования
	AVZ_InvalidCallbackID    = 0xC000000A, // Некорректный код колбека
	AVZ_InvalidScriptID      = 0xC000000B, // Некорректный ID скрипта
	AVZ_InvalidScriptMode    = 0xC000000C, // Некорректный ID режима запуска скрипта
	AVZ_InvalidScriptDBHandle= 0xC000000D, // Некорректный хендл базы
	AVZ_InvalidScript        = 0xC000000E, // Ошибка в скрипте
};
#define AVZ_SUCC(e) (e) == AVZ_OK
#define AVZ_FAIL(e) (e) != AVZ_OK

struct CAVZScanKernel;

// Типы событий AVZ, приходящих в ScriptCallback
enum AVZMSG
{
	logNormal = 0,
	logInfo   = 1,
	logAlert  = 2,
	logVirus  = 3,
	logGood   = 4,
	logError  = 10,
	logDebug  = 11,
};


//////////////////////////////////////////////////////////////////////////
//
//	Коллбеки
//	Во всех коллбеках первым параметром должен идти контекст задачи
//
	enum AVZCallback_t
	{
		avzcb_ScriptCallback = 0,	// Соответствует tScriptCallback
		avzcb_GetLocalizedStr,		// Соответствует tGetLocalizedStr
		avzcb_FreeLocalizedStr,		// Соответствует tFreeLocalizedStr
		avzcb_CheckFileCallback,	// Колбек проверки файла
		avzcb_LAST,
	};
	// Определение коллбека исполнения скрипта:
	// pTaskContext - контекст запущенного скрипта (для обратной связи коллбека с объектом, вызвавшим AVZ_GetSomeInfo)
	// остальные параметры - описание события (для отображения, например, прогресс-бара)
	// Если коллбек возвращает AVZ_Cancel, то дальнейшее исполнение скрипта прекращается и AVZ_GetSomeInfo тоже возвращает AVZ_Cancel.
	// Распределяешь память под szMsg AVZ, убивает сразу после возврата управления из коллбека. Я буду просто копировать эти строки для своих нужд в коллбеке.
	typedef  AVZERROR (  WINAPI *tScriptCallback ) ( 
		IN void* pTaskContext,
		IN tDWORD dwProgressCur,   // Номер текущего события
		IN tDWORD dwProgressMax,   // Общее количество события (м.б. приблизительным – я его отмасштабирую для прогресс-бара)
		IN wchar_t* szMsg,        // Описание события (надо придумать, как его локализовывать)
		IN tDWORD    dwMsgSize,    // Размер строки описания события
		IN AVZMSG   dwMsgType     // Тип события
		);
	//////////////////////////////////////////////////////////////////////////
	//
	// Проверка файла из-под avzkrnl внешним антивирусом (выполняется по команде скрипта CheckFile)
	//
		enum AVZCheckResult_t
		{
			avzcr_Error = 0xC0000000,      // - ошибка (файл не найден или к нему нет доступа)
			avzcr_Clean = 0,               // - файл не значится в базе безопасных и не опознан как вредоносный
			avzcr_VirusFound = 1,          // - файл опознан как вредоносный объект
			avzcr_Suspicious = 2,          // - файл не значится в базе безопасных и подозревается как вредоносный объект
			avzcr_CleanBase = 3,           // - файл опознан по базе чистых объектов внешнего антивируса		
		};
		typedef  AVZERROR ( WINAPI *tCheckFile ) ( 
			IN void* pTaskContext,				// Контекст задачи
			IN wchar_t* szFileName,				// Имя файла для проверки
			OUT AVZCheckResult_t* pCheckResult	// Результат проверки
		);
	//
	// Проверка файла из-под avzkrnl внешним антивирусом
	//
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//
	// Коллбеки локализации
	//
		// Получение строки по идентификатору
		typedef  AVZERROR ( WINAPI *tGetLocalizedStr ) ( 
			IN void* pTaskContext,    // Контекст задачи
			IN  char* szID,           // Идентификатор локализуемой строки
			OUT wchar_t** pszResult   // Локализованная строка
			);
		// Освобождение буфера
		typedef  AVZERROR ( WINAPI *tFreeLocalizedStr ) ( 
			IN void* pTaskContext,    // Контекст задачи
			IN wchar_t* szResult      // Указатель на буфер с локализованной строкой для освобождения памяти
			);
	//
	// Коллбеки локализации
	//
	//////////////////////////////////////////////////////////////////////////
//
//	Коллбеки
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  Инициализация AVZ. 
//
	AVZERROR AVZ_InitializeExA( 
			 OUT CAVZScanKernel** ppAVZAScanKernel, // Контекст ядра AVZ
			 IN const char* szBasesPath,            // Путь к базам AVZ
			 IN const char* szProductType           // Тип продукта
			 );	
	AVZERROR AVZ_InitializeExW( 
			 OUT CAVZScanKernel** ppAVZAScanKernel, // Контекст ядра AVZ
			 IN const wchar_t* szPath,              // Путь к базам AVZ
			 IN const wchar_t* szProductType        // Тип продукта
			 );
	AVZERROR AVZ_SetCallbackCtx(
			IN CAVZScanKernel* pAVZAScanKernel,     // Контекст ядра AVZ
			IN void* pTaskContext                   // Контекст задачи. Будет приходит первым параметром в каждом коллбеке
			);
	AVZERROR AVZ_SetCallbackProc(
			IN CAVZScanKernel* pAVZAScanKernel,     // Контекст ядра AVZ
			IN AVZCallback_t dwCallBackID,			// Идентификатор коллбека
			IN void* pCallBackProc					// Указатель на коллбек
			);
	// Окончание работы с AVZ
	AVZERROR AVZ_Done( 
			 IN CAVZScanKernel* pAVZAScanKernel     // Контекст ядра AVZ
			 );
//
//  Инициализация AVZ. 
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//	Работа с базами
//
	enum AVZ_DBStatus_t
	{
		AVZ_DBStatus_MainBase		= 1, // - главная база -
		AVZ_DBStatus_Macros			= 2, // - база анализатора макросов Office (пока не применяется) -
		AVZ_DBStatus_Clean			= 4, // - база чистых +
		AVZ_DBStatus_ScriptsHeur	= 8, // - база скриптов эвристической проверки системы +
		AVZ_DBStatus_ScriptsVul		= 16, // - база скриптов эвристической проверки системы, подсистема ИПУ (искатель уязвимостей) +
		AVZ_DBStatus_ScriptsRecovery= 32, // - база скриптов восстановления системы +
		AVZ_DBStatus_Scripts		= 64, // - база стандартных скриптов +
		AVZ_DBStatus_Memory			= 128, // - база сканера памяти -
		AVZ_DBStatus_MemoryHeur		= 256, // - база экспертной системы для сканера памяти -
		AVZ_DBStatus_Drivers		= 512, // - база ядраных комппонент +/- (когда я перейду на драйвер AVP, то -)
	};
	// Получение статуса загрузки баз AVZ (возвращает битовую маску из DBStatus_t)
	tDWORD    AVZ_GetDBStatus( 
			 IN CAVZScanKernel* pAVZAScanKernel     // Контекст ядра AVZ
			 );

	// Перегрузка баз AVZ (после апдейта)
	AVZERROR AVZ_ReloadBase( 
			 IN CAVZScanKernel* pAVZAScanKernel     // Контекст ядра AVZ
			 );		
//
//	Работа с базами
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//	Работа с AVZ
//
	// Проверка файла антивирусным ядром AVZ (неиспользуемая функция)
	AVZERROR AVZ_CheckFile( 
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN char* szFileName,                   // Имя файла
			 IN OUT char* szVerdict                 // Результат проверки
			 );

	// Исполнение произвольного скрипта AVZ и получение результата
	AVZERROR AVZ_GetSomeInfo( 
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN wchar_t* szScript,                  // Текст скрипта
			 IN void* pScriptContext,               // Контекст скрипта (this)
			 IN OUT IStream* pXMLStream             // Стрим для получения результат работы скрипта
			 );

	// Упрощенное исполнение скрипта
	// { return AVZ_GetSomeInfo(pAVZAScanKernel, szScript, NULL, NULL, NULL); };
	AVZERROR AVZ_RunScript( 
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN wchar_t* szScript                   // Текст скрипта
			 );

	// Получение из XML готовой структуры (или связанного списка однотипных структур)
	// Формат описания структуры (szBufFormat):
	//		типы: 
	//			ftByte (1 байт без знака),
	//			ftWord (2 байта без знака), 
	//			ftDWORD (4 байта без знака), 
	//			ftInteger (4 байта со знаком),  
	//			ftPChar - указатель на Ascii строку c завершающим нулем,
	//			ftPWChar - указатель на Unicode строку c завершающим нулем. 
	//		Разделители:
	//			запятая - разделитель между типом и дефолтным значением
	//			точка с запятой - разделитель описания полей
	//		Дефолтное значение:
	//			число в десятичном формате (для цифровых типов) 
	//			строка (без кавычек, символы ",", ";" и "=" в строке не допустимы)
	//		Пробел не является разделителем
	//		Количество пробелов не регулируется форматом
	// Пример szBufFormat:
	//		L"PID=ftDWORD,0; File=ftPWChar,; CheckResult=ftInteger,-1; Descr=ftPWChar,; LegalCopyright=ftPWChar,; CmdLine=ftPWChar,; Size=ftDWORD,0; Attr=ftPWChar,; CreateDate=ftPWChar,; ChageDate=ftPWChar,; MD5=ftPWChar,"
	//		Эта строка создаст связанный список следующего формата (с соблюдением порядка и установкой дефолтных значений, указанных в szBufFormat):
	// 		struct record_t
	// 		{
	// 			record_t* m_pNextItem; // Это поле указывает на следующий элемент связанного списка. При отсутствии связи = NULL
	// 			tDWORD     m_dwPID;
	// 			wchar_t*  m_szFile; 
	// 			int       m_iCheckResult; 
	// 			wchar_t*  m_szDescr; 
	// 			wchar_t*  m_szLegalCopyright; 
	// 			wchar_t*  m_szCmdLine; 
	// 			tDWORD     m_szSize; 
	// 			wchar_t*  m_szAttr; 
	// 			wchar_t*  m_szCreateDate; 
	// 			wchar_t*  m_szChageDate; 
	// 			wchar_t*  m_szMD5;
	// 		}
	// Функция может формировать специальные коды ошибок:
	// AVZ_InvalidStream - функции передан указатель на объект ISTREAM = NULL
	// AVZ_StreamReadError - ошибка чтения данных из потока
	// AVZ_XMLParseError - ошибка в ходе парсинга XML
	// AVZ_XMLTokenNotFound - указанный тег не найден
	// AVZ_InvalidFormatMask - ошибки в описании формата структуры
	AVZERROR AVZ_DeserializeXML( 
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN IStream* pXMLStream,                // Стрим с XML
			 IN wchar_t* szTagName,                 // Имя тега первого уровня, чье содержимое будет преобразовано в структуру
			 IN wchar_t* szBufFormat,               // Описание структуры
			 OUT void** ppAVZBuffer,                // Указатель на первый элемент связанного списка структур
			 OUT tDWORD* pdwAVZBufSize              // Размер pAVZBuffer (нужен только для отладки)
			 );

	// Освобождение буфера, выделенного в ядре AVZ.
	// Используется после функции AVZ_DeserializeXML
	AVZERROR AVZ_Free( 
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN void* pAVZBuffer                    // Буфер для освобождения
			 );

	//////////////////////////////////////////////////////////////////////////
	//
	// Исполнение скрипта, хранящегося в базе AVZ
	//	Любой скрипт устроен так, что Check/Fix можно вызывать многократно без ущерба для системы. 
	//	Check/Fix симметричны, т.е. в теории можно после Fix сразу еще раз вызвать Check и 
	//	если он вернет 0, то это сигнал для нас о том, что проблема действительно была исправлена ...
	AVZERROR AVZ_LoadCustomScriptDB(
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 wchar_t* szDBFilename,					// Имя скриптовой базы
			 OUT void** ppDBHandle					// Контекст скриптовой базы
			 );
	AVZERROR AVZ_FreeCustomScriptDB(
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN void* pDBHandle						// Контекст скриптовой базы
			 );
	AVZERROR AVZ_GetCustomScriptCount(
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN void* pDBHandle,					// Контекст скриптовой базы
			 OUT tDWORD* pdwCount					// Количество скриптов
			 );
	enum CustomScriptRunMode_t 
	{
		csmode_Check = 1, 
		csmode_Fix = 2,
		csmode_SuperFix = 3,
		csmode_CheckUndo = 4,
		csmode_Undo = 5,
	};
	enum csCheckResult_t
	{
		csres_Check_Unknown = -1,
		csres_Check_Clean = 0,		// проблема не найдена либо уже пофиксена
		csres_Check_FoundLow = 1,	// найдена незначительная проблема, которая скорее всего не окажет значимого влияния на безопасность и работоспособность системы
		csres_Check_FoundMedium = 2,// найдена проблема средней тяжести
		csres_Check_FoundHigh = 3,	// найдена опасная проблема, которую настоятельно рекомендуется немедленно пофиксить
	};
	enum csFixResult_t
	{
		csres_Fix_OK = 0,			// Фиксация выполнена 
		csres_Fix_Fail = 1,			// Фиксация не удалась
		csres_Fix_NeedReboot = 2,	// Фиксация выполнена, требуется перезагрузка компьютера
	};
	enum csCheckUndoResult_t
	{
		csres_CheckUndo_NotFound = 0,	// Откат невозможен
		csres_CheckUndo_OK = 1,			// Откат возможен
	};
	enum csUndoResult_t
	{
		csres_Undo_OK = 0,			// Откат выполнен
		csres_Undo_Fail = 1,		// Откат не удался
		csres_Undo_NeedReboot = 2,	// Откат выполнен, требуется перезагрузка компьютера
	};
	AVZERROR AVZ_RunCustomScript(
			 IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
			 IN void* pDBHandle,					// Контекст скриптовой базы
			 IN tDWORD dwScriptID,					// идентификатор скрипта (отсчет от 0 до N-1). Если индекс вне диапазона, то прерывание с ошибкой AVZ_InvalidScriptID  = C000000B
			 IN tDWORD dwScriptMode,				// режим запуска скрипта (csmode_Check/csmode_Fix/...). Если код не 1 или 2, то прерывание с ошибкой AVZ_InvalidScriptMode = C000000C
			 OUT tDWORD* pdwResult					// для csmode_Check:csCheckResult_t, для csmode_Fix и csmode_SuperFix:csFixResult_t, для csmode_CheckUndo:csCheckUndoResult_t, для csmode_Undo:csUndoResult_t
			 );
	//
	// Исполнение скрипта, хранящегося в базе AVZ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	//	Связка AVZ с эмулятором
	//
		// Начало обработки файла
		AVZERROR AVZ_SR_Init(
			IN CAVZScanKernel* pAVZAScanKernel,     // Контекст ядра AVZ
			IN const wchar_t* szFileName,			// Имя обрабатываемого файлы
			OUT tDWORD* pdwSessionID				// Идентификатор сессии 
			);
		// Окончание обработки файла
		AVZERROR AVZ_SR_Done(
			IN tDWORD dwSessionID
			);
		// типы событий для AVZ_SR_AddEvent
		enum AVZ_SR_Event_t {
			AVZ_SR_EVENT_API_STR = 0,				// буфер содержит текстовое представление вызванной API-функции
		};
		// Добавление события. Вызывается эмулятором многократно для передачи различных событий о поведении изучаемого файла
		AVZERROR AVZ_SR_AddEvent(
			IN tDWORD dwSessionID, 					// Идентификатор сессии 
			IN tDWORD dwEventType,					// код типа события
			IN const void* pEventData,				// указатель на буфер с данными, описывающими событие
			IN tDWORD dwEventDataSize				// размер данных
			);
		// Передача блока памяти для анализа. Вызывается эмулятором многократно
		AVZERROR AVZ_SR_Memory(
			IN tDWORD dwSessionID, 					// Идентификатор сессии 
			IN tDWORD dwProcessID,					// Идентификатор процесса
			IN tDWORD qwBaseAddr,					// Базовый адрес
			IN const void* pMemory,					// указатель на буфер памяти
			IN tDWORD dwDataSize					// Размер буфера
			);
		// типы рейтингов для AVZ_SR_GetSecurityRating
		enum AVZ_SR_SecurityRating_t {
			AVZ_SR_RATING_SEVERITY = 0,				// общий рейтинг опасности/зловредности по N-балльной шкале
		};
		// Запрос SecurityRating. ARatingType - код типа рейтинга
		AVZERROR AVZ_SR_GetSecurityRating(
			IN tDWORD dwSessionID, 					// Идентификатор сессии 
			IN tDWORD ARatingType,					// код типа рейтинга
			OUT tDWORD* pRes						// Рейтинг
			);
	//
	//	Связка AVZ с эмулятором
	//
	//////////////////////////////////////////////////////////////////////////
//
//	Работа с AVZ
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  Typedefs for explicit import
//
	typedef AVZERROR (AVZCALL * tAVZ_InitializeExW)( 
		OUT CAVZScanKernel** ppAVZAScanKernel, // Контекст ядра AVZ
		IN const wchar_t* szPath,              // Путь к базам AVZ
		IN const wchar_t* szProductType        // Тип продукта
		);
	typedef AVZERROR (AVZCALL * tAVZ_Done)( 
		IN CAVZScanKernel* pAVZAScanKernel     // Контекст ядра AVZ
		);
	typedef AVZERROR (AVZCALL * tAVZ_SR_Init)(
		IN CAVZScanKernel* pAVZAScanKernel,     // Контекст ядра AVZ
		IN const wchar_t* szFileName,			// Имя обрабатываемого файлы
		OUT tDWORD* pdwSessionID				// Идентификатор сессии 
		);
	typedef AVZERROR (AVZCALL * tAVZ_SR_Done)(
		IN tDWORD dwSessionID
		);
	typedef AVZERROR (AVZCALL * tAVZ_SR_AddEvent)(
		IN tDWORD dwSessionID, 					// Идентификатор сессии 
		IN tDWORD dwEventType,					// код типа события
		IN const void* pEventData,				// указатель на буфер с данными, описывающими событие
		IN tDWORD dwEventDataSize				// размер данных
		);
	typedef AVZERROR (AVZCALL * tAVZ_SR_Memory)(
		IN tDWORD dwSessionID, 					// Идентификатор сессии 
		IN tDWORD dwProcessID,					// Идентификатор процесса
		IN tDWORD qwBaseAddr,					// Базовый адрес
		IN const void* pMemory,					// указатель на буфер памяти
		IN tDWORD dwDataSize					// Размер буфера
		);
	typedef AVZERROR (AVZCALL * tAVZ_SR_GetSecurityRating)(
		IN tDWORD dwSessionID, 					// Идентификатор сессии 
		IN tDWORD ARatingType,					// код типа рейтинга
		OUT tDWORD* pRes						// Рейтинг
		);
//
//	Typedefs for explicit import
//
//////////////////////////////////////////////////////////////////////////



#endif//_AVZAPI_H_
