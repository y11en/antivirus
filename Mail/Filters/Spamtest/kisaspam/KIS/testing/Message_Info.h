/*!
 * Kaspersky lab application info file
 * Copyright (C) 2005 Kaspersky Labs.
 *
 * \file DataBase.h
 * \author Vitaly Denisov
 * \date 2005
 * \brief message classes for KIS-interface
 *
 */

#ifndef  _MESSAGE_INFO_H
#define  _MESSAGE_INFO_H


#ifndef pgMESSAGE_PART_CONTENT_LENGTH
    #define pgMESSAGE_PART_CONTENT_LENGTH   0
#endif
#ifndef pgMESSAGE_INTERNET_HEADERS
    #define pgMESSAGE_INTERNET_HEADERS   0
#endif


class INFO
{
public:
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
	// GetData возвращает указатель на поток данных.
	// Указатель жив, пока не вызвано ReleaseData и жив объект MESSAGEPART_INFO. 
	// Указатель должен быть освобожден вызовом ReleaseData.
	virtual IStream* GetData() = 0;
	virtual void ReleaseData();
};

class MESSAGE_INFO : public INFO
{
public:
    // GetMessagePart возращает указатель на MESSAGEPART_INFO по порядковому номеру num
	// Указатель должен быть освобожден вызовом ReleaseMessagePart
    virtual MESSAGEPART_INFO* GetMessagePart(unsigned long num) = 0;
	virtual void ReleaseMessagePart(MESSAGEPART_INFO* pMessagePart) = 0;
};

#endif//_MESSAGE_INFO
