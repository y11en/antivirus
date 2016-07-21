// (С) Зайцев Олег, 2006
#include "StdAfx.h"
#include <ntddk.h>
#include <ntdef.h>
#include <avzrkm.h>

// Необходимо для совместимости с NT4, который не поддерживает *WithTag ...
#undef ExFreePool
#undef ExAllocatePool

// Имя устройства (в формате UnicodeString, формируется макросом)
PRESET_UNICODE_STRING (usDeviceName,       CSTRING (\\Device\\h7uiW4oq));
// Имя символьной ссыки (в формате UnicodeString, формируется макросом)
PRESET_UNICODE_STRING (usSymbolicLinkName, CSTRING (\\??\\h7uiW4oqx));
// Имя переменной и именем функции
PRESET_UNICODE_STRING (usZwQueryInformationProcess, CSTRING (ZwQueryInformationProcess));
PRESET_UNICODE_STRING (usObOpenObjectByPointer, CSTRING (ObOpenObjectByPointer));
PRESET_UNICODE_STRING (usZwOpenProcess, CSTRING (ZwOpenProcess));
PRESET_UNICODE_STRING (usZwTerminateProcess, CSTRING (ZwTerminateProcess));
PRESET_UNICODE_STRING (usPsSetLoadImageNotifyRoutine, CSTRING (PsSetLoadImageNotifyRoutine));
PRESET_UNICODE_STRING (usPsRemoveLoadImageNotifyRoutine, CSTRING (PsRemoveLoadImageNotifyRoutine));
PRESET_UNICODE_STRING (usRtlGetVersion, CSTRING (RtlGetVersion));


// Глобальные переменные - указатели на объекты "Устройство"
PDEVICE_OBJECT  gpDeviceObject  = NULL;
PDEVICE_CONTEXT gpDeviceContext = NULL;

// Указатель на функцию ZwQueryInformationProcess
PZwQueryInformationProcess		ZwQueryInformationProcess = NULL;
PObOpenObjectByPointer			ObOpenObjectByPointer = NULL;
PZwOpenProcess					ZwOpenProcess = NULL;
PZwTerminateProcess				ZwTerminateProcess = NULL;
PPsRemoveLoadImageNotifyRoutine	zPsRemoveLoadImageNotifyRoutine = NULL;
PPsSetLoadImageNotifyRoutine	zPsSetLoadImageNotifyRoutine = NULL;
PRtlGetVersion                  zRtlGetVersion = NULL;

extern "C" 	{
	extern  PWORD NtBuildNumber;       // Номер сборки NT
	extern  PVOID *MmSystemRangeStart; // Нижняя граница ядра
}

// Массив процессов
PROCESS_ITEM PROCESS_LIST[0xFF];

// Указатель на "голову" списка драйверов
PDRIVER_LIST_ITEM DRIVER_LIST = NULL;

PVOID  pkThread = NULL;
BOOLEAN TerminateThreadsFlag = FALSE;

// Смещения в структуре EPROCESS
ULONG ActiveProcessLinkOffset = 0; // Структура ActiveProcessLinks
ULONG ProcessNameOffset = 0;       // Имя процесса
ULONG FullProcessNameOffset = 0;   // Полное имя процесса
ULONG PIDOffset = 0;               // PID процесса
ULONG PEBOffset = 0;               // PEB процесса

ULONG TrustedPID = 0;				// PID процесса, который имеет право на управление драйвером
PDRIVER_UNLOAD pDriverUnload = NULL; // Указатель на процедуру выгрузки драйвера

// Адрес, с которого начинается пространство ядра
ULONG KernelStartAddr = 0x800000;

RTL_OSVERSIONINFOEXW VersionInfo;

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

// Освобождение памяти Unicoce строки
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

// Получение полного имени процесса - по EPROCESS
NTSTATUS GetProcessExeName(PVOID pEPROCESS, PUNICODE_STRING ProcessImageName)
{
	NTSTATUS status;
	ULONG returnedLength;
	ULONG bufferLength;
	PVOID buffer = NULL;
	PUNICODE_STRING imageName;
	HANDLE Handle = 0; 

	if (!ZwQueryInformationProcess || !ObOpenObjectByPointer) 
		return STATUS_UNSUCCESSFUL;

	status = ObOpenObjectByPointer(pEPROCESS,0,0,1,0,0,&Handle);

	if (status != STATUS_SUCCESS) {
		return status; 
	}
	__try {

		// Первый вызов - определение размера
		status = ZwQueryInformationProcess( Handle, 
			ProcessImageFileName,
			NULL, // buffer
			0, // buffer size
			&returnedLength);

		if (STATUS_INFO_LENGTH_MISMATCH != status)
			return status;

		// Вычисление размера буфера
		bufferLength = returnedLength - sizeof(UNICODE_STRING);
		// Распределение памяти
		AllocateUnicodeString(ProcessImageName, (USHORT)bufferLength);
		// Котроль
		if (!ProcessImageName->Buffer || ProcessImageName->MaximumLength < bufferLength) 
			return STATUS_UNSUCCESSFUL;

		// Выделение временного буфера для строки
		buffer = ExAllocatePool(PagedPool, returnedLength);
		// Контроль
		if (NULL == buffer) {
			FreeUnicodeString(ProcessImageName);
			return STATUS_UNSUCCESSFUL;
		}

		// Запрос данных
		status = ZwQueryInformationProcess( Handle, 
			ProcessImageFileName,
			buffer,
			returnedLength,
			&returnedLength);
	} __finally {
		if (Handle != 0)
			ZwClose(Handle);
	}

	if (status == STATUS_SUCCESS) {
		imageName = (PUNICODE_STRING) buffer;		
		RtlCopyUnicodeString(ProcessImageName, imageName);
		zDbgPrint("Image Name = %S", ProcessImageName->Buffer);        
	}

	// Осовобождение пула
	ExFreePool(buffer);

	return status;
}

