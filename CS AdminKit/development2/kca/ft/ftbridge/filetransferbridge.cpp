/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filetransferbridge.cpp
* \author Шияфетдинов Дамир
* \date 2005
* \brief Реализация класса для взаимодействия процессов с FileTransfer'ом сетевого агента		
*/

#include "gsoap/soapH.h"

#include <kca/ft/filetransferbridge.h>

const char c_RequestFlagTag[]	= "RequestFlag";
const char c_MessageIdTag[]		= "MessageId";

#define IMPLEMENT_SOAP_GETTYPE(_type) int _type::getType(){return SOAP_TYPE_##_type;};

IMPLEMENT_SOAP_GETTYPE(xsd__anyType);
IMPLEMENT_SOAP_GETTYPE(xsd__anySimpleType);
IMPLEMENT_SOAP_GETTYPE(xsd__decimal_);
IMPLEMENT_SOAP_GETTYPE(xsd__integer_);
IMPLEMENT_SOAP_GETTYPE(xsd__long_);

struct Namespace namespaces[] = 
{ 
   {"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/"}, 
   {"SOAP-ENC","http://schemas.xmlsoap.org/soap/encoding/"}, 
   {"xsi", "http://www.w3.org/1999/XMLSchema-instance"}, 
   {"xsd", "http://www.w3.org/1999/XMLSchema"}, 
   {"ns", "urn:person"},
   {"param", "http://tempuri"},
   {NULL, NULL} 
}; 

#define KLFTB_SENDRECV_TIMEOUT	60// 5
#define KLFTB_CONNECT_TIMEOUT	60// 5


//#define KLFTB_TRACE_ON	1

#ifdef KLFTB_TRACE_ON

#include <std/trc/trace.h>

#define KLCS_MODULENAME L"KLTFBRIDGE"

#define KLFTB_TRACE1 KLSTD_TRACE1
#define KLFTB_TRACE2 KLSTD_TRACE2
#define KLFTB_TRACE3 KLSTD_TRACE3
#define KLFTB_TRACE4 KLSTD_TRACE4

#else

#define KLFTB_TRACE1
#define KLFTB_TRACE2
#define KLFTB_TRACE3
#define KLFTB_TRACE4

#endif

//!\brief Функции серализация структуры заголовка SOAP
void soap_mark_message_desc(struct soap* soap, const struct message_desc_t *desc)
{
	soap_embedded(soap, &desc->requestFlag, SOAP_TYPE_xsd__int);
	soap_embedded(soap, &desc->messageId, SOAP_TYPE_xsd__int);
}

void soap_default_message_desc(struct soap* soap, struct message_desc_t *desc)
{
	desc->requestFlag = 0;
	desc->messageId = 0;
}

int soap_out_message_desc(struct soap* soap, const char*tag, int id, 
						   const struct message_desc_t *desc, const char* type)
{
	soap_element_begin_out(soap, tag, id, type); // print XML beginning tag
	soap_out_xsd__int(soap, c_RequestFlagTag, -1, &desc->requestFlag, "xsd:int");	
	soap_out_xsd__int(soap, c_MessageIdTag, -1, &desc->messageId, "xsd:int");
	soap_element_end_out(soap, tag); // print XML ending tag 

	return SOAP_OK;
}

struct message_desc_t *soap_in_message_desc(struct soap* soap, const char* tag, 
											struct message_desc_t *desc, const char* type)
{
	short soap_flag_code = 1, soap_flag_module = 1, soap_flag_file = 1, soap_flag_line = 1, soap_flag_message = 1;
	if (soap_element_begin_in(soap, tag, 0))
		return NULL;	
	if (soap->null)
	{	
		soap->error = SOAP_NULL;
		return NULL;
	}
	if (*soap->type && soap_match_tag(soap, soap->type, type)) 
	{ 
		soap->error = SOAP_TAG_MISMATCH; 
		return NULL; // type mismatch 
	} 	

	if (*soap->href) 
	{
		desc = (struct message_desc_t *)soap_id_forward(soap, soap->href, desc, SOAP_TYPE_message_desc, 
				SOAP_TYPE_message_desc, sizeof(struct message_desc_t), 0, NULL );
	}
	else if (soap->body) 
	{ 
		soap_default_message_desc(soap, desc );
		for (;;)
		{	soap->error = SOAP_TAG_MISMATCH;
			if (soap_flag_code && soap->error == SOAP_TAG_MISMATCH)
				if (soap_in_xsd__int(soap, c_RequestFlagTag, &desc->requestFlag, "xsd:int"))
				{	soap_flag_code = 0;
					continue;
				}
			if (soap_flag_module && soap->error == SOAP_TAG_MISMATCH)
				if (soap_in_xsd__int(soap, c_MessageIdTag, &desc->messageId, "xsd:int"))
				{	soap_flag_module = 0;
					continue;
				}
			if (soap->error == SOAP_TAG_MISMATCH)
				soap->error = soap_ignore_element(soap);
			if (soap->error == SOAP_NO_TAG)
				break;
			if (soap->error)
			{	return NULL;
			}
		}
	} 

