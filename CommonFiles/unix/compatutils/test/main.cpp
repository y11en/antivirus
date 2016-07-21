#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#if defined(__unix__)
#include <stdint.h>
#include <wchar.h>
#include <unix/compatutils/compatutils.h>
#endif

void checkresult(bool res, const char *func, const char *desc)
{
	printf("%s\t:%s\t%s\n", res? "OK  " : "Fail", func, desc );
}

void invalid_parameter(
		       const wchar_t * expression,
		       const wchar_t * function, 
		       const wchar_t * file, 
		       int line,
		       uintptr_t pReserved
		       )
{
}

#define countof(a) (sizeof(a)/sizeof(a[0]))
#define reset() errno = 0; buf[0] = (char)0xCC; buf[1] = (char)0xCC; buf[2] = (char)0xCC; ret = 0; 

int ret;

void test_sprintf_s()
{
	const char *fname = "sprintf_s";

	char buf[3];

	reset();
	ret = sprintf_s(NULL, countof(buf), "");
	checkresult (ret == -1 && errno == EINVAL, fname, "buffer == NULL");

	reset();
	ret = sprintf_s(buf, countof(buf), NULL);
	checkresult (ret == -1 && errno == EINVAL, fname, "format == NULL");

	reset();
	ret = sprintf_s(buf, countof(buf), "%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal");

	reset();
	ret = sprintf_s(buf, countof(buf), "%d", 11);
	checkresult (ret == 2 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "exact");

	reset();
	ret = sprintf_s(buf, countof(buf), "%d", 111);
	checkresult (ret == -1 && errno == ERANGE && buf[0] == '\0', fname, "overrun");
}


void test__snprintf_s()
{
	const char *fname = "_snprintf_s";

	char buf[3];

	reset();
	ret = _snprintf_s(NULL, countof(buf), 1, "");
	checkresult (ret == -1 && errno == EINVAL, fname, "buffer == NULL");

	reset();
	ret = _snprintf_s(buf, countof(buf), 0, NULL);
	checkresult (ret == -1 && errno == EINVAL, fname, "format == NULL, count == 0");

	reset();
	ret = _snprintf_s(buf, countof(buf), 1, NULL);
	checkresult (ret == -1 && errno == EINVAL, fname, "format == NULL, count == 1");

	reset();
	ret = _snprintf_s(buf, countof(buf), 0, "");
	checkresult (ret == 0 && errno == 0, fname, "count == 0");

	reset();
	ret = _snprintf_s(buf, countof(buf), -2, "");
	checkresult (ret == 0 && errno == 0, fname, "count < 0");

	reset();
	ret = _snprintf_s(buf, countof(buf), 1, "%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal");

	reset();
	ret = _snprintf_s(buf, countof(buf), 2, "%d", 11);
	checkresult (ret == 2 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "exact");

	reset();
	ret = _snprintf_s(buf, countof(buf), 3, "%d", 111);
	checkresult (ret == -1 && errno == ERANGE && buf[0] == '\0', fname, "overrun");

	reset();
	ret = _snprintf_s(buf, countof(buf), 4, "%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, count > buffer");

	reset();
	ret = _snprintf_s(buf, countof(buf), 1, "%d", 111);
	checkresult (ret == -1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "overrun with normal count");

	reset();
	ret = _snprintf_s(buf, countof(buf), 2, "%d", 111);
	checkresult (ret == -1 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "overrun with exact count");

	reset();
	ret = _snprintf_s(buf, countof(buf), _TRUNCATE, "%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal with truncate");

	reset();
	ret = _snprintf_s(buf, countof(buf), _TRUNCATE, "%d", 11);
	checkresult (ret == 2 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "exact with truncate");

	reset();
	ret = _snprintf_s(buf, countof(buf), _TRUNCATE, "%d", 111);
	checkresult (ret == -1 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "overrun with truncate");
}

#if defined (_WIN32)

