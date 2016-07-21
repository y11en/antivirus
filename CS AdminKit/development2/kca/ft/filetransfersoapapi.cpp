/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file AF/filetransfersoapapi.cpp
 * \author Ўи€фетдинов ƒамир
 * \date 2004
 * \brief SOAP-интерфейс FileTransfer'а 
 */

#include <std/base/klbase.h>
#include "soapH.h"
#include "soapStub.h"
#include "boost/crc.hpp"

#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/err/klerrors.h>
#include <std/par/paramsi.h>
#include <std/err/klerrors.h>

#include <kca/prts/prxsmacros.h>

#include "filetransferimp.h"
#include "filereceiver.h"
#include "filereceiverbridge.h"
#include <kca/ft/errors.h>
#include <kca/ft/filetransferbridge.h>

#define KLCS_MODULENAME L"KLFT"


#define HANDLER_BEGIN	\
		KLERR_TRY

#define HANDLER_END	\
		KLERR_CATCH(pError)	\
			KLPAR::ExceptionForSoap(soap, pError, r.error);	\
		KLERR_ENDTRY;

#define FTBRIDGE_HANDLER_BEGIN	\
		KLERR_TRY

#define FTBRIDGE_HANDLER_END	\
		KLERR_CATCH(pError)	\
			KLERR_SAY_FAILURE( 2, pError );\
			r.res = (-1);\
		KLERR_ENDTRY;

