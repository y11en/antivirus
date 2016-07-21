/*!
* (C) 2004 "Kaspersky Lab"
*
* \file File.cpp
* \author Дамир Шияфетдинов
* \date 2004 
* 
*/

#include <std/base/klbase.h>
#include <std/conv/klconv.h>
#include <std/trc/trace.h>
#include <std/par/params.h>
#include <std/err/klerrors.h>
#include <std/err/error.h>

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#include "cabsdk/mscompress.hpp"
#else
#include "libmspack/mspack/mspack.h"
#endif 

#include "file.h"
#include <kca/ft/errors.h>

#include <sstream>

namespace KLFT {

	#define KLCS_MODULENAME L"KLFT"

	#define ADD_PARAMS_VALUE(params, name, type, Var) { \
		KLSTD::CAutoPtr<KLPAR::type> pVal; \
		KLPAR::CreateValue(Var, &pVal); \
		(params)->AddValue(name, pVal); }

	#define GET_PARAMS_VALUE(params, name, type, typeType, Var) { \
		KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, true); \
		KLPAR_CHKTYPE(pVal, typeType, name); \
		Var = pVal->GetValue(); }

	#define GET_PARAMS_VALUE_NO_THROW(params, name, type, typeType, Var) { \
		KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, false); \
		if (pVal) { \
			KLPAR_CHKTYPE(pVal, typeType, name); \
			Var = pVal->GetValue(); \
		}}

	const wchar_t c_serializeParamsStatus[]			= L"ftp_Status";
	const wchar_t c_serializeParamsFileName[]		= L"ftp_FileName";
	const wchar_t c_serializeParamsFileId[]			= L"ftp_FileId";
	const wchar_t c_serializeParamsDescription[]	= L"ftp_Description";
	const wchar_t c_serializeParamsFullFileSize[]	= L"ftp_FullSize";
	const wchar_t c_serializeParamsMulticastRecvSize[]	= L"ftp_MulticastRecvSize";
	const wchar_t c_serializeParamsUploadPos[]		= L"ftp_UploadPos";
	const wchar_t c_serializeParamsCreateTime[]		= L"ftp_CreateTime";
	const wchar_t c_serializeParamsFileType[]		= L"ftp_FileType";
	const wchar_t c_serializeParamsIsDir[]			= L"ftp_IsDirectory";
	const wchar_t c_serializeParamsMaxClients[]		= L"ftp_MaxClients";
	const wchar_t c_serializeParamsClientName[]		= L"ftp_ClientName";	
	const wchar_t c_serializeParamsLocalName[]		= L"ftp_LocalName";	
	const wchar_t c_serializeParamsUpdateAgentsList[] = L"ftp_UAList";
	const wchar_t c_serializeParamsUpdateAgentStatus[] = L"ftp_UAFileStatus";

	const wchar_t c_serializeParamsNotSendLocsList[]= L"ftp_NotSendLocsList";
	const wchar_t c_serializeParamsSentLocsList[]	= L"ftp_SentLocsList";
	const wchar_t c_serializeParamsUploadLocsList[]	= L"ftp_UploadLocsList";

	const wchar_t c_serializeParamsLocExRemoteName[]= L"ftp_RemoteName";
	const wchar_t c_serializeParamsLocExLocalComponentName[] = L"ftp_LocalComponentName";
	const wchar_t c_serializeParamsLocExLocation[]	= L"ftp_Location";	
	const wchar_t c_serializeParamsLocExPorts[]		= L"ftp_Ports";
	const wchar_t c_serializeParamsLocExUnicastFlag[]	= L"ftp_UnicastFlag";
	const wchar_t c_serializeParamsLocExUnicastSSLFlag[]	= L"ftp_UnicastSSLFlag";

	const wchar_t c_serializeParamsUAConnName[] = L"ftp_UAConnName";
	const wchar_t c_serializeParamsUALocation[] = L"ftp_UALocation";
	const wchar_t c_serializeParamsUASSLLocation[] = L"ftp_UASSLLocation";
	const wchar_t c_serializeParamsUAMulticastAddr[] = L"ftp_UAMulticastAddr";

	const wchar_t c_serializeParamsChunkList[] = L"ftp_ChunksList";
	const wchar_t c_serializeParamsChunkPos[] = L"ftp_ChunkPos";
	const wchar_t c_serializeParamsChunkSize[] = L"ftp_ChunkSize";

	const wchar_t c_serializeParamsSyncFolderSubfoldersList[] = L"ftp_SyncSubfolders";

	

	void TraceLocsList( const KLTRAP::Transport::LocationExList &locsList )
	{
		KLTRACE3( KLCS_MODULENAME, L"Locations list:\n" );
		KLTRAP::Transport::LocationExList::const_iterator it = locsList.begin();
		int counter = 1;
		for( ; it!=locsList.end(); it++, counter++ )
		{
			std::wstring outStr;

			outStr += L"'"; outStr += it->remoteComponentName;outStr += L"'";

			outStr += L"-'";outStr += it->localComponentName;outStr += L"'";

			outStr += L"-'"; outStr += it->location; outStr += L"'";

			if ( it->ports.empty() ) outStr += L"-'ports list empty'";
			else outStr += L"-'ports list isn't empty'";

			if ( it->unicastConnectFlag ) outStr += L"-'unicast true'";
			else outStr += L"-'unicast false'";

			if ( it->useSSLUnicast ) outStr += L"-'ssl true'";
			else outStr += L"-'ssl false'";

			KLTRACE3( KLCS_MODULENAME, L"\tLocation (%d): %ls\n", 
				counter, outStr.c_str() );
		}
	}

#define TraceReceivedChunksList( list )\
	{\
		KLTRACE3( KLCS_MODULENAME, L"Recieved chunk list:\n" );\
		RecievedChunkList::iterator it = list.begin();\
		wchar_t tmpBuff[1024];\
		std::wstring outStr;\
		swprintf( tmpBuff, L"size - %d\n", list.size() );\
		outStr+=tmpBuff;\
		for( ; it!=list.end(); it++ )\
		{\
			swprintf( tmpBuff, L"pos - %8d nextpos - %8d\n", it->pos, (it->pos + it->size) );\
			outStr+=tmpBuff;\
		}\
		outStr+=L"\n\n";\
		KLTRACE3( KLCS_MODULENAME, outStr.c_str() );\
	}

