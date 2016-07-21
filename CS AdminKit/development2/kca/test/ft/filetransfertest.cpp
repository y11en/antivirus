/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file FileTransferTest.cpp
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Тест библиотеки FT.
 *
 */

#ifdef WIN32
#	include <stdio.h>
#	include <conio.h>
#	include <windows.h>
#endif

#include <std/base/kldefs.h>
#include <std/trc/trace.h>
#include <std/conv/klconv.h>
#include <std/base/klbase.h>
#include <std/par/value.h>
#include <std/io/klio.h>

#include <kca/init/init_kca.h>

#include <transport/ev/eventsource.h>

#include <kca/ft/filetransfer.h>
#include <kca/ft/foldersync.h>

#include <common/measurer.h>

#include "filetransfertest.h"

const wchar_t c_WorkingFolder[] = L"$FileTransferTmpDir";
const wchar_t c_WorkingFolderClient[] = L"$FileTransferTmpDir.client";

const wchar_t c_LocalConnectionName[]	= L"TestFileTransfer;1.0;local;1";
const wchar_t c_RemoteConnectionName[]	= L"TestFileTransfer;1.0;remote;1";
const wchar_t c_ClientLocation[]			= L"http://127.0.0.1:31";
const int	  c_ClientLocationPort			= 31;
const wchar_t c_ServerLocation[]			= L"http://127.0.0.1:32";

#define KLCS_MODULENAME L"KLFTTEST"

#ifdef WIN32
void FTDeleteFolderTree(const wchar_t * pszFolder)
{
	if (pszFolder)
	{
		WIN32_FIND_DATA FindFileData;

#ifdef UNICODE
		std::wstring sFolder(pszFolder);
		HANDLE hFind = ::FindFirstFile(std::wstring(sFolder + L"\\*.*").c_str(), &FindFileData);
		if (hFind == (void *)0xFFFFFFFF) return;
		while(true)
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				DeleteFile(std::wstring(sFolder + L"\\" + std::wstring(FindFileData.cFileName)).c_str());
			else
			{
				if ( std::wstring(FindFileData.cFileName)!=L"." && std::wstring(FindFileData.cFileName)!=L".." )
				{
					FTDeleteFolderTree(std::wstring(sFolder + L"\\" + std::wstring(FindFileData.cFileName)).c_str());
					KLSTD_RemoveDirectory( std::wstring(sFolder + L"\\" + std::wstring(FindFileData.cFileName)).c_str(), false);
				}
			}
#else
		KLSTD_USES_CONVERSION;
		std::string sFolder( KLSTD_W2A(pszFolder) );
		HANDLE hFind = ::FindFirstFile(std::string(sFolder + "\\*.*").c_str(), &FindFileData);
		while(true)
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				DeleteFile(std::string(sFolder + "\\" + std::string(FindFileData.cFileName)).c_str());			
#endif
			if (!::FindNextFile(hFind, &FindFileData)) break;
		}

		::FindClose(hFind);
	}
}
#elif defined(__UNIX__)
	// Unix
	void FTDeleteFolderTree(LPCTSTR pszFolder){};
#else // N_PLAT_NLM
	void FTDeleteFolderTree(wchar_t *pszFolder){};
#endif


void CFileTransferServer::Initialize(KLTST2::TestData2* pTestData)
{	
	m_pTestData = NULL;
#ifdef _WIN32
	KLSTD_USES_CONVERSION;

	std::wstring cmdLine = KLSTD_T2W(::GetCommandLine());
	// данный тест запускается только из теста UploadTest
	if ( cmdLine.find(L"-s")==std::wstring::npos ) return;
#endif
	m_pTestData = pTestData;

	KLCSKCA_Initialize();
	CALL_MODULE_INIT(KLFT);
}

KLSTD_NOTHROW void CFileTransferServer::Deinitialize() throw()
{
	if ( m_pTestData == NULL ) return;
	CALL_MODULE_DEINIT(KLFT);
	KLCSKCA_Deinitialize();
}


