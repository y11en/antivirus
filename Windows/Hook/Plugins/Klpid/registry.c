#include "g_precomp.h"
#pragma hdrstop

//-----------------------------------------
// Registry Management .............
//-----------------------------------------
#ifndef ISWIN9X

NTSTATUS	MyQueryRegistryValue(PUNICODE_STRING	puRegString, 
								 PUNICODE_STRING	puRegValue,
								 PULONG				Data,
								 ULONG				DataSize)
{
	NTSTATUS	                            ntStatus;
	OBJECT_ATTRIBUTES						ObjAttr;
	HANDLE									hKey        = NULL;
	PKEY_VALUE_PARTIAL_INFORMATION			Buffer      = NULL;
	ULONG									Size        = sizeof(ULONG) * 3 + DataSize * 2;
	
	do 
	{		
		InitializeObjectAttributes(&ObjAttr, puRegString, OBJ_CASE_INSENSITIVE, NULL, NULL);
		
		ntStatus = ZwCreateKey (&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, NULL);
		
		if (!NT_SUCCESS(ntStatus)) 
		{			
			break;
		}
        
        if ( Buffer = ExAllocatePool (NonPagedPool, Size) )
        {
            ntStatus = ZwQueryValueKey(hKey, puRegValue, KeyValuePartialInformation, Buffer, Size, &Size );

            if ( !NT_SUCCESS(ntStatus) )
            {
                DbgPrint("Unable Query (%ls). Status = %x \n", puRegValue->Buffer, ntStatus);
                ExFreePool (Buffer);
                break;
            }
            
            if (DataSize > Buffer->DataLength)
            {
                DbgPrint("Too Little Datasize \n");
            }
            
            RtlCopyMemory (Data, Buffer->Data, DataSize);
            ExFreePool (Buffer);
            
            if (!NT_SUCCESS(ntStatus)) 
            {
                DbgPrint ("unable to Query Value  :( \n");		
                break;
            }
        }		
			
	} while (FALSE);
	// Finalisation here
	
    if (hKey)
        ZwClose (hKey);
	
	return ntStatus;
}

NTSTATUS	MyWriteRegistryValue(PUNICODE_STRING	puRegString, 
								 PUNICODE_STRING	puRegValue,
								 PVOID				Data,
								 ULONG				DataSize)
{
	NTSTATUS	ntStatus;
	OBJECT_ATTRIBUTES						ObjAttr;
	HANDLE									hKey        = NULL;
	PKEY_VALUE_PARTIAL_INFORMATION			Buffer      = NULL;
	ULONG									Size = 0x100;
	
	do 
	{		
		InitializeObjectAttributes( &ObjAttr, puRegString, OBJ_CASE_INSENSITIVE, NULL, NULL );
		
		ntStatus = ZwCreateKey (&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, NULL);
		
		if (!NT_SUCCESS(ntStatus)) 
		{			
			break;
		}
		
		ntStatus = ZwSetValueKey( hKey, puRegValue, 0, REG_BINARY, Data, DataSize );
		
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("Unable to Write Registry (%ls). Status = %x \n", puRegValue->Buffer , ntStatus);			
			break;
		}	
	} while (FALSE);
	// Finalisation here

	if ( hKey )
        ZwClose (hKey);
	
	return ntStatus;
}

NTSTATUS	QueryMyParam(PVOID	Value, ULONG	ValueSize, PCWSTR	Key)
{
	UNICODE_STRING	KeyStr, KeyValue;
    
	RtlInitUnicodeString(&KeyStr , SETUP_STRING_KEY);
	RtlInitUnicodeString(&KeyValue , Key);

	return  MyQueryRegistryValue(&KeyStr, &KeyValue, Value, ValueSize );
}

