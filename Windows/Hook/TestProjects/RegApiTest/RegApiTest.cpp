/*
	Пример использования registry api mini-klif'а
*/

#include "stdafx.h"

#include "..\..\hook\avpgcom.h"
#include "..\..\hook\FSDrvLib.h"
#include "..\..\hook\IDriver.h"
#include "..\..\hook\hookspec.h"
#include "..\..\hook\funcs.h"

#pragma warning(disable:4996)	// no "deprecated" warning

void Init()
{
	if (!FSDrvInterceptorInit())
		printf("\nConnection to driver failed\n");
	else
		AddFSFilterW(FSDrvGetDeviceHandle(), FSDrvGetAppID(), L"*", "*", DEADLOCKWDOG_TIMEOUT,
		FLT_A_DEFAULT, FLTTYPE_REGS, IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, 0,
		PostProcessing, NULL);
}

void DeInit()
{
	FSDrvInterceptorDone();
}

// енумерация подключей заданного ключа
bool EnumKeys(PWCHAR KeyPath, ULONG EnumIndex, PWCHAR KeyName)
{
	char Buffer1[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(REG_OP_REQUEST)];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST)Buffer1;
	PREG_OP_REQUEST pRegRequest = (PREG_OP_REQUEST)pRequest->m_Buffer;
				
	pRequest->m_DrvID = FLTTYPE_REGS;
	pRequest->m_IOCTL = _AVPG_IOCTL_REG_ENUMKEY;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(REG_OP_REQUEST);

	lstrcpynW(pRegRequest->m_KeyPath, KeyPath, MAX_REGPATH_LEN);
	pRegRequest->m_EnumIndex = EnumIndex;

#define EKBUFFER_SIZE	(sizeof(REG_OP_RESULT)+sizeof(REG_ENUM_RESULT)+sizeof(WCHAR)*MAX_KEYVALUENAME_LEN)

	CHAR Buffer2[EKBUFFER_SIZE];
	PREG_OP_RESULT pRegOp_Result = (PREG_OP_RESULT)Buffer2;

	ULONG RetSize = EKBUFFER_SIZE;
	if (!(IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, pRegOp_Result, &RetSize) && pRegOp_Result->m_Status == RegOp_StatusSuccess))
		return false;

	lstrcpyW(KeyName, ((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result);

	return true;
}

// енумерация значений заданного ключа
bool EnumValues(PWCHAR KeyPath, ULONG EnumIndex, PWCHAR ValueName)
{
	char Buffer1[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(REG_OP_REQUEST)];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST)Buffer1;
	PREG_OP_REQUEST pRegRequest = (PREG_OP_REQUEST)pRequest->m_Buffer;
				
	pRequest->m_DrvID = FLTTYPE_REGS;
	pRequest->m_IOCTL = _AVPG_IOCTL_REG_ENUMVALUE;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(REG_OP_REQUEST);

	lstrcpynW(pRegRequest->m_KeyPath, KeyPath, MAX_REGPATH_LEN);
	pRegRequest->m_EnumIndex = EnumIndex;

#define EVBUFFER_SIZE	(sizeof(REG_OP_RESULT)+sizeof(REG_ENUM_RESULT)+sizeof(WCHAR)*MAX_KEYVALUENAME_LEN)

	CHAR Buffer2[EVBUFFER_SIZE];
	PREG_OP_RESULT pRegOp_Result = (PREG_OP_RESULT)Buffer2;

	ULONG RetSize = EVBUFFER_SIZE;
	if (!(IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, pRegOp_Result, &RetSize) && pRegOp_Result->m_Status == RegOp_StatusSuccess))
		return false;

	lstrcpyW(ValueName, ((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result);

	return true;
}

// чтение значения
bool QueryValue(PWCHAR KeyPath, PWCHAR ValueName, PULONG pType, PULONG pDataLen, PVOID pDataBuffer OPTIONAL, ULONG cbDataBuffer)
{
	char Buffer1[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(REG_OP_REQUEST)];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST)Buffer1;
	PREG_OP_REQUEST pRegRequest = (PREG_OP_REQUEST)pRequest->m_Buffer;
				
	pRequest->m_DrvID = FLTTYPE_REGS;
	pRequest->m_IOCTL = _AVPG_IOCTL_REG_QUERYVALUE;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(REG_OP_REQUEST);


	lstrcpynW(pRegRequest->m_KeyPath, KeyPath, MAX_REGPATH_LEN);
	lstrcpynW(pRegRequest->m_ValueName, ValueName, MAX_KEYVALUENAME_LEN);

#define QVBUFFER_SIZE	(sizeof(REG_OP_RESULT)+sizeof(REG_QUERY_RESULT)+1024/*начальный размер данных - может и мало будет*/)

	CHAR Buffer2[QVBUFFER_SIZE];
	PREG_OP_RESULT pRegOp_Result = (PREG_OP_RESULT)Buffer2;

	ULONG RetSize = QVBUFFER_SIZE;
	if (!IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, pRegOp_Result, &RetSize))
		return false;
	if (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
	{
// буфер на стеке мал - выделяем из кучи
		RetSize = sizeof(REG_OP_RESULT)+pRegOp_Result->m_BufferLen;
		pRegOp_Result = (PREG_OP_RESULT)HeapAlloc(GetProcessHeap(), 0, RetSize);
		if (!pRegOp_Result)
			return false;

		if (!IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, pRegOp_Result, &RetSize))
		{
			HeapFree(GetProcessHeap(), 0, pRegOp_Result);
			return false;
		}
	}
	if (pRegOp_Result->m_Status != RegOp_StatusSuccess)
	{
		if ((PCHAR)pRegOp_Result != (PCHAR)Buffer2)
			HeapFree(GetProcessHeap(), 0 ,pRegOp_Result);
		return false;
	}

	PREG_QUERY_RESULT RegQuery_Result = (PREG_QUERY_RESULT)pRegOp_Result->m_Buffer;

	*pType = RegQuery_Result->m_Type;
	*pDataLen = RegQuery_Result->m_ResultLen;
	if (pDataBuffer)
		memcpy(pDataBuffer, RegQuery_Result->m_Result, min(RegQuery_Result->m_ResultLen, cbDataBuffer));

