/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file foldersync.h
 * \author Дамир Шияфетдинов
 * \date 2006
 * \brief 
 * 
 */

#include <sstream>

#include <std/base/klbase.h>
#include <std/thr/sync.h>
#include <std/err/klerrors.h>
#include <std/par/par_gsoap.h>
#include <std/io/klio.h>
#include <std/time/kltime.h>
#include <std/conv/klconv.h>
#include <std/trc/trace.h>
#include <std/tmstg/timeoutstore.h>

#include <transport/tr/errors.h>

#include "boost/crc.hpp"
#include "openssl/md5.h"

#include <avp/text2bin.h>

#include <transport/tr/errors.h>
#include <transport/tr/transportproxy.h>

#include <kca/ft/filetransfer.h>
#include <kca/ft/errors.h>
#include <kca/ft/foldersync.h>
#include <kca/ft/filesender.h>
#include <kca/prts/prxsmacros.h>

#include "file.h"

#ifdef DEBUG
	#pragma comment(lib, "../openssl/lib/debug/ssleay32.lib")
	#pragma comment(lib, "../openssl/lib/debug/libeay32.lib")		
#else
	#pragma comment(lib, "../openssl/lib/release/ssleay32.lib")
	#pragma comment(lib, "../openssl/lib/release/libeay32.lib")	
#endif //DEBUG

#define KLCS_MODULENAME L"KLFT"

namespace KLFT {

	const wchar_t c_FolderSyncDirExt[]				= L"package";
	const wchar_t c_FolderSyncDirDestination[]		= L"destination";	
	const wchar_t c_FolderSyncDirDestinationWrite[]	= L"destination.mc";	
	const wchar_t c_FolderSyncDirDestinationNew[]	= L"dn";
	const wchar_t c_FolderSyncFilesInfo[]			= L"filesinfo";
	const wchar_t c_FolderSyncDirUploading[]		= L"uploading";

	const wchar_t c_serializeParamsServerSubfolders[]	= L"fs_syncsubfolders";
	const wchar_t c_serializeParamsFilesList[]			= L"fs_files";

		const wchar_t c_serializeParamsInfoFileName[]		= L"filename";
		const wchar_t c_serializeParamsInfoFileNameHash[]	= L"filenamehash";
		const wchar_t c_serializeParamsInfoFileCRC[]		= L"filecrc";
		const wchar_t c_serializeParamsInfoFileFlags[]		= L"fileflags";
	

	const int	  c_fileAccessorCacheSize				= 10;
	const int	  c_fileWriteAccessorCacheSize			= 3;
	const int	  c_fileAccessorLifeTime				= 30 * 1000;

	class FolderSyncImp : public KLSTD::KLBaseImpl<FolderSync> 
	{	
	public:

		class SyncFolderFileAccessor : public KLSTD::KLBaseImpl<KLTMSG::TimeoutObject>
		{
		public:
			KLTMSG_DECLARE_TYPEID(KLFT::FolderSyncImp::SyncFolderFileAccessor)
			SyncFolderFileAccessor( FolderSyncImp* pSynFolder, const std::wstring &fileName )
			{
				m_pSynFolder = pSynFolder;
				m_lastUseTime = 0;
				m_lastWorkPos = 0;
				m_fileName = fileName;
			}

			~SyncFolderFileAccessor()
			{
				m_pSynFolder->OnFileAccessorDestroy( m_fileName );
			}

			FolderSyncImp*					m_pSynFolder;
			time_t							m_lastUseTime;			
			std::wstring					m_fileName;
			KLSTD::CAutoPtr<KLSTD::File>	m_pFile;
			unsigned long					m_fullFileSize;
			unsigned long					m_lastWorkPos;
		};

		FolderSyncImp();

		~FolderSyncImp();

		//!\brief Инициалимзирует серверную часть сихронизации
		virtual void InitServer( const std::wstring &fileId,
			const std::wstring &workingFolder, 
			const std::wstring &dirFilePath );

		//!\brief Инициалимзирует клиентскую часть сихронизации
		virtual void InitClient( const std::wstring &fileId,
			const std::wstring &workingFolder );

		//!\brief Инициалимзирует клиентскую часть сихронизации работающим с неполным набором файлов.
		virtual void InitEndPoint( const std::wstring &fileId,
			const std::wstring &workingFolder );

		//!\brief Возвращает идентификатор файла папки синхронизации
		virtual std::wstring GetFileId();

		//!\brief Возвращает информация обо всех файлах в директории. 
		virtual void GetFullSyncInfo( FolderFilesInfo &folderFilesInfo );		

		//!\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
		bool CheckFullSyncContent( const FolderFilesInfo &clientFolderFilesInfo,			
			const KLFT::FolderSync::SubfoldersList &subfoldersList );

		//!\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
		virtual bool MakeFullSyncContent( const FolderFilesInfo &clientFolderFilesInfo,
			const std::wstring &archiveFileId,
			const KLFT::FolderSync::SubfoldersList &subfoldersList );

		//!\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
		virtual bool GetFullSyncInfo( const FileInfoMap &clientFolderFilesInfo,
			FolderFilesInfo &diffFolderFilesInfo,
			const FolderSync::SubfoldersList &subfoldersList,
			FolderSync::SubfoldersHashList &serverSubfoldersHashList );

		//!\brief Синхронизирует папку в системе FT с указанной в процессе инциализации папкой 
		virtual void SyncServerFolder();

		//!\brief Синхронизация папки клиента с использованнием полученным от сервера архивом изменений
		virtual void Sync( const std::wstring &contentFileId );

		//!\brief Устанавливает список файлов, которые требуют обновления
		virtual void SetNeedToUpdateFilesInfo( const FolderFilesInfo &badFolderFilesInfo,
			SubfoldersHashList &serverSubfoldersHashList );

		//!\brief Определяет необходимость закачки новой версии указанного файла с агента обновления
		virtual bool IsNeedToUpdateFile( const std::wstring& syncFolderFileName, bool &notExists );

		//!\brief Возвращает темпоральное имя файла в спец. папке для закачки с агента обновления
		virtual std::wstring GetUploadingTemporyFileName( const std::wstring& id );

		//!\brief Замешает или добавляет файл в папку синхронизации
		virtual void UpdateFile( const std::wstring &destFileName, 
			const std::wstring &syncFolderFileName );

		//!\brief Сохраняет папку FT по указанному пользователем пути
		virtual void SaveFolderAs( const std::wstring &copyFolderName, 
			bool overwrite = false );

		//!\brief Возвращает информацию о файле папки синхронизации
		virtual void GetFolderFileInfo( const std::wstring &folderFilePath, 
			KLFT::FileInfo &fi );

		//!\brief Возвращает часть файла папки синхронизации
		virtual unsigned long  GetFolderFileChunk( const std::wstring& folderFilePath,
			unsigned long startPosInFile, unsigned long neededSize,
			unsigned long &fullFileSize,
			void **ppBuff, unsigned long buffSize, bool &endOfFile );

		//!\brief Записывает часть файла полученного по multicast рассылке 
		//   Сохранение происходит в отдельную папку, и после закачи всего файла
		//		он копируется в основную папку
		virtual unsigned long  WriteFolderFileChunk( const std::wstring& folderFilePath,
			unsigned long startPosInFile, unsigned long fullFileSize,
			void *ppBuff, unsigned long buffSize, bool &endOfFile );

