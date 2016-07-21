#ifndef _GET_ADDRESS_FROM_STRING_H_
#define _GET_ADDRESS_FROM_STRING_H_

#include <string>

#ifndef IN
#define IN   
#endif
#ifndef OUT
#define OUT  
#endif

inline void StrToUpper( IN OUT std::string& str )
{
	for (size_t 
		i = 0;
		i < str.length();
		i++
		)
		if ( isascii(str[i]) )
			str[i] = toupper(str[i]);
}

inline void GetAddressFromString( IN std::string sz, OUT std::string& szAddress )
{
	//////////////////////////////////////////////////////////////////////////
	//
	// Вынимаем адрес из строки. Если там стоит префикс SMTP: - игнорируем его
	//
	
	int address_end = sz.rfind( ">" );
	if ( address_end == std::string::npos )
		address_end = sz.length();
	
	int address_begin = sz.rfind( "<", address_end-1 );
	if ( address_begin == std::string::npos )
		address_begin = 0;
	else
		address_begin++;

	StrToUpper(sz);

	if ( sz.find( "SMTP:" ) != std::string::npos )
		address_begin += 5;
	szAddress = sz.substr(address_begin, address_end-address_begin);
	if ( szAddress == ":" ) 
		szAddress = "";
	if ( szAddress.substr(0, 4) == ":/O=" )
		szAddress.insert(0, "EX");


	//
	// Вынимаем адрес из строки
	//
	//////////////////////////////////////////////////////////////////////////
}

#endif//_GET_ADDRESS_FROM_STRING_H_