// Получение полного имени процесса - по PID
NTSTATUS GetProcessExeNameByPID(DWORD APID, PUNICODE_STRING ProcessImageName)
{
	NTSTATUS status;
	ULONG returnedLength;
	ULONG bufferLength;
	PVOID buffer = NULL;
	PUNICODE_STRING imageName;
	HANDLE Handle = 0; 

	if (!ZwQueryInformationProcess || !ZwOpenProcess) 
		return STATUS_UNSUCCESSFUL;

	CLIENT_ID clientId; 
	OBJECT_ATTRIBUTES oa; 
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL); 
	clientId.UniqueProcess = (HANDLE)APID; 
	clientId.UniqueThread = NULL; 


	status = ZwOpenProcess(&Handle, PROCESS_ALL_ACCESS, &oa, &clientId); 
	if (status != STATUS_SUCCESS) {
		return status; 
	}
	__try {

		// Первый вызов - определение размера
		status = ZwQueryInformationProcess( Handle, 
			ProcessImageFileName,
			NULL, // buffer
			0, // buffer size
			&returnedLength);

		if (STATUS_INFO_LENGTH_MISMATCH != status)
			return status;

		// Вычисление размера буфера
		bufferLength = returnedLength - sizeof(UNICODE_STRING);
		// Распределение памяти
		AllocateUnicodeString(ProcessImageName, (USHORT)bufferLength);
		// Контроль
		if (!ProcessImageName->Buffer || ProcessImageName->MaximumLength < bufferLength) 
			return STATUS_UNSUCCESSFUL;

		// Выделение временного буфера для строки
		buffer = ExAllocatePool(PagedPool, returnedLength);
		// Контроль
		if (NULL == buffer) {
			FreeUnicodeString(ProcessImageName);
			return STATUS_UNSUCCESSFUL;
		}

		// Запрос данных
		status = ZwQueryInformationProcess( NtCurrentProcess(), 
			ProcessImageFileName,
			buffer,
			returnedLength,
			&returnedLength);
	} __finally {
		if (Handle != 0)
			ZwClose(Handle);
	}

	if (status == STATUS_SUCCESS) {
		imageName = (PUNICODE_STRING) buffer;		
		RtlCopyUnicodeString(ProcessImageName, imageName);
		zDbgPrint("Image Name = %S", ProcessImageName->Buffer);        
	}

	// Осовобождение пула
	ExFreePool(buffer);

	return status;
}

// Инициализация списка процессов
VOID InitProcessList() 
{
	for (int i = 0; i < 0xFF; i++) {
		PROCESS_LIST[i].PID = 0xFFFFFFFF;
		PROCESS_LIST[i].EPROCESS = NULL;
		PROCESS_LIST[i].FULL_NAME.Buffer = NULL;
		PROCESS_LIST[i].FULL_NAME.Length = 0;
		PROCESS_LIST[i].FULL_NAME.MaximumLength = 0;
	}
}


// Добавление процесса в список
VOID AddToProcessList(DWORD APID, PVOID AEPROCESS) 
{
	for (int i = 0; i < 0xFF; i++)
		if (PROCESS_LIST[i].PID == 0xFFFFFFFF) {
			PROCESS_LIST[i].PID      = APID;
			PROCESS_LIST[i].EPROCESS = AEPROCESS;
			// Короткое имя процесса
			if (AEPROCESS != NULL &&
				(DWORD)AEPROCESS != 0xFFFFFFFF && 
				MmIsAddressValid(AEPROCESS) &&
				ProcessNameOffset != 0 &&
				MmIsAddressValid((PVOID)((DWORD)AEPROCESS + ProcessNameOffset))
				) 
				memcpy(&PROCESS_LIST[i].NAME, (PVOID)((DWORD)AEPROCESS + ProcessNameOffset), 16);

			if (AEPROCESS != NULL &&
				(DWORD)AEPROCESS != 0xFFFFFFFF && 
				MmIsAddressValid(AEPROCESS))
				if (GetProcessExeName((PVOID)AEPROCESS, &PROCESS_LIST[i].FULL_NAME) != STATUS_SUCCESS) {
					FreeUnicodeString(&PROCESS_LIST[i].FULL_NAME);
					GetProcessExeNameByPID(APID, &PROCESS_LIST[i].FULL_NAME);
				}
				return;
		}		
}

// Удаление процесса из списка
VOID DeleteFromProcessList(DWORD APID, PVOID AEPROCESS) 
{
	for (int i = 0; i < 0xFF; i++)
		if (PROCESS_LIST[i].EPROCESS == AEPROCESS) {
			PROCESS_LIST[i].PID = 0xFFFFFFFF;
			PROCESS_LIST[i].EPROCESS = (PVOID)0xFFFFFFFF;
			if (PROCESS_LIST[i].FULL_NAME.MaximumLength > 0) {
				FreeUnicodeString(&PROCESS_LIST[i].FULL_NAME);				
			}
			return;
		}
}

// **** Поиск адреса EPROCESS по PID процесса ****
PVOID SearchEPROCESSPtr(DWORD APID) 
{
	__try {
		// Смещения полей структуры EPROCESS
		if (ActiveProcessLinkOffset == 0 || PIDOffset == 0) 
			return NULL; 
		// Повышение привилегий
		KIRQL OldIRQL = KeRaiseIrqlToDpcLevel();			 
		// Поиск текущего процесса
		PEPROCESS   CurrentProcess    = PsGetCurrentProcess(); 
		if (!CurrentProcess) 
			return NULL;
		PLIST_ENTRY CurrentProcessAPL = (PLIST_ENTRY)((ULONG)CurrentProcess + ActiveProcessLinkOffset);
		PLIST_ENTRY ProcessAPL        = CurrentProcessAPL;
		ULONG ProcessPID;
		PVOID EPROCESSPtr = NULL;
		do  {
			// Контроль доступности памяти
			if (!MmIsAddressValid(ProcessAPL))
				break;
			// Поиск PID процесса
			ProcessPID  = *(PULONG)((ULONG)ProcessAPL - ActiveProcessLinkOffset + PIDOffset);
			if (ProcessPID == APID) {
				EPROCESSPtr = (PVOID)((DWORD)ProcessAPL - ActiveProcessLinkOffset);
				break;
			}
			// Переход на следующий EPROCESS
			ProcessAPL = ProcessAPL -> Flink;
		} while (ProcessAPL != CurrentProcessAPL);
		KeLowerIrql(OldIRQL);
		return EPROCESSPtr;

	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return NULL;
	}
}

