/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file AF/filetransfersoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2004
 * \brief SOAP-интерфейс FileTransfer'а 
 */

class SOAPUpdateAgentInfo {
public:	
	xsd__wstring connectionName;
	xsd__wstring location;
	xsd__wstring sslLocation;	
	xsd__wstring multicastAddr;
};

struct SOAPUpdateAgentInfosList
{
	SOAPUpdateAgentInfo* 	__ptr;
	int						__size;
};


int klft_InitiateFileUpload(
						xsd__wstring	fileName,
						xsd__wstring	fileId,
						xsd__wstring	description,
						xsd__unsignedLong	dirFlag,
						xsd__unsignedLong	fullFileSize,						
						xsd__wstring	serverComponent,
						xsd__wstring	remoteClientName,
						xsd__unsignedLong	uploadDelay,
						xsd__unsignedLong	nextChunkDelay,
						xsd__int			fileType,
						xsd__unsignedLong	forUpdateAgentFlag,
						struct SOAPUpdateAgentInfosList *updatesAgents,
						struct klft_InitiateFileUploadResponse
						{
							struct param_error	error;
						}	&r );

int klft_RegisterUpdateAgent(
						xsd__wstring	strAddress,
						xsd__wstring	strNoSSLAddress,
						xsd__wstring	strMulticastIp,
						param__params	pParOptions,
						struct klft_RegisterUpdateAgentResponse
						{
							struct param_error	error;
						}	&r );

int klft_InitiateServerFileUpload(						
						xsd__wstring	fileId,
						xsd__unsignedLong	dirFlag,
						xsd__unsignedLong	lifeTime,
						xsd__unsignedLong	fullFileSize,
						struct klft_InitiateServerFileUploadResponse
						{							
							struct param_error	error;
						}	&r );

int klft_CancelServerFileUpload(						
						xsd__wstring	fileId,
						struct klft_CancelServerFileUploadResponse
						{							
							struct param_error	error;
						}	&r );

int klft_PutNextFileChunk(						
						xsd__wstring	fileId,						
						struct klft_file_chunk_t	chunk,
						struct klft_PutNextFileChunkResponse
						{							
							struct param_error	error;
						}	&r );

class SOAPFolderFileInfo {
public:	
	xsd__unsignedInt	CRC;
	xsd__string			FileNameHash;
};

struct SOAPFilesInfoList
{
	SOAPFolderFileInfo* 	__ptr;
	int						__size;
};

struct SOAPSyncFolderSubfoldersList
{
	xsd__wstring*			__ptr;
	int						__size;
};

struct SOAPSyncFolderOptions
{
	struct SOAPSyncFolderSubfoldersList subfoldersList;
};

struct SOAPFolderHashList {	
	xsd__string*			__ptr;
	int						__size;
};


int klft_GetSyncFolderDiff(						
						xsd__wstring	fileDescription,
						struct SOAPFilesInfoList *clientFilesList,
						struct SOAPSyncFolderOptions *subfoldersOptions,
						struct klft_GetSyncFolderDiffResponse
						{							
							struct param_error	error;
							xsd__wstring		syncFolderFileId;
							xsd__boolean		folderIsEqual;
							struct SOAPFilesInfoList *diffFilesList;
							struct SOAPFolderHashList *folderHashList;
						}	&r );

int klft_GetSyncFolderFileInfo(
						xsd__wstring	syncFolderFileId,
						xsd__wstring	folderFileName,						
						struct klft_GetSyncFolderFileInfoResponse
						{
							struct klft_file_info_t	info;
							struct param_error	error;
						}	&r );

int klft_GetSyncFolderFileChunk(
						xsd__wstring	syncFolderFileId,
						xsd__wstring	folderFileName,						
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,
						struct klft_GetSyncFolderFileChunkResponse
						{							
							struct klft_file_chunk_t	chunk;
							struct param_error			error;							
						}	&r );


