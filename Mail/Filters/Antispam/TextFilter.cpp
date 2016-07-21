// TextFilter.cpp: implementation of the CTextFilter class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4786)

#include "stdafx.h"
#include "stl_port.h"
#include "TextFilter.h"
#include "toupper.h"
#include "fill_stop_words.h"
#include "text_normalize.h"


#define MAX_SIG_LINES	5
#define MAX_SIG_LEN	300

size_t CutSignature(const char* szString)
{
	char c;
	const char* p;
	const char* s = szString;
	size_t nSize;
	size_t nSigLen = 0;
	size_t nLineLen = 0;
	size_t nLinesCount = 0;
	bool bLine = false;
	
	if (!s)
		return 0;
	if (!*s)
		return 0;
	nSize = strlen(s);
	p=s+nSize;
	while (--p != s)
	{
		c = *p;
		if  ( c==0x20 || c==0x09 )
			; // ignore
		else if ( c==0x0A || c==0x0D )
		{
			if (bLine)
			{
				nLinesCount = 0;
				nSigLen = 0;
				nSize  = p-s+1;
			}
			else if (nLineLen)
			{
				nSigLen += nLineLen;
				if (nSigLen > MAX_SIG_LEN)
					break;
				nLinesCount++;
				if (nLinesCount > MAX_SIG_LINES)
					break;
			}
			nLineLen = 0;
			bLine = false;
		}
		else if ( c=='_' || c=='-' || c=='=' || c=='~' )
		{
			if (nLineLen == 0)
				bLine = true;
		}
		else
		{
			bLine = false;
			nLineLen++;
		}
	}; // while (--p != s)
	return nSize;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextFilter::CTextFilter()
{
	FILL_STOP_WORDS(m_stop_words);
}

CTextFilter::~CTextFilter()
{

}

void CTextFilter::Process(char* szText, results_t& results)
{
	size_t sign_pos = CutSignature(szText);
	if ( stl_port::strlen(szText) - sign_pos > 100 ) 
		szText[sign_pos] = '\0';
	static cdelimeter delimeter(" ,:;=()[]/!?\t\r\n\"'");
	ProcessText(szText, &delimeter, results);
}

void CTextFilter::normalize_per_language(IN OUT char* token, size_t token_length, results_t& results)
{
	PrepareWord(token, token_length, true);
	if ( results.szCharset == "WINDOWS-1251" ) 
	{
		// Если длина токена больше 4 символов
		if ( token_length > 4 )
		{
			// Ищем последнюю согласную букву в токене
			static cdelimeter delimeter("бвгджзклмнпрстфхцчшщ");
			size_t termination_pos = stl_port::find_last_of(token, &delimeter, token_length);
			if ( 
				termination_pos != STRING_NPOS &&
				termination_pos > 3
				)
				// Отрезаем окончания, состоящие целиком из гласных букв
				token[termination_pos+1] = '\0';
		}
	}
}

bool CTextFilter::delete_doubled_chars(IN char* token)
{
	bool bResult = false;
	// Не трогаем токены-цифры
	if ( stl_port::find_first_not_of(token, "0123456789") != STRING_NPOS ) 
	{
		unsigned char c, n;
		char *end;
		while(c = *token++)
		{
			end = token;
			while((n = *end) && c == n)
				end++;
			if(end != token)
			{
				stl_port::erase(token, 0, end - token);
				bResult = true;
			}
		}
	}
	return bResult;
}

bool CTextFilter::find_rus_lat(IN char* token, results_t& results)
{
	// - Латинские буквы живут под номерами 65-122
	// - Национальные буквы живут под номерами 128-255
	bool bLat = false;
	bool bRus = false;
	unsigned char c;
	while(c = *token++)
	{
		if(c > 65 && c < 122)
		{
			results.ulLatChars++;
			bLat = true;
		}
		if(c > 128 && c < 255) 
		{
			results.ulRusChars++;
			bRus = true;
		}
		if(bLat && bRus)
			return true;
	}
	return false;
}

void CTextFilter::summ_upper_and_lower_chars(IN OUT char* token, results_t& results)
{
	unsigned long ulUpperChars = 0;
	unsigned long ulLowerChars = 0;
	unsigned long tmpi = 0;
	unsigned char c;
	const char *p = token;
	while(c = *p++)
	{
		if(isalpha(c))
		{
			(c & 0x20) ? ulLowerChars++ : ulUpperChars++;
			tmpi++;
		}
	}
	(ulUpperChars > 1) ? ToUpper(token) : ToLower(token);
	results.ulUpperChars += ulUpperChars;
	results.ulLowerChars += ulLowerChars;
	results.ulChars += tmpi;
}

bool CTextFilter::CheckToken(IN char* token, OUT results_t& results)
{
	size_t token_length = stl_port::strlen(token);

	if ( !token_length )
		return false;

	// Считаем трики с @ (если после единственной @ в токене встречается точка - то это почтовый адрес, а не трик)
	{
		static cdelimeter delimeter("@");
		static cdelimeter delimeter_dot(".");
		size_t pos_sobaka1 = stl_port::find_first_of( token, &delimeter);
		size_t pos_dot = stl_port::find_last_of( token, &delimeter_dot, token_length);
		if ( pos_sobaka1 != STRING_NPOS )
		{
			if ( (pos_dot != STRING_NPOS) && (pos_dot > pos_sobaka1) ) 
			{
				// Это SMTP-адрес
				// Отцепим от адреса знаки '<' и '>', если они есть
				static cdelimeter delimeter_less("<");
				static cdelimeter delimeter_more(">");
				size_t pos_less = stl_port::find_first_of( token, &delimeter_less );
				size_t pos_more = stl_port::find_first_of( token, &delimeter_more );
				if ( (pos_less == 0) && (pos_more == token_length-1) ) 
				{
					token[pos_more] = '\0';
					token[pos_less] = '\0';
					token = token + pos_less + 1;
				}
				results.ulSMTPAddress++;
				ToUpper(token);
				results.tokens.push_back(token_t(token));
				return true;
			}
			else
			{
				// Это трик с @
				results.ulTextTricks++;
//				return false;
			}
		}
		else
		{
			if ( pos_dot != STRING_NPOS ) 
			{
				static cdelimeter delimeter(".");
				return ProcessText(token, &delimeter, results);
			}
		}
	}

	// Игнорируем стоп-слова
	if ( m_stop_words.find(token) != m_stop_words.end() ) 
		return false;

//	// Игнорируем "служебные" слова (например, мат.выражения). 
//	static cdelimeter delimeter("<>=#&*+-{}~^`");
//	if ( stl_port::find_first_of( token, &delimeter) != STRING_NPOS ) // 26.07.04 removed %
//		return false;

	// Подсчитаем количество букв в верхнем и нижнем регистре
	summ_upper_and_lower_chars(token, results);

	// Нормализуем удвоенные символы ("дребезг клавиатуры")
	delete_doubled_chars(token);

//	// Игнорируем слишком маленькие токены
//	if ( token_length < 3 ) 
//		return false;

	// Считаем слова, в которых встречаются спецсимволы, употребимые в спам-триках
	static cdelimeter delimeter2("$|`");
	if ( stl_port::find_first_of(token, &delimeter2) != STRING_NPOS ) 
	{
		results.ulTextTricks++;
//		return false;
	}

	// Посчитаем слова, в которых перемешаны национальные и латинские буквы
	if ( find_rus_lat(token, results) ) 
	{
		results.ulRusLatTricks++;
//		return false;
	}

	// Нормализуем слова в зависимости от языка
	normalize_per_language(token, token_length, results);

	results.tokens.push_back(token_t(token));

	return true;
}

bool CTextFilter::ProcessText( IN char* szText, const cdelimeter* pdelimeter, OUT results_t& results)
{
	bool bTextWasParsed = false;
	size_t pos_begin = 0;
	size_t pos_end = 0;
	size_t text_length = stl_port::strlen(szText);

	if ( !text_length )
		return false;

	pos_end = stl_port::find_first_of(szText, pdelimeter);
	if ( pos_end == STRING_NPOS )
		pos_end = text_length;

	szText[pos_end] = '\0';
	bTextWasParsed = CheckToken(szText, results);

	if ( pos_end == text_length )
		return bTextWasParsed;

	char* szCurrentPos = szText + pos_end + 1;

	do 
	{
		pos_end = stl_port::find_first_of(szCurrentPos, pdelimeter);
		if ( pos_end != STRING_NPOS )
			szCurrentPos[pos_end] = '\0';
		if ( stl_port::strlen(szCurrentPos) ) 
			CheckToken(szCurrentPos, results);
		if ( pos_end != STRING_NPOS )
			szCurrentPos += pos_end + 1;
		else 
			szCurrentPos = szText + text_length;
	} 
	while ( szCurrentPos < (szText + text_length) );

	return bTextWasParsed;
}