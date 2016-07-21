#include "kl_string.h"

template <class T> 
T* CKl_SimpleString<T>::Find(T* Str)
{
	return NULL;
}

template <class T>
size_t CKl_SimpleString<T>::Length()
{
	return m_length;
}

template <class T>
size_t CKl_SimpleString<T>::Size()
{
	return m_size;
}

template <class T>
T& CKl_SimpleString<T>::operator [] (size_t i )
{
	return m_buffer[i];
}
//---------------------------------------------------------

unsigned char*
CKl_AString::Find( unsigned char* Str )
{
	return (unsigned char*)strstr( (const char*)m_buffer, (const char*)Str );
}