		//!\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
		bool IfExistsUnprocessedLastSyncChangedFiles();

		//!\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
		void GetUnprocessedLastSyncChangedFiles( FolderFilesInfo &folderFilesInfo, 
			unsigned long &syncTickCount );

		//!\brief Сбрасывает флаг обработки для указанного файла
		void ProcessedLastSyncChangedFiles( const std::string &fileNameHash,
			unsigned long syncTickCount );

		//!\brief Удаляет все рабочие файлы
		virtual void DeleteWorkingFiles();

		//!\brief Определяет наличие файла в syncfolder
		virtual bool IsFileExists( const std::wstring& syncFolderFileName, bool &bCanBeUploaded );

		//!\brief Возвращает список файлов которые были удалены 
		virtual void GetDeletedFilesList( const FolderFilesInfo &clientFolderFilesInfo,			
			const KLFT::FolderSync::SubfoldersList &subfoldersList, FileInfoList &deletedFolderFiles  );

		//!\brief Удаляет файлы которые былим удаленны в папке источнике 
		virtual void SyncDeletedFiles( const FileInfoList &deletedFolderFiles  );
		
		//!\brief Возвращает кол-во секунд, прощедщих с предыдущей синхронизацией.
		virtual long GetLastFullSyncTimeout();

		void OnFileAccessorDestroy( const std::wstring &storageKey );

	private:
		typedef std::list<std::wstring> FileAccessorIdList;		
		typedef std::vector<std::wstring> FilesSubfoldersList;
		typedef std::vector<std::string> FilesSubfoldersHashList;

		std::wstring	m_folderPath;
		std::wstring	m_destPath;
		std::wstring	m_workingFolder;
		std::wstring	m_fileId;
		bool			m_bEndPointMode;

		FileInfoMap		m_filesInfo;
		FileInfoMap		m_uploadingFilesInfo;

		FileInfoMap		m_needToUpdateFilesInfo;

		FilesSubfoldersList	m_subfoldersList;

		time_t			m_lastFullSync;
		unsigned long	m_lastSyncTickCount;

		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pFileAccessorCricSec;

		FileAccessorIdList	m_fileAccessorIdsList;
		FileAccessorIdList	m_writeFileAccessorsIdsList;

		SubfoldersHashList		m_serverSubfoldersHashList;
		SubfoldersHashList		m_serverSubfoldersHashListFromUA;

	protected:

		void ReadFilesInfoList();
		void SaveFilesInfoList();

		void ResetChangeFromLastSync();
		void SetDeletedFileFlag();

		bool CheckFileAccessorCache( const std::wstring &fileName,
			bool forWrite = false );
		void GetFileAccessor( const std::wstring &fileName,
			SyncFolderFileAccessor **ppFileAccessor, bool &isDir,
			bool forWrite = false );		
		void ClearFileAccessorCache();
		void EraseUnusedFileAccessor(bool forWrite);
		void EraseFileAccessor( const std::wstring &fileName );

		void CopyPath( const std::wstring &from, const std::wstring &to,
			const std::wstring &archivePath,
			bool bOverwrite,			
			bool bFillFilesInfo );

		std::string CountFileNameHash( const std::wstring& fileName );

		unsigned long CountFileCRC( const std::wstring &filePath );

		void FillServerSubfoldersHashList();
	};

	


	FolderSyncImp::FolderSyncImp()
	{
		m_lastFullSync = (-1);
		m_lastSyncTickCount = 0;
		m_bEndPointMode = false;

		KLSTD_CreateCriticalSection( &m_pFileAccessorCricSec );
	}

	FolderSyncImp::~FolderSyncImp()
	{
		ClearFileAccessorCache();
	}

	//!\brief Инициалимзирует серверную часть сихронизации
	void FolderSyncImp::InitServer( const std::wstring &fileId,
			const std::wstring &workingFolder, 
			const std::wstring &dirFilePath )
	{
		m_workingFolder = workingFolder;
		m_fileId = fileId;

		m_folderPath = m_workingFolder + L"/" + fileId + L"." + c_FolderSyncDirExt;

		KLERR_BEGIN
		{
			KLSTD_CreateDirectory( m_folderPath.c_str(), true );
		}
		KLERR_ENDT(3);

		m_destPath = dirFilePath;

		ReadFilesInfoList();
	}

	//!\brief Инициалимзирует клиентскую часть сихронизации
	void FolderSyncImp::InitClient( const std::wstring &fileId,
			const std::wstring &workingFolder )
	{
		std::wstring wstrDir, wstrName, wstrExt;		
		KLSTD_SplitPath( workingFolder, wstrDir, wstrName, wstrExt );

		if ( wstrExt.empty() || wstrDir.empty() ) wstrDir = workingFolder;

		m_workingFolder = wstrDir;
		m_fileId = fileId;

		m_folderPath = m_workingFolder + L"/" + fileId + L"." + c_FolderSyncDirExt;

		KLERR_BEGIN
		{
			KLSTD_CreateDirectory( m_folderPath.c_str(), true );
		}
		KLERR_ENDT(3);

		KLERR_BEGIN
		{
			KLSTD_DeletePath( (m_folderPath + L"/" + c_FolderSyncDirDestinationWrite).c_str() );
		}
		KLERR_ENDT(3);

		ReadFilesInfoList();
	}

	//!\brief Инициалимзирует клиентскую часть сихронизации работающим с неполным набором файлов.
	void FolderSyncImp::InitEndPoint( const std::wstring &fileId,
			const std::wstring &workingFolder )
	{
		std::wstring wstrDir, wstrName, wstrExt;		
		KLSTD_SplitPath( workingFolder, wstrDir, wstrName, wstrExt );

		if ( wstrExt.empty() || wstrDir.empty() ) wstrDir = workingFolder;

		m_workingFolder = wstrDir;
		m_fileId = fileId;

		m_folderPath = m_workingFolder + L"/" + fileId + L"." + c_FolderSyncDirExt;

		KLERR_BEGIN
		{
			KLSTD_CreateDirectory( m_folderPath.c_str(), true );
		}
		KLERR_ENDT(3);

		KLERR_BEGIN
		{
			KLSTD_DeletePath( (m_folderPath + L"/" + c_FolderSyncDirDestinationWrite).c_str() );
		}
		KLERR_ENDT(3);

		m_bEndPointMode = true;

		ReadFilesInfoList();
	}

	//!\brief Возвращает идентификатор файла папки синхронизации
	std::wstring FolderSyncImp::GetFileId()
	{
		return m_fileId;
	}

	//!\brief Возвращает информация обо всех файлах в директории. 
	void FolderSyncImp::GetFullSyncInfo( FolderFilesInfo &folderFilesInfo )
	{
		folderFilesInfo.FilesList.clear();

		FileInfoMap::iterator it = m_filesInfo.begin();
		for( ; it != m_filesInfo.end(); ++it )
		{
			folderFilesInfo.FilesList.push_back( it->second );
		}
		
		folderFilesInfo.bFileNameHashed = true;
	}

