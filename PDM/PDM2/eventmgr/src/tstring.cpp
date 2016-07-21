#include <assert.h>
#include "../include/types.h"
#include "../include/tstring.h"

tstring tstralloc( size_t chars )
{
	return new tchar[chars+1];
}

void tstrfree ( tcstring string )
{
	if (string)
		delete [] (tstring)string;
}


size_t tstrlen(tcstring string)
{
	size_t len = 0; 
	tcstring string_ptr = string;
	while (tstrchar(string_ptr))
	{
		len++;
		string_ptr = tstrinc(string_ptr);
	};
	return len;
}

size_t tstrlenb(tcstring string)
{
	size_t len = 0; 
	tcstring string_ptr = string;
	while (tstrchar(string_ptr))
	{
		len += tcharlenb(string_ptr);
		string_ptr = tstrinc(string_ptr);
	};
	return len;
}

size_t tstrlenbz(tcstring string)
{
	size_t len = 0; 
	tcstring string_ptr = string;
	while (tstrchar(string_ptr))
	{
		len += tcharlenb(string_ptr);
		string_ptr = tstrinc(string_ptr);
	};
	len += tcharlenb(string_ptr);
	return len;
}

tcstring tstrgetlastchar(tcstring string)
{
	tcstring last_ptr = NULL;
	tcstring string_ptr = string;
	while (tstrchar(string_ptr))
	{
		last_ptr = string_ptr;
		string_ptr = tstrinc(string_ptr);
	};
	return last_ptr;
}

tstring tstrcpy( tstring dst, tcstring src )
{
	assert(src && dst);
	tstring string1_ptr = dst;
	tcstring string2_ptr = src;
	tchar c;
	do {
		c = tstrchar(string2_ptr);
		tstrchar(string1_ptr) = c;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
	} while (c);
	return dst;
}

tstring tstrncpy( tstring dst, tcstring src, size_t size )
{
	assert(src && dst);
	tstring string1_ptr = dst;
	tcstring string2_ptr = src;
	tchar c;
	size_t copied = 0;
	do {
		c = tstrchar(string2_ptr);
		tstrchar(string1_ptr) = c;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
		copied++;
		if (copied == size)
			break;
	} while (c);
	return dst;
}

tstring tstrcat( tstring dst, tcstring src )
{
	assert(src && dst);
	tstring string1_ptr = dst;
	tcstring string2_ptr = src;
	tchar c;
	while (tstrchar(string1_ptr))
		string1_ptr = tstrinc(string1_ptr);
	do {
		c = tstrchar(string2_ptr);
		tstrchar(string1_ptr) = c;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
	} while (c);
	return dst;
}

int tstrcmp( tcstring string1, tcstring string2 )
{
	assert(string1 && string2);
	if (!string1)
		return -1;
	if (!string2)
		return 1;
	tcstring string1_ptr = string1;
	tcstring string2_ptr = string2;
	tchar c, c2;
	do {
		c = tstrchar(string1_ptr);
		c2 = tstrchar(string2_ptr);
		if (c < c2)
			return -1;
		if (c > c2)
			return 1;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
	} while (c);
	return 0;
}

int tstrcmpbegin( tcstring string1, tcstring strbegin )
{
	assert(string1 && strbegin);
	if (!string1)
		return -1;
	if (!strbegin)
		return 1;
	tcstring string1_ptr = string1;
	tcstring string2_ptr = strbegin;
	tchar c, c2;
	do {
		c2 = tstrchar(string2_ptr);
		if (c2 == 0)
			return 0;
		c = tstrchar(string1_ptr);
		if (c < c2)
			return -1;
		if (c > c2)
			return 1;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
	} while (c);
	return 0;
}