NTSTATUS	SetMyParam(PVOID	Value, ULONG	ValueSize, PCWSTR	Key)
{
	UNICODE_STRING	KeyStr, KeyValue;
    
	RtlInitUnicodeString(&KeyStr , SETUP_STRING_KEY);
	RtlInitUnicodeString(&KeyValue , Key);

	return  MyWriteRegistryValue(&KeyStr, &KeyValue, Value, ValueSize );
}
//------------------------------------------------------------------------------------------
// функции для Борьки. Записывают значения разных типов в реестр рядом с klpid/Parameters
// REG_DWORD
// REG_BINARY
// REG_SZ
//------------------------------------------------------------------------------------------
NTSTATUS	KlQueryParam(PVOID Param, PULONG ParamSize, ULONG ParamType, PCHAR ParamName)
{
	NTSTATUS								ntStatus;	
	UNICODE_STRING							KeyStr;
	UNICODE_STRING							KeyValue, uStr;
	HANDLE									hKey;

	OBJECT_ATTRIBUTES						ObjAttr;	
	PKEY_VALUE_PARTIAL_INFORMATION			Buffer;
	ANSI_STRING								AnsiStr;
	ULONG									RetSize, Temp1, Temp2;
	
	RtlInitUnicodeString(&KeyStr , SETUP_STRING_KEY);
	RtlInitAnsiString(&AnsiStr, (PCSZ)ParamName);	
	
	RtlAnsiStringToUnicodeString(&KeyValue , &AnsiStr, TRUE);
			
	// открываем ключик
	InitializeObjectAttributes(&ObjAttr, &KeyStr, OBJ_CASE_INSENSITIVE, NULL, NULL);	
	ntStatus = ZwCreateKey (&hKey, KEY_READ, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, NULL);

	if (ntStatus != STATUS_SUCCESS)
	{		
		RtlFreeUnicodeString(&KeyValue);
		return ntStatus;
	}
	
	// читаем ключик  в память, которую мы для этого выделим. Будем выделять в 2 раза больше, чем нужно
	Buffer = ExAllocatePool ( NonPagedPool, sizeof(ULONG) * 3  + (*ParamSize) * 2 );
	if (Buffer == NULL)
	{	
		RtlFreeUnicodeString(&KeyValue);
		return STATUS_UNSUCCESSFUL;
	}
	
	ntStatus = ZwQueryValueKey( hKey, &KeyValue, KeyValuePartialInformation, Buffer, sizeof(ULONG) * 3 + (*ParamSize) * 2, &RetSize );	
	
	if (ntStatus == STATUS_BUFFER_TOO_SMALL)
	{
		// Сообщим юзеру, сколько памяти нужно выделить. Юзеру не нужен размер заголовка.
		if (ParamType == REG_SZ)
		{
			*ParamSize = (RetSize - sizeof(ULONG) * 3) / 2;
		}
		else
		{		
			*ParamSize = RetSize -  sizeof(ULONG) * 3 ;			
		}
	}

	if (ntStatus == STATUS_SUCCESS)
	{
		if ( ParamType == REG_SZ )
		{		
			
			uStr.Buffer = (PWSTR)Buffer->Data;
			uStr.Length = (USHORT)Buffer->DataLength;
			uStr.MaximumLength = uStr.Length;

			*ParamSize = ( RetSize - sizeof(ULONG) * 3 ) / 2;
			RtlUnicodeStringToAnsiString(&AnsiStr, &uStr, TRUE);
			RtlCopyMemory ( Param, AnsiStr.Buffer , AnsiStr.Length );
			RtlFreeAnsiString(&AnsiStr);
		}
		else
		{			
			RtlCopyMemory (Param, Buffer->Data , Buffer->DataLength);
			*ParamSize = RetSize - sizeof(ULONG)*3;
		}
	}

	RtlFreeUnicodeString(&KeyValue);
	
	ExFreePool(Buffer);
	ZwClose (hKey);
	
	return ntStatus;
}
//-----------------------------------------------------------------------------------------
// Имя ParamName - передается в Ansi
// Если передается строка, то она Ansi ...
NTSTATUS	KlWriteParam(PVOID Param, PULONG ParamSize, ULONG ParamType, PCHAR ParamName)
{
	NTSTATUS								ntStatus;	
	UNICODE_STRING							KeyStr, KeyValue, uStr;
	HANDLE									hKey;
	
	OBJECT_ATTRIBUTES						ObjAttr;	
	PKEY_VALUE_PARTIAL_INFORMATION			Buffer;	
	ANSI_STRING								AnsiStr;	
	
	RtlInitUnicodeString(&KeyStr , SETUP_STRING_KEY);
	RtlInitAnsiString(&AnsiStr, (PCSZ)ParamName);	
	RtlAnsiStringToUnicodeString(&KeyValue , &AnsiStr, TRUE);
	
	// надо конвертнуть строку в юникод
	RtlInitAnsiString(&AnsiStr, (PCSZ)Param);
	RtlAnsiStringToUnicodeString(&uStr, &AnsiStr, TRUE);	
	
	// открываем ключик
	InitializeObjectAttributes(&ObjAttr, &KeyStr, OBJ_CASE_INSENSITIVE, NULL, NULL);	

	ntStatus = ZwCreateKey (&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, NULL);	
	
	if (ntStatus != STATUS_SUCCESS)
	{
		RtlFreeUnicodeString(&uStr);
		RtlFreeUnicodeString(&KeyValue);
		return ntStatus;
	}

	if (ParamType == REG_SZ)
	{
		ntStatus = ZwSetValueKey( hKey, &KeyValue, 0, ParamType, uStr.Buffer, uStr.MaximumLength );
	}
	else
	{
		ntStatus = ZwSetValueKey( hKey, &KeyValue, 0, ParamType, Param, *ParamSize );
	}

	RtlFreeUnicodeString(&KeyValue);
	RtlFreeUnicodeString(&uStr);

	return ntStatus;
}

