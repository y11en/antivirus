// StringTokenizer.h: interface for the CStringTokenizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGTOKENIZER_H__533C17C7_B86C_44B9_851F_EA97828D2558__INCLUDED_)
#define AFX_STRINGTOKENIZER_H__533C17C7_B86C_44B9_851F_EA97828D2558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

template<typename CHAR_TYPE>
class BasicStringTokenizer  
{
public:
	typedef std::basic_string<CHAR_TYPE> string_t;

	BasicStringTokenizer(const CHAR_TYPE* szSource, const CHAR_TYPE* szDelimiters /* = _T(" ,\t\r\n")*/)
	{
		Init(szSource, szDelimiters);
	}

	bool Init(const CHAR_TYPE* szSource, const CHAR_TYPE* szDelimiters)
	{
		if (!szSource || !szDelimiters)
			return false;
		
		m_strSource = szSource;
		m_strDelimit = szDelimiters;
		m_nPos = 0;
		return true;
	}

	bool GetNext(OUT string_t &strToken)
	{
		m_nPos = m_strSource.find_first_not_of(m_strDelimit, m_nPos);
		if (m_nPos == string_t::npos)	// no more valid characters in source string
			return FALSE;

		string_t::size_type nEndPos = m_strSource.find_first_of(m_strDelimit, m_nPos);

		if (nEndPos == string_t::npos)
			strToken.assign(m_strSource.begin() + m_nPos, m_strSource.end());
		else
			strToken.assign(m_strSource.begin() + m_nPos, m_strSource.begin() + nEndPos);
		
		m_nPos = nEndPos;
		return true;
	}

private:
	string_t		m_strSource;
	string_t		m_strDelimit;
	size_t			m_nPos;	// starting position for tokenizing
};

typedef BasicStringTokenizer<TCHAR> CStringTokenizer;
typedef BasicStringTokenizer<wchar_t> CStringTokenizerW;

#endif // !defined(AFX_STRINGTOKENIZER_H__533C17C7_B86C_44B9_851F_EA97828D2558__INCLUDED_)