#ifdef _WIN32
	
	int CountFileInPath( const std::wstring &filePathOrFileName )
	{
		if ( !KLSTD_IsDirectory( filePathOrFileName.c_str() ) )
		{
			return 1;
		}

		int countFile = 1; // для архивации архива папки

		std::vector<std::wstring> subDirsVecNames, filesVecNames;

		std::wstring mask = filePathOrFileName + KLSTD::c_szwPathDelimiters[0] + L"*.*";

		KLSTD_GetSubdirsByMask( mask, subDirsVecNames );
		KLSTD_GetFilesByMask( mask, filesVecNames );

		countFile += filesVecNames.size();

		std::vector<std::wstring>::iterator it = subDirsVecNames.begin();
		for( ; it != subDirsVecNames.end(); ++it )
		{
			std::wstring subDirPath = filePathOrFileName + KLSTD::c_szwPathDelimiters[0] + *it;
			countFile += CountFileInPath( subDirPath );			
		}

		return countFile;
	}


	bool AddFolderInArchive( mszip::cab_compressor *zip, const std::wstring &folderFullPath,
		const std::wstring &fileId, const std::wstring &workingFolder,
		const std::wstring &folderArchExt )
	{
		unsigned long max_size = std::numeric_limits<long>::max();
		std::string folder = KLSTD_W2A2( workingFolder.c_str() );
				
		std::wstring folderArchiveName;
		std::wstring wstrDir, wstrName, wstrExt;		
		KLSTD_SplitPath( folderFullPath, wstrDir, wstrName, wstrExt );

		if ( zip!=NULL )
		{		
			wstrDir.clear();// = L"";
			KLSTD_MakePath( wstrDir, wstrName, wstrExt, folderArchiveName );

			if ( folderArchiveName.empty() ) return false;

			folderArchiveName += L".";
			folderArchiveName += folderArchExt;
		}
		else
		{
			folderArchiveName = fileId;
		}

		{		
			std::string archiveName = KLSTD_W2A2( folderArchiveName.c_str() );
			mszip::cab_compressor zipFolder(mszip::NONE_TYPE, max_size, folder + "\\", archiveName );

			WIN32_FIND_DATA FindFileData;
			std::wstring sFolder(folderFullPath);
			HANDLE hFind = ::FindFirstFile( KLSTD_W2CT2( std::wstring(sFolder+ L"\\*.*").c_str() ), &FindFileData);
			if (hFind == (void *)0xFFFFFFFF) return false;
			while(true)
			{
				std::wstring curFileName( KLSTD_T2CW2( FindFileData.cFileName ) );
				if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					zipFolder.compress_file_wide( std::wstring(sFolder + L"\\" + curFileName).c_str() );
					KLSTD_Sleep(0);
				}
				else 
				{
					if ( !(curFileName==L"." || curFileName==L".." ))
					{					
						bool addRes = AddFolderInArchive( &zipFolder, std::wstring(sFolder + L"\\" + curFileName),
								fileId, workingFolder, folderArchExt );
						if ( !addRes ) return false;
					}
				}

				if (!::FindNextFile(hFind, &FindFileData)) break;
			}
				
			::FindClose(hFind);
		}		

		if ( zip!=NULL )
		{
			zip->compress_file_wide( std::wstring(workingFolder + L"\\" + folderArchiveName).c_str() );
			KLSTD_Sleep(10);

			KLSTD_Unlink( std::wstring(workingFolder + L"\\" + folderArchiveName).c_str(), false );
		}

		return true;
	}

	bool AddFileInArchive( mszip::cab_compressor *zip, const std::wstring &filePath,
		const std::wstring &archiveFileName, const std::wstring &workingFolder )
	{
		unsigned long max_size = std::numeric_limits<long>::max();
		std::string folder = KLSTD_W2A2( workingFolder.c_str() );
		
		{		
			std::string archiveName = KLSTD_W2A2( archiveFileName.c_str() );
			mszip::cab_compressor zipFolder(mszip::NONE_TYPE, max_size, folder + "\\", archiveName );

			std::wstring curFileName( filePath );

			zipFolder.compress_file_wide( curFileName.c_str() );			
		}

		return true;
	}

	bool IsFileInfoArchPathLess( const KLFT::FileSender::FileInfo& fi1, 
		const KLFT::FileSender::FileInfo& fi2 )
	{
		return fi1.archiveFilePath.length()<fi2.archiveFilePath.length();
	}


	

	bool AddInArchive( 
		mszip::cab_compressor *zip, 
		const std::wstring &curArchPath,
		KLFT::FileSender::FileInfo *curFileInfo,
		KLFT::FileSender::FileInfoList &addFiles,
		const std::wstring &copyFileName,
		const std::wstring &fileId,
		long *archivedFilesCount = NULL )
	{	
		std::wstring wstrWorkingDir, wstrName, wstrExt;	
		std::wstring wstrWorkingFileName, wstrWorkingFileExt;
		KLSTD_SplitPath( copyFileName, wstrWorkingDir, wstrWorkingFileName, wstrWorkingFileExt );

		std::string folder = KLSTD_W2A2( wstrWorkingDir.c_str() );
		if ( !folder.empty() && (folder[folder.length()-1]=='/' || folder[folder.length()-1]=='\\' ) )
		{
			folder.erase( folder.length()-1, 1 );
		}

		unsigned long max_size = std::numeric_limits<long>::max();		
		std::wstring fullArchiveName;		

		bool curIsDir = false;
		bool archiveFileInOtherDir = false;

		std::string archiveName;
		if ( KLSTD_IsDirectory( curFileInfo->sourceFilePath.c_str() ) )
		{
			std::wstring folderArchiveName;
	
			if ( zip!=NULL )
			{	
				folderArchiveName = curArchPath;

				if ( !curFileInfo->archiveFilePath.empty() )
				{	
					if ( !folderArchiveName.empty() ) folderArchiveName += L"\\";
					folderArchiveName += curFileInfo->archiveFilePath;
				}
				if ( folderArchiveName.empty() ) 
				{
					std::wstring wstrDir;
					KLSTD_SplitPath( curFileInfo->sourceFilePath, wstrDir, wstrName, wstrExt );

					folderArchiveName = wstrName;
				}
				else
				{
					folderArchiveName += L".";
					folderArchiveName += fileId;
				}
			}
			else
			{
				folderArchiveName = wstrWorkingFileName + wstrWorkingFileExt;
			}

			archiveName = KLSTD_W2A2( folderArchiveName.c_str() );
			fullArchiveName = wstrWorkingDir + L"\\" + folderArchiveName;
			curIsDir = true;
		}
		else
		{
			std::wstring folderArchiveName;

			std::wstring wstrDir;
			KLSTD_SplitPath( curFileInfo->sourceFilePath, wstrDir, wstrName, wstrExt );

			if ( curFileInfo->archiveFilePath==curArchPath && zip!=NULL ) folderArchiveName = wstrName + wstrExt;
			else 
			{
				archiveFileInOtherDir = true;
				if ( !curFileInfo->archiveFilePath.empty() )
				{
					folderArchiveName = curFileInfo->archiveFilePath + L"." + fileId;
				}
				else
				{				
					folderArchiveName = wstrWorkingFileName + wstrWorkingFileExt;
				}
			}

			archiveName = KLSTD_W2A2( folderArchiveName.c_str() );
			fullArchiveName = wstrWorkingDir + L"\\" + folderArchiveName;
		}
		
		int tempCountFiles = 0;
		{			
			mszip::cab_compressor zipFolder(mszip::NONE_TYPE, max_size, folder + "\\", archiveName );

			if ( curIsDir )
			{	
				WIN32_FIND_DATA FindFileData;
				std::wstring sFolder(curFileInfo->sourceFilePath);
				HANDLE hFind = ::FindFirstFile( KLSTD_W2CT2( std::wstring(sFolder+ L"\\*.*").c_str() ), &FindFileData);
				if (hFind == (void *)0xFFFFFFFF) return false;
				while(true)
				{
					std::wstring curFileName( KLSTD_T2CW2( FindFileData.cFileName ) );
					if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
						zipFolder.compress_file_wide( std::wstring(sFolder + L"\\" + curFileName).c_str() );
						tempCountFiles++;
						if ( tempCountFiles>=10 )
						{
							if ( archivedFilesCount!=NULL ) (*archivedFilesCount)+=tempCountFiles;
							tempCountFiles = 0;
						}
						KLSTD_Sleep(0);
					}
					else 
					{
						if ( !(curFileName==L"." || curFileName==L".." ))
						{	
							KLFT::FileSender::FileInfo subDirFileInfo;
							subDirFileInfo.sourceFilePath = curFileInfo->sourceFilePath + L"\\" + curFileName;
							subDirFileInfo.archiveFilePath = curFileName;

							bool addRes = AddInArchive( &zipFolder, L"", &subDirFileInfo,
								addFiles, copyFileName, fileId, archivedFilesCount );
							if ( !addRes ) return false;
						}
					}

					if (!::FindNextFile(hFind, &FindFileData)) break;
				}
					
				::FindClose(hFind);
			}
			else
			{
				std::wstring compressFileName = curFileInfo->sourceFilePath.c_str();
				std::string::size_type pos;
				if ( (pos = compressFileName.find_last_of(L'/'))!=std::string::npos )
					compressFileName[pos] = L'\\';

				std::wstring onlyFileName;
				if ( curFileInfo->archiveNewFileName.empty() )
				{
					std::wstring::size_type pos = compressFileName.find_last_of(L'\\');
					if(std::wstring::npos != pos) onlyFileName = compressFileName.c_str() + pos + 1;
				}
				else
				{
					onlyFileName = curFileInfo->archiveNewFileName.c_str();
				}				
				
				if ( zip!=NULL && !archiveFileInOtherDir )
					zip->compress_file_wide( compressFileName, onlyFileName );
				else
					zipFolder.compress_file_wide( compressFileName, onlyFileName );

				if ( archivedFilesCount!=NULL ) (*archivedFilesCount)++;
			}

			KLFT::FileSender::FileInfoList::iterator it = addFiles.begin();
			for( ;it != addFiles.end(); )
			{
				if ( it->archiveFilePath==curFileInfo->archiveFilePath )
				{
					KLFT::FileSender::FileInfo fi = (*it);
					it = addFiles.erase( it );
					bool addRes = AddInArchive( &zipFolder, 
						curFileInfo->archiveFilePath, &fi, addFiles, copyFileName, fileId,
						archivedFilesCount );
					if ( !addRes ) return false;
					it = addFiles.begin();
				}
				else  ++it;
			}			

			if ( zip==NULL )
			{	
				for( it = addFiles.begin();it != addFiles.end(); )
				{
					KLFT::FileSender::FileInfo fi = (*it);
					it = addFiles.erase( it );
					bool addRes = AddInArchive( &zipFolder, curArchPath, &fi, addFiles, copyFileName, fileId, archivedFilesCount );
					if ( !addRes ) return false;
					it = addFiles.begin();
				}
			}		
		}

		if ( archivedFilesCount!=NULL ) (*archivedFilesCount)+=tempCountFiles;

		if ( zip!=NULL && (curIsDir || archiveFileInOtherDir ) )
		{
			zip->compress_file_wide( fullArchiveName.c_str() );
			if ( archivedFilesCount!=NULL ) (*archivedFilesCount)++;
			KLSTD_Sleep(10);

			KLSTD_Unlink( fullArchiveName.c_str(), false );
		}		
	
		return true;
	}

	void SplitPathIntoFolders( const std::wstring &path, std::vector<std::wstring> &folders )
	{
		folders.clear();
		std::wstring wstrFolder;
		unsigned int prevPos = 0, i = 0;
		unsigned int len = path.length();
		for( ; i < len - 1; i++ )
		{
			if ( path[i]==L'/' || path[i]==L'\\' )
			{
				wstrFolder = path.substr( prevPos, i - prevPos );
				prevPos = i + 1;
				folders.push_back( wstrFolder );
			}
		}
		
		wstrFolder = path.substr( prevPos, i + 1 - prevPos );
		folders.push_back( wstrFolder );
	}
	
	bool AddInArchive2( 
		mszip::cab_compressor *rootZip, 
		const std::wstring &curArchPath,		
		KLFT::FileSender::FileInfoList &addFiles,
		const std::wstring &copyFileName,
		const std::wstring &fileId,
		bool rootCall )
	{
		std::wstring wstrWorkingDir, wstrName, wstrExt;	
		std::wstring wstrWorkingFileName, wstrWorkingFileExt;
		KLSTD_SplitPath( copyFileName, wstrWorkingDir, wstrWorkingFileName, wstrWorkingFileExt );

		std::wstring folder = wstrWorkingDir;
		if ( !folder.empty() && (folder[folder.length()-1]==L'/' || folder[folder.length()-1]==L'\\' ) )
		{
			folder.erase( folder.length()-1, 1 );
		}

//KLTRACE3( KLCS_MODULENAME, L"AddInArchive2 curArchPath - '%ls' copyFileName - '%ls' rootZip - %d\n", 
//		 curArchPath.c_str(), copyFileName.c_str(), rootZip );

		unsigned long max_size = std::numeric_limits<long>::max();		

		KLFT::FileSender::FileInfoList::iterator it = addFiles.begin();
		for( ;it != addFiles.end(); )
		{
			KLFT::FileSender::FileInfo fi = (*it);
			if ( fi.archiveFilePath==curArchPath )
			{	
				std::wstring onlyFileName;
				if ( fi.archiveNewFileName.empty() )
				{
					std::string::size_type pos = fi.sourceFilePath.find_last_of(L'\\');
					std::string::size_type lpos = fi.sourceFilePath.find_last_of(L'/');
					if(std::string::npos != lpos && lpos>pos ) pos = lpos;
					if(std::string::npos != pos) onlyFileName = fi.sourceFilePath.c_str() + pos + 1;
				}
				else
				{
					onlyFileName = fi.archiveNewFileName.c_str();
				}				


				rootZip->compress_file_wide( fi.sourceFilePath, onlyFileName );

				it = addFiles.erase( it );
			}
			else ++it;
		}

		std::wstring folderArchiveName;

		it = addFiles.begin();
		for( ;it != addFiles.end(); )
		{
			KLFT::FileSender::FileInfo fi = (*it);
			bool match = false;
			if ( !curArchPath.empty() )
			{
				int pos = fi.archiveFilePath.find(curArchPath);
				if ( pos==0 )
				{
					if ( fi.archiveFilePath.length()!=curArchPath.length() )
					{
						wchar_t lastSym = fi.archiveFilePath[curArchPath.length()];
						if ( lastSym==L'/' || lastSym==L'\\' ) match = true;
					}
					else match = true;
				}
			}
			else match = true;
					
			if ( match )
			{
				std::vector<std::wstring> archiveFilePathFoldesList;				
				std::wstring nextCurrentDir;

				SplitPathIntoFolders( fi.archiveFilePath, archiveFilePathFoldesList );
				int restOfArchivePathPos = archiveFilePathFoldesList.size();

				if ( curArchPath.empty() )
				{
					restOfArchivePathPos = 0;
					folderArchiveName = archiveFilePathFoldesList[0] + L"." + fileId;
				}
				else
				{						
					std::vector<std::wstring> curFoldesList;
					SplitPathIntoFolders( curArchPath, curFoldesList );						

					for( restOfArchivePathPos = 0; restOfArchivePathPos<(int)curFoldesList.size() && 
						restOfArchivePathPos<(int)archiveFilePathFoldesList.size(); restOfArchivePathPos++ )
					{
						if ( curFoldesList[restOfArchivePathPos]!=archiveFilePathFoldesList[restOfArchivePathPos] )
							break;
					}

					folderArchiveName = archiveFilePathFoldesList[restOfArchivePathPos] + L"." + fileId;
				}

				if ( restOfArchivePathPos>=0 )
				{
					for( int i = 0; i <= restOfArchivePathPos; i++ )
					{
						if ( !nextCurrentDir.empty() ) nextCurrentDir += L"/";
						nextCurrentDir += archiveFilePathFoldesList[i];
					}
				}
				

				{
					mszip::cab_compressor *zipFolder = NULL;
					mszip::cab_compressor curentFolderZip;

					curentFolderZip.InitWide( mszip::NONE_TYPE, max_size, folder + L"\\", 
						folderArchiveName ); 
					zipFolder = &curentFolderZip;

					bool addRes = AddInArchive2( zipFolder, 
						nextCurrentDir, addFiles, copyFileName, fileId, false );
					if ( !addRes ) return false;
				}
				
				std::wstring prevCompressName = folderArchiveName;
				
				rootZip->compress_file_wide( folder + L"\\" + prevCompressName );

				KLSTD_Unlink( std::wstring(folder + L"\\" + prevCompressName).c_str(), false );

				it = addFiles.begin();
			}
			else  ++it;
		}

		return true;
	}

