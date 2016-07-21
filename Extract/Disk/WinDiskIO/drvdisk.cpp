#include "llda.h"
#include "ptrthread.h"
#include "../../windows/hook/hook/fssync.h"

#pragma comment( lib, "fssync.lib" )

void*
__cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{	
	void* ptr = NULL;
	ptr = HeapAlloc( GetProcessHeap(), 0, size );
	return ptr; 
};

void __cdecl MemFree(PVOID pOpaquePtr, void** pptr)
{
	if (!*pptr)
		return;
	HeapFree( GetProcessHeap(), 0, *pptr );

	*pptr = NULL;
};

cDrvAccess::cDrvAccess (
	hOBJECT hContext
	) : cDiskAccess( hContext ), m_pContext( NULL ), m_pVolumes( NULL ), m_pwchNativeName( NULL ), m_pwchSavedName( NULL )
{
	HRESULT hResult = DRV_Register (
		&m_pContext,
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | 
		_CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		MemAlloc,
		MemFree,
		NULL
		);

	Reset();
}

cDrvAccess::~cDrvAccess()
{
	if (m_pContext)
		DRV_UnRegister( &m_pContext );

	Reset();
}

bool
cDrvAccess::Init (
	PWCHAR pwchVolumeName
	)
{
	if (!m_pContext)
		return false;

	if (pwchVolumeName)
		return FillInfo( pwchVolumeName );
	else
	{
		HRESULT hResult = DRV_Disk_Enum( m_pContext, &m_pVolumes, &m_VolumesLen );
		if (!SUCCEEDED( hResult ))
		{
			m_pVolumes = NULL;
			m_VolumesLen = 0;
			m_pwchVolume = NULL;
		}
		else
			return true;
	}

	return false;
}

void 
cDrvAccess::Reset (
	)
{
	if (m_pVolumes)
		MemFree( NULL, (void**) &m_pVolumes );

	if (m_pwchNativeName)
		MemFree( NULL, (void**) &m_pwchNativeName );
	if (m_pwchSavedName)
		MemFree( NULL, (void**) &m_pwchSavedName );
	
	m_VolumesLen = 0;
	m_pwchVolume = NULL;
	
	memset( &m_Geometry, 0, sizeof(DISK_GEOMETRY) );
	memset( &m_PartitionInfo, 0, sizeof( PARTITION_INFORMATION) );
	m_Origin = OID_LOGICAL_DRIVE;
	m_bPartitionInfoReady = false;
}

tERROR
cDrvAccess::Next (
	)
{
	if (!m_pVolumes)
		return errUNEXPECTED;

	if (m_pwchVolume)
	{
		if (m_pwchVolume >= m_pVolumes + m_VolumesLen / sizeof(WCHAR))
			return errEND_OF_THE_LIST;
	}
	else
	{
		m_pwchVolume = m_pVolumes;
		return errOK;
	}

	m_pwchVolume += lstrlenW( m_pwchVolume ) + 1;
	if (m_pwchVolume >= m_pVolumes + m_VolumesLen / sizeof(WCHAR))
		return errEND_OF_THE_LIST;

	if (lstrlenW( m_pwchVolume ))
		return errOK;

	m_pwchVolume = m_pVolumes + 1;

	return errEND_OF_THE_LIST;
}

tERROR
cDrvAccess::ExportNameToBuff (
	tDWORD* out_size,
	tCHAR* buffer,
	tDWORD size
	)
{
	PWCHAR pwchName = NULL;
	if (m_pwchSavedName)
		pwchName = m_pwchSavedName;
	else
	{
		if (!m_pwchVolume)
			return errUNEXPECTED;

		if (m_pwchVolume >= m_pVolumes + m_VolumesLen / sizeof(WCHAR))
			return errEND_OF_THE_LIST;

		pwchName = m_pwchVolume;
	}

	unsigned int len = (lstrlenW( pwchName ) + 1) * sizeof(WCHAR);

	*out_size = len;
	if (buffer && size)
	{
		if (size < len)
			return errBUFFER_TOO_SMALL;
		memcpy( buffer, pwchName, len );
	}

	return errOK;
}

tORIG_ID
cDrvAccess::GetOrigin (
	)
{
	return m_Origin;
}

tERROR
cDrvAccess::Read (
	tPTR pBuffer,
	tQWORD offset,
	tDWORD size,
	tDWORD* pBytesRet
	)
{
	LARGE_INTEGER* pOffset = (LARGE_INTEGER*) &offset;

	if (m_bPartitionInfoReady)
	{
		if ((tQWORD) m_PartitionInfo.PartitionLength.QuadPart < offset)
			return errPARAMETER_INVALID;
	}

	HRESULT hResult = DRV_Disk_Read( m_pContext, m_pwchNativeName, pBuffer, size, pOffset, (LPDWORD) pBytesRet );

	if (!SUCCEEDED( hResult ) || !*pBytesRet)
		return errNOT_OK;

	if (*pBytesRet != size)
		return errEOF;

	return errOK;
}

