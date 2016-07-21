
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

#include "../filereceiverbridgesoapapi.h"
