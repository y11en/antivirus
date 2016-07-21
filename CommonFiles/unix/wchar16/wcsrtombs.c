#include <string.h>
#include <wchar.h>

#include <libutf/utf.h>

size_t wcsrtombs(char *dest, const wchar_t **source, size_t len, mbstate_t *ps)
{
	Rune** src = (Rune**) source;
	char* dst = (char*) dest;
	size_t count = 0;
	while ( 1 ) {
		char tmp [UTFmax] = {0};
		int n = runetochar ( tmp, *src );
		if ( dst ) {
			memcpy ( dst,tmp, n );
			dst += n;
			len -= n;
		}
		*src += 1;
		count += n;
		if ( dst && !len )
			break;
		if ( **src == 0 ) {
			*src = 0;
			break;
		}
	}
	return count;
}