	if (soap->body && soap_element_end_in(soap, tag)) 
      return NULL; 

	if ( desc->requestFlag == 1 ) desc->requestFlag = 0;

	return desc;
}

namespace KLFT {

	template <class T>
	class ftstring_buff
	{
	public:
		ftstring_buff()
		{
			ptr = NULL;
		}

		ftstring_buff(T *str)
		{
			ptr = NULL;
			assign(str);
		}
		
		~ftstring_buff()
		{
			if ( ptr!=NULL ) delete[] ptr;
		}

		T* operator=(T *str)
		{
			return assign(str);
		}


		T* c_str()
		{
			return ptr;
		}

		T* assign(T *str)
		{
			if ( ptr!=NULL ) delete[] ptr;

			ptr = NULL;
			if ( str==NULL ) return NULL;			

			int sn = Len(str) + 1;
			ptr = new T[sn];
			memcpy( ptr, str, sn * sizeof(T) );
			return ptr;
		}
				
		int Len(T *str)
		{
			if ( str==NULL ) return 0;
			T *p = str;			
			int n = 0;
			while( *p!=0 )
			{
				n++, p++;
			}

			return n;
		}
		
		
	private:
		T* ptr;
	};

	typedef ftstring_buff<wchar_t> ftwstring;

	#define FTSOAP_FREE(soap)\
		{\
			(*soap)->keep_alive = 0;\
			soap_destroy(*soap);\
			soap_end(*soap);\
			free(*soap);\
			*soap = NULL;\
		}

	class FileTransferBridgeImpl : public FileTransferBridge
	{
	public:

		FileTransferBridgeImpl()
		{			
			m_soap = NULL;
			m_messageIdCounter = 0;
		}

		//!\brief Функция производит подключение к серверу администрирования
		virtual ConnectResult Connect()
		{
			return ConnectInternal( false );			
		}

		/*!
			\brief Функция производит подключение от подчиненного сервера к главному
				серверу администрирования
		*/
		virtual ConnectResult ConnectToMasterServer()
		{
			return ConnectInternal( true );
		}
		
		ConnectResult ConnectInternal( bool toMasterServer )
		{
			m_soap = soap_new();
			soap_init( m_soap );
			m_soap->mode |= SOAP_IO_KEEPALIVE; m_soap->imode |= SOAP_IO_KEEPALIVE; m_soap->omode |= SOAP_IO_KEEPALIVE;
			m_soap->keep_alive = 1;
			m_soap->recv_timeout = KLFTB_SENDRECV_TIMEOUT;
			m_soap->send_timeout = KLFTB_SENDRECV_TIMEOUT;			
			m_soap->connect_timeout = KLFTB_CONNECT_TIMEOUT;

			int sres = soap_connect( m_soap, "http://127.0.0.1:30523", NULL );
			KLFTB_TRACE2( 3, L"Conneting to server: master - %d res - %d\n", toMasterServer, sres );
			if ( sres!=0 ) 
			{
				FTSOAP_FREE(&m_soap);
				return CR_ConnectionNagentError;
			}

			MakeSOAPMessDesc( m_soap );

			klftbridge_CreateReceiverResponse r;
			sres = soap_call_klftbridge_CreateReceiver( m_soap, NULL, NULL, true, r );

			KLFTB_TRACE2( 3, L"CreateReceiver: soap_res - %d r.res - %d\n", sres, r.res );
			if ( sres!=0 )
			{
				FTSOAP_FREE(&m_soap);
				return CR_ConnectionNagentError;
			}

			if ( r.res!=CR_Ok || r.receiverId==NULL ) 
			{
				FTSOAP_FREE(&m_soap);
				return CR_WrongReceiverId;
			}

			m_recieverId = r.receiverId;

			KLFTB_TRACE1( 3, L"CreateReceiver: m_recieverId - '%ls'\n", m_recieverId.c_str() );

			MakeSOAPMessDesc( m_soap );

			klftbridge_ConnectResponse connectres;
			sres = soap_call_klftbridge_Connect( m_soap, NULL, NULL, m_recieverId.c_str(), 
				toMasterServer, connectres );

			KLFTB_TRACE2( 3, L"Connect: sres - %d connectres.res - %d\n", sres, connectres.res );
			if ( sres!=0 ) 
			{
				FTSOAP_FREE(&m_soap);
				return CR_ConnectionServerError;
			}

			if ( connectres.res!=CR_Ok ) 
			{
				FTSOAP_FREE(&m_soap);
				return (KLFT::ConnectResult)connectres.res;
			}

			soap_free(m_soap);

			return CR_Ok;
		}