int klft_InitiateFileUploadStub(
						struct soap* soap, 
						wchar_t*		fileName,
						wchar_t*		fileId,
						wchar_t*		description,	
						xsd__unsignedLong	dirFlag,
						xsd__unsignedLong	fullFileSize,						
						wchar_t*		serverComponent,
						wchar_t*		remoteClientName,
						xsd__unsignedLong	uploadDelay,
						xsd__unsignedLong	nextChunkDelay,
						xsd__int			fType,
						xsd__unsignedLong	forUpdateAgentFlag,
						struct SOAPUpdateAgentInfosList *updatesAgents,
						struct klft_InitiateFileUploadResponse &r )
{
	HANDLER_BEGIN
	{
		KLTRAP::Transport::ConnectionFullName connName;
		KLTR_GetTransport()->GetConnectionName( soap, connName );

		KLFT::FileType fileType = (KLFT::FileType)fType;
		KLFT::UpdateAgentLocationList uaLocs;

		if ( updatesAgents!=NULL && updatesAgents->__size!=0 )
		{
			for( int i = 0; i < updatesAgents->__size; ++i )
			{
				KLFT::UpdateAgentLocation uaLoc;
				uaLoc.strConnectionName = updatesAgents->__ptr[i].connectionName;
				uaLoc.strLocation = updatesAgents->__ptr[i].location;
				uaLoc.strSSLLocation = updatesAgents->__ptr[i].sslLocation;
				uaLoc.strMulticastAddr = updatesAgents->__ptr[i].multicastAddr;

				uaLocs.push_back( uaLoc );
			}			
		}

		if ( fileType==KLFT::FT_Package || fileType==KLFT::FT_Folder )
		{
			((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->InitiatePackageUpload( 
				fileName, fileId, description, 
				(dirFlag>0),
				(fileType==KLFT::FT_Package), connName, 
				fileType,
				(unsigned long)uploadDelay, (unsigned long)nextChunkDelay );
		}
		else
		{
			bool useOnlyExistConn = false;
			if ( fileType==KLFT::FT_SyncArchive )
			{
				KLFT::FileDescription fDesc;
				KLERR_IGNORE( fDesc = ((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->GetFileInfo( description ) );
				if ( fDesc.m_type==KLFT::FT_Package )
				{
					// дл€ распространени€ разницы пакеджей по иерархии серверов используем существующие соединение
					useOnlyExistConn = true;
				}
			}
			if ( !useOnlyExistConn )
			{
				connName.remoteComponentName = remoteClientName;
			}

			((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->InitiateFileUpload( 
				fileName, fileId, description, 
				(dirFlag>0), (unsigned long)fullFileSize, 
				useOnlyExistConn, connName,
				fileType,
				(KLFT::UploadingType)forUpdateAgentFlag, uaLocs,			
				(unsigned long)uploadDelay, (unsigned long)nextChunkDelay );
		}
	}
	HANDLER_END
	return SOAP_OK;
}

int klft_RegisterUpdateAgentStub(
						struct soap*	soap, 
						xsd__wstring	strAddress,
						xsd__wstring	strNoSSLAddress,
						xsd__wstring	strMulticastAddr,
						param__params	pParOptions,
						struct klft_RegisterUpdateAgentResponse &r )
{
	HANDLER_BEGIN
	{		
		KLSTD::CAutoPtr<KLPAR::Params> pParOpts;
		
		KLPAR::ParamsFromSoap( pParOptions, &pParOpts );
		if ( pParOpts==NULL ) KLPAR_CreateParams( &pParOpts );

		long nGroupId = (-1), nAgentHostId = (-1);
		std::wstring connName;
		std::wstring strCertPswd;
		KLSTD::CAutoPtr<KLPAR::BinaryValue> pCert;

		GET_PARAMS_VALUE_NO_THROW_ALL( pParOpts, KLFT::UPDATE_AGENT_GROUP_ID, IntValue, INT_T, nGroupId );
		KLSTD::CAutoPtr<KLPAR::BinaryValue> pVal = (KLPAR::BinaryValue*)pParOpts->GetValue2(KLFT::UPDATE_AGENT_CERTIFICATE, false); 
		if (pVal && pVal->GetType() == KLPAR::Value::BINARY_T) { 
			pCert = pVal; 
		}
		GET_PARAMS_VALUE_NO_THROW_ALL( pParOpts, KLFT::UPDATE_AGENT_CERTIFICATE_PSWD, StringValue, STRING_T, strCertPswd );
		GET_PARAMS_VALUE_NO_THROW_ALL( pParOpts, KLFT::UPDATE_AGENT_HOST_ID, IntValue, INT_T, nAgentHostId );		
		GET_PARAMS_VALUE_NO_THROW_ALL( pParOpts, KLFT::UPDATE_AGENT_CONN_NAME, StringValue, STRING_T, connName );

		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->RegisterUpdateAgent( nAgentHostId, connName, 
			strAddress, strNoSSLAddress,
			strMulticastAddr, nGroupId, pCert, strCertPswd );

	}
	HANDLER_END
	return SOAP_OK;
};

int klft_InitiateServerFileUploadStub(	
						struct soap*		soap,
						xsd__wstring		fileId,
						xsd__unsignedLong	dirFlag,
						xsd__unsignedLong	lifeTime,
						xsd__unsignedLong	fullFileSize,
						struct klft_InitiateServerFileUploadResponse &r )
{
	HANDLER_BEGIN
	{
		KLTRAP::Transport::ConnectionFullName connName;
		KLTR_GetTransport()->GetConnectionName( soap, connName );
		
		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->InitiateServerFileUpload( 
			fileId, dirFlag!=0, (unsigned long)fullFileSize, (unsigned long)lifeTime,
			connName.localComponentName, connName.remoteComponentName );
		
	}
	HANDLER_END
	return SOAP_OK;
};

int klft_CancelServerFileUploadStub(	
						struct soap*		soap,
						xsd__wstring		fileId,
						struct klft_CancelServerFileUploadResponse &r )
{
	HANDLER_BEGIN
	{
		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->DeleteFile( fileId );
	}
	HANDLER_END
	return SOAP_OK;
};

int klft_GetNextFileChunkUAStub(
						struct soap* soap,
						wchar_t*	remoteClientName,
						wchar_t*	fileName,
						wchar_t*	fileId,
						xsd__unsignedLong startFilePos,
						xsd__unsignedLong neededSize,
						xsd__boolean	  useUpdaterFolder,
						xsd__boolean	  forUpdateAgent,
						struct klft_GetNextFileChunkOptions *options,
						struct klft_GetNextFileChunkResponse &r )
{
	HANDLER_BEGIN
	{
		char *pBuff = NULL;
		unsigned long fullFileSize = 0;
		int buffSize = 0;
		std::wstring rClientName = remoteClientName;
		KLPRCI::ComponentId compId;
		KLERR_IGNORE( compId.GetFromString( remoteClientName ) );
		if ( !compId.instanceId.empty() ) rClientName = compId.instanceId;

		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->GetNextFileChunk( 
			rClientName,
			fileName, fileId, 
			(unsigned long)startFilePos, (unsigned long)neededSize,
			fullFileSize, useUpdaterFolder, 
			(options==NULL)?0:(unsigned long)options->nMulticastRecvSize,
			&pBuff, buffSize );

		if ( buffSize==0 && fullFileSize!=0 )
			KLFT_THROW_ERROR( ERR_FILE_OPERATION );

		ULONG64 buffCrc = 0;
		if ( buffSize==0 )
		{
			r.chunk.buff.__ptr = NULL;
			r.chunk.buff.__size = 0;
		}
		else
		{
			r.chunk.buff.__ptr = ( unsigned char *) soap_malloc( soap, buffSize );
			memcpy( r.chunk.buff.__ptr, pBuff, buffSize );
			delete[] pBuff;
			r.chunk.buff.__size = buffSize;

			KLSTD::CPointer<boost::crc_32_type> crc32;
			crc32=new boost::crc_32_type;
			KLSTD_CHKMEM(crc32);
			crc32->process_bytes( r.chunk.buff.__ptr, r.chunk.buff.__size );
			buffCrc = crc32->checksum();
		}

		r.chunk.CRC = buffCrc;
		r.chunk.fileId = KLPAR::soap_strdup(soap, fileId);;
		r.chunk.fileName = KLPAR::soap_strdup(soap, fileName);
		r.chunk.startPosInFile = startFilePos;
		r.chunk.fullFileSize = fullFileSize;
	}
	KLERR_CATCH(pError)

		KLERR_BEGIN

		if ( useUpdaterFolder )
		{
			((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->DeleteUpdaterFile( 
				fileId );
		}

		KLERR_END

			KLPAR::ExceptionForSoap(soap, pError, r.error);	

	KLERR_ENDTRY;
		
	return SOAP_OK;
}


int klft_PutNextFileChunkStub(			
						struct soap*				soap, 
						xsd__wstring				fileId,						
						struct klft_file_chunk_t	chunk,
						struct klft_PutNextFileChunkResponse &r )
{
	HANDLER_BEGIN
	{
		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->PutNextFileChunk( 
			fileId, (unsigned long)chunk.startPosInFile, 
			(unsigned long)chunk.fullFileSize, 
			(unsigned long)chunk.CRC, chunk.buff.__ptr, chunk.buff.__size );
	}
	HANDLER_END
	return SOAP_OK;
};

int klft_GetSyncFolderDiffStub(
						struct soap*				soap, 
						xsd__wstring	fileDescription,
						struct SOAPFilesInfoList *clientFilesList,
						struct SOAPSyncFolderOptions *syncFolderOptions,
						struct klft_GetSyncFolderDiffResponse &r )
{
	HANDLER_BEGIN
	{
		KLFT::FolderSync::FileInfoMap clientFolderFilesInfo;

		if ( clientFilesList!=NULL && clientFilesList->__size>0 )
		{
			for( int c = 0; c < clientFilesList->__size; c++ )
			{
				KLFT::FolderSync::FolderFileInfo fi( clientFilesList->__ptr[c].CRC, clientFilesList->__ptr[c].FileNameHash );
				clientFolderFilesInfo[clientFilesList->__ptr[c].FileNameHash] = fi;
			}
		}

		KLFT::FolderSync::SubfoldersList subfoldersList;
		if ( syncFolderOptions!=NULL && syncFolderOptions->subfoldersList.__size>0 )
		{
			for( int c = 0; c < syncFolderOptions->subfoldersList.__size; c++ )
			{				
				subfoldersList.push_back( syncFolderOptions->subfoldersList.__ptr[c] );
			}
		}

		std::wstring syncFolderFileId;		
		KLFT::FolderSync::FolderFilesInfo diffFolderFilesInfo;
		KLFT::FolderSync::SubfoldersHashList subfoldersHashList;

		r.folderIsEqual = (((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->GetSyncFolderDiff( 
			fileDescription, clientFolderFilesInfo,
			syncFolderFileId,
			diffFolderFilesInfo, 
			subfoldersList, subfoldersHashList )==false);

		r.syncFolderFileId = KLPAR::soap_strdup( soap, syncFolderFileId.c_str() );
		
		r.diffFilesList = (SOAPFilesInfoList *)soap_malloc( soap, sizeof(SOAPFilesInfoList) );
		r.diffFilesList->__size = diffFolderFilesInfo.FilesList.size();
		if ( r.diffFilesList->__size>0 )
		{
			r.diffFilesList->__ptr = soap_new_SOAPFolderFileInfo( soap, r.diffFilesList->__size );								
			KLFT::FolderSync::FileInfoList::iterator it = diffFolderFilesInfo.FilesList.begin();
			int c = 0;
			for( ; it != diffFolderFilesInfo.FilesList.end(); ++it, ++c )
			{
				r.diffFilesList->__ptr[c].CRC = it->CRC;
				r.diffFilesList->__ptr[c].FileNameHash = KLPAR::soap_strdup( soap, it->FileNameHash.c_str() );
			}
		}
		else
		{
			r.diffFilesList->__ptr = NULL;
		}
		
		r.folderHashList = (SOAPFolderHashList *)soap_malloc( soap, sizeof(SOAPFolderHashList) );
		r.folderHashList->__size = subfoldersHashList.size();
		if ( r.folderHashList->__size>0 )
		{
			r.folderHashList->__ptr = (xsd__string*)soap_malloc( soap, (sizeof(xsd__string) * r.folderHashList->__size) );
			KLFT::FolderSync::SubfoldersHashList::iterator it = subfoldersHashList.begin();
			for( int c = 0; it != subfoldersHashList.end(); ++it, ++c )
			{
				r.folderHashList->__ptr[c] = KLPAR::soap_strdup( soap, (*it).c_str() );
			}
		}
		else
		{
			r.folderHashList->__ptr = NULL;
		}
	}
	HANDLER_END
	return SOAP_OK;
};

int klft_GetSyncFolderFileInfoStub(
						struct soap*				soap, 
						xsd__wstring	syncFolderFileId,
						xsd__wstring	folderFileName,						
						struct klft_GetSyncFolderFileInfoResponse &r )
{
	HANDLER_BEGIN
	{
		KLTRAP::Transport::ConnectionFullName connName;
		KLTR_GetTransport()->GetConnectionName( soap, connName );

		KLFT::FileInfo fi;
		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->GetSyncFolderFileInfo( 
			connName.remoteComponentName,
			syncFolderFileId, folderFileName, fi );

		r.info.fileName = KLPAR::soap_strdup(soap, folderFileName);
		r.info.isDir = fi.m_isDirectory;
		r.info.readPermission = true;
		r.info.createTime = fi.m_createTime;
		r.info.fullFileSize = fi.m_fileSize;
	}
	HANDLER_END
	return SOAP_OK;
};

int klft_GetSyncFolderFileChunkStub(
						struct soap*				soap, 
						xsd__wstring	syncFolderFileId,
						xsd__wstring	folderFileName,						
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,
						struct klft_GetSyncFolderFileChunkResponse &r )
{
	HANDLER_BEGIN
	{
		KLTRAP::Transport::ConnectionFullName connName;
		KLTR_GetTransport()->GetConnectionName( soap, connName );

		void *pBuff = NULL;
		unsigned long fullFileSize = 0;
		unsigned long buffSize = 0;		
		bool endOfFile = false;
		
		((KLFT::FileTransferImp *)KLFT_GetFileTransfer())->GetSyncFolderFileChunk( 
			connName.remoteComponentName,
			syncFolderFileId, folderFileName, 
			(unsigned long)startFilePos, (unsigned long)neededSize,
			fullFileSize,
			&pBuff, buffSize,
			endOfFile );

		if ( buffSize==0 && fullFileSize!=0 )
			KLFT_THROW_ERROR( ERR_FILE_OPERATION );

		ULONG64 buffCrc = 0;
		if ( buffSize==0 )
		{
			r.chunk.buff.__ptr = NULL;
			r.chunk.buff.__size = 0;
		}
		else
		{
			r.chunk.buff.__ptr = ( unsigned char *) soap_malloc( soap, buffSize );
			memcpy( r.chunk.buff.__ptr, pBuff, buffSize );
			delete (char *)pBuff;
			r.chunk.buff.__size = buffSize;

			KLSTD::CPointer<boost::crc_32_type> crc32;
			crc32=new boost::crc_32_type;
			KLSTD_CHKMEM(crc32);
			crc32->process_bytes( r.chunk.buff.__ptr, r.chunk.buff.__size );
			buffCrc = crc32->checksum();
		}

		r.chunk.CRC = buffCrc;
		r.chunk.fileId = KLPAR::soap_strdup(soap, syncFolderFileId);;
		r.chunk.fileName = KLPAR::soap_strdup(soap, folderFileName);
		r.chunk.startPosInFile = startFilePos;
		r.chunk.fullFileSize = fullFileSize;
	}
	HANDLER_END
	return SOAP_OK;
};



SOAP_FMAC1 int SOAP_FMAC2 klftbridge_CreateReceiverStub( struct soap* soap,
						xsd__boolean		useUpdaterFolder,
						struct klftbridge_CreateReceiverResponse &r )
{
	FTBRIDGE_HANDLER_BEGIN
	{
		KLTRAP::Transport::ConnectionFullName connName;
		KLTR_GetTransport()->GetConnectionName( soap, connName );

		KLSTD::CAutoPtr<KLFT::FileReceiverBridgeBase> pFileReceiverBridge = KLFT_GetFileReceiverBridge();

		std::wstring receiverid;
		pFileReceiverBridge->CreateReceiver( receiverid,
			connName.localComponentName,
			connName.remoteComponentName );

		r.receiverId = KLPAR::soap_strdup(soap, receiverid.c_str());
	}
	FTBRIDGE_HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klftbridge_ConnectStub( struct soap* soap, 
						xsd__wstring			receiverId,
						xsd__boolean			toMasterServer,
						struct klftbridge_ConnectResponse &r )
{
	FTBRIDGE_HANDLER_BEGIN
	{
		KLSTD::CAutoPtr<KLFT::FileReceiverBridgeBase> pFileReceiverBridge = KLFT_GetFileReceiverBridge();
		
		KLFT::ConnectResult res;
		
		if ( !toMasterServer )
			res = pFileReceiverBridge->ConnectToServer( receiverId );
		else
			res = pFileReceiverBridge->ConnectToMasterServer( receiverId );

		r.res = res;
	}
	FTBRIDGE_HANDLER_END
	return SOAP_OK;
};


SOAP_FMAC1 int SOAP_FMAC2 klftbridge_GetFileInfoStub( struct soap* soap,
						xsd__wstring	receiverId,
						xsd__wstring	fileName,
						struct klftbridge_GetFileInfoResponse &r )
{
	FTBRIDGE_HANDLER_BEGIN
	{
		KLSTD::CAutoPtr<KLFT::FileReceiverBridgeBase> pFileReceiverBridge = KLFT_GetFileReceiverBridge();
		
		KLFT::FileInfo fi;
		KLFT::FileOpeartionResult res = pFileReceiverBridge->GetFileInfo( 
			receiverId, fileName, fi );

		r.res = res;
		if ( res==KLFT::FR_Ok  )
		{
			r.info.fileName = KLPAR::soap_strdup(soap, fileName);
			r.info.isDir = fi.m_isDirectory;
			r.info.readPermission = true;
			r.info.createTime = fi.m_createTime;
			r.info.fullFileSize = fi.m_fileSize;
		}
	}
	FTBRIDGE_HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klftbridge_GetNextFileChunkStub( struct soap* soap,
						xsd__wstring	receiverId,
						xsd__wstring	fileName,						
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,						
						struct klftbridge_GetNextFileChunkResponse &r )
{
	FTBRIDGE_HANDLER_BEGIN
	{
		KLSTD::CAutoPtr<KLFT::FileReceiverBridgeBase> pFileReceiverBridge = KLFT_GetFileReceiverBridge();
		
		KLSTD::CArrayPointer2<char> pBuff( new char[(int)neededSize] );
		int buffSize = 0;

		KLFT::FileOpeartionResult res = pFileReceiverBridge->GetFileChunk( 
			receiverId, fileName, 
			(unsigned long)startFilePos, pBuff.get(), (int)neededSize, buffSize );

		if ( res!=KLFT::FR_Ok && res!=KLFT::FR_OkDownloaded ) 
		{
			r.res = res;
			return SOAP_OK;
		}

		ULONG64 buffCrc = 0;
		if ( buffSize==0 )
		{
			r.chunk.buff.__ptr = NULL;
			r.chunk.buff.__size = 0;
		}
		else
		{
			r.chunk.buff.__ptr = ( unsigned char *) soap_malloc( soap, buffSize );
			memcpy( r.chunk.buff.__ptr, pBuff.get(), buffSize );			
			r.chunk.buff.__size = buffSize;

			KLSTD::CPointer<boost::crc_32_type> crc32;
			crc32=new boost::crc_32_type;
			KLSTD_CHKMEM(crc32);
			crc32->process_bytes( r.chunk.buff.__ptr, r.chunk.buff.__size );
			buffCrc = crc32->checksum();
		}

		r.chunk.CRC = buffCrc;
		r.chunk.fileId = KLPAR::soap_strdup(soap, fileName);
		r.chunk.fileName = KLPAR::soap_strdup(soap, fileName);
		r.chunk.startPosInFile = startFilePos;
		r.chunk.fullFileSize = 0;

		r.res = res;
	}
	FTBRIDGE_HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klftbridge_ReleaseReceiverStub( struct soap* soap,
						xsd__wstring			receiverId,
						struct klftbridge_ReleaseReceiverResponse &r )
{
	FTBRIDGE_HANDLER_BEGIN
	{
		KLSTD::CAutoPtr<KLFT::FileReceiverBridgeBase> pFileReceiverBridge = KLFT_GetFileReceiverBridge();
		
		pFileReceiverBridge->ReleaseReceiver( receiverId );
	}
	FTBRIDGE_HANDLER_END
	return SOAP_OK;
};


