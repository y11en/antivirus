#include "llda.h"

void
ResolveGranularity (
	tDWORD block_size,
	tQWORD* poffset,
	tDWORD reqsize,
	tDWORD* poffset_align,
	tDWORD* pnew_size
	)
{
	tDWORD block_count;

	block_count = reqsize / block_size;
	if (reqsize % block_size)
		block_count++;

	*poffset_align = ((DWORD)(*poffset)) % block_size;
	if (*poffset_align)
		block_count++;

	*pnew_size = block_count * block_size;	
}

cDiskAccess::cDiskAccess (
	hOBJECT hContext
	)
{
	m_hContext = hContext;
}

cDiskAccess::~cDiskAccess()
{
}

tQWORD
cDiskAccess::GetSize (
	)
{
	__int64 result = 0;

	if (m_bPartitionInfoReady)
		result = (__int64) m_PartitionInfo.PartitionLength.QuadPart;
	else
	{
		result = ((__int64)
		(m_Geometry.BytesPerSector * m_Geometry.SectorsPerTrack * m_Geometry.TracksPerCylinder)) * 
		((__int64)m_Geometry.Cylinders.QuadPart);
	}

	return *(tQWORD*) &result;
}

tBOOL
cDiskAccess::IsPartitional (
	)
{
	return cTRUE;
}

tDWORD
cDiskAccess::GetDriveType (
	)
{
	PR_TRACE(( g_root, prtWARNING, "wdiskop\tdrive type 0x%x", m_DriveType ));
	return m_DriveType;
}