long CFileTransferServer::Run()
{	
	if ( m_pTestData == NULL ) return 0;

	FTDeleteFolderTree( c_WorkingFolder );
	KLSTD_RemoveDirectory( c_WorkingFolder, false);
	KLSTD_CreateDirectory( c_WorkingFolder, true );

	int usedPort = 0;
	KLTR_GetTransport()->AddListenLocation( c_RemoteConnectionName, 
		c_ServerLocation, usedPort );

	KLFT_GetFileTransfer()->InitServer( c_WorkingFolder,
		c_RemoteConnectionName );

	m_pTestData->WaitForStop();	// ждем сигнал на запуск теста

	KLTRAP::Transport::LocationExList locsList;
	KLTRAP::Transport::LocationEx loc;
	loc.remoteComponentName = c_LocalConnectionName;
	loc.location = L"http://127.0.0.1";
	loc.ports.push_back( c_ClientLocationPort );	
	locsList.push_back( loc );

	std::wstring fileId;
	KLFT_GetFileTransfer()->SendFile( L"klcskca.dll", locsList, 
		L"Test file for transfering", fileId );

	m_pTestData->WaitForStop();

	KLTR_GetTransport()->DeleteListenLocation( c_RemoteConnectionName,
		c_ServerLocation, usedPort );

	return 0;
}

void UploadedEventCallback(
        const KLPRCI::ComponentId & subscriber, 
        const KLPRCI::ComponentId & publisher,        
        const KLEV::Event *event,
		void *context )
{
	KLSTD::Semaphore *pSem = (KLSTD::Semaphore *)context;
	if ( pSem!=NULL ) pSem->Post();
}


void CUploadTest::Initialize(KLTST2::TestData2* pTestData)
{	
	m_pTestData = pTestData;

	KLCSKCA_Initialize();
	CALL_MODULE_INIT(KLFT);

	const wchar_t *c_FTServerStartCommand = L"LOGFILE \"log_ft_server_test_dll.txt\"; TRACEFILE \"trace_ft_server_test_dll.txt\"  level 5; RUN TEST \"FT_FileTransferServer\" WITH SYNC \"FileTransferServerTestStoper\" FROM \"kcatest.dll\";";

	m_pTestData->InstantiateTester( c_FTServerStartCommand );	

	KLSTD_Sleep( 3000 );	// wait server
}

KLSTD_NOTHROW void CUploadTest::Deinitialize() throw()
{
	CALL_MODULE_DEINIT(KLFT);
	KLCSKCA_Deinitialize();
}

class ConnInvoker :
public KLSTD::KLBaseImpl<KLFT::FileTransfer::TransferConnectionInvoker>
{
public:
	virtual void CreateConnection( const std::wstring &localComponentName,
				const std::wstring &remoteComponentName )
	{
		KLTR_GetTransport()->AddClientConnection( localComponentName.c_str(),
			remoteComponentName.c_str(), c_ServerLocation );
	}
};


long CUploadTest::Run()
{
		FTDeleteFolderTree( c_WorkingFolderClient );
	KLSTD_RemoveDirectory( c_WorkingFolderClient, false);
	KLSTD_CreateDirectory( c_WorkingFolderClient, true );

	int usedPort = 0;
	KLTR_GetTransport()->AddListenLocation( c_LocalConnectionName, 
		c_ClientLocation, usedPort );

	KLFT_GetFileTransfer()->InitClient( c_WorkingFolderClient,
		c_RemoteConnectionName, c_LocalConnectionName,  L"",
		NULL,
		new ConnInvoker );
	
	KLSTD::CAutoPtr<KLSTD::Semaphore> pSem;

	KLSTD_CreateSemaphore( &pSem, 0 );

	// подписываемся на событие об окончании загрузки файла
	KLEV_GetEventSource()->Subscribe( KLPRCI::ComponentId(),
		KLPRCI::ComponentId(), KLFT::EVENT_TYPE_UPLOADED, NULL,
		UploadedEventCallback, (void *)&(*pSem) );

	// запускаем FileTransferServer тест
	m_pTestData->SendStop(L"FileTransferServerTestStoper");

	pSem->Wait();

	// останавливаем FileTransferServer тест
	m_pTestData->SendStop(L"FileTransferServerTestStoper");
	m_pTestData->ReleaseSyncObject(L"FileTransferServerTestStoper");

	KLTR_GetTransport()->DeleteListenLocation( c_LocalConnectionName,
		c_ClientLocation, usedPort );
	
	return 0;
}


void CFileTransferFolderSync::Initialize(KLTST2::TestData2* pTestData)
{	
	m_pTestData = pTestData;

	KLCSKCA_Initialize();
	CALL_MODULE_INIT(KLFT);
}

KLSTD_NOTHROW void CFileTransferFolderSync::Deinitialize() throw()
{
	if ( m_pTestData == NULL ) return;
	CALL_MODULE_DEINIT(KLFT);
	KLCSKCA_Deinitialize();
}