// Добавление элемента списка драйверов
PDRIVER_LIST_ITEM AddDriverEntry(IN PUNICODE_STRING  FullImageName,
								 IN PIMAGE_INFO  ImageInfo)
{
	PDRIVER_LIST_ITEM Tmp = NULL;
	__try {
		// Выделение памяти
		Tmp = (PDRIVER_LIST_ITEM) ExAllocatePool(PagedPool, sizeof(DRIVER_LIST_ITEM));
		// Контроль успешности операции
		if (Tmp == NULL)
			return Tmp;
		// Выделение памяти для хранения строки с полным именем
		AllocateUnicodeString(&Tmp->FULL_NAME, FullImageName->MaximumLength);
		// Копирование строки
		if (Tmp->FULL_NAME.Buffer != NULL)
			RtlCopyUnicodeString(&Tmp->FULL_NAME, FullImageName);
		// Заполнение полей
		Tmp->ImageBase = ImageInfo->ImageBase;
		Tmp->ImageSize = ImageInfo->ImageSize;
		// Подключение к списку 
		Tmp->NextItem = DRIVER_LIST;
		Tmp->PrevItem = NULL;
		if (DRIVER_LIST != NULL)
			DRIVER_LIST->PrevItem = Tmp;
		DRIVER_LIST = Tmp;
		zDbgPrint("Driver item - add. Base=%X, Image Name = '%ws' \n", ImageInfo->ImageBase, FullImageName->Buffer);  
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return NULL;
	}
	return Tmp;
}

// Удаление элемента списка драйверов
VOID DeleteDriverEntry(IN PDRIVER_LIST_ITEM Item)
{
	// Список пуст ? Если да, то удалять нечего
	if (DRIVER_LIST == NULL) 
		return;
	// Проверка допустимости адреса элемента
	if (!MmIsAddressValid(Item)) 
		return;
	zDbgPrint("Driver item - delete. Base=%X, Image Name = '%ws' \n", Item->ImageBase, Item->FULL_NAME.Buffer);  
	// N-1 должен ссылаться на N+1
	if (Item->PrevItem != NULL && MmIsAddressValid(Item->PrevItem))
		((PDRIVER_LIST_ITEM)Item->PrevItem)->NextItem = Item->NextItem;
	// N+1 должен ссылаться на N-1
	if (Item->NextItem != NULL && MmIsAddressValid(Item->NextItem))
		((PDRIVER_LIST_ITEM)Item->NextItem)->PrevItem = Item->PrevItem;
	// Это первый элемент в цепочке ? Если да, то первым элементом станет N+1
	if (DRIVER_LIST == Item) {
		DRIVER_LIST = (PDRIVER_LIST_ITEM)Item->NextItem;
		zDbgPrint("Drivers list is empty !!");
	}
	// Освобождение строки 
	FreeUnicodeString(&Item->FULL_NAME);
	// Освобождние памяти, выделенной для хранения элемента
	ExFreePool(Item);
}


// Удаление элемента списка драйверов
NTSTATUS DeleteDriverEntryByImageBase(IN PVOID ImageBase)
{
	// Список пуст ? Если да, то удалять нечего
	if (DRIVER_LIST == NULL) 
		return STATUS_SUCCESS;
	// Проверка допустимости адреса элемента
	if (!MmIsAddressValid(DRIVER_LIST)) 
		return STATUS_UNSUCCESSFUL;

	__try {
		PDRIVER_LIST_ITEM Tmp = DRIVER_LIST, TekItem;
		// Сканирование списка
		while (Tmp) {
			TekItem = Tmp;
			Tmp = (PDRIVER_LIST_ITEM)Tmp->NextItem;
			// По этому базовому адресу ничего нет ?? Тогда удалим эту запись
			if (!MmIsAddressValid(TekItem) || (MmIsAddressValid(TekItem) && TekItem->ImageBase == ImageBase))
				DeleteDriverEntry(TekItem);
		}
		return STATUS_SUCCESS;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_UNSUCCESSFUL;
	}
}


// Удаление записей несуществующих драйверов 
NTSTATUS DeleteInvalidDrivers()
{
	// Список пуст ? Если да, то проверять нечего
	if (DRIVER_LIST == NULL) 
		return STATUS_UNSUCCESSFUL;
	__try {
		PDRIVER_LIST_ITEM Tmp = DRIVER_LIST, TekItem;
		// Сканирование списка
		while (Tmp != NULL) {
			TekItem = Tmp;
			Tmp = (PDRIVER_LIST_ITEM)Tmp->NextItem;
			// По этому базовому адресу ничего нет ?? Тогда удалим эту запись
			if (!MmIsAddressValid(TekItem->ImageBase))
				DeleteDriverEntry(TekItem);
		}
		return STATUS_SUCCESS;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_UNSUCCESSFUL;
	}
}

// Построение списка существующих драйверов 
NTSTATUS CreateDriversList(PVOID Buffer, PDWORD BufSize)
{
	// Список пуст ? Если да, то копировать нечего
	if (DRIVER_LIST == NULL) 
	{
		*BufSize = 0;
		return STATUS_SUCCESS;
	}
	__try {
		PDRIVER_LIST_ITEM Tmp = DRIVER_LIST;
		DWORD UsedBufSize = 0;
		// Сканирование списка
		while (Tmp != NULL) {
			// Если в буфере есть место, то копируем туда данные (первые 8 байт - базовый адрес + размер)
			if (UsedBufSize + 8 < *BufSize) 
			{
				memcpy((PVOID)((DWORD)Buffer + UsedBufSize), Tmp, 8);
				UsedBufSize += 8;
			}
			else 
				return STATUS_BUFFER_TOO_SMALL;
			Tmp = (PDRIVER_LIST_ITEM)Tmp->NextItem;
		}
		// Построение списка успешно - возвращаем объем, задействованный в буфере
		*BufSize = UsedBufSize;
		return STATUS_SUCCESS;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_UNSUCCESSFUL;
	}
}

