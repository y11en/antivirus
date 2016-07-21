// IncrediBuild.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "objbase.h"
#include "stdio.h"

#define FILE_TIME_DAY ((__int64)24*60*60*10000000L)

int main(int argc, char* argv[])
{
	__int64 nTrialDays = 44;
	if (argc > 1)
		nTrialDays = atol(argv[1]);
	if (nTrialDays == 0)
	{
		printf("Usage: %s [TrialDays]\n", argv[0]);
		return 0;
	}

	BYTE guid[16];
	double d;
	__int64 n;
	FILETIME ft;
	FILETIME ft_1900;
	SYSTEMTIME st;
	st.wYear = 1899;
	st.wMonth = 12;
	st.wDay = 30;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &ft_1900);
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	n = *(__int64*)&ft;
	n = n + nTrialDays*FILE_TIME_DAY;
	*(__int64*)&ft = n;
	FileTimeToSystemTime(&ft, &st);
	printf("New expiration date is %02d.%02d.%02d %02d:%02d:%02d, %d days left\n",
			st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, nTrialDays);
	// calculate number of days since 1900 (delphi 
	n = n - *(__int64*)&ft_1900;
	d = (double)(n / FILE_TIME_DAY);
	d += (double)(n % FILE_TIME_DAY) / FILE_TIME_DAY;
	// copy double into GUID
	memcpy(&guid[8], &d, 8);
	// calculate checksum
	*(DWORD*)&guid = guid[8] * guid[9] * guid[10] * guid[11];
	*(WORD*)&guid[4] = guid[12] * guid[13];
	*(WORD*)&guid[6] = guid[14] * guid[15];
	LPOLESTR pGuidStr;
	if (FAILED(StringFromCLSID((GUID&)guid, &pGuidStr)))
	{
		printf("ERROR: Cannot convert CLSID to string\n");
		return -1;
	}
	printf("Guid is %S\n", pGuidStr);

	if (ERROR_SUCCESS != RegSetValueW(HKEY_CLASSES_ROOT, L"Interface\\{D1328CD2-264B-4104-B800-A7774E74753B}\\ProxyStubClsid32", REG_SZ, pGuidStr, wcslen(pGuidStr)*sizeof(WCHAR)))
		printf("ERROR: Cannot set value with new CLSID\n");
	else
		printf("Registry successfuly updated\n");

	CoTaskMemFree(pGuidStr);
	return 0;
}