#endif // _WIN32

	bool CreateArchiveFile( const std::wstring &folderPath, const std::wstring &workingFolder,
		const std::wstring &fileId )
	{
#ifdef _WIN32
		return AddFolderInArchive( NULL, folderPath, fileId, workingFolder, fileId );
#else
		return false;
#endif // _WIN32
	}

	bool CreateArchiveFile( KLFT::FileSender::FileInfo *filesArray, int filesArraySize,
		const std::wstring &copyFileName, const std::wstring &fileId,
		long &totalFilesCount, long &archivedFilesCount )
	{
#ifdef _WIN32
		totalFilesCount = archivedFilesCount = 0;
		KLFT::FileSender::FileInfoList addFiles;
		
		for( int i = 0; i < filesArraySize; i++ )
		{
			addFiles.push_back( filesArray[i] );
			KLERR_IGNORE( totalFilesCount += CountFileInPath( filesArray[i].sourceFilePath ) );
		}

		std::sort( addFiles.begin(), addFiles.end(), IsFileInfoArchPathLess );
		

		KLFT::FileSender::FileInfo curFi = addFiles[0];
		addFiles.erase( addFiles.begin() );

		return AddInArchive( NULL, L"", &curFi, addFiles, copyFileName, fileId, &archivedFilesCount );

#else
		return false;
#endif // _WIN32
	}

	bool CreateArchiveFile2( KLFT::FileSender::FileInfo *filesArray, int filesArraySize,
		const std::wstring &copyFileName, const std::wstring &fileId )
	{
#ifdef _WIN32
		KLFT::FileSender::FileInfoList addFiles;
		
		for( int i = 0; i < filesArraySize; i++ )
		{

			addFiles.push_back( filesArray[i] );
		}

		std::sort( addFiles.begin(), addFiles.end(), IsFileInfoArchPathLess );

//		for( i = 0; i < addFiles.size(); i++ )
//		{
//			KLTRACE3( KLCS_MODULENAME, L"CreateArchiveFile filename - '%ls' - '%ls'\n", addFiles[i].archiveFilePath.c_str(), addFiles[i].sourceFilePath.c_str() );			
//		}

		std::wstring wstrWorkingDir, wstrName, wstrExt;	
		std::wstring wstrWorkingFileName, wstrWorkingFileExt;
		KLSTD_SplitPath( copyFileName, wstrWorkingDir, wstrWorkingFileName, wstrWorkingFileExt );

		std::string folder = KLSTD_W2A2( wstrWorkingDir.c_str() );
		if ( !folder.empty() && (folder[folder.length()-1]=='/' || folder[folder.length()-1]=='\\' ) )
		{
			folder.erase( folder.length()-1, 1 );
		}

		unsigned long max_size = std::numeric_limits<long>::max();
		mszip::cab_compressor zipFolder;
		zipFolder.Init( mszip::NONE_TYPE, max_size, folder + "\\", 
			(const char *)KLSTD_W2A2(fileId.c_str()) ); 


		return AddInArchive2( &zipFolder, L"", addFiles, copyFileName, fileId, true );
#else
		return false;
#endif // _WIN32
	}

	
	bool ExtractArchiveFile( const std::wstring &folderPathPar, const std::wstring &workingFolder,
		const std::wstring &archiveFile, const std::wstring &fileId )
	{
#ifdef _WIN32		

		unsigned long max_size = std::numeric_limits<long>::max();
		std::string folder = KLSTD_W2A2( workingFolder.c_str() );
		std::wstring folderPath = folderPathPar;
		if ( !folderPath.empty() && folderPath[folderPath.length()-1]!=L'/' && folderPath[folderPath.length()-1]!=L'\\' )
		{
			folderPath.append( L"/" );
		}
		std::string archiveName = KLSTD_W2A2( archiveFile.c_str() );
		mszip::cab_decompressor unzip(mszip::NONE_TYPE, max_size, folder + "\\", archiveName );

		unzip.extract( std::string(KLSTD_W2A2( folderPath.c_str() )) );

		{	
			WIN32_FIND_DATA FindFileData;
			std::wstring sFolder(folderPath);
			HANDLE hFind = ::FindFirstFile( KLSTD_W2CT2( std::wstring(sFolder + L"\\*." + fileId ).c_str() ), &FindFileData);
			if (hFind != (void *)0xFFFFFFFF)
			{
				while(true)
				{
					std::wstring curFileName( sFolder + std::wstring(KLSTD_T2CW2( FindFileData.cFileName )) );
					std::wstring wstrDir, wstrName, wstrExt;		
					KLSTD_SplitPath( curFileName, wstrDir, wstrName, wstrExt );
					wstrDir.clear();// = L"";
					std::wstring fName;
					KLSTD_MakePath( wstrDir, wstrName, wstrExt, fName );
					
					KLSTD_CreatePath( std::wstring(folderPath + wstrName).c_str() );				
					bool addRes = ExtractArchiveFile( folderPath + wstrName + L"\\", 
						folderPath,
						fName, fileId );				
					if ( !addRes ) 
					{
						KLSTD_Unlink( curFileName.c_str() );
						return false;
					}
					
					if (!::FindNextFile(hFind, &FindFileData)) 
					{
						KLSTD_Unlink( curFileName.c_str() );
						break;
					}
					
					KLSTD_Unlink( curFileName.c_str() );				
				}
				::FindClose(hFind);
			}
		}		

		return true;
#elif !defined(N_PLAT_NLM)		

		std::string wfolder = KLSTD_W2A2( workingFolder.c_str() );
		std::string archiveName = KLSTD_W2A2( archiveFile.c_str() );
		
		struct mscab_decompressor *cabd;
		struct mscabd_cabinet *cab;
		struct mscabd_folder *folder;
		struct mscabd_file *file;

		cabd = mspack_create_cab_decompressor(NULL);
		if ( cabd==NULL ) return false;

		cab = cabd->open(cabd, (char *)archiveName.c_str() );
		if ( cab==NULL ) return false;

		file = cab->files;
		if ( file==NULL ) return false;

		for( ; file != NULL; file = file->next )
		{
			std::string outFileName;
			outFileName = wfolder + "/" + file->filename;
			int exres = cabd->extract( cabd, file, (char *)outFileName.c_str() );
			if ( exres!=MSPACK_ERR_OK ) return false;
		}

		cabd->close( cabd, cab );
		mspack_destroy_cab_decompressor( cabd );
#endif // _WIN32
	}

	
	
	File::File()
	{
		KLSTD_CreateCriticalSection( &m_pCricSec );
		m_delay = 0;
		m_uploadChunkTimeout = 0;
		m_uploadChunkFailedCount = 0;
		m_lastOperationTime = 0;
		m_createTime = 0;
		m_createFileTime = 0;
		m_broadcastId = (-1);
		m_inBroadcastFlag = false;
		m_BroadcastCount = 0;
		m_isDirectory = false;
		m_maxNumberUploadingClients = c_defaultUploadingClients;
		m_customFileMode = false;
		m_updateAgentFileStatus = File::UAFS_None;
		m_calculatedUploadDelayTimeout = 0;
		m_partlySavedFile = false;
		m_fileType = KLFT::FT_None;
		m_multicastRecvSize = 0;
		m_totalFilesInArchive = m_archivedFilesCount = 0;
	}
	
	File::~File()
	{
	}

	void File::Init( const std::wstring &fileId, const std::wstring &copyFileName,
		KLFT::FileSender::FileInfo *filesArray, int filesArraySize )
	{
		std::wstring filePath = filesArray[0].sourceFilePath;

		std::wstring wstrDir, wstrName, wstrExt;
		KLSTD_SplitPath( filePath, wstrDir, wstrName, wstrExt );
		wstrDir.clear();
		KLSTD_MakePath( wstrDir, wstrName, wstrExt, m_fileName );

		m_fileId = fileId;
		m_description.clear();// = L"";
		m_copyFileName = copyFileName;
		m_infoFilePath.clear();// = L"";
		time( &m_createTime );

#ifdef _WIN32
		m_isDirectory = true;
		
		CreateArchiveFile( filesArray, filesArraySize, m_copyFileName, fileId, 
			m_totalFilesInArchive, m_archivedFilesCount );
#else
		filePath = filesArray[0].sourceFilePath;
		KLSTD_CopyFile( filePath.c_str(), m_copyFileName.c_str(), false );
#endif
				
	}

	//!\brief Инициализация переменных класса
	void File::Init( const std::wstring &fileId,
			const std::wstring &filePath, const std::wstring &description,
			const std::wstring &workingFolder, bool copyFile,
			bool createFile, bool updaterFile )
	{
		std::wstring wstrDir, wstrName, wstrExt;
		KLSTD_SplitPath( filePath, wstrDir, wstrName, wstrExt );
		wstrDir.clear();// = L"";
		KLSTD_MakePath( wstrDir, wstrName, wstrExt, m_fileName );

		m_fileId = fileId;
		m_description = description;
		m_copyFileName = workingFolder + L"/";
		m_copyFileName += fileId;
		m_infoFilePath = m_copyFileName + c_InfoFileExt;
		time( &m_createTime );

		if ( copyFile ) 
		{
#ifdef _WIN32
			if ( KLSTD_IsDirectory( filePath.c_str() ) )
			{
				m_isDirectory = true;

				CreateArchiveFile( filePath, workingFolder, fileId );
			}
			else
				KLSTD_CopyFile( filePath.c_str(), m_copyFileName.c_str(), false );

#else
			KLSTD_CopyFile( filePath.c_str(), m_copyFileName.c_str(), false );
#endif
		
			KLSTD_CreateFile( m_copyFileName.c_str(), KLSTD::SF_READ | KLSTD::SF_WRITE,
				KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ | KLSTD::AF_WRITE, 0,
				&m_file, 10000 );

			m_fullFileSize = (unsigned long)m_file->GetSize();
			m_createFileTime = KLSTD::GetFileCreateTime( m_copyFileName );
		}
		else
		{
			m_fullFileSize = 0;

			if ( createFile )
			{
				KLSTD_Unlink( m_copyFileName.c_str(), false );
				KLSTD_CreateFile( m_copyFileName, KLSTD::SF_READ | KLSTD::SF_WRITE,
					KLSTD::CF_CREATE_NEW, KLSTD::AF_READ | KLSTD::AF_WRITE, 0,
					&m_file, 10000 );				
			}
			else
			{
				m_copyFileName = filePath;
				if ( KLSTD_IsDirectory( filePath.c_str() ) )
				{
					m_isDirectory = true;
				}
				else				
				{
					if ( updaterFile )
					{
#ifdef _WIN32
						HANDLE hFile = INVALID_HANDLE_VALUE;
						hFile=CreateFile(
							KLSTD_W2CT2(m_copyFileName.c_str()),
							GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
							NULL,
							OPEN_EXISTING,
							0, 0 );
						if(hFile==INVALID_HANDLE_VALUE) KLSTD_THROW_LASTERROR();

						DWORD dwSizeHigh=0;
						DWORD dwResult=GetFileSize(hFile, &dwSizeHigh);
						if(dwResult==-1) KLSTD_THROW_LASTERROR();
						m_fullFileSize = (unsigned long)(AVP_qword(dwResult) | (AVP_qword(dwSizeHigh) << 32));
						CloseHandle(hFile);
#endif
					}
					else
					{
						KLSTD_CreateFile( m_copyFileName, KLSTD::SF_READ | KLSTD::SF_WRITE,
							KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ, 0,
							&m_file, 10000 );
						m_fullFileSize = (unsigned long)m_file->GetSize();
						m_createFileTime = KLSTD::GetFileCreateTime( m_copyFileName );
					}
				}
			}
		}

		
		m_currentUploadPos = 0;
	}
	
	//!\brief Создает описание файла для отсылки клиентам
	void File::CreateServerFile( const std::wstring &filePath, 
		const std::wstring &description, 		
		const std::wstring &workingFolder,
		const KLTRAP::Transport::LocationExList &locsList, 
		unsigned long broadcastDelay,
		std::wstring &fileId,
		int maxNumberUploadingClients )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( workingFolder.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		if ( fileId.empty() ) fileId = KLSTD_CreateGUIDString();

		Init( fileId, filePath, description, workingFolder, true, false );

		m_currentUploadPos = m_fullFileSize;

		m_notSendLocsList = locsList;

		m_currentStatus = FileDescription::Sending;

		m_delay = broadcastDelay;

		KLTRACE3( KLCS_MODULENAME, L"File::CreateServerFile fileId - '%ls' description - '%ls' fileName - '%ls'\n",
			fileId.c_str(), description.c_str(), filePath.c_str() );

		TraceLocsList( m_notSendLocsList );

		m_maxNumberUploadingClients = maxNumberUploadingClients;

		CalculateDelayTimeout();

		SaveInfoFile();
	}
	
	//\brief Установка максимального кол-ва клиентов для закачки 
	void File::SetMaxNumberUploadingClients( int maxNumberUploadingClients )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_maxNumberUploadingClients = maxNumberUploadingClients;
	}
	
	//!\brief Создает описание файла для загрузки с сервера
	void File::CreateClientFile( const std::wstring &fileName, const std::wstring &fileId,
		const std::wstring &description, bool isDir, unsigned long fullFileSize,
		KLFT::FileType fileType,
		const bool useConnectionNames,
		const KLTRAP::Transport::ConnectionFullName &connFullName, 		
		unsigned long uploadDelay, unsigned long chunkTimeout,
		bool forUpdateAgentFlag,
		const UpdateAgentLocationList &updateAgentsLocs,
		const std::wstring &workingFolder )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( workingFolder.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		Init( fileId, L"", description, workingFolder, false, true );
		m_fileName = fileName;
		m_fullFileSize = fullFileSize;
		m_delay = KLSTD_Random( 0, uploadDelay );
		m_uploadChunkTimeout = 0;		
		m_clientName = connFullName.remoteComponentName;
		if ( useConnectionNames )
		{
			m_localName = connFullName.localComponentName;
		}
		m_isDirectory = isDir;
		if ( forUpdateAgentFlag ) m_updateAgentFileStatus = (File::UAFS_DownloadingFromServer | File::UAFS_UseOnlyForUpdateAgentFlag);

		m_fileType = fileType;
		m_updateAgentsLocs = updateAgentsLocs;

		m_currentStatus = FileDescription::Uploading;

		CalculateDelayTimeout();

		KLTRACE3( KLCS_MODULENAME, L"File::CreateClientFile fileId - '%ls' description - '%ls' fileName - '%ls' Is directory - %d fullFileSize - %d upload delay - %d forUpdateAgentFlag - %d ualocs.size - %d\n",
			fileId.c_str(), description.c_str(), fileName.c_str(), isDir, fullFileSize, m_delay, forUpdateAgentFlag, updateAgentsLocs.size() );

		SaveInfoFile();
	}

	//!\brief Обновляет описание файла для загрузки с сервера
	void File::UpdateClientFile( const std::wstring &fileName, const std::wstring &fileId,
			const std::wstring &description, bool isDir, unsigned long fullFileSize,
			KLFT::FileType fileType,
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName, 
			unsigned long uploadDelay, unsigned long chunkTimeout,
			bool forUpdateAgentFlag,
			const UpdateAgentLocationList &updateAgentsLocs,
			const std::wstring &workingFolder )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( workingFolder.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );
	
		m_description = description;
		m_copyFileName = workingFolder + L"/";
		m_copyFileName += fileId;
		m_infoFilePath = m_copyFileName + c_InfoFileExt;
		time( &m_createTime );

		m_fileName = fileName;
		m_fullFileSize = fullFileSize;
		m_delay = KLSTD_Random( 0, uploadDelay );
		m_uploadChunkTimeout = 0;		
		m_clientName = connFullName.remoteComponentName;
		if ( useConnectionNames )
		{
			m_localName = connFullName.localComponentName;
		}
		m_isDirectory = isDir;
		if ( forUpdateAgentFlag ) m_updateAgentFileStatus = (File::UAFS_DownloadingFromServer || File::UAFS_UseOnlyForUpdateAgentFlag);

		m_fileType = fileType;
		m_updateAgentsLocs = updateAgentsLocs;

		m_currentStatus = FileDescription::Uploading;

		KLTRACE3( KLCS_MODULENAME, L"File::CreateClientFile ( UpdateClientFile ) fileId - '%ls' description - '%ls' fileName - '%ls' Is directory - %d fullFileSize - %d upload delay - %d forUpdateAgentFlag - %d\n",
			fileId.c_str(), description.c_str(), fileName.c_str(), isDir, fullFileSize, m_delay, forUpdateAgentFlag );

		SaveInfoFile();
	}
	
	//!\brief Создает описание package для получения с сервера
	void File::CreatePackageFile( const std::wstring &fileName, const std::wstring &fileId,
			const std::wstring &description, 
			KLFT::FileType fileType,
			bool isDir,
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName, 
			unsigned long uploadDelay, unsigned long chunkTimeout,
			const std::wstring &workingFolder )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( workingFolder.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		m_fileId = fileId;
		m_description = description;		
		time( &m_createTime );
		
		m_fileName = fileName;
		m_fullFileSize = 0;
		m_delay = KLSTD_Random( 0, uploadDelay );
		m_uploadChunkTimeout = 0;		
		m_clientName = connFullName.remoteComponentName;
		if ( useConnectionNames )
		{
			m_localName = connFullName.localComponentName;
		}
		m_isDirectory = isDir;		
		m_copyFileName = workingFolder + L"/";
		m_copyFileName += fileId;
		m_infoFilePath = m_copyFileName + c_InfoFileExt;

		m_fileType = fileType;
		
		KLFT_CreateFolderSync( &m_pFolderSync );
		m_pFolderSync->InitClient( fileId, workingFolder );

		m_currentStatus = FileDescription::Uploading;

		KLTRACE3( KLCS_MODULENAME, L"File::CreatePackageFile fileId - '%ls' description - '%ls' fileName - '%ls' Is directory - %d upload delay - %d\n",
			fileId.c_str(), description.c_str(), fileName.c_str(), isDir, m_delay );

		SaveInfoFile();
	}

	//!\brief Создает описание файла для загрузки с UA
	void File::CreateEndPointFile( const std::wstring &fileId,
			const std::wstring &description,
			const std::wstring &workingFolder )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( workingFolder.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		m_fileId = fileId;
		m_description = description;		
		time( &m_createTime );
		
		m_fileType = KLFT::FT_FolderEndPoint;
		m_customFileMode = true;
				
		KLFT_CreateFolderSync( &m_pFolderSync );
		m_pFolderSync->InitEndPoint( fileId, workingFolder );

		m_currentStatus = FileDescription::Uploaded;

		KLTRACE3( KLCS_MODULENAME, L"File::CreateEndPointFile fileId - '%ls' description - '%ls'\n",
			fileId.c_str(), description.c_str() );

		SaveInfoFile();
	}


	//!\brief Обновляет пакетный файл ( выставляет признак необходимости синхронизации )
	void File::UpdatePackageFile( KLFT::FileType &newFileType )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileType!=KLFT::FT_Package && m_fileType!=KLFT::FT_Folder && m_fileType!=FT_FolderEndPoint )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

		m_currentStatus = FileDescription::Uploading;	
		m_lastOperationTime = 0;
		m_fileType = newFileType;

		KLTRACE3( KLCS_MODULENAME, L"File::UpdatePackageFile fileId - '%ls'\n",
			m_fileId.c_str() );

		SaveInfoFile();
	}

	//!\brief Преобразует клиентский файл в серверный формат 
	void File::ConvertToServerFile( KLPAR::Params *pFullFileInfo )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		std::wstring fileId;
		GET_PARAMS_VALUE( pFullFileInfo, KLFT::SER_FILE_ID, StringValue, STRING_T, fileId );
		if ( fileId!=m_fileId )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

