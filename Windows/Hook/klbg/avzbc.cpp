// (С) Зайцев Олег, 2006
#include <fltkernel.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include "avzbc.h"
#include "avz.h"
#include "main.h"

#pragma warning(disable:4995)	// deprecated functions

extern "C" PWORD NtBuildNumber; // Номер сборки NT

// global filter object
extern "C" PFLT_FILTER g_pFilterObj;

// Текстовые константы
PRESET_UNICODE_STRING (usImagePath,       CSTRING (ImagePath));
// Имя файла для протоколирования
UNICODE_STRING		usLogFile;
// Путь к папке для карантина
UNICODE_STRING		usQrPath;
// Признак того, что протоколирование разрешено
BOOLEAN EnableLog = FALSE;
// Признак того, что карантин разрешен
BOOLEAN EnableQr  = FALSE;
BOOLEAN AnsiLog   = TRUE;
// Счетчик карантина
int QrCounter = 1;
// Размер последнего скопированного файла
DWORD LastFileSize = 0;

static BOOLEAN g_bLogInited = FALSE;
static PFLT_INSTANCE g_pLogFileInstance = NULL;
static HANDLE g_hLogFile = NULL;
static PFILE_OBJECT g_pLogFileObject = NULL;

// Выделение памяти размером Size байт для Unicode строки 
VOID AllocateUnicodeString(PUNICODE_STRING us, USHORT Size)
{
	if (!us) return;
	__try {
		us->Length = 0;
		us->MaximumLength = 0;
		us->Buffer = (PWSTR)ExAllocatePool(PagedPool, Size);
		if (us->Buffer) {
			us->Length = 0;
			us->MaximumLength = Size;
		}
	} __except(EXCEPTION_EXECUTE_HANDLER) {	}
}

// Освобождение памяти Unicode строки
VOID FreeUnicodeString(PUNICODE_STRING us)
{
	if (!us) return;
	__try {
		if (us->MaximumLength > 0 && us->Buffer) 
			ExFreePool(us->Buffer);
		us->Length = 0;
		us->MaximumLength = 0;
	} __except(EXCEPTION_EXECUTE_HANDLER) {	}
}

// Очистка  Unicode строки символами с кодом 0 
VOID ClearUnicodeString(PUNICODE_STRING us)
{
	if (!us) return;
	__try {
		if (us->MaximumLength > 0 && us->Buffer) 
			memset(us->Buffer, 0, us->MaximumLength);
		us->Length = 0;
	} __except(EXCEPTION_EXECUTE_HANDLER) {	}
}

VOID AddToLog(WCHAR *txt, PUNICODE_STRING txt1, ULONG *StatusCode)
{
	NTSTATUS status;
	
	// Проверка, разрешено ли протоколирование
	if (!EnableLog) return;

	if ( !g_bLogInited )
	{
		OBJECT_ATTRIBUTES logfileOA;
		IO_STATUS_BLOCK ioSB;

		g_pLogFileInstance = GetInstanceForFile( &usLogFile );
		if ( !g_pLogFileInstance )
			return;

		InitializeObjectAttributes(
			&logfileOA,
			&usLogFile,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			NULL,
			NULL
			);

		status = FltCreateFile(
			g_pFilterObj,
			g_pLogFileInstance,
			&g_hLogFile,
			SYNCHRONIZE | DELETE | FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES,
			&logfileOA,
			&ioSB,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ,
			FILE_SUPERSEDE,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0,
			0
			);
		if ( !NT_SUCCESS(status) )
			FltObjectDereference( g_pLogFileInstance );
		else
		{
			status = ObReferenceObjectByHandle(
				g_hLogFile,
				0,
				*IoFileObjectType,
				KernelMode,
				(PVOID *) &g_pLogFileObject,
				NULL
				);
			if ( !NT_SUCCESS(status) )
			{
				FltClose( g_hLogFile );
				FltObjectDereference( g_pLogFileInstance );
				return;
			}
			else
				g_bLogInited = TRUE;
		}
	}

	// Буфер строки
	WCHAR			Buffer[1024], CodeTxt[20];
	ULONG          BuffSize = 0;
	// Очистка буфера строки
	RtlZeroMemory(&Buffer, sizeof(Buffer));
	// Копирование строки
	wcscpy(Buffer, txt);
	// Добавление второй необязательной строки
	if (txt1 != NULL && txt1->Buffer != NULL) {
		__try {
			for (int i = 0; i < txt1->Length / 2; i++)
				if (txt1->Buffer[i] == 0)
					txt1->Buffer[i] = L'*';
		} __except(EXCEPTION_EXECUTE_HANDLER) {}
		wcscat(Buffer, (wchar_t *)txt1->Buffer);
	}
	// Добавление парметра
	if (StatusCode != NULL) {
		if (*StatusCode == 0)
			wcscat(Buffer, L" - succeeded");
		else {
			wcscat(Buffer, L" - failed (0x");
			swprintf(CodeTxt, L"%X", *StatusCode);
			wcscat(Buffer, (wchar_t *)CodeTxt);
			wcscat(Buffer, L")");
		}
	}
	// Добавление к нему CRLF	
	wcscat(Buffer, L"\r\n");
	BuffSize = (ULONG) wcslen(Buffer) * 2;

	ULONG cbWritten;

	zDbgPrint("%S", Buffer);
	if (AnsiLog) {
		// Преобразование буфера в ANSI
		ANSI_STRING		AnsiString;
		UNICODE_STRING     UnicodeString;

		RtlInitUnicodeString(&UnicodeString, Buffer);
		if (RtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, TRUE) == STATUS_SUCCESS)	  
			// Запись AnsiString 
			status = FltWriteFile(
				g_pLogFileInstance,
				g_pLogFileObject,
				NULL,
				AnsiString.Length,
				AnsiString.Buffer,
				0,
				&cbWritten,
				NULL,
				NULL
				);
	}
	else 
		// Запись буфера c UNICODE строкой
		status = FltWriteFile(
			g_pLogFileInstance,
			g_pLogFileObject,
			NULL,
			BuffSize,
			Buffer,
			0,
			&cbWritten,
			NULL,
			NULL
		);
}

