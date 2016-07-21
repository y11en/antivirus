#include "stl_port.h"

namespace stl_port
{
	void erase(char* token, size_t pos, size_t count)
	{
		if(!count)
			return; 
		size_t token_size = strlen(token);
		if(!token_size)
			return;
		if(pos + count > token_size)
		{
			token[pos] = '\0';
			return;
		}
		memmove(token + pos, token + pos + count, token_size + 1 - pos - count);
	}
	
	size_t find_first_not_of(char* token, char* delimeters, size_t pos)
	{
		size_t token_size = strlen(token);

		if ( token_size == 0 )
			return STRING_NPOS;

		size_t i = pos;
		size_t j = 0;
		size_t delimeters_size = strlen(delimeters);
		for ( i = pos; i < token_size; i++ )
		{
			for ( j = 0; j < delimeters_size; j++ )
			{
				if ( token[i] != delimeters[j] )
					return i;
			}
		}
		return STRING_NPOS;
	}

	/*inline */size_t find_first_of(char* token, const cdelimeter* pdelimeters)
	{
		char* pcpos = token;
		while (*token)
		{
			unsigned char c = *token;
			if (pdelimeters->buf[c])
				return token - pcpos;

			token++;
		}

		return STRING_NPOS;
	}

	/*inline */size_t find_last_of(char* token, const cdelimeter* pdelimeters, size_t pos)
	{
		size_t token_left = (pos > strlen(token)) ? 0 : pos;
		while (token_left)
		{
			if (pdelimeters->buf[token[token_left]])
				return token_left;

			token_left--;
		}

		return STRING_NPOS;
	}

	/*inline */size_t find_last_of(char* token, char* delimeters, size_t pos)
	{
		size_t token_size = strlen(token);

		if ( token_size == 0 )
			return STRING_NPOS;
		if ( pos > token_size )
			return STRING_NPOS;

		size_t i = pos;
		size_t j = 0;
		size_t delimeters_size = strlen(delimeters);
		for ( i = pos; i > 0 ; i-- )
		{
			for ( j = 0; j < delimeters_size; j++ )
			{
				if ( token[i] == delimeters[j] )
					return i;
			}
		}
		return STRING_NPOS;
	}
}