//KLPAR_SerializeToFileName( L"#pFullFileInfo.xml", pFullFileInfo );

		GET_PARAMS_VALUE( pFullFileInfo, KLFT::SER_MAX_CLIENT, IntValue, INT_T, m_maxNumberUploadingClients );
		GET_PARAMS_VALUE( pFullFileInfo, KLFT::SER_CREATE_TIME, DateTimeValue, DATE_TIME_T, m_createTime );

		KLSTD::CAutoPtr<KLPAR::Params> pHostPars;

		GET_PARAMS_VALUE( pFullFileInfo, KLFT::SER_HOSTS_DESCS, ParamsValue, PARAMS_T, pHostPars );

		m_notSendLocsList.clear();

		KLPAR::ParamsNames parNames;
		pHostPars->GetNames( parNames );
		KLPAR::ParamsNames::iterator it = parNames.begin();		
		for( ; it != parNames.end(); ++it )
		{
			KLSTD::CAutoPtr<KLPAR::Params> pHostDesc;
			
			GET_PARAMS_VALUE( pHostPars, (*it).c_str(), ParamsValue, PARAMS_T, pHostDesc );

			int hostId;
			std::wstring hostStrId, connName, hostDisplayName;

			GET_PARAMS_VALUE( pHostDesc, KLFT::SER_HOST_ID, IntValue, INT_T, hostId );
			GET_PARAMS_VALUE( pHostDesc, KLFT::SER_HOST_STR_ID, StringValue, STRING_T, hostStrId );
			GET_PARAMS_VALUE( pHostDesc, KLFT::SER_HOST_CONN_NAME, StringValue, STRING_T, connName );
			GET_PARAMS_VALUE( pHostDesc, KLFT::SER_HOST_DISPLAY_NAME, StringValue, STRING_T, hostDisplayName );

			KLTRAP::Transport::LocationEx locEx;

			locEx.remoteComponentName = connName + L";" + hostStrId;
				
			m_notSendLocsList.push_back( locEx );
		}

		m_currentUploadPos = m_fullFileSize;

		m_currentStatus = FileDescription::Sending;

		m_updateAgentFileStatus |= File::UAFS_Downloaded;

		KLTRACE3( KLCS_MODULENAME, L"File::ConvertToServerFile fileId - '%ls' description - '%ls'\n",
			m_fileId.c_str(), m_description.c_str() );
		KLTRACE3( KLCS_MODULENAME, L"File::ConvertToServerFile not send:\n");
		TraceLocsList( m_notSendLocsList );

		SaveInfoFile();				
	}

	//!\brief Создает описание файла для загрузки 
	void File::CreateServerUploadedClientFile( const std::wstring &fileId,
			bool isDir, unsigned long fullFileSize,
			const std::wstring &remoteClientName, 			
			const std::wstring &workingFolder )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( workingFolder.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		Init( fileId, L"", L"", workingFolder, false, true );		
		m_fullFileSize = fullFileSize;		
		m_clientName = remoteClientName;
		m_isDirectory = isDir;

		m_fileType = KLFT::FT_CustomServerUploaded;

		m_customFileMode = true;

		m_currentStatus = FileDescription::Uploading;

		KLTRACE3( KLCS_MODULENAME, L"File::CreateCustomClientFile fileId - '%ls' Is directory - %d fullFileSize - %d\n",
			fileId.c_str(), isDir, fullFileSize );

		SaveInfoFile();
	}

	//!\brief Создает описание файла из папки updater для отсылки с сервера
	void File::CreateUpdaterFolderFile( const std::wstring &filePath, 
		int maxUploadClients )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		Init( filePath, filePath, L"", L"", false, false, true );

		m_customFileMode = true;
		m_currentStatus = FileDescription::Uploading;
		m_maxNumberUploadingClients = maxUploadClients;
	}

	//!\brief Создает описание файла для отсылки на сервер
	void File::CreateSendToFile( KLFT::FileSender::FileInfo *filesArray, int filesArraySize,
		std::wstring &fileId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( fileId.empty() ) fileId = KLSTD_CreateGUIDString();

		std::wstring wstrTmpFileName;
		KLSTD_GetTempFile( wstrTmpFileName );
		Init( fileId, wstrTmpFileName, filesArray, filesArraySize );

		m_customFileMode = true;
		m_currentStatus = FileDescription::Uploading;		

		ReadFileSize();
	}
	
	//!\brief Создает описание файла из файла описания ( процедура востановления )
	void File::CreateFromInfoFile( const std::wstring &infoFilePath )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_infoFilePath = infoFilePath;
		int pos = infoFilePath.find(c_InfoFileExt);
		if ( pos==std::wstring::npos) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, infoFilePath.c_str() );

		KLERR_TRY
		{		
			RestoreFromInfoFile();
		}
		KLERR_CATCH( pError )
		{
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, infoFilePath.c_str() );
		}
		KLERR_ENDTRY

		if ( m_fileType==KLFT::FT_Package || m_fileType==KLFT::FT_Folder ||
			m_fileType==KLFT::FT_FolderEndPoint )
		{
			KLFT_CreateFolderSync( &m_pFolderSync );
			m_pFolderSync->InitClient( m_fileId, infoFilePath );
		}
		else
		{
			std::wstring wstrDir, wstrName, wstrExt;		
			KLSTD_SplitPath( m_infoFilePath, wstrDir, wstrName, wstrExt );
			m_copyFileName = wstrDir + L"/";
			m_copyFileName += m_fileId;
			
			KLSTD_CreateFile( m_copyFileName.c_str(), KLSTD::SF_READ | KLSTD::SF_WRITE,
				KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ | KLSTD::AF_WRITE, 0,
				&m_file, 10000 );
		}
	}
	
	//!\brief Возвращает идентификатор файла
	std::wstring File::GetFileId()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_fileId;
	}

	//!\brief Возвращает идентификатор клиента, выданный сервером
	std::wstring File::GetClientName()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_clientName;
	}

	//!\brief Возвращает лоакльное имя соединение, которое надо использовать для закачки данного файла
	std::wstring File::GetLocalName()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_localName;
	}
	
	//!\brief Получение информации о файле
	FileDescription File::GetFileDescription()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileDescription fdesc;

		fdesc.m_status = m_currentStatus;
		fdesc.m_type = Type();
		fdesc.m_fileName = m_fileName;
		fdesc.m_fileId = m_fileId;
		fdesc.m_description = m_description;
		fdesc.m_createFileTime = m_createFileTime;
		fdesc.m_operationTime = m_createTime;
		fdesc.m_fileSize = m_fullFileSize;
		fdesc.m_currentUploadPos = m_currentUploadPos;
		fdesc.m_sentLocsList = m_sentLocsList;
		fdesc.m_notSendLocsList = m_notSendLocsList;
		fdesc.m_uploadLocsList = m_uploadLocsList;
		fdesc.m_isDirectory = m_isDirectory;
		fdesc.m_maxNumberUploadingClients = m_maxNumberUploadingClients;
		fdesc.m_multicastReceived = m_multicastRecvSize;

		return fdesc;
	}
	
	//!\brief Статус рабочего файла
	void File::SetStatus( FileDescription::FileStatus newStatus )
	{	
		KLTRACE3( KLCS_MODULENAME, L"File::SetStatus fileId - '%ls' new status - %d previous status - %d\n",
			m_fileId.c_str(), newStatus, m_currentStatus );

		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_currentStatus = newStatus;

		if ( m_fileType==FT_Package || m_fileType==FT_Folder )
		{
			SaveInfoFile();
		}
	}

	//!\brief Статус рабочего файла
	FileDescription::FileStatus File::GetStatus()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_currentStatus;
	}
	
	//!\brief Добавление новой "порции" файла
	bool File::AddChunk( void *buff, unsigned long buffSize, unsigned long CRC, 
		unsigned long startPosInFile, unsigned long fullFileSize,
		bool fromMulticast )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( buff==NULL || buffSize==0 )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		if ( m_file==NULL )
			KLFT_THROW_ERROR1( ERR_BAD_FILE_OBJECT, m_fileId.c_str() );

		if ( m_fullFileSize!=fullFileSize )
			KLFT_THROW_ERROR1( ERR_ERROR_OF_UPLOAD, m_fileId.c_str() );

		if ( !fromMulticast && m_currentUploadPos!=startPosInFile )
			KLFT_THROW_ERROR1( ERR_ERROR_OF_UPLOAD, m_fileId.c_str() );

		m_file->Seek( startPosInFile, KLSTD::ST_SET );
		AVP_dword addOffset = m_file->Write( buff, buffSize );

		if ( addOffset<=0 )
			KLFT_THROW_ERROR1( ERR_ERROR_OF_UPLOAD, m_fileId.c_str() );

		if ( !fromMulticast )
		{
			m_currentUploadPos+=addOffset;
		}
		else
		{
			if ( startPosInFile<m_currentUploadPos )
			{
				KLTRACE3( KLCS_MODULENAME, L"File::AddChunk (multicast packet arrived after TCP uploading started) fileId - '%ls",
					m_fileId.c_str() );
				return true;
			}
		}

		time(&m_lastOperationTime);

		if ( !fromMulticast || m_recievedChunks.size()==0 )
		{
			KLTRACE3( KLCS_MODULENAME, L"File::AddChunk fileId - '%ls' size - %d Upload pos - %d Chunk size - %d multicast - %d\n",
				m_fileId.c_str(), m_currentUploadPos, fullFileSize, buffSize, fromMulticast );
		}

		if ( fromMulticast )			
		{
			PartlySavedFile( true );
		}

		bool partlyNotCheckFullSaved = true;
		int multicastRecivedBuffSize = buffSize;
		if ( m_partlySavedFile )
		{
			CloseChunkType chunkType;
			RecievedChunkList::iterator it = GetCloseChunksListItem( startPosInFile, 
				buffSize, chunkType );

			if ( it==m_recievedChunks.end() )
			{
				RecievedChunk rchunk;
				rchunk.Init( startPosInFile, buffSize );				
				bool added = false;
				for( RecievedChunkList::iterator fit = m_recievedChunks.begin(); fit!=m_recievedChunks.end(); fit++ )
				{
					if ( fit->pos > rchunk.pos )
					{
						m_recievedChunks.insert( fit, rchunk );
						added = true;
						break;
					}
				}
				if ( !added ) m_recievedChunks.push_back( rchunk );
			}
			else
			{
				multicastRecivedBuffSize = 0;

				if ( chunkType==CCT_Equal ) 
				{
					if ( it->size < buffSize ) 
					{
						multicastRecivedBuffSize = buffSize - it->size;
						it->size = buffSize;
					}
				}
				if ( chunkType==CCT_Right )
				{
					if ( (startPosInFile+buffSize)>(it->pos + it->size) )
					{
						multicastRecivedBuffSize = (buffSize - ((it->pos + it->size) - startPosInFile));
						it->size += multicastRecivedBuffSize;
					}
				}
				if ( chunkType==CCT_Left )
				{
					multicastRecivedBuffSize = it->pos - startPosInFile + it->size;
					it->size = multicastRecivedBuffSize;
					it->pos = startPosInFile;
				}

				if ( m_recievedChunks.size()==1 && it->pos==0 ) partlyNotCheckFullSaved = true;
				else partlyNotCheckFullSaved = false;

				ClueRecievedChunksList();
			}

			//TraceReceivedChunksList( m_recievedChunks );
		}

		if ( fromMulticast )
		{
			m_multicastRecvSize += multicastRecivedBuffSize;
		}

		if ( m_currentUploadPos==fullFileSize && (!fromMulticast || partlyNotCheckFullSaved) )
		{
			SetStatus( FileDescription::Uploaded );

			// если качаем файл для UpdateAgent то сохранение статуса проиведем только после получения списка рассылки
			if ( !(GetUpdateAgentFileStatus() & File::UAFS_DownloadingFromServer) ) 
			{	
				SaveInfoFile();
			}
			
			return true;
		}
		else
		{
			if ( m_currentUploadPos>fullFileSize )
				KLFT_THROW_ERROR1( ERR_ERROR_OF_UPLOAD, m_fileId.c_str() );
			
			SaveInfoFile();
			return false;
		}
	}

	//!\brief Считывает очередную "порции" файла
	int File::GetChunk( const std::wstring &remoteClientName,
		unsigned long startPosInFile,			
		void *buff, unsigned long buffSize, bool &endOfFile )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		endOfFile = false;

		if ( buff==NULL || buffSize==0 )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		unsigned long read = 0;
		if ( m_customFileMode )
		{
#ifdef _WIN32
			HANDLE hFile = INVALID_HANDLE_VALUE;
			hFile=CreateFile(
				KLSTD_W2CT2(m_copyFileName.c_str()),
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				0, 0 );
			if(hFile==INVALID_HANDLE_VALUE) 
			{
				std::wstring wstrDir, wstrName, wstrExt;
				KLSTD_SplitPath( m_copyFileName, wstrDir, wstrName, wstrExt );
				KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, 
					std::wstring( wstrName + wstrExt ).c_str() );
			}

			DWORD dwSizeHigh=0;
			DWORD dwResult=GetFileSize(hFile, &dwSizeHigh);
			if(dwResult==-1) KLSTD_THROW_LASTERROR();
			m_fullFileSize = (unsigned long)(AVP_qword(dwResult) | (AVP_qword(dwSizeHigh) << 32));

			AVP_longlong llOffset = startPosInFile;
			LONG lDistanceToMoveHigh=(long)(llOffset >> 32);
			LONG lDistanceToMove=(long)llOffset;
			dwResult=SetFilePointer(hFile, lDistanceToMove, &lDistanceToMoveHigh, FILE_BEGIN);
			if(dwResult==-1) KLSTD_THROW_LASTERROR();
			DWORD dwRead=0;
			if(!ReadFile(hFile, buff, buffSize, &dwRead, NULL)) KLSTD_THROW_LASTERROR();
			read = dwRead;
			CloseHandle( hFile );
