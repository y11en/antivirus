
/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file ftbridgesoapsoap.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief SOAP header для библиотеки ftbridge
 */
                                             
class xsd__anyType {
public:
	virtual int getType();
}; 

class xsd__anySimpleType: public xsd__anyType {
public:
	virtual int getType();
}; 

typedef bool xsd__boolean; 

typedef char *xsd__decimal; 
class xsd__decimal_: public xsd__anySimpleType {
public:
	xsd__decimal __item;
	virtual int getType();
}; 

typedef char *xsd__integer; 
class xsd__integer_: public xsd__decimal_ {
public:
	xsd__integer __item;
	virtual int getType();
}; 

typedef LONG64 xsd__long; 

class xsd__long_: public xsd__integer_ {
public:
	xsd__long __item;
	virtual int getType();
}; 

typedef long xsd__int; 

typedef ULONG64 xsd__unsignedLong; 

typedef unsigned char xsd__unsignedByte; 

typedef wchar_t *xsd__wstring; 

extern typedef struct message_desc_t{
	long requestFlag;
	long messageId;
} message_desc;

struct SOAP_ENV__Header 
{ 
	message_desc messDesc;	
}; 

/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file AF/filereceiverbridgesoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2005
 * \brief SOAP-интерфейс FileReceiver'а 
 */


struct xsd__klft_file_chunk_buff
{
	xsd__unsignedByte*	__ptr;
	int					__size;
};

struct klft_file_chunk_t
{
	xsd__wstring		fileName;
	xsd__wstring		fileId;
	
	xsd__unsignedLong	CRC;
	xsd__unsignedLong	startPosInFile;
	xsd__unsignedLong	fullFileSize;

	struct xsd__klft_file_chunk_buff buff;
};

struct klft_file_info_t
{
	xsd__wstring		fileName;
	xsd__boolean		isDir;
	xsd__boolean		readPermission;
	xsd__unsignedLong	createTime;
	xsd__unsignedLong	fullFileSize;
};

int klftbridge_CreateReceiver( xsd__boolean		useUpdaterFolder,
						struct klftbridge_CreateReceiverResponse
						{
							xsd__wstring				receiverId;							
							xsd__long					res;
						}	&r );

int klftbridge_Connect( xsd__wstring			receiverId,
					    xsd__boolean			toMasterServer,
						struct klftbridge_ConnectResponse
						{
							xsd__long					res;
						}	&r );

int klftbridge_GetFileInfo(
						xsd__wstring	receiverId,
						xsd__wstring	fileName,
						struct klftbridge_GetFileInfoResponse
						{
							struct klft_file_info_t	info;
							xsd__long				res;
						}	&r );

int klftbridge_GetNextFileChunk(
						xsd__wstring	receiverId,
						xsd__wstring	fileName,						
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,						
						struct klftbridge_GetNextFileChunkResponse
						{
							struct klft_file_chunk_t	chunk;
							xsd__long					res;
						}	&r );

int klftbridge_ReleaseReceiver( xsd__wstring			receiverId,
						struct klftbridge_ReleaseReceiverResponse
						{
							xsd__long					res;
						}	&r );


