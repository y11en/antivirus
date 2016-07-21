#ifndef __llda
#define __llda

#include <windows.h>

#include <Prague/prague.h>
#include <prague/pr_cpp.h>
#include <prague/pr_oid.h>

void
ResolveGranularity (
	tDWORD block_size,
	tQWORD* poffset,
	tDWORD reqsize,
	tDWORD* poffset_align,
	tDWORD* pnew_size
	);

class cDiskAccess
{
public:
	cDiskAccess(hOBJECT hContext);
	virtual ~cDiskAccess();

	virtual bool Init( PWCHAR pwchVolumeName ) {return false;}
	virtual void Reset() {m_bPartitionInfoReady = false;}
	virtual tERROR Next(){ return errUNEXPECTED;}

	tQWORD GetCylinders (){return *(tQWORD*) &m_Geometry.Cylinders;}
	tDWORD GetTracksPerCylinder(){return m_Geometry.TracksPerCylinder;}
	tDWORD GetSectorsPerTrack(){return m_Geometry.SectorsPerTrack;}
	tDWORD GetBytesPerSector(){return m_Geometry.BytesPerSector;}
	tDWORD GetMediaType(){return m_Geometry.MediaType;}
	tQWORD GetSize();

	virtual tORIG_ID GetOrigin() {return OID_LOGICAL_DRIVE;}

	virtual tDWORD GetDriveNumber(){return m_DriveNumber;}
	virtual tDWORD GetDriveType();

	virtual tBOOL IsPartitional();
	virtual tBYTE GetPartitionalType(){return m_PartitionInfo.PartitionType;}
	virtual tBOOL IsBootable(){return m_PartitionInfo.BootIndicator;}
	virtual tBOOL IsPartitionRecognized(){return m_PartitionInfo.RecognizedPartition;}
	virtual tBOOL IsReadOnly(){return cFALSE;}

	virtual tERROR ExportNameToBuff( tDWORD* out_size, tCHAR* buffer, tDWORD size ){return errUNEXPECTED;};

	virtual tERROR Read( tPTR pBuffer, tQWORD offset, tDWORD size, tDWORD* pBytesRet)
	{
		return errNOT_IMPLEMENTED;
	}

	virtual tERROR Write( tPTR pBuffer, tQWORD offset, tDWORD size, tDWORD* pBytesRet )
	{
		return errNOT_IMPLEMENTED;
	}

protected:
	virtual bool FillInfo( PWCHAR pwchVolumeName ){ return false;};

	hOBJECT		m_hContext;

	DISK_GEOMETRY			m_Geometry;
	PARTITION_INFORMATION	m_PartitionInfo;
	bool					m_bPartitionInfoReady;
	
	tDWORD					m_DriveNumber;
	tDWORD					m_DriveType;
};

class cDrvAccess : public cDiskAccess
{
public:
	cDrvAccess(hOBJECT hContext);
	~cDrvAccess();

	bool Init( PWCHAR pwchVolumeName );
	void Reset();
	tERROR Next();

	tORIG_ID GetOrigin();
	
	tERROR ExportNameToBuff( tDWORD* out_size, tCHAR* buffer, tDWORD size );

	tERROR Read( tPTR pBuffer, tQWORD offset, tDWORD size, tDWORD* pBytesRet );
	tERROR Write( tPTR pBuffer, tQWORD offset, tDWORD size, tDWORD* pBytesRet );

	tERROR GetDriveId( tPTR p_DriveID, tDWORD DriveIdLen );

protected:
	HRESULT FindVolumeNativeName( PWCHAR pwchVolumeName );
	bool FillInfo( PWCHAR pwchVolumeName );

private:
	PVOID					m_pContext;
	PWCHAR					m_pVolumes;
	ULONG					m_VolumesLen;
	PWCHAR					m_pwchVolume;
	tORIG_ID				m_Origin;

	PWCHAR					m_pwchSavedName;
	PWCHAR					m_pwchNativeName;
};

class cGenericAccess : public cDiskAccess
{
public:
	cGenericAccess(hOBJECT hContext);
	virtual ~cGenericAccess();

	bool Init( PWCHAR pwchVolumeName );
	void Reset();
	tERROR Next();

	tERROR ExportNameToBuff( tDWORD* out_size, tCHAR* buffer, tDWORD size );

	tERROR Read( tPTR pBuffer, tQWORD offset, tDWORD size, tDWORD* pBytesRet );
	tERROR Write( tPTR pBuffer, tQWORD offset, tDWORD size, tDWORD* pBytesRet );

protected:
	bool FillInfo( PWCHAR pwchVolumeName );

private:
	HANDLE					m_hVolume;
	hSTRING					m_hVolumes;
};

#endif // __llda