
#ifndef __std__print_h
#define __std__print_h

#include <iostream>
#include <cwchar>
#include <stdarg.h>

void _safe_printer( std::wostream& s, const wchar_t *format, va_list args );

#endif

// Local Variables:
// mode: C++
// End:
