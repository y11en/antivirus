#include <wchar.h>

#include <libutf/utf.h>

int wcsncmp ( const wchar_t *s1, 
	      const wchar_t *s2,
	      size_t n )
{
	return runestrncmp ( (Rune*) s1, (Rune*) s2, n );
}

