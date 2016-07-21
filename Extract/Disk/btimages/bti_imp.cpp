#include "bti_imp.h"
#include "../Windows/hook/hook/avpgcom.h"


typedef struct _t_media_id
{
	tBOOL	m_bValid;
	tDWORD	m_AreaID;
	tDT		m_DT;
	tDWORD	m_dwHashId;
	tQWORD	m_MediaSize;
	
	tDWORD	m_dwSectorSize;
	tDWORD	m_dwSectorHash;
	tBYTE	m_dwBootSector[1];
}media_id;

//+ ------------------------------------------------------------------------------------------
#define _bti_pattern		"btimptrn"
#define _bti_pattern_len	sizeof(_bti_pattern)
#define _bti_pattern_lenwz	(sizeof(_bti_pattern) - sizeof(""))
//+ ------------------------------------------------------------------------------------------


#define _xor_first_ 13
#define _xor_func(_pbuf, _bufsize, _direction)\
{\
        __int8* pbuf = (__int8*) _pbuf;\
        unsigned __int32 __cou;\
        unsigned __int8 __tmp;\
        unsigned __int8 __prev;\
        unsigned __int8 _xora = _xor_first_;\
        if (_direction)\
        {\
                __prev = _xora;\
                for (__cou = 0; __cou < _bufsize; __cou++)\
                {\
                        _xora++;\
                        __tmp = pbuf[__cou];\
                        pbuf[__cou]  = pbuf[__cou] ^ __prev;\
                        __prev = _xora + __tmp;\
                }\
        }\
        else\
        {\
                __prev = _xora;\
                for (__cou = 0; __cou < _bufsize; __cou++)\
                {\
                        _xora++;\
                        pbuf[__cou]  = pbuf[__cou] ^ __prev;\
                        __prev = _xora + pbuf[__cou];\
                }\
        }\
}

unsigned long CountCRC32(unsigned int size, unsigned char* ptr, unsigned long old_crc)
{
    old_crc = ~old_crc;
    while(size-- != 0)
    {
        unsigned long i;
        unsigned long c = *ptr++;
        for( i=0; i<8; i++)
        {
            if (((old_crc^c)&1)!=0)
                old_crc = (old_crc>>1) ^ 0xEDB88320;
            else
                old_crc = (old_crc>>1);
            c>>=1;
        }
    }
    return (~old_crc);
}

