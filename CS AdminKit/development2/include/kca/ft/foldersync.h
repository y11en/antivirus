/*!
* (C) 2005 "Kaspersky Lab"
*
* \file foldersync.h
* \author Шияфетдинов Дамир
* \date 2006
* \brief Интерфейс объекта синхронизации содержимого дирректорий
*
*/

#ifndef KLFT_FOLDERSYNC_H
#define KLFT_FOLDERSYNC_H

#include <map>

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include "../kca/ft/filereceiver.h"

namespace KLFT {

	const int FileFlags_ChangeFromLastSync	= 0x1;
	const int FileFlags_DeletedFile			= 0x2;
	
	class FolderSync : public KLSTD::KLBase 
	{
	public:
  
		/*!
			\brief Струкура описания файла
		*/
		struct FolderFileInfo
		{
			FolderFileInfo() { CRC = 0; MaskFileFlags = 0; }
			FolderFileInfo( unsigned long _CRC, const std::string &fNameHash ) 
			{ 
				CRC = _CRC; 
				FileNameHash = fNameHash;
				MaskFileFlags = 0;
			}

			FolderFileInfo( unsigned long _CRC,
				const std::wstring &fName,
				const std::string &fNameHash ) 
			{ 
				CRC = _CRC; 
				FullFileName = fName;
				FileNameHash = fNameHash;
				MaskFileFlags = 0;
			}

			FolderFileInfo( const FolderFileInfo &fi )
			{
				CRC = fi.CRC;
				FullFileName = fi.FullFileName;
				FileNameHash = fi.FileNameHash;
				FilePathIndex = fi.FilePathIndex;
				MaskFileFlags = fi.MaskFileFlags;
			}

			unsigned long	CRC;
			std::wstring	FullFileName;
			std::string		FileNameHash;
			int				FilePathIndex;
			unsigned short	MaskFileFlags;
		};

		typedef std::vector<FolderFileInfo> FileInfoList;
		typedef std::map<std::string,FolderFileInfo> FileInfoMap;

		typedef std::vector<std::wstring> SubfoldersList;
		typedef std::vector<std::string> SubfoldersHashList;

		class FolderFilesInfo
		{
		public:
			FolderFilesInfo()
			{
				bFileNameHashed = false;
			}

			FolderFilesInfo( FolderFilesInfo &ffi )
			{
				FilesList = ffi.FilesList;
				bFileNameHashed = ffi.bFileNameHashed;

			}

		public:

			bool			bFileNameHashed;
			FileInfoList	FilesList;

		};
		/*!
			\brief Инициалимзирует серверную часть сихронизации

			\param fileId		[in] Идентификатор файла
			\param workingFolder[in] Рабочая директория FT
			\param dirFilePath	[in] Директория источника информации
		*/
		virtual void InitServer( const std::wstring &fileId,
			const std::wstring &workingFolder, 
			const std::wstring &dirFilePath ) = 0;

		/*!
			\brief Инициалимзирует клиентскую часть сихронизации

			\param fileId		[in] Идентификатор файла
			\param workingFolder[in] Рабочая директория FT
		*/
		virtual void InitClient( const std::wstring &fileId,
			const std::wstring &workingFolder ) = 0;

		/*!
			\brief Инициалимзирует клиентскую часть сихронизации работающим с неполным набором файлов.
			Поддерживает режим обновления файлов и контроль корретности файла по сравнению с файлом на 
			агенте обновления

			\param fileId		[in] Идентификатор файла
			\param workingFolder[in] Рабочая директория FT
		*/
		virtual void InitEndPoint( const std::wstring &fileId,
			const std::wstring &workingFolder ) = 0;

		/*!
			\brief Возвращает идентификатор файла папки синхронизации
		*/
		virtual std::wstring GetFileId() = 0;

		/*!
			\brief Возвращает информация обо всех файлах в директории. Данная 
			информация используется для синхронизации с серверной стороной

			\param folderFilesInfo	[out] Информация обо всех файлах директории		
		*/
		virtual void GetFullSyncInfo( FolderFilesInfo &folderFilesInfo ) = 0;	

		/*!
			\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
			Если возвращает false то директории равны

			\param clientFolderFilesInfo[in] Информация обо всех файлах директории клиента
		*/
		virtual bool CheckFullSyncContent( const FolderFilesInfo &clientFolderFilesInfo,			
			const KLFT::FolderSync::SubfoldersList &subfoldersList ) = 0;

		/*!
			\brief Проверяет содержимое папки со списом файлов поступившим с клиента. Формирует
			архив полной синхронизации для отправки на клиента.
			Если возвращает false то директории равны и файл не сформирован

			\param clientFolderFilesInfo[in] Информация обо всех файлах директории клиента
			\param archiveFileId		[out] Имя файла архива, в рабочей директории FT
		*/
		virtual bool MakeFullSyncContent( const FolderFilesInfo &clientFolderFilesInfo,
			const std::wstring &archiveFileId,
			const KLFT::FolderSync::SubfoldersList &subfoldersList ) = 0;

		/*!
			\brief Проверяет содержимое папки со списом файлов поступившим с клиента. Формирует
			список не равных файлов.
			Если возвращает false то директории равны

			\param clientFolderFilesInfo[in] Информация обо всех файлах директории клиента
			\param diffFolderFilesInfo	[out] Список различных или отсутствующих файлов
			\param serverSubfoldersHashList [out] Список папок с которыми UA проводил синхронизацию
		*/
		virtual bool GetFullSyncInfo( const FileInfoMap &clientFolderFilesInfo,
			FolderFilesInfo &diffFolderFilesInfo,
			const FolderSync::SubfoldersList &subfoldersList,
			SubfoldersHashList &serverSubfoldersHashList ) = 0;

