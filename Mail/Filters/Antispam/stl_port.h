#ifndef _STL_PORT_H_
#define _STL_PORT_H_

#include <string.h>

#define STRING_NPOS -1

class cdelimeter
{
public:
	cdelimeter(char* pch)
	{
		memset(buf, 0, sizeof(buf));
		while ((unsigned char)(*pch))
		{
			buf[(unsigned char)(*pch)] = 1;
			pch++;
		}
	}

	~cdelimeter()
	{
	}

	char buf[256];
};

namespace stl_port
{
	inline size_t strlen(const char* token)
	{
		if ( token && token[0] )
			return ::strlen(token);
		else
			return 0;
	}

	void erase(char* token, size_t pos, size_t count);
	
	size_t find_first_not_of(char* token, char* delimeters, size_t pos = 0);

	size_t find_first_of(char* token, const cdelimeter* pdelimeters);

	size_t find_last_of(char* token, const cdelimeter* pdelimeters, size_t pos = 0);
}

#endif//_STL_PORT_H_