void test_swprintf_s()
{
	const char *fname = "swprintf_s";

	wchar_t buf[3];

	reset();
	ret = swprintf_s(NULL, countof(buf), L"");
	checkresult (ret == -1 && errno == EINVAL, fname, "buffer == NULL");

	reset();
	ret = swprintf_s(buf, countof(buf), NULL);
	checkresult (ret == -1 && errno == EINVAL, fname, "format == NULL");

	reset();
	ret = swprintf_s(buf, countof(buf), L"%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal");

	reset();
	ret = swprintf_s(buf, countof(buf), L"%d", 11);
	checkresult (ret == 2 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "exact");

	reset();
	ret = swprintf_s(buf, countof(buf), L"%d", 111);
	checkresult (ret == -1 && errno == ERANGE && buf[0] == '\0', fname, "overrun");
}

void test__snwprintf_s()
{
	const char *fname = "_snwprintf_s";

	wchar_t buf[3];

	reset();
	ret = _snwprintf_s(NULL, countof(buf), 1, L"");
	checkresult (ret == -1 && errno == EINVAL, fname, "buffer == NULL");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 0, NULL);
	checkresult (ret == -1 && errno == EINVAL, fname, "format == NULL, count == 0");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 1, NULL);
	checkresult (ret == -1 && errno == EINVAL, fname, "format == NULL, count == 1");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 0, L"");
	checkresult (ret == 0 && errno == 0, fname, "count == 0");

	reset();
	ret = _snwprintf_s(buf, countof(buf), -2, L"");
	checkresult (ret == 0 && errno == 0, fname, "count < 0");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 1, L"%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 2, L"%d", 11);
	checkresult (ret == 2 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "exact");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 3, L"%d", 111);
	checkresult (ret == -1 && errno == ERANGE && buf[0] == '\0', fname, "overrun");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 4, L"%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, count > buffer");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 1, L"%d", 111);
	checkresult (ret == -1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "overrun with normal count");

	reset();
	ret = _snwprintf_s(buf, countof(buf), 2, L"%d", 111);
	checkresult (ret == -1 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "overrun with exact count");

	reset();
	ret = _snwprintf_s(buf, countof(buf), _TRUNCATE, L"%d", 1);
	checkresult (ret == 1 && errno == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal with truncate");

	reset();
	ret = _snwprintf_s(buf, countof(buf), _TRUNCATE, L"%d", 11);
	checkresult (ret == 2 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "exact with truncate");

	reset();
	ret = _snwprintf_s(buf, countof(buf), _TRUNCATE, L"%d", 111);
	checkresult (ret == -1 && errno == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "overrun with truncate");
}
#endif

void test_strcpy_s()
{
	const char *fname = "strcpy_s";

	char buf[3];

	reset();
	ret = strcpy_s(NULL, countof(buf), "");
	checkresult (ret == EINVAL, fname, "dst == NULL");

	reset();
	ret = strcpy_s(buf, countof(buf), NULL);
	checkresult (ret == EINVAL && buf[0] == '\0', fname, "src == NULL");

	reset();
	ret = strcpy_s(buf, 0, "");
	checkresult (ret == EINVAL && buf[0] == (char)0xCC, fname, "size == 0");

	reset();
	ret = strcpy_s(buf, countof(buf), "");
	checkresult (ret == 0 && buf[0] == '\0', fname, "normal, 0 chars");

	reset();
	ret = strcpy_s(buf, countof(buf), "1");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, less chars");

	reset();
	ret = strcpy_s(buf, countof(buf), "11");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "normal, exact chars");

	reset();
	ret = strcpy_s(buf, countof(buf), "111");
	checkresult (ret == ERANGE && buf[0] == '\0', fname, "overrun");
}

void test_wcscpy_s()
{
	const char *fname = "wcscpy_s";

	wchar_t buf[3];

	reset();
	ret = wcscpy_s(NULL, countof(buf), L"");
	checkresult (ret == EINVAL, fname, "dst == NULL");

	reset();
	ret = wcscpy_s(buf, countof(buf), NULL);
	checkresult (ret == EINVAL && buf[0] == '\0', fname, "src == NULL");

	reset();
	ret = wcscpy_s(buf, 0, L"");
	checkresult (ret == EINVAL && buf[0] == (wchar_t)(char)0xCC, fname, "size == 0");

	reset();
	ret = wcscpy_s(buf, countof(buf), L"");
	checkresult (ret == 0 && buf[0] == '\0', fname, "normal, 0 chars");

	reset();
	ret = wcscpy_s(buf, countof(buf), L"1");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, less chars");

	reset();
	ret = wcscpy_s(buf, countof(buf), L"11");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "normal, exact chars");

	reset();
	ret = wcscpy_s(buf, countof(buf), L"111");
	checkresult (ret == ERANGE && buf[0] == '\0', fname, "overrun");
}