#endif
		}
		else
		{
			if ( m_file==NULL )
				KLFT_THROW_ERROR1( ERR_BAD_FILE_OBJECT, m_fileId.c_str() );
			
			if ( startPosInFile>=m_fullFileSize )
				KLFT_THROW_ERROR1( ERR_ERROR_OF_UPLOAD, m_fileId.c_str() );
						
			m_file->Seek( startPosInFile, KLSTD::ST_SET );
			
			read = m_file->Read( buff, buffSize );
		}
		
		if ( startPosInFile+read >= m_fullFileSize )		
		{
			endOfFile = true;
			UploadingClientsList::iterator fit = m_uploadingClients.find( remoteClientName );
			if ( fit != m_uploadingClients.end() )
			{
				m_uploadingClients.erase( fit );
			}
		}

		return read;

	}
	
	//!\brief Добавление клиента в список обработанных
	bool File::AddSentLocation( const std::wstring &remoteName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		bool found = false;

		KLTRAP::Transport::LocationExList::iterator it = m_notSendLocsList.begin();
		for( ; it != m_notSendLocsList.end(); ++it )
		{
			if ( (*it).remoteComponentName.find(remoteName)!=std::wstring::npos )
			{
				m_sentLocsList.push_back( (*it) );
				m_notSendLocsList.erase( it );
				found = true;				
				break;
			}
		}

		if ( !found ) return false;
			
		if ( m_notSendLocsList.empty() ) 
		{
			SetStatus( FileDescription::Sent );						
		}

		SaveInfoFile();

		KLTRACE3( KLCS_MODULENAME, L"File::AddSentLocation fileId - '%ls'\n", m_fileId.c_str() );
		KLTRACE3( KLCS_MODULENAME, L"File::AddSentLocation not send:\n");
		TraceLocsList( m_notSendLocsList );
		KLTRACE3( KLCS_MODULENAME, L"File::AddSentLocation sent:\n");
		TraceLocsList( m_sentLocsList );

		return true;
	}
	
	//!\brief Добавление клиента в список обработанных
	bool File::AddUploadedLocation( const std::wstring &remoteName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		KLTRAP::Transport::LocationExList::iterator it = m_sentLocsList.begin();
		for( ; it != m_sentLocsList.end(); ++it )
		{
			if ( (*it).remoteComponentName.find(remoteName)!=std::wstring::npos ) break;
		}

		if ( it == m_sentLocsList.end() ) return false;

		m_uploadLocsList.push_back( *it );

		bool res = false;
		if ( CheckDeliveredStatus() && m_notSendLocsList.empty() )
		{
			SetStatus( FileDescription::Delivered );
			res = true;
		}

		SaveInfoFile();

		KLTRACE3( KLCS_MODULENAME, L"File::AddUploadedLocation fileId - '%ls' ", m_fileId.c_str() );
		TraceLocsList( m_uploadLocsList );

		return res;
	}

	//!\brief Добавляет список клиентов для отправки
	void File::AddClientsForSend( const KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( GetStatus()==FileDescription::Uploaded ||
			GetStatus()==FileDescription::Uploading ||
			GetStatus()==FileDescription::MulticastUploading ) 
		{
			// данный файл клиентский
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, m_fileId.c_str() );
		}			
		
		KLTRAP::Transport::LocationExList listForAdd;		
		KLTRAP::Transport::LocationExList::const_iterator clit = locsList.begin();
		for( ; clit != locsList.end(); ++clit )
		{
			bool found = false;
			bool emptyFlag = m_notSendLocsList.empty();
			KLTRAP::Transport::LocationExList::iterator it = m_notSendLocsList.begin();
			for( ; it != m_notSendLocsList.end() && !found ; ++it )
			{	
				if ( it->remoteComponentName==clit->remoteComponentName )
					found = true;
			}
			if ( !found )
			{
				listForAdd.push_back( (*clit) );
			}
		}

		if ( !listForAdd.empty() )
		{
			KLTRAP::Transport::LocationExList::iterator clait = listForAdd.begin();
			for( ; clait!= listForAdd.end(); ++clait )
			{
				m_notSendLocsList.push_back( (*clait) );
			}

			SetStatus( FileDescription::Sending );
			SaveInfoFile();
		}	

		CalculateDelayTimeout();

		KLTRACE3( KLCS_MODULENAME, L"File::AddClientsForSend fileId - '%ls' ", m_fileId.c_str() );
		TraceLocsList( m_notSendLocsList );
	}

	//!\brief Удаляет из спсика клиентов для отправки
	void File::DeleteClients( const std::list<std::wstring> &compsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( GetStatus()==FileDescription::Uploaded ||
			GetStatus()==FileDescription::Uploading ||
			GetStatus()==FileDescription::MulticastUploading ) 
		{
			// данный файл клиентский
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, m_fileId.c_str() );
		}			
				
		std::list<std::wstring>::const_iterator clit = compsList.begin();
		for( ; clit != compsList.end(); ++clit )
		{			
			KLTRAP::Transport::LocationExList::iterator it = m_notSendLocsList.begin();
			for( ; it != m_notSendLocsList.end(); ++it )
			{	
				if ( it->remoteComponentName==(*clit) )
				{
					m_notSendLocsList.erase( it );
					break;
				}
			}

			it = m_sentLocsList.begin();
			for( ; it != m_sentLocsList.end(); ++it )
			{	
				if ( it->remoteComponentName==(*clit) )
				{
					m_sentLocsList.erase( it );
					break;
				}
			}
		}

		if ( m_notSendLocsList.empty() )
		{
			SetStatus( FileDescription::Sent );
			if ( CheckDeliveredStatus() )
				SetStatus( FileDescription::Delivered );
		}	

		SaveInfoFile();

		KLTRACE3( KLCS_MODULENAME, L"File::DeleteClients fileId - '%ls' ", m_fileId.c_str() );
		TraceLocsList( m_notSendLocsList );
	}

	//!\brief Обнавляет список клиентов для указанного файла
	void File::UpdateClientsList( const KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( GetStatus()==FileDescription::Uploaded ||
			GetStatus()==FileDescription::Uploading ||
			GetStatus()==FileDescription::MulticastUploading ) 
		{
			// данный файл клиентский
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, m_fileId.c_str() );
		}			

		KLTRAP::Transport::LocationExList::const_iterator clit = locsList.begin();
		bool addNew = false;
		for( ; clit != locsList.end(); ++clit )
		{
			bool found = false;
			unsigned int listPos = 0;			
			for( ;listPos < m_notSendLocsList.size() && !found; ++listPos )
			{	
				if ( m_notSendLocsList[listPos].remoteComponentName==clit->remoteComponentName )
				{					
					found = true;
				}
			}
			listPos = 0;			
			for( ;listPos < m_sentLocsList.size() && !found; ++listPos )
			{	
				if ( m_sentLocsList[listPos].remoteComponentName==clit->remoteComponentName )
				{					
					found = true;
				}
			}
			if ( !found ) 
			{
				m_notSendLocsList.push_back((*clit));			
				addNew = true;
			}
		}
		
		KLTRAP::Transport::LocationExList::iterator itloc = m_notSendLocsList.begin();
		while( itloc != m_notSendLocsList.end() )
		{
			KLTRAP::Transport::LocationExList::const_iterator clit = locsList.begin();			
			for( ; clit != locsList.end(); ++clit )
			{
				if ( itloc->remoteComponentName==clit->remoteComponentName ) break;
			}
			if ( clit==locsList.end() )  itloc = m_notSendLocsList.erase( itloc );
			else itloc++;
		}

		itloc = m_sentLocsList.begin();
		while( itloc != m_sentLocsList.end() )
		{
			KLTRAP::Transport::LocationExList::const_iterator clit = locsList.begin();			
			for( ; clit != locsList.end(); ++clit )
			{
				if ( itloc->remoteComponentName==clit->remoteComponentName ) break;
			}
			if ( clit==locsList.end() )  itloc = m_sentLocsList.erase( itloc );
			else itloc++;
		}

		if ( addNew ) SetStatus( FileDescription::Sending );
		
		if ( m_notSendLocsList.empty() )
		{
			SetStatus( FileDescription::Sent );
			if ( CheckDeliveredStatus() )
				SetStatus( FileDescription::Delivered );
		}	

		if ( GetStatus()==FileDescription::Sending && Type()==FT_Package )
		{
			if ( m_pFolderSync!=NULL ) m_pFolderSync->SyncServerFolder();
		}

		SaveInfoFile();

		CalculateDelayTimeout();
		
		KLTRACE3( KLCS_MODULENAME, L"File::UpdateClientsList fileId - '%ls' ", m_fileId.c_str() );
		TraceLocsList( m_notSendLocsList );
	}

	//!\brief Перезаписывает список клиентов для указанного файла
	void File::ResetClientsList( const KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( GetStatus()==FileDescription::Uploaded ||
			GetStatus()==FileDescription::Uploading ||
			GetStatus()==FileDescription::MulticastUploading ) 
		{
			// данный файл клиентский
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, m_fileId.c_str() );
		}			

		m_notSendLocsList = locsList;
		m_sentLocsList.clear();

		SetStatus( FileDescription::Sending );
		
		if ( m_notSendLocsList.empty() )
		{
			SetStatus( FileDescription::Sent );
			if ( CheckDeliveredStatus() )
				SetStatus( FileDescription::Delivered );
		}	

		if ( GetStatus()==FileDescription::Sending && Type()==FT_Package )
		{
			if ( m_pFolderSync!=NULL ) m_pFolderSync->SyncServerFolder();
		}

		SaveInfoFile();

		CalculateDelayTimeout();
		
		KLTRACE3( KLCS_MODULENAME, L"File::ReplaceClientsList fileId - '%ls' ", m_fileId.c_str() );
		TraceLocsList( m_notSendLocsList );
	}

	//!\brief Проверяет список клиентов
	bool File::CheckUploadingClient( const std::wstring &remoteClientName,
			bool &uploadingClientsExeeded  )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		uploadingClientsExeeded = false;
		bool found = false;

		if ( !m_customFileMode && m_fileType!=KLFT::FT_Folder )
		{
			KLTRAP::Transport::LocationExList::iterator it = m_notSendLocsList.begin();
			for( ; it != m_notSendLocsList.end(); ++it )
			{	
				if ( it->remoteComponentName.find(remoteClientName)!=std::wstring::npos )
				{
					found = true;
					break;
				}
			}
			
			if ( found ) return found;
			
			it = m_sentLocsList.begin();
			for( ; it != m_sentLocsList.end(); ++it )
			{	
				if ( it->remoteComponentName.find(remoteClientName)!=std::wstring::npos )
				{
					found = true;
					break;
				}
			}
		}
		else found = true;

		if ( found )
		{
			int countUploadingNowClients = 0;
			bool needCheck = true;
			if ( m_uploadingClients.size()!=0 )
			{
				// ищем клиентов который длительное вермя не качали файл и снимаем
				// для них статус "качает сейчас" ( необходимо для ограничения кол-ва одноверменно
				// работающих клиентов )
				time_t cutTime;
				time( &cutTime );
				UploadingClientsList::iterator it = m_uploadingClients.begin();
				for( ;it != m_uploadingClients.end(); ++it )
				{
					if ( it->second.uploadingNow &&
						(cutTime - it->second.lastAccessTime)>c_defaultUploadingChunkDelay )
					{
						it->second.uploadingNow = false;
					}
					if ( it->second.uploadingNow ) 
					{
						countUploadingNowClients++;
						if ( it->first==remoteClientName ) needCheck = false;
					}
				}
			}
			
			if ( needCheck && countUploadingNowClients>=m_maxNumberUploadingClients )
			{
				uploadingClientsExeeded = true;
				return false;
			}


			UploadingClientsList::iterator fit = m_uploadingClients.find( remoteClientName );
			if ( fit != m_uploadingClients.end() )
			{
				time_t( &(fit->second.lastAccessTime) );
				fit->second.uploadingNow = true;
			}
			else
			{
				UploadingClientDesc clDesc;
				clDesc.clientName = remoteClientName;
				clDesc.uploadingNow = true;
				time_t( &clDesc.lastAccessTime );
				
				m_uploadingClients[remoteClientName] = clDesc;
			}
		}

		return found;
	}
	
	//!\brief Копирует готовый файл по указанному пути
	void File::SaveFileAs( const std::wstring &filePath, bool overwrite )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( GetStatus()==FileDescription::Uploading ||
			GetStatus()==FileDescription::MulticastUploading )
		{
			KLFT_THROW_ERROR1( ERR_NOT_UPLOADED, m_fileId.c_str() );
		}

		if ( m_fileType==KLFT::FT_Package || m_fileType==KLFT::FT_Folder )
		{
			if ( m_pFolderSync==NULL )
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

			m_pFolderSync->SaveFolderAs( filePath, overwrite );

			return;
		}

		if ( !m_isDirectory )
		{		
			std::wstring wstrDir, wstrName, wstrExt;
			KLSTD_SplitPath( m_fileName, wstrDir, wstrName, wstrExt );
			wstrDir.clear();// = L"";
			std::wstring fileName;
			KLSTD_MakePath( wstrDir, wstrName, wstrExt, fileName );
		
			KLSTD_CopyFile( m_copyFileName.c_str(), 
				std::wstring( filePath + L"\\" + fileName).c_str(), false );
		}
		else
		{
			KLSTD_CreatePath( filePath.c_str() );
			std::wstring wstrDir, wstrName, wstrExt;
			KLSTD_SplitPath( m_copyFileName, wstrDir, wstrName, wstrExt );			
			std::wstring fileName;
			KLSTD_MakePath( L"", wstrName, wstrExt, fileName );
			ExtractArchiveFile( filePath + L"\\", wstrDir, fileName, m_fileId );
		}
	}
	
	//!\brief Удаляет всю информацию о данном файле
	void File::DeleteWorkingFiles()
	{
		KLSTD_TRACE1( 3, L"DeleteWorkingFiles. File id - '%ls'\n",
							m_fileId.c_str() );

		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_file = NULL;
		
		if ( m_pFolderSync!=NULL ) 
		{			
			m_pFolderSync->DeleteWorkingFiles();
			m_pFolderSync = NULL;
		}
		
		if ( !m_customFileMode || Type()==KLFT::FT_CustomServerUploaded )
		{
			KLERR_BEGIN
				KLSTD_Unlink( m_copyFileName.c_str() );
			KLERR_ENDT( 3 )
				
			KLERR_BEGIN
				KLSTD_Unlink( m_infoFilePath.c_str() );
			KLERR_ENDT( 3 )
		}
	}

	//!\brief Функции блокировки доступа к файлу
	void File::Lock()
	{
		m_pCricSec->Enter();
	}

	void File::UnLock()
	{
		m_pCricSec->Leave();
	}

	//!\brief Опеределяет необходимо ли проводить закачку
	bool File::IsNeedToUpload()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_customFileMode ) return false;
		if ( m_currentStatus!=FileDescription::Uploading &&			
			m_currentStatus!=FileDescription::MulticastUploading ) return false;

		long multicastDeltaTimeout = 0;
		if ( m_currentStatus==FileDescription::MulticastUploading )
			multicastDeltaTimeout = c_UploadAfterMulticastTimeout;

		bool result = false;

		time_t currTime;
		time( &currTime );

		if ( m_lastOperationTime==0 )
		{			
			if ( (m_createTime + ( m_delay ) + ( m_uploadChunkTimeout / 1000 ) ) < 
				( currTime - multicastDeltaTimeout )) result = true;
		}
		else
		{
			if ( ( m_lastOperationTime + ( m_uploadChunkTimeout / 1000 ) )  <= 
				( currTime - multicastDeltaTimeout ) ) 
				result = true;
		}