// Удаление ключа реестра
NTSTATUS zRegDeleteKey(IN PUNICODE_STRING pusRegistryPath)
{
	// Указатель на строку некорректный - тогда выход
	if (!MmIsAddressValid(pusRegistryPath)) return STATUS_UNSUCCESSFUL;
	zDbgPrint("zRegDeleteKey (%S)", pusRegistryPath->Buffer);
	// Подготовка атрибутов объекта "Ключ рестра"
	OBJECT_ATTRIBUTES ob;
	InitializeObjectAttributes(&ob, pusRegistryPath, OBJ_CASE_INSENSITIVE + OBJ_KERNEL_HANDLE + OBJ_FORCE_ACCESS_CHECK, NULL, NULL);
	NTSTATUS ns;
	HANDLE KeyHandle;
	// Открытие ключа
	ns = ZwOpenKey(&KeyHandle, DELETE, &ob);
	zDbgPrint("ZwOpenKey  = %X", ns);
	if (NT_SUCCESS(ns)) {
		ns = ZwDeleteKey(KeyHandle);
		zDbgPrint("ZwDeleteKey  = %X", ns);
		ZwClose(KeyHandle);
	}
	return ns;
}
BOOLEAN IsValidHexNumber(IN PUNICODE_STRING pusStr)
{
	if (!MmIsAddressValid(pusStr)) return FALSE;
	for (int i = 0; i < pusStr->Length / 2; i++) 
		switch (pusStr->Buffer[i]) {
		case L'0' : break;		case L'1' : break;		case L'2' : break;
		case L'3' : break;		case L'4' : break;		case L'5' : break;
		case L'6' : break;		case L'7' : break;		case L'8' : break;
		case L'9' : break;		case L'A' : break;		case L'B' : break;
		case L'C' : break;		case L'D' : break;		case L'E' : break;
		case L'F' : break;		case L'a' : break;		case L'b' : break;
		case L'c' : break;		case L'd' : break;		case L'e' : break;
		case L'f' : break;
		default:
			return FALSE;
	}
	return TRUE;
}