void test_strncpy_s()
{
	const char *fname = "strncpy_s";

	char buf[3];

	reset();
	ret = strncpy_s(NULL, countof(buf), "", 0);
	checkresult (ret == EINVAL, fname, "dst == NULL");

	reset();
	ret = strncpy_s(buf, countof(buf), NULL, 0);
	checkresult (ret == 0 && buf[0] == 0, fname, "src == NULL");

	reset();
	ret = strncpy_s(buf, 0, "", 0);
	checkresult (ret == EINVAL && buf[0] == (char)0xCC, fname, "size == 0");

	reset();
	ret = strncpy_s(buf, countof(buf), "", 0);
	checkresult (ret == 0 && buf[0] == '\0', fname, "normal, 0 chars");

	reset();
	ret = strncpy_s(buf, countof(buf), "1", sizeof("1"));
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, less chars");

	reset();
	ret = strncpy_s(buf, countof(buf), "11", sizeof("11"));
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "normal, exact chars");

	reset();
	ret = strncpy_s(buf, countof(buf), "111", sizeof("111"));
	checkresult (ret == ERANGE && buf[0] == '\0', fname, "overrun");
}


void test_strcat_s()
{
	const char *fname = "strcat_s";

	char buf[3];

	reset();
	ret = strcat_s(NULL, countof(buf), "");
	checkresult (ret == EINVAL, fname, "dst == NULL");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = strcat_s(buf, countof(buf), NULL);
	checkresult (ret == EINVAL && buf[0] == '\0', fname, "src == NULL");

	reset();
	ret = strcat_s(buf, 0, "");
	checkresult (ret == EINVAL && buf[0] == (char)0xCC, fname, "size == 0");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = strcat_s(buf, countof(buf), "");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, 0 chars");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = strcat_s(buf, countof(buf), "1");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "normal, exact chars");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = strcat_s(buf, countof(buf), "11");
	checkresult (ret == ERANGE && buf[0] == '\0', fname, "overrun");

}

void test_wcscat_s()
{
	const char *fname = "wcscat_s";

	wchar_t buf[3];

	reset();
	ret = wcscat_s(NULL, countof(buf), L"");
	checkresult (ret == EINVAL, fname, "dst == NULL");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = wcscat_s(buf, countof(buf), NULL);
	checkresult (ret == EINVAL && buf[0] == '\0', fname, "src == NULL");

	reset();
	ret = wcscat_s(buf, 0, L"");
	checkresult (ret == EINVAL && buf[0] == (wchar_t)(char)0xCC, fname, "size == 0");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = wcscat_s(buf, countof(buf), L"");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '\0', fname, "normal, 0 chars");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = wcscat_s(buf, countof(buf), L"1");
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '\0', fname, "normal, exact chars");

	reset();
	buf[0] = '1';
	buf[1] = '\0';
	ret = wcscat_s(buf, countof(buf), L"11");
	checkresult (ret == ERANGE && buf[0] == '\0', fname, "overrun");

}