long CFileTransferFolderSync::Run()
{	
	/*KLSTD_DeletePath( c_WorkingFolder );
	KLSTD_CreateDirectory( c_WorkingFolder, true );

	KLSTD_DeletePath( c_WorkingFolderClient );
	KLSTD_CreateDirectory( c_WorkingFolderClient, true );

	KLSTD_Unlink( L"O:/Packages/NetAgent 1.0/klcskca.pdb" );
	KLSTD_Unlink( L"O:/Packages/NetAgent 1.0/keys/klcskca.ilk" );

	KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
	KLFT_CreateFolderSync( &pFolderSync );

	std::wstring fileId = KLSTD_CreateGUIDString();
	pFolderSync->InitServer( fileId, c_WorkingFolder, L"O:/Packages/NetAgent 1.0" );
	pFolderSync->SyncServerFolder();

	KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSyncClient;
	KLFT_CreateFolderSync( &pFolderSyncClient );

	std::wstring fileIdClient = KLSTD_CreateGUIDString();
	pFolderSyncClient->InitClient( fileIdClient, c_WorkingFolderClient );

	KLFT::FolderSync::FolderFilesInfo clientFolderFilesInfo;
	pFolderSyncClient->GetFullSyncInfo( clientFolderFilesInfo );

	std::wstring archiveFileId = KLSTD_CreateGUIDString();
	KLFT::FolderSync::SubfoldersList subfoldersList;
	pFolderSync->MakeFullSyncContent( clientFolderFilesInfo, archiveFileId, subfoldersList );

	KLSTD_CopyFile( (std::wstring(c_WorkingFolder) + L"/" + archiveFileId).c_str(), 
		(std::wstring(c_WorkingFolderClient) + L"/" + archiveFileId).c_str(), false );

	pFolderSyncClient->Sync( archiveFileId );
	
	pFolderSyncClient->GetFullSyncInfo( clientFolderFilesInfo );

	archiveFileId = KLSTD_CreateGUIDString();

	KLSTD_CopyFile( L"O:/CS AdminKit/development2/bin/dlld/klcskca.pdb",
		L"O:/Packages/NetAgent 1.0/klcskca.pdb", false );

	KLSTD_CopyFile( L"O:/CS AdminKit/development2/bin/dlld/klcskca.ilk",
		L"O:/Packages/NetAgent 1.0/keys/klcskca.ilk", false );

	pFolderSync->SyncServerFolder();
	pFolderSync->MakeFullSyncContent( clientFolderFilesInfo, archiveFileId, subfoldersList );

	KLSTD_CopyFile( (std::wstring(c_WorkingFolder) + L"/" + archiveFileId).c_str(), 
		(std::wstring(c_WorkingFolderClient) + L"/" + archiveFileId).c_str(), false );

	pFolderSyncClient->Sync( archiveFileId );*/





	KLSTD_DeletePath( c_WorkingFolder );
	KLSTD_CreateDirectory( c_WorkingFolder, true );

	KLSTD_DeletePath( c_WorkingFolderClient );
	KLSTD_CreateDirectory( c_WorkingFolderClient, true );

	KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
	KLFT_CreateFolderSync( &pFolderSync );

	std::wstring fileId = KLSTD_CreateGUIDString();
	pFolderSync->InitServer( fileId, std::wstring(L"..\\dlld\\") + c_WorkingFolder, L"O:/Test/#ForSyncFolderTest/" );
	pFolderSync->SyncServerFolder();

	std::wstring archFileId = KLSTD_CreateGUIDString();
	
	KL_TMEASURE_BEGIN(L"MakeFullSyncContent", 1)
	
	pFolderSync->MakeFullSyncContent( KLFT::FolderSync::FolderFilesInfo(),
			archFileId,
			KLFT::FolderSync::SubfoldersList() );

//std::wst/ring archFileId = L"a3c14d77-be52-493b-a981-39afdbc21923";

	KL_TMEASURE_END()

	KL_TMEASURE_BEGIN(L"KLFT_ExtractArchiveFile", 1)
	
	KLFT_ExtractArchiveFile( 		
		L"O:/CS AdminKit/development2/bin/dlld/$FileTransferTmpDir", 
		L"O:/CS AdminKit/development2/bin/dlld/$FileTransferTmpDir/" + archFileId );

	KL_TMEASURE_END()
		


	/*KLSTD_DeletePath( c_WorkingFolder );
	KLSTD_CreateDirectory( c_WorkingFolder, true );

	KLSTD_DeletePath( c_WorkingFolderClient );
	KLSTD_CreateDirectory( c_WorkingFolderClient, true );

	KLFT_GetFileTransfer()->InitClient( c_WorkingFolder, L"", L"", L"", NULL, NULL );*/

	

	return 0;
}


