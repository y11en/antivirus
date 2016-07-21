/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Transport.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Главный интерфейс транспортного модуля для связи агент-продукт
 *
 *  Данный файл содержит описание абстрактного интерфейса транспортного модуля 
 *  для связи агент-продукт Transport.
 *  Данный модуль построен на библитеки gSOAP и предназначен для контроля состоянием
 *  текущих соединений и реализует механизм обработки клиентских запросов на серверной 
 *  стороне.
 */


#ifndef KLTR_TRANSPORT_H
#define KLTR_TRANSPORT_H

#include <string>
#include <list>

#include <std/par/params.h>

#include <kca/prci/componentid.h>
#include <transport/wat/common.h>
#include <transport/wat/authserver.h>
#include <transport/tr/trafficmanager.h>

#include "common.h"
#include "errors.h"
#include "notifier.h"

// forward declaration
struct soap;

namespace KLTRAP {	

	const int g_TransportCurrentVersion = 2;

//#define KL_OPENSSL_SUPPORT
	
/*!
 * \brief Главный интерфайс транспортного модуля.
 *	
 *		Имена соединений кодируется транспортным именем компоненты. Для конвертации
 *			ComponentId в траспортный вид и обратно пользуйтесь функциям 
 *			( ConvertComponentIdToTransportName и ConvertTransportNameToComponentId см. ниже )
 */ 	

	const int c_DefaultMaxNumberOfConns = 200;
	const int c_DefaultInactiveConnTimeout = 120000;

	const wchar_t	c_TRLocalLocationPrefic[]	= L"http://127.0.0.1";
	const wchar_t	c_TRLocationPrefic[]		= L"http://";
	const int		c_TRLocationPreficLen		= 7;

	const wchar_t c_AnyAddrLocation[]	= L"*any_addr*";

	const int c_DefaultNumberOfConncurentThreads = 5;

	//!< Функции обработки запросов клиента библиотекой gSOAP
	typedef int (*SoapServeFunc)( struct soap *soap );	

	//! Список имен соединений
	typedef std::list<std::wstring> ConnectionsNameList;

	class ConnectionControl : public KLSTD::KLBaseQI
	{
	public:
		
		enum ActionType
		{
			ConnectionEstablished,	// Если функция CheckConnection вернет false для данного 
									// действия соединение будет принудительно закрыто
			UnusedConnectionCheck	// В данном действие производиться проверка возможности
									// закрыть данное соединени как давно неиспользуемое и не 
									// критичное для базовой функциональности. true - данное 
									// соединение может быть закрыто. Траснпортная подсистема 
									// закрывает неиспользованные соединения только если достигнут 
									// лимит количества соедиенений в данном процессе
		};

		virtual bool CheckConnection( ActionType actionType, 
				const std::wstring &localName,
				const std::wstring &remoteName,
				bool remoteConnection ) = 0;
	};

	
	/*!\typedef Описание прокси для соединения */
	struct ProxyInfo{
		std::wstring	location;		//!< Location прокси сервера ( format: http://proxy_host:proxy_port )
		std::wstring	authUserName;	//!< Имя пользователя, если прокси требует аутентификации
		std::string		authUserPswd;	//!< Пароль пользователя
	};

	class Transport 
	{
	public:

		/*!\enum Коды оповещении изменения статуса соединений */
		enum ConnectionStatus{			
			ComponentConnected,			//!< Произошло соединения с удаленной компонентой
			ComponentDisconnected,		//!< Соединение с удаленной компонентой разорванно
			ConnectionEstablished,		//!< Соединение было успешно установелнно
			ConnectionBroken,			//!< Соединение разорвано			
			UnicastPacketArrived,		//!< Пришел unicast пакет
			PingTimeoutElapsed			//!< Истекло время Ping'а
		};
		
		/*!\enum Приоритеты соединений */
		enum ConnectionPriority
		{
			PriorityLow		= 1,
			PriorityNormal	= 0,		// значение по умолчанию
			PriorityHigh	= 2
		};

		/*!\struct Описатель location'а */
		struct Location{
			Location() { unicastConnectFlag = useSSLUnicast = bCompressTraffic = false; }
			Location( const Location &l ) { 
				location			= l.location;				
				remoteComponentName = l.remoteComponentName;
				unicastConnectFlag	= l.unicastConnectFlag;
				useSSLUnicast		= l.useSSLUnicast;
                proxyInfo           = l.proxyInfo;
				bCompressTraffic	= l.bCompressTraffic;
			}			
			std::wstring		remoteComponentName;	//!< Имя удаленной компоненты
			std::wstring		location;				//!< URL
			bool				unicastConnectFlag;		//!< Признак unicast соединения
			bool				useSSLUnicast;			//!< Для посылки unicast используется подпись SSL ( сертификаты беруться у существующего listener'а )			
			ProxyInfo			proxyInfo;				//!< Информация о прокси сервере ( используется если location в структуре не пустая строка )
			bool				bCompressTraffic;		//!< Использовать сжатия трафика для указанного колена
		};

		/*!\typedef Список описателей*/
		typedef std::vector<Location> LocationsList;