// Поиск имени драйвера по базовому адресу
NTSTATUS GetDriverImageByBase(DWORD Base, PVOID Buffer, PDWORD BufSize)
{
	// Список пуст ? Если да, то копировать нечего
	if (DRIVER_LIST == NULL) 
	{
		*BufSize = 0;
		return STATUS_UNSUCCESSFUL;
	}
	__try {
		PDRIVER_LIST_ITEM Tmp = DRIVER_LIST;
		// Сканирование списка
		while (Tmp != NULL) {
			// Если в буфере есть место, то копируем туда данные (первые 8 байт - базовый адрес + размер)
			if ((DWORD)Tmp->ImageBase == Base) 
				if (Tmp->FULL_NAME.Length <= *BufSize)
				{
					if (!MmIsAddressValid(Tmp->FULL_NAME.Buffer))
						return STATUS_UNSUCCESSFUL;
					if (Tmp->FULL_NAME.Buffer != NULL) {
						memcpy(Buffer, Tmp->FULL_NAME.Buffer, Tmp->FULL_NAME.Length);
						*BufSize = Tmp->FULL_NAME.Length;
					} else 
						*BufSize = 0;
					return STATUS_SUCCESS;
				} else
					return STATUS_BUFFER_TOO_SMALL;
				Tmp = (PDRIVER_LIST_ITEM)Tmp->NextItem;
		}
		// Обход списка завершен и ничего не нашли - тогда ошибка
		return STATUS_UNSUCCESSFUL;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_UNSUCCESSFUL;
	}
}


// **** Мониторинг загрузки драйверов и образов EXE ****
VOID MyLoadImageNotifyRoutine(IN PUNICODE_STRING  FullImageName,
							  IN HANDLE  ProcessId, 
							  IN PIMAGE_INFO  ImageInfo)
{
	// Добавление драйвера в список
	if (ProcessId == 0) 
	{
		zDbgPrint("Load Image. PID = %d, Image Name = %ws \n", ProcessId, FullImageName->Buffer); 
		// Драйвером считаем только модуль в адресном пространстве ядра
		if ((DWORD)ImageInfo->ImageBase >= KernelStartAddr) {
			DeleteDriverEntryByImageBase(ImageInfo->ImageBase);
			AddDriverEntry(FullImageName, ImageInfo);
		}
	}
}

// **** Мониторинг создания/завершение процессов ****
VOID MyCreateProcessNotifyRoutine(IN HANDLE  ParentId,
								  IN HANDLE  ProcessId,
								  IN BOOLEAN  Create)
{
	/* PEPROCESS        EProcess;
    if(!NT_SUCCESS(PsLookupProcessByProcessId(Pid,&EProcess))) 
    {
        return 0;
    }
    ObDereferenceObject(EProcess);
    */
	__try {
		if (Create) {
			zDbgPrint("Create process.  ParentId = %d, ProcessId = %d \n", ParentId, ProcessId);
			// Добавление процесса
			AddToProcessList((DWORD)ProcessId, SearchEPROCESSPtr((DWORD)ProcessId));		
		}
		else {
			zDbgPrint("Delete process.  ParentId = %d, ProcessId = %d \n", ParentId, ProcessId);
			// Удаление процесса
			DeleteFromProcessList((DWORD)ProcessId, PsGetCurrentProcess());
		}
		zDbgPrint("Current PID = %d \n", PsGetCurrentProcessId());
		zDbgPrint("Current EPROCESS = %d \n", PsGetCurrentProcess());
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return;
	}
}

// Убиение процесса по EPROCESS
NTSTATUS KillProcessByEPROCESS(PVOID pEPROCESS)
{
	NTSTATUS           status    = STATUS_UNSUCCESSFUL; // Код возврата
	HANDLE Handle = 0; 

	__try {
		if (!ZwTerminateProcess || !ObOpenObjectByPointer) 
			return STATUS_UNSUCCESSFUL;

		// Контроль доступности памяти
		if (!MmIsAddressValid(pEPROCESS)) 
			return STATUS_UNSUCCESSFUL;

		status = ObOpenObjectByPointer(pEPROCESS,0,0,1,0,0,&Handle);

		if (status != STATUS_SUCCESS)
			return status;   

		status = ZwTerminateProcess(Handle, 0);
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_UNSUCCESSFUL;
	}
}

// Убиение процесса по EPROCESS
NTSTATUS KillProcessByPID(DWORD PID)
{
	NTSTATUS           status    = STATUS_UNSUCCESSFUL; // Код возврата
	HANDLE Handle = 0; 

	__try {
		if (!ZwTerminateProcess || !ObOpenObjectByPointer) 
			return STATUS_UNSUCCESSFUL;

		for (int i = 0; i < 0xFF; i++)
			if (PROCESS_LIST[i].PID == PID) {
				// Контроль доступности памяти
				if (!MmIsAddressValid(PROCESS_LIST[i].EPROCESS)) 
					return STATUS_UNSUCCESSFUL;
				status = ObOpenObjectByPointer(PROCESS_LIST[i].EPROCESS,0,0,1,0,0,&Handle);

				if (status != STATUS_SUCCESS)
					return status;   
				status = ZwTerminateProcess(Handle, 0);
			}
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_UNSUCCESSFUL;
	}
}

