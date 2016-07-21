/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file File.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Интерфейс рабочего файла
 * 
 */

#ifndef KLFT_FILE_H
#define KLFT_FILE_H

#include <map>

#include <std/base/klbase.h>
#include <std/io/klio.h>

#include <kca/ft/updateagentinfo.h>
#include <kca/ft/filetransfer.h>
#include <kca/ft/filesender.h>
#include <kca/ft/foldersync.h>

namespace KLFT {

	const wchar_t c_InfoFileExt[] = L".info";
	
	const int c_defaultUploadingChunkDelay	= 60;	// 1 min

	class File : public KLSTD::KLBaseImpl<KLSTD::KLBase>
	{			
	public:

		struct UploadingClientDesc
		{
			std::wstring	clientName;
			time_t			lastAccessTime;
			bool			uploadingNow;
		};

		enum UpdateAgentFileStatus
		{
			UAFS_None = 0x0,
			UAFS_DownloadingFromServer = 0x1,
			UAFS_Downloaded = 0x2,
			UAFS_UseOnlyForUpdateAgentFlag = 0x4
		};

		File();

		virtual ~File();
		
		/*!
			\brief Создает описание файла для отсылки клиентам
		*/
		void CreateServerFile( const std::wstring &filePath, 
			const std::wstring &description, 			
			const std::wstring &workingFolder, 
			const KLTRAP::Transport::LocationExList &locsList,
			unsigned long broadcastDelay,
			std::wstring &fileId,
			int maxNumberUploadingClients );

		/*
			\brief Установка максимального кол-ва клиентов для закачки 
		 */
		void SetMaxNumberUploadingClients( int maxNumberUploadingClients );

		/*!
			\brief Создает описание файла для загрузки с сервера
		*/
		void CreateClientFile( const std::wstring &fileName, const std::wstring &fileId,
			const std::wstring &description, bool isDir, unsigned long fullFileSize,
			KLFT::FileType fileType,
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName, 
			unsigned long uploadDelay, unsigned long chunkTimeout,
			bool forUpdateAgentFlag,
			const UpdateAgentLocationList &updateAgentsLocs,
			const std::wstring &workingFolder );

		/*!
			\brief Обновляет описание файла для загрузки с сервера
		*/
		void UpdateClientFile( const std::wstring &fileName, const std::wstring &fileId,
			const std::wstring &description, bool isDir, unsigned long fullFileSize,
			KLFT::FileType fileType,
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName, 
			unsigned long uploadDelay, unsigned long chunkTimeout,
			bool forUpdateAgentFlag,
			const UpdateAgentLocationList &updateAgentsLocs,
			const std::wstring &workingFolder );
		
		/*!
			\brief Преобразует клиентский файл в серверный формат 
					( для дальнейшего распространения UpdateAgent'ом )
		*/
		void ConvertToServerFile( KLPAR::Params *pFullFileInfo );

		/*!
			\brief Создает описание файла для загрузки 
		*/
		void CreateServerUploadedClientFile( const std::wstring &fileId,
			bool isDir, unsigned long fullFileSize,
			const std::wstring &remoteClientName, 			
			const std::wstring &workingFolder );

		/*!
			\brief Создает описание файла из папки updater для отсылки с сервера
		*/
		void CreateUpdaterFolderFile( const std::wstring &filePath, 
			int maxUploadClients = c_defaultUploadingClients );

		/*!
			\brief Создает описание файла для отсылки на сервер
		*/
		void CreateSendToFile( KLFT::FileSender::FileInfo *filesArray, int filesArraySize, 
			std::wstring &fileId );

		/*!
			\brief Создает описание package для получения с сервера
		*/
		void CreatePackageFile( const std::wstring &fileName, const std::wstring &fileId,
			const std::wstring &description, 
			KLFT::FileType fileType,
			bool isDir,
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName, 
			unsigned long uploadDelay, unsigned long chunkTimeout,
			const std::wstring &workingFolder );

		/*!
			\brief Создает описание файла для загрузки с UA
		*/
		void CreateEndPointFile( const std::wstring &fileId,
			const std::wstring &description,
			const std::wstring &workingFolder );

		/*!
			\brief Обновляет пакетный файл ( выставляет признак необходимости синхронизации )
		*/
		void UpdatePackageFile( KLFT::FileType &newFileType );


		/*!
			\brief Создает описание файла из файла описания ( процедура востановления )
		*/
		void CreateFromInfoFile( const std::wstring &infoFilePath );

		/*!
			\brief Возвращает идентификатор файла
		*/
		std::wstring GetFileId();