		/*!\struct Описатель соединения*/
		struct ConnectionFullName{
			ConnectionFullName() {}
			ConnectionFullName( const ConnectionFullName &c ) {
				localComponentName = c.localComponentName;
				remoteComponentName = c.remoteComponentName;
			}
			std::wstring		localComponentName;		//!< Имя локальной компоненты
			std::wstring		remoteComponentName;	//!< Имя удаленной компоненты
		};

		/*!\typedef Список описателей соединений*/
		typedef std::vector<ConnectionFullName> ConnectionsFullNameList;


		/*!\struct Расширенный описатель location'а */
		struct LocationEx{
			LocationEx() { unicastConnectFlag = useSSLUnicast = false; userData = NULL; }
			LocationEx( const LocationEx &l ) { 
				location			= l.location;
				ports				= l.ports;
				remoteComponentName = l.remoteComponentName;
				localComponentName	= l.localComponentName;
                userData            = l.userData;
				unicastConnectFlag  = l.unicastConnectFlag;
				useSSLUnicast		= l.useSSLUnicast;
			}
			bool operator == ( const LocationEx &lex )
			{
				if (	remoteComponentName==lex.remoteComponentName &&
						localComponentName==lex.localComponentName &&
						location==lex.location &&
						ports==lex.ports &&
						unicastConnectFlag==lex.unicastConnectFlag &&
						userData==lex.userData ) return true;
				else
					return false;
			}
			std::wstring		remoteComponentName;	//!< Имя удаленной компоненты
			std::wstring		localComponentName;		//!< Используется для поиска уже существующего соедиенения ( только в случае если location пустой )
			std::wstring		location;				//!< URL без порта
			std::vector<short>	ports;					//!< Список портов для подключения 
			void				*userData;				//!< Данные пользователя ( контроль за памятью должен осуществлять вызывающий код )
			bool				unicastConnectFlag;		//!< Признак unicast соединения
			bool				useSSLUnicast;			//!< Для посылки unicast используется подпись SSL ( сертификаты беруться у существующего listener'а )
		};

		/*!\typedef Список описателей*/
		typedef std::vector<LocationEx> LocationExList;
		
				
		/*!\struct Параметры для создания соединения */
		struct ConnectionInfo
		{
			ConnectionInfo()
			{
				pingTimeout = c_Undefined;
				priority = PriorityNormal;
				pCustomCredentials = NULL;
				pCustomAuthClientCtx = NULL;
				pRemotePublicSSLKey = NULL;
				pLocalPublicSSLKey = NULL;
				bConnForGateway = false;
				pProxyInfo = NULL;
				bMakeTwoWayAuth = false;
				bCompressTraffic = false;
				bCreateSSLConnection = false;
				bSimpleConnection = false;
				bDontCloseByTimeoutError = false;				
			}

			std::wstring		localName;			//!< Локальное имя компоненты
			std::wstring		remoteName;			//!< Удаленное имя компоненты
			std::wstring		remoteLocation;		//!< Адрес удаленной компоненты

			int					pingTimeout;		//!< Период пингования созаденного соединения
			ConnectionPriority	priority;			//!< Приоритет создаваемого соединения

			KLPAR::Params	   *pCustomCredentials;		//!< Используется для пользовательской аутонтификации
			KLWAT::ClientContext *pCustomAuthClientCtx;	//!< Сохраняется локально для данного соединения

			bool				bCreateSSLConnection;	//!< Создавать SSL соединение даже если pRemotePublicSSLKey == NULL
			KLPAR::BinaryValue *pRemotePublicSSLKey;	//!< Публичный ключ SSL удаленной компоненты
			KLPAR::BinaryValue *pLocalPublicSSLKey;		//!< Публичный ключ SSL локальной компоненты компоненты 
														// используется спец. схема авторизации
			bool				bConnForGateway;		//!< Флаг использования данного соединения для gateway

			const ProxyInfo		*pProxyInfo;			//!< Информация для создания соединения через прокси сервер

			bool				bMakeTwoWayAuth;		//!< Для данного соединения будет проводиться двухстороная аутонтификация

			bool				bCompressTraffic;		//!< Признак сжатие трафика для данного соединения

			bool				bSimpleConnection;		//!< При создании данного соединения не производиться handshake. На серверной
														// стороне для созданного соединения будет автоматически сгенерированно уникальное имя.
														// Аутентификацонный hanshake также проводиться не будет
			
			bool				bDontCloseByTimeoutError;		//!< Признак не закрывать соединения в случае возникновения ошибки timeout expired
		};
		

		/*!\enum Состояние соединения в Broeadcast вызовах */
		enum BroadcastState {
			BroadcastConnectionEstablished,		//!< Установленно соединения
			BroadcastConnectError,				//!< Невозможно установить соединение
			BroadcastFinished					//!< Выполнении broadcast завершено
		};

		typedef int BroadcastId;

		class Broadcast : public KLSTD::KLBase
		{
		public:
			/*!\brief Callback функция broadcast вызова */
			virtual int BroadcastCallback( BroadcastId broadcastId, 
				BroadcastState state, 
				const wchar_t *localComponentName, 
				const wchar_t *remoteComponentName,
				void *userData ) = 0;
		};

