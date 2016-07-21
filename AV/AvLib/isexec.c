#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>

#include <AV/iface/i_engine.h>
#include <AV/plugin/p_avlib.h>

#include "isexecbuff.h"

#include <string.h>

tERROR iTryGetName(hIO io, tPROPID prop, tDWORD* pdwSize, tCODEPAGE* pCP)
{
	tERROR error;
	tDWORD dwSize;
	tCODEPAGE cp;

	if( PR_FAIL(error = CALL_SYS_PropertyGetStrCP(io, &cp, prop)))
		return error;
	if( PR_FAIL(error = CALL_SYS_PropertyGetStr(io, &dwSize, prop, NULL, 0, cp)))
		return error;
	if (pCP)
		*pCP = cp;
	if (pdwSize)
		*pdwSize = dwSize;

	return error;
}


tERROR iGetName(hIO io, tDWORD* pdwSize, tBYTE** ppName)
{
	tERROR error;
	tDWORD dwSize;
	tBYTE* pBuffer;
	static tPROPID propid_virtual_name = 0;
	tPROPID prop;
	tCODEPAGE cp;

	if (!ppName)
		return errPARAMETER_INVALID;

	if (!propid_virtual_name)
		error = CALL_Root_RegisterCustomPropId( (hROOT)CALL_SYS_ParentGet(io, IID_ROOT), &propid_virtual_name, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING );
	
	
	error = iTryGetName(io, prop = propid_virtual_name, &dwSize, &cp);
	if (PR_FAIL(error))
		error = iTryGetName(io, prop = pgOBJECT_NAME, &dwSize, &cp);
	if (PR_FAIL(error))
		error = iTryGetName(io, prop = pgOBJECT_FULL_NAME, &dwSize, &cp);

	if (PR_FAIL(error))
		return error;

	if (PR_FAIL(error = CALL_SYS_ObjHeapAlloc(io, &pBuffer, dwSize)))
		return error;

	if (PR_FAIL(error = CALL_SYS_PropertyGetStr(io, NULL, prop, pBuffer, dwSize, cp)))
	{
		CALL_SYS_ObjHeapFree(io, pBuffer);
		return error;
	}

	if (cp == cCP_UNICODE)
	{
		// simple convert Unicode->ANSI, non-ACSII character >127 replaced with '?'
		tDWORD i;
		tWCHAR* pwc = (tWCHAR*)pBuffer;
		dwSize /= 2;
		for (i=0; i<dwSize; i++)
		{
			tWCHAR c = pwc[i];
			if (c >= 0x80)
				c = '?';
			pBuffer[i] = (tBYTE)c;
		}
		cp = cCP_ANSI;
	}

	if (cp != cCP_ANSI)
	{
		CALL_SYS_ObjHeapFree(io, pBuffer);
		return error = errCODEPAGE_NOT_SUPPORTED;
	}

	*ppName = pBuffer;

	if (pdwSize)
		*pdwSize = dwSize;

	return error;
}
	


tERROR iGetExtensionFromName(tBYTE* pName, tDWORD dwSize, tBYTE* pBuffer, tDWORD dwBuffSize, tDWORD* pdwOutSize)
{
	tERROR error = errOK;
	tDWORD dwExtSize = 0;
	tDWORD dwExtPtr;
	tDWORD dwStreamPtr;
	
	dwStreamPtr = dwExtPtr = dwSize;
	while (dwExtPtr > 0)
	{
		dwExtPtr--;
		if (pName[dwExtPtr] == '\\' || pName[dwExtPtr] == '/')
		{
			dwExtPtr = 0;
			break;
		}
		else
		if (pName[dwExtPtr] == '.')
		{
			dwExtSize = dwStreamPtr - dwExtPtr;
			if (dwExtSize > dwBuffSize && dwBuffSize!=0)
				error = errBUFFER_TOO_SMALL;
			memcpy(pBuffer, pName+dwExtPtr, min(dwExtSize, dwBuffSize));
			break;
		}
		else
		if (pName[dwExtPtr] == ':')
			dwStreamPtr = dwExtPtr;
	}

	if (dwExtSize == 0)
	{
		if (dwExtSize < dwBuffSize)
			pBuffer[dwExtSize++] = '.'; // force empty extension
		else
			error = errBUFFER_TOO_SMALL;
	}

	if (dwExtSize < dwBuffSize)
		pBuffer[dwExtSize] = 0; // zero terminate

	if (pdwOutSize)
		*pdwOutSize = dwExtSize;

	return error;
}

tERROR iGetStreamFromName(tBYTE* pName, tDWORD dwSize, tBYTE* pBuffer, tDWORD dwBuffSize, tDWORD* pdwOutSize)
{
	tERROR error = errOK;
	tDWORD dwStreamSize = 0;
	tDWORD dwStreamPtr;
	
	dwStreamPtr = dwSize;
	while (dwStreamPtr > 0)
	{
		dwStreamPtr--;
		if (pName[dwStreamPtr] == '\\' || pName[dwStreamPtr] == '/')
		{
			dwStreamPtr = 0;
			break;
		}
		else
		if (pName[dwStreamPtr] == ':')
		{
			dwStreamSize = dwSize - dwStreamPtr;
			if (pBuffer)
			{
				if (dwStreamSize > dwBuffSize && dwBuffSize!=0)
					error = errBUFFER_TOO_SMALL;
				memcpy(pBuffer, pName+dwStreamPtr, min(dwStreamSize, dwBuffSize));
			}
			break;
		}
	}

	if (pdwOutSize)
		*pdwOutSize = dwStreamSize;

	if (dwStreamSize < dwBuffSize)
		pBuffer[dwStreamSize] = 0; // zero terminate

	if (dwStreamSize == 0)
		error = errNOT_FOUND;
	return error;
}