void test_memcpy_s()
{
	const char *fname = "memcpy_s";

	char buf[3];

	reset();
	ret = memcpy_s(NULL, countof(buf), "", 1);
	checkresult (ret == EINVAL, fname, "dst == NULL");

	reset();
	ret = memcpy_s(buf, countof(buf), NULL, 1);
	checkresult (ret == EINVAL && buf[0] == '\0', fname, "src == NULL");

	reset();
	ret = memcpy_s(buf, 0, "", 1);
	checkresult (ret == ERANGE && buf[0] == (char)0xCC, fname, "dstsize == 0");

	reset();
	ret = memcpy_s(buf, countof(buf), "", 0);
	checkresult (ret == 0 && buf[0] == (char)0xCC, fname, "srcsize == 0");

	reset();
	ret = memcpy_s(buf, countof(buf), "", 1);
	checkresult (ret == 0 && buf[0] == '\0', fname, "normal, 1 byte");

	reset();
	ret = memcpy_s(buf, countof(buf), "111", 3);
	checkresult (ret == 0 && buf[0] == '1' && buf[1] == '1' && buf[2] == '1', fname, "normal, exact bytes");

	reset();
	ret = memcpy_s(buf, countof(buf), "1111", 4);
	checkresult (ret == ERANGE && buf[0] == '\0' && buf[1] == '\0' && buf[2] == '\0', fname, "overrun");
}

void test_mbstowcs_s()
{
	const char *fname = "mbstowcs_s";

	wchar_t buf[3];
	
	size_t result = 0;

	reset();
	ret = mbstowcs_s(&result, NULL, 0, fname, strlen(fname));
	checkresult (ret == 0 && result == (strlen(fname) + 1), fname, "dst == NULL");

	reset();
	ret = mbstowcs_s(&result, NULL, 1, fname, strlen(fname));
	checkresult (ret == EINVAL, fname, "dst == NULL && sizeInWords > 0");

	reset();
	ret = mbstowcs_s(&result, buf, countof(buf), 0, strlen(fname));
	checkresult (ret == EINVAL && buf[0] == 0, fname, "src == NULL");

	reset();
	ret = mbstowcs_s(&result, buf, countof(buf), fname, 2);
	checkresult (ret == 0 && result == 3 && buf[0] == L'm' && buf[1] == L'b' && buf[2] == 0, fname, "normal");

	reset();
	ret = mbstowcs_s(&result, buf, countof(buf), fname, strlen(fname));
	checkresult (ret == ERANGE && buf[0] == 0, fname, "overrun");

	reset();
	ret = mbstowcs_s(&result, buf, countof(buf), fname, _TRUNCATE);
	checkresult (ret == STRUNCATE && result == 3 && buf[0] == L'm' && buf[1] == L'b' && buf[2] == 0, fname, "truncate");
}

void test_wcstombs_s()
{
	const char *fname = "wcstombs_s";
	const wchar_t *test = L"test";

	char buf[3];
	
	size_t result = 0;

	reset();
	ret = wcstombs_s(&result, NULL, 0, test, wcslen(test));
	checkresult (ret == 0 && result == (wcslen(test) + 1), fname, "dst == NULL");

	reset();
	ret = wcstombs_s(&result, NULL, 1, test, wcslen(test));
	checkresult (ret == EINVAL, fname, "dst == NULL && sizeInBytes > 0");

	reset();
	ret = wcstombs_s(&result, buf, countof(buf), 0, wcslen(test));
	checkresult (ret == EINVAL && buf[0] == 0, fname, "src == NULL");

	reset();
	ret = wcstombs_s(&result, buf, countof(buf), test, 2);
	checkresult (ret == 0 && result == 3 && buf[0] == 't' && buf[1] == 'e' && buf[2] == 0, fname, "normal");

	reset();
	ret = wcstombs_s(&result, buf, countof(buf), test, wcslen(test));
	checkresult (ret == ERANGE && buf[0] == 0, fname, "overrun");

	reset();
	ret = wcstombs_s(&result, buf, countof(buf), test, _TRUNCATE);
	checkresult (ret == STRUNCATE && result == 3 && buf[0] == 't' && buf[1] == 'e' && buf[2] == 0, fname, "truncate");
}

int main()
{
#ifdef _WIN32
	_set_invalid_parameter_handler((_invalid_parameter_handler)invalid_parameter);
#endif


	test_sprintf_s();
	test__snprintf_s();    
#if defined(_WIN32)    
	test_swprintf_s();    
	test__snwprintf_s();
#endif

    

	test_strcpy_s();
	test_wcscpy_s();

	test_strncpy_s();
	test_strcat_s();
	test_wcscat_s();

	test_memcpy_s();

	test_mbstowcs_s();

	test_wcstombs_s();

	return 0;
}
