/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filetransferserver.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Интерфейс класса рассылки файлов
*
*/

#ifndef FTS_FILETRANSFERSERVER_H
#define FTS_FILETRANSFERSERVER_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <server/fts/serverfiledescription.h>
#include <kca/ft/filetransferconst.h>

#include <kca/ft/errors.h>

#include <server/db/dbconnection.h>
#include <server/srvinst/srvinstdata.h>

namespace KLFT
{
	struct UpdateAgentInfo;
}

namespace KLFTS {

	const wchar_t c_DefaultMulticastId[] = L"http://225.6.7.8:15001";

	class KLSTD_NOVTABLE FileTransferServer : public KLSTD::KLBase 
	{
	public:

		struct FileTransferServerOptions
		{
			FileTransferServerOptions()
			{
				bSendUnicast = true;
			}

			bool bSendUnicast;
		};

		struct GetFileChunkParams
		{
			GetFileChunkParams()
			{
				bUseUpdaterFolder = bForUpdateAgent = false;
			}
			
			GetFileChunkParams( const GetFileChunkParams &p )
			{
				bUseUpdaterFolder = p.bUseUpdaterFolder;
				bForUpdateAgent = p.bForUpdateAgent;
				wstrNagentHostId = p.wstrNagentHostId;
			}
			
			bool			bUseUpdaterFolder;
			bool			bForUpdateAgent;
			std::wstring	wstrNagentHostId;
		};	
	

		/*!
			\brief Инициализация подсистемы для работы в качестве сервера

			Данная функция считывает информацию из рабочей папки. Если существуют незаконченный серверный
			команды ( посылка сигналов клиентам ), то незавершенные операции ставяться на выполнение

			\param workingFolder			[in]  Путь к папке на диске, в которой FileTransfer будет хранить внутренние данные
			\param maxNumberOfUploadThreads [in]  Максимально возможное кол-во клиентов для считывания файла с сервера
			\param serverConnName			[in]  Имя соединения 
		*/
		virtual void InitServer( KLSRV::SrvData* pSrvData,
			const std::wstring &workingFolder,
			const std::wstring &serverConnName,
			FileTransferServerOptions &options = FileTransferServerOptions() ) = 0;

		/*!
			\brief Установка папки для файлов Updater'а	
		*/
		virtual void SetUpdaterFolder( const std::wstring &updaterFolderName,
			int maxNumberUploadingClients = KLFT::c_defaultUploadingClients ) = 0;

		/*!
			\brief Прекрашает работу подсистемы
		*/
		virtual void Shutdown() = 0;

		/*!
			\brief Регестрирует нового агента обновлений

			\param nAgentHostId		[in] Идентификатор хоста, который будет использоватся в качестве агента обновлений ( UA )
			\param nGroupId			[in] Идентификатор группы, хосты из который данный UA будет обслуживать 
						( если nGroupId < 0 то будет испльзованна группу который пердлежит UA )
			\param strAddress		[in] TCP/IP адресс UA ( если указнна пустая строка, то будет 
						использован адрес указнный в описания данного хоста + порт 13000 )
			\param strMulticastIP	[in] Multicast адрес рассылки ( если передана пустая строка,
						то адресс будет подобран автоматически )
			\param pCertificate		[in] Сертификат, который будет использовать для подписи 
						пакетов рассылки и при создании подключений 
						( если передан NULL то сертификат будет сгенерирован внутри функции )			
		*/
		virtual void RegisterUpdateAgent( KLFT::UpdateAgentInfo &updateAgentInfo, 
			bool checkExisting = true ) = 0;


		/*!
			\brief Удаляет ранее зарегестрированный агент обновлений

			\param nAgentHostId		[in] Идентификатор хоста, который будет использоватся в качестве агента обновлений ( UA )
		*/
		virtual void DeleteUpdateAgent( const std::wstring &nAgentHostId ) = 0;

		
		/*!
			\brief Производить рассылку сигнала на клиенты о необходимости закачки указанного файла

			Данная функция добавляет задачу ( выполняемое в рамках отдельного потока )
			которая выполняет след. последовательность действий:
				1. Копирует файл в рабочую дирректорию FileTrabsfer'а
				2. Выдает данному файлу уникальный идетификатор
				3. Создает соединения с каждым из клиентов и оповещает о необходимости закачки файла по переданному идентификатору

			\param filePath		[in]  Путь к файлу 
			\param hostsList	[in]  Список идентификаторов хостов, которым необходимо отослать данный файл
			\param fileId		[out] Уникальный идентификатор, сгенерированной подсистемы, связанный с переданным файлом
		*/
		virtual void SendFile( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &filePath, 
			const HostsIdsList &hostsList,
			const std::wstring &fileDescription,
			std::wstring &fileId, 
			KLPAR::Params *pParContext = NULL,
			int maxNumberUploadingClients = KLFT::c_defaultUploadingClients ) = 0;
		