// ***** Обработчик событий Открытия/Закрытия/Очистки *****
NTSTATUS DispatchCreateCloseControl (PDEVICE_OBJECT pDeviceObject,
									 PIRP           pIrp)
{
	PIO_STACK_LOCATION pisl;
	DWORD              dInfo = 0;
	NTSTATUS           ns    = STATUS_NOT_IMPLEMENTED; // Код возврата

	// Получаем размещение IRP стека
	pisl = IoGetCurrentIrpStackLocation (pIrp);
	zDbgPrint("Device Open !");	
	// Проверяем код поддерживаемых функций
	switch (pisl->MajorFunction)
	{
	case IRP_MJ_CREATE:
	case IRP_MJ_CLEANUP:
	case IRP_MJ_CLOSE:
		{
			ns = STATUS_SUCCESS;
			break;
		}
	}

	// Завершаем IRP запрос
	pIrp->IoStatus.Status      = ns;
	pIrp->IoStatus.Information = dInfo;
	IoCompleteRequest (pIrp, IO_NO_INCREMENT);
	return ns;
}

// ***** Обработчик событий IO Control *****
NTSTATUS DispatchControl (PDEVICE_OBJECT pDeviceObject,
						  PIRP           pIrp)
{
	PIO_STACK_LOCATION pisl;
	DWORD              dInfo = 0;
	NTSTATUS           ns    = STATUS_NOT_IMPLEMENTED; // Код возврата
	// Получение расположения IRP стека
	pisl = IoGetCurrentIrpStackLocation (pIrp);

	// Входной буфер и его размер
	PVOID  InputBuffer         = pIrp->AssociatedIrp.SystemBuffer;
	ULONG  InputBufferLength   = pisl->Parameters.DeviceIoControl.InputBufferLength;
	// Выходной буфер и его размер
	PVOID  OutputBuffer        = pIrp->AssociatedIrp.SystemBuffer;
	ULONG  OutputBufferLength  = pisl->Parameters.DeviceIoControl.OutputBufferLength;
	// Код управление 
	ULONG  IoControlCode		  = pisl->Parameters.DeviceIoControl.IoControlCode;
	// Вывод данных для отладки
	zDbgPrint("IoControlCode = %x\n", IoControlCode);	
	zDbgPrint("InputBufferLength  = %x\n", InputBufferLength);	
	zDbgPrint("OutputBufferLength = %x\n", OutputBufferLength);	
	// Получение кода процесса
	HANDLE  CurrentProcess = PsGetCurrentProcess();

	// Настройка PID процесса, который может управлять драйвером
	if (IoControlCode == IOCTL_Z_SETTRUSTEDPID) {
		//Проверка размера входного буфера
		if (OutputBufferLength >= 16) 
		{
			DWORD TmpPID = 0xFFFFFFFF, X, X1, X2;
			memcpy(&TmpPID, InputBuffer, 4);
			memcpy(&X, (PVOID)((DWORD)InputBuffer+4), 4);
			memcpy(&X1, (PVOID)((DWORD)InputBuffer+8), 4);
			memcpy(&X2, (PVOID)((DWORD)InputBuffer+12), 4);
			DWORD Y = (X1*7+X2*3+761-TmpPID)*(X1+7) + TmpPID*2; 
			zDbgPrint("Control codes - Y=%d, X=%d, X1=%d, X2=%d", TmpPID, Y, X, X1, X2);
			if (X == Y && Y > 1500) {
				zDbgPrint("Trusted Process = %d", TmpPID);
				TrustedPID = TmpPID; 
				dInfo = 0;
				ns = STATUS_SUCCESS;         
			}
		}
	}

	if ((DWORD)PsGetCurrentProcessId() == TrustedPID) {
		// ***** Получение IOCTL_Z_GET_PROCESSLIST *****
		if (IoControlCode == IOCTL_Z_GET_PROCESSLIST) {
			zDbgPrint("IOCTL_Z_GET_PROCESSLIST \n");
			if (OutputBufferLength >= sizeof(PROCESS_LIST)) {				
				// Возврат списка процессов
				memcpy(OutputBuffer, &PROCESS_LIST, sizeof(PROCESS_LIST));
				dInfo = sizeof(PROCESS_LIST);
				ns = STATUS_SUCCESS;   
			} 
			else 
				ns = STATUS_BUFFER_TOO_SMALL; // Возврат ошибки
		}

		// ***** Получение текущего PID процесса из EPROCESS - IOCTL_Z_GET_EPROCESS_PID *****
		if (IoControlCode == IOCTL_Z_GET_EPROCESS_PID) {
			zDbgPrint("IOCTL_Z_GET_EPROCESS_PID \n");
			if (InputBufferLength >= 4 && OutputBufferLength >= 4) {				
				DWORD Indx;
				memcpy(&Indx, InputBuffer, 4);
				if (Indx >= 0 && Indx < 0xFF && 
					PROCESS_LIST[Indx].EPROCESS != NULL && 
					(DWORD)PROCESS_LIST[Indx].EPROCESS != 0xFFFFFFFF && 
					MmIsAddressValid(PROCESS_LIST[Indx].EPROCESS) &&
					MmIsAddressValid((PVOID)((DWORD)PROCESS_LIST[Indx].EPROCESS + PIDOffset))) 
				{
					// Возврат PID процесса, считанного из EPROCESS
					memcpy(OutputBuffer, (PVOID)((DWORD)PROCESS_LIST[Indx].EPROCESS + PIDOffset), 4);
					dInfo = 4;
					ns = STATUS_SUCCESS;
				}
				else 
					ns = STATUS_UNSUCCESSFUL; // Возврат ошибки

			} 
			else 
				ns = STATUS_BUFFER_TOO_SMALL; // Возврат ошибки
		}

		// ***** Получение текущего NAME процесса из EPROCESS - IOCTL_Z_GET_EPROCESS_NAME *****
		if (IoControlCode == IOCTL_Z_GET_EPROCESS_NAME) {
			zDbgPrint("IOCTL_Z_GET_EPROCESS_NAME \n");
			if (InputBufferLength >= 4 && OutputBufferLength >= 0x0F) {				
				DWORD Indx;
				memcpy(&Indx, InputBuffer, 4);
				if (Indx >= 0 && Indx < 0xFF && 
					PROCESS_LIST[Indx].EPROCESS != NULL && 
					(DWORD)PROCESS_LIST[Indx].EPROCESS != 0xFFFFFFFF && 
					MmIsAddressValid(PROCESS_LIST[Indx].EPROCESS) &&
					MmIsAddressValid((PVOID)((DWORD)PROCESS_LIST[Indx].EPROCESS + ProcessNameOffset))) 
				{
					// Возврат NAME процесса, считанного из EPROCESS
					memcpy(OutputBuffer, (PVOID)((DWORD)PROCESS_LIST[Indx].EPROCESS + ProcessNameOffset), 0x0F);
					dInfo = 0x0F;
					ns = STATUS_SUCCESS;
				}
				else 
					ns = STATUS_UNSUCCESSFUL; // Возврат ошибки

			} 
			else 
				ns = STATUS_BUFFER_TOO_SMALL; // Возврат ошибки
		}

		// ***** Получение полного имени процесса - IOCTL_Z_GET_EPROCESS_FULLNAME *****
		if (IoControlCode == IOCTL_Z_GET_EPROCESS_FULLNAME) {
			zDbgPrint("IOCTL_Z_GET_EPROCESS_FULLNAME \n");
			if (InputBufferLength >= 4) {				
				DWORD Indx;
				memcpy(&Indx, InputBuffer, 4);
				if (Indx >= 0 && Indx < 0xFF && 
					PROCESS_LIST[Indx].FULL_NAME.Buffer != NULL && 
					PROCESS_LIST[Indx].FULL_NAME.Length > 0 &&
					OutputBufferLength >= PROCESS_LIST[Indx].FULL_NAME.Length) 
				{
					// Возврат NAME процесса, считанного из EPROCESS
					memcpy(OutputBuffer, PROCESS_LIST[Indx].FULL_NAME.Buffer, PROCESS_LIST[Indx].FULL_NAME.Length);
					dInfo = PROCESS_LIST[Indx].FULL_NAME.Length;
					ns = STATUS_SUCCESS;
				}
				else 
					ns = STATUS_UNSUCCESSFUL; // Возврат ошибки

			} 
			else 
				ns = STATUS_BUFFER_TOO_SMALL; // Возврат ошибки
		}

		// ***** Убиение процесса - IOCTL_Z_KILL_PROCESS *****
		if (IoControlCode == IOCTL_Z_KILL_PROCESS) {
			zDbgPrint("IOCTL_Z_KILL_PROCESS \n");
			if (InputBufferLength >= 4) {				
				DWORD Indx;
				memcpy(&Indx, InputBuffer, 4);
				if (Indx >= 0 && Indx < 0xFF && 
					PROCESS_LIST[Indx].PID > 0 && 
					PROCESS_LIST[Indx].EPROCESS != NULL &&
					MmIsAddressValid(PROCESS_LIST[Indx].EPROCESS)) 
				{
					// Возврат NAME процесса, считанного из EPROCESS
					ns = KillProcessByEPROCESS(PROCESS_LIST[Indx].EPROCESS);
					dInfo = 0;
				}
				else 
					ns = STATUS_UNSUCCESSFUL; // Возврат ошибки

			} 
			else 
				ns = STATUS_BUFFER_TOO_SMALL; // Возврат ошибки
		}

		// ***** Запрос списка загруженных драйверов - IOCTL_Z_GET_DRIVER_LIST *****
		if (IoControlCode == IOCTL_Z_GET_DRIVER_LIST) {
			zDbgPrint("IOCTL_Z_GET_DRIVER_LIST \n");
			DWORD Tmp = OutputBufferLength;
			dInfo = 0;
			// Удаление записей, которые ссылаются на несуществующую память
			DeleteInvalidDrivers();
			// Генерация списка драйверов
			ns = CreateDriversList(OutputBuffer, &Tmp);
			if (ns == STATUS_SUCCESS)
				dInfo = Tmp;
		}

		// ***** Запрос имени драйвера - IOCTL_Z_GET_DRIVER_FULLNAME *****
		if (IoControlCode == IOCTL_Z_GET_DRIVER_FULLNAME) {
			zDbgPrint("IOCTL_Z_GET_DRIVER_FULLNAME \n");
			if (InputBufferLength >= 4) 
			{				
				DWORD Base;
				memcpy(&Base, InputBuffer, 4);
				DWORD Tmp = OutputBufferLength;
				dInfo = 0;
				// Поиск имени драйвера
				ns = GetDriverImageByBase(Base, OutputBuffer, &Tmp);
				if (ns == STATUS_SUCCESS)
					dInfo = Tmp;
			} else
				ns = STATUS_UNSUCCESSFUL; // Возврат ошибки

		}

		// ***** Запрос имени драйвера - IOCTL_Z_GET_DRIVER_FULLNAME *****
		if (IoControlCode == IOCTL_Z_PING) {
			zDbgPrint("IOCTL_Z_PING \n");
			if (InputBufferLength >= 4) 
			{				
				DWORD X;
				memcpy(&X, InputBuffer, 4);
				if (X == 88462009719383)
					ns = STATUS_SUCCESS;
				else
					ns = STATUS_UNSUCCESSFUL; 
			} else
				ns = STATUS_UNSUCCESSFUL; // Возврат ошибки
		}

		// ***** Убиение процесса - IOCTL_Z_KILL_PROCESS *****
		if (IoControlCode == IOCTL_Z_KILL_PROCESS_BY_PID) {
			zDbgPrint("IOCTL_Z_KILL_PROCESS_BY_PID \n");
			if (InputBufferLength >= 4) {				
				DWORD PID;
				memcpy(&PID, InputBuffer, 4);
				if (PID > 0 && PID < 0xFFFFFFFF) 
				{
					// Возврат NAME процесса, считанного из EPROCESS
					ns = KillProcessByPID(PID);
					dInfo = 0;
				}
				else 
					ns = STATUS_UNSUCCESSFUL; // Возврат ошибки

			} 
			else 
				ns = STATUS_BUFFER_TOO_SMALL; // Возврат ошибки
		}

		// ****** Разрешение на выгрузку драйвера - IOCTL_Z_ENABLEULNOAD *****
		if (IoControlCode == IOCTL_Z_ENABLEULNOAD) 
			if (InputBufferLength >= 4) 
			{
				DWORD Tmp;
				memcpy(&Tmp, InputBuffer, 4);
				zDbgPrint("IOCTL_Z_ENABLEULNOAD NewState=%d \n", Tmp);
				// Подключение обработчика выгрузки драйвера
				if (Tmp != 0) {
					zDbgPrint("DriverUnload proc inserted \n");
					gpDeviceContext->pDriverObject->DriverUnload  = pDriverUnload;
				}
				else {
					zDbgPrint("DriverUnload proc removed \n");
					gpDeviceContext->pDriverObject->DriverUnload  = NULL;
				}
				ns = STATUS_SUCCESS;
			}
	}
	// Завершение IRP запроса
	pIrp->IoStatus.Status      = ns;
	pIrp->IoStatus.Information = dInfo;
	IoCompleteRequest (pIrp, IO_NO_INCREMENT);
	return ns;  
}

