

#include <windows.h>
#include <stdio.h>
#include "ntddndis.h"        
#include "IoctlNdisQuery.h"
#pragma warning(disable:4996)
DWORD query_dword_oid(char * adaptername,DWORD OID_NAME)
{
	DWORD nResult;
	DWORD ReturnedCount;
	DWORD buf;
	nResult = IoctlNdisQuery(adaptername,OID_NAME,&buf, sizeof(buf),&ReturnedCount);
	if(nResult==0 && ReturnedCount==4)
		return buf;
	else
		return 0;
}

DWORD IoctlNdisQuery(
   char     *ioqAdapterName,
   ULONG    ioqOidCode,
   PVOID    ioqBuffer,
   UINT     ioqBufferLength,
   DWORD	*ioqBytesWritten
   )
{

   char       ioqDeviceName[512];
   DWORD      ioqResult = ERROR_SUCCESS;
   HANDLE     ioqhandle;

   *ioqBytesWritten = 0;
   LPDWORD BytesWritten=ioqBytesWritten;

   _snprintf(ioqDeviceName,sizeof(ioqDeviceName),"\\\\.\\%s",ioqAdapterName);
   ioqhandle = CreateFile(ioqDeviceName,0,FILE_SHARE_READ,0,
									OPEN_EXISTING,0,INVALID_HANDLE_VALUE);

    if (ioqhandle != INVALID_HANDLE_VALUE)
    {
        if( !DeviceIoControl(ioqhandle,IOCTL_NDIS_QUERY_GLOBAL_STATS,
							&ioqOidCode,sizeof(ioqOidCode),ioqBuffer,
							ioqBufferLength,BytesWritten,0))
        {
//            printf("   DeviceIoControl returned an error = %d\n",
              ioqResult = GetLastError();
        }
    }
    else
    {
//		printf("   CreateFile returned an error = %d\n",
        ioqResult = GetLastError();
    }
   return( ioqResult );
}