	//!\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
	bool FolderSyncImp::CheckFullSyncContent( const FolderFilesInfo &clientFolderFilesInfo,			
			const KLFT::FolderSync::SubfoldersList &subfoldersList )
	{
		std::vector<KLFT::FileSender::FileInfo> filesArray;

		FileInfoMap::iterator it = m_filesInfo.begin();		
		bool equalFound = false;

		for( ; it != m_filesInfo.end(); ++it )
		{
			int filePathIndx = it->second. FilePathIndex;

			FolderSync::SubfoldersList::const_iterator subsIt = subfoldersList.begin();			
			for ( ;subsIt != subfoldersList.end(); ++subsIt )
			{
				if ( m_subfoldersList[filePathIndx]==(*subsIt) ) break;
			}
// отклюяаем проверку необходимых папок клиенту на этапе тестирования
//			if ( subsIt==subfoldersList.end() && !subfoldersList.empty() ) continue;

			FileInfoList::const_iterator cit = clientFolderFilesInfo.FilesList.begin();
			bool foundAndEqual = false;
			for( ; cit != clientFolderFilesInfo.FilesList.end() && !foundAndEqual; ++cit )		
			{
				if ( it->second.FileNameHash==cit->FileNameHash && it->second.CRC==cit->CRC ) foundAndEqual = true;
			}

			if ( !foundAndEqual ) return true;
			else equalFound = true;
		}

		return !equalFound;
	}
	
	const wchar_t *c_skipDefaultSyncFoldersList[2] = {
		L"bases/as",
		L"AutoPatches"
	};

	//!\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
	bool FolderSyncImp::MakeFullSyncContent( const FolderFilesInfo &clientFolderFilesInfo,
			const std::wstring &archiveFileId,
			const KLFT::FolderSync::SubfoldersList &subfoldersList )
	{
		std::vector<KLFT::FileSender::FileInfo> filesArray;

		bool equalFound = false;

		if ( m_filesInfo.empty() ) 
		{
			// возможно синхронизация не проводилась ни разу ( запустим синхронизацию )
			SyncServerFolder();
		}

		FolderSync::SubfoldersList::const_iterator trSubsIt = subfoldersList.begin();			
		for ( ;trSubsIt!= subfoldersList.end(); ++trSubsIt )
		{
			KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::MakeFullSyncContent... subfolder - '%ls'\n",
				(*trSubsIt).c_str() );
		}

		FileInfoMap::iterator it = m_filesInfo.begin();

		for( ; it != m_filesInfo.end(); ++it )
		{
			int filePathIndx = it->second.FilePathIndex;

			FolderSync::SubfoldersList::const_iterator subsIt = subfoldersList.begin();			
			for ( ;subsIt != subfoldersList.end(); ++subsIt )
			{
				if ( m_subfoldersList[filePathIndx]==(*subsIt) ) break;
			}
// отклюяаем проверку необходимых папок клиенту на этапе тестирования
//			if ( subsIt==subfoldersList.end() && !subfoldersList.empty() ) continue;

			// проверяем папку в skip листе
			if ( subsIt==subfoldersList.end() )
			{
				std::wstring curFilePath = m_subfoldersList[filePathIndx];
				bool bFoundSkip = false;
				for( int sc = 0; sc < KLSTD_COUNTOF(c_skipDefaultSyncFoldersList); sc++ )
				{
					if ( curFilePath.find(c_skipDefaultSyncFoldersList[sc])==0)
					{
						bFoundSkip = true; break;						
					}
				}
				if ( bFoundSkip ) 
				{
					equalFound = true;
					continue;
				}
			}

			FileInfoList::const_iterator cit = clientFolderFilesInfo.FilesList.begin();
			bool foundAndEqual = false;
			for( ; cit != clientFolderFilesInfo.FilesList.end() && !foundAndEqual; ++cit )		
			{
				if ( it->second.FileNameHash==cit->FileNameHash && it->second.CRC==cit->CRC ) foundAndEqual = true;
			}

			if ( !foundAndEqual )
			{
				std::wstring fName = it->second.FullFileName;

				std::wstring wstrDir, wstrName, wstrExt;		
				KLSTD_SplitPath( fName, wstrDir, wstrName, wstrExt );

				if ( !wstrDir.empty() && (wstrDir[wstrDir.length()-1]=='/' || wstrDir[wstrDir.length()-1]=='\\' ) )
				{
					wstrDir.erase( wstrDir.length()-1, 1 );
				}

				std::wstring destFName = m_folderPath + L"/" + c_FolderSyncDirDestination + L"/" + fName;
				KLFT::FileSender::FileInfo fi( destFName, wstrDir );

				filesArray.push_back( fi );
			}	
			else equalFound = true;
		}

		if ( !equalFound )
		{
			KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::MakeFullSyncContent... create whole folder archive. archiveFileId - '%ls'.\n",
				archiveFileId.c_str() );

			// archive all folder
			CreateArchiveFile( m_folderPath + L"/" + c_FolderSyncDirDestination, 
				m_workingFolder + c_FileTransferTempDir + L"/", archiveFileId );