#else // ISWIN9X

NTSTATUS	KlQueryParam(PVOID Param, PULONG ParamSize, ULONG ParamType, PCHAR ParamName)
{
	HKEY	hKey;
	LONG	Status;
	Status = RegCreateKey( HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\VxD\\klpid\\parameters",&hKey);
		
	if (Status != ERROR_SUCCESS)
	{
//		nprintf ("Unable to query Key\n", Status);
		return Status;
	}
//	nprintf ("Query ParamName='%s'\n", ParamName);
//	nprintf ("ParamSize =%x\n", *ParamSize);

	Status = RegQueryValueEx(hKey, ParamName, NULL, &ParamType, Param, ParamSize);		
//	nprintf ("QueryValue Status = %x\n", Status);
	return Status;
}

NTSTATUS	KlWriteParam(PVOID Param, PULONG ParamSize, ULONG ParamType, PCHAR ParamName)
{	
	HKEY hKey;
	LONG	Status;

#ifdef _DEBUG
	if (ParamType == REG_DWORD)
	{
		if (stricmp("SLevel", ParamName) == 0)
		{
			if ( *(ULONG*)Param == 0 )
			{
__asm int 3
			}
		}
	}
#endif

	Status = RegCreateKey( HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\VxD\\klpid\\parameters",&hKey);
	if (Status != ERROR_SUCCESS)
	{
		nprintf ("Unable to query Key\n", Status);
		return Status;
	}
	Status = RegSetValueEx(hKey, ParamName, 0, ParamType, Param, *ParamSize);
	nprintf ("SetValue Status = %x\n", Status);
	return Status;
}

LONG	QueryMyParam(PVOID	Value, 
					 ULONG	ValueSize, 
					 PCHAR	Key)
{
	HKEY hKey;
	NTSTATUS	ntStatus;
	if ( ERROR_SUCCESS != RegCreateKey( HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\VxD\\klpid\\parameters",&hKey) )
	{
        nprintf ("unable to open registry\n");
	}

	ntStatus = RegQueryValueEx(hKey, Key, NULL, 0, Value, &ValueSize);		
	return ntStatus;
}

NTSTATUS	SetMyParam(PVOID	Value, 
					   ULONG	ValueSize, 
					   PCHAR	Key)
{
	HKEY hKey;
	NTSTATUS	ntStatus;
	if (ERROR_SUCCESS != RegCreateKey( HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\VxD\\klpid\\parameters",&hKey))
	{
		nprintf ("unable to open registry\n");
	}
    
	ntStatus = RegSetValueEx(hKey, Key, 0, REG_BINARY, Value, ValueSize);
	return ntStatus;
}

#endif // ISWIN9X