//KLTRACE3( KLCS_MODULENAME, L"File::IsNeedToUpload fileId - '%ls' result - %d m_lastOperationTime - %d currTime - %d m_uploadChunkTimeout - %d\n",
//			m_fileId.c_str(), result, m_lastOperationTime, currTime, m_uploadChunkTimeout );

		return result;
	}

	//!\brief Опеределяет необходимо ли проводить рассылку сигнала о выгрузке
	bool File::IsNeedToBroadcast( bool checkInBroadcastFlag )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_customFileMode ) return false;
		if ( checkInBroadcastFlag && m_inBroadcastFlag ) return false;
		if ( m_currentStatus!=FileDescription::Sending ) return false;
		if ( m_notSendLocsList.size()==0 ) return false;

		KLTRAP::Transport::LocationExList::iterator it = m_notSendLocsList.begin();
		for( ; it != m_notSendLocsList.end(); ++it )
		{
			if ( (!(*it).location.empty() && !(*it).ports.empty()) || !(*it).localComponentName.empty() )
				break;
		}

		if ( it == m_notSendLocsList.end() ) return false;	// остались клиенты без адресов

		bool result = false;

		time_t currTime;
		time( &currTime );

		if ( m_lastOperationTime==0 )
		{
			result = true;
		}
		else
		{
			if ( ( m_lastOperationTime + ( m_uploadChunkTimeout / 1000 ) )  < currTime ) 
				result = true;
		}

		if ( result ) m_lastOperationTime = currTime;

		return result;
	}

	//!\brief Сохраняет текущее время как время последней операции с файлом
	void File::InBroadcastFlag( bool inBroadcast /*= false*/ )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_inBroadcastFlag = inBroadcast;
		if ( m_inBroadcastFlag ) m_BroadcastCount++;
	}

	//!\brief Возвращает кол-во произведенный Broadcast'ов для данного файла
	int File::GetBroadcastCount()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_BroadcastCount;
	}

	//!\brief Сбрасывает кол-во произведенный Broadcast'ов для данного файла
	void File::ResetBroadcastCount( int count )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_BroadcastCount = count;
	}
	
	//!\brief Увеличивает время задержки переда началом закачки файла
	void File::IncreaseStartUploadingTimeout( int addDelay )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_delay += addDelay;
	}

	//!\brief Изменяет timeout между операции получения порции файла
	void File::ChangeUploadDelayTimeout( bool increase, bool usingUAConnection )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( increase ) 
		{
			if ( m_uploadChunkTimeout<c_MaxChunkTimeout )
			{
				if ( usingUAConnection && m_calculatedUploadDelayTimeout>0 )				
					m_uploadChunkTimeout += (m_calculatedUploadDelayTimeout * 1000);
				else
					m_uploadChunkTimeout += c_NextChunkTimeoutAddDelta;
			
				m_uploadChunkFailedCount++;
			}
		}
		else
		{
			if ( (m_uploadChunkTimeout - c_NextChunkTimeoutAddDelta ) >=0 )
			{
				m_uploadChunkTimeout = 0;
			}
		}

		time(&m_lastOperationTime);
	}

	//!\brief Возвращает общее количество ошибок при закачки файла.
	long File::GetUploadChunkFailedCount()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_uploadChunkFailedCount;
	}

	//!\brief Функции работы с broadcastId
	KLTRAP::Transport::BroadcastId File::GetBroadcastId()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_broadcastId;
	}

	void File::SetBroadcastId( KLTRAP::Transport::BroadcastId broadcastId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_broadcastId = broadcastId;
	}

	//!\brief Файл из папки Updater
	bool File::IsCustomModeFile()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_customFileMode;
	}

	//!\brief Файл закачки Update Agent для долнейшего распространения
	bool File::IsForUpdateAgentFile()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return (m_updateAgentFileStatus!=UAFS_None);
	}

	File::UpdateAgentFileStatus File::GetUpdateAgentFileStatus()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return (File::UpdateAgentFileStatus)m_updateAgentFileStatus;
	}

	void File::ResetUseOnlyForUpdateAgentFlag()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_updateAgentFileStatus!=UAFS_None )
		{
			m_updateAgentFileStatus &= (~UAFS_UseOnlyForUpdateAgentFlag);
		}
	}

	bool File::GetUseOnlyForUpdateAgentFlag()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return ((m_updateAgentFileStatus & UAFS_UseOnlyForUpdateAgentFlag)!=0);
	}

	//!\brief Считывает текущую длинну файла
	void File::ReadFileSize()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_customFileMode )
		{
#ifdef _WIN32
			HANDLE hFile = INVALID_HANDLE_VALUE;
			hFile=CreateFile(
				KLSTD_W2CT2(m_copyFileName.c_str()),
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				0, 0 );
			if(hFile==INVALID_HANDLE_VALUE)
			{
				std::wstring wstrDir, wstrName, wstrExt;
				KLSTD_SplitPath( m_copyFileName, wstrDir, wstrName, wstrExt );
				KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, 
					std::wstring( wstrName + wstrExt ).c_str() );
			}

			DWORD dwSizeHigh=0;
			DWORD dwResult=GetFileSize(hFile, &dwSizeHigh);
			if(dwResult==-1) KLSTD_THROW_LASTERROR();
			m_fullFileSize = (unsigned long)(AVP_qword(dwResult) | (AVP_qword(dwSizeHigh) << 32));

			CloseHandle( hFile );
