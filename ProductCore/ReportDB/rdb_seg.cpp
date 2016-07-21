#include <minmax.h>
#include <string.h>
#include "rdb_seg.h"
#include "md5.h"
#include <report.h>
#include <../PDM/PDM2/eventmgr/include/cregkey.h>
#include <assert.h>

cReportDBSegment::cReportDBSegment()
{
	m_bInited = false;
//	m_procdb = NULL;
	m_evtdb = NULL;
	m_objdb = NULL;
}

cReportDBSegment::~cReportDBSegment()
{
	m_bInited = false;
	if (m_evtdb)
		delete m_evtdb;
//	if (m_procdb_owner && m_procdb)
//		delete m_procdb;
	if (m_objdb)
		delete m_objdb;
}


tERROR cReportDBSegment::Init(
							  IN cReportDBHelper* pReportHelper, 
							  IN size_t nEventSize,
							  IN bool bWriter, 
							  IN bool bCreate, 
							  IN uint32 nSegment, 
							  IN cObjectDB* pGlobalData
							  )
{
	if (m_bInited)
		return errALREADY;
	if (!pReportHelper)
		return errPARAMETER_INVALID;
	m_pHelper = pReportHelper;
	alFile* pDB;

	m_nSegment = nSegment;
	// cleanup old files if any
	if (bCreate && nSegment)
		m_pHelper->DeleteFile(L"*.*", nSegment);
	// init events database
	tERROR err = m_pHelper->OpenFile(L"events.dat", nSegment, bCreate, bWriter, &pDB);
	if (PR_FAIL(err))
		return err;
	if (!pDB->GetTime(&m_nCreateTime, NULL, NULL))
		return errOBJECT_READ;
	m_evtdb = new cPlainDB();
	if (!m_evtdb)
		return errNOT_ENOUGH_MEMORY;
	if (!m_evtdb->Init(pDB, (uint32)nEventSize))
	{
		pDB->Release();
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	if (!m_evtdb->GetRecordCount())
	{ 
//		tEvent rec = {0};
//		m_evtdb->AddRecord(&rec, sizeof(rec), NULL);
	
	}

	// init data database
	m_objdb = new cObjectDB();
	if (!m_objdb)
		return errNOT_ENOUGH_MEMORY;

	alFile* pDBT;
	alFile* pDBI;
	alFile* pDBD;
	m_pHelper->OpenFile(L"objbt.dat", nSegment, bCreate, bWriter, &pDBT);
	m_pHelper->OpenFile(L"objid.dat", nSegment, bCreate, bWriter, &pDBI);
	m_pHelper->OpenFile(L"objdt.dat", nSegment, bCreate, bWriter, &pDBD);
	if (PR_FAIL(m_objdb->Init(nSegment, pDBT, pDBI, pDBD)))
	{
		if (pDBT) pDBT->Release();
		if (pDBI) pDBI->Release();
		if (pDBD) pDBD->Release();
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	
	// init global data
	if (pGlobalData)
		m_pGlobalData = pGlobalData;
	else
		m_pGlobalData = m_objdb;

	m_bInited = true;
	return errOK;
}


tERROR 
cReportDBSegment::GetEvents (
		   IN tEventId nFirstEvent, 
		   IN size_t nEventsCount, 
		   IN void* pEvents, 
		   IN size_t nBufferSize, 
		   OUT OPTIONAL size_t* pnEventsRead
		   )
{
	uint32 nSegment = GET_SEGMENT_FROM_ID(nFirstEvent);
	if ( nSegment && nSegment != m_nSegment )
		return errOBJECT_NOT_FOUND;
	size_t nEventsRead;
	m_evtdb->GetRecords(nFirstEvent, nEventsCount, pEvents, nBufferSize, &nEventsRead);
	if (pnEventsRead)
		*pnEventsRead = nEventsRead;
	if (nEventsCount != nEventsRead)
		return errEOF;
	return errOK;
}
tERROR 
cReportDBSegment::AddEvent (
					   IN const void* pEvent,
					   IN size_t nEventSize,
					   OUT OPTIONAL tEventId* pnEventId
					   )
{
	tEventId nEventId;
	if (!m_evtdb->AddRecord(pEvent, nEventSize, &nEventId))
		return errOBJECT_WRITE;
	if (pnEventId)
		*pnEventId = MAKE_LONG_OBJECT_ID(nEventId);
	return errOK;
}

tERROR 
cReportDBSegment::UpdateEvent (
							IN tEventId nEventId, 
							IN const void* pEvent,
							IN size_t nEventSize
							)
{
	uint32 nSegment = GET_SEGMENT_FROM_ID(nEventId);
	if ( nSegment && nSegment != m_nSegment )
		return errOBJECT_NOT_FOUND;
	if (!m_evtdb->UpdateRecord(MAKE_SHORT_OBJECT_ID(nEventId), pEvent, nEventSize))
		return errOBJECT_WRITE;
	return errOK;
}


tObjectHash
cReportDBSegment::CalcObjectHash(
	IN uint32       nObjType,
	IN const void*  pName,
	IN size_t       cbNameSize,
	IN tObjectId    nParentId,
	IN uint32       nFlags
	)
{
	if (!pName || 0 == cbNameSize)
		return 0;
	MD5_CTX md5;
	MD5Init(&md5);
	uint32 nType32 = nObjType;
	MD5Update (&md5, (unsigned char*)&nType32, sizeof(nType32));
	if (nParentId)
		MD5Update (&md5, (unsigned char*)&nParentId, sizeof(nParentId));
	if (nFlags & RF_CASE_INSENSETIVE)
	{
		size_t i=0, pos=0;
		const unsigned short* p = (const unsigned short*)pName;
		for (i=0; i<cbNameSize/2; i++)
		{
			unsigned short c = p[i];
			if (c>='a' && c<='z' || c>=0x80)
			{
				c = m_pHelper->ConvertWC2UPR(c);
				MD5Update(&md5, (unsigned char*)(p+pos), (i-pos)*2);
				MD5Update(&md5, (unsigned char*)&c, 2);
				pos = i+1;
			}
		}
		MD5Update(&md5, (unsigned char*)(p+pos), cbNameSize-pos*2);
	}
	else
		MD5Update (&md5, (unsigned char*)pName, cbNameSize);
	MD5Final(&md5);
	uint64* pu64 = (uint64*)md5.digest;
	pu64[0] ^= pu64[1];
	return pu64[0];
}



tERROR
cReportDBSegment::FindObjectEx(
	IN bool         bGlobal,
	IN uint8        nObjType,
	IN uint32       nFlags,
	IN const void*  pName,
	IN size_t       cbNameSize,
	IN tObjectId    nParentId,
	IN bool         bCreate,
	OUT tObjectId*  pnObjectId
	)
{
	tObjectHash hash;
	void* pUtf8 = NULL;
	tERROR error = errOK;
	uint32 nObjFlags = nFlags;
	cObjectDB* pobjdb = bGlobal ? m_pGlobalData : m_objdb;
	if (pnObjectId)
		*pnObjectId = 0;
	if (!pName && !cbNameSize && !nParentId && pnObjectId && bCreate) // create unnamed object
		return pobjdb->AddObject(0, 0, (uint8)nFlags, nObjType, 0, 0, pnObjectId, NULL);
	if (!pName || 0 == cbNameSize || !pnObjectId)
		return errPARAMETER_INVALID;
	hash = CalcObjectHash(nObjType, pName, cbNameSize, nParentId, nObjFlags);
	if (0 == hash)
		return errOBJECT_INCOMPATIBLE;
	cRDBHelperAutoMem _AutoMemUtf8(m_pHelper, &pUtf8);
	if (nFlags & RF_UNICODE)
	{
		size_t nUtf8NameSize;
		bool bSuccess = false;
		if (m_pHelper->ConvertWC2UTF8((const wchar_t*)pName, cbNameSize, 0, 0, &nUtf8NameSize) && nUtf8NameSize > 0 && nUtf8NameSize < cbNameSize)
		{
			pUtf8 = m_pHelper->Alloc(nUtf8NameSize+8);
			if (!pUtf8)
				return errNOT_ENOUGH_MEMORY;
			if (!m_pHelper->ConvertWC2UTF8((const wchar_t*)pName, cbNameSize, (char*)pUtf8, nUtf8NameSize, 0))
				return errCODEPAGE_CONVERSION_UNAVAILABLE;
			pName = pUtf8;
			cbNameSize = nUtf8NameSize;
		}
		else
		{
			nObjFlags &= (~RF_UNICODE); // no conversion for this object
		}
	}
	if (bCreate)
		error = pobjdb->AddObject(hash, nParentId, nObjFlags, nObjType, pName, cbNameSize, pnObjectId, NULL);
	else
		error = pobjdb->FindObject(hash, pnObjectId);
	return error;
}

uint8 GetDirType(uint8 nObjectType)
{
	switch(nObjectType)
	{
	case eFile: return eDirectory;
	case eRegValue: return eRegKey;
	}
	return nObjectType;
}

bool IsUnicodePrefix(const wchar_t* pPath, size_t nPathSize)
{
	const wchar_t* p = pPath;
	if (p[0] == '\\' && (p[1] == '\\' || p[1] == '?') && 
		(p[2] == '?') && (p[3] == '\\'))
		return true;
	return false;
}

size_t IsLogicalDiskName(const wchar_t* pPath, size_t nNameSize)
{
	if (nNameSize < 2)
		return 0;
	if (pPath[1] != ':') // drive letter
		return 0;
	if ((pPath[0] >= 'A' && pPath[0] <= 'Z')
		|| (pPath[0] >= 'a' && pPath[0] <= 'z'))
	{
		return 2;
	}
	return 0;
}

size_t IsVolumeName(const wchar_t* pPath, size_t nNameSize)
{
	// check \\?\Volume{GUID}\...
	if (nNameSize < 49)
		return 0;
	if (pPath[10]!='{' || pPath[47]!='}' || pPath[48]!='\\')
		return 0;
	if (!IsUnicodePrefix(pPath, nNameSize))
		return 0;
	if (0 != _wcsnicmp(pPath+4, L"Volume", 6))
		return 0;
	return 49;
}

size_t IsRegRootName(const wchar_t* pPath, size_t nNameSize)
{
	if (nNameSize == 2) // binary value?
	{
		uint32 nkey = *(uint32*)pPath - 0x80000000;
		if (nkey < countof(sRootKeyNames))
			return 2;
		return 0;
	}
	if (nNameSize < 4)
		return 0;
	if (pPath[0] != 'H' || pPath[1] != 'K')
		return 0;
	if (pPath[2] == 'L' && pPath[3] == 'M')
		return 4;
	if (pPath[2] == 'C' && pPath[3] == 'R')
		return 4;
	if (pPath[2] == 'C' && pPath[3] == 'U')
		return 4;
	if (pPath[2] != 'E' || pPath[3] != 'Y')
		return 0;
	for (size_t i=0; i<countof(sRootKeyNames); i++)
	{
		size_t nLen = wcslen(sRootKeyNames[i]);
		if (nLen > nNameSize)
			continue;
		if (0 == wcsncmp(pPath, sRootKeyNames[i], nLen))
			return nLen;
	}
	return 0;
}

uint32 GetObjectDefaultFlags(uint8 nObjectType)
{
	uint32 nFlags = 0;
	switch (nObjectType)
	{
	case eDevice:
	case eVolume:       return RF_UNICODE | RF_CASE_INSENSETIVE;
	case eLogicalDisk:  return RF_UNICODE | RF_CASE_INSENSETIVE | RF_MAP_SUBST | RF_MAP_VOLUME;
	case eFile:
	case eDirectory:    return RF_UNICODE | RF_CASE_INSENSETIVE | RF_SPLIT_PATH | RF_MAP_SUBST | RF_MAP_VOLUME;
	case eRegKey:       return RF_UNICODE | RF_CASE_INSENSETIVE | RF_SPLIT_PATH;
	case eRegValue:     return RF_UNICODE | RF_CASE_INSENSETIVE;
	}

	return 0;
}

tERROR
cReportDBSegment::FindObject(
									 IN bool         bGlobal,
									 IN uint8        nObjType, 
									 IN uint32       nFlags,
									 IN const void*  pName, 
									 IN OPTIONAL size_t cbNameSize,
									 IN bool         bCreate,
									 IN tObjectId    nParentObjectId,
									 OUT tObjectId*  pnObjectId
									 )
{
	uint32 nDefaultFlags = GetObjectDefaultFlags(nObjType); // get default flags
	if (nFlags)
	{
		nDefaultFlags |= (nFlags & 0xFFFF); // set override flags
		nDefaultFlags &= ~((nFlags >> 16) & 0xFFFF); // drop RF_DONT() override flags
	}
	nFlags = nDefaultFlags;
	if (!cbNameSize && !cbNameSize && bCreate) // create unnamed object
		return FindObjectEx(bGlobal, nObjType, nFlags, 0, 0, nParentObjectId, true, pnObjectId);
	if (!pName && cbNameSize)
		return errPARAMETER_INVALID;
	if (pName && !cbNameSize)
		cbNameSize = cSIZE_WSTR; // default value
	if (cSIZE_WSTR == cbNameSize) // string
		cbNameSize = wcslen((const wchar_t*) pName)*sizeof(wchar_t);
	if (cSIZE_STR == cbNameSize) // string
		cbNameSize = strlen((const char*) pName);
	if (0 == (nFlags & (RF_SPLIT_PATH | RF_SPLIT_FULL_PATH)))
		return FindObjectEx(bGlobal, nObjType, nFlags, pName, cbNameSize, nParentObjectId, bCreate, pnObjectId);

	// here we deal with Unicode paths
	const wchar_t* name = (const wchar_t*)pName;
	size_t size = cbNameSize / sizeof(wchar_t);
	size_t p = 0;

	tObjectId nObjectId = 0;
	tERROR error = errNOT_OK;
	
	if (nParentObjectId)
	{
		// some parent already specified
		nObjectId = nParentObjectId;
	}
	else
	{
		// recognize devices
		uint8 nDeviceType = 0;
		size_t nDeviceNameSize;
		if (nObjType == eRegKey || nObjType == eRegValue)
		{
			if (nDeviceNameSize = IsRegRootName(name+p, size-p))
				nDeviceType = eRegRoot;
		}
		else if (nDeviceNameSize = IsVolumeName(name+p, size-p))
		{
			nDeviceType = eVolume;
		}
		else
		{
			if (IsUnicodePrefix(name+p, size-p))
				p+=4;
			if (nDeviceNameSize = IsLogicalDiskName(name+p, size-p))
				nDeviceType = eLogicalDisk;
		}
		if (nDeviceType)
		{
			error = FindObjectEx(bGlobal, nDeviceType, nFlags | RF_CASE_INSENSETIVE, name+p, nDeviceNameSize*sizeof(wchar_t), 0, true, &nObjectId);
			if (PR_FAIL(error))
				return error;
			p += nDeviceNameSize;

			tObjectId nRelatedObject;
			if (PR_SUCC(GetObjectData(nObjectId, &nRelatedObject, sizeof(nRelatedObject), NULL, eObjectMapping)) && nRelatedObject!=0) 
			{
				uint8 nRealDeviceType = 0;
				if (PR_SUCC(GetObjectName(nRelatedObject, NULL, NULL, NULL, 0, &nRealDeviceType)))
				{

					if ((nRealDeviceType == eDirectory) && (nFlags & RF_MAP_SUBST))
						nObjectId = nRelatedObject;
					else if ((nRealDeviceType == eVolume) && (nFlags & RF_MAP_VOLUME))
						nObjectId = nRelatedObject;
					else if ((nRealDeviceType == eRegKey) && (nFlags & RF_MAP_SUBST))
						nObjectId = nRelatedObject;
					else if ((nRealDeviceType == eRegRoot))
						nObjectId = nRelatedObject;
				}
			}
			nFlags |= RF_CASE_INSENSETIVE;
		}
	}

	// split directories
	while (p < size)
	{
		size_t i, len;
		bool bDir = false;
		while (p < size && (name[p]=='\\' || name[p]=='/')) // skip all slashes
			p++;
		if (p == size) // no more parts
			break;
		for (i=p; i<size; i++)
		{
			if (name[i] == '\\' || name[i] == '/')
			{
				bDir = true;
				break;
			}
		}
		if (p > 0 && name[p-1]== '/') // include forward slash in name to preserve it
			p--; 
		len = i-p;
		error = FindObjectEx(bGlobal, bDir ? GetDirType(nObjType) : nObjType, nFlags, name+p, len*sizeof(wchar_t), nObjectId, bCreate, &nObjectId);
		if (PR_FAIL(error))
			return error;
		p=i;
	}
	if (pnObjectId)
		*pnObjectId = nObjectId;
	return error;
}

void insert_bytes(void* buffer, size_t buffer_size, size_t data_size, size_t insert_size)
{
	if (insert_size > buffer_size)
		return;
	size_t move_size = min(data_size, buffer_size - insert_size);
	if (move_size)
		memmove((byte*)buffer+insert_size, (byte*)buffer, move_size);
	return;
}

tERROR 
cReportDBSegment::GetObjectName(
						   IN tObjectId nObjectId, 
						   OUT OPTIONAL void* pNameBuff, 
						   IN size_t nNameBuffSize, 
						   OUT OPTIONAL size_t* pnNameSize,
						   IN uint32 p_nFlags,
						   OUT OPTIONAL uint8* pnObjectType
						   )
{
	tERROR error;
	tObjectId nParentId;
	size_t nTotalSize = 0;
	size_t nNameSize;
	cObjectDB* pobjdb = m_objdb;
	bool bFirstQuery = true;
	uint32 nFlags = 0;
	if (GET_SEGMENT_FROM_ID(nObjectId) == 0 && m_pGlobalData)
		pobjdb = m_pGlobalData;
	else if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;

	if (!pNameBuff && nNameBuffSize)
		return errPARAMETER_INVALID;
	if (!pNameBuff && !pnNameSize && !pnObjectType)
		return errPARAMETER_INVALID;

	while (nObjectId)
	{
		uint8 nObjectFlags = 0;
		uint8 nObjectType = 0;

		// prepare to concatenate names in buffer and volume mapping
		error = pobjdb->GetObjectName(nObjectId, NULL, 0, &nNameSize, &nParentId, &nObjectFlags, &nObjectType); // get parent name size
		if (PR_FAIL(error))
			return error;
		if (bFirstQuery)
		{
			nFlags = GetObjectDefaultFlags(nObjectType);
			if (p_nFlags)
			{
				nFlags |= (p_nFlags & 0xFFFF); // set override flags
				nFlags &= ~((p_nFlags >> 16) & 0xFFFF); // drop RF_DONT() override flags
			}
		}
		
		// perform mappings 
		if ((nObjectType == eVolume && (nFlags & RF_MAP_VOLUME)) // volume->logical disk
			|| (nObjectType == eRegKey)) // 0x80000002->HKEY_LOCAL_MACHINE
		{
			tObjectId nMapping;
			if (PR_SUCC(GetObjectData(nObjectId, &nMapping, sizeof(nMapping), NULL, eObjectMapping)) && nMapping!=0)
			{
				nObjectId = nMapping;
				continue;
			}
		}
		if (bFirstQuery)
		{
			if (pnObjectType)
				*pnObjectType = nObjectType;
			bFirstQuery = false;
		}
		if (!pNameBuff && !pnNameSize)
			return errOK;

		if (nObjectFlags & RF_UNICODE)
		{
			size_t nWCNameSize;
			void* pUtf8 = NULL;
			bool bSuccess = false;
			pUtf8 = m_pHelper->Alloc(nNameSize);
			if (!pUtf8)
				return errNOT_ENOUGH_MEMORY;
			cRDBHelperAutoMem _AutoMemUtf8(m_pHelper, &pUtf8);
			error = pobjdb->GetObjectName(nObjectId, pUtf8, nNameSize, NULL, NULL, NULL, NULL);
			if (PR_FAIL(error))
				return error;
			if (!m_pHelper->ConvertUTF82WC((const char*)pUtf8, nNameSize, 0, 0, &nWCNameSize))
				return errCODEPAGE_CONVERSION_UNAVAILABLE;
			if (nNameBuffSize)
			{
				insert_bytes(pNameBuff, nNameBuffSize, nTotalSize, nWCNameSize);
				if (!m_pHelper->ConvertUTF82WC((const char*)pUtf8, min(nNameBuffSize, nNameSize), (wchar_t*)pNameBuff, nNameBuffSize/sizeof(wchar_t), &nNameSize))
					return errCODEPAGE_CONVERSION_UNAVAILABLE;
			}
			nTotalSize += nWCNameSize;
		}
		else
		{
			insert_bytes(pNameBuff, nNameBuffSize, nTotalSize, nNameSize);
			error = pobjdb->GetObjectName(nObjectId, pNameBuff, nNameBuffSize, NULL, NULL, NULL, NULL);
			if (errBUFFER_TOO_SMALL == error)
				error = errOK;
			if (PR_FAIL(error))
				return error;
			nTotalSize += nNameSize;
		}
		if (nParentId && nTotalSize >= 2 && nNameBuffSize >= 2) // check - is need to add backslash? 
		{
			if (*(wchar_t*)pNameBuff != '/' && *(wchar_t*)pNameBuff != '\\')
			{
				insert_bytes(pNameBuff, nNameBuffSize, nTotalSize, 2);
				if (nObjectType == eRegValue)
					*(wchar_t*)pNameBuff = '/';
				else
					*(wchar_t*)pNameBuff = '\\';
				nTotalSize += 2;
			}
		}
		nObjectId = nParentId; // for next loop 
	};
	for (size_t z=0; z<2 && nTotalSize+z < nNameBuffSize; z++)
		((byte*)pNameBuff)[nTotalSize+z] = 0;
	if (pnNameSize)
		*pnNameSize = nTotalSize;
	if (pNameBuff && nNameBuffSize < nTotalSize)
		return errBUFFER_TOO_SMALL;
	return errOK;
}

tERROR
cReportDBSegment::GetObjectParent (
	IN tObjectId nObjectId,
	OUT tObjectId* pnParentId,
	OUT OPTIONAL uint8*  pnObjectType
	)
{
	cObjectDB* pobjdb = m_objdb;
	if (GET_SEGMENT_FROM_ID(nObjectId) == 0 && m_pGlobalData)
		pobjdb = m_pGlobalData;
	else if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;

	return pobjdb->GetObjectName(nObjectId, NULL, 0, NULL, pnParentId, NULL, pnObjectType);
}

tERROR
cReportDBSegment::GetObjectData (
			   IN tObjectId nObjectId,
			   OUT OPTIONAL void* pDataBuff, 
			   IN size_t nDataBuffSize, 
			   OUT OPTIONAL size_t* pnDataSize,
			   IN uint32 nDataType
			   )
{
	cObjectDB* pobjdb = m_objdb;
	if (GET_SEGMENT_FROM_ID(nObjectId) == 0 && m_pGlobalData)
		pobjdb = m_pGlobalData;
	else if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	return pobjdb->GetObjectData(nObjectId, pDataBuff,	nDataBuffSize, pnDataSize, nDataType);
}

tERROR
cReportDBSegment::SetObjectData (
								 IN tObjectId nObjectId,
								 IN void* pData, 
								 IN size_t nDataSize,
								 IN uint32 nDataType
								 )
{
	cObjectDB* pobjdb = m_objdb;
	if (GET_SEGMENT_FROM_ID(nObjectId) == 0 && m_pGlobalData)
		pobjdb = m_pGlobalData;
	else if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	return pobjdb->SetObjectData(nObjectId, pData,	nDataSize, nDataType);
}
uint64 cReportDBSegment::GetRecordCount()
{
	return m_evtdb->GetRecordCount();
}

uint64 cReportDBSegment::GetSize()
{
	return m_evtdb->GetSize() + m_objdb->GetSize();
}

uint64 cReportDBSegment::GetTime()
{
	uint64 nSegmentTime = m_pHelper->GetSystemTime() - m_nCreateTime;
	return nSegmentTime / 10000000L;
}