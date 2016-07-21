// TransactionTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


VOID GetSystemTimeStr(PCHAR Buf, DWORD cchBuf)
{
	SYSTEMTIME SystemTime;
	GetSystemTime(&SystemTime);
	sprintf_s(Buf, cchBuf, "%hd.%hd.%hd %hd:%hd:%hd", SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear,
		SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
}

int wmain(int argc, PWCHAR argv[])
{
	HANDLE hTrans;

	if (argc < 4)
	{
		printf("usage example:\ttransactiontest c:\\test1.txt d:\\test2.txt system\\currentcontrolset\\services\\test\n");
		return -1;
	}

	PWCHAR File1Name = argv[1];
	PWCHAR File2Name = argv[2];
	PWCHAR RegPath = argv[3];

	hTrans = CreateTransaction(NULL, NULL, 0, 0, 0, NULL/*infinite*/, L"test transaction");
	if (INVALID_HANDLE_VALUE != hTrans)
	{
		HANDLE hFile1;

		hFile1 = CreateFileTransactedW(File1Name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
			0, NULL, hTrans, NULL, NULL);
		if (INVALID_HANDLE_VALUE != hFile1)
		{
			printf("success creating first file: %S\n", File1Name);

			HANDLE hFile2;
			hFile2 = CreateFileTransactedW(File2Name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
				0, NULL, hTrans, NULL, NULL);
			if (INVALID_HANDLE_VALUE != hFile2)
			{
				printf("success creating second file: %S\n", File2Name);

				HANDLE hFile2Dup;
				hFile2Dup = CreateFileTransactedW(File2Name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
					0, NULL, hTrans, NULL, NULL);
				if (INVALID_HANDLE_VALUE != hFile2Dup)
				{
					HKEY hKey;
					DWORD dwResult;
					DWORD dwDisposition;

					printf("success opening second file dup: %S\n", File2Name);

					dwResult = RegCreateKeyTransactedW(HKEY_LOCAL_MACHINE, RegPath, 0, NULL,
						REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, &dwDisposition, hTrans, NULL);
					if (ERROR_SUCCESS == dwResult)
					{
						printf("success creating registry key: HKLM\\%S\n", RegPath);

						CHAR Buf[0x200];
						DWORD cbWritten;
						BOOL bResult;

						GetSystemTimeStr(Buf, _countof(Buf));

						printf("\nwriting date/time to first file..\n");
						bResult = WriteFile(hFile1, Buf, (DWORD)strlen(Buf), &cbWritten, NULL);
						if (bResult)
							printf("success!\n");
						else
							printf("fail!\n");

						printf("\nwriting date/time to second file..\n");
						bResult = WriteFile(hFile2, Buf, (DWORD)strlen(Buf), &cbWritten, NULL);
						if (bResult)
							printf("success!\n");
						else
							printf("fail!\n");

						printf("\nsetting value to registry..\n");
						dwResult = RegSetValueExA(hKey, "test", 0, REG_SZ, (PBYTE)Buf, (DWORD)strlen(Buf)+1);
						if (ERROR_SUCCESS == dwResult)
							printf("success!\n");
						else
							printf("fail!\n");

						printf("\ncomitting transaction..\n");
						bResult = CommitTransaction(hTrans);
						if (bResult)
							printf("success!\n");
						else
							printf("fail!\n");

						RegCloseKey(hKey);
					}

					CloseHandle(hFile2Dup);
				}
				else
					printf("failed opening second file dup: %S last error = %d\n", File2Name, GetLastError());

				CloseHandle(hFile2);
			}
			else
				printf("failed creating second file: %S\n", File2Name);

			CloseHandle(hFile1);
		}
		else
			printf("failed creating first file: %S\n", File1Name);

		CloseHandle(hTrans);
	}
	else
		printf("failed creating transaction\n");

	return 0;
}