// Подготовка строки - замена макросов и т.п.
VOID PrepareParamString(IN PUNICODE_STRING pusStr)
{
	//return;
	UNICODE_STRING usTmp;
	// Выделение временной строки
	AllocateUnicodeString(&usTmp, pusStr->MaximumLength);
	__try {
		ClearUnicodeString(&usTmp);
		int Poz = 0;
		// Цикл анализа строки и поиска макросов
		for (int i = 0; i < pusStr->Length / 2; i++) 
			if (pusStr->Buffer[i] != L'\\') {
				// Это не спецсимвол ? Тогда копируем
				usTmp.Buffer[Poz] = pusStr->Buffer[i];
				Poz++;
			} 
			else {
				// Ошибка - в строке недостаточно данных
				if ((pusStr->Length / 2) - i < 6) {
					usTmp.Buffer[Poz] = pusStr->Buffer[i];
					Poz++;
					continue;				
				}
				i++; // пропуск слеша
				if (pusStr->Buffer[i] != L'u')	{ // Ошибка - за слешем должен идти символ u
					i--;
					usTmp.Buffer[Poz] = pusStr->Buffer[i];
					Poz++;
					continue;				
				}
				i++; // пропуск "u"
				// Выделение временной строки
				UNICODE_STRING usTmpNum;
				AllocateUnicodeString(&usTmpNum, 16);
				ClearUnicodeString(&usTmpNum);
				// Чтение 4х символов - код Unicode символа
				for (int j = 0; j<4; j++)
					usTmpNum.Buffer[j] = pusStr->Buffer[i+j];
				usTmpNum.Length = 8;
				// Пропуск кода в строке
				i += 3;
				// Преобразование кода в число
				ULONG Value = 0;
				// Вставка полученного кода в строку при условии успешной конвертации
				if (IsValidHexNumber(&usTmpNum) && RtlUnicodeStringToInteger(&usTmpNum, 16, &Value) == STATUS_SUCCESS) {
					zDbgPrint("Value = %X", Value);
					usTmp.Buffer[Poz] = (WCHAR)(WORD)Value;
					Poz++;
				} else 
				{
					i -= 5;
					for (int j = 0; j<6; j++) {
						usTmp.Buffer[Poz] = pusStr->Buffer[i];
						Poz++;
						i++;
					}	
					i--;
				};				
				// Освобождение временной строки
				FreeUnicodeString(&usTmpNum);
			}		
			usTmp.Length = Poz * 2;
			zDbgPrint("Prepared buffer = %S, length = %X", usTmp.Buffer, usTmp.Length);
			// Копирование полученной строки в результат)
			RtlCopyUnicodeString(pusStr, &usTmp);
	} __except(EXCEPTION_EXECUTE_HANDLER) {	return; }
	// Освобождение временной строки
	FreeUnicodeString(&usTmp);
}

// Чтение параметра реестра с указанным именем
NTSTATUS zReadRegStr(IN PUNICODE_STRING pusRegistryPath, IN PUNICODE_STRING pusParam, OUT PUNICODE_STRING pusValue)
{
	zDbgPrint("zReadRegStr");
	NTSTATUS res = STATUS_UNSUCCESSFUL;
	// Контроль указателей
	if (!MmIsAddressValid(pusRegistryPath)) return STATUS_UNSUCCESSFUL;
	if (!MmIsAddressValid(pusParam)) return STATUS_UNSUCCESSFUL;
	if (!MmIsAddressValid(pusValue)) return STATUS_UNSUCCESSFUL;
	__try {
		// Подготовка атрибутов объекта "Ключ рестра"
		OBJECT_ATTRIBUTES ob;
		InitializeObjectAttributes(&ob, pusRegistryPath, OBJ_CASE_INSENSITIVE, NULL, NULL);
		HANDLE KeyHandle;
		// Открытие ключа
		res = ZwOpenKey(&KeyHandle, KEY_QUERY_VALUE, &ob);
		zDbgPrint("ZwOpenKey  = %X", res);
		if (NT_SUCCESS(res)) {
			// Выделение памяти (заголовок + длина строки)
			KEY_VALUE_PARTIAL_INFORMATION *pKeyInfo;
			// Размер буфера памяти
			ULONG InfoSize = sizeof(KEY_VALUE_PARTIAL_INFORMATION) + pusValue->MaximumLength;
			pKeyInfo = (KEY_VALUE_PARTIAL_INFORMATION *)ExAllocatePool(PagedPool, InfoSize);
			// Память выделена ? Тогда пробуем читать данные
			if (pKeyInfo != NULL) 
			{
				// Чтение ключа 
				res = ZwQueryValueKey(KeyHandle, pusParam, KeyValuePartialInformation, pKeyInfo, InfoSize, &InfoSize);
				zDbgPrint("ZwQueryValueKey (%S) = %X, Type=%X", pusParam->Buffer, res, pKeyInfo->Type);
				if (res == STATUS_SUCCESS && 
					(pKeyInfo->Type == REG_SZ || pKeyInfo->Type == REG_EXPAND_SZ) && 
					pusValue->MaximumLength >= pKeyInfo->DataLength) 
				{
					RtlCopyMemory(pusValue->Buffer, pKeyInfo->Data, pKeyInfo->DataLength);
					pusValue->Length = (USHORT)pKeyInfo->DataLength-2;
					zDbgPrint("ZwQueryValueKey, STR = (%S)", pusValue->Buffer);
				}
				else
					res = STATUS_UNSUCCESSFUL;
				// Освобождение памяти
				ExFreePool(pKeyInfo);
			}
			ZwClose(KeyHandle);
		}
		return res; 
	} __except(EXCEPTION_EXECUTE_HANDLER) {	return STATUS_UNSUCCESSFUL; }
}

