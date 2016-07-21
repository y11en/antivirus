#include <errno.h>
#include <wchar.h>

#include <libutf/utf.h>

size_t mbsrtowcs(wchar_t *dest, const char **source, size_t len, mbstate_t *ps)
{
	char** src = (char**) source;
	Rune* dst = (Rune*) dest;
	size_t count = 0;
	while ( 1 ) {
		Rune tmp = 0;
		int n = chartorune ( &tmp, *src );
		if ( ( n == 1 ) && ( tmp == Runesync ) ) {
			errno = EILSEQ;
			return (size_t) -1;
		}
		else {
			if ( dst )
				*dst++ = tmp;
			*src += n;
			++count;
		}
		if ( dst && !--len )
			break;
		if ( **src == 0 ) {
			*src = 0;
			break;
		}
	}
	return count;
}
