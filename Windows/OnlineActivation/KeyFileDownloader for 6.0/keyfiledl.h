/*!
	(С) 2003 "Kaspersky Lab"

	\file keyfiledl.h
	\author Guzhov Andrey
	\date 26.05.2003
	\brief Интерфейсные функции библиотеки скачивания ключей лицензирования из Internet.
*/

#ifndef KFD_KEYFILEDL_H
#define KFD_KEYFILEDL_H

#include <windows.h>

#ifdef KEYFILEDOWNLOADER_EXPORTS
	#define KFD_DLL_API __declspec(dllexport)
#else
	#define KFD_DLL_API __declspec(dllimport)
#endif

// Коды возврата интерфейсных функций.

const int ERR_SUCCESS			= 0;	//!< успешное завершение
const int ERR_BAD_HWND			= 1;	//!< некорректный дескриптор диалога
const int ERR_BAD_RID			= 2;	//!< некорректный идентификатор ресурса
const int ERR_BAD_KEY			= 3;	//!< неверные данные в поле "ключ"
const int ERR_BAD_URL			= 4;	//!< некорректный URL
const int ERR_BAD_FOLDER		= 5;	//!< некорректный целевой каталог
const int ERR_MEM_ALLOC			= 6;	//!< ошибка выделения памяти
const int ERR_CONV_ANSI			= 7;	//!< ошибка конвертации параметров в ansi строку (url, folder, agent)
const int ERR_THR_CREATE		= 8;	//!< ошибка создания рабочего потока
const int ERR_THR_STARTED		= 9;	//!< рабочий поток уже запущен
const int ERR_THR_STOPPED		= 10;	//!< рабочий поток не запущен
const int ERR_UNSPECIFIED		= 11;	//!< неспецифицированная ошибка выполнения операции


// сообщения об этапах выполнения

// категория "прогресс выполнения"
const int MSG_INITING			= 101;	// инициализация соединения
const int MSG_POSTING_DATA		= 102;	// отсыл данных на сервер
const int MSG_SAVING_TMP		= 103;	// сохранение лицензии во временном каталоге
const int MSG_VERIFYING			= 104;	// проверка лицензии
const int MSG_RETRYING			= 105;	// повторная попытка получения лицензии
const int MSG_SAVING_DST		= 106;	// сохранение лицензии в целевом каталоге
const int MSG_POSTING_STAT		= 107;	// отсылка статуса завершения операции на сервер
const int MSG_COMPLETED			= 108;	// задача успешно завершена

// категория "прогресс http соединения"
const int MSG_CONNECTING		= 701;	// соединение с сервером
const int MSG_RESOLVING			= 702;	// разрешение IP адреса
const int MSG_SENDING			= 703;	// отсыл сообщения
const int MSG_SENT				= 704;	// сообщение послано 
const int MSG_RECEIVING			= 705;	// получение ответа сервера
const int MSG_RECEIVED			= 706;	// ответа сервера получен
const int MSG_REDIRECTING		= 707;	// перенаправление

// сообщения об ошибках

// категория "ошибка выполнения"
const int MSGERR_CANCELLED		= 600;	// задача отменена
const int MSGERR_INITING		= 601;	// ошибка инициализации
const int MSGERR_FILLDATA		= 602;	// ошибка заполнения формы данными
const int MSGERR_SENDING		= 603;	// ошибка посыла http-запроса
const int MSGERR_BADCONTENT		= 604;	// сервер прислал некорректный контент
const int MSGERR_GETSTATUS		= 605;	// невозможно получить статус ответа
const int MSGERR_SAVETEMP		= 606;	// ошибка при сохранении временного файла
const int MSGERR_SAVEDST		= 607;	// ошибка при сохранении целевого файла
const int MSGERR_BADLICENSE		= 608;	// лицензия не действительна
const int MSGERR_CORRUPTLIC		= 609;	// получить не испорченную лицензию не удалось
const int MSGERR_BADLOCALDATE	= 610;	// неверная локальная дата на машине пользователя