// Системный поток драйвера
VOID DriverThreadProc(IN PVOID StartContext)
{
	LARGE_INTEGER Interval;
	while (TerminateThreadsFlag == FALSE) {
		zDbgPrint("Thread !!");
		Interval.LowPart = -150000000;
		Interval.HighPart = -1;
		KeDelayExecutionThread(KernelMode, FALSE, &Interval);
		// Удаление записей для драйверов, которые выгружены и отсустсвуют по базовому адресу
		DeleteInvalidDrivers();
	}
	zDbgPrint("Thread - terminated");
	PsTerminateSystemThread(STATUS_SUCCESS);
}

// ***** Выгрузка драйвера *****
void DriverUnload (PDRIVER_OBJECT pDriverObject)
{
	zDbgPrint("Driver unload ...");
	// Удаление функции мониторинга процессов
	PsSetCreateProcessNotifyRoutine(*MyCreateProcessNotifyRoutine, true);
	// Удаление функции мониторинга драйверов
	if (zPsRemoveLoadImageNotifyRoutine != NULL)
		zPsRemoveLoadImageNotifyRoutine(*MyLoadImageNotifyRoutine);
	// Остановка потоков
	if (pkThread != 0) {
		TerminateThreadsFlag = TRUE;
		zDbgPrint("Driver unload ... - stop thread + wait");
		KeWaitForSingleObject(pkThread, Executive, KernelMode, FALSE, NULL);
		zDbgPrint("Driver unload ... - wait complete, thread stopped");
		ObDereferenceObject(pkThread);
	}
	// Удаление символьной ссылки
	IoDeleteSymbolicLink (&usSymbolicLinkName);
	// Удаление устройства
	IoDeleteDevice(gpDeviceObject);
	zDbgPrint("Driver unload complete.");
}


