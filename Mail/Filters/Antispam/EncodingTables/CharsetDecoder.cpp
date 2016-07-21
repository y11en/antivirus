// CharsetDecoder.cpp: implementation of the CCharsetDecoder class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4786)
#include "CharsetDecoder.h"
#include "japan/decoder.h"
#include <windows.h>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCharsetDecoder::CCharsetDecoder()
{

}

CCharsetDecoder::~CCharsetDecoder()
{

}

void CCharsetDecoder::AddCharset(const char* charset_name, BYTE* charset)
{
	m_known_charsets[charset_name] = charset; //decode_table_from_oem(charset);
}


bool CCharsetDecoder::Decode(
							 const char* input, 
							 string& output,
							 const char* charset_name_FROM, 
							 const char* charset_name_TO 
							 )
{
	if(charset_name_FROM && (
		0 == strcmp(charset_name_FROM, "UTF-16") ||
		0 == strcmp(charset_name_FROM, "EUC-JP") ||
		0 == strcmp(charset_name_FROM, "ISO-2022-JP") ||
		0 == strcmp(charset_name_FROM, "SHIFT-JIS")) &&
		decode_japanese(input, output)
		)
	{
		decode_utf(output.c_str(), output, true);
		return true;
	}

	if ( charset_name_FROM && (0==strcmp(charset_name_FROM, "IBM866")) ) 
	{
		decode_oem(input, output);
		return true;
	}
	if ( charset_name_FROM && (0==strcmp(charset_name_FROM, "UTF-8")) ) 
	{
		decode_utf(input, output, true);
		return true;
	}
	if ( charset_name_FROM && (0==strcmp(charset_name_FROM, "UTF-7")) ) 
	{
		decode_utf(input, output, false);
		return true;
	}

	BYTE decodeTable[256];	
	known_charsets_t::const_iterator iTable2 = m_known_charsets.find(charset_name_FROM);
	
	if ( charset_name_TO ) 
	{
		known_charsets_t::const_iterator iTable1 = m_known_charsets.find(charset_name_TO);
		if ( 
			iTable1 == m_known_charsets.end() ||
			iTable2 == m_known_charsets.end() 
			) 
			return false;

		BYTE* Table1 = iTable1->second;
		BYTE* Table2 = iTable2->second;
		unsigned long i = 0;
		while ( i < 256 ) 
		{
			decodeTable[ Table1[i] ] = Table2[i];
			i++;
		}
	}
	else if ( iTable2 != m_known_charsets.end() ) 
	{
		unsigned long i = 0;
		while ( i < 256 ) 
		{
			decodeTable[ i ] = iTable2->second[i];
			i++;
		}
	}
	else
	{
		output = input;
		return false;
	}

	decode(input, decodeTable, output);

	return true;
}

void CCharsetDecoder::decode(const char* input, BYTE* decodeTable, string& output)
{
	output.reserve(output.size() + strlen(input));
	unsigned char c, d;
	while(c = *input++) 
	{
		d = decodeTable[c];
		output += d ? d : c;
	}
}

void CCharsetDecoder::decode_oem(const char* input, string& output)
{
	output.reserve(output.size() + strlen(input));
	char c;
	while(c = *input++)
		output += decode_char_from_oem(c);
}

char& CCharsetDecoder::decode_char_from_oem(char& _char)
{
	char c[2] = {_char,0};
	char c1[2] = {0,0};
	if ( OemToChar(c, c1) )
		_char = c1[0];
	return _char;
}

char& CCharsetDecoder::decode_char_to_oem(char& _char)
{
	char c[2] = {_char,0};
	char c1[2] = {0,0};
	if ( CharToOem(c, c1) )
		_char = c1[0];
	return _char;
}

BYTE* CCharsetDecoder::decode_table_from_oem(BYTE* charset)
{
	unsigned long i = 0;
	while ( i < 256 ) 
	{
		decode_char_from_oem((char&)charset[i]);
		i++;
	}
	return charset;
}

void CCharsetDecoder::decode_utf(const char* input, string& output, bool bUtf8)
{
	std::vector<wchar_t> strWC(strlen(input) + 1);
	if(MultiByteToWideChar(bUtf8 ? CP_UTF8 : CP_UTF7, 0, input, -1, &strWC[0], strWC.size()) != 0)
	{
		int size = WideCharToMultiByte(CP_ACP, 0, &strWC[0], -1, NULL, 0, 0, 0);
		if (size != 0)
		{
			std::vector<char> strMB(size);
			if (WideCharToMultiByte(CP_ACP, 0, &strWC[0], -1, &strMB[0], strMB.size(), 0, 0) != 0)
				output = &strMB[0];
		}
		else
			output = "";
	}
	else
		output = "";
}