/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	FSSyncCheck.cpp
*		\brief	пример использования библиотеки взаимодействия с драйвером-перехватчиком
*		
*		\author Andrew Sobko
*		\date	05.01.2003 10:57:33
*		
*		Example:	
*		
*		
*		
*/		



#include "stdafx.h"

#include "../../hook/FSSync.h"

int main(int argc, char* argv[])
{
	if (FSDrv_Init())
	{
		OutputDebugString("init ok\n");

		if (FSDrv_AddInvThreadByHandle((HANDLE) GetCurrentThreadId()))
			OutputDebugString("add invisible by handle ok (before register)\n");
		else
			OutputDebugString("add invisible by handle failed (before register)\n");

		if (FSDrv_RegisterInvisibleThread())
		{
			OutputDebugString("register invisible ok\n");

			if (FSDrv_AddInvThreadByHandle((HANDLE) GetCurrentThreadId()))
			{
				OutputDebugString("add invisible by handle ok\n");

				if (FSDrv_DelInvThreadByHandle((HANDLE) GetCurrentThreadId()))
					OutputDebugString("del invisible by handle ok\n");
				else
					OutputDebugString("del invisible by handle failed\n");

			}
			else
				OutputDebugString("add invisible by handle failed\n");

			FSDrv_UnRegisterInvisibleThread();
		}

		FSDrv_Done();
	}

	return 0;
}
