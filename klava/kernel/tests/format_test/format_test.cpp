// format_test.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include <math.h>
#include <string.h>
#include <kl_types.h>
#include "klava/formatting.h"

//test print all flags, width, types, and persizion configuration 
int test1()
{
	printf(" test from http://samba.anu.edu.au/rsync/doxygen/head/snprintf_8c.html\n");
	char buf1[1024];
	char buf2[1024];
	char *int_fmt[] = {
			"%-1.5d",
			"%1.5d",
			"%123.9d",
			"%5.5d",
			"%10.5d",
			"% 10.5d",
			"%+22.33d",
			"%01.3d",
			"%4d",
			"%d",
			NULL
	};
	int int_nums[] = { -1, 134, 91340, 341, 0203, 0};
	char *str_fmt[] = {
		    "10.5s",
			"5.10s",
			"10.1s",
			"0.10s",
			"10.0s",
			"1.10s",
			"%s",
			"%.1s",
			"%.10s",
			"%10s",
			NULL
	};
	char *str_vals[] = {"hello", "a", "", "a longer string", NULL};

	char *int2_fmt[] = {
			"%-1.5i",
			"%1.5i",
			"%123.9i",
			"%5.5i",
			"%10.5i",
			"% 10.5i",
			"%+22.33i",
			"%01.3i",
			"%4i",
			"%i",
			NULL
	};
	int int2_nums[] = { -1, 134, 91340, 341, 0203, 0};

	char *uint_fmt[] = {
			"%-1.5u",
			"%1.5u",
			"%123.9u",
			"%5.5u",
			"%10.5u",
			"% 10.5u",
			"%+22.33u",
			"%01.3u",
			"%4u",
			"%u",
			NULL
	};
	unsigned int uint_nums[] = { 1, 134, 91340, 341, 0203, 0};

	char *pointer_fmt[] = {
			"%-1.5x",
			"%1.5x",
			"%123.9x",
			"%5.5x",
			"%10.5x",
			"% 10.5x",
			"%+22.33x",
			"%01.3x",
			"%4x",
			"%x",
			NULL
	};
	int fake;
	int* pointer_nums[] = { &fake+1, &fake+2, &fake+3, &fake+4, &fake+5, 0};

	char *intX_fmt[] = {
			"%-1.5X",
			"%1.5X",
			"%123.9X",
			"%5.5X",
			"%10.5X",
			"% 10.5X",
			"%+22.33X",
			"%01.3X",
			"%4X",
			"%X",
			NULL
	};
	char *intx_fmt[] = {
			"%-1.5x",
			"%1.5x",
			"%123.9x",
			"%5.5x",
			"%10.5x",
			"% 10.5x",
			"%+22.33x",
			"%01.3x",
			"%4x",
			"%x",
			NULL
	};
	
	char *into_fmt[] = {
			"%-1.5o",
			"%1.5o",
			"%123.9o",
			"%5.5o",
			"%10.5o",
			"% 10.5o",
			"%+22.33o",
			"%01.3o",
			"%4o",
			"%o",
			NULL
	};
	
	int x, y;
	int fail = 0;
	int num = 0;
	
	printf ("Testing kl_snprintf format codes against system sprintf...\n");
	
	for (x = 0; int_fmt[x] ; x++) {
		for (y = 0; int_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, int_fmt[x], int_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), int_fmt[x], int_nums[y]);
			sprintf (buf2, int_fmt[x], int_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					int_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, int_fmt[x]);
				fail++;
			}
			num++;
		}
	}
	
	for (x = 0; str_fmt[x] ; x++) {
		for (y = 0; str_vals[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, str_fmt[x], str_vals[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), str_fmt[x], str_vals[y]);
			sprintf (buf2, str_fmt[x], str_vals[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					str_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, str_fmt[x]);
				fail++;
			}
			num++;
		}
	}

	for (x = 0; int2_fmt[x] ; x++) {
		for (y = 0; int2_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, int2_fmt[x], int2_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), int2_fmt[x], int2_nums[y]);
			sprintf (buf2, int2_fmt[x], int2_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					int2_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, int2_fmt[x]);
				fail++;
			}
			num++;
		}
	}

	for (x = 0; uint_fmt[x] ; x++) {
		for (y = 0; uint_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, uint_fmt[x], uint_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), uint_fmt[x], uint_nums[y]);
			sprintf (buf2, uint_fmt[x], uint_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					uint_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, uint_fmt[x]);
				fail++;
			}
			num++;
		}
	}
	for (x = 0; pointer_fmt[x] ; x++) {
		for (y = 0; pointer_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, pointer_fmt[x], pointer_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), pointer_fmt[x], pointer_nums[y]);
			sprintf (buf2, pointer_fmt[x], pointer_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					pointer_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, pointer_fmt[x]);
				fail++;
			}
			num++;
		}
	}

	for (x = 0; intX_fmt[x] ; x++) {
		for (y = 0; int_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, intX_fmt[x], int_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), intX_fmt[x], int_nums[y]);
			sprintf (buf2, intX_fmt[x], int_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					intX_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, intX_fmt[x]);
				fail++;
			}
			num++;
		}
	}	
	for (x = 0; intx_fmt[x] ; x++) {
		for (y = 0; int_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, intx_fmt[x], int_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), intx_fmt[x], int_nums[y]);
			sprintf (buf2, intx_fmt[x], int_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					intx_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, intx_fmt[x]);
				fail++;
			}
			num++;
		}
	}
	for (x = 0; into_fmt[x] ; x++) {
		for (y = 0; int_nums[y] != 0 ; y++) {
			int l1 = kl_snprintf(NULL, 0, into_fmt[x], int_nums[y]);
			int l2 = kl_snprintf(buf1, sizeof(buf1), into_fmt[x], int_nums[y]);
			sprintf (buf2, into_fmt[x], int_nums[y]);
			if (strcmp (buf1, buf2)) {
				printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
					into_fmt[x], buf1, buf2);
				fail++;
			}
			if (l1 != l2) {
				printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, into_fmt[x]);
				fail++;
			}
			num++;
		}
	}	
	printf ("%d tests failed out of %d.\n", fail, num);
	
	return 0;
	
}