const int SERVER_ERROR_MASK		= 0x1000;					// маска ошибок возвращаемых сервером активации
const int MSGERR_KEYNOTFOUND	= SERVER_ERROR_MASK | 600;	// лицензионный ключ не найден на сервере активации
const int MSGERR_BLOCKED		= SERVER_ERROR_MASK | 601;	// лицензионный ключи или код активации заблокирован
const int MSGERR_SRVERROR		= SERVER_ERROR_MASK | 603;	// внутренняя ошибка сервера активации
const int MSGERR_MALFORMEDREQ	= SERVER_ERROR_MASK | 620;	// недостаточно данных в запросе на акитвацию
const int MSGERR_WRONGCUSTORPWD	= SERVER_ERROR_MASK | 621;	// неверный ID пользователя или пароль
const int MSGERR_WRONACTGCODE	= SERVER_ERROR_MASK | 622;	// неверный код активации
const int MSGERR_PERIODEXPIRED	= SERVER_ERROR_MASK | 633;	// период активации истек
const int MSGERR_NOMOREATTEMPTS	= SERVER_ERROR_MASK | 634;	// количество попыток активации исчерпано

// http ошибки класса 4хх
const int MSGERR_CLNT_SYNTHAX	= 400;	// ошибка синтаксиса в запросе
const int MSGERR_CLNT_UNAUTH	= 401;	// необходима авторизация
const int MSGERR_CLNT_FORBID	= 403;	// доступ к ресурсу запрещен
const int MSGERR_CLNT_NOTFND	= 404;	// ресурс не найден
const int MSGERR_CLNT_BADMETH	= 405;	// метод запрещен
const int MSGERR_CLNT_NONACPT	= 406;	// не найдено информации требуемого формата
const int MSGERR_CLNT_PRXAUTHRQ	= 407;	// требуется прокси-авторизация
const int MSGERR_CLNT_TIMOUT	= 408;	// таймаут ожидания запроса
const int MSGERR_CLNT_STCONFL	= 409;	// конфликт - необходима доп. информация
const int MSGERR_CLNT_GONE		= 410;	// ресурс удален с сервера насовсем
const int MSGERR_CLNT_LENREQ	= 411;	// требуется длина содержимого
const int MSGERR_CLNT_PREFAIL	= 412;	// вычисление предусловия дало ложь
const int MSGERR_CLNT_TOOLARGE	= 413;	// запрос слишком велик
const int MSGERR_CLNT_URILONG	= 414;	// URI слишком длинный
const int MSGERR_CLNT_UNSMEDIA	= 415;	// не поддерживаемый формат данных
const int MSGERR_CLNT_OTHER		= 499;	// неспециф. ошибка на стороне клиента

// http ошибки классса 5хх
const int MSGERR_SERV_INT		= 500;	// внутренняя ошибка сервера
const int MSGERR_SERV_NSUP		= 501;	// функциональность не поддерживается
const int MSGERR_SERV_BADGATE	= 502;	// некорректный ответ от шлюза
const int MSGERR_SERV_UNVL		= 503;	// ресурс временно недоступен
const int MSGERR_SERV_GATIMO	= 504;	// таймаут ожидания ответа от шлюза
const int MSGERR_SERV_PROT		= 505;	// протокол не поддерживается сервером
const int MSGERR_SERV_OTHER		= 599;	// неспециф. ошибка на стороне сервера

const int MSGERR_HTTP_OTHER		= 799;	// неспециф. ошибка http


// статусы завершения операции получения лицензии
const wchar_t MSG_LIC_OK[]		= L"SUCCESSFUL. PID=%d, VER=%s";
const wchar_t MSG_LIC_BAD[]		= L"WRONG. PID=%d, VER=%s";
const wchar_t MSG_LIC_CORRUPT[]	= L"CORRUPTED. PID=%d, VER=%s";

#pragma pack(1)