	/*!
		\brief Callback - функция для оповещении об изменении статуса соединений

		
		\param status  [in]  Статус соединения
		\param remoteComponentName [in] Транспортное имя компоненты с которой производилось соединение
		\param localComponentName [in] Транспортное имя компоненты от лица которой 
				было установленно данное соединение		

		\return Пока не используется
	*/
		typedef int (*StatusNotifyCallback)( Transport::ConnectionStatus status, 
			const wchar_t *remoteComponentName, const wchar_t *localComponentName,
			bool remoteConnFlag );

		virtual ~Transport() {}

	/*!
      \brief Функция инициализации транспорта. 

      \param maxNumberOfConns [in] Максимальное количество соединений.      
      \exception TRERR_INVALID_PARAMETER 
    */
        virtual void Initialize( int maxNumberOfConns, SoapServeFunc soapServe = NULL ) = 0;

	/*!
	  \brief Устанавливает серверный режим работы
	*/
		virtual void SetIOPortMode( bool usePort = true, 
			int numberOfThreads = c_DefaultNumberOfConncurentThreads ) = 0;
		
	/*!
      \brief Функция установки timeout'ов.

      Указанные при вызове данной функции значение будет использованы в дальнейшем
	  в качестве timeout для базовых транспортных операций.

      \param recvTimeout [in] Таймаут для операции receive ( в милисекундах ).
      \param sendTimeout [in] Таймаут для операции send ( в милисекундах ).
	  \param connectTimeout [in] Таймаут для операции connect и accept ( в милисекундах ).
      \exception TRERR_INVALID_PARAMETER 
    */
        virtual void SetTimeouts( int recvTimeout, int sendTimeout, int connectTimeout ) = 0;
		
	/*!
	  \brief Функции для установки timeout'ов

	  \param recvTimeout [in] Таймаут для операции receive ( в милисекундах ).
      \param sendTimeout [in] Таймаут для операции send ( в милисекундах ).
	  \param connectTimeout [in] Таймаут для операции connect и accept ( в милисекундах ).
      \exception TRERR_INVALID_PARAMETER 
    */
        virtual void SetRemoteCallsTimeouts( int recvTimeout, int sendTimeout, int connectTimeout ) = 0;

	/*!
	  \brief Функции для установки timeout'ов

	  \param recvTimeout [in] Таймаут для операции receive ( в милисекундах ).
      \param sendTimeout [in] Таймаут для операции send ( в милисекундах ).
	  \param connectTimeout [in] Таймаут для операции connect и accept ( в милисекундах ).
      \exception TRERR_INVALID_PARAMETER 
    */
        virtual void SetLocalCallsTimeouts( int recvTimeout, int sendTimeout, int connectTimeout ) = 0;

	/*!
	  \brief Функция установки connectTimeout
	
	  \return Предудущее значение
	 */
		virtual int SetConnectTimeout( int connectTimeout ) = 0;

	/*!
      \brief Функциия установки callback функции.

      Данная callback функция предназначена для оповешения клиента о изменении статуса
	  контролируемого соединения. Функция обработки событий транспорта должна после своей 
	  работы вызвать предыдущую функциу. При деинициализации подсистема должна
	  установить в транспорт адрес предыдущей функции обработки.
	  
      \param newCallback [in] callback функция для оповешения.      
	  \return Предыдущую функцию обработки или NULL
    */
        virtual StatusNotifyCallback SetStatusCallback( StatusNotifyCallback newCallback ) = 0;

	/*!
      \brief Функциия установки callback функции.

		В отличии от функции SetStatusCallback callbaсk фунция вызывается без блокировки 
		объекта транспорта и может быть вызвана одновременно из разных потоков.
	 */
		virtual StatusNotifyCallback SetStatusCallback2( StatusNotifyCallback newCallback ) = 0;

	/*!
	  \brief Возвращает интерфейс для регистрации получателей нотификаций от транспорта

	  Имеет расширенные функции по сравнению с использованием SetStatusCallback
	*/
		virtual KLTR::TransportNotifier *Notifier() = 0;

	/*!
      \brief Функциия добавляет location для доступа к данной компоненте из другого процесса.
      

	  \param localComponentName [in] Имя компоненты от лица которой происходит добавление
				location'а.
      \param location [in] location для прослушивания ( формат http://127.0.0.1:port ).      
	  \param usedPort [out] Номер порта, на котором будет осуществлятся listen. Если в location
			порт не указан ( http://127.0.0.1 ) или равен 0 ( http://127.0.0.1:0 ) порт будет
			выбран автоматически.

	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
    */
        virtual void AddListenLocation( const wchar_t *localComponentName, 
			const wchar_t *location, int &usedPort ) = 0;

	/*!
	  \brief Добавляет SSL location для доступа к данной компоненте из другого процесса.

	  \param  binaryPrivateSSLKey [in] Приватный ключ серверного процесса
	  \param  binaryPublicSSLKey [in] Паблик SSL ключ 
	  \param  keyPassword [in] Пароль для приватного ключа
	*/
		virtual void AddSSLListenLocation( const wchar_t *localComponentName, 
			const wchar_t *location, int &usedPort, 
			KLPAR::BinaryValue *binaryPrivateSSLKey,
			KLPAR::BinaryValue *binaryPublicSSLKey,
			const wchar_t *keyPassword ) = 0;