// Удаление файла с указанным именем
NTSTATUS zDeleteFile(IN PUNICODE_STRING pusFileName)
{
	NTSTATUS status;
	PFLT_INSTANCE pInstance;
	HANDLE hFile = NULL;
	OBJECT_ATTRIBUTES fileOA;
	IO_STATUS_BLOCK ioSB;

	pInstance = GetInstanceForFile( pusFileName );
	if ( !pInstance )
		return STATUS_UNSUCCESSFUL;

	InitializeObjectAttributes(
		&fileOA,
		pusFileName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
		);
	status = FltCreateFile(
			g_pFilterObj,
			pInstance,
			&hFile,
			DELETE,
			&fileOA,
			&ioSB,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			FILE_OPEN,
			FILE_DELETE_ON_CLOSE,
			NULL,
			0,
			0
		);
	if ( NT_SUCCESS(status) )
		FltClose( hFile );

	FltObjectDereference( pInstance );

	return status;
}

// копирование файлов
NTSTATUS zCopyFile(IN PUNICODE_STRING pusFileName1, IN PUNICODE_STRING pusFileName2)
{
	LastFileSize = 0;

// get instances here
	PFLT_INSTANCE	pInstance1,
					pInstance2;

	pInstance1 = GetInstanceForFile( pusFileName1 );
	if ( !pInstance1 )
		return STATUS_UNSUCCESSFUL;
	pInstance2 = GetInstanceForFile( pusFileName2 );
	if ( !pInstance2 )
	{
		FltObjectDereference( pInstance1 );
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status;
	OBJECT_ATTRIBUTES	file1OA,
						file2OA;

	InitializeObjectAttributes(
		&file1OA,
		pusFileName1,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
		);
	InitializeObjectAttributes(
		&file2OA,
		pusFileName2,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
		);

	HANDLE	hFile1 = NULL,
			hFile2 = NULL;
	IO_STATUS_BLOCK ioSB;
	PFILE_OBJECT	pFileObject1 = NULL,
					pFileObject2 = NULL;
// Открытие исходного файла на чтение
	status = FltCreateFile(
		g_pFilterObj,
		pInstance1,
		&hFile1,
		SYNCHRONIZE | FILE_READ_DATA,
		&file1OA,
		&ioSB,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		FILE_OPEN, 
		FILE_SYNCHRONOUS_IO_NONALERT, 
		NULL,
		0,
		0
		);
	if ( NT_SUCCESS(status) )
	{
		status = ObReferenceObjectByHandle(
			hFile1,
			0,
			*IoFileObjectType,
			KernelMode,
			(PVOID *) &pFileObject1,
			NULL
			);
		if ( NT_SUCCESS(status) )
		{
			// Создание целевого файла
			status = FltCreateFile(
				g_pFilterObj,
				pInstance2,
				&hFile2,
				SYNCHRONIZE | DELETE | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES,
				&file2OA,
				&ioSB,
				NULL,
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
				FILE_SUPERSEDE, 
				FILE_SYNCHRONOUS_IO_NONALERT, 
				NULL,
				0,
				0
				);
			if ( NT_SUCCESS(status) )
			{
				status = ObReferenceObjectByHandle(
					hFile2,
					0,
					*IoFileObjectType,
					KernelMode,
					(PVOID *) &pFileObject2,
					NULL
					);
				if ( NT_SUCCESS(status) )
				{
					// Выделение памяти для буфера
					ULONG BufSize = 64 * 1024;
					PVOID Buf = ExAllocatePoolWithTag(PagedPool, BufSize, 'BFbz');
					if ( !Buf )
						status = STATUS_INSUFFICIENT_RESOURCES;
					else
					{
						ULONG cbRead;

						// *** Копирование ***
						do {
							// Чтение в буфер очередной порции данных
							cbRead = 0;
							status = FltReadFile(
								pInstance1,
								pFileObject1,
								NULL,
								BufSize,
								Buf,
								0,
								&cbRead,
								NULL,
								NULL
								);

							if ( NT_SUCCESS(status) && 0 != cbRead )
							{
								ULONG cbWritten = 0;

								LastFileSize += cbRead;
								status = FltWriteFile(
									pInstance2,
									pFileObject2,
									NULL,
									cbRead,
									Buf,
									0,
									&cbWritten,
									NULL,
									NULL
									);
							}
						} while ( NT_SUCCESS(status) && BufSize == cbRead );

						ExFreePool( Buf );
					}

					ObDereferenceObject( pFileObject2 );
				}

				FltClose( hFile2 );
			}

			ObDereferenceObject( pFileObject1 );
		}

		FltClose( hFile1 );
	}

	FltObjectDereference( pInstance1 );
	FltObjectDereference( pInstance2 );

	return status;
}

// Самоуничтожение
NTSTATUS zDeleteDriverParams(IN PUNICODE_STRING pusRegistryPath)
{
	// Указатель на строку некорректный - тогда выход
	if (!MmIsAddressValid(pusRegistryPath)) return STATUS_UNSUCCESSFUL;
	zDbgPrint("zDeleteDriverKey = %S", pusRegistryPath->Buffer);
	NTSTATUS ns;
	// Удаление параметров ключа, отвечающего за автозапуск драйвера
	ns = RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, usImagePath.Buffer);
	ns = RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, L"Start");
	ns = RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, L"Type");
	ns = RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, L"DisplayName");
	ns = RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, L"ErrorControl");
	return STATUS_SUCCESS;
}