		/*!
			\brief Возвращает идентификатор клиента, выданный сервером
		*/
		std::wstring GetClientName();

		/*!
			\brief Возвращает лоакльное имя соединение, которое надо использовать для закачки данного файла
		*/
		std::wstring GetLocalName();

		/*!
			\brief Получение информации о файле
		*/
		FileDescription GetFileDescription();

		/*!
			\brief Статус рабочего файла
		*/
		void SetStatus( FileDescription::FileStatus newStatus );

		/*!
			\brief Статус рабочего файла
		*/
		FileDescription::FileStatus GetStatus();

		/*!
			\brief Добавление новой "порции" файла

			\return false - если это был последний chunk
		*/
		bool AddChunk( void *buff, unsigned long buffSize, unsigned long CRC, 
			unsigned long startPosInFile, unsigned long fullFileSize,
			bool fromMulticast = false );

		/*!
			\brief Считывает очередную "порции" файла

			\return кол-во прочитанных байт
		*/
		int GetChunk( const std::wstring &remoteClientName,
			unsigned long startPosInFile,			
			void *buff, unsigned long buffSize, bool &endOfFile );

		/*!
			\brief Добавление клиента в список обработанных
		*/
		bool AddSentLocation( const std::wstring &remoteName );

		/*!
			\brief Добавление клиента в список обработанных
		*/
		bool AddUploadedLocation( const std::wstring &remoteName );

		/*!
			\brief Добавляет список клиентов для отправки
		*/
		void AddClientsForSend( const KLTRAP::Transport::LocationExList &locsList );

		/*!
			\brief Удаляет из спсика клиентов для отправки
		*/
		void DeleteClients( const std::list<std::wstring> &compsList );

		/*!
			\brief Обнавляет список клиентов для указанного файла
		*/
		void UpdateClientsList( const KLTRAP::Transport::LocationExList &locsList );

		/*!
			\brief Перезаписывает список клиентов для указанного файла
		*/
		void ResetClientsList( const KLTRAP::Transport::LocationExList &locsList );

		/*!
			\brief Проверяет список клиентов
		*/
		bool CheckUploadingClient( const std::wstring &remoteClientName, 
			bool &uploadingClientsExceeded );

		/*!
			\brief Копирует готовый файл по указанному пути
		*/
		void SaveFileAs( const std::wstring &filePath, bool overwrite );

		/*!
			\brief Удаляет всю информацию о данном файле
		*/
		void DeleteWorkingFiles();

		/*!
			\brief Функции блокировки доступа к файлу
		*/
		void Lock();
		void UnLock();

		/*!
			\brief Опеределяет необходимо ли проводить закачку
		*/
		bool IsNeedToUpload();

		/*!
			\brief Опеределяет необходимо ли проводить рассылку сигнала о выгрузке
		*/
		bool IsNeedToBroadcast( bool checkInBroadcastFlag = true );

		/*!
			\brief Сохраняет текущее время как время последней операции с файлом
		*/
		void InBroadcastFlag( bool inBroadcast = false );

		/*!
			\brief Возвращает кол-во произведенный Broadcast'ов для данного файла
		*/
		int GetBroadcastCount();

		/*!
			\brief Сбрасывает кол-во произведенный Broadcast'ов для данного файла
		*/
		void ResetBroadcastCount( int count = 0 );

		/*!
			\brief Увеличивает время задержки переда началом закачки файла
		*/
		void IncreaseStartUploadingTimeout( int addDelay );

		/*!
			\brief Изменяет timeout между операции получения порции файла
		*/
		void ChangeUploadDelayTimeout( bool increase = true, bool usingUAConnection = false );
		
		/*!
			\brief Возвращает общее количество ошибок при закачки файла.
		*/
		long GetUploadChunkFailedCount();

		/*!
			\brief Функции работы с broadcastId
		*/
		KLTRAP::Transport::BroadcastId GetBroadcastId();
		void SetBroadcastId( KLTRAP::Transport::BroadcastId broadcastId );

		/*!
			\brief Файл из папки Updater
		*/
		bool IsCustomModeFile();

		/*!
			\brief Файл закачки Update Agent для долнейшего распространения
		*/
		bool IsForUpdateAgentFile();

		/*!
			\brief Возвращает статус файла агента обновлений
		*/
		UpdateAgentFileStatus GetUpdateAgentFileStatus();

		/*!
			\brief Признак необходимости публикации событий при закачки данного файла на агент обновлений
		*/
		void ResetUseOnlyForUpdateAgentFlag();
		bool GetUseOnlyForUpdateAgentFlag();
		