		/*!
			\brief Синхронизирует папку в системе FT с указанной в процессе инциализации папкой 
			Данная операция возможна только на серверной стороне
		*/
		virtual void SyncServerFolder() = 0;

		/*!
			\brief Синхронизация папки клиента с использованнием полученным от сервера 
			архивом изменений

			\param contentFileId	[in] Идентификатор файла изменений
		*/
		virtual void Sync( const std::wstring &contentFileId ) = 0;

		/*!
			\brief Устанавливает список файлов, которые требуют обновления

			\param badFolderFilesInfo	[in] Список файлов
			\param serverSubfoldersHashList [in] Список папок с которыми UA проводил синхронизацию
		*/
		virtual void SetNeedToUpdateFilesInfo( const FolderFilesInfo &badFolderFilesInfo,
			SubfoldersHashList &serverSubfoldersHashList ) = 0;

		/*!
			\brief Определяет необходимость закачки новой версии указанного файла с агента обновления

			\param syncFolderFileName	[in] Путь к файлу в папке синхронизации
		*/
		virtual bool IsNeedToUpdateFile( const std::wstring& syncFolderFileName, bool &notExists ) = 0;

		/*!
			\brief Возвращает темпоральное имя файла в спец. папке для закачки с агента обновления

			\param id	[in] Идентификатор затравка
		*/
		virtual std::wstring GetUploadingTemporyFileName( const std::wstring& id ) = 0;


		/*!
			\brief Замешает или добавляет файл в папку синхронизации

			\param destFileName			[in] Путь к исходному файлу 
			\param syncFolderFileName	[in] Имя файла в папке синхронизации
		*/
		virtual void UpdateFile( const std::wstring &destFileName, 
			const std::wstring &syncFolderFileName ) = 0;

				
		/*!
			\brief Возвращает кол-во секунд, прощедщих с предыдущей синхронизацией.
				(-1) синхронизация не проводилась
		*/
		virtual long GetLastFullSyncTimeout() = 0;

		/*!
			\brief Сохраняет папку FT по указанному пользователем пути

			\param copyFolderName	[in] Путь для сохранения
			\param overwite			[in] Признак перезаписи, в случае если файл уже существует
		*/
		virtual void SaveFolderAs( const std::wstring &copyFolderName, 
			bool overwrite = false ) = 0;

		/*!
			\brief Возвращает информацию о файле папки синхронизации

			\param folderFilePath	[in] Путь к файлу в рамках директории синхронизации
			\param fi				[out] Информация о файле
		*/
		virtual void GetFolderFileInfo( const std::wstring &folderFilePath, 
			KLFT::FileInfo &fi ) = 0;

		/*!
			\brief Возвращает часть файла папки синхронизации

			\param folderFilePath	[in] Путь к файлу в рамках директории синхронизации
			\param ...
		*/
		virtual unsigned long GetFolderFileChunk( const std::wstring& folderFilePath,
			unsigned long startPosInFile, unsigned long neededSize,
			unsigned long &fullFileSize,
			void **ppBuff, unsigned long buffSize, bool &endOfFile ) = 0;

		/*!
			\brief Записывает часть файла полученного по multicast рассылке 
			   Сохранение происходит в отдельную папку, и после закачи всего файла
				он копируется в основную папку
		*/
		virtual unsigned long  WriteFolderFileChunk( const std::wstring& folderFilePath,
			unsigned long startPosInFile, unsigned long fullFileSize,
			void *ppBuff, unsigned long buffSize, bool &endOfFile ) = 0;
		
		/*!
			\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
		*/
		virtual bool IfExistsUnprocessedLastSyncChangedFiles() = 0;

		/*!
			\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
		*/
		virtual void GetUnprocessedLastSyncChangedFiles( FolderFilesInfo &folderFilesInfo, 
			unsigned long &syncTickCount ) = 0;

		/*!
			\brief Сбрасывает флаг обработки для указанного файла
		*/
		virtual void ProcessedLastSyncChangedFiles( const std::string &fileNameHash,
			unsigned long syncTickCount ) = 0;

		/*!
			\brief Удаляет все рабочие файлы
		*/
		virtual void DeleteWorkingFiles() = 0;

		/*!
			\brief Определяет наличие файла в syncfolder
		*/
		virtual bool IsFileExists( const std::wstring& syncFolderFileName, bool &bCanBeUploaded ) = 0;

		/*!
			\brief Возвращает список файлов которые были удалены 

			\param clientFolderFilesInfo[in] Информация обо всех файлах директории клиента
		*/
		virtual void GetDeletedFilesList( const FolderFilesInfo &clientFolderFilesInfo,			
			const KLFT::FolderSync::SubfoldersList &subfoldersList, FileInfoList &deletedFolderFiles  ) = 0;

		/*!
			\brief Удаляет файлы которые былим удаленны в папке источнике 
		*/
		virtual void SyncDeletedFiles( const FileInfoList &deletedFolderFiles  ) = 0;



	};

} // end namespace KLFT

KLCSKCA_DECL void KLFT_CreateFolderSync( KLFT::FolderSync **ppFolderSync );

#endif // KLFT_FOLDERSYNC_H