	/*!
      \brief Функциия добавляет location для прослущивания unicast пакетов.

	  При приходе unicast пакета будет вызванна callback функция транспорта со статусом
	  UnicastPacketArrived.

	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
    */
        virtual void AddUnicastListenLocation( int unicastPort ) = 0;

	/*!
      \brief Функциия удаляет location для доступа к данной компоненте.

	  \param localComponentName [in] Имя компоненты от лица которой был добавлен данный
				location.
      \param location [in] location для прослушивания ( формат http://127.0.0.1:port ).      
	  \param usedPort [out] Номер порта, который был возвращен функцией AddListenLocation в
			момент добавления данного location'а.
	  \exception TRERR_INVALID_PARAMETER, TRERR_NOT_FOUND_CONNECTION	  
    */
        virtual void DeleteListenLocation( const wchar_t *localComponentName, 
			const wchar_t *location, int usedPort ) = 0;
		
	/*!
      \brief Функциия удаляет location для доступа к данной компоненте и закрывается 
		все созданные к данной компоненте соединения.

	  \param localComponentName [in] Имя компоненты от лица которой был добавлен данный
				location.
      \param location [in] location для прослушивания ( формат http://127.0.0.1:port ).      
	  \param usedPort [out] Номер порта, который был возвращен функцией AddListenLocation в
			момент добавления данного location'а.
	  \exception TRERR_INVALID_PARAMETER, TRERR_NOT_FOUND_CONNECTION	  
    */
        virtual void DeleteListenerAndCloseConnections( const wchar_t *localComponentName, 
			const wchar_t *location, int usedPort ) = 0;


	/*!
      \brief Функциия удаляет unicast listener.
    */
        virtual void DeleteUnicastListenLocation( int unicastPort ) = 0;

	/*!
      \brief Функциия добавляет клиентское соединение.

      Функция пытается установить клиентское соединение с сервером.

	  \param localComponentName [in] Наименование клиента, от лица которого производиться 
				данное соединения. Данная имя будет использованно на строне сервера в 
				качестве имени обратного соединения. Является транспортным имененм компоненты.
	  \param remoteComponentName [in]	Имя клиентского соединения ( используется в дальнейшем для доступа к данному соединению ).
				Данное имя является транспортным именем удаленной компоненты.
      \param remoteServerLocation [in] location удлаенного сервера ( формат http://ip_addr:port ).	  
	  \param pingTimeout [in] Временной период вызова Ping метода от сервера к клиенту.
	  \param connForGateway [in] Данное соедиенение будет использоваться для gateway. Если установлен 
				данный флаг, то конкретно данное соедиенения не будет использоваться для распространения 
				подписок
	  \param priority [in] Приоритет создаваемого соединения
	  \param makeTwoWayAuthentication [in] Флаг установки двухнаправленной аутентификации.
				По умолчаню аутентификация производиться только на вызываемой стороне
				
	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_CONN_ALREADY_EXISTS,
			TRERR_CONN_HANSHAKE
	  
	  \return Результат операции false - соединение с указанным адресом не установеленно
    */
        virtual bool AddClientConnection( const wchar_t *localComponentName, 
			const wchar_t *remoteComponentName, const wchar_t *remoteServerLocation,
			int pingTimeout = c_Undefined, bool connForGateway = false,
			const KLPAR::Params * pCustomCredentials = NULL,
			ConnectionPriority priority = PriorityNormal,
			bool makeTwoWayAuthentication = false,
			KLWAT::ClientContext *customAuthClientCtx = NULL,
			const ProxyInfo* proxyInfo = NULL ) = 0;

	/*!
      \brief Функциия добавляет клиентское SSL соединение.

	  \param  binaryPublicSSLKey [in] Паблик SSL ключ
	  \param  keyPassword [in] Пароль для приватного ключа
	*/		
        virtual bool AddSSLClientConnection( const wchar_t *localComponentName, 
			const wchar_t *remoteComponentName, const wchar_t *remoteServerLocation,			
			KLPAR::BinaryValue *binaryPublicSSLKey, const KLPAR::Params * pCustomCredentials = NULL,
			int pingTimeout = c_Undefined, bool connForGateway = false,
			ConnectionPriority priority = PriorityNormal,
			KLWAT::ClientContext *customAuthClientCtx = NULL,
			const ProxyInfo* proxyInfo = NULL ) = 0;

	/*!
	  \brief Функция посылает Unicast пакет и ждет создания соединение удаленной стороной

	*/
		virtual bool SendUnicastAndWaitConnection( const wchar_t *remoteLocation,
			const wchar_t *waitRemoteComponentName, const wchar_t *waitLocalComponentName,
			int timeout ) = 0;

	/*!
      \brief Закрывает существующее клиентское соединение.
        
	  \param localComponentName [in] Наименование клиента, от лица которого было созданно 
				данное соединения.
	  \param remoteComponentName [in]	Имя клиентского соединения	  	  	  
	  
	  \return Результат операции false - указаннное соединение не найдено в списке контролируемых соединений
    */
        virtual bool CloseClientConnection( const wchar_t *localComponentName, 
			const wchar_t *remoteComponentName ) = 0;