tERROR pr_call AVLIB_IsExecutableExtension(hIO io, tDWORD* pdwResult)
{
	tERROR error;
	tDWORD dwExtSize;
	tDWORD dwNameSize;
	tDWORD dwStreamNameSize;
	tBYTE* pName;
	tBYTE  byExtension[cMAX_EXT_SIZE];

	if (pdwResult == NULL)
		return errPARAMETER_INVALID;

#ifdef _DEBUG
	if (PR_FAIL(CALL_SYS_ObjectCheck((hOBJECT)io, (hOBJECT)io, IID_IO, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		PR_TRACE((io, prtERROR, "AVLIB_IsProgram: Input object is not compaible with IID_IO"));
		return errOBJECT_INCOMPATIBLE;
	}
#endif

	if( PR_FAIL(error = iGetName(io, &dwNameSize, &pName)))
		return error;

	error = iGetExtensionFromName(pName, dwNameSize, byExtension, sizeof(byExtension), &dwExtSize);
	if (error == errBUFFER_TOO_SMALL)
	{
		error = errOK;
		dwExtSize = sizeof(byExtension);
	}

	if (PR_SUCC(error))
		error = AVLIB_IsExecutableExtensionBuff(byExtension, dwExtSize, pdwResult);

	// extract stream name
	{
		tERROR error;
		error = iGetStreamFromName(pName, dwNameSize, NULL, 0, &dwStreamNameSize);
		// VIK 30.03.04
		//if (error = errBUFFER_TOO_SMALL)
		if (error == errBUFFER_TOO_SMALL)
			error = errOK;
		if (PR_SUCC(error))
			*pdwResult |= cAVLIB_STREAM;
	}


	CALL_SYS_ObjHeapFree(io, pName);
	
	return error;
}

tERROR pr_call AVLIB_IsExecutableFormat(hIO io, tDWORD* pdwResult)
{
	tERROR error;
	tBYTE byBuffer[cFORMAT_DATA_SIZE];
	tDWORD dwSize;

#ifdef _DEBUG
	if (PR_FAIL(CALL_SYS_ObjectCheck((hOBJECT)io, (hOBJECT)io, IID_IO, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		PR_TRACE((io, prtERROR, "AVLIB_IsProgram: Input object is not compaible with IID_IO"));
		return errOBJECT_INCOMPATIBLE;
	}
#endif

	if (PR_FAIL(error = CALL_IO_SeekRead(io, &dwSize, 0, byBuffer, sizeof(byBuffer))))
	{
		PR_TRACE((io, prtERROR, "AVLIB_IsProgram: Cannot read data from file"));
		dwSize = 0;
	}

	if (PR_SUCC(error))
		error = AVLIB_IsExecutableFormatBuff(byBuffer, dwSize, pdwResult);

	return error;
}

tERROR pr_call AVLIB_IsExecutableFormatLite(hIO io, tDWORD* pdwResult)
{
	tERROR error;
	tBYTE byBuffer[cFORMAT_DATA_SIZE];
	tDWORD dwSize;

#ifdef _DEBUG
	if (PR_FAIL(CALL_SYS_ObjectCheck((hOBJECT)io, (hOBJECT)io, IID_IO, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		PR_TRACE((io, prtERROR, "AVLIB_IsProgram: Input object is not compaible with IID_IO"));
		return errOBJECT_INCOMPATIBLE;
	}
#endif

	if (PR_FAIL(error = CALL_IO_SeekRead(io, &dwSize, 0, byBuffer, sizeof(byBuffer))))
	{
		PR_TRACE((io, prtERROR, "AVLIB_IsProgram: Cannot read data from file"));
		dwSize = 0;
	}

	if (PR_SUCC(error))
		error = AVLIB_IsExecutableFormatBuffLite(byBuffer, dwSize, pdwResult);

	return error;
}

tERROR pr_call AVLIB_IsProgram(hIO io, tDWORD* pdwResult)
{
	tERROR error;
	tERROR ret_error = errNOT_OK;
	tDWORD dwResult;

	if (pdwResult == NULL)
		return error = errPARAMETER_INVALID;

	*pdwResult = 0;
	
	dwResult = 0;
	error = AVLIB_IsExecutableExtension(io, &dwResult);
	if( PR_SUCC(error))
	{
		ret_error = errOK;
		*pdwResult |= dwResult;
	}
	
	dwResult = 0;
	error = AVLIB_IsExecutableFormat(io, &dwResult);
	if( PR_SUCC(error))
	{
		ret_error = errOK;
		*pdwResult |= dwResult;
	}

	return ret_error;
}