// выделяли буфер?
	if ((PCHAR)pRegOp_Result != (PCHAR)Buffer2)
		HeapFree(GetProcessHeap(), 0 ,pRegOp_Result);

	return true;
}

void PrintTabs(ULONG Depth)
{
	for (ULONG i = 0;i < Depth; i++)
		printf("\t");
}

bool PrintValue(PWCHAR KeyPath, PWCHAR ValueName, ULONG Depth)
{
	ULONG	Type;
	ULONG	DataLen;
	char	Buffer[1024];

	PVOID	pBuffer = Buffer;
	bool	Success = false;
	if (QueryValue(KeyPath, ValueName, &Type, &DataLen, Buffer, sizeof(Buffer)))
	{
		if (DataLen > sizeof(Buffer))
		{
// буфер на стеке мал - выделяем из кучи
			pBuffer = HeapAlloc(GetProcessHeap(), 0, DataLen);
			if (pBuffer)
			{
				if (QueryValue(KeyPath, ValueName, &Type, &DataLen, pBuffer, DataLen))
					Success = true;
			}
		}
		else
			Success = true;
	}

	if (Success)
	{
		PrintTabs(Depth);
		printf("%S: type = %d, data length = %d", ValueName[0]?ValueName:L"(Default)"/*дефолтовая валушка == L""*/, Type, DataLen);
		switch (Type)
		{
		case REG_DWORD:
			printf(", data = 0x%08X", *(PULONG)pBuffer);
			break;
		case REG_SZ:
		case REG_EXPAND_SZ:
			printf(", data = %S", pBuffer);
			break;
		}

		printf("\n");
	}

// если выделяли буфер
	if (pBuffer != Buffer)
		HeapFree(GetProcessHeap(), 0, pBuffer);

	return Success;
}

// рекурсивная ф-я
void EnumRegistry(PWCHAR RootKey/* должно быть выделено MAX_REGPATH_LEN WCHARов*/, ULONG Depth)
{
// не будем перегружать стек
	PWCHAR KeyName = (PWCHAR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR)*MAX_KEYVALUENAME_LEN);
	if (!KeyName)
		return;
	PWCHAR ValueName = (PWCHAR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR)*MAX_KEYVALUENAME_LEN);
	if (!ValueName)
	{
		HeapFree(GetProcessHeap(), 0, KeyName);
		return;
	}

// енумерим валушки, начиная с дефолтовой
	PrintTabs(Depth);
	printf("Values:\n");