	/*!
	  \brief Возвращает список имен активных соединений

	  \param connsList [out] Список активных соединений
	*/
		virtual void GetConnectionsList( ConnectionsNameList &connsList ) = 0;

	/*!
	  \brief Возвращает список имен активных соединений

	  \param connsList [out] Список активных соединений
	*/
		virtual void GetConnectionsList( ConnectionsFullNameList &connsList ) = 0;

	/*!
	  \brief Определяет активность указанного соединения
	  
	  \param remoteComponentName [in] Имя соединения для проверки
	  \return признак активности
	*/
		virtual bool IsConnectionActive( const wchar_t *remoteComponentName ) = 0;

	/*!
	  \brief Определяет активность указанного соединения
	  
	  \param localComponentName [in] Наименование клиента, от лица которого было созданно 
				данное соединения.
	  \param remoteComponentName [in] Имя соединения для проверки
	  \return признак активности
	*/
		virtual bool IsConnectionActive( const wchar_t *localComponentName,
			const wchar_t *remoteComponentName ) = 0;

	/*!
      \brief Фукция возвращает location указанного соединения.

      \param remoteComponentName [in] Имя клиентского соединения 
	  \param result [out]  location сервера ( формат http://ip_addr:port ).
    */
        virtual void GetLocation( const wchar_t *remoteComponentName, std::wstring &location ) = 0;

	/*!
	   \brief Функция опеределяет представляет ли транспорт указанную компоненту

		Транспорт пердставлет компоненту в том случае если от лица компоненты был добавлен 
		location для доступа ( функция AddListenLocation )

		\param componentName [in] Транспортное имя компоненты
	*/
		virtual bool IsLocalComponent( const wchar_t *componentName ) = 0;

	/*!
      \brief Возвращает структуру soap для указанного клиентского соединения.

	  Возвращаемая данной функции структура SOAP можеть быть использованна клиентом для 
	  осуществления вызовов серверных функций. Возвращаемая структура не должна ни каким образом 
	  модифицироваться клиентом.

	  \param localComponentName [in] Наименование клиента, от лица которого будет производиться вызов с 
						использованием возвращенной структуры
      \param remoteComponentName [in] Имя клиентского соединения 
	  \param lockConnection [in] Флаг блокировки данного соединения от вызовов из других потоков. Блокировка
					снимается вызовом функции FreeClientConnection.
	  \param result [out] Указатель на структуру soap. Если равен NULL, то описание данного соединение отсутствует
		( см. функцию
    */
        virtual void GetClientConnection( const wchar_t *localComponentName, const wchar_t *remoteComponentName, 
			bool lockConnection, struct soap **result ) = 0;

	/*!
	  \brief Освобождает структуру soap выданную функцией GetClientConnection

	   После вызова данной функции пользоваться структуру soap нельзя

	   \param soap [in] Указатель на структуру soap. 
	   \param unlockConnection [in] Признак снятия блокировки с соединения
	*/
		virtual void FreeClientConnection( struct soap *soap, bool unlockConnection ) = 0;

	/*!
      \brief Возвращает временной период для отправки сообщения Ping.

	  Если сервер установил оперделенное время для Ping'а, то клиент должен с указанным
	  интервалом осуществлять отсылку Ping сообщения при помощи функции SendPingMessage

	  \param localComponentName [in] Имя клиента, от лица которого было установелнно соединения
      \param remoteComponentName [in] Имя соединения
	  \return Временной период. Если равен 0 то сервер не требует отслыки Ping сообщения 
				от клиента
    */
        virtual int GetPingTimeout( const wchar_t *localComponentName, const wchar_t *remoteComponentName ) = 0;

	/*!
      \brief Функция осуществляет отправку ping сообщения.

	  \param localComponentName [in] Имя клиента, от лица которого было установелнно соединения
      \param remoteComponentName [in] Имя клиентского соединения
    */
        virtual bool SendPingMessage( const wchar_t *localComponentName, const wchar_t *remoteComponentName ) = 0;

	/*!
	  \brief Функция определения имени соединения по структуре SOAP.
		
		  Данная функция проверяет статус соединения, представленного данной структурой soap
		  и если соединение активно возвращает имя найденного соединения.

	  \param soap [in] Структура soap описания соединения
	  \param connName [out] Имя найденного соединения
	  \result false - если данное соединение уже неактивно
	*/
		virtual bool GetConnectionName( const struct soap *soap, std::wstring &connName ) const = 0;

	/*!
	  \brief Функция определения имени соединения по структуре SOAP.
		
		  Данная функция проверяет статус соединения, представленного данной структурой soap
		  и если соединение активно возвращает имя найденного соединения.

	  \param soap [in] Структура soap описания соединения
	  \param connName [out] Полное имя соединения найденного соединения
	  \result false - если данное соединение уже неактивно
	*/
		virtual bool GetConnectionName( const struct soap *soap, ConnectionFullName &connName ) const = 0;

	/*!
	  \breif Функция производит блокировку процедуры соединения для конкретного процесса

	  \param remoteClineName [in] Имя компоненты ( в транспортном виде ) для блокировки процеддуры
	*/
		virtual void LockConnectHandshake( const wchar_t *remoteClientName ) = 0;