		/*!
			\brief Производить рассылку сигнала на клиенты о необходимости закачки указанного пакета

			В рамках данной функции пакет представлен в виде файловой директории. При отсылки 
			информации клиенту производится синхронизация директорий на сервере и ранее полученной 
			директории на клиенте.
			
			\param fileDirPath		[in]  Путь к директории
			\param locsList		[in]  Список клиентов, которым необходимо отослать данный файл
			\param fileId		[out] Уникальный идентификатор, сгенерированной подсистемы, связанный с переданным файлом
		*/
		virtual void SendPackage( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &filePath, 
			const HostDescList &hostsList,			
			const std::wstring &fileDescription,
			std::wstring &fileId, 
			KLPAR::Params *pParContext = NULL,
			int maxNumberUploadingClients = KLFT::c_defaultUploadingClients ) = 0;
		
		/*!
			\brief Синхронизирует пользовательскую папку для последующей рассылки клиентам

			При отсылки информации клиенту производится синхронизация директорий на сервере и ранее полученной 
			директории на клиенте.
						
			\param fileId		[out] Уникальный идентификатор ранее созданного файла ( package )
		*/
		virtual void SyncPackage( KLDB::DbConnectionPtr pDbConn,			
			std::wstring &fileId ) = 0;
		
		/*!
			\brief Создает папку синхронизации для агентов обновлений
			
			После создания описания в БД производиться синхронизация содержимого указанной пользовательской
			 папки с внутренней копией. Рассылает агентам обновлений информацию о необходимости провести
			 полную синхронизацию папки.
			
			\param fileDirPath		[in]  Путь к директории
			\param groupIds		[in]  Список идентификаторов групп, для которых необходимо распространять данную папку
										Если массив пустой, папку будет распространяться на все агенты обновлений 
										данного сервера администрирования.
			\param fileId		[out] Уникальный идентификатор, сгенерированной подсистемы, связанный с переданным файлом
		*/
		virtual void CreateFolderSync( const std::wstring &filePath, 			
			const std::wstring &syncFolderName,
			std::wstring &fileId, 
			KLPAR::Params *pParContext = NULL,
			int maxNumberUploadingClients = KLFT::c_defaultUploadingClients ) = 0;

		/*!
			\brief Производит синхронизацию папки
			
			Данная функция производит поиск описателя папки синхронизации в БД и в случае
			удачного поиска осуществляет сихронизацию пользовательской папки с копией папки в системе FT.
			Производит рассылки сигнала агентами обновлений о необходимости получения пакетов синхронищации.
			
			\param syncFolderName	[in]  Имя папки синхронизации
		*/
		virtual void InitiateFolderSync( KLDB::DbConnectionPtr pDbConn, const std::wstring &syncFolderName ) = 0;

		/*
			\brief Установка максимального кол-ва клиентов для закачки для указанного
					файла
			\param fileId		[in]  Идентификатор файла
		 */
		virtual void SetMaxNumberUploadingClients( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			int maxNumberUploadingClients ) = 0;

		/*!
			\brief Добавляет набор указанных клиентов в список рассылки
				для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param locsList		[in]  Список клиентов, которым необходимо отослать данный файл
		*/
		virtual void AddClientsToFile( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			const HostsIdsList &hostsList ) = 0;

		/*!
			\brief Удаляет набор указанных клиентов из список рассылки
				для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param compsList	[in]  Список клиентов
		*/
		virtual void DeleteClientsFromFile( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			const HostsIdsList &hostsList ) = 0;

		/*!
			\brief Перезаписывает список клиентов для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param locsList		[in]  Список клиентов, которым необходимо отослать данный файл
		*/
		virtual void ReplaceClientsList( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			const HostsIdsList &hostsList ) = 0;		
		
		/*!
			\brief Обновляет список клиентов для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param hostsList	[in]  Список клиентов, которым необходимо отослать данный файл
			\param bDeleteAbsent [in]	true - удаляет из файла клиентов которых нет в переданном 
											списке. Все текущие операции для удаленных клиентов 
											прекращаются.
										false - только новые клиенты из списка будут добавлены в 
											список рассылки файла. Если данному клиенту файл уже 
											отсылался то поведение зависит от флага bResendExisting
			\param bResendExisting [in] true - клиентам из переданного списка будет отправлен данный 
											файл даже в случае если отправка данного файла клиенту уже
											производилась
										false - если данному клиенту файл уже отправлялся, то указанный 
											клиент игнорируется				
		*/
		virtual void UpdateClientsList( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			const HostsIdsList &hostsList,
			bool  bDeleteAbsent = false,
			bool  bResendExisting = false ) = 0;
		
		/*!
			\brief Добавляет набор указанных клиентов в список рассылки
				для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param hostsList	[in]  Список клиентов, которым необходимо отослать данный файл
		*/
		virtual void AddClientsToFile( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			const HostDescList &hostsList ) = 0;
		