			KLSTD_Rename( (m_workingFolder + c_FileTransferTempDir + L"/" + archiveFileId).c_str(), 
					(m_workingFolder + L"/" + archiveFileId).c_str() );
		}
		else
		{
			if ( !filesArray.empty() )
			{
				KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::MakeFullSyncContent... create folder archive filesArray.size() - %d. archiveFileId - '%ls'.\n",
					filesArray.size(), archiveFileId.c_str() );

				// archive only changed files
				CreateArchiveFile2( &filesArray[0], filesArray.size(),	
					m_workingFolder + c_FileTransferTempDir + L"/" + archiveFileId, archiveFileId );

				KLSTD_Rename( (m_workingFolder + c_FileTransferTempDir + L"/" + archiveFileId).c_str(), 
					(m_workingFolder + L"/" + archiveFileId).c_str() );
			}
			else 
			{
				KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::MakeFullSyncContent...Folders are equal.\n" );
				return false;
			}
		}

		return true;
	}

	//!\brief Проверяет содержимое папки со списом файлов поступившим с клиента. 
	bool FolderSyncImp::GetFullSyncInfo( const FileInfoMap &clientFolderFilesInfo,
			FolderFilesInfo &diffFolderFilesInfo,
			const FolderSync::SubfoldersList &subfoldersList,
			FolderSync::SubfoldersHashList &serverSubfoldersHashList )
	{

#pragma message("----------------WARNING: Need to write------------------")

		diffFolderFilesInfo.FilesList.clear();
	
		FileInfoMap::iterator it = m_filesInfo.begin();
		for( ; it != m_filesInfo.end(); ++it )
		{
			int filePathIndx = it->second. FilePathIndex;

			FolderSync::SubfoldersList::const_iterator subsIt = subfoldersList.begin();			
			for ( ;subsIt != subfoldersList.end(); ++subsIt )
			{
				if ( m_subfoldersList[filePathIndx]==(*subsIt) ) break;
			}
// отклюяаем проверку необходимых папок клиенту на этапе тестирования
//			if ( subsIt==subfoldersList.end() && !subfoldersList.empty() ) continue;
			
			FileInfoMap::const_iterator cit = clientFolderFilesInfo.find( it->second.FileNameHash );
			if ( cit!=clientFolderFilesInfo.end() )
			{
				if ( it->second.CRC!=cit->second.CRC )
				{
					diffFolderFilesInfo.FilesList.push_back( it->second );
				}
			}
			else 
				diffFolderFilesInfo.FilesList.push_back( it->second );
		}
		
		diffFolderFilesInfo.bFileNameHashed = true;

		bool bNotEqual = !(diffFolderFilesInfo.FilesList.empty());
		if ( clientFolderFilesInfo.size()==0 ) bNotEqual = true;

		serverSubfoldersHashList = m_serverSubfoldersHashList;

		return bNotEqual;
	}

	//!\brief Синхронизация папки клиента с использованнием полученным от сервера архивом изменений
	void FolderSyncImp::Sync( const std::wstring &contentFileId )
	{
		std::wstring destDir = m_folderPath + L"/" + c_FolderSyncDirDestination;
		std::wstring destDirNew = m_folderPath + L"/" + c_FolderSyncDirDestinationNew + L"/" + contentFileId;

		KLERR_IGNORE( KLSTD_CreatePath( destDirNew.c_str() ) );

		ExtractArchiveFile( destDirNew, m_workingFolder,
			contentFileId, contentFileId );

		KLERR_BEGIN
		{
			KLSTD_CreateDirectory( destDir.c_str(), true );
		}
		KLERR_ENDT(3);

		ResetChangeFromLastSync();

		CopyPath( destDirNew, destDir, L"", true, true );

		FillServerSubfoldersHashList();

		time(&m_lastFullSync);
		m_lastSyncTickCount = KLSTD::GetSysTickCount();

		SaveFilesInfoList();

		KLERR_IGNORE( KLSTD_DeletePath( destDirNew.c_str() ) );
	}

	//!\brief Устанавливает список файлов, которые требуют обновления
	void FolderSyncImp::SetNeedToUpdateFilesInfo( const FolderFilesInfo &badFolderFilesInfo,
			SubfoldersHashList &serverSubfoldersHashList )
	{		
		m_needToUpdateFilesInfo.clear();

		FileInfoList::const_iterator it = badFolderFilesInfo.FilesList.begin();

		for( ;it != badFolderFilesInfo.FilesList.end(); ++it )
		{
			m_needToUpdateFilesInfo[it->FileNameHash] = *it;
			

//KLTRACE3( KLCS_MODULENAME, L"FolderSyncImp::SetNeedToUpdateFilesInfo FileNameHash - '%hs'.\n", it->FileNameHash.c_str() );		
		}

		m_serverSubfoldersHashListFromUA = serverSubfoldersHashList;

		KLTRACE3( KLCS_MODULENAME, L"FolderSyncImp::SetNeedToUpdateFilesInfo m_needToUpdateFilesInfo.size()- %d.\n",
					m_needToUpdateFilesInfo.size() );		
	}

	//!\brief Определяет необходимость закачки новой версии указанного файла с агента обновления
	bool FolderSyncImp::IsNeedToUpdateFile( const std::wstring& syncFolderFileName, bool &notExists )
	{
		notExists = false;
		std::string fnhash = CountFileNameHash( syncFolderFileName );
		FileInfoMap::iterator fit = m_needToUpdateFilesInfo.find(fnhash);

		if ( fit!=m_needToUpdateFilesInfo.end() ) 
		{
			return true;
		}
		else
		{
			fit = m_filesInfo.find(fnhash);
			if ( fit==m_filesInfo.end() )
			{				
				std::wstring wstrDir, wstrName, wstrExt;
				KLSTD_SplitPath( syncFolderFileName, wstrDir, wstrName, wstrExt );
				std::string strDirHash = CountFileNameHash( wstrDir );
				FilesSubfoldersHashList::iterator it = 
					std::find( m_serverSubfoldersHashListFromUA.begin(), m_serverSubfoldersHashListFromUA.end(), strDirHash );
				if ( it!=m_serverSubfoldersHashListFromUA.end() ) notExists = true; // файлов из указанной папки также нет и на UA
				return true;
			}
		}

		return false;
	}

	//!\brief Возвращает темпоральное имя файла в спец. папке для закачки с агента обновления
	std::wstring FolderSyncImp::GetUploadingTemporyFileName( const std::wstring& id )
	{
		std::wstring upDir = m_folderPath + L"/" + c_FolderSyncDirUploading;

		KLERR_IGNORE( KLSTD_CreateDirectory( upDir.c_str(), true ) );

		std::wstring fn = (upDir + L"/" + id + L".upload" );

		return fn;
	}

	//!\brief Замешает или добавляет файл в папку синхронизации
	void FolderSyncImp::UpdateFile( const std::wstring &destFileName, 
			const std::wstring &syncFolderFileName )
	{
		std::wstring destDir = m_folderPath + L"/" + c_FolderSyncDirDestination;

//		if ( !KLSTD_IfExists(destFileName.c_str()) )
//			KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, destFileName.c_str() );

		KLERR_IGNORE( KLSTD_CreateDirectory( destDir.c_str(), true ) );

		std::wstring fromFile = destFileName;
		std::wstring toFile = (destDir + L"/" + syncFolderFileName);

		KLTRACE3( KLCS_MODULENAME, L"FolderSyncImp::UpdateFile... Copy the file after reciving from multicast/or uploading in updater task. fileFrom - '%ls' fileTo - '%ls'.\n",
					fromFile.c_str(), toFile.c_str() );

		std::wstring wstrDir, wstrName, wstrExt;
		KLSTD_SplitPath( toFile, wstrDir, wstrName, wstrExt );
		if ( !KLSTD_IfExists( wstrDir.c_str() ) )  
		{
			KLSTD_CreatePath( wstrDir.c_str() );
		}		

		KLSTD_CopyFile( fromFile.c_str(), 
			toFile.c_str(), true );

		KLSTD_Unlink( fromFile.c_str(), false );

		unsigned long fcrc = CountFileCRC( toFile );
		
		FolderFileInfo fi( fcrc, syncFolderFileName, CountFileNameHash( syncFolderFileName ) );

		FilesSubfoldersList::iterator it = m_subfoldersList.begin();
		int subFolderCount = 0;
		for( ;it != m_subfoldersList.end(); ++it, subFolderCount++ )
		{
			if ( wstrDir==(*it) ) break;
		}

		if ( it == m_subfoldersList.end() ) m_subfoldersList.push_back( wstrDir );

		fi.FilePathIndex = subFolderCount;
		fi.MaskFileFlags = fi.MaskFileFlags & FileFlags_ChangeFromLastSync;

		m_filesInfo[fi.FileNameHash] = fi;

		FileInfoMap::iterator fit = m_needToUpdateFilesInfo.find(fi.FileNameHash);
		if ( fit!=m_needToUpdateFilesInfo.end() )
		{
			m_needToUpdateFilesInfo.erase( fit );
		}
		
		SaveFilesInfoList();

		KLTRACE3( KLCS_MODULENAME, L"FolderSyncImp::UpdateFile...OK. Copy the file after reciving from multicast/or uploading in updater task. fileFrom - '%ls' fileTo - '%ls'.\n",
					fromFile.c_str(), toFile.c_str() );
	}

	//!\brief Синхронизирует папку в системе FT с указанной в процессе инциализации папкой 
	void FolderSyncImp::SyncServerFolder()
	{
		m_filesInfo.clear();
		m_subfoldersList.clear();

		KLERR_BEGIN
		{
			KLSTD_CreateDirectory( (m_folderPath + L"/" + c_FolderSyncDirDestination).c_str(), true );
		}
		KLERR_ENDT(2)

		ResetChangeFromLastSync();
		SetDeletedFileFlag();
		
		CopyPath( m_destPath, m_folderPath + L"/" + c_FolderSyncDirDestination, L"", true, true );

		time(&m_lastFullSync);

		SaveFilesInfoList();
	}

	//!\brief Сохраняет папку FT по указанному пользователем пути
	void FolderSyncImp::SaveFolderAs( const std::wstring &copyFolderName, 
			bool overwrite )
	{
		CopyPath( m_folderPath + L"/" + c_FolderSyncDirDestination, copyFolderName, L"", overwrite, false );
	}

	//!\brief Возвращает информацию о файле папки синхронизации
	void FolderSyncImp::GetFolderFileInfo( const std::wstring &folderFilePath, 
			KLFT::FileInfo &fi )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		KLSTD::CAutoPtr<SyncFolderFileAccessor> pFileAccessor;
		bool isDir;
		GetFileAccessor( folderFilePath, &pFileAccessor, isDir );

		fi.m_fileName = folderFilePath;
		fi.m_isDirectory = isDir;
		if ( !isDir )
		{
			fi.m_fileSize = pFileAccessor->m_fullFileSize;			
		}
		else
		{
			fi.m_fileSize = 0;			
		}

		fi.m_createTime = 0;
		fi.m_readPermissionOk = true;		
	}

	//!\brief Возвращает часть файла папки синхронизации
	unsigned long FolderSyncImp::GetFolderFileChunk( const std::wstring& folderFilePath,
			unsigned long startPosInFile, unsigned long neededSize,
			unsigned long &fullFileSize,
			void **ppBuff, unsigned long buffSize, bool &endOfFile )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		KLSTD::CAutoPtr<SyncFolderFileAccessor> pFileAccessor;
		bool isDir;
		GetFileAccessor( folderFilePath, &pFileAccessor, isDir );

		if ( neededSize==0 )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		int bs = pFileAccessor->m_fullFileSize - startPosInFile;
		if ( bs<0 || ( bs==0  && pFileAccessor->m_fullFileSize!=0 ) )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		if ( (unsigned)bs>neededSize ) bs = neededSize;

		endOfFile = false;
		unsigned long read = 0;		
		if ( bs!=0 )
		{			
			if ( *ppBuff==NULL ) *ppBuff = new char[bs];
			pFileAccessor->m_pFile->Seek( startPosInFile, KLSTD::ST_SET );

			read = pFileAccessor->m_pFile->Read( *ppBuff, bs );		
		}
		else 
		{
			endOfFile = true;
		}

		fullFileSize = pFileAccessor->m_fullFileSize;

		if ( (startPosInFile + read) >= pFileAccessor->m_fullFileSize )		
		{
			endOfFile = true;
			EraseFileAccessor( folderFilePath );
		}				

		return read;
	}

	//!\brief Записывает часть файла полученного по multicast рассылке 
	unsigned long FolderSyncImp::WriteFolderFileChunk( const std::wstring& folderFilePath,
			unsigned long startPosInFile, unsigned long fullFileSize,
			void *ppBuff, unsigned long buffSize, bool &endOfFile )
	{
		endOfFile = false;

		if ( startPosInFile!=0 && !CheckFileAccessorCache( folderFilePath, true ) )
		{
			KLTRACE5( KLCS_MODULENAME, L"FolderSyncImp::WriteFolderFileChunk. Files can be saved only from start position. fileName - '%ls'.\n",
					folderFilePath.c_str() );
			return 0;
		}

		KLSTD::CAutoPtr<SyncFolderFileAccessor> pFileAccessor;
		bool isDir;
		GetFileAccessor( folderFilePath, &pFileAccessor, isDir, true );

		if ( startPosInFile!=pFileAccessor->m_lastWorkPos )
		{
			KLTRACE5( KLCS_MODULENAME, L"FolderSyncImp::WriteFolderFileChunk. Files can't be saved with gaps. fileName - '%ls' lastPos - %d savePos - %d.\n",
					folderFilePath.c_str(), pFileAccessor->m_lastWorkPos, startPosInFile );
			return 0;
		}

		unsigned long wrote = pFileAccessor->m_pFile->Write( ppBuff, buffSize );
		if ( wrote!=buffSize )
		{
			KLTRACE3( KLCS_MODULENAME, L"FolderSyncImp::WriteFolderFileChunk. Write operation error. fileName - '%ls' wrote - %d need - %d.\n",
					folderFilePath.c_str(), wrote, buffSize );
			return 0;
		}

		pFileAccessor->m_lastWorkPos = startPosInFile + buffSize;
		pFileAccessor->m_fullFileSize = fullFileSize;

		if ( (startPosInFile+buffSize)>=fullFileSize )
		{
			endOfFile = true;
			
			std::wstring accessorFName = pFileAccessor->m_fileName;			

			pFileAccessor = NULL;
			EraseFileAccessor( folderFilePath );

			std::wstring destFName = m_folderPath + L"/" + c_FolderSyncDirDestinationWrite + L"/" +
					accessorFName;

			UpdateFile( destFName, accessorFName );
		}

		return wrote;
	}


	//!\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
	bool FolderSyncImp::IfExistsUnprocessedLastSyncChangedFiles()
	{
//return true;

		FileInfoMap::iterator it = m_filesInfo.begin();		
		
		for(; it != m_filesInfo.end(); ++it )
		{
			if ( it->second.MaskFileFlags & FileFlags_ChangeFromLastSync ) return true;
		}

		return false;
	}

	//!\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
	void FolderSyncImp::GetUnprocessedLastSyncChangedFiles( FolderFilesInfo &folderFilesInfo, 
			unsigned long &syncTickCount )
	{		
		FileInfoMap::iterator it = m_filesInfo.begin();		
		
		for(; it != m_filesInfo.end(); ++it )
		{
			if ( it->second.MaskFileFlags & FileFlags_ChangeFromLastSync ) 
			{
				folderFilesInfo.FilesList.push_back( it->second );
			}
		}

		syncTickCount = m_lastSyncTickCount;
	}

	//!\brief Сбрасывает флаг обработки для указанного файла
	void FolderSyncImp::ProcessedLastSyncChangedFiles( const std::string &fileNameHash,
			unsigned long syncTickCount )
	{
		if ( syncTickCount != m_lastSyncTickCount ) return;

		FileInfoMap::iterator it = m_filesInfo.find(fileNameHash);
		if ( it!=m_filesInfo.end() ) 
		{
			it->second.MaskFileFlags  = it->second.MaskFileFlags & (~FileFlags_ChangeFromLastSync);
		}
	}
	
	
	//!\brief Удаляет все рабочие файлы
	void FolderSyncImp::DeleteWorkingFiles()
	{
		if ( !m_folderPath.empty() )
		{
			KLERR_BEGIN
			{			
				KLSTD_DeletePath( m_folderPath.c_str() );	
			}
			KLERR_ENDT(2)
		}
	}

	//!\brief Возвращает кол-во секунд, прощедщих с предыдущей синхронизацией.
	long FolderSyncImp::GetLastFullSyncTimeout()
	{
		if ( m_lastFullSync<0 ) return (-1);

		time_t curT; 
		time( &curT );

		return (curT - m_lastFullSync);
	}


	void FolderSyncImp::ReadFilesInfoList()
	{
		std::wstring infoFile = m_folderPath + L"/" + c_FolderSyncFilesInfo;

		KLERR_BEGIN
		{	
			m_filesInfo.clear();
			m_subfoldersList.clear();

			
			KLSTD::CAutoPtr<KLPAR::Params> pParSer;
			KLPAR_DeserializeFromFileName( infoFile, &pParSer );

			KLSTD::CAutoPtr<KLPAR::Params> pParInfos;

			if ( pParSer->DoesExist(c_serializeParamsFilesList) )
			{
				GET_PARAMS_VALUE( pParSer, c_serializeParamsFilesList, ParamsValue, PARAMS_T, pParInfos );

				KLSTD::CAutoPtr<KLPAR::Params> pParServerSubloders;
				GET_PARAMS_VALUE( pParSer, c_serializeParamsServerSubfolders, ParamsValue, PARAMS_T, pParServerSubloders );
				
				m_serverSubfoldersHashList.clear();
				KLPAR::ParamsNames parNames;
				pParServerSubloders->GetNames( parNames );
				KLPAR::ParamsNames::iterator it = parNames.begin();			
				for( ; it != parNames.end(); ++it )
				{
					m_serverSubfoldersHashList.push_back( std::string(KLSTD_W2A2( (*it).c_str() )) );
				}
			}
			else
			{
				pParInfos = pParSer;
			}

			KLPAR::ParamsNames parNames;
			pParInfos->GetNames( parNames );
			KLPAR::ParamsNames::iterator it = parNames.begin();			
			for( ; it != parNames.end(); ++it )
			{
				KLSTD::CAutoPtr<KLPAR::Params> parInfo;

				GET_PARAMS_VALUE( pParInfos, (*it).c_str(), ParamsValue, PARAMS_T, parInfo );

				FolderFileInfo fi;
				GET_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileName, StringValue, STRING_T, fi.FullFileName );
				std::wstring fnamehash;
				GET_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileNameHash, StringValue, STRING_T, fnamehash );
				fi.FileNameHash = KLSTD_W2A2( fnamehash.c_str() );
				GET_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileCRC, IntValue, INT_T, fi.CRC );
				long mask;
				GET_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileFlags, IntValue, INT_T, (long)mask );
				fi.MaskFileFlags = (short)mask;

				std::wstring wstrDir, wstrName, wstrExt;		
				KLSTD_SplitPath( fi.FullFileName, wstrDir, wstrName, wstrExt );
			
				FilesSubfoldersList::iterator it = m_subfoldersList.begin();
				int subFolderCount = 0;
				for( ;it != m_subfoldersList.end(); ++it, subFolderCount++ )
				{
					if ( wstrDir==(*it) ) break;
				}

				if ( it == m_subfoldersList.end() ) m_subfoldersList.push_back( wstrDir );
				fi.FilePathIndex = subFolderCount;

				m_filesInfo[fi.FileNameHash] = fi;				
			}
		}
		KLERR_ENDT(3)

		KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::ReadFilesInfoList m_filesInfo.size() - %d\n", m_filesInfo.size() );
	}

	void FolderSyncImp::SaveFilesInfoList()
	{
		std::wstring infoFile = m_folderPath + L"/" + c_FolderSyncFilesInfo;

		KLSTD::CAutoPtr<KLPAR::Params> pParInfos;
		KLPAR_CreateParams( &pParInfos );

		FileInfoMap::iterator it = m_filesInfo.begin();		
		int counter = 0;
		
		for(; it != m_filesInfo.end(); ++it, counter++ )
		{
			std::wostringstream ws;
			ws << counter;

			KLSTD::CAutoPtr<KLPAR::Params> parInfo;
			KLPAR_CreateParams( &parInfo );

			ADD_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileName, StringValue, it->second.FullFileName.c_str() );
			ADD_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileNameHash, StringValue, KLSTD_A2W2(it->second.FileNameHash.c_str()) );
			ADD_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileCRC, IntValue, it->second.CRC );			
			ADD_PARAMS_VALUE( parInfo, c_serializeParamsInfoFileFlags, IntValue, it->second.MaskFileFlags );
			
			ADD_PARAMS_VALUE( pParInfos, ws.str().c_str(), ParamsValue, parInfo );
		}

		KLSTD::CAutoPtr<KLPAR::Params> pParServerSubloders;
		KLPAR_CreateParams( &pParServerSubloders );

		FilesSubfoldersHashList::iterator sit = m_serverSubfoldersHashList.begin();
		for( ; sit != m_serverSubfoldersHashList.end(); ++sit )
		{
			ADD_PARAMS_VALUE( pParServerSubloders, KLSTD_A2W2((*sit).c_str()), IntValue, 0 );			
		}

		KLSTD::CAutoPtr<KLPAR::Params> pParSer;
		KLPAR_CreateParams( &pParSer );

		ADD_PARAMS_VALUE( pParSer, c_serializeParamsServerSubfolders, ParamsValue, pParServerSubloders );
		ADD_PARAMS_VALUE( pParSer, c_serializeParamsFilesList, ParamsValue, pParInfos );

		KLPAR_SerializeToFileName2( infoFile, KLPAR_SF_BINARY, pParSer );

		KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::SaveFilesInfoList m_filesInfo.size() - %d\n", m_filesInfo.size() );
	}

	void FolderSyncImp::ResetChangeFromLastSync()
	{
		FileInfoMap::iterator it = m_filesInfo.begin();		
		
		for(; it != m_filesInfo.end(); ++it )
		{
			it->second.MaskFileFlags  = it->second.MaskFileFlags & (~FileFlags_ChangeFromLastSync);			
		}
	}

	void FolderSyncImp::SetDeletedFileFlag()
	{
		FileInfoMap::iterator it = m_filesInfo.begin();		
		
		for(; it != m_filesInfo.end(); ++it )
		{
			it->second.MaskFileFlags  = it->second.MaskFileFlags & FileFlags_DeletedFile;
		}
	}

	void FolderSyncImp::OnFileAccessorDestroy( const std::wstring &storageKey )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		FileAccessorIdList::iterator it = std::find( m_fileAccessorIdsList.begin(),
			m_fileAccessorIdsList.end(), storageKey );

		if ( it!=m_fileAccessorIdsList.end() ) m_fileAccessorIdsList.erase(it);
		else
		{
			FileAccessorIdList::iterator it = std::find( m_writeFileAccessorsIdsList.begin(),
				m_writeFileAccessorsIdsList.end(), storageKey );

			if ( it!=m_writeFileAccessorsIdsList.end() ) m_writeFileAccessorsIdsList.erase(it);				
		}
	}

	bool FolderSyncImp::CheckFileAccessorCache( const std::wstring &fileName,
			bool forWrite )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		FileAccessorIdList *fileIdsList = &m_fileAccessorIdsList;
		if ( forWrite ) fileIdsList = &m_writeFileAccessorsIdsList;

		FileAccessorIdList::iterator it = std::find( fileIdsList->begin(),
			fileIdsList->end(), fileName );

		if ( it!=fileIdsList->end() ) return true;
		else return false;
	}

	void FolderSyncImp::GetFileAccessor( const std::wstring &fileName,
		SyncFolderFileAccessor **ppFileAccessor, bool &isDir, 
		bool forWrite )
	{
		isDir = false;

		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		FileAccessorIdList *fileIdsList = &m_fileAccessorIdsList;
		if ( forWrite ) fileIdsList = &m_writeFileAccessorsIdsList;

		FileAccessorIdList::iterator it = std::find( fileIdsList->begin(),
			fileIdsList->end(), fileName );

		KLSTD::CAutoPtr<KLTMSG::TimeoutStore2> pTimeoutStore;
		::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);

		if ( it!=fileIdsList->end() ) 
		{
			pTimeoutStore->GetAt( (*it), (KLTMSG::TimeoutObject**)(ppFileAccessor) );
			pTimeoutStore->Prolong( (*it) );
			time( &((*ppFileAccessor)->m_lastUseTime) );
		}
		else
		{
			EraseUnusedFileAccessor(forWrite);

			AVP_dword cf = KLSTD::CF_OPEN_EXISTING;
			AVP_dword af = KLSTD::AF_READ;
			std::wstring destFName = m_folderPath + L"/" + c_FolderSyncDirDestination + L"/" +
				fileName;

			if ( forWrite )
			{
				destFName = m_folderPath + L"/" + c_FolderSyncDirDestinationWrite + L"/" +
					fileName;
				std::wstring wstrDir, wstrName, wstrExt;
				KLSTD_SplitPath( destFName, wstrDir, wstrName, wstrExt );
				KLSTD_CreatePath( wstrDir.c_str() );
				cf = KLSTD::CF_CREATE_ALWAYS;
				af = KLSTD::AF_READ | KLSTD::AF_WRITE;
			}
			else
			{
				if ( !KLSTD_IfExists(destFName.c_str()) )
					KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, destFName.c_str() );

				if ( KLSTD_IsDirectory( destFName.c_str() ) )
				{
					isDir = true;
					*ppFileAccessor = NULL;
					return;
				}
			}

			*ppFileAccessor = new SyncFolderFileAccessor( this, fileName );
			time( &((*ppFileAccessor)->m_lastUseTime) );

			KLERR_TRY
			{
				KLSTD_CreateFile( destFName, KLSTD::SF_READ | KLSTD::SF_WRITE,
					cf, af, 0,
					&(*ppFileAccessor)->m_pFile );
			}
			KLERR_CATCH( pError )
			{
				KLERR_SAY_FAILURE( 2, pError );
				KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, destFName.c_str() );
			}
			KLERR_ENDTRY

			if ( !forWrite )
			{
				(*ppFileAccessor)->m_fullFileSize = (unsigned long)(*ppFileAccessor)->m_pFile->GetSize();
			}

			fileIdsList->push_back( fileName );
		
			pTimeoutStore->Insert( (*ppFileAccessor), c_fileAccessorLifeTime, fileName );
		}
	}

	void FolderSyncImp::ClearFileAccessorCache()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		FileAccessorIdList::iterator it = m_fileAccessorIdsList.begin();		

		while( it!=m_fileAccessorIdsList.end() ) 
		{
			KLSTD::CAutoPtr<KLTMSG::TimeoutStore2> pTimeoutStore;
			::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);
			if (pTimeoutStore) pTimeoutStore->Remove( (*it) );
			it = m_fileAccessorIdsList.begin();
		}

		it = m_writeFileAccessorsIdsList.begin();

		while( it!=m_writeFileAccessorsIdsList.end() ) 
		{	
			KLSTD::CAutoPtr<KLTMSG::TimeoutStore2> pTimeoutStore;
			::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);
			if (pTimeoutStore) pTimeoutStore->Remove( (*it) );

			it = m_writeFileAccessorsIdsList.begin();
		}
	}
	
	void FolderSyncImp::EraseUnusedFileAccessor(bool forWrite)
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		unsigned int upLimit = c_fileAccessorCacheSize;
		FileAccessorIdList *fileIdsList = &m_fileAccessorIdsList;
		if ( forWrite ) 
		{
			fileIdsList = &m_writeFileAccessorsIdsList;
			upLimit = c_fileWriteAccessorCacheSize;
		}		

		if ( fileIdsList->size()<=upLimit) return;

		KLSTD::CAutoPtr<KLTMSG::TimeoutStore2> pTimeoutStore;
		::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);
		if (!pTimeoutStore)  return;

		while( fileIdsList->size()>upLimit )
		{
			time_t minTime = 0;			
			FileAccessorIdList::iterator it = fileIdsList->begin();
			FileAccessorIdList::iterator minit = fileIdsList->end();

			for( ;it!=fileIdsList->end(); ++it ) 
			{
				KLSTD::CAutoPtr<SyncFolderFileAccessor> pFileAccessor;

				pTimeoutStore->GetAt( (*it), (KLTMSG::TimeoutObject**)(&pFileAccessor) );
				if ( minTime==0 || pFileAccessor->m_lastUseTime<minTime )
				{
					minTime = pFileAccessor->m_lastUseTime;
					minit = it;
				}
			}

			if ( minit!=fileIdsList->end() )
			{
				fileIdsList->erase(minit);
			}
		}
	}

	void FolderSyncImp::EraseFileAccessor( const std::wstring &fileName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pFileAccessorCricSec );

		FileAccessorIdList::iterator it = std::find( m_fileAccessorIdsList.begin(),
			m_fileAccessorIdsList.end(), fileName );

		if ( it!=m_fileAccessorIdsList.end() ) 
		{
			KLSTD::CAutoPtr<KLTMSG::TimeoutStore2> pTimeoutStore;
			::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);
			pTimeoutStore->Remove( (*it) );
		}
	}


	void FolderSyncImp::CopyPath( const std::wstring &from, const std::wstring &to, 
		const std::wstring &archivePath,
		bool bOverwrite,		
		bool bFillFilesInfo )
	{
#ifdef _WIN32

		WIN32_FIND_DATA FindFileData;
		std::wstring sFolder(from);		
		std::wstring archiveFullPath = to;
		if ( !archivePath.empty() )
		{
			archiveFullPath = to + L"/" + archivePath;
			KLSTD_CreateDirectory( archiveFullPath.c_str(), false );
		}
		HANDLE hFind = ::FindFirstFile( KLSTD_W2CT2( std::wstring(sFolder+ L"\\*.*").c_str() ), &FindFileData);
		if (hFind == (void *)0xFFFFFFFF) return;
		std::wstring curArchFPath;
		KLERR_TRY
		{
			while(true)
			{
				std::wstring curFileName( KLSTD_T2CW2( FindFileData.cFileName ) );

				curArchFPath = archivePath.empty()?curFileName:archivePath + L"/" + curFileName;
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if ( !(curFileName==L"." || curFileName==L".." ))
					{		
						CopyPath( from + L"/" + curFileName, 
							to, curArchFPath, bOverwrite, bFillFilesInfo );
					}
				}
				else 
				{	
					std::wstring fromFile = (from + L"/" + curFileName);
					std::wstring toFile = (archiveFullPath + L"/" + curFileName);
					KLSTD_CopyFile( fromFile.c_str(), 
						toFile.c_str(), bOverwrite );

					if ( bFillFilesInfo )
					{
						if ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
						{
							SetFileAttributes( KLSTD_W2CT2( toFile.c_str() ), 
								FindFileData.dwFileAttributes  & ~FILE_ATTRIBUTE_READONLY );
						}

						unsigned long fcrc = CountFileCRC( fromFile );
						
						FolderFileInfo fi( fcrc, curArchFPath, CountFileNameHash( curArchFPath ) );

						fi.MaskFileFlags  = fi.MaskFileFlags & FileFlags_ChangeFromLastSync;						

						std::wstring wstrDir, wstrName, wstrExt;		
						KLSTD_SplitPath( fi.FullFileName, wstrDir, wstrName, wstrExt );

						FilesSubfoldersList::iterator it = m_subfoldersList.begin();
						int subFolderCount = 0;
						for( ;it != m_subfoldersList.end(); ++it, subFolderCount++ )
						{
							if ( wstrDir==(*it) ) break;
						}

						if ( it == m_subfoldersList.end() ) m_subfoldersList.push_back( wstrDir );
						fi.FilePathIndex = subFolderCount;

						m_filesInfo[fi.FileNameHash] = fi;					
					}
				}

				if (!::FindNextFile(hFind, &FindFileData)) break;
			}
		}
		KLERR_CATCH(pError)
		{
			::FindClose(hFind);

			KLERR_SAY_FAILURE( 3, pError );
			
			KLERR_RETHROW();
		}
		KLERR_ENDTRY
			
		::FindClose(hFind);
