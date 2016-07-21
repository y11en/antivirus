#ifndef _INLINE_NAMESPACE_H_
#define _INLINE_NAMESPACE_H_

namespace INLINE
{
	inline int isdigit(int ch)
	{
		switch( ch ) 
		{
		case 48: //	0:
		case 49: //	1:
		case 50: //	2:
		case 51: //	3:
		case 52: //	4:
		case 53: //	5:
		case 54: //	6:
		case 55: //	7:
		case 56: //	8:
		case 57: //	9:
			return 1;
			break;
		default:
			return 0;
			break;
		}
	};
	
	inline int isalpha(int ch)
	{
		if ( 
			(ch >= 'A' && ch <= 'Z')
			||
			(ch >= 'a' && ch <= 'z')
			||
			(ch == '.') ||
			(ch == ',') ||
			(ch == ';') ||
			(ch == ':') ||
			(ch == '~') ||
			(ch == '\'') ||
			(ch == '!') ||
			(ch == '?') ||
			(ch == '`') ||
			(ch == '\\') ||
			(ch == '/') ||
			(ch == '|') ||
			(ch == '-') ||
			(ch == '+') ||
			(ch == '<') ||
			(ch == '>') ||
			(ch == '@') ||
			(ch == '#') ||
			(ch == '$') ||
			(ch == '%') ||
			(ch == '^') ||
			(ch == '(') ||
			(ch == ')') ||
			(ch == '=') ||
			(ch == '_') ||
			(ch == '"')
			) 
			return 1;
		else
			return 0;
	};

	inline int isspace (int ch)
	{
		if (
			(ch >= 0x09 && ch <= 0x0d)
			||
			(ch == 0x20)
			)
			return 1;
		else
			return 0;
	};

	inline int toupper(int ch)
	{
		if ( INLINE::isalpha(ch) && ch >= 97 ) 
			return ch - 32;
		else
			return ch;
	};

	inline void ToUpper(char* sz, int size)
	{
		for ( int i = 0; i < size; i++  )
			sz[i] = toupper(sz[i]);
	};
}

#endif//_INLINE_NAMESPACE_H_