unsigned char CleanBoot_Fat16[512] = {
	0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x01, 0x01, 0x00, 
	0x02, 0xE0, 0x00, 0x40, 0x0B, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x6F, 0x7D, 0x69, 0xD4, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 
	0x4D, 0x45, 0x20, 0x20, 0x20, 0x20, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x33, 0xC9, 
	0x8E, 0xD1, 0xBC, 0xF0, 0x7B, 0x8E, 0xD9, 0xB8, 0x00, 0x20, 0x8E, 0xC0, 0xFC, 0xBD, 0x00, 0x7C, 
	0x38, 0x4E, 0x24, 0x7D, 0x24, 0x8B, 0xC1, 0x99, 0xE8, 0x3C, 0x01, 0x72, 0x1C, 0x83, 0xEB, 0x3A, 
	0x66, 0xA1, 0x1C, 0x7C, 0x26, 0x66, 0x3B, 0x07, 0x26, 0x8A, 0x57, 0xFC, 0x75, 0x06, 0x80, 0xCA, 
	0x02, 0x88, 0x56, 0x02, 0x80, 0xC3, 0x10, 0x73, 0xEB, 0x33, 0xC9, 0x8A, 0x46, 0x10, 0x98, 0xF7, 
	0x66, 0x16, 0x03, 0x46, 0x1C, 0x13, 0x56, 0x1E, 0x03, 0x46, 0x0E, 0x13, 0xD1, 0x8B, 0x76, 0x11, 
	0x60, 0x89, 0x46, 0xFC, 0x89, 0x56, 0xFE, 0xB8, 0x20, 0x00, 0xF7, 0xE6, 0x8B, 0x5E, 0x0B, 0x03, 
	0xC3, 0x48, 0xF7, 0xF3, 0x01, 0x46, 0xFC, 0x11, 0x4E, 0xFE, 0x61, 0xBF, 0x00, 0x00, 0xE8, 0xE6, 
	0x00, 0x72, 0x39, 0x26, 0x38, 0x2D, 0x74, 0x17, 0x60, 0xB1, 0x0B, 0xBE, 0xA1, 0x7D, 0xF3, 0xA6, 
	0x61, 0x74, 0x32, 0x4E, 0x74, 0x09, 0x83, 0xC7, 0x20, 0x3B, 0xFB, 0x72, 0xE6, 0xEB, 0xDC, 0xA0, 
	0xFB, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC, 0x98, 0x40, 0x74, 0x0C, 0x48, 0x74, 0x13, 0xB4, 0x0E, 
	0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xEF, 0xA0, 0xFD, 0x7D, 0xEB, 0xE6, 0xA0, 0xFC, 0x7D, 0xEB, 
	0xE1, 0xCD, 0x16, 0xCD, 0x19, 0x26, 0x8B, 0x55, 0x1A, 0x52, 0xB0, 0x01, 0xBB, 0x00, 0x00, 0xE8, 
	0x3B, 0x00, 0x72, 0xE8, 0x5B, 0x8A, 0x56, 0x24, 0xBE, 0x0B, 0x7C, 0x8B, 0xFC, 0xC7, 0x46, 0xF0, 
	0x3D, 0x7D, 0xC7, 0x46, 0xF4, 0x29, 0x7D, 0x8C, 0xD9, 0x89, 0x4E, 0xF2, 0x89, 0x4E, 0xF6, 0xC6, 
	0x06, 0x96, 0x7D, 0xCB, 0xEA, 0x03, 0x00, 0x00, 0x20, 0x0F, 0xB6, 0xC8, 0x66, 0x8B, 0x46, 0xF8, 
	0x66, 0x03, 0x46, 0x1C, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 0xEB, 0x5E, 0x0F, 0xB6, 0xC8, 
	0x4A, 0x4A, 0x8A, 0x46, 0x0D, 0x32, 0xE4, 0xF7, 0xE2, 0x03, 0x46, 0xFC, 0x13, 0x56, 0xFE, 0xEB, 
	0x4A, 0x52, 0x50, 0x06, 0x53, 0x6A, 0x01, 0x6A, 0x10, 0x91, 0x8B, 0x46, 0x18, 0x96, 0x92, 0x33, 
	0xD2, 0xF7, 0xF6, 0x91, 0xF7, 0xF6, 0x42, 0x87, 0xCA, 0xF7, 0x76, 0x1A, 0x8A, 0xF2, 0x8A, 0xE8, 
	0xC0, 0xCC, 0x02, 0x0A, 0xCC, 0xB8, 0x01, 0x02, 0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4, 0x42, 
	0x8B, 0xF4, 0x8A, 0x56, 0x24, 0xCD, 0x13, 0x61, 0x61, 0x72, 0x0B, 0x40, 0x75, 0x01, 0x42, 0x03, 
	0x5E, 0x0B, 0x49, 0x75, 0x06, 0xF8, 0xC3, 0x41, 0xBB, 0x00, 0x00, 0x60, 0x66, 0x6A, 0x00, 0xEB, 
	0xB0, 0x4E, 0x54, 0x4C, 0x44, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x52, 0x65, 
	0x6D, 0x6F, 0x76, 0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x73, 0x20, 0x6F, 0x72, 0x20, 0x6F, 0x74, 
	0x68, 0x65, 0x72, 0x20, 0x6D, 0x65, 0x64, 0x69, 0x61, 0x2E, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73, 
	0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 
	0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61, 
	0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0xCB, 0xD8, 0x55, 0xAA
};

//+ ------------------------------------------------------------------------------------------
cMutex* bti_InitMutex(hOBJECT hbti, tDWORD dwRepositaryNameHash)
{
	tERROR error;
	cMutex*  hSync;

	if (PR_FAIL(error = hbti->sysCreateObject((hOBJECT*)&hSync, IID_MUTEX)))
	{
		PR_TRACE((g_root, prtERROR, "bti\tSynchronization object creation failed with %terr", error));
		return NULL;
	}

	tDWORD dwBytes;
	tCHAR  sName[0x200];

	dwBytes = pr_sprintf((tCHAR*)sName, sizeof(sName), "sync_bti_%08X", dwRepositaryNameHash);

	if (PR_FAIL(error = hSync->set_pgGLOBAL(cTRUE)))
	{
		PR_TRACE((g_root, prtERROR, "bti\tfailed to set pgGLOBAL for synchronization object with %terr", error));
	}
	else if (PR_FAIL(error = hSync->set_pgOBJECT_NAME(sName, dwBytes)))
	{
		PR_TRACE((g_root, prtERROR, "bti\tSynchronization object creation failed with %terr", error));
	}
	else if (PR_FAIL(error = hSync->sysCreateObjectDone()))
	{
		PR_TRACE((g_root, prtERROR, "bti\tSynchronization object creation(done) failed with %terr", error));
	}
	else
		return hSync;

	hSync->sysCloseObject();
	
	return NULL;
}