	/*!
	  \brief Снимает блокировку с процедуры соединения
	*/
		virtual void UnlockConnectHandshake() = 0;

	/*!
	  \brief Данная функция создает описание для broadcast вызова

	  Broadcast пытается подключиться к переданным адресам и после успешного 
	  подключения вызывает callback функцию с соотвествующим статусом передавая
	  в нее описатель созданного соединения.

	  \param localComponentName [in] Транспортное имя компоненты от лица которой 
				будет установленно данное соединение		
	  \param locsList [in] Список адресов
	  \param callback [in] Callback функция, которая будет вызванна для каждого соединения
				см. описание формата функции
	  \param callbackContext [in] Контекст для callback функции
	  \exception TRERR_INVALID_PARAMETER
	*/
		virtual BroadcastId CreateBroadcast( const wchar_t *localComponentName, 
			LocationExList &locsList, Broadcast *broadcast ) = 0;

	/*!
	  \brief Функция запускает broadcast вызов

	   В данную функцию передается идентификатор полученный из функции CreateBroadcastCall

	   \exception TRERR_INVALID_PARAMETER
	*/
		virtual void StartBroadcast( const BroadcastId broadcastId ) = 0; 

	/*!
	  \brief Функция останавливает работу broadcast вызова

	  \param broadcastId [in] Идентификатор вызова
	   \exception TRERR_INVALID_PARAMETER
	*/
		virtual void StopBroadcast( const BroadcastId broadcastId ) = 0;

	/*!
	  \brief Функция останавливает gateway соединения

	  \param localComponentName [in] Наименование клиента, от лица которого производиться 
				данное соединения. 
	  \param locsList [in]	Список узлов, через которые должено быть установлено соединение      
	  \param pingTimeout [in] Временной период вызова Ping метода от сервера к клиенту.
				
	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_CONN_ALREADY_EXISTS,
			TRERR_CONN_HANSHAKE
	  
	  \return Результат операции false - соединение с указанным адресом не установеленно
	*/
		virtual bool CreateGatewayConnection( const wchar_t *localComponentName, 
			LocationsList &locsList, int connectTimeout = c_Undefined,			
			ConnectionPriority priority = PriorityNormal,			
			KLWAT::ClientContext *customAuthClientCtx = NULL ) = 0;

	/*!
	  \brief Функция позволяющая обработать входящие запросы к транспорту

	  \param timeOut [in] Время ожидания входящих запросов в милиссекундах. Если timeout 
				равен 0 то функция вополнит только текущие ожидающие запросы
	*/
		virtual int ProcessRequests( int timeOut = 0 ) = 0;

	/*!
	   \brief Функция опеределяет тип соединения с которым ассоциированна структура soap

		\param soap [in] указатель на структуру soap, полученную функцией GetClientConnection
	*/
		virtual bool IsLocalConnection( const struct soap *soap ) = 0;
		
	/*!
      \brief Функциия добавляет клиентское SSL соединение.	  
	*/		
        virtual bool AddSSLClientConnection( const wchar_t *localComponentName, 
			const wchar_t *remoteComponentName, const wchar_t *remoteServerLocation,			
			int pingTimeout = c_Undefined, bool connForGateway = false,
			ConnectionPriority priority = PriorityNormal,
			const ProxyInfo* proxyInfo = NULL ) = 0;

	/*!
	  \brief Устанавливает параметры соединений

	  \param maxConnsNum [in] максимально допустимое кол-во соединений
	  \param maxInactiveConnTime [in] максимально допустимое время неактивности соединения 
			( Ping на активности не влияет ), после которого данное соединения может быть закрыто
	*/
		virtual void SetConnectionsParams( int maxConnsNum = c_DefaultMaxNumberOfConns,			
			int maxInactiveConnTime = c_DefaultInactiveConnTimeout ) = 0;
		
	/*!
	  \brief Функция останавливает gateway соединения с использование SSL

	  \param localComponentName [in] Наименование клиента, от лица которого производиться 
				данное соединения. 
	  \param locsList [in]	Список узлов, через которые должено быть установлено соединение      
	  \param pingTimeout [in] Временной период вызова Ping метода от сервера к клиенту.
				
	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_CONN_ALREADY_EXISTS,
			TRERR_CONN_HANSHAKE
	  
	  \return Результат операции false - соединение с указанным адресом не установеленно
	*/
		virtual bool CreateSSLGatewayConnection( const wchar_t *localComponentName, 
			LocationsList &locsList, int connectTimeout = c_Undefined,
			ConnectionPriority priority = PriorityNormal,
			KLWAT::ClientContext *customAuthClientCtx = NULL ) = 0;
		
	/*!
	  \brief Функция получения удаленного адреса соединения

	  \param soap Стурктура описания соедиения

	  \return удаленный location. Формат http://xxx.xxx.xxx.xxx без порта
				В случае ошибку возвращает пустую строку
	*/
		virtual std::wstring GetRemoteLocation( struct soap *soap ) = 0;

	/*!
	  \brief Функция получения локального адреса соединения

	  \param soap Стурктура описания соедиения

	  \return удаленный location. Формат http://xxx.xxx.xxx.xxx без порта
				В случае ошибку возвращает пустую строку
	*/
		virtual std::wstring GetLocalLocation( struct soap *soap ) = 0;
		