#endif
	}

	std::string FolderSyncImp::CountFileNameHash( const std::wstring& fileName )
	{

#if defined(_WIN32) || defined (N_PLAT_NLM)
		MD5_CTX m_Md5Context;
		
		MD5_Init(&m_Md5Context);
		MD5_Update(&m_Md5Context, fileName.data(), 
			(unsigned long)(fileName.size()*sizeof(wchar_t)));
		
		unsigned char digest[MD5_DIGEST_LENGTH];
		MD5_Final(digest, &m_Md5Context);
				
		char text[B2T_LENGTH(sizeof(digest)) + 1];
		memset( text, 0, sizeof(text) );
		BinToText(digest, sizeof(digest), text, sizeof(text));
		return text;
#else
		KLSTD_USES_CONVERSION;
		return KLSTD_W2A(fileName.c_str());
#endif
	}

	unsigned long FolderSyncImp::CountFileCRC( const std::wstring &filePath )
	{
		const int c_CountCRCBuffSize = 32768;

		KLSTD::CAutoPtr<KLSTD::File>    pFile;
		KLSTD_CreateFile(
                    filePath,
                    KLSTD::SF_READ,
                    KLSTD::CF_OPEN_EXISTING,
                    KLSTD::AF_READ,
                    KLSTD::EF_SEQUENTIAL_SCAN,
                    &pFile);

		KLSTD::CPointer<boost::crc_32_type> crc32;
		crc32=new boost::crc_32_type;
		KLSTD_CHKMEM(crc32);

		KLSTD::CArrayPointer2<char> pBuff( new char[c_CountCRCBuffSize] );
		int nBuffSize = c_CountCRCBuffSize;

		bool eof = false;

		while( !eof )
		{
			int nReadBytes = pFile->Read( pBuff.get(), nBuffSize );
			if ( nReadBytes!=nBuffSize ) eof = true;
			if ( nReadBytes>0 )
			{
				crc32->process_bytes( pBuff.get(), nReadBytes );
			}
		}

		return crc32->checksum();
	}

	void FolderSyncImp::FillServerSubfoldersHashList()
	{
		m_serverSubfoldersHashList.clear();

		FilesSubfoldersList::iterator it = m_subfoldersList.begin();
		for( ; it != m_subfoldersList.end(); ++it )
		{
			m_serverSubfoldersHashList.push_back( CountFileNameHash( *it ) );
		}
	}

	//!\brief Определяет наличие файла в syncfolder
	bool FolderSyncImp::IsFileExists( const std::wstring& syncFolderFileName, bool &bCanBeUploaded )
	{
		bCanBeUploaded = false;
		std::string fnhash = CountFileNameHash( syncFolderFileName );
		FileInfoMap::iterator fit = m_filesInfo.find(fnhash);

		if ( fit!=m_filesInfo.end() ) 
		{
			return true;
		}
		else
		{
			bCanBeUploaded = true;
			return false;
		}
	}

	//!\brief Возвращает список файлов которые были удалены 
	void FolderSyncImp::GetDeletedFilesList( const FolderFilesInfo &clientFolderFilesInfo,			
			const KLFT::FolderSync::SubfoldersList &subfoldersList, FileInfoList &deletedFolderFiles  )
	{
		std::vector<KLFT::FileSender::FileInfo> filesArray;

		FileInfoMap::iterator it = m_filesInfo.begin();				
		deletedFolderFiles.clear();

		for( ; it != m_filesInfo.end(); ++it )
		{
			int filePathIndx = it->second.FilePathIndex;

			FolderSync::SubfoldersList::const_iterator subsIt = subfoldersList.begin();			
			for ( ;subsIt != subfoldersList.end(); ++subsIt )
			{
				if ( m_subfoldersList[filePathIndx]==(*subsIt) ) break;
			}
// отклюяаем проверку необходимых папок клиенту на этапе тестирования
//			if ( subsIt==subfoldersList.end() && !subfoldersList.empty() ) continue;

			FileInfoList::const_iterator cit = clientFolderFilesInfo.FilesList.begin();
			bool found = false;
			for( ; cit != clientFolderFilesInfo.FilesList.end() && !found; ++cit )		
			{
				if ( it->second.FileNameHash==cit->FileNameHash ) found = true;
			}

			if ( found && it->second.MaskFileFlags & FileFlags_DeletedFile ) 
			{
				deletedFolderFiles.push_back( it->second );
			}
		}

		KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::GetDeletedFilesList... count - %d\n",
				deletedFolderFiles.size() );
	}

	//!\brief Удаляет файлы которые былим удаленны в папке источнике 
	void FolderSyncImp::SyncDeletedFiles( const FileInfoList &deletedFolderFiles  )
	{
		bool bDeleteFileFlag = false;

		KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::SyncDeletedFiles... count - %d\n",
				deletedFolderFiles.size() );

		FileInfoList::const_iterator dit = deletedFolderFiles.begin();				
		
		for( ; dit != deletedFolderFiles.end(); ++dit )
		{
			FileInfoMap::iterator it = m_filesInfo.begin();
			for( ; it != m_filesInfo.end(); ++it )
			{
				if ( it->second.FileNameHash==dit->FileNameHash )
				{
					KLTRACE4( KLCS_MODULENAME, L"FolderSyncImp::SyncDeletedFiles Unlink - '%ls'\n",
						it->second.FullFileName.c_str() );

					KLSTD_Unlink( it->second.FullFileName.c_str(), false );
					bDeleteFileFlag = true;

					m_filesInfo.erase( it );
					break;
				}
					
			}
		}

		if ( bDeleteFileFlag )
		{
			SaveFilesInfoList();
		}
	}

	

} // end namespace KLFT


/*\brief Создает объект класса FolderSync */
KLCSKCA_DECL void KLFT_CreateFolderSync( KLFT::FolderSync **ppFolderSync )
{
	KLSTD_CHKOUTPTR(ppFolderSync);
	*ppFolderSync = new KLFT::FolderSyncImp();
	KLSTD_CHKMEM(*ppFolderSync);
}
