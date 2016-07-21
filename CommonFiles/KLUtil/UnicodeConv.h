// UnicodeConv.h: interface for the UnicodeConv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNICODECONV_H__0A589594_68BE_4C9B_B342_E0E110313890__INCLUDED_)
#define AFX_UNICODECONV_H__0A589594_68BE_4C9B_B342_E0E110313890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma warning(disable: 4001)    // Single line comment 
#include "defs.h"
#include <string>
#include <stdio.h>

namespace KLUTIL
{

template <typename _TYPE, size_t _BUFFSIZE> struct StackBuffer
{
	StackBuffer() : m_pMemPtr(NULL), m_bFreeMemPtr(false){}
	virtual ~StackBuffer(){ if (m_bFreeMemPtr) free (m_pMemPtr); m_pMemPtr = NULL;}
	
	_TYPE* alloc_buffer(size_t need_size)
	{ 
		if (m_bFreeMemPtr)
			free (m_pMemPtr);

		m_pMemPtr = NULL;
		m_bFreeMemPtr = false;
		
		if (need_size <= sizeof (m_Buff)/sizeof(_TYPE))
			return m_Buff;
		
		m_AllocSize = need_size;
		m_bFreeMemPtr = true;
		return m_pMemPtr = (_TYPE*)malloc(need_size*sizeof(_TYPE));
	}

	void* alloc_buffer_in_bytes(size_t need_size)
	{
		return alloc_buffer (need_size%sizeof(_TYPE)?(need_size/sizeof(_TYPE))+1:need_size/sizeof(_TYPE));
	}
	
	void set_buffer(const _TYPE* pBuff, size_t buff_size = 0)
	{
		if (m_bFreeMemPtr)
			free (m_pMemPtr);
		
		m_pMemPtr = (_TYPE*)pBuff;
		m_AllocSize = buff_size;
		m_bFreeMemPtr = false;
	}
	
	_TYPE* get_buffer()
	{
		return m_pMemPtr?m_pMemPtr:m_Buff;
	}
	
	size_t get_buffer_size() const
	{
		return m_pMemPtr?m_AllocSize:sizeof (m_Buff)/sizeof(_TYPE);
	}

	size_t get_buffer_size_in_bytes() const
	{
		return m_pMemPtr?m_AllocSize*sizeof(_TYPE):sizeof (m_Buff);
	}
	
	
	
protected:
	_TYPE m_Buff[_BUFFSIZE];
	_TYPE* m_pMemPtr;
	size_t m_AllocSize;
	bool	m_bFreeMemPtr;
};

struct StringStackBufferA : public StackBuffer<char, MAX_PATH>
{
	StringStackBufferA() { m_Buff[0] = '\0';}
	void Format(LPCSTR formatstring, ...) 
	{
		va_list args;
		va_start(args, formatstring);
		
		while ( _vsnprintf( (char*)get_buffer  (), get_buffer_size  ()/sizeof(char), formatstring, args) == -1 )
			alloc_buffer  (get_buffer_size  () + MAX_PATH);
	}

	bool IsEmptyString(){ return *get_buffer  () == '\0';}
};

struct StringStackBufferW : public StackBuffer<wchar_t, MAX_PATH*sizeof(wchar_t)>
{
	StringStackBufferW() { m_Buff[0] = L'\0';}
	void Format(LPCWSTR formatstring, ...) 
	{
		va_list args;
		va_start(args, formatstring);
		
		while( _vsnwprintf( (WCHAR*)get_buffer  (), get_buffer_size  ()/sizeof(WCHAR), formatstring, args) == -1 )
			alloc_buffer  (get_buffer_size  () + sizeof(WCHAR)*256);
	}
	