	/*!
      \brief Функциия добавляет location для прослущивания unicast пакетов и проверкой SSL подписи.

	  При приходе unicast пакета будет вызванна callback функция транспорта со статусом
	  UnicastPacketArrived.

	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
    */
        virtual void AddSSLUnicastListenLocation( int unicastPort, KLPAR::BinaryValue *binaryPublicSSLKey ) = 0;

	/*!
	  \brief Функция посылает Unicast пакет и ждет создания соединение удаленной стороной

		Пакет подписывается SSL сертификатом

	*/
		virtual bool SendSSLUnicastAndWaitConnection( const wchar_t *remoteLocation,
			const wchar_t *waitRemoteComponentName, const wchar_t *waitLocalComponentName,
			int timeout, KLPAR::BinaryValue *binaryPrivateSSLKey, const wchar_t *keyPassword ) = 0;
		
	/*!
	  \brief Функция останавливает gateway соединения с использование SSL и custom authentication

	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_CONN_ALREADY_EXISTS,
			TRERR_CONN_HANSHAKE
	  
	  \return Результат операции false - соединение с указанным адресом не установеленно
	*/
		virtual bool CreateSSLGatewayConnection( const wchar_t *localComponentName, 
			LocationsList &locsList, KLPAR::BinaryValue *binaryPublicSSLKey,
			const KLPAR::Params * pCustomCredentials, 
			int connectTimeout = c_Undefined,
			ConnectionPriority priority = PriorityNormal,
			KLWAT::ClientContext *customAuthClientCtx = NULL ) = 0;
		
	/*!
	   \brief Функция опеределяет тип соединения		
	*/
		virtual bool IsLocalConnection( const wchar_t *localComponentName, 
			const wchar_t *remoteComponentName ) = 0;

	/*!
	   \brief Функция установки callback'a для контроля над соединениями
	*/
		virtual void SetConnectionControl( ConnectionControl *connControl ) = 0;

	/*!
	   \brief Функция устанавливает максимальное кол-во соединений для данного процесса
	*/
		virtual void SetMaxNumberOfConnections( int maxNumberOfConns ) = 0;

	/*!
	   \brief Функция возвращает максимальное кол-во соединений для данного процесса
	*/
		virtual int GetMaxNumberOfConnections() = 0;
		
	/*!
      \brief Функциия добавляет клиентское соединение.

      Функция пытается установить клиентское соединение с сервером.

	  \param connInfo [in] Параметры для создания соединения
				
	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_CONN_ALREADY_EXISTS,
			TRERR_CONN_HANSHAKE
	  
	  \return Результат операции false - соединение с указанным адресом не установеленно
    */
		virtual bool AddClientConnection2( const ConnectionInfo &connInfo ) = 0;

	/*!
	  \brief Функция посылает Unicast пакет 

	  \return false - ошибка отправки unicast пакета 

	  \param remoteLocation [in] Адрес удаленного компьютера
	  \param pParams [in] Параметры для отсылки
	  \param binaryPrivateSSLKey [in] Private ключ для подписи пакета
	  \param keyPassword [in] Пароль ключа
	  \param timeout [in] Время в течение которого будут отправляется Unicast пакета ( msec )
						Период отсылки сигналов 200 msec

	  \exception TRERR_INVALID_PARAMETER, TRERR_LOAD_CERT_ERR
	*/
		virtual bool SendUnicastPacket( const wchar_t *remoteLocation,
			KLPAR::Params *pParams,
			KLPAR::BinaryValue *binaryPrivateRSAKey,
			int timeout = 500 ) = 0;

	/*!
	  \brief Проверяет SSL подпись Unicast пакета 

	  \return false - ошибка проверки подписи

	  \param pTransportPacket [in] Пакет для проверки	  
	  \param binaryPublicSSLKey [in] Public ключ для проверки подписи пакета
	  
	  \exception TRERR_INVALID_PARAMETER
	*/
		virtual bool CheckPacketSSLSign( 
			const KLPAR::BinaryValue *pTransportPacket,
			const KLPAR::BinaryValue *binaryPublicRSAKey ) = 0;

	/*!
	  \brief Добавляет локальный сокетный listiner для прямого подключения ( без аутентификации и 
			транспортного hanshake'а )
			Используется для работы локальных библиотек для взаимодействия с системой администрирования
			( пример: FileTransferBridge - соединяется с сетевым агентом для получения файлов обнавлений )
	  
		\exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
    */
        virtual void AddSimpleSocketListenLocation( const wchar_t *localComponentName, 
				const wchar_t *location, int &usedPort ) = 0;

	/*!
	  \brief Создает соединение подключенное к Multicast группе с возможностью рассылки пакетов

		\exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
	*/
		virtual void CreateMulticastGroupSender( const wchar_t *multicastGroupLocation,
			KLPAR::BinaryValue *binaryPrivateSSLKey ) = 0;

	/*!
	  \brief Создает соединение подключенное к Multicast группе с возможностью получения пакетов
			Пакеты полученные по данному соединению будет переданы по средствам функции нотификаци
			KLTR::TransportNotifier с типом нотификации MulticastPacketArrived

		\exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
	*/
		virtual void JoinMulticastGroup( const wchar_t *multicastGroupLocation,
			KLPAR::BinaryValue *binaryPublicSSLKey ) = 0;

