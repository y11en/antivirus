#include "llda.h"
#include "ptrthread.h"

cGenericAccess::cGenericAccess (
	hOBJECT hContext
	) : cDiskAccess( hContext ), m_hVolume( NULL ), m_hVolumes( NULL )
{
	Reset();
}

cGenericAccess::~cGenericAccess()
{
	if (m_hVolume)
		CloseHandle( m_hVolume );
	m_hVolume = NULL;

	m_hVolumes = NULL;
}

bool
cGenericAccess::Init (
	PWCHAR pwchVolumeName
	)
{
	if (pwchVolumeName)
		return FillInfo( pwchVolumeName );
	else
	{
		tERROR error = m_hContext->sysCreateObjectQuick( (hOBJECT*) &m_hVolumes, IID_STRING, PID_ANY, 0 );
		if (PR_SUCC( error ))
			return true;
	}

	return false;
}

void 
cGenericAccess::Reset (
	)
{
	if (m_hVolume)
		CloseHandle( m_hVolume );
	m_hVolume = NULL;

	if (m_hVolumes)
		m_hVolumes->sysRevertObject( false );

	memset( &m_Geometry, 0, sizeof(DISK_GEOMETRY) );
	memset( &m_PartitionInfo, 0, sizeof( PARTITION_INFORMATION) );
	m_bPartitionInfoReady = false;
}

tERROR
cGenericAccess::Next (
	)
{
	tERROR error = exDiskExLogical_Next( m_hVolumes );
	return error;
}

tERROR
cGenericAccess::ExportNameToBuff (
	tDWORD* out_size,
	tCHAR* buffer,
	tDWORD size
	)
{
	tERROR error = errOK;

	if (!m_hVolumes)
		return errUNEXPECTED;

	return m_hVolumes->ExportToBuff( out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z );
}

tERROR
cGenericAccess::Read (
	tPTR pBuffer,
	tQWORD offset,
	tDWORD size,
	tDWORD* pBytesRet
	)
{
	LARGE_INTEGER* pOffset = (LARGE_INTEGER*) &offset;
	LONG dwPtrLow =  pOffset->LowPart;
	LONG dwPtrHigh = pOffset->HighPart;

	DWORD pos = SetFilePointer( m_hVolume, dwPtrLow, &dwPtrHigh, FILE_BEGIN );
	DWORD dwError = GetLastError();
	
	if (INVALID_SET_FILE_POINTER == dwPtrLow && NO_ERROR != dwError)
		return errOUT_OF_OBJECT;

	BOOL bResult = ReadFile( m_hVolume, pBuffer, size, (LPDWORD) pBytesRet, NULL );

	if (!bResult || !*pBytesRet)
		return errNOT_OK;

	if (*pBytesRet != size)
		return errEOF;

	return errOK;
}

tERROR
cGenericAccess::Write (
	tPTR pBuffer,
	tQWORD offset,
	tDWORD size,
	tDWORD* pBytesRet
	)
{
	LARGE_INTEGER* pOffset = (LARGE_INTEGER*) &offset;
	LONG dwPtrLow =  pOffset->LowPart;
	LONG dwPtrHigh = pOffset->HighPart;

	DWORD pos = SetFilePointer( m_hVolume, dwPtrLow, &dwPtrHigh, FILE_BEGIN );
	DWORD dwError = GetLastError();
	
	if (INVALID_SET_FILE_POINTER == dwPtrLow && NO_ERROR != dwError)
		return errOUT_OF_OBJECT;

	BOOL bResult = WriteFile( m_hVolume, pBuffer, size, (LPDWORD) pBytesRet, NULL );

	if (!bResult || !*pBytesRet)
		return errNOT_OK;

	if (*pBytesRet != size)
		return errEOF;

	return errOK;
}

bool
cGenericAccess::FillInfo (
	PWCHAR pwchVolumeName
	)
{
	if (!pwchVolumeName)
		return false;

	WCHAR pwchVolume[7] = L"\\\\.\\A:";
	pwchVolume[4] = pwchVolumeName[0];

	m_hVolume = CreateFileW( pwchVolume, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hVolume)
	{
		m_hVolume = NULL;

		return false;
	}

	bool bGeometryReady = false;
	DISK_GEOMETRY Geometry;
	DWORD BytesReturned;

	m_DriveNumber = pwchVolumeName[0] - L'A';
	m_DriveType = cFILE_DEVICE_DISK;
	
	pwchVolume[0] = pwchVolumeName[0];
	pwchVolume[1] = L':';
	pwchVolume[2] = L'\\';
	pwchVolume[3] = 0;

	DWORD drvType = GetDriveTypeW( pwchVolume );
	if (DRIVE_CDROM == drvType)
		m_DriveType = cFILE_DEVICE_CD_ROM;

	for (int cou = 0; cou < 3; cou++)
	{
		if (DeviceIoControl (
			m_hVolume,
			IOCTL_DISK_GET_DRIVE_GEOMETRY,
			NULL,
			0,
			&Geometry,
			sizeof(DISK_GEOMETRY),
			(LPDWORD) &BytesReturned,
			NULL
			))
		{
			memcpy( &m_Geometry, &Geometry, sizeof(DISK_GEOMETRY) );

			for (int cou2 = 0; cou2 < 3; cou2++)
			{
				PARTITION_INFORMATION PartitionalInfo;
				if (DeviceIoControl (
					m_hVolume,
					IOCTL_DISK_GET_PARTITION_INFO,
					NULL,
					0,
					&PartitionalInfo,
					sizeof(PARTITION_INFORMATION),
					&BytesReturned,
					NULL
					))
				{
					m_bPartitionInfoReady = true;
					memcpy( &m_PartitionInfo, &PartitionalInfo, sizeof(PARTITION_INFORMATION) );
					break;
				}

				if (GetLastError() != ERROR_NOT_READY)
					break;

				Sleep(10);
			}

			return true;
		}
		
		if (ERROR_NOT_READY != GetLastError())
			break;

		Sleep(10);
	}

	CloseHandle( m_hVolume );
	m_hVolume = NULL;

	return false;
}