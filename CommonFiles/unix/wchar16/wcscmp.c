#include <wchar.h>

#include <libutf/utf.h>

int wcscmp ( const wchar_t *s1, 
	     const wchar_t *s2 )
{
	return runestrcmp ( (Rune*) s1, (Rune*) s2 );
}