	bool IsEmptyString(){ return *get_buffer  () == L'\0';}
};


inline void InitAndConvert_W2A(StringStackBufferA& This, const WCHAR* pwsz)
{
	if (pwsz == NULL)
		return;
	
	
	int nRet = ::WideCharToMultiByte(CP_ACP, 0, pwsz, -1, This.get_buffer (), This.get_buffer_size  (), NULL, NULL );
	
	
	if((nRet==0) && (GetLastError  () == ERROR_INSUFFICIENT_BUFFER))
	{
		nRet = ::WideCharToMultiByte(CP_ACP, 0, pwsz, -1, NULL, 0, NULL, NULL );
		if (nRet)
			::WideCharToMultiByte(CP_ACP, 0, pwsz, -1, This.alloc_buffer  (nRet), nRet, NULL, NULL );
	}
}

inline void InitAndConvert_A2W(StringStackBufferW& This, const char* pwsz)
{
	if (pwsz == NULL)
		return;
	
	int nRet = ::MultiByteToWideChar(CP_ACP, 0, pwsz, -1, This.get_buffer (), This.get_buffer_size ());

	if((nRet==0) && (GetLastError  () == ERROR_INSUFFICIENT_BUFFER))
	{
		nRet = ::MultiByteToWideChar(CP_ACP, 0, pwsz, -1, NULL, 0);
		if (nRet)
			::MultiByteToWideChar(CP_ACP, 0, pwsz, -1, This.alloc_buffer (nRet), nRet);
	}
}


struct str_base_conversion_to_char : public StringStackBufferA
{
	explicit str_base_conversion_to_char(const WCHAR* pwsz){ InitAndConvert_W2A(*this, pwsz);}
	explicit str_base_conversion_to_char(const std::wstring& pwsz){ InitAndConvert_W2A(*this, pwsz.c_str  ());}
	explicit str_base_conversion_to_char(const char* pwsz){set_buffer  (pwsz);}
	explicit str_base_conversion_to_char(const std::string& pwsz){set_buffer  (pwsz.c_str  ());}
};



/////////////////////////////////

struct str_base_conversion_to_wchar : public StringStackBufferW
{
	explicit str_base_conversion_to_wchar(const char* pwsz){ InitAndConvert_A2W(*this, pwsz);}
	explicit str_base_conversion_to_wchar(const std::string& pwsz){ InitAndConvert_A2W(*this, pwsz.c_str  ());}

	explicit str_base_conversion_to_wchar(const WCHAR* pwsz){set_buffer  (pwsz);}
	explicit str_base_conversion_to_wchar(const std::wstring& pwsz){set_buffer  (pwsz.c_str  ());}
};


#define __LPSTR(str)	(KLUTIL::str_base_conversion_to_char(str).get_buffer())
#define __LPCSTR(str)	__LPSTR(str)

#ifdef _UNICODE
	#define StringStackBufferT	KLUTIL::StringStackBufferW
	#define InitAndConvert_A2T(stack_buff, str)	KLUTIL::InitAndConvert_A2W(stack_buff, str)
	#define InitAndConvert_W2T(stack_buff, str)	(str)
	#define __LPTSTR(str)	(KLUTIL::str_base_conversion_to_wchar(str).get_buffer())
	#define __LPCTSTR(str)	__LPTSTR(str)
#else
	#define StringStackBufferT	KLUTIL::StringStackBufferA
	#define InitAndConvert_A2T(stack_buff, str)	(str)
	#define InitAndConvert_W2T(stack_buff, str)	KLUTIL::InitAndConvert_W2A(stack_buff, str)
	#define __LPTSTR(str)	(KLUTIL::str_base_conversion_to_char(str).get_buffer())
	#define __LPCTSTR(str)	__LPTSTR(str)
#endif

#define __LPWSTR(str)	(KLUTIL::str_base_conversion_to_wchar(str).get_buffer())
#define __LPCWSTR(str)	__LPWSTR(str)

} // namespace KLUTIL

#endif // !defined(AFX_UNICODECONV_H__0A589594_68BE_4C9B_B342_E0E110313890__INCLUDED_)