		//!\brief Возвращает информацию о файле
		virtual FileOpeartionResult GetFileInfo( const wchar_t *fileName,
			FileInfo &fileInfo )
		{
			if ( m_soap==NULL || m_recieverId.c_str()==NULL ) return FR_WrongReceiverId;

			MakeSOAPMessDesc( m_soap );

			klftbridge_GetFileInfoResponse r;
			int sres = soap_call_klftbridge_GetFileInfo( m_soap, NULL, NULL, m_recieverId.c_str(), (wchar_t*)fileName, r );
			if ( sres!=0 ) 
			{
				KLFTB_TRACE1( 3, L"GetFileInfo: sres - %d\n", sres );

				FTSOAP_FREE(&m_soap);
				return FR_ErrorInOperation;
			}

			if ( r.res!=FR_Ok ) 
			{
				KLFTB_TRACE1( 4, L"GetFileInfo: r.res - %d\n", r.res );
				return (FileOpeartionResult)r.res;
			}

			fileInfo.m_isDirectory = r.info.isDir;
			fileInfo.m_fileSize = (long)r.info.fullFileSize;
			fileInfo.m_createTime = (time_t)r.info.createTime;
			fileInfo.m_readPermissionOk = r.info.readPermission;

			return FR_Ok;
		}

		//!\brief Закачивает с сервера порцию файла
		virtual FileOpeartionResult GetFileChunk( const wchar_t *fileName,
			long startPos, void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize )
		{
			if ( m_soap==NULL || m_recieverId.c_str()==NULL ) return FR_WrongReceiverId;

			MakeSOAPMessDesc( m_soap );

			klftbridge_GetNextFileChunkResponse r;
			int sres = soap_call_klftbridge_GetNextFileChunk( m_soap, NULL, NULL, 
				m_recieverId.c_str(), (wchar_t *)fileName, startPos, chunkBuffSize, r );

			if ( sres!=0 || r.res!=FR_Ok || r.res!=FR_OkDownloaded )
			{
				KLFTB_TRACE4( 3, L"GetNextFileChunk: sres - %d fileName - '%ls' startPos - %d r.res - %d\n", 
					sres, fileName, startPos, r.res );
			}

			if ( sres!=0 ) 
			{
				FTSOAP_FREE(&m_soap);
				return FR_ErrorInOperation;
			}

			if ( r.res==FR_FileNotFound || r.res==FR_Timeout )
			{
				return (FileOpeartionResult)r.res;
			}

			if ( r.res==FR_ErrorInOperation || r.res==FR_WrongReceiverId )				
			{
				FTSOAP_FREE(&m_soap);
				return (FileOpeartionResult)r.res;
			}

			memcpy( chunkBuff, r.chunk.buff.__ptr, r.chunk.buff.__size );
			receivedChunkSize = r.chunk.buff.__size;

			return (FileOpeartionResult)r.res;
		}

		//!\brief Разрывает соединение с сервером
		virtual void Disconnect()
		{
			if ( m_soap!=NULL ) FTSOAP_FREE(&m_soap);			
		}

	private:
		struct soap		*m_soap;
		int				m_messageIdCounter;
		ftwstring		m_recieverId;

	protected:
		/* Helpers function */

		void MakeSOAPMessDesc( struct soap *soap )
		{
			message_desc *messDesc = (message_desc *)soap_malloc( soap,
					sizeof(message_desc) );
			messDesc->requestFlag = 1;
			messDesc->messageId = m_messageIdCounter++;
			soap->header = (struct SOAP_ENV__Header *)messDesc;
		}

	};

} // end namespace KLFT

/*!
	\brief Функция создает экземпляр класса FileTransferBridge, который после использования
	необходимо освободить функцией KLFT_ReleaseFileTransferBridge
*/
void KLFT_CreateFileTransferBridge( KLFT::FileTransferBridge **ppBridge )
{
	*ppBridge = new KLFT::FileTransferBridgeImpl();
}

/*!
	\brief Функция освобождает экземпляр класса FileTransferBridge
*/
void KLFT_ReleaseFileTransferBridge( KLFT::FileTransferBridge **ppBridge )
{
	if ( *ppBridge==NULL ) return;
	delete *ppBridge;
	*ppBridge = NULL;
}