//+ ------------------------------------------------------------------------------------------

tERROR bti_init(hSTRING hRepositoryName, hIO* pRepositoryIO)
{
	hIO hio;
	if (PR_SUCC(hRepositoryName->sysCreateObject((hOBJECT*) &hio, IID_IO, PID_WIN32_NFIO)))
	{
		hRepositoryName->ExportToProp(0, cSTRING_WHOLE, (cObject*) hio, pgOBJECT_FULL_NAME);
		hio->propSetDWord(pgOBJECT_OPEN_MODE, fOMODE_CREATE_IF_NOT_EXIST | fOMODE_OPEN_IF_EXIST);
		hio->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_READ | fACCESS_WRITE);
		if (PR_SUCC(hio->sysCreateObjectDone()))
		{
			//check container
			tCHAR pattern[_bti_pattern_len];
			memcpy(pattern, _bti_pattern, _bti_pattern_len);

			tDWORD dwsize = 0;
			tQWORD qsize = 0;
			hio->GetSize(&qsize, IO_SIZE_TYPE_EXPLICIT);
			if (qsize == 0)
			{
				// new FILE - generate header and return;
				if (PR_SUCC(hio->SeekWrite(&dwsize, 0, pattern, _bti_pattern_len)) && dwsize == _bti_pattern_len)
				{
					*pRepositoryIO = hio;
					return errOK;
				}

				PR_TRACE((g_root, prtERROR, "bti\tcan't write pattern"));
				hio->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
			}
			else
			{
				tCHAR pattern_check[_bti_pattern_len];
				if (PR_SUCC(hio->SeekRead(&dwsize, 0, pattern_check, _bti_pattern_lenwz)) && dwsize == _bti_pattern_lenwz)
				{
					if (memcmp(pattern, pattern_check, _bti_pattern_lenwz) == 0)
					{
						*pRepositoryIO = hio;
						return errOK;
					}
				}

				PR_TRACE((g_root, prtERROR, "bti\twrong pattern"));
			}
		}

		hio->sysCloseObject();
	}

	return errOBJECT_CANNOT_BE_INITIALIZED;
}

media_id* bti_fill_media(hIO llio)
{
	tDWORD sectorsize = llio->propGetDWord(plBytesPerSector);
	if (!sectorsize)
		return 0;

	tDWORD drivetype = llio->propGetDWord(plDriveType);	// DEVICE_TYPE

	if (drivetype == FILE_DEVICE_CD_ROM || drivetype == FILE_DEVICE_DVD)
		return 0;

	tDWORD mediatype = llio->propGetDWord(plMediaType);	// MEDIA_TYPE
	if (mediatype == Unknown)
		return 0;

	media_id* pmedia;
	if (PR_FAIL(llio->heapAlloc((tPTR*) &pmedia, sizeof(media_id) + sectorsize)))
		return 0;

	memset(pmedia, 0, sizeof(media_id));

	pmedia->m_bValid = cTRUE;
	Now(&pmedia->m_DT);
	pmedia->m_dwSectorSize = sectorsize;

	//+ ------------------------------------------------------------------------------------------

	OSVERSIONINFO OSVer;
	OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&OSVer);

	DISK_ID_INFO info;
	WCHAR DriveName[DISKNAMEMAXLEN]; //for 9x - first byte is drive letter
	
	memset(DriveName, 0, sizeof(DriveName));

	if (OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
		llio->get_pgOBJECT_FULL_NAME(DriveName, sizeof(DriveName), cCP_UNICODE);
	else
		llio->get_pgOBJECT_FULL_NAME(DriveName, sizeof(DriveName), cCP_ANSI);

	tERROR error;

	tORIG_ID origin = llio->get_pgOBJECT_ORIGIN();

	error = exDiskExQueryDriveID((tSTRING) DriveName, (tDWORD*) &info, sizeof(info));
	if (PR_FAIL(error))
	{
		memset(&info, 0, sizeof(DISK_ID_INFO));
		if (origin == OID_PHYSICAL_DISK)
		{
			memcpy(info.m_PartitionName, DriveName, min(DISKNAMEMAXLEN, sizeof(info.m_PartitionName)));
			info.m_PartitionName[countof(info.m_PartitionName) - 1] = 0;
			error = errOK;
		}
		else
		{
			if (mediatype != FixedMedia)
			{
				DWORD dwSerialNumber = 0;

				((char*)DriveName)[1] = ':';
				((char*)DriveName)[2] = '\\';
				if (GetVolumeInformation((char*) DriveName, 0, 0, &dwSerialNumber, NULL, NULL, NULL, 0))
				{
					memcpy(info.m_PartitionName, DriveName, min(DISKNAMEMAXLEN, sizeof(info.m_PartitionName)));
					info.m_PartitionName[countof(info.m_PartitionName) - 1] = 0;
					memcpy(info.m_SerialNumber, &dwSerialNumber, sizeof(dwSerialNumber));

					error = errOK;
				}
			}
		}
	}

	if (PR_SUCC(error))
	{
		pmedia->m_dwHashId = CountCRC32(sizeof(info), (unsigned char*) &info, 0);

		tDWORD dwr;
		error = llio->SeekRead(&dwr, 0, pmedia->m_dwBootSector, sectorsize);

		_xor_func(pmedia->m_dwBootSector, sectorsize, 1);

		if (PR_SUCC(error) || (dwr == sectorsize))
		{
			error = llio->GetSize(&pmedia->m_MediaSize, IO_SIZE_TYPE_EXPLICIT);
		}

		if (PR_SUCC(error))
		{
			if (origin == OID_LOGICAL_DRIVE && mediatype == FixedMedia)
			{
				//! this is boot - special calc hash
				pmedia->m_dwSectorHash = CountCRC32(sectorsize, (unsigned char*) &pmedia->m_dwBootSector, 0);
			}
			else
			{
				// mbr and removable
				pmedia->m_dwSectorHash = CountCRC32(sectorsize, (unsigned char*) &pmedia->m_dwBootSector, 0);
				switch (mediatype)
				{
				case FixedMedia:
					break;
				case RemovableMedia:
					break;
				default:
					pmedia->m_dwHashId = CountCRC32(sizeof(mediatype), (unsigned char*) &mediatype, pmedia->m_dwHashId);
					break;
				}
			}
		}
	}

	if (PR_SUCC(error))
		return pmedia;

	llio->heapFree(pmedia);
	return 0;
}

