#include <wchar.h>
#include <libutf/utf.h>

wint_t towupper ( wint_t wc )
{
	return (Rune) toupperrune((Rune) wc);
}