	/*!
	  \brief Производит рассылку mutlticast пакета

		\exception 
	*/
		virtual void SendMulticastPacket( const wchar_t *multicastGroupLocation, 
			KLPAR::BinaryValue *pMulticastPacket ) = 0;

	/*!
	  \brief Удаляет ранее созданное multicast соединение функциями CreateMulticastGroupSender и
			JoinMulticastGroup

		\exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
	*/
		virtual void DeleteMulticastConnection( const wchar_t *multicastGroupLocation ) = 0;
		
	/*!
      \brief Функциия обновляет существующий unicast listiner ( SSL обновляется сертификат )

	  При приходе unicast пакета будет вызванна callback функция транспорта со статусом
	  UnicastPacketArrived.

	  \exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
    */
        virtual void UpdateSSLUnicastListenLocation( int unicastPort, KLPAR::BinaryValue *binaryPublicSSLKey ) = 0;


	/*!
	  \brief Добавляет сокетный SSL listiner для прямого подключения ( без аутентификации и 
			транспортного hanshake'а )
	  
		\exception TRERR_INVALID_PARAMETER, TRERR_WRONG_ADDR, TRERR_LOCATION_ALREADY_EXISTS
    */
        virtual void AddSSLSimpleSocketListenLocation( const wchar_t *localComponentName, 
				const wchar_t *remoteNameForConnection,
				const wchar_t *location, int &usedPort,
				KLPAR::BinaryValue *binaryPrivateSSLKey,
				KLPAR::BinaryValue *binaryPublicSSLKey,
				const wchar_t *keyPassword,
				const int maxNumberOfConcurentThreads = 0 ) = 0;

	/*!
	  \brief Устанавливает пользовательский контекст аутентификации для текущего соединения.
			Вызов данной функции возможет только в рамках обработки gSOAP функции

		\exception TRERR_NOT_FOUND_CONNECTION, TRERR_INVALID_PARAMETER
	*/
		virtual void SetCustomAuthContextForConnection( const struct soap *soap,
			const KLWAT::CustomAccessToken& customAccessToken ) = 0;

	/*!
      \brief Закрывает существующее клиентское соединение.
        
	  \param localComponentName [in] Наименование клиента, от лица которого было созданно 
				данное соединения.
	  \param remoteComponentName [in]	Имя клиентского соединения	  	  	  
	  \param waitCleanEventQueue [in] false - не производить ожидания закрытия соединения
	  
	  \return Результат операции false - указаннное соединение не найдено в списке контролируемых соединений
    */
        virtual bool CloseClientConnection2( const wchar_t *localComponentName, 
			const wchar_t *remoteComponentName, bool waitCleanEventQueue ) = 0;

	/*!
	  \brief Возвращает указатель на TrafficManager
	*/
		virtual KLTR::TrafficManager *GetTrafficManager() = 0;

	/*!
      \brief Возвращает структуру soap для указанного клиентского соединения.

	  Возвращаемая данной функции структура SOAP можеть быть использованна клиентом для 
	  осуществления вызовов серверных функций. Возвращаемая структура не должна ни каким образом 
	  модифицироваться клиентом.
    */
        virtual void GetClientConnectionById( int connId, 
			bool lockConnection, struct soap **result ) = 0;

	/*!
      \brief Возвращает идентификатор клиентского соединения.	  
    */
		virtual int GetConnectionId( const wchar_t *localComponentName,
			const wchar_t *remoteComponentName ) = 0;

	/*!
      \brief Возвращает клиентский контекст соединения если NULL то соединения не найдено
    */
		virtual void GetConnectionClientContext( int connId, 
			KLSTD::CAutoPtr<KLWAT::ClientContext> &pClientContext ) = 0;

	/*!
      \brief Возвращает локальную маску подсети, которая используется для интерфейса
		при подключении по указанному location.
    */
		virtual void GetLocalSubnetMaskForConnection( const wchar_t *remoteLocation, 
			unsigned long &localSubnetMask ) = 0;
	};

	/*!
		\brief Конвертация из KLPRCI::ComponentId в транспортного имя компоненты.

			Формат транспортного представления компоненты имеет следующий формат:
					"ProductName;Version;ComponentName;InstanceId"

		\param trComponentName [out] - транспортное имя компоненты
		\param componentId [in] - идентификатор компоненты
	*/	
	KLCSTR_DECL void ConvertComponentIdToTransportName( std::wstring &trComponentName, 
		const KLPRCI::ComponentId &componentId );

	/*!
		\brief Конвертация из транспортного имя компоненты в KLPRCI::ComponentId.

		\param componentId [out] - идентификатор компоненты
		\param trComponentName [in] - транспортное имя компоненты		

		\return true - конвертация имена прошла успешна
	*/	
	KLCSTR_DECL bool ConvertTransportNameToComponentId( KLPRCI::ComponentId &componentId,
		const std::wstring &trComponentName );

} // namespace KLTRAP



#endif // KLTR_TRANSPORT_H

// Local Variables:
// mode: C++
// End:

