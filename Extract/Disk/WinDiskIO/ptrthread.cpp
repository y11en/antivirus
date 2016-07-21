#include "ptrthread.h"
#include <prague.h>
#include <iface/i_buffer.h>
#include "llda.h"

tERROR exDiskExLogical_GetDriveType(hSTRING hStrLogical, tDWORD* pDriveType)
{
	if (!hStrLogical || !pDriveType)
		return errPARAMETER_INVALID;
	
	tDWORD Length = 0;
	PWCHAR pwchDrive = NULL;

	tERROR error = hStrLogical->Length( &Length );
	if (!PR_SUCC( error ))
		return errPARAMETER_INVALID;

	error = hStrLogical->heapAlloc( (tPTR*) &pwchDrive,  Length + sizeof(WCHAR) );
	if (!PR_SUCC( error ))
		return errNOT_ENOUGH_MEMORY;

	error = hStrLogical->ExportToBuff( &Length, cSTRING_WHOLE, pwchDrive, Length, cCP_UNICODE, 0 );
	if (PR_SUCC( error ))
	{
		*pDriveType = GetDriveTypeW( pwchDrive );
		error = errOK;
	}

	hStrLogical->heapFree( pwchDrive );
	
	return error;
}

tERROR exDiskExLogical_Next(hSTRING hStrLogical)
{
	if (!hStrLogical)
		return errHANDLE_INVALID;

	tPROPID propid_buffer;
	tPTR ptr = 0;

	hROOT hroot = (hROOT) CALL_SYS_ParentGet(hStrLogical, IID_ROOT);
	if (!hroot)
		return errHANDLE_INVALID;

	if (PR_FAIL(CALL_Root_RegisterCustomPropId(hroot, &propid_buffer, cLLDISKIO_BUFFER_PROP, pTYPE_OBJECT)))
		return errOBJECT_CANNOT_BE_INITIALIZED;

	hBUFFER hBuf = NULL;

	tERROR err;
	hBuf = (hBUFFER) CALL_SYS_PropertyGetDWord(hStrLogical, propid_buffer);
	if (!hBuf)
	{
		if (PR_SUCC(CALL_SYS_ObjectCreateQuick(hStrLogical, &hBuf, IID_BUFFER, PID_ANY, SUBTYPE_ANY)))
		{
			err = CALL_SYS_PropertySetDWord(hStrLogical, propid_buffer, (tDWORD) hBuf);
			if (PR_SUCC(err))
			{
				DWORD len = GetLogicalDriveStrings(0, 0);
				err = CALL_Buffer_SizeLock(hBuf, &ptr, len);
				if (PR_SUCC(err))
				{
					GetLogicalDriveStrings(len, (char*) ptr);
					CALL_Buffer_Unlock(hBuf);
				}
			}
			if (PR_FAIL(err))
			{
				CALL_SYS_ObjectClose(hBuf);
				hBuf = NULL;
			}
		}
	}

	if (hBuf)
		CALL_Buffer_Lock(hBuf, &ptr);

	if (!ptr || !hBuf)
		return errOBJECT_CANNOT_BE_INITIALIZED;

	int len = lstrlen((char*) ptr);
	tQWORD total_len;
	CALL_Buffer_GetSize(hBuf, &total_len, IO_SIZE_TYPE_EXPLICIT);
	err = errOK;
	if (len)
	{
		err = CALL_String_ImportFromBuff(hStrLogical, 0, ptr, len, cCP_ANSI, cSTRING_Z);
		memcpy(ptr, (char*) ptr + len + 1, (tDWORD) total_len - len);
		total_len -= (len + 1);
	}

	CALL_Buffer_Unlock(hBuf);
	CALL_Buffer_SetSize(hBuf, total_len);

	if (len)
		return err;

	return errEND_OF_THE_LIST;
}

tBOOL pr_call exDiskExQueryLockStatus(tCHAR DrvName, tBOOL* pbLocked)
{
	return FALSE;
}

tERROR pr_call exDiskExIsPartition(tCHAR p_DrvName, tBOOL* p_pbIsPartition)
{
	return errNOT_SUPPORTED;
}

tERROR pr_call exDiskExQueryDriveID(tSTRING p_DrvName, tDWORD* p_DriveID, tDWORD DriveIdLen)
{
	tERROR error = errUNEXPECTED;
	
	cDrvAccess* pLLDisk = new cDrvAccess( NULL );

	if (!pLLDisk)
		return errNOT_ENOUGH_MEMORY;

	if (pLLDisk->Init( (PWCHAR) p_DrvName ))
		error = pLLDisk->GetDriveId( p_DriveID, DriveIdLen );

	delete pLLDisk;

	return error;
}

//+ ------------------------------------------------------------------------------------------

tERROR pr_call exDiskExIsSubst( hSTRING hStrLogical )
{
	if( !hStrLogical )
		return errPARAMETER_INVALID;

	char drvname[MAX_PATH];
	tERROR err = CALL_String_ExportToBuff(hStrLogical, 0, cSTRING_WHOLE, drvname, sizeof(drvname), cCP_ANSI, cSTRING_Z);
	if (PR_FAIL(err))
		return err;

	drvname[2] = 0;
	tCHAR pTargetPath[MAX_PATH];
	if( !::QueryDosDevice(drvname, pTargetPath, MAX_PATH) )
		return errNOT_OK;

	if( !strncmp(pTargetPath, "\\??\\", 4) )
		return errOK;

	return errNOT_OK;
}
