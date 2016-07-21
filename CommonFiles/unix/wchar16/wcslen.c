#include <wchar.h>

#include <libutf/utf.h>

size_t wcslen(const wchar_t *s)
{
	return runestrlen ( (Rune*) s );
}