tERROR bti_find_image(hIO pRepositoryIO, media_id* pmedia, tDWORD *pAreaID, tDWORD* pdwSectorHash, tQWORD* pfilepos)
{
	tERROR errorRet = errNOT_FOUND;
	tERROR error;

	tQWORD pos = _bti_pattern_len; 
	tQWORD fileposret = 0;

	media_id media_tmp;

	tDWORD find_area = *pAreaID;

	tDWORD dwr;
	while (1)
	{
		error = pRepositoryIO->SeekRead(&dwr, pos, &media_tmp, sizeof(media_id));
		if (PR_FAIL(error) || dwr != sizeof(media_id))
		{
			// corrupted file!
			pRepositoryIO->SetSize(pos);
			if (PR_FAIL(errorRet))
				fileposret = pos;
			break;
		}
		else
		{
			if (media_tmp.m_bValid)
			{
				if ((media_tmp.m_dwSectorSize == pmedia->m_dwSectorSize) &&
					(media_tmp.m_MediaSize == pmedia->m_MediaSize) &&
					(media_tmp.m_dwHashId == pmedia->m_dwHashId))
				{
		;			if (find_area == BTIMAGES_LAST)
					{
						*pAreaID = media_tmp.m_AreaID;
						*pdwSectorHash = media_tmp.m_dwSectorHash;
					
						fileposret = pos;						
						errorRet = errOK;
					}
					else if (find_area == media_tmp.m_AreaID)
					{
						*pdwSectorHash = media_tmp.m_dwSectorHash;
						
						fileposret = pos;
						errorRet = errOK;
						break;
					}
				}
			}

			pos += sizeof(media_id) + media_tmp.m_dwSectorSize;
		}
	}

	if (pfilepos)
		*pfilepos = fileposret;

	return errorRet;
}
//+ ------------------------------------------------------------------------------------------
tBOOL IsMediaFloppy_Fat16(media_id* pmedia)
{
	if (pmedia->m_dwSectorSize == 512)
	{
		tBYTE mediatype = *(pmedia->m_dwBootSector + 0x15);
		if (mediatype == 0xf0)	//"high-density 3.5-inch floppy disk";
			return cTRUE;
		else
		{
			PR_TRACE((g_root, prtIMPORTANT, "bti\tdefault boot not found for media 0x%x", mediatype));
		}
	}

	return cFALSE;
}