bool tstrcmpinc( tcstring* pstring1, tcstring strcmp )
{
	assert(pstring1 && *pstring1 && strcmp);
	if (!pstring1)
		return false;
	if (!*pstring1)
		return false;
	if (!strcmp)
		return false;
	tcstring string1_ptr = *pstring1;
	tcstring string2_ptr = strcmp;
	tchar c, c2;
	do {
		c2 = tstrchar(string2_ptr);
		if (c2 == 0)
		{
			*pstring1 = string1_ptr;
			return true;
		}
		c = tstrchar(string1_ptr);
		if (c < c2)
			return false;
		if (c > c2)
			return false;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
	} while (c);
	return false;
}

int tstrncmp( tcstring string1, tcstring string2, size_t count )
{
	assert(string1 && string2 && count);
	if (!string1)
		return -1;
	if (!string2)
		return 1;
	size_t len = 0;
	tcstring string1_ptr = string1;
	tcstring string2_ptr = string2;
	tchar c, c2;
	do {
		c = tstrchar(string1_ptr);
		c2 = tstrchar(string2_ptr);
		if (c < c2)
			return -1;
		if (c > c2)
			return 1;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
		len++;
	} while (c && len < count);
	return 0;
}

tstring tstrupr( tstring string )
{
	assert(string);
	tchar c;
	tstring ptr = string;
	do {
		c = tstrchar(ptr);
		if (c >= 'a' && c <= 'z')
			tstrchar(ptr) = c & (~0x20);
		ptr = tstrinc(ptr);
	} while (c);
	return string;
}

tcstring tstrchr( tcstring string, tchar s )
{
	assert(string && s);
	tchar c;
	tcstring ptr = string;
	do {
		c = tstrchar(ptr);
		if (c == s)
			return ptr;
		ptr = tstrinc(ptr);
	} while (c);
	return NULL;
}

tcstring tstrrchr( tcstring string, tchar s )
{
	assert(string && s);
	tchar c;
	tcstring ptr = string;
	tcstring result = NULL;
	do {
		c = tstrchar(ptr);
		if (c == s)
			result = ptr;
		ptr = tstrinc(ptr);
	} while (c);
	return result;
}

tcstring tstrstr( tcstring string1, tcstring string2 )
{
	assert(string1 && string2 );
	tcstring ptr = string1;
	tchar c = tstrchar(string2);
	size_t len = tstrlen(string2);
	if (0 == c)
		return NULL;
	while (ptr = tstrchr(ptr, c))
	{
		if (0 == tstrncmp(ptr, string2, len))
			return ptr;
		ptr = tstrinc(ptr);
	};
	return NULL;
}

// this function will realloc string!!!
tstring tstrappend(tstring string, tcstring append)
{
	assert(string && append );
	tstring new_str = tstralloc( tstrlen(string) + tstrlen(append) + sizeof(tchar) );
	if (!new_str)
		return NULL;
	tstrcpy(new_str, string);
	tstrcat(new_str, append);
	tstrfree(string);
	return new_str;
}

tstring tstrdup( tcstring string )
{
	if (!string)
		return NULL;
	tstring new_str = tstralloc( tstrlen(string) );
	if (!new_str)
		return NULL;
	tstrcpy(new_str, string);
	return new_str;
}

#define LCASE(c) (c>='A'? (c<='Z'? (c|0x20) : c) : c)
#define UCASE(c) (c>='a'? (c<='z'? (c&=~0x20) : c) : c)

int tstricmp( tcstring string1, tcstring string2 )
{
	assert(string1 && string2 );
	if (!string1)
		return -1;
	if (!string2)
		return 1;
	tcstring string1_ptr = string1;
	tcstring string2_ptr = string2;
	tchar c, c2;
	do {
		c = tstrchar(string1_ptr);
		c2 = tstrchar(string2_ptr);
		c = LCASE(c);
		c2 = LCASE(c2);
		if (c < c2)
			return -1;
		if (c > c2)
			return 1;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
	} while (c);
	return 0;
}