// Удаление драйвера
NTSTATUS zDeleteDriver(IN PUNICODE_STRING pusDriver, BOOLEAN DeleteFile)
{
	__try {
		ULONG ns = STATUS_UNSUCCESSFUL;
		// Указатель на строку некорректный - тогда выход
		if (!MmIsAddressValid(pusDriver)) return STATUS_UNSUCCESSFUL;
		if (!MmIsAddressValid(pusDriver->Buffer)) return STATUS_UNSUCCESSFUL;
		zDbgPrint("zDeleteDriver (%S)", pusDriver->Buffer);
		// Подготовка имени драйвера
		WCHAR wszTmp[500] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
		wcscat(wszTmp, pusDriver->Buffer);
		UNICODE_STRING TmpKey;
		RtlInitUnicodeString(&TmpKey, wszTmp);
		// Удаление файла драйвера
		if (DeleteFile) {
			UNICODE_STRING TmpUS;
			AllocateUnicodeString(&TmpUS, 1024);
			if (zReadRegStr(&TmpKey, &usImagePath, &TmpUS) == STATUS_SUCCESS) {
				ns = zDeleteFile(&TmpUS);
				AddToLog(L"Delete File ", &TmpUS, &ns);
			}
			FreeUnicodeString(&TmpUS);
		}
		// Удаление ключа реестра драйвера
		zDeleteDriverParams(&TmpKey);
		ns = zRegDeleteKey(&TmpKey);
		return ns;
	} __except(EXCEPTION_EXECUTE_HANDLER) {	return STATUS_UNSUCCESSFUL; }
}