//! Структура данных пользователя
/*!
	Необходима для передачи данных, вводимых пользователем в инсталяторе. 
	#pragma pack(1) необходима для выравнивания данных структуры на границу
	байта, т.к. инсталятор по-другому не понимает.
*/
struct CUserData
{
	int		nProdId;		//!< идентификатор устанавливаемого продукта
	LPWSTR	lpszProdVer;	//!< версия устанавливаемого продукта
	LPWSTR	lpszAgent;		//!< идентификатор агента HTTP (User-Agent)
	LPWSTR	lpszUrl;		//!< URL сервера активации в формате http://server/path
	LPWSTR	lpszDstFolder;	//!< целевой каталог для сохранения ключа
	LPWSTR	lpszKeyNum;		//!< ключ в формате XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
	LPWSTR	lpszCompany;	//!< название компании пользователя
	LPWSTR	lpszName;		//!< имя пользователя
	LPWSTR	lpszCountry;	//!< название страны пользователя
	LPWSTR	lpszCity;		//!< название города пользователя
	LPWSTR	lpszAddress;	//!< адрес пользователя
	LPWSTR	lpszTelephone;	//!< телефон пользователя
	LPWSTR	lpszFax;		//!< факс пользователя
	LPWSTR	lpszEmail;		//!< и-мейл пользователя
	LPWSTR	lpszWww;		//!< URL пользователя
	BOOL	bUseIeSettings;	//!< использовать настройки IE
	BOOL	bUseProxy;		//!< использовать прокси сервера для разрешения имен
	BOOL	bTryAutoAuth;	//!< пытаться авторизоваться автоматически
	LPWSTR	lpszProxyList;	//!< список прокси серверов, разделитель - пробел
	LPWSTR	lpszProxyBypass;//!< список адресов для которых не использовать прокси, разделитель - пробел
};

#pragma pack()

//! Функция получения ключа инсталлятором.
/*!
	Запускает рабочий поток и возвращает управление. В потоке выполняется 
	скачивание ключа из Internet по протоколу HTTP/1.0. В качестве адреса 
	используется параметр lpszUrl. Ключ проверятеся на корректность  путем 
	сравнения идентификатора продукта, содержащегося в ключе и переданного 
	в параметре CUserData.nProdId. Ключ не должен быть триальным, OEM, 
	апгрейдным, истекшим. Если эти условия выполняются, то ключ сохраняется 
	в целевом каталоге CUserData.lpszDstFolder.

	Статус выполнения потока сообщается посредством передачи сообщений элементу 
	nDlgItem диалога hDialog. Описания сообщений находятся в файле cmndefs.h.
	Сообщения посылаются функцией Win32 API SendMessage().

	\param hDialog			[in]	дескриптор диалога
	\param nDlgItem			[in]	идентификатор элемента диалога
	\param pUserData		[in]	указатель на структуру данных пользователя

	\return ERR_SUCCESS рабочий поток успешно создан
	\return ERR_BAD_HWND передан некорректный дескриптор диалога
	\return ERR_BAD_RID передан некорректный идентификатор элемента диалога
	\return ERR_BAD_KEY неверные данные в поле ключ
	\return ERR_BAD_URL передан неверный URL
	\return ERR_BAD_FOLDER передан неверный целевой каталог
	\return ERR_MEM_ALLOC ошибка выделения памяти
	\return ERR_CONV_ANSI ошибка конвертации параметров в ANSI
	\return ERR_THR_CREATE ошибка создания рабочего потока
	\return ERR_THR_STARTED рабочий поток уже запущен (при попытке повторного запуска)
*/
KFD_DLL_API int WINAPI StartDownloadKeyFile(
						HWND			hDialog,
						int				nDlgItem,
						const CUserData	*pUserData
);

//! Коллбек-функция оповещения о прогрессе получения ключа.
/*!
	Запускает рабочий поток и возвращает управление. В потоке выполняется 
	скачивание ключа из Internet по протоколу HTTP/1.0. В качестве адреса 
*/

