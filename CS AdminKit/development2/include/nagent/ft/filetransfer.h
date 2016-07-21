/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file FileTransfer.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Главный интерфейс модуля передачи файлов
 * 
 */

#ifndef KLFT_FILETRANSFER_H
#define KLFT_FILETRANSFER_H

#include <std/base/klbase.h>
#include <kca/prci/componentid.h>

#include "filedescription.h"

namespace KLFT {

	const int c_UploadDelay			= 6000;		// 6 secs 
	const int c_NextChunkTimeout	= 1000;		// 1 secs	

	const int c_DefaultChunkSize	= 32768;		// 32 Kb

	const int c_defaultUploadingClients	= 10;
	
	/*!
		Имя компоненты FileTransfer
	*/
	const wchar_t c_FileTransferComponentName[] = L"FileTransfer";

	/*!
    * \brief Генерируемые события ( publisher данных событий является переданное в 
		процессе инициализации локальное имя )
    */
	
	const wchar_t EVENT_TYPE_START_UPLOAD[]			= L"KLFT_StartUpload";			// Старт операции загрузки на клиенте
	const wchar_t EVENT_TYPE_UPLOADED[]				= L"KLFT_Uploaded";				// Окончание операции загрузки на клиенте	
	const wchar_t EVENT_TYPE_UPLOADING_CANCELED[]	= L"KLFT_UploadingCanceled";	// Сервер прервал операцию загрузки данного файла

	
	/*!
    * \brief Тело событий
    */

	const wchar_t EVENT_BODY_FILE_ID[]			= L"KLFT_FileId";		// Идентификатор файла 

	/*!
    * \brief Главный интерфейс модуля File Transfer (FT).
    *  
    */

	class KLSTD_NOVTABLE FileTransfer : public KLSTD::KLBase 
	{	
	public:
		
		class TransferConnectionInvoker : public KLSTD::KLBase
		{
		public:
			virtual void CreateConnection( const std::wstring &localComponentName,
				const std::wstring &remoteComponentName ) = 0;
		};

	public:

		/*!
			\brief Инициализация подсистемы для работы в качестве сервера

			Данная функция считывает информацию из рабочей папки. Если существуют незаконченный серверный
			команды ( посылка сигналов клиентам ), то незавершенные операции ставяться на выполнение

			\param workingFolder			[in]  Путь к папке на диске, в которой FileTransfer будет хранить внутренние данные
			\param maxNumberOfUploadThreads [in]  Максимально возможное кол-во клиентов для считывания файла с сервера
			\param serverComponentId		[in]  Идентификатор сервера ( клиент будет коннектикся к данному процессу используя данный ComponentId )
		*/
		virtual void InitServer( const std::wstring &workingFolder,
			const std::wstring &serverConnName ) = 0;

		/*!
			\brief Инициализация подсистемы для работы в качестве клиента
			
			Данная функция считывает информацию из рабочей папки. 
			Производит докачку "недозакаченных" файлов
		*/
		virtual void InitClient( const std::wstring &workingFolder,			
			const std::wstring &serverRemoteConnName,
			const std::wstring &localConnName,
			TransferConnectionInvoker *connectionInvoker ) = 0;

		/*!
			\brief Установка папки для файлов Updater'а	
		*/
		virtual void SetUpdaterFolder( const std::wstring &updaterFolderName,
			int maxNumberUploadingClients = c_defaultUploadingClients ) = 0;

		/*!
			\brief Прекрашает работу подсистемы
		*/
		virtual void Shutdown() = 0;
		
		/*!
			\brief Производить рассылку сигнала на клиенты о необходимости закачки указанного файла

			Данная функция добавляет задачу ( выполняемое в рамках отдельного потока )
			которая выполняет след. последовательность действий:
				1. Копирует файл в рабочую дирректорию FileTrabsfer'а
				2. Выдает данному файлу уникальный идетификатор
				3. Создает соединения с каждым из клиентов и оповещает о необходимости закачки файла по переданному идентификатору

			\param filePath		[in]  Путь к файлу 
			\param locsList		[in]  Список клиентов, которым необходимо отослать данный файл
			\param fileId		[out] Уникальный идентификатор, сгенерированной подсистемы, связанный с переданным файлом
		*/
		virtual void SendFile( const std::wstring &filePath, 
			const KLTRAP::Transport::LocationExList &locsList,
			const std::wstring &fileDescription,
			std::wstring &fileId, 
			int maxNumberUploadingClients = c_defaultUploadingClients ) = 0;

		/*
			\brief Установка максимального кол-ва клиентов для закачки для указанного
					файла
			\param fileId		[in]  Идентификатор файла
		 */
		virtual void SetMaxNumberUploadingClients( const std::wstring &fileId, 
			int maxNumberUploadingClients ) = 0;

		/*!
			\brief Добавляет набор указанных клиентов в список рассылки
				для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param locsList		[in]  Список клиентов, которым необходимо отослать данный файл
		*/
		virtual void AddClientsToFile( const std::wstring &fileId, 
			const KLTRAP::Transport::LocationExList &locsList ) = 0;

		/*!
			\brief Удаляет набор указанных клиентов из список рассылки
				для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param compsList	[in]  Список клиентов
		*/
		virtual void DeleteClientsFromFile( const std::wstring &fileId, 
			const std::list<std::wstring> &compsList ) = 0;
		
		/*!
			\brief Обнавляет список клиентов для указанного файла
			
			\param fileId		[in]  Идентификатор файла
			\param locsList		[in]  Список клиентов, которым необходимо отослать данный файл
		*/
		virtual void UpdateClientsList( const std::wstring &fileId, 
			const KLTRAP::Transport::LocationExList &locsList ) = 0;

		/*!
			\brief Возвращает список идентификаторов рабочих файлов
		*/
		virtual void GetFilesList( std::vector<std::wstring> &filesIdsList ) = 0;

		/*!
			\brief Возвращает полную информацию об файле

			\param fleId		[in] Идентификатор файла
		*/
		virtual FileDescription GetFileInfo( const std::wstring &fileId ) = 0;

		/*!
			\brief Удаляет описание данного файла и прекращает все незаконченные операции

			\param fileId		[in] Идентификатор файла
		*/
		virtual void DeleteFile( const std::wstring &fileId ) = 0;

		/*!
			\brief Копирует закачанный файл по указанному пути			

			\param fileId		[in] Идентификатор файла
		*/
		virtual void SaveFileAs( const std::wstring &fileId,
			const std::wstring &copyFolderPath ) = 0;

		/*!
			\brief Возвращает локальное транспортное имя FileTransfer'а
		*/
		virtual void GetLocalConnectionName( std::wstring &localName ) = 0;
		
	};		

} // end namespace KLFT


//!\brief Функция для архивации папок
KLCSKCA_DECL bool KLFT_ArchiveFolder( const std::wstring &folderPathFrom, 
									 const std::wstring &archiveFileName );

//!\brief Функция для извлечения файлов из архива
KLCSKCA_DECL bool KLFT_ExtractArchiveFile( const std::wstring &folderPathTo, 
										  const std::wstring &archiveFileName );

/*
	\brief Функции инициализаци/деинициализации библиотеки передачи файлов
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSKCA_DECL, KLFT );

/*\brief Возвращает указатель на глобальный объект класса FileTransfer */
KLCSKCA_DECL KLFT::FileTransfer *KLFT_GetFileTransfer();


#endif // !defined(KLFT_FILETRANSFER_H)