// Карантин файла
NTSTATUS zQuarantineFile(IN PUNICODE_STRING pusFileName)
{
	if (!EnableQr) 
		return STATUS_UNSUCCESSFUL;

	ULONG ns = STATUS_UNSUCCESSFUL, CopyNS;
	// Выделение памяти для строк
	PWCHAR TmpStr = NULL, Buffer = NULL;

	TmpStr = (PWCHAR)ExAllocatePool(PagedPool, 1024);
	if (TmpStr == NULL) 
		return STATUS_UNSUCCESSFUL;
	Buffer = (PWCHAR)ExAllocatePool(PagedPool, 1024);
	if (Buffer == NULL)
	{
		ExFreePool(TmpStr);
		return STATUS_UNSUCCESSFUL;
	}

	WCHAR FileName[64];		
	// Генерация имени файла
	swprintf(FileName, L"bcqr%.5d", QrCounter);
	// Сборка имени DAT файла
	wcscpy(TmpStr, usQrPath.Buffer);
	wcscat(TmpStr, FileName);
	wcscat(TmpStr, L".dta");
	QrCounter++;
	zDbgPrint("DTA file name = (%S)", TmpStr);
	UNICODE_STRING TmpUS;	
	RtlInitUnicodeString(&TmpUS, TmpStr);
	// Копирование файла
	CopyNS = zCopyFile(pusFileName, &TmpUS);
	// **** Создание INI файла-описания ****
	// Сборка имени INI файла
	wcscpy(TmpStr, usQrPath.Buffer);
	wcscat(TmpStr, FileName);
	wcscat(TmpStr, L".ini");
	RtlInitUnicodeString(&TmpUS, TmpStr);
	zDbgPrint("INI file name = (%S)", TmpUS.Buffer);

	NTSTATUS statusINI;

	PFLT_INSTANCE pInstance = GetInstanceForFile( &TmpUS );
	if ( pInstance )
	{
		// Подготовка атрибутов файла
		OBJECT_ATTRIBUTES inifileOA;
		HANDLE hINIFile = NULL;
		IO_STATUS_BLOCK ioSB;

		InitializeObjectAttributes(
			&inifileOA,
			&TmpUS,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,
			NULL
			);

		// Создание/открытие файла
		statusINI = FltCreateFile(
			g_pFilterObj,
			pInstance,
			&hINIFile,
			SYNCHRONIZE | DELETE | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES,
			&inifileOA,
			&ioSB,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			0, 
			FILE_SUPERSEDE, 
			FILE_SYNCHRONOUS_IO_NONALERT, 
			NULL,
			0,
			0
			);
		zDbgPrint("INI: ZwCreateFile = (%X)", statusINI);
		if ( NT_SUCCESS(statusINI) )
		{
			PFILE_OBJECT pINIFileObject = NULL;

			statusINI = ObReferenceObjectByHandle(
				hINIFile,
				0,
				*IoFileObjectType,
				KernelMode,
				(PVOID *) &pINIFileObject,
				NULL
				);
			if ( NT_SUCCESS(statusINI) )
			{
				// Подготовка буфера с описанием файла
				wcscpy(Buffer, L"[InfectedFile]");
				wcscat(Buffer, L"\r\n");
				wcscat(Buffer, L"Src=");
				wcscat(Buffer, pusFileName->Buffer);
				wcscat(Buffer, L"\r\n");
				wcscat(Buffer, L"Infected=");
				wcscat(Buffer, FileName);
				wcscat(Buffer, L".dat");
				wcscat(Buffer, L"\r\n");
				wcscat(Buffer, L"Virus=BootCleaner quarantine");
				wcscat(Buffer, L"\r\n");
				WCHAR wszTmp[15];
				swprintf(wszTmp, L"Size=%i", LastFileSize);
				wcscat(Buffer, wszTmp);
				wcscat(Buffer, L"\r\n");
				swprintf(wszTmp, L"CopyStatus=%X", CopyNS);
				wcscat(Buffer, wszTmp);
				zDbgPrint("INI text = (%S)", Buffer);

				// Преобразование буфера в из UNICODE -> ANSI
				ANSI_STRING		AnsiString;
				UNICODE_STRING  UnicodeString;
				RtlInitUnicodeString(&UnicodeString, Buffer);
				if (RtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, TRUE) == STATUS_SUCCESS)	  
				{
					ULONG cbWritten;

					// Запись AnsiString 
					statusINI = FltWriteFile(
						pInstance,
						pINIFileObject,
						NULL,
						AnsiString.Length,
						AnsiString.Buffer,
						0,
						&cbWritten,
						NULL,
						NULL
						);
				}

				ObDereferenceObject( pINIFileObject );
			}

			FltClose(hINIFile);
		}

		FltObjectDereference( pInstance );
	}

	ExFreePool(TmpStr);
	ExFreePool(Buffer);

	return CopyNS;
}

// Карантин драйвера
NTSTATUS zQRDriver(IN PUNICODE_STRING pusDriver)
{
	__try {
		ULONG ns = STATUS_UNSUCCESSFUL;
		// Указатель на строку некорректный - тогда выход
		if (!MmIsAddressValid(pusDriver)) return STATUS_UNSUCCESSFUL;
		if (!MmIsAddressValid(pusDriver->Buffer)) return STATUS_UNSUCCESSFUL;
		zDbgPrint("zQrDriver (%S)", pusDriver->Buffer);
		// Подготовка имени драйвера
		WCHAR wszTmp[500] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
		wcscat(wszTmp, pusDriver->Buffer);
		UNICODE_STRING TmpKey;
		RtlInitUnicodeString(&TmpKey, wszTmp);
		// Карантин
		UNICODE_STRING TmpUS;
		AllocateUnicodeString(&TmpUS, 1024);
		if (zReadRegStr(&TmpKey, &usImagePath, &TmpUS) == STATUS_SUCCESS) {
			ns = zQuarantineFile(&TmpUS);
			AddToLog(L" Quarantine File ", &TmpUS, &ns);
		}
		FreeUnicodeString(&TmpUS);
		return ns;
	} __except(EXCEPTION_EXECUTE_HANDLER) {	return STATUS_UNSUCCESSFUL; }
}

