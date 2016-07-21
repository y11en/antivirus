/*!
 * (C) 2007 "Kaspersky Lab"
 *
 * \file fileuploader.h
 * \author Дамир Шияфетдинов
 * \date 2007
 * \brief Интерфейс класса закачки файла с UA
 * 
 */

#include <std/trc/trace.h>
#include <std/io/klio.h>
#include <std/par/par_gsoap.h>

#include <transport/tr/transportproxy.h>

#include "soapH.h"

#include "fileuploader.h"

#define KLCS_MODULENAME L"KLFTUPLOADER"

namespace KLFT {

	void FileUploader::GetSyncFolderFile( const KLTRAP::Transport::ConnectionFullName &remoteConnection, 
			const KLTRAP::Transport::ConnectionFullName &fromConnection, 
			KLFT::FolderSync *pFolderSync,
			const std::wstring &fileName, 
			const std::wstring &operationId )
	{
		std::wstring tempUploadFileName = pFolderSync->GetUploadingTemporyFileName( operationId );
		
		KLSTD_TRACE4( 3, L"FileUploader::GetSyncFolderFile fileName - '%ls' localName - '%ls' remoteName - '%ls' folderSyncId - '%ls'\n", 
					fileName.c_str(), remoteConnection.localComponentName.c_str(), 
					remoteConnection.remoteComponentName.c_str(), pFolderSync->GetFileId().c_str() );

		KLSTD::CAutoPtr<KLSTD::File> pFile;
		KLSTD_CreateFile( tempUploadFileName, KLSTD::SF_READ | KLSTD::SF_WRITE,
					KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_READ | KLSTD::AF_WRITE, 0,
					&pFile );
		
		KLTRAP::TransportProxy trProxy;		
		trProxy.Initialize( remoteConnection.localComponentName.c_str(), 
			remoteConnection.remoteComponentName.c_str() );
		
		int neededSize = c_MaxChunkSize;		
		unsigned long startFilePos = 0;
		bool endOfFile = false;

		while( !endOfFile )
		{				
			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
			
			struct soap* pSoap = proxyLocker.Get();
			
			struct klft_GetNextFileChunkResponse res;
			

			soap_call_klft_GetNextFileChunk( pSoap, NULL, NULL,
						(wchar_t*)remoteConnection.localComponentName.c_str(),						
						(wchar_t*)fileName.c_str(), (wchar_t*)fileName.c_str(),
						startFilePos, neededSize,
						true, false, NULL,
						res );
			
			proxyLocker.CheckResult();
			
			if ( res.error.code )
			{	
				KLSTD_TRACE2( 3, L"FileUploader::GetSyncFolderFile pSoap->error - %d res.error.code - %d\n", 
					pSoap->error, res.error.code );

				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );				
				throw pError;
			}

				
			KLSTD_TRACE2( 3, L"FileUploader::GetSyncFolderFile call result buffSize - %d fullFileSize - %d\n", 
				res.chunk.buff.__size, res.chunk.fullFileSize );

			pFile->Write( res.chunk.buff.__ptr, res.chunk.buff.__size );

			startFilePos += res.chunk.buff.__size;
			if ( startFilePos>=res.chunk.fullFileSize ) endOfFile = true;
		}

		pFile = NULL;
		KLSTD_TRACE2( 3, L"FileUploader::GetSyncFolderFile UpdateFile fileName - '%ls' tempName - '%ls'\n", 
					fileName.c_str(), tempUploadFileName.c_str() );

		pFolderSync->UpdateFile( tempUploadFileName, fileName );

		KLSTD_Unlink( tempUploadFileName.c_str(), false );
	}
	

} // end namespace KLFT