typedef void (WINAPI* KFD_NOTIFICATION_CALLBACK)(void* pContext, unsigned int nStatusCode);

//! Функция получения ключа с использованием коллбека
/*!
	Работает аналогично функции StartDownloadKeyFile
*/
KFD_DLL_API int WINAPI StartDownloadKeyFileCallback(
						KFD_NOTIFICATION_CALLBACK fCallback,
						void* pContext,
						const CUserData	*pUserData
);

//! Функция синхронного получения ключа с использованием коллбека
/*!
	Работает аналогично функции StartDownloadKeyFile
*/
KFD_DLL_API int WINAPI StartDownloadKeyFileSyncCallback(
						KFD_NOTIFICATION_CALLBACK fCallback,
						void* pContext,
						const CUserData	*pUserData
);


//! Функция отмены получения ключа.
/*!
	Выставляет флаг завершения потока и возвращает управление. Вызвавшее
	приложение будет оповещено о завершении потока сообщением MSGERR_CANCELLED.

	\return ERR_SUCCESS флаг останова успешно выставлен
	\return ERR_THR_STOPPED рабочий поток не был запущен
*/
KFD_DLL_API int WINAPI StopDownloadKeyFile();

//! Функция получения имени файла ключа
/*!
	Должна вызываться после получения сообщения MSG_COMPLETED.

	\param szName	[out] буфер для сохранения имени файла
	\param pLength	[in, out] размер буфера

	\return размер имени файла в TCHAR без завершающего нуля
	\return 0 в случае ошибки, pLength содержит требуемый размер буфера
*/
KFD_DLL_API int WINAPI GetKeyFileName(TCHAR* szName, int* pLength);

//! Функция установки имени и пароля пользователя для авторизации на прокси-сервере. 
/*!
	Должна вызываться на коллбеке при получении сообщения MSGERR_CLNT_PRXAUTHRQ.

	\param szUserName	[in] имя пользователя
	\param szPassword	[in] пароль пользователя

	\return ERR_SUCCESS операция успешно выполнена, в противном случае GetLastError()
*/
KFD_DLL_API int WINAPI SetCredentialProxy(TCHAR* szUserName, TCHAR* szPassword);

//! Функция установки имени и пароля пользователя для авторизации на http-сервере. 
/*!
	Должна вызываться на коллбеке при получении сообщения MSGERR_CLNT_UNAUTH.

	\param szUserName	[in] имя пользователя
	\param szPassword	[in] пароль пользователя

	\return ERR_SUCCESS операция успешно выполнена, в противном случае GetLastError()
*/
KFD_DLL_API int WINAPI SetCredentialServer(TCHAR* szUserName, TCHAR* szPassword);

//! Функция инициализации библиотеки. 
/*!
	Должна вызываться до вызова остальных интерфейсных функций.

	\return ERR_SUCCESS инициализация успешно выполнена
*/
KFD_DLL_API int WINAPI InitLibrary();

//! Функция деинициализации библиотеки
/*!
	Должна вызываться перед выгрузкой библиотеки. Выставляет флаг
	завершения рабочего потока и ожидает его завершения.
	
	\warning Нельзя вызывать данную функцию в обработчике сообщений 
	от библиотеки, т.к. это приведет к DeadLock.

	\return ERR_SUCCESS деинициализация успешно выполнена
*/
KFD_DLL_API int WINAPI DeinitLibrary();

//! Функция тестирования кода сообщения на принадлежность к классу ошибок сервера активации.
/*!
	Клиент может вызывать данную функция чтобы определить, 
	является ли код сообщения ошибкой сервера активации.
	
	\return истина если код сообщения является ошибкой сервера активации
*/
KFD_DLL_API bool WINAPI IsActivationServerError(const int msg_code);

#define NATIVE_SERVER_ERROR(x) \
	((x & SERVER_ERROR_MASK & (~x)))


#endif // KFD_KEYFILEDL_H