		/*
			\brief Считывает текущую длинну файла
		 */
		void ReadFileSize();

		/*
			\brief Возвращает расчетное время задержки загрузки файла клиентов
		 */
		int GetCalculateDelayTimeout();

		/*
			\brief Возвращает список агентов обновлений
		 */
		UpdateAgentLocationList GetUpdateAgentList();

		/*
			\brief Признка частично сохраняемого файла ( по multicast рассылке )
		 */
		bool PartlySavedFile();
		void PartlySavedFile( bool newVal );

		/*
			\brief Возращает размер и позицию следоущего необходимого блока
		 */
		bool GetNextNeededChunk( unsigned long &pos, unsigned long &size, 
			unsigned long buffsize );

		/*!
			\brief Тип файла
		*/
		KLFT::FileType Type();

		/*!
			\brief Возвращает информацию об файлах папки синхронизации
		*/
		void GetFullSyncInfo( KLFT::FolderSync::FolderFilesInfo &folderFileInfo );

		/*!
			\brief Применяет на папку архив синхронизации с сервера ( полная синхронизация клиента )
		*/
		void ApplyPackageSyncArchive( const std::wstring& syncArchiveFileId );

		/*!
			\brief Возвращает указатель на интерфейс FolderSync
		*/
		void GetFolderSync( KLSTD::CAutoPtr<KLFT::FolderSync> &pFolderSync );
		
		/*!
			\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
		*/
		bool IfExistsUnprocessedLastSyncChangedFiles();

		/*!
			\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
		*/
		bool GetUnprocessedLastSyncChangedFiles( std::vector<std::wstring>& folderFiles );

		/*!
			\brief Сбрасывает флаг обработки для указанного файла
		*/
		void ProcessedLastSyncChangedFiles( const std::wstring& folderFilePath );

		/*!
			\brief Функции для работы со списков подпапок для синхронизации
		*/
		KLFT::FolderSync::SubfoldersList GetSubfoldersListForFolderSync();
		bool AddSubfoldersForFolderSync( const std::wstring &requestedFilePath );
		bool AppendSubfoldersForFolderSync( const KLFT::FolderSync::SubfoldersList &subfoldersList );

		//!\brief Возвращает процент процесса архивирования
		int GetArchivatePercent();

		//!\brief Сохраняет info файл
		void SaveInfoFile();
		

	private:
		
		enum CloseChunkType{ CCT_None, CCT_Left, CCT_Right, CCT_Equal };

		class RecievedChunk
		{
		public:

			RecievedChunk()
			{
				pos = size = 0;
			}

			RecievedChunk( unsigned long _pos, unsigned long _size )
			{
				Init( _pos, _size );
			}

			RecievedChunk( const RecievedChunk &rc )
			{
				pos = rc.pos;
				size = rc.size;
			}

			void Init( unsigned long _pos, unsigned long _size )
			{
				pos = _pos, size = _size;
			}

			unsigned long pos;
			unsigned long size;
		};

		typedef std::list<RecievedChunk> RecievedChunkList;

		typedef std::map<std::wstring,UploadingClientDesc> UploadingClientsList;
		
		
		// Данные класса
		KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pCricSec;

		FileDescription::FileStatus					m_currentStatus;

		std::wstring								m_fileName;
		std::wstring								m_copyFileName;
		std::wstring								m_fileId;
		std::wstring								m_infoFilePath;
		std::wstring								m_description;
		std::wstring								m_clientName;
		std::wstring								m_localName;
		bool										m_isDirectory;
		unsigned long								m_fullFileSize;
		unsigned long								m_multicastRecvSize;
		unsigned long								m_currentUploadPos;
		time_t										m_createTime;
		bool										m_customFileMode;
		KLFT::FileType								m_fileType;

		long										m_updateAgentFileStatus;

		UpdateAgentLocationList						m_updateAgentsLocs;


		KLTRAP::Transport::BroadcastId				m_broadcastId;

		bool										m_inBroadcastFlag; 
		int											m_BroadcastCount; 
		time_t										m_lastOperationTime;
		long										m_delay;
		long										m_uploadChunkTimeout;
		long										m_uploadChunkFailedCount;

		KLTRAP::Transport::LocationExList			m_sentLocsList;			//! Список клиентов, которым были отосланны сигналы на загрузку данного файла
		KLTRAP::Transport::LocationExList			m_notSendLocsList;		//! Список клиентов, для которых еще не посланны сигналы на загрузку
		