#endif
		}
	}

	//\brief Возвращает расчетное время задержки загрузки файла клиентов
	int File::GetCalculateDelayTimeout()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_calculatedUploadDelayTimeout;
	}

	//\brief Возвращает список агентов обновлений
	UpdateAgentLocationList File::GetUpdateAgentList()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( IsForUpdateAgentFile() ) return UpdateAgentLocationList();

		return m_updateAgentsLocs;
	}

	bool File::PartlySavedFile()
	{
		return m_partlySavedFile;
	}

	void File::PartlySavedFile( bool newVal )
	{
		if ( newVal==true && m_partlySavedFile==false )
		{
			SetStatus( FileDescription::MulticastUploading );
			m_partlySavedFile = true;
			SaveInfoFile();
		}
	}

	//\brief Возращает размер и позицию следоущего необходимого блока
	bool File::GetNextNeededChunk( unsigned long &nextChunkPos, unsigned long &nextChunkSize, 
			unsigned long buffsize )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( !m_partlySavedFile ) return false;

		nextChunkPos = 0;
		nextChunkSize = buffsize;
		if ( m_recievedChunks.empty() ) return true;

		if ( m_recievedChunks.size()==1 && m_recievedChunks.begin()->pos==0 &&
			m_recievedChunks.begin()->size==m_fullFileSize ) return false;

		RecievedChunkList::iterator  it = m_recievedChunks.begin();

		for( ; it != m_recievedChunks.end(); ++it )
		{
			unsigned long maxSize = 0;
			unsigned long pos = 0;
			bool found = false;
			RecievedChunk& rc = (*it);
			if ( it==m_recievedChunks.begin() )
			{
				if ( rc.pos!=0 )
				{
					maxSize = rc.pos;
					pos = 0;
					found = true;
				}
			}
			if ( !found )
			{
				pos = rc.pos + rc.size;
				unsigned long nextPos = m_fullFileSize;
				RecievedChunkList::iterator  nit = it;
				nit++;
				if ( nit!=m_recievedChunks.end() ) nextPos = nit->pos;
				maxSize = nextPos - pos;
				found = true;
			}

			if ( found )
			{
				if ( maxSize>buffsize ) nextChunkSize = buffsize;
				else nextChunkSize = maxSize;
				nextChunkPos = pos;
				m_currentUploadPos = pos;
				return true;
			}
		}

		return false;
	}

	//!\brief Тип файла
	KLFT::FileType File::Type()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_fileType;
	}

	//!\brief Возвращает информацию об файлах папки синхронизации
	void File::GetFullSyncInfo( KLFT::FolderSync::FolderFilesInfo &folderFileInfo )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileType!=KLFT::FT_Package && m_fileType!=KLFT::FT_Folder )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

		if ( m_pFolderSync==NULL )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

		m_pFolderSync->GetFullSyncInfo( folderFileInfo );
	}

	//!\brief ( полная синхронизация клиента )
	void File::ApplyPackageSyncArchive( const std::wstring& syncArchiveFileId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileType!=KLFT::FT_Package && m_fileType!=KLFT::FT_Folder )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

		if ( m_pFolderSync==NULL )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, m_fileId.c_str() );

		m_pFolderSync->Sync( syncArchiveFileId );
	}

	//!\brief Возвращает указатель на интерфейс FolderSync
	void File::GetFolderSync( KLSTD::CAutoPtr<KLFT::FolderSync> &pFolderSync )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		pFolderSync = m_pFolderSync;
	}
	
	//!\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
	bool File::IfExistsUnprocessedLastSyncChangedFiles()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileType!=KLFT::FT_Folder ) return false;

		if ( !m_lastSyncFilesInfo.FilesList.empty() ) return true;

		bool res = m_pFolderSync->IfExistsUnprocessedLastSyncChangedFiles();

		if ( res )
		{
			m_pFolderSync->GetUnprocessedLastSyncChangedFiles( 
				m_lastSyncFilesInfo, m_lastSyncTickCount );			
		}

		return res;
	}

	//!\brief Функции для работы со списков подпапок для синхронизации
	KLFT::FolderSync::SubfoldersList File::GetSubfoldersListForFolderSync()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return m_vecFoldersForSync;
	}

	bool File::AddSubfoldersForFolderSync( const std::wstring &requestedFilePath )
	{
		std::wstring wstrDir, wstrName, wstrExt;		
		KLSTD_SplitPath( requestedFilePath, wstrDir, wstrName, wstrExt );

		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		KLFT::FolderSync::SubfoldersList::iterator it = m_vecFoldersForSync.begin();
		bool found = false;

		for( ; it!=m_vecFoldersForSync.end() && !found ; ++it )
		{
			if ( wstrDir==(*it) ) found = true;
		}

		if ( !found ) 
		{
			KLTRACE3( KLCS_MODULENAME, L"File::AddSubfoldersForFolderSync fileId - '%ls' filePath - '%ls'\n",
				m_fileId.c_str(), requestedFilePath.c_str() );

			m_vecFoldersForSync.push_back( wstrDir );
			SaveInfoFile();
		}

		return (!found);
	}

	bool File::AppendSubfoldersForFolderSync( const KLFT::FolderSync::SubfoldersList &subfoldersList )
	{
		bool added = false;

		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		KLFT::FolderSync::SubfoldersList::const_iterator ait = subfoldersList.begin();

		for( ; ait!=subfoldersList.end(); ++ait )
		{
			KLFT::FolderSync::SubfoldersList::iterator it = m_vecFoldersForSync.begin();		

			for( ; it!=m_vecFoldersForSync.end(); ++it )
			{
				if ( (*it)==(*ait) ) break;
			}

			if ( it!=m_vecFoldersForSync.end() )
			{
				m_vecFoldersForSync.push_back( (*ait) );
				added = true;
			}
		}
		
		return added;

	}

	//!\brief Возвращает признак наличия необработанных с последней синхронизацией файлов
	bool File::GetUnprocessedLastSyncChangedFiles( std::vector<std::wstring>& folderFiles )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileType!=KLFT::FT_Folder ) return false;

		if ( m_lastSyncFilesInfo.FilesList.empty() ) return false;

		KLFT::FolderSync::FileInfoList::iterator it = m_lastSyncFilesInfo.FilesList.begin();
		for( ;it != m_lastSyncFilesInfo.FilesList.end(); ++it )
		{
			folderFiles.push_back(it->FullFileName);
		}

		return true;
	}

	//!\brief Сбрасывает флаг обработки для указанного файла
	void File::ProcessedLastSyncChangedFiles( const std::wstring& folderFilePath )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileType!=KLFT::FT_Folder ) return;

		KLFT::FolderSync::FileInfoList::iterator it = m_lastSyncFilesInfo.FilesList.begin();
		for( ;it != m_lastSyncFilesInfo.FilesList.end(); ++it )
		{
			if ( it->FullFileName==folderFilePath )
			{
				m_pFolderSync->ProcessedLastSyncChangedFiles( it->FileNameHash,
					m_lastSyncTickCount );
				m_lastSyncFilesInfo.FilesList.erase( it );
				break;
			}
		}
	}

	//!\brief Возвращает процент процесса архивирования
	int File::GetArchivatePercent()
	{
		return (int)((double)((double)m_archivedFilesCount/(double)m_totalFilesInArchive) * 100.0);
	}
	
	//!\brief Сохраняет info файл
	void File::SaveInfoFile()
	{
		if ( m_customFileMode ) return;

		KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
		KLPAR_CreateParams( &parSerFile );

		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsFileName, StringValue, m_fileName.c_str() );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsFileId, StringValue, m_fileId.c_str() );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsStatus, IntValue, m_currentStatus );		
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsDescription, StringValue, m_description.c_str() );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsFullFileSize, IntValue, m_fullFileSize );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsMulticastRecvSize, IntValue, m_multicastRecvSize );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsUploadPos, IntValue, m_currentUploadPos );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsCreateTime, DateTimeValue, m_createTime );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsIsDir, BoolValue, m_isDirectory );
		long tmpVal = (long)m_fileType;
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsFileType, IntValue, tmpVal );
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsMaxClients, IntValue, m_maxNumberUploadingClients );	
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsClientName, StringValue, m_clientName.c_str() );	
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsLocalName, StringValue, m_localName.c_str() );			
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsUpdateAgentStatus, IntValue, m_updateAgentFileStatus );

		AddLocationListExInParams( parSerFile, c_serializeParamsNotSendLocsList, 
				m_notSendLocsList );

		AddLocationListExInParams( parSerFile, c_serializeParamsSentLocsList, 
				m_sentLocsList );

		AddLocationListExInParams( parSerFile, c_serializeParamsUploadLocsList, 
				m_uploadLocsList );

		AddUAListInParams( parSerFile );

		AddChunksListInParams( parSerFile );

		AddSubfoldersListInParams( parSerFile );

		KLPAR_SerializeToFileName2( m_infoFilePath, KLPAR_SF_BINARY, parSerFile );
	}

	//!\brief Востанавливает объект из info файла
	void File::RestoreFromInfoFile()
	{
		KLSTD::CAutoPtr<KLPAR::Params> parSerFile;

		KLPAR_DeserializeFromFileName( m_infoFilePath, &parSerFile );

		GET_PARAMS_VALUE( parSerFile, c_serializeParamsFileName, StringValue, STRING_T, m_fileName );
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsFileId, StringValue, STRING_T, m_fileId );
		long status;
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsStatus, IntValue, INT_T, status );
		m_currentStatus = (KLFT::FileDescription::FileStatus)status;
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsDescription, StringValue, STRING_T, m_description );
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsFullFileSize, IntValue, INT_T, m_fullFileSize );
		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsMulticastRecvSize, IntValue, INT_T, m_multicastRecvSize );		
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsUploadPos, IntValue, INT_T, m_currentUploadPos );
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsCreateTime, DateTimeValue, DATE_TIME_T, m_createTime );
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsIsDir, BoolValue, BOOL_T, m_isDirectory );
		long tmpVal = FT_Simple;		
		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsFileType, IntValue, INT_T, tmpVal );		
		m_fileType = (KLFT::FileType)tmpVal;
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsMaxClients, IntValue, INT_T, m_maxNumberUploadingClients );
		GET_PARAMS_VALUE( parSerFile, c_serializeParamsClientName, StringValue, STRING_T, m_clientName );			
		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsLocalName, StringValue, STRING_T, m_localName );

		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsUpdateAgentStatus, IntValue, INT_T, status );
		m_updateAgentFileStatus = (UpdateAgentFileStatus)status;
		
		GetLocationListExFromParams( parSerFile, c_serializeParamsNotSendLocsList, 
				m_notSendLocsList );

		GetLocationListExFromParams( parSerFile, c_serializeParamsSentLocsList, 
				m_sentLocsList );

		GetLocationListExFromParams( parSerFile, c_serializeParamsUploadLocsList, 
				m_uploadLocsList );

		GetUAListFromParams( parSerFile );
		GetChunkListFromParams( parSerFile );
		GetSubfoldersListInParams( parSerFile );

		KLTRACE3( KLCS_MODULENAME, L"File::RestoreFromInfoFile fileId - '%ls' description - '%ls' fileName - '%ls' Is directory - %d fullFileSize - %d status - %d\n",
			m_fileId.c_str(), m_description.c_str(), m_fileName.c_str(), m_isDirectory, m_fullFileSize, m_currentStatus );
	}

	//!\brief Вспомогательные функции для сериализации locationEx
	void File::AddLocationListExInParams( KLPAR::Params *parSerFile, 
		const wchar_t *fieldName, KLTRAP::Transport::LocationExList &locsList )
	{
		if ( locsList.empty() ) return;

		KLSTD::CAutoPtr<KLPAR::Params> parLocList;
		KLPAR_CreateParams( &parLocList );

		KLTRAP::Transport::LocationExList::iterator it = locsList.begin();
		int counter = 0;
		for(; it != locsList.end(); ++it, counter++ )
		{
			std::wostringstream ws;
			ws << counter;

			AddLocationExInParams( parLocList, ws.str().c_str(), *(it) );
		}

		ADD_PARAMS_VALUE( parSerFile, fieldName, ParamsValue, parLocList );
	}

	void File::AddLocationExInParams( KLPAR::Params *parSerFile, 
		const wchar_t *fieldName, KLTRAP::Transport::LocationEx &loc )
	{
		KLSTD::CAutoPtr<KLPAR::Params> parLoc;
		KLPAR_CreateParams( &parLoc );

		ADD_PARAMS_VALUE( parLoc, c_serializeParamsLocExRemoteName, StringValue, loc.remoteComponentName.c_str() );
		ADD_PARAMS_VALUE( parLoc, c_serializeParamsLocExLocalComponentName, StringValue, loc.localComponentName.c_str() );		
		ADD_PARAMS_VALUE( parLoc, c_serializeParamsLocExLocation, StringValue, loc.location.c_str() );

		KLSTD::CAutoPtr<KLPAR::Params> parPorts;
		KLPAR_CreateParams( &parPorts );
		std::vector<short>::iterator it = loc.ports.begin();
		int counter = 0;
		for( ; it != loc.ports.end(); ++it, counter++ )
		{
			std::wostringstream ws;
			ws << counter;
			ADD_PARAMS_VALUE( parPorts, ws.str().c_str(), IntValue, (*it) );
		}

		ADD_PARAMS_VALUE( parLoc, c_serializeParamsLocExPorts, ParamsValue, parPorts );
		ADD_PARAMS_VALUE( parLoc, c_serializeParamsLocExUnicastFlag, IntValue, loc.unicastConnectFlag );
		ADD_PARAMS_VALUE( parLoc, c_serializeParamsLocExUnicastSSLFlag, BoolValue, loc.useSSLUnicast );

		ADD_PARAMS_VALUE( parSerFile, fieldName, ParamsValue, parLoc );		
	}

	void File::AddUAListInParams( KLPAR::Params *parSerFile )
	{
		KLSTD::CAutoPtr<KLPAR::Params> parUAList;
		KLPAR_CreateParams( &parUAList );

		UpdateAgentLocationList::iterator it = m_updateAgentsLocs.begin();
		int counter = 0;
		for( ; it != m_updateAgentsLocs.end(); ++it, counter++ )
		{
			KLSTD::CAutoPtr<KLPAR::Params> parUALoc;
			KLPAR_CreateParams( &parUALoc );

			ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUAConnName, StringValue, it->strConnectionName.c_str() );
			ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUALocation, StringValue, it->strLocation.c_str() );
			ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUASSLLocation, StringValue, it->strSSLLocation.c_str() );
			ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUAMulticastAddr, StringValue, it->strMulticastAddr.c_str() );
			
			std::wostringstream ws;
			ws << counter;

			ADD_PARAMS_VALUE( parUAList, ws.str().c_str(), ParamsValue, parUALoc );
		}
		
		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsUpdateAgentsList, ParamsValue, parUAList );
	}

	void File::AddChunksListInParams( KLPAR::Params *parSerFile )
	{
		KLSTD::CAutoPtr<KLPAR::Params> parChunkList;
		KLPAR_CreateParams( &parChunkList );

		RecievedChunkList::iterator it = m_recievedChunks.begin();
		int counter = 0;
		for( ; it != m_recievedChunks.end(); ++it, counter++ )
		{
			KLSTD::CAutoPtr<KLPAR::Params> parChunk;
			KLPAR_CreateParams( &parChunk );

			ADD_PARAMS_VALUE( parChunk, c_serializeParamsChunkPos, IntValue, it->pos );
			ADD_PARAMS_VALUE( parChunk, c_serializeParamsChunkSize, IntValue, it->size );
						
			std::wostringstream ws;
			ws << counter;

			ADD_PARAMS_VALUE( parChunkList, ws.str().c_str(), ParamsValue, parChunk );
		}

		ADD_PARAMS_VALUE( parSerFile, c_serializeParamsChunkList, ParamsValue, parChunkList );
	}

	void File::AddSubfoldersListInParams( KLPAR::Params *parSerFile )
	{
		KLSTD::CAutoPtr<KLPAR::Params> parSubfoldersList;
		KLPAR_CreateParams( &parSubfoldersList );

		KLFT::FolderSync::SubfoldersList::iterator it = m_vecFoldersForSync.begin();
		int counter = 0;
		for( ; it != m_vecFoldersForSync.end(); ++it, counter++ )
		{
			std::wostringstream ws;
			ws << counter;

			ADD_PARAMS_VALUE( parSubfoldersList, ws.str().c_str(), StringValue, (*it).c_str() );
		}
		
		if ( counter ) 
			ADD_PARAMS_VALUE( parSerFile, c_serializeParamsSyncFolderSubfoldersList, ParamsValue, parSubfoldersList );
	}
	
	//!\brief Вспомогательные функции для десериализации locationEx
	void File::GetLocationListExFromParams( KLPAR::Params *parSerFile, 
		const wchar_t *fieldName, KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::CAutoPtr<KLPAR::Params> parLocList;

		GET_PARAMS_VALUE_NO_THROW( parSerFile, fieldName, ParamsValue, PARAMS_T, parLocList );
		if ( parLocList==NULL ) return;

		KLPAR::ParamsNames parNames;
		parLocList->GetNames( parNames );
		KLPAR::ParamsNames::iterator it = parNames.begin();
		for( ; it != parNames.end(); ++it )
		{
			KLTRAP::Transport::LocationEx loc;
			GetLocationListExFromParams( parLocList, (*it).c_str(), loc );
			locsList.push_back( loc );
		}
	}

	void File::GetLocationListExFromParams( KLPAR::Params *parSerFile, 
		const wchar_t *fieldName, KLTRAP::Transport::LocationEx &loc )
	{
		KLSTD::CAutoPtr<KLPAR::Params> parLoc;

		GET_PARAMS_VALUE_NO_THROW( parSerFile, fieldName, ParamsValue, PARAMS_T, parLoc );
		if ( parLoc==NULL ) return;

		GET_PARAMS_VALUE( parLoc, c_serializeParamsLocExRemoteName, StringValue, STRING_T, loc.remoteComponentName );
		GET_PARAMS_VALUE( parLoc, c_serializeParamsLocExLocalComponentName, StringValue, STRING_T, loc.localComponentName );
		GET_PARAMS_VALUE( parLoc, c_serializeParamsLocExLocation, StringValue, STRING_T, loc.location );

		KLSTD::CAutoPtr<KLPAR::Params> parPorts;
		GET_PARAMS_VALUE( parLoc, c_serializeParamsLocExPorts, ParamsValue, PARAMS_T, parPorts );		
		KLPAR::ParamsNames parNames;
		parPorts->GetNames( parNames );
		KLPAR::ParamsNames::iterator it = parNames.begin();		
		for( ; it != parNames.end(); ++it )
		{
			int port;
			GET_PARAMS_VALUE( parPorts, (*it).c_str(), IntValue, INT_T, port );			
			loc.ports.push_back( port );
		}

		int uFlag = 0;
		GET_PARAMS_VALUE( parLoc, c_serializeParamsLocExUnicastFlag, IntValue, INT_T, uFlag );
		if ( parLoc->DoesExist(c_serializeParamsLocExUnicastSSLFlag) )
		{
			GET_PARAMS_VALUE( parLoc, c_serializeParamsLocExUnicastSSLFlag, BoolValue, BOOL_T, loc.useSSLUnicast );
		}
		else
			loc.useSSLUnicast = true;

		loc.unicastConnectFlag = (uFlag!=0);
	}

	void File::GetUAListFromParams( KLPAR::Params *parSerFile )
	{
		m_updateAgentsLocs.clear();

		KLSTD::CAutoPtr<KLPAR::Params> parUAList;
		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsUpdateAgentsList, ParamsValue, PARAMS_T, parUAList );
		if ( parUAList==NULL ) return;

		KLPAR::ParamsNames parNames;
		parUAList->GetNames( parNames );
		KLPAR::ParamsNames::iterator it = parNames.begin();		
		for( ; it != parNames.end(); ++it )
		{
			KLSTD::CAutoPtr<KLPAR::Params> parUALoc;
			GET_PARAMS_VALUE( parUAList, (*it).c_str(), ParamsValue, PARAMS_T, parUALoc );			

			UpdateAgentLocation uaLoc;
			GET_PARAMS_VALUE( parUALoc, c_serializeParamsUAConnName, StringValue, STRING_T, uaLoc.strConnectionName );
			GET_PARAMS_VALUE( parUALoc, c_serializeParamsUALocation, StringValue, STRING_T, uaLoc.strLocation );
			GET_PARAMS_VALUE( parUALoc, c_serializeParamsUASSLLocation, StringValue, STRING_T, uaLoc.strSSLLocation );
			GET_PARAMS_VALUE( parUALoc, c_serializeParamsUAMulticastAddr, StringValue, STRING_T, uaLoc.strMulticastAddr );

			m_updateAgentsLocs.push_back( uaLoc );
		}		
	}

	void File::GetChunkListFromParams( KLPAR::Params *parSerFile )
	{
		m_recievedChunks.clear();
//		if ( !m_updateAgentsLocs.empty() && m_updateAgentFileStatus==File::UAFS_None ) m_partlySavedFile = true;
//		else m_partlySavedFile = false;

		KLSTD::CAutoPtr<KLPAR::Params> parChunkList;
		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsChunkList, ParamsValue, PARAMS_T, parChunkList );
		if ( parChunkList==NULL ) return;

		KLPAR::ParamsNames parNames;
		parChunkList->GetNames( parNames );
		KLPAR::ParamsNames::iterator it = parNames.begin();		
		for( ; it != parNames.end(); ++it )
		{
			KLSTD::CAutoPtr<KLPAR::Params> parChunk;
			GET_PARAMS_VALUE( parChunkList, (*it).c_str(), ParamsValue, PARAMS_T, parChunk );

			RecievedChunk rChunk;
			GET_PARAMS_VALUE( parChunk, c_serializeParamsChunkPos, IntValue, INT_T, rChunk.pos );
			GET_PARAMS_VALUE( parChunk, c_serializeParamsChunkSize, IntValue, INT_T, rChunk.size );

			m_recievedChunks.push_back( rChunk );
//			m_partlySavedFile = true;
		}		
	}

	void File::GetSubfoldersListInParams( KLPAR::Params *parSerFile )
	{
		m_vecFoldersForSync.clear();
		
		KLSTD::CAutoPtr<KLPAR::Params> parSubfolderList;
		GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeParamsSyncFolderSubfoldersList, ParamsValue, PARAMS_T, parSubfolderList );
		if ( parSubfolderList==NULL ) return;

		KLPAR::ParamsNames parNames;
		parSubfolderList->GetNames( parNames );
		KLPAR::ParamsNames::iterator it = parNames.begin();		
		for( ; it != parNames.end(); ++it )
		{
			std::wstring subfolderName;
			GET_PARAMS_VALUE( parSubfolderList, (*it).c_str(), StringValue, STRING_T, subfolderName );

			m_vecFoldersForSync.push_back( subfolderName );
		}		
	}

	//!\brief Проверяет что все клиенты скачали файл
	bool File::CheckDeliveredStatus()
	{		
		KLTRAP::Transport::LocationExList::const_iterator clit = m_sentLocsList.begin();
		for( ; clit != m_sentLocsList.end(); ++clit )
		{
			bool found = false;
			KLTRAP::Transport::LocationExList::iterator it = m_uploadLocsList.begin();
			for( ; it != m_uploadLocsList.end() && !found ; ++it )
			{	
				if ( it->remoteComponentName==clit->remoteComponentName )
					found = true;
			}
			if ( !found ) return false;
		}

		return true;
	}

	//!\brief Функция расчитывает время задержки перед закачкой файла на клиенте
	void File::CalculateDelayTimeout()
	{
		int exeededCount = 1;
		if ( m_currentStatus != FileDescription::Uploading )
		{
			// для серверных файлов
			int totalWorkingClients = m_sentLocsList.size() + m_notSendLocsList.size();
			int exeededCount = (int)((double)( totalWorkingClients + m_maxNumberUploadingClients - 1 ) / (double)m_maxNumberUploadingClients); 
			if ( exeededCount>=1 ) exeededCount--;
			else 
			{
				m_calculatedUploadDelayTimeout = 0;
				return;
			}
		}
		
		// берем скорость пересылки равную 1 Мб\сек 
		int mbSpeed = 1;
		int mbFileSize = (int)((double)m_fullFileSize / (double)(1024 * 1024));
		m_calculatedUploadDelayTimeout = ( exeededCount * mbFileSize ) / mbSpeed;
	}

	//!\brief Возвращает описатель ранее сохраненного блока для конкатинации
	File::RecievedChunkList::iterator File::GetCloseChunksListItem( 
		unsigned long pos, unsigned long size,
		CloseChunkType &chunkType )
	{
		chunkType = File::CCT_None;
		RecievedChunkList::iterator  it = m_recievedChunks.begin();

		for( ; it != m_recievedChunks.end(); ++it )
		{
			RecievedChunk &rc = (*it);
			if ( rc.pos==pos ) 
			{ 
				chunkType = CCT_Equal; break; 
			}
			if ( rc.pos<pos && (rc.pos+rc.size)>=(pos) ) 
			{ 
				chunkType = CCT_Right; break; 
			}
			if ( rc.pos>pos && rc.pos<=((pos + size)) ) 
			{ 
				chunkType = CCT_Left; break; 
			}
		}

		return it;
	}
	
	//!\brief Склеивает рядом расположенный chunk'и
	void File::ClueRecievedChunksList()
	{
		RecievedChunkList::iterator  it = m_recievedChunks.begin();

		while( it != m_recievedChunks.end() )
		{
			RecievedChunk &rc = (*it);
			RecievedChunkList::iterator nextit = it;
			nextit++;
			bool erased = false;
			if ( nextit!=m_recievedChunks.end() )
			{
				if ( rc.pos+rc.size==nextit->pos )
				{
					rc.size += nextit->size;
					m_recievedChunks.erase(nextit);
					erased = true;
				}
			}

			if ( !erased ) ++it;
		}
	}

} // end namespace KLFT