// Отключение драйвера
NTSTATUS zDisableDriver(IN PUNICODE_STRING pusDriver)
{
	__try {
		ULONG x = SERVICE_DISABLED;
		// Запись в параметр Start кода 4
		return RtlWriteRegistryValue(RTL_REGISTRY_SERVICES,
			pusDriver->Buffer,
			L"Start",
			REG_DWORD,
			&x,
			4);
	} __except(EXCEPTION_EXECUTE_HANDLER) {	return STATUS_UNSUCCESSFUL; }
}

// Самоуничтожение
NTSTATUS zSelfDestroy(IN PUNICODE_STRING pusRegistryPath)
{
	// Указатель на строку некорректный - тогда выход
	if (!MmIsAddressValid(pusRegistryPath)) return STATUS_UNSUCCESSFUL;
	zDbgPrint("zSelfDestroy = %S", pusRegistryPath->Buffer);
	NTSTATUS ns;
	// Удаление своего файла драйвера
	UNICODE_STRING TmpUS;
	AllocateUnicodeString(&TmpUS, 1024);	
	if (zReadRegStr(pusRegistryPath, &usImagePath, &TmpUS) == STATUS_SUCCESS)
		zDeleteFile(&TmpUS);
	FreeUnicodeString(&TmpUS);
	// Удаление параметров ключа, отвечающего за автозапуск драйвера
	ns = zDeleteDriverParams(pusRegistryPath);
	return ns;
}