tERROR bti_save(hIO pRepositoryIO, hIO llio)
{
	tERROR error = errMODULE_UNKNOWN_FORMAT;
	media_id* pmedia = bti_fill_media(llio);
	if (!pmedia)
		return error;

	tQWORD filepos;
	tDWORD AreaID = BTIMAGES_LAST;

	tDWORD	dwSectorHash = 0;
	if PR_SUCC(bti_find_image(pRepositoryIO, pmedia, &AreaID, &dwSectorHash, &filepos))
	{
		if (dwSectorHash == pmedia->m_dwSectorHash)
			error = errOK;
		else
		{
			if (AreaID == 0)
			{
				// add new
				tDWORD	dwSectorSizeTmp = pmedia->m_dwSectorSize;
				pmedia->m_dwSectorSize = 33;	// думаю, такого размера не найдешь в реале
				bti_find_image(pRepositoryIO, pmedia, &AreaID, &dwSectorHash, &filepos);
				
				pmedia->m_dwSectorSize = dwSectorSizeTmp;
			}

			pmedia->m_AreaID = AreaID;
			error = pRepositoryIO->SeekWrite(0, filepos, pmedia, sizeof(media_id) + pmedia->m_dwSectorSize);
		}
	}
	else
	{
		// add first
		pmedia->m_AreaID = 0;
		error = pRepositoryIO->SeekWrite(0, filepos, pmedia, sizeof(media_id) + pmedia->m_dwSectorSize);
	}

	llio->heapFree(pmedia);

	return error;
}

tERROR bti_restore(hIO pRepositoryIO, hIO llio, tDWORD p_index)
{
	tERROR error = errUNEXPECTED;

	PR_TRACE((g_root, prtIMPORTANT, "bti\trestore request"));
	
	media_id* pmedia = bti_fill_media(llio);
	if (!pmedia)
	{
		PR_TRACE((g_root, prtERROR, "bti\tfailed: no media"));
		return error;
	}

	tDWORD AreaID = p_index;

	tQWORD filepos;
	tDWORD	dwSectorHash = 0;

	tDWORD dwr;

	error = bti_find_image(pRepositoryIO, pmedia, &AreaID, &dwSectorHash, &filepos);
	if (PR_FAIL(error))
	{
		if (pmedia->m_dwSectorSize == 512)
		{
			_xor_func(pmedia->m_dwBootSector, pmedia->m_dwSectorSize, 0);
			if (IsMediaFloppy_Fat16(pmedia))
			{
				memcpy(pmedia->m_dwBootSector, CleanBoot_Fat16, sizeof(CleanBoot_Fat16));
				error = errOK;

				PR_TRACE((g_root, prtIMPORTANT, "bti\trestoring default boot sector (floppy)"));
			}
		}
	}
	else
	{
		if (PR_SUCC(error))
		{
			tDWORD rsize = sizeof(media_id) + pmedia->m_dwSectorSize;
			
			error = pRepositoryIO->SeekRead(&dwr, filepos, pmedia, rsize);
			_xor_func(pmedia->m_dwBootSector, pmedia->m_dwSectorSize, 0);

			if (dwr != rsize)
				error = errUNEXPECTED;
		}
	}

	if (PR_SUCC(error))
		error = llio->SeekWrite(&dwr, 0, pmedia->m_dwBootSector, pmedia->m_dwSectorSize);

	llio->heapFree(pmedia);

	PR_TRACE((g_root, prtIMPORTANT, "bti\trestore request result %terr", error));

	return error;
}

tERROR bti_getimagedatetime(hIO pRepositoryIO, hIO llio, tDWORD p_index, tDATETIME* p_dt)
{
	tERROR error = errMODULE_UNKNOWN_FORMAT;
	
	media_id* pmedia = bti_fill_media(llio);
	if (!pmedia)
		return error;

	tDWORD AreaID = p_index;

	tQWORD filepos;
	tDWORD	dwSectorHash = 0;
	error = bti_find_image(pRepositoryIO, pmedia, &AreaID, &dwSectorHash, &filepos);
	if (PR_SUCC(error))
	{
		tDWORD rsize = sizeof(media_id) + pmedia->m_dwSectorSize;
		tDWORD dwr;
		
		error = pRepositoryIO->SeekRead(&dwr, filepos, pmedia, rsize);
		if (PR_SUCC(error) && (dwr == rsize))
			memcpy(p_dt, pmedia->m_DT, sizeof(tDATETIME));
		
		error = errOK;
	}

	llio->heapFree(pmedia);
	return error;
}