// дефолтовая не установлена
	if (!PrintValue(RootKey, L"", Depth))
	{
		PrintTabs(Depth);
		printf("(Default):\tvalue not set\n");
	}
	for (ULONG ValueIndex = 0; EnumValues(RootKey, ValueIndex, ValueName); ValueIndex++)
	{
// пропускаем установленное дефолтовое значение
		if (ValueName[0])
			PrintValue(RootKey, ValueName, Depth);
	}

	ULONG OldLen = lstrlenW(RootKey);
	for (ULONG KeyIndex = 0; EnumKeys(RootKey, KeyIndex, KeyName); KeyIndex++)
	{
		lstrcatW(RootKey, L"\\");
		lstrcatW(RootKey, KeyName);

		PrintTabs(Depth);
		printf("%S\n", KeyName);

		EnumRegistry(RootKey, Depth+1);

		RootKey[OldLen] = 0;
	}

	HeapFree(GetProcessHeap(), 0 ,ValueName);
	HeapFree(GetProcessHeap(), 0, KeyName);
}

// создание ключа
void CreateKey()
{
	char Buffer[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(REG_OP_REQUEST)];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST)Buffer;
	PREG_OP_REQUEST pRegRequest = (PREG_OP_REQUEST)pRequest->m_Buffer;
				
	printf("Create key...\n");

	ZeroMemory(pRequest, sizeof(Buffer));
	pRequest->m_DrvID = FLTTYPE_REGS;
	pRequest->m_IOCTL = _AVPG_IOCTL_REG_CREATEKEY;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(REG_OP_REQUEST);

	printf("key path: ");
	scanf("%S", pRegRequest->m_KeyPath);

	REG_OP_RESULT RegOp_Result;
	ULONG RetSize = sizeof(RegOp_Result);

	if (!IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, &RegOp_Result, &RetSize))
	{
		printf("IDriverExternalDrvRequest() failed!\n");
		return;
	}

	switch (RegOp_Result.m_Status)
	{
// создали ключ
	case RegOp_StatusSuccess:
		printf("Key created successfully!\n" );
		break;
// ключ уже существует
	case RegOp_StatusAlreadyExisting:
		printf("Key beeing created already exists!\n");
	}
}

// создание/установка значения
void SetValue()
{
	char Buffer1[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(REG_OP_REQUEST) + 1024/*max value data size*/];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST)Buffer1;
	PREG_OP_REQUEST pRegRequest = (PREG_OP_REQUEST)pRequest->m_Buffer;
				
	printf("Set key value...\n");

	ZeroMemory(pRequest, sizeof(Buffer1));
	pRequest->m_DrvID = FLTTYPE_REGS;
	pRequest->m_IOCTL = _AVPG_IOCTL_REG_SETVALUE;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(Buffer1)-sizeof(EXTERNAL_DRV_REQUEST);

	printf("key path: ");
	scanf("%S", pRegRequest->m_KeyPath);
	printf("value name: ");
	scanf("%S", pRegRequest->m_ValueName);
	printf("value type: ");
	scanf("%d", &(pRegRequest->m_Type));
	printf("value size: ");
	scanf("%d", &(pRegRequest->m_ValueSize));
	printf("value data: ");
	scanf("%S", pRegRequest->m_ValueData);

#define SVBUFFER_SIZE	(sizeof(REG_OP_RESULT))

	CHAR Buffer2[SVBUFFER_SIZE];
	PREG_OP_RESULT pRegOp_Result = (PREG_OP_RESULT)Buffer2;

	ULONG RetSize = SVBUFFER_SIZE;
	if (!IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, pRegOp_Result, &RetSize))
	{
		printf("IDriverExternalDrvRequest() failed!\n");
		return;
	}

	printf("Status = %d\n", pRegOp_Result->m_Status);
}

int main(int argc, char* argv[])
{
	Init();

	ULONG Type = 0;
	ULONG DataLen = 0;
	CHAR Buffer[0x200];

	ZeroMemory( Buffer, sizeof(Buffer) );
	for (ULONG i = 0; i < 10000; i++)
	{
		BOOL bResult = QueryValue(L"\\registry\\machine\\system\\controlset003\\services\\mup",
			L"ImagePath", &Type, &DataLen, Buffer, sizeof(Buffer));
		printf( "bResult = %d string = %S\n", bResult, Buffer );
	}

	return 0;

	CreateKey();
	printf("\n");
	SetValue();
	printf("\n");
//---------------------------------------------------------------------------------------------------------
	WCHAR RootKey[MAX_REGPATH_LEN];

	printf("MACHINE\n");
	lstrcpyW(RootKey, L"\\registry\\machine");
	EnumRegistry(RootKey, 1);

	printf("USER\n");
	lstrcpyW(RootKey, L"\\registry\\user");
	EnumRegistry(RootKey, 1);
//---------------------------------------------------------------------------------------------------------

	DeInit();

	return 0;
}
