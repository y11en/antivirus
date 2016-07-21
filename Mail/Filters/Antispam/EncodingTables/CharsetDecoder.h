// CharsetDecoder.h: interface for the CCharsetDecoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARSETDECODER_H__FEE6E779_DC1E_4BF2_A8F1_CDF9F6B496B3__INCLUDED_)
#define AFX_CHARSETDECODER_H__FEE6E779_DC1E_4BF2_A8F1_CDF9F6B496B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>

using namespace std;

typedef unsigned char BYTE;

class CCharsetDecoder  
{
public:
	CCharsetDecoder();
	virtual ~CCharsetDecoder();

	typedef vector<BYTE> charset_t;

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: AddCharset
	//! \brief			:	Добавляет набор символов к известным наборам
	//!						Внимание! Изменяет исходный набор символов, преобразуя его (без проверок) OEM->WIN
	//! \return			: void 
	//! \param          : char* charset_name
	//! \param          : charset_t charset
	void AddCharset(const char* charset_name, BYTE* charset);

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Decode
	//! \brief			:	Декодирует текст в направлении charset_name_FROM->charset_name_TO
	//! \return			: bool 
	//! \param          : const char* input
	//! \param          : string& output
	//! \param          : const char* charset_name_FROM
	//! \param          : const char* charset_name_TO = "WINDOWS-1251"
	bool Decode(
		const char* input, 
		string& output,
		const char* charset_name_FROM, 
		const char* charset_name_TO = 0
		);

private:
	typedef map<std::string, BYTE*> known_charsets_t;
	known_charsets_t m_known_charsets; // Здесь мы будем хранить известные вектора перекодировки
	typedef map<char, char> decodeTable_t;

	void decode(const char* input, BYTE* decodeTable, string& output);
	void decode_oem(const char* input, string& output);
	char& decode_char_from_oem(char& _char);
	char& decode_char_to_oem(char& _char);
	BYTE* decode_table_from_oem(BYTE* charset);
	void decode_utf(const char* input, string& output, bool bUtf8);

};

#endif // !defined(AFX_CHARSETDECODER_H__FEE6E779_DC1E_4BF2_A8F1_CDF9F6B496B3__INCLUDED_)
