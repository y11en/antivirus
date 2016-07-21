#include <stdio.h>
#include <stdarg.h>

#ifdef _MSC_VER
#	include <conio.h>
#endif

#if KL_UNICODE
#	define kl_astprintf  kl_aswprintf
#	define kl_vastprintf kl_vaswprintf
#else
#	define kl_astprintf  kl_asprintf
#	define kl_vastprintf kl_vasprintf
#endif

inline
int kl_vasprintf(char **strp, const char *fmt, va_list ap)
{
	int req_buf_size;

	if (!strp)
		return -1;

#ifdef _MSC_VER
	req_buf_size = _vscprintf(fmt, ap);
#else
	//C99 conformance required
	req_buf_size = vsnprintf(NULL, 0, fmt, ap);
#endif

	if (req_buf_size < 0)
		return -1;

	// count terminating zero
	req_buf_size++;

	*strp = (char *) malloc(req_buf_size * sizeof(char));

	if (!*strp)
		return -1;

	return vsnprintf(*strp, req_buf_size, fmt, ap);
}

inline
int kl_asprintf(char **strp, const char *fmt, ...)
{
	int result;

	va_list ap;
	va_start (ap, fmt);

	result = kl_vasprintf(strp, fmt, ap);

	va_end(ap);

	return result;
}

#ifdef _MSC_VER

inline
int kl_vaswprintf(wchar_t **strp, const wchar_t *fmt, va_list ap)
{
	int req_buf_size;

	if (!strp)
		return -1;

#ifdef _MSC_VER
	req_buf_size = _vscwprintf(fmt, ap);
#else
	//C99 conformance required
	req_buf_size = vsnwprintf(NULL, 0, fmt, ap);
#endif

	if (req_buf_size < 0)
		return -1;

	// count terminating zero
	req_buf_size++;

	*strp = (wchar_t *) malloc(req_buf_size * sizeof(wchar_t));

	if (!*strp)
		return -1;

	return vsnwprintf(*strp, req_buf_size, fmt, ap);
}

inline
int kl_aswprintf(wchar_t **strp, const wchar_t *fmt, ...)
{
	int result;

	va_list ap;
	va_start (ap, fmt);

	result = kl_vaswprintf(strp, fmt, ap);

	va_end(ap);

	return result;
}

#endif
