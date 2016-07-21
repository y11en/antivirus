#ifndef _KL_STRING_H_
#define _KL_STRING_H_

#include "kl_buffer.h"
#include "kl_list.h"

// классы-хелперы для работы со строчками

template <class T>
class CKl_SimpleString
{
public:
    T*          m_buffer;		// буффер строки.
    size_t      m_size;			// размер буфера
    size_t      m_length;		// кол-во символов строки без учета терминирующего нуля
    

    virtual T*          Find( T*  Str );        // поиск подстроки
    
	size_t      Size();
    size_t      Length();

    T&  operator[] (size_t  i);

    CKl_SimpleString ( T* Str = NULL );
    virtual ~CKl_SimpleString();
};

// ansi строка
class CKl_AString : public CKl_SimpleString<unsigned char>
{
public:
	virtual unsigned char* Find( unsigned char*  Str );        // поиск подстроки    

    CKl_AString( unsigned char*   Str = NULL );
    virtual ~CKl_AString();
};

// unicode строка
class CKl_UString : public CKl_SimpleString<unsigned short>
{
public:
    CKl_UString( wchar_t* Str = NULL );
    virtual ~CKl_UString();
};


#endif