int tstrnicmp( tcstring string1, tcstring string2, size_t count )
{
	assert(string1 && string2 && count);
	if (!string1)
		return -1;
	if (!string2)
		return 1;
	size_t len = 0;
	tcstring string1_ptr = string1;
	tcstring string2_ptr = string2;
	tchar c, c2;
	do {
		c = tstrchar(string1_ptr);
		c2 = tstrchar(string2_ptr);
		c = LCASE(c);
		c2 = LCASE(c2);
		if (c < c2)
			return -1;
		if (c > c2)
			return 1;
		string1_ptr = tstrinc(string1_ptr);
		string2_ptr = tstrinc(string2_ptr);
		len++;
	} while (c && len < count);
	return 0;
}

static bool tstrMatchOkay(tcstring pattern)
{
	while(tstrchar(pattern) == '*')
		pattern = tstrinc(pattern);
	
	if (tstrchar(pattern) != 0)
		return false;
	
	return true;
}

bool tstrMatchWithPattern(tcstring string, tcstring pattern_ucase, bool case_sensetivity)
{
	tchar s, p;
	tcstring pattern = pattern_ucase;
	
	p = tstrchar(pattern);
	if ('*' == p)
	{
		do { 
			pattern = tstrinc(pattern); 
		} while ('*' == (p = tstrchar(pattern)));

		s = tstrchar(string);
		while(s && p)
		{
			if (case_sensetivity == false)
				s = UCASE(s);
			if ((s == '*') || (s == p) || (p == '?'))
			{
				if(tstrMatchWithPattern(tstrinc(string), tstrinc(pattern), case_sensetivity)) 
					return true;
			}
			string = tstrinc(string);
			s = tstrchar(string);
		}
		return tstrMatchOkay(pattern);
	} 
	
	s = tstrchar(string);
	while (s && p != '*')
	{
		if (case_sensetivity == false)
			s = UCASE(s);
		
		if( (s != p) && (p != '?') )
			return false;
		
		pattern = tstrinc(pattern);
		p = tstrchar(pattern);
		string = tstrinc(string);
		s = tstrchar(string);
	}
	
	if (s)
		return tstrMatchWithPattern(string, pattern, case_sensetivity);
	
	return tstrMatchOkay(pattern);
}

char* tstrchrA( const char* string, char s )
{
	assert(string && s);
	char c;
	const char* ptr = string;
	do {
		c = *ptr;
		if (c == s)
			return (char*)ptr;
		ptr++;
	} while (c);
	return NULL;
}

char* tstrichrA( const char* string, char s )
{
	assert(string && s);
	char c;
	const char* ptr = string;
	do {
		c = *ptr;
		c = LCASE(c);
		if (c == s)
			return (char*)ptr;
		ptr++;
	} while (c);
	return NULL;
}

size_t tstrlenA(const char* string)
{
	size_t len = 0; 
	while (*string)
	{
		len++;
		string++;
	};
	return len;
}

int tstrnicmpA( const char* string1, const char* string2, size_t count )
{
	assert(string1 && string2 && count);
	if (!string1)
		return -1;
	if (!string2)
		return 1;
	size_t len = 0;
	const char* string1_ptr = string1;
	const char* string2_ptr = string2;
	char c, c2;
	do {
		c = *string1_ptr;
		c2 = *string2_ptr;
		c = LCASE(c);
		c2 = LCASE(c2);
		if (c < c2)
			return -1;
		if (c > c2)
			return 1;
		string1_ptr++;
		string2_ptr++;
		len++;
	} while (c && len < count);
	return 0;
}

const char*   tstristrA( const char* string1, const char* string2 )
{
	assert(string1 && string2 );
	const char* ptr = string1;
	char c = string2[0];
	if (0 == c)
		return NULL;
	size_t len = tstrlenA(string2);
	while (ptr = tstrichrA(ptr, c))
	{
		if (0 == tstrnicmpA(ptr, string2, len))
			return ptr;
		ptr++;
	};
	return NULL;
}