		/*!
			\brief Обнавляет список клиентов для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param hostsList	[in]  Список клиентов, которым необходимо отослать данный файл
			\param bDeleteAbsent [in]	true - удаляет из файла клиентов которых нет в переданном 
											списке. Все текущие операции для удаленных клиентов 
											прекращаются.
										false - только новые клиенты из списка будут добавлены в 
											список рассылки файла. Если данному клиенту файл уже 
											отсылался то поведение зависит от флага bResendExisting
			\param bResendExisting [in] true - клиентам из переданного списка будет отправлен данный 
											файл даже в случае если отправка данного файла клиенту уже
											производилась
										false - если данному клиенту файл уже отправлялся, то указанный 
											клиент игнорируется				
		*/
		virtual void UpdateClientsList( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			const HostDescList &hostsList,
			bool  bDeleteAbsent = false,
			bool  bResendExisting = false ) = 0;


		/*!
			\brief Устанавливает пользовательский контекст для файла

			\param fileId		[in]  Идентификатор файла
			\param pParContext	[in]  Пользовательский контекст
		*/
		virtual void SetFileUserContext( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId, 
			KLPAR::Params *pParContext ) = 0;

		/*!
			\brief Возвращает список идентификаторов рабочих файлов
		*/
		virtual void GetFilesList( std::vector<std::wstring> &filesIdsList ) = 0;

		/*!
			\brief Возвращает полную информацию об файле
			
			\param fleId		[in] Идентификатор файла
		*/
		virtual ServerFileDescription GetFileInfo( const std::wstring &fileId ) = 0;

		/*!
			\brief Удаляет описание данного файла и прекращает все незаконченные операции

			\param fileId		[in] Идентификатор файла
		*/
		virtual void DeleteFile( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &fileId ) = 0;

		/*!
			\brief Копирует закачанный файл по указанному пути			

			\param fileId		[in] Идентификатор файла
		*/
		virtual void SaveFileAs( const std::wstring &fileId,
			const std::wstring &copyFolderPath ) = 0;
		
		/*!
			\brief Определяет есть ли для данного соединения файлы для отправки
		*/
		virtual bool HasNotSentFiles( int nHostId ) = 0;

		/*!
			\brief Функция создает соединение для отсылки удаленному хосту информацию
			 о необходиомости закачки файла
		*/
		virtual void InitiateRemoteConnectionAndSendSignal( const std::wstring &localConnName, 
				const std::wstring &remoteConnName, KLDB::DbConnectionPtr pCon,
				int nHostId, bool inPingHostMode = true ) = 0;
		
		/*!
			\brief Определяет начали ли указанных хост процесс загрузки файла

			\exception ERR_WRONG_FILE_ID
		*/
		virtual bool IsFileUploadingByHost( const std::wstring &fileId, const std::wstring &hostName ) = 0;

		/*!
			\brief Возвращает полное описание файла для конкретного агента обновления

			\exception ERR_WRONG_FILE_ID
		*/
		virtual void GetFullFileInfo( const std::wstring &fileId, 
			const KLTRAP::Transport::ConnectionFullName &connFullName,
			KLPAR::Params **ppFileInfo ) = 0;
		
		/*
			\brief Функция проверки статуса хоста на признак агента обновления. Отсылка информации на хост
		*/
		virtual void CheckHostUpdateAgentStatus( KLDB::DbConnectionPtr pCon,
			const KLTRAP::Transport::ConnectionFullName &hostConnName,
			const int nHostId ) = 0;

		/*
			\brief Возвращает очередную порцию файла
		*/
		virtual int GetNextFileChunk( 
			const std::wstring &remoteClientName,
			const std::wstring &fileName, const std::wstring &fileId,
			unsigned long pos, unsigned long neededSize,
			unsigned long &fullFileSize,
			GetFileChunkParams &params,
			char **ppBuff, int &buffSize ) = 0;

		/*
			\brief Возвращает информацию о файлах в папке Updater
		*/
		virtual void GetUpdaterFolderFileInfo( const std::wstring &fileName, 
			KLFT::FileInfo &r, const std::wstring &wstrNagentHostId ) = 0;

		/*!
			\brief Возвращает список агентов обновлений которые используються при распространении
			 указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param uaHostsList	[out]  Список хостов агентов обновлений
		*/
		virtual void GetFileUAList( const std::wstring &fileId, 
			HostsIdsList &uaHostsList ) = 0;

		/*!
			\brief Устанавливает параметры системы проверки обновлений

			\param nCheckTaskId	[in]  Идентификатор задачи проверки обновлений
			\param wstrReserveUpdatesFolder	[in]  Пусть к резервному хранилищу обновлений
		*/
		virtual void SetCheckUpdatesParams( long nCheckTaskId, 
			const std::wstring &wstrReserveUpdatesFolder ) = 0;

		

	};

} // end namespace FTS

/*
	\brief Функции инициализаци/деинициализации библиотеки передачи файлов
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSSRV_DECL, KLFTS );


/*\brief Возвращает указатель на глобальный объект класса FileTransfer */
KLCSSRV_DECL KLFTS::FileTransferServer *KLFTS_GetFileTransferServer();


#endif // FTS_FILETRANSFERSERVER_H