const wchar_t c_FolderArchExtName[] = L"klfolderextnamekl";

bool KLFT_ArchiveFolder( const std::wstring &folderPathFrom, const std::wstring &archiveFileName )
{
	std::wstring wstrDir, wstrName, wstrExt;		
	KLSTD_SplitPath( archiveFileName, wstrDir, wstrName, wstrExt );
	
	if ( wstrDir.empty() ) wstrDir = L".";
	if ( !wstrExt.empty() ) wstrName = wstrName + L"." + wstrExt;
	
#ifdef _WIN32
	return KLFT::AddFolderInArchive( NULL, folderPathFrom, wstrName, wstrDir, c_FolderArchExtName );
#else
	return false;
#endif // _WIN32		
}

bool KLFT_ExtractArchiveFile( const std::wstring &folderPathTo, const std::wstring &archiveFileName )
{
	std::wstring wstrDir, wstrName, wstrExt;		
	std::wstring fTo = folderPathTo;
	KLSTD_SplitPath( archiveFileName, wstrDir, wstrName, wstrExt );
	
	if ( wstrDir.empty() ) wstrDir = L".";
	if ( !wstrExt.empty() ) wstrName = wstrName + L"." + wstrExt;
	if ( !fTo.empty() )
	{
		if ( fTo[fTo.length()-1]!=L'/' ) 
		{
			fTo.append( L"/" );
		}
	}
	
		
	return KLFT::ExtractArchiveFile( fTo, wstrDir, wstrName, c_FolderArchExtName );
}
