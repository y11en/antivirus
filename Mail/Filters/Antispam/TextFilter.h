// TextFilter.h: interface for the CTextFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTFILTER_H__0C12DD32_0184_46A0_8912_164946CA67D2__INCLUDED_)
#define AFX_TEXTFILTER_H__0C12DD32_0184_46A0_8912_164946CA67D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <map>

using namespace std;

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

#include "stl_port.h"

class CTextFilter  
{
public:
	CTextFilter();
	virtual ~CTextFilter();

	struct token_t
	{
		char* token1;
		char* token2;

		token_t(char* _token1, char* _token2 = 0):
			token1(_token1),
			token2(_token2)
			{;};
	};

	typedef vector<token_t> tokens_t;
	struct results_t 
	{
		results_t(){clear();}

		unsigned long ulTextTricks;			// Количество неклассифицированных триков в тексте
		unsigned long ulLatChars;			// Количество символов из первой половины таблицы ANSI
		unsigned long ulRusChars;			// Количество символов из второй половины таблицы ANSI
		unsigned long ulRusLatTricks;		// Количество триков со смешением букв различных алфавитов
		unsigned long ulSMTPAddress;		// Количество адресов в тексте
		unsigned long ulUpperChars;			// Количество букв верхнего регистра во всех токенах
		unsigned long ulLowerChars;			// Количество букв нижнего регистра во всех токенах
		unsigned long ulChars;				// Общее количество букв в токенах
		std::string   szCharset;			// Кодировка документа
		tokens_t tokens;					// Вектор токенов

		void clear()
		{
			ulTextTricks = 0;
			ulLatChars = 0;
			ulRusChars = 0;
			ulRusLatTricks = 0;
			ulSMTPAddress = 0;
			ulUpperChars = 0;
			ulLowerChars = 0;
			ulChars = 0;
			szCharset.erase();
			tokens.clear();
		};
	};
	
	void Process(char* szText, results_t& results);

private:
	void normalize_per_language(IN OUT char* token, size_t token_length, results_t& results);
	static bool delete_doubled_chars(IN char* token);
	bool find_rus_lat(IN char* token, results_t& results);
	void summ_upper_and_lower_chars(IN OUT char* token, results_t& results);
	bool CheckToken(IN char* token, OUT results_t& results);
	bool ProcessText( IN char* szText, const cdelimeter* pdelimeter, OUT results_t& results);
	typedef map<string, int> stop_words_t;
	stop_words_t m_stop_words;
};

#endif // !defined(AFX_TEXTFILTER_H__0C12DD32_0184_46A0_8912_164946CA67D2__INCLUDED_)