template <class T>
int call_kl_snprintf(char* buff, size_t count, const char * format, T val)
{
	return kl_snprintf(buff, count, format, val);
}

template <class T>
int call_sprintf(char* buff, const char * format, T val)
{
	return sprintf(buff, format, val);
}


int testIN()
{
	printf(" test I32 and I64 format specification\n");
	char buf1[1024];
	char *int32_fmt[] = {
			"%I32d",
			"%I32u",
			NULL
	};
	char *int64_fmt[] = {
			"%I64d",
			"%I64u",
			NULL
	};
	char *int32_res[] = {"-2147483648","4294967295"};
	char *int64_res[] = {"-9223372036854775808","18446744073709551615"};

	int64_t i64 = INT64_MIN;
	uint64_t u64 = UINT64_MAX;
	int32_t i32 = INT32_MIN;
	uint32_t u32 = UINT32_MAX;

	int num = 0;
	int fail = 0;

	{
		int l1 = call_kl_snprintf(NULL, 0, int32_fmt[0], i32);
		int l2 = call_kl_snprintf(buf1, sizeof(buf1), int32_fmt[0], i32);
		if (strcmp (buf1, int32_res[0])) {
			printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
				int32_fmt[0], buf1, int32_res[0]);
			fail++;
		}
		if (l1 != l2) {
			printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, int32_fmt[0]);
			fail++;
		}
		++num;
	}
	
	{
		int l1 = call_kl_snprintf(NULL, 0, int32_fmt[1], u32);
		int l2 = call_kl_snprintf(buf1, sizeof(buf1), int32_fmt[1], u32);
		if (strcmp (buf1, int32_res[1])) {
			printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
				int32_fmt[1], buf1, int32_res[1]);
			fail++;
		}
		if (l1 != l2) {
			printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, int32_fmt[1]);
			fail++;
		}
		++num;
	}

	{
		int l1 = call_kl_snprintf(NULL, 0, int64_fmt[0], i64);
		int l2 = call_kl_snprintf(buf1, sizeof(buf1), int64_fmt[0], i64);
		if (strcmp (buf1, int64_res[0])) {
			printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
				int64_fmt[0], buf1, int64_res[0]);
			fail++;
		}
		if (l1 != l2) {
			printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, int64_fmt[0]);
			fail++;
		}
		++num;
	}

	{
		int l1 = call_kl_snprintf(NULL, 0, int64_fmt[1], u64);
		int l2 = call_kl_snprintf(buf1, sizeof(buf1), int64_fmt[1], u64);
		if (strcmp (buf1, int64_res[1])) {
			printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t sprintf = [%s]\n", 
				int64_fmt[1], buf1, int64_res[1]);
			fail++;
		}
		if (l1 != l2) {
			printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, int64_fmt[1]);
			fail++;
		}
		++num;
	}
	printf ("%d tests failed out of %d.\n", fail, num);
	return 0;

}