// ***** Инициализация драйвера *****
NTSTATUS DriverInitialize (PDRIVER_OBJECT  pDriverObject,
						   PUNICODE_STRING pusRegistryPath)
{
	PDEVICE_OBJECT pDeviceObject = NULL;
	NTSTATUS       ns            = STATUS_DEVICE_CONFIGURATION_ERROR; // Код возврата
	// Создание устройства
	if ((ns = IoCreateDevice (pDriverObject, DEVICE_CONTEXT_,
		&usDeviceName, FILE_DEVICE_UNKNOWN,
		0, FALSE, &pDeviceObject))
		== STATUS_SUCCESS)
	{
		// Устройство создано - создаем символьной ссылки
		if ((ns = IoCreateSymbolicLink (&usSymbolicLinkName,
			&usDeviceName))
			== STATUS_SUCCESS)
		{ 
			// Ссылка (и естественно устройство !) созданы - запоминаем ссылки
			gpDeviceObject  = pDeviceObject;
			gpDeviceContext = (PDEVICE_CONTEXT)pDeviceObject->DeviceExtension;

			gpDeviceContext->pDriverObject = pDriverObject;
			gpDeviceContext->pDeviceObject = pDeviceObject;
		}
		else
		{
			// Устройство создано, а ссылка - нет. Тогда удалим устройство
			IoDeleteDevice (pDeviceObject);
		}
	}
	zDbgPrint("Driver Initialize, Status = %d", ns);
	return ns; 
}

// Запрос информации о версии системы
VOID GetVersionInfo()
{
 memset(&VersionInfo, 0, sizeof(VersionInfo));
 // Если дин. определение адреса было неуспешным, значит система < XP
 if (!zRtlGetVersion) return;
 VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
 // Запрос версии
 NTSTATUS Res = zRtlGetVersion((PRTL_OSVERSIONINFOW)&VersionInfo);
 zDbgPrint("VersionInfo.dwMajorVersion = %X", VersionInfo.dwMajorVersion);
 zDbgPrint("VersionInfo.dwMinorVersion = %X", VersionInfo.dwMinorVersion);
 zDbgPrint("VersionInfo.wServicePackMajor = %X", VersionInfo.wServicePackMajor);
 zDbgPrint("VersionInfo.wServicePackMinor = %X", VersionInfo.wServicePackMinor);
}