tERROR
cDrvAccess::Write (
	tPTR pBuffer,
	tQWORD offset,
	tDWORD size,
	tDWORD* pBytesRet
	)
{
	LARGE_INTEGER* pOffset = (LARGE_INTEGER*) &offset;

	HRESULT hResult = DRV_Disk_Write( m_pContext, m_pwchNativeName, pBuffer, size, pOffset, (LPDWORD) pBytesRet );

	if (!SUCCEEDED( hResult ) || !*pBytesRet)
		return errNOT_OK;

	if (*pBytesRet != size)
		return errEOF;

	return errOK;
}

HRESULT
cDrvAccess::FindVolumeNativeName (
	PWCHAR pwchVolumeName
	)
{
	WCHAR pwchNameTmpBuf[MAX_PATH * 2];
	PWCHAR pwchNameTmp = pwchNameTmpBuf;
	int len = lstrlenW( pwchVolumeName );
	if (1 == len)
	{
		WCHAR szDrive[3] = L" :";
		szDrive[0] = pwchVolumeName[0];
		if (!QueryDosDeviceW( szDrive, pwchNameTmpBuf, _countof(pwchNameTmpBuf) ))
			return E_FAIL;
	}
	else
		pwchNameTmp = pwchVolumeName;

	HRESULT hResult = DRV_Disk_GetGeometry( m_pContext, pwchNameTmp, &m_Geometry );
	if (SUCCEEDED( hResult ))
	{
		if (!m_Geometry.BytesPerSector)
			return E_FAIL;

		len = lstrlenW( pwchNameTmp );
		m_pwchNativeName = (PWCHAR) MemAlloc( NULL, (len + 1) * sizeof(WCHAR), 0 );
		if (!m_pwchNativeName)
			return E_FAIL;
		memcpy( m_pwchNativeName, pwchNameTmp, (len + 1) * sizeof(WCHAR) );

		PVOID pSector = MemAlloc( NULL, m_Geometry.BytesPerSector, 0 );
		if (pSector)
		{
			tDWORD BytesRet = m_Geometry.BytesPerSector;
			tERROR error = Read( pSector, 0, m_Geometry.BytesPerSector, &BytesRet );
			MemFree( NULL, &pSector );

			if (PR_FAIL(error) || BytesRet != m_Geometry.BytesPerSector)
				return E_FAIL;
		}
	}
	
	return hResult;
}

bool
cDrvAccess::FillInfo (
	PWCHAR pwchVolumeName
	)
{
	if (!pwchVolumeName)
		return false;

	int len = lstrlenW( pwchVolumeName );
	if (!len)
		return false;

	if (L':' == pwchVolumeName[1])
	{
		pwchVolumeName[1] = 0;
		len = 1;
	}

	m_pwchSavedName = (PWCHAR) MemAlloc( NULL, (len + 1) * sizeof(WCHAR), 0 );
	if (!m_pwchSavedName)
		return false;
	memcpy( m_pwchSavedName, pwchVolumeName, (len + 1) * sizeof(WCHAR) );

	if (1 == len)
		m_DriveNumber = pwchVolumeName[0] - L'A';
	else
	{
		m_DriveNumber = 0;
		m_Origin = OID_PHYSICAL_DISK;
	}

	HRESULT hResult = FindVolumeNativeName( pwchVolumeName );
	if (!SUCCEEDED( hResult ))
		return false;

	hResult = DRV_Disk_GetType( m_pContext, m_pwchNativeName, (LPDWORD) &m_DriveType );
	if (!SUCCEEDED( hResult ))
		m_DriveType = cFILE_DEVICE_DISK;

	hResult = DRV_Disk_GetPartitionalInformation( m_pContext, m_pwchNativeName, &m_PartitionInfo );
	if (SUCCEEDED( hResult ))
	{
		if (m_PartitionInfo.PartitionLength.QuadPart)
		{
			m_DriveNumber = m_PartitionInfo.PartitionNumber;
			m_bPartitionInfoReady = true;
		}
	}
	else
	{
		if (OID_PHYSICAL_DISK == m_Origin)
			m_DriveNumber = 0;
	}

	return true;
}


tERROR
cDrvAccess::GetDriveId (
	tPTR p_DriveID,
	tDWORD DriveIdLen
	)
{
	if (!p_DriveID || sizeof(DISK_ID_INFO) != DriveIdLen)
		return errPARAMETER_INVALID;

	memset( p_DriveID, 0, DriveIdLen );

	HRESULT hResult = DRV_Disk_GetDiskId( m_pContext, m_pwchNativeName, (PDISK_ID_INFO) p_DriveID );
	if (SUCCEEDED(hResult) )
		return errOK;
	
	return errNOT_SUPPORTED;
}