template <class T>
bool testTypeT(const char* format, T val, const char* expectedResult)
{
	bool res = true;
	char buf[1024];
	int l1 = call_kl_snprintf(NULL, 0, format, val);
	int l2 = call_kl_snprintf(buf, sizeof(buf), format, val);
	if (strcmp (buf, expectedResult)) {
		printf("kl_snprintf doesn't match Format: %s\n\tkl_snprintf = [%s]\n\t expected = [%s]\n", 
			format, buf, expectedResult);
		res = false;
	}
	if (l1 != l2) {
		printf("kl_snprintf l1 != l2 (%d %d) %s\n", l1, l2, format);
		res = false;
	}
	return res;
}

int testSizes()
{
	printf(" test size format specification\n");

	int num = 0;
	int fail = 0;
	{
		int32_t val = INT32_MIN;
		if (!testTypeT("%I32d", val, "-2147483648"))
			++fail;
		num++;
	}
	{
		uint32_t val = UINT32_MAX;
		if (!testTypeT("%I32u", val, "4294967295"))
			++fail;
		num++;
	}
	{
		int64_t val = INT64_MIN;
		if (!testTypeT("%I64d", val, "-9223372036854775808"))
			++fail;
		num++;
	}
	{
		uint64_t val = UINT64_MAX;
		if (!testTypeT("%I64u", val, "18446744073709551615"))
			++fail;
		num++;
	}
	{
		long val = 123;
		if (!testTypeT("%lu", val, "123"))
			++fail;
		num++;
	}
	{
		short val = -20;
		if (!testTypeT("%hd", val, "-20"))
			++fail;
		num++;
	}
	{
		unsigned short val = 20;
		if (!testTypeT("%hu", val, "20"))
			++fail;
		num++;
	}
	{
		signed char val = -10;
		if (!testTypeT("%hhd", val, "-10"))
			++fail;
		num++;
	}
	{
		unsigned char val = 10;
		if (!testTypeT("%hhu", val, "10"))
			++fail;
		num++;
	}
	{
		intmax_t val = 10;
		if (!testTypeT("%jd", val, "10"))
			++fail;
		num++;
	}
	{
		uintmax_t val = 10;
		if (!testTypeT("%ju", val, "10"))
			++fail;
		num++;
	}
	{
		size_t val = 10;
		if (!testTypeT("%zu", val, "10"))
			++fail;
		num++;
	}
	{
		ptrdiff_t val = 10;
		if (!testTypeT("%tu", val, "10"))
			++fail;
		num++;
	}
	printf ("%d tests failed out of %d.\n", fail, num);
	return 0;

}


int main(int argc, char* argv[])
{
	testIN();
	test1();
	testSizes();
	return 0;
}

