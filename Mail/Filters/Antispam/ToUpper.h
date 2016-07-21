#ifndef _TOUPPER_H_
#define _TOUPPER_H_

#include "stl_port.h"
inline void ToUpper(char* str)
{
	_strupr(str);
}

inline void ToLower(char* str)
{
	_strlwr(str);
}

#include <string>
inline void ToUpper(std::string& str)

{
	std::string::iterator _F = str.begin(), _L = str.end();
	for(; _F != _L; _F++)
		*_F = toupper(*_F);
}

inline void ToLower(std::string& str)
{
	std::string::iterator _F = str.begin(), _L = str.end();
	for(; _F != _L; _F++)
		*_F = tolower(*_F);
}

#endif//_TOUPPER_H_