// Настройка указателей на элементы EPROCESS
VOID SetupEPROCESSPTR()
{
	zDbgPrint("NtBuildNumber = %X", *NtBuildNumber);
	zDbgPrint("(*NtBuildNumber) & 0x00ffffff = %X", (*NtBuildNumber) & 0x00ffffff);
	// Инициализация смещений в EPROCESS
	switch (((*NtBuildNumber) & 0x00ffffff)) {
	 case  2195:  // Win 2k
		 zDbgPrint("Win 2k !");
		 PIDOffset               = 0x09C;
		 ActiveProcessLinkOffset = 0xA0;
		 ProcessNameOffset       = 0x01FC;
		 break;
	 case 2600:   // Win XP
		 zDbgPrint(" Win XP !");
		 PIDOffset               = 0x084;
		 ActiveProcessLinkOffset = 0x088;
		 ProcessNameOffset       = 0x174;
		 PEBOffset               = 0x1b0;
		 FullProcessNameOffset   = 0x1f4;
		 break;
	 case 3790:  // W2K3 SP0/SP1
		 // Запрос данных о версии
		 GetVersionInfo();
		 if (VersionInfo.wServicePackMajor == 0) // SP0 ?
		 {
			 zDbgPrint("  W2K3 SP0  !");
			 PIDOffset               = 0x084;
			 ActiveProcessLinkOffset = 0x088;
			 ProcessNameOffset       = 0x154;
			 PEBOffset               = 0x190;
			 FullProcessNameOffset   = 0x1d4;
		 }
		 if (VersionInfo.wServicePackMajor == 1) // SP1 ?
		 {
			 zDbgPrint("  W2K3 SP1  !");
			 PIDOffset               = 0x094;
			 ActiveProcessLinkOffset = 0x98;
			 ProcessNameOffset       = 0x164;
			 PEBOffset               = 0x1a0;
			 FullProcessNameOffset   = 0x1e4;
		 }
		 break;
	 case 6000:  // Vista
		 PIDOffset               = 0x09c;
		 ActiveProcessLinkOffset = 0x0a0;
		 ProcessNameOffset       = 0x14c;
		 PEBOffset               = 0x188;
		 FullProcessNameOffset   = 0x1cc;
		 break;
	}	
}

// Инициализация таблицы динамического импорта
VOID InitImport()
{
	ZwQueryInformationProcess		= (PZwQueryInformationProcess)		MmGetSystemRoutineAddress(&usZwQueryInformationProcess);
	ObOpenObjectByPointer			= (PObOpenObjectByPointer)			MmGetSystemRoutineAddress(&usObOpenObjectByPointer);
	ZwOpenProcess					= (PZwOpenProcess)					MmGetSystemRoutineAddress(&usZwOpenProcess);
	ZwTerminateProcess				= (PZwTerminateProcess)				MmGetSystemRoutineAddress(&usZwTerminateProcess);
	zPsSetLoadImageNotifyRoutine	= (PPsSetLoadImageNotifyRoutine)	MmGetSystemRoutineAddress(&usPsSetLoadImageNotifyRoutine);
	zDbgPrint("PsSetLoadImageNotifyRoutine = %X", zPsSetLoadImageNotifyRoutine);
	zPsRemoveLoadImageNotifyRoutine	= (PPsRemoveLoadImageNotifyRoutine)	MmGetSystemRoutineAddress(&usPsRemoveLoadImageNotifyRoutine);
	zDbgPrint("PsRemoveLoadImageNotifyRoutine = %X", zPsRemoveLoadImageNotifyRoutine);
	zRtlGetVersion	= (PRtlGetVersion)	MmGetSystemRoutineAddress(&usRtlGetVersion);
	zDbgPrint("zRtlGetVersion = %X", zRtlGetVersion);
}

// ***** Точка входа в драйвер *****
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pusRegistryPath)
{
	// Настройка адресов импортируемых функций
	InitImport();
	// Подготовка списка
	InitProcessList();
	// Настройка смещений в EPROCESS
	SetupEPROCESSPTR();
	// Настройка границы ядра
	zDbgPrint("MmSystemRangeStart = %X", *MmSystemRangeStart);		
	KernelStartAddr = (ULONG)*MmSystemRangeStart;
	// Устновка функций мониторинга
	NTSTATUS Res1 = STATUS_UNSUCCESSFUL, Res2 = STATUS_UNSUCCESSFUL;
	// Установка мониторинга запуска/останова процессов
	Res1 = PsSetCreateProcessNotifyRoutine(*MyCreateProcessNotifyRoutine, false); 
	// Установка мониторинга загрузки драйверов и образов исполняемых файлов (если поддерживается, XP+)
	if (zPsSetLoadImageNotifyRoutine != NULL) 
		Res2 = zPsSetLoadImageNotifyRoutine(*MyLoadImageNotifyRoutine);
	// Возврат результата инициализации. 
	if ((Res1 == STATUS_SUCCESS) || (Res2 == STATUS_SUCCESS)) {
		zDbgPrint("Res1 = %X", Res1);
		zDbgPrint("Res2 = %X", Res2);
		// Если хотя-бы одна функция отработала успешно, то драйвер должен остаться в памяти
		// Подключение обработчика выгрузки драйвер (если выгрузка допустима)
		if (Res2 != STATUS_SUCCESS || ((Res2 == STATUS_SUCCESS) && (zPsRemoveLoadImageNotifyRoutine != NULL)))
		{
			zDbgPrint("Driver unload enabled");
			pDriverUnload  = DriverUnload;
		}
		else
			pDriverUnload  = NULL;
		pDriverObject->DriverUnload  = NULL;
		// Создание ссылки
		if ((DriverInitialize (pDriverObject, pusRegistryPath))	== STATUS_SUCCESS)   
		{		
			// Подключение обработчика DEVICE_CONTROL	 
			pDriverObject->MajorFunction [IRP_MJ_DEVICE_CONTROL] = DispatchControl;
			// Подключение обработчиков CREATE/CLOSE/CLEANUP
			pDriverObject->MajorFunction [IRP_MJ_CLEANUP] = 
				pDriverObject->MajorFunction [IRP_MJ_CREATE] = 
				pDriverObject->MajorFunction [IRP_MJ_CLOSE] =  DispatchCreateCloseControl;
		}

		if (Res2 == STATUS_SUCCESS) {
			TerminateThreadsFlag = FALSE;
			zDbgPrint("Start Thread");
			// Создание потока
			HANDLE hThread = 0;
			PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, DriverThreadProc, NULL);
			zDbgPrint("hThread = %X", hThread);
			// Получение адреса объекта по ссылке на него
			ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, NULL, KernelMode, &pkThread, NULL);
			zDbgPrint("pkThread = %X", pkThread);
			ZwClose(hThread);
		}

		return STATUS_SUCCESS; 
	}
}