// Выполнить карантин файлов. pusRegistryPath - ключ с "канвой", DeteteKey - признак надобности удаления заданий
extern "C"
NTSTATUS ExecuteQuarantine(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey)
{
	// Подготовка строк
	WCHAR wszTmp[100];
	UNICODE_STRING TmpKey, TmpUS;	
	AllocateUnicodeString(&TmpUS, 1024);
	// ********* Цикл карантина служб и драйверов **********
	int i = 1;
	ULONG ns = 0;
	i = 1;
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"QrSvc%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Удаление драйвера
		PrepareParamString(&TmpUS);
		ns = zQRDriver(&TmpUS);
		AddToLog(L"Quarantine service file ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием 
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}
	// ******* Цикл карантина файлов ******
	i = 1;
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"QrFile%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Карантин файла
		ns = zQuarantineFile(&TmpUS);
		AddToLog(L"QuarantineFile ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}
	// Освобождение памяти
	FreeUnicodeString(&TmpUS);
	return STATUS_SUCCESS;
}

// Выполнить копирование файлов. pusRegistryPath - ключ с "канвой", DeteteKey - признак надобности удаления заданий
extern "C"
NTSTATUS ExecuteCopyFiles(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey)
{
	// Подготовка строк
	WCHAR wszTmp[100];
	UNICODE_STRING TmpKey, TmpUS;	
	AllocateUnicodeString(&TmpUS, 1024);
	// ********* Цикл карантина служб и драйверов **********
	int i = 1;
	ULONG ns = 0;
	while (i < 1024) 
	{
		// Подготовка имени параметра 1
		swprintf(wszTmp, L"CpFileF%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера 1
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Удаление ключа реестра 1 с заданием
		RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		// Подготовка имени параметра 2
		UNICODE_STRING TmpUS1;	
		AllocateUnicodeString(&TmpUS1, 1024);
		swprintf(wszTmp, L"CpFileT%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS1);
		// Чтение парамера 1
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS1) != STATUS_SUCCESS) break;
		// Копирования файла
		ns = zCopyFile(&TmpUS, &TmpUS1);
		AddToLog(L"Copy File ", &TmpUS, &ns);
		FreeUnicodeString(&TmpUS1);
		// Удаление ключа реестра 2 с заданием
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}

	// Освобождение памяти
	FreeUnicodeString(&TmpUS);
	return STATUS_SUCCESS;
}

// Выполнить удаление файлов. pusRegistryPath - ключ с "канвой", DeteteKey - признак надобности удаления заданий
extern "C"
NTSTATUS ExecuteDeleteFiles(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey)
{
	// Подготовка строк
	WCHAR wszTmp[100];
	UNICODE_STRING TmpKey, TmpUS;	
	AllocateUnicodeString(&TmpUS, 1024);
	// ********* Цикл карантина служб и драйверов **********
	int i = 1;
	ULONG ns = 0;
	// ******* Цикл удаления файлов ******
	i = 1;
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"DelFile%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Удаление файла
		ns = zDeleteFile(&TmpUS);
		AddToLog(L"DeleteFile ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}
	// Освобождение памяти
	FreeUnicodeString(&TmpUS);
	return STATUS_SUCCESS;
}

// Выполнить удаление драйверов. pusRegistryPath - ключ с "канвой", DeteteKey - признак надобности удаления заданий
extern "C"
NTSTATUS ExecuteDeleteDrivers(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey)
{
	// Подготовка строк
	WCHAR wszTmp[100];
	UNICODE_STRING TmpKey, TmpUS;	
	AllocateUnicodeString(&TmpUS, 1024);
	// ********* Цикл карантина служб и драйверов **********
	int i = 1;
	ULONG ns = 0;

	// ******* Цикл удаления служб и драйверов ******
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"DelSvc%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Удаление драйвера
		PrepareParamString(&TmpUS);
		ns = zDeleteDriver(&TmpUS, TRUE);
		AddToLog(L"Delete Service & File ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием 
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}

	// Освобождение памяти
	FreeUnicodeString(&TmpUS);
	return STATUS_SUCCESS;
}

// Выполнить удаление драйверов. pusRegistryPath - ключ с "канвой", DeteteKey - признак надобности удаления заданий
extern "C"
NTSTATUS ExecuteREGOper(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey)
{
	// Подготовка строк
	WCHAR wszTmp[100];
	UNICODE_STRING TmpKey, TmpUS;	
	AllocateUnicodeString(&TmpUS, 1024);
	// ********* Цикл карантина служб и драйверов **********
	int i = 1;
	ULONG ns = 0;
	// ******* Цикл удаления служб и драйверов (на уровне ключа реестра) ******
	i = 1;
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"DelSvcReg%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Удаление драйвера
		PrepareParamString(&TmpUS);
		ns = zDeleteDriver(&TmpUS, false);
		AddToLog(L"Delete Service ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием 
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}

	// ******* Цикл удаления ключей реестра ******
	i = 1;
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"DelRegKey%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Удаление ключа
		ns = zRegDeleteKey(&TmpUS);
		AddToLog(L"Delete RegKey ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием 
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}
	// ******* Цикл блокировки служб и драйверов ******
	i = 1;
	while (i < 1024) 
	{
		// Подготовка имени параметра
		swprintf(wszTmp, L"DisableSvc%i", i);
		RtlInitUnicodeString(&TmpKey, wszTmp);
		ClearUnicodeString(&TmpUS);
		// Чтение парамера
		if (zReadRegStr(pusRegistryPath, &TmpKey, &TmpUS) != STATUS_SUCCESS) break;
		// Блокировка драйвера
		PrepareParamString(&TmpUS);
		ns = zDisableDriver(&TmpUS);
		AddToLog(L"Disable Service ", &TmpUS, &ns);
		// Удаление ключа реестра с заданием 
		if (DeteteKey || ns == STATUS_SUCCESS)
			RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pusRegistryPath->Buffer, wszTmp);
		i++;
	}
	// Освобождение памяти
	FreeUnicodeString(&TmpUS);
	return STATUS_SUCCESS;
}

extern "C"
VOID AVZBCInit( __in PUNICODE_STRING pusRegistryPath )
{
	AllocateUnicodeString(&usLogFile, 1024);	
	AllocateUnicodeString(&usQrPath, 1024);

	// Чтение настроек протоколирования
	UNICODE_STRING TmpKey;	

	RtlInitUnicodeString(&TmpKey, L"LogFile");
	if (zReadRegStr(pusRegistryPath, &TmpKey, &usLogFile) == STATUS_SUCCESS) {
		zDbgPrint("LogFile = %S", usLogFile.Buffer);
		EnableLog = TRUE;
	}

	// Настройка карантина
	RtlInitUnicodeString(&TmpKey, L"QrPath");
	if (zReadRegStr(pusRegistryPath, &TmpKey, &usQrPath) == STATUS_SUCCESS) {
		zDbgPrint("QrPath = %S", usQrPath.Buffer);
		EnableQr = TRUE;
		AddToLog(L"Quarantine path: ", &usQrPath, NULL);
	}
}

extern "C"
VOID AVZBCUnInit()
{
	if ( g_bLogInited )
	{
		ObDereferenceObject( g_pLogFileObject );
		FltClose( g_hLogFile );
		FltObjectDereference( g_pLogFileInstance );
	}

	FreeUnicodeString(&usLogFile);	
	FreeUnicodeString(&usQrPath);		
}