		KLTRAP::Transport::LocationExList			m_uploadLocsList;		//! Список клиентов, которые уже скачали данный файл

		KLSTD::CAutoPtr<KLSTD::File>				m_file;
		
		time_t										m_createFileTime;

		UploadingClientsList						m_uploadingClients;		//! Список клиентов которые производят загрузку данного файла
		int											m_maxNumberUploadingClients;
		int											m_calculatedUploadDelayTimeout;	//! расчетное время задержки закачки на клиенте

		RecievedChunkList							m_recievedChunks;
		bool										m_partlySavedFile;		

		KLSTD::CAutoPtr<KLFT::FolderSync>			m_pFolderSync;

		KLFT::FolderSync::FolderFilesInfo			m_lastSyncFilesInfo;
		unsigned long								m_lastSyncTickCount;

		KLFT::FolderSync::SubfoldersList			m_vecFoldersForSync;

		long										m_totalFilesInArchive, m_archivedFilesCount; //! Для контроля процесса архивации

	protected:
		
		//!\brief Инициализация переменных класса
		void Init( const std::wstring &fileId,
			const std::wstring &filePath, const std::wstring &description,
			const std::wstring &workingFolder, bool copyFile, bool createFile,
			bool updaterFile = false );

		void Init( const std::wstring &fileId, const std::wstring &copyFileName,
			KLFT::FileSender::FileInfo *filesArray, int filesArraySize );

		//!\brief Вспомогательные функции для сериализации 
		void AddLocationListExInParams( KLPAR::Params *parSerFile, 
			const wchar_t *fieldName, KLTRAP::Transport::LocationExList &locsList );
		void AddLocationExInParams( KLPAR::Params *parSerFile, 
			const wchar_t *fieldName, KLTRAP::Transport::LocationEx &loc );
		void AddUAListInParams( KLPAR::Params *parSerFile );		
		void AddChunksListInParams( KLPAR::Params *parSerFile );
		void AddSubfoldersListInParams( KLPAR::Params *parSerFile );

		//!\brief Востанавливает объект из info файла
		void RestoreFromInfoFile();

		//!\brief Вспомогательные функции для десериализации
		void GetLocationListExFromParams( KLPAR::Params *parSerFile, 
			const wchar_t *fieldName, KLTRAP::Transport::LocationExList &locsList );
		void GetLocationListExFromParams( KLPAR::Params *parSerFile, 
			const wchar_t *fieldName, KLTRAP::Transport::LocationEx &loc );
		void GetUAListFromParams( KLPAR::Params *parSerFile );
		void GetChunkListFromParams( KLPAR::Params *parSerFile );
		void GetSubfoldersListInParams( KLPAR::Params *parSerFile );

		//!\brief Проверяет что все клиенты скачали файл
		bool CheckDeliveredStatus();

		//!\brief Функция расчитывает время задержки перед закачкой файла на клиенте
		void CalculateDelayTimeout();

		//!\brief Возвращает описатель ранее сохраненного блока для конкатинации
		RecievedChunkList::iterator GetCloseChunksListItem( 
			unsigned long pos, unsigned long size, CloseChunkType &chunkType );

		//!\brief Склеивает рядом расположенный chunk'и
		void ClueRecievedChunksList();

	};

	class FileAutoUnlock
	{
	public:
		
		FileAutoUnlock() { m_file = NULL; }
		~FileAutoUnlock() 
		{			
			if ( m_file ) m_file->UnLock();
		}
		
		void Init( File *file )
		{
			m_file = file;
			if ( m_file ) m_file->Lock();
		}

		File *Get()
		{
			return m_file;
		}
		
		void Free()
		{
			if ( m_file ) m_file->UnLock();
			m_file = NULL;
		}
		
		
	private:
		File *m_file;
	};

	bool CreateArchiveFile( KLFT::FileSender::FileInfo *filesArray, int filesArraySize,
		const std::wstring &copyFileName, const std::wstring &fileId,
		long &totalFilesCount, long &archivedFilesCount );
	
	bool CreateArchiveFile2( KLFT::FileSender::FileInfo *filesArray, int filesArraySize,
		const std::wstring &copyFileName, const std::wstring &fileId );

	bool CreateArchiveFile( const std::wstring &folderPath, const std::wstring &workingFolder,
		const std::wstring &fileId );

	bool ExtractArchiveFile( const std::wstring &folderPath, const std::wstring &workingFolder,
		const std::wstring &archiveFile, const std::wstring &fileId );
	
} // end namespace KLFT

#endif // !defined(KLFT_FILE_H)
