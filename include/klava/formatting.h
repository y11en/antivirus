// formatting.h
//

#ifndef formatting_h_INCLUDED
#define formatting_h_INCLUDED

#include <stdarg.h>
#include <stddef.h>

int kl_sprintf (char* buff, const char * format, ...);
int kl_vsprintf (char* buff, const char * format, va_list arglist);
int kl_scprintf (const char * format, ...);
int kl_vscprintf (const char * format, va_list arglist);
int kl_snprintf (char* buff, size_t count, const char * format, ...);
int kl_vsnprintf (char* buff, size_t count, const char * format, va_list arglist);


#endif // formatting_h_INCLUDED


