#ifndef  _MESSAGE_INFO_H
#define  _MESSAGE_INFO_H

#ifndef __objidl_h__
#include <objidl.h> // IStream
#endif


// TODO (DVi): include real header
//#ifndef pgMESSAGE_PART_CONTENT_LENGTH
//    #define pgMESSAGE_PART_CONTENT_LENGTH   0
//#endif
//#ifndef pgMESSAGE_INTERNET_HEADERS
//    #define pgMESSAGE_INTERNET_HEADERS   0
//#endif

/*
    #include <iface/i_mailmsg.h> // В этом файле определены ulPropID
	// global Mail Message properties 
	pgMESSAGE_FROM						
	pgMESSAGE_ORIGINAL_FROM				
	pgMESSAGE_DISPLAY_FROM				
	pgMESSAGE_ORIGINAL_DISPLAY_FROM		
	pgMESSAGE_TO						
	pgMESSAGE_DISPLAY_TO				
	pgMESSAGE_CC						
	pgMESSAGE_DISPLAY_CC				
	pgMESSAGE_SUBJECT					
	pgMESSAGE_DATE						
	pgMESSAGE_MAILID					
	pgMESSAGE_INTERNET_HEADERS			
	pgMESSAGE_MIME_VERSION				
	pgMESSAGE_RETURN_PATH				
	
	// global Mail Message Part properties 
	pgMESSAGE_PART_CONTENT_TYPE			
	pgMESSAGE_PART_TRANSFER_ENCODING	
	pgMESSAGE_PART_FILE_NAME			
	pgMESSAGE_PART_DISPLAY_NAME			
	pgMESSAGE_PART_CONTENT_ID			
	pgMESSAGE_PART_CONTENT_DESCRIPTION	
	pgMESSAGE_PART_CHARSET				
	pgMESSAGE_PART_CONTENT_LENGTH		
*/

class INFO
{
public:
	virtual ~INFO() {}
	// GetPropStr возвращает указатель на строковое свойство (например, pgMESSAGE_FROM)
	// В случае отсутствия свойства возвращается NULL
	// Указатель жив, пока жив объект INFO.
	virtual const char* GetPropStr(  unsigned long ulPropID ) = 0;
	// GetPropLong возвращает указатель на числовое свойство (например, pgMESSAGE_PART_CONTENT_LENGTH)
	// В случае отсутствия свойства возвращается 0
	virtual const long  GetPropLong( unsigned long ulPropID ) = 0;
};

class MESSAGEPART_INFO : public INFO
{
public:
	virtual ~MESSAGEPART_INFO() {}
	// GetData возвращает указатель на поток данных.
	// Указатель жив, пока не вызвано ReleaseData и жив объект MESSAGEPART_INFO. 
	// Указатель должен быть освобожден вызовом ReleaseData.
	virtual IStream* GetData() = 0;
	virtual void ReleaseData() = 0;
};

class MESSAGE_INFO : public INFO
{
public:
	virtual ~MESSAGE_INFO() {}
    // GetMessagePart возращает указатель на MESSAGEPART_INFO по порядковому номеру num
	// Указатель должен быть освобожден вызовом ReleaseMessagePart
    virtual MESSAGEPART_INFO* GetMessagePart(unsigned long num) = 0;
	virtual void ReleaseMessagePart(MESSAGEPART_INFO* pMessagePart) = 0;
	virtual const char* GetRawMessage() = 0;
};

#endif//_MESSAGE_INFO