void CFileTransferMulticast::Initialize(KLTST2::TestData2* pTestData)
{	
	m_pTestData = pTestData;

	KLCSKCA_Initialize();
	CALL_MODULE_INIT(KLFT);
}

KLSTD_NOTHROW void CFileTransferMulticast::Deinitialize() throw()
{
	if ( m_pTestData == NULL ) return;
	CALL_MODULE_DEINIT(KLFT);
	KLCSKCA_Deinitialize();
}

int CFileTransferMulticast::GetFileInfo( const std::wstring &fileId, 
			const std::wstring &fileName,
			unsigned long &fullSize,
			std::wstring &folderName )
{
	
	return 0;
}

int CFileTransferMulticast::GetFileChunk( const std::wstring &fileId, 
			const std::wstring &fileName,
			unsigned long pos,
			void *buff, unsigned long buffSize, 
			bool &endOfFile )
{
	m_pFileSend->Seek( pos, KLSTD::ST_SET );
	int read = (int)m_pFileSend->Read( buff, buffSize );
	if ( pos+read>=m_lFileSize ) endOfFile = true;
	return read;
}

int CFileTransferMulticast::OnFileSent( const std::wstring &fileId,
			const std::wstring &fileName, 
			const unsigned long sentSize )
{
	m_pStopSem->Post();
	return 0;
}

long CFileTransferMulticast::Run()
{	
	const std::wstring sendFileName = L"klcskca.dll";
	const std::wstring multicastAddr = L"http://225.6.7.8:15000";
	KLSTD::CAutoPtr<KLFT::MulticastSender> pMulticastSender;

	FTDeleteFolderTree( c_WorkingFolderClient );
	KLSTD_RemoveDirectory( c_WorkingFolderClient, false);
	KLSTD_CreateDirectory( c_WorkingFolderClient, true );

	int usedPort = 0;
	KLTR_GetTransport()->AddListenLocation( c_LocalConnectionName, 
		c_ClientLocation, usedPort );

	KLFT_GetFileTransfer()->InitClient( c_WorkingFolderClient,
		c_RemoteConnectionName, c_LocalConnectionName,  L"",
		NULL,
		new ConnInvoker );

	KLFT_CreateMulticastSender( &pMulticastSender );
	pMulticastSender->Init( multicastAddr.c_str(),
		NULL, this );
	
	KLTR_GetTransport()->JoinMulticastGroup( multicastAddr.c_str(), NULL );

	KLSTD_CreateFile( sendFileName, 0, KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ, KLSTD::SF_READ|KLSTD::SF_WRITE, &m_pFileSend, 0);

	KLSTD_CreateSemaphore( &m_pStopSem, 0 );

	m_lFileSize = (unsigned long)m_pFileSend->GetSize();
	pMulticastSender->SendFile( sendFileName, m_lFileSize );	

	m_pStopSem->Wait();

	KLSTD_Sleep( 1000 );

	m_pFileSend = NULL;

	KLFT::FileDescription fd = KLFT_GetFileTransfer()->GetFileInfo( sendFileName );

	pMulticastSender->Stop();

	/*
	
	if ( PartlySavedFile() )
		{
			const std::wstring sendFileName = L"klcskca.dll";

			KLSTD::CAutoPtr<KLSTD::File> pFileSend;
			KLSTD_CreateFile( sendFileName, 0, KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ, KLSTD::SF_READ|KLSTD::SF_WRITE, &pFileSend, 0);

			std::auto_ptr<char>	resBuff;
			resBuff = std::auto_ptr<char>(new char[c_DefaultChunkSize]);

			while( true )
			{
				unsigned long startFilePos = 0, neededSize = c_DefaultChunkSize;
				bool bfchunk = GetNextNeededChunk( startFilePos, neededSize, neededSize );
				if ( !bfchunk )
				{
					KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Error: can't get next chunk for file. File id - '%ls'\n", 
								sendFileName.c_str() );
					break;
				}

				pFileSend->Seek( startFilePos, KLSTD::ST_SET );
				int read = (int)pFileSend->Read( resBuff.get(), neededSize );
				AddChunk( resBuff.get(), neededSize, 0, startFilePos, m_fullFileSize, false );
			}
		}

	 */

	return 0;
}

void CFileTransferBridge::Initialize(KLTST2::TestData2* pTestData)
{	
	m_pTestData = pTestData;
}

KLSTD_NOTHROW void CFileTransferBridge::Deinitialize() throw()
{
	if ( m_pTestData == NULL ) return;
}


long CFileTransferBridge::Run()
{
	

	return 0;
}