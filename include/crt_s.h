#if !defined(__CRT_S_H__included__) && !defined( __COMPATUTILS_H__) && !defined(_WIN32)
#define __CRT_S_H__included__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

inline int strcpy_s(char *strDestination, size_t numberOfElements, const char *strSource) {
	strcpy(strDestination, strSource);
	return 0;
}
inline int strcpy_s(char *strDestination, const char *strSource) {
	strcpy(strDestination, strSource);
	return 0;
}

inline int strncpy_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count) {
	strncpy(strDest, strSource, count);
	return 0;
}
inline int strncpy_s(char *strDest, const char *strSource, size_t count) {
	strncpy(strDest, strSource, count);
	return 0;
}

inline int strcat_s(char *strDestination, size_t numberOfElements, const char *strSource) {
	strcat(strDestination, strSource);
	return 0;
}
inline int strcat_s(char *strDestination, const char *strSource) {
	strcat(strDestination, strSource);
	return 0;
}

inline int sprintf_s(char * str, size_t size, const char * format, ...) {
	int result;
	va_list ap;
	va_start(ap, format);
	result = vsnprintf(str, size, format, ap);
	va_end (ap);
	return result;
}
inline int sprintf_s(char * str, const char * format, ...) {
	int result;
	va_list ap;
	va_start(ap, format);
	result = vsprintf(str, format, ap);
	va_end (ap);
	return result;
}

inline int vsprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list argptr) {
	return vsprintf(buffer, format, argptr);
}
inline int vsprintf_s(char *buffer, const char *format, va_list argptr) {
	return vsprintf(buffer, format, argptr);
}

inline int fopen_s(FILE** pFile, const char *filename, const char *mode) {
	if (FILE * fp = fopen(filename, mode)) {
		*pFile = fp;
		return 0;
	}
	
	return (errno == 0 ? EFAULT : errno);
}

#endif // __CRT_S_H__included__
