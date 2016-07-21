#include "Initialization.h"
#include "EnumFunc.h"
#include "ParsLineForFileNames.h"

#include <Prague/prague.h>
#include <Prague/iface/i_inifile.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_inifile.h>

#include <AV/iface/i_startupenum2.h>
#include <AV/structs/s_startupenum2.h>

extern bool MatchWithPatternCh(const char* String, const char* Pattern, bool CaseSensetivity, int chStopChar);


tERROR EnumContext::SendFileFound()
{
	tERROR errtmp;
	cStringObj strtmp;
	const wchar_t* pRoot = m_SendData.m_sRoot;

	tDWORD dwSize = sizeof(ACTION_MESSAGE_DATA);
	if (m_sFullFilePathName && !wcsncmp(m_sFullFilePathName, L"\\\\?\\", 4))
		m_SendData.m_sFilePath = m_sFullFilePathName + 4;
	else
		m_SendData.m_sFilePath = m_sFullFilePathName;

	if (m_SendData.m_ObjType==OBJECT_REGISTRY_DATA_TYPE && m_SendData.m_sRoot && !wcsncmp(m_SendData.m_sRoot, L"\\{", 2))
	{
		strtmp = L"HKCR";
		strtmp += m_SendData.m_sRoot;
		m_SendData.m_sRoot = (wchar_t*) strtmp.data();
	}
	
	errtmp = m_pSendToObj->sysSendMsg(pmc_STARTUPENUM2,mc_FILE_FOUNDED,NULL,&m_SendData,&dwSize);
	m_SendData.m_sRoot = const_cast<wchar_t*>(pRoot);

	return errtmp;
}

tERROR EnumContext::SendError(const wchar_t* pFileName, const wchar_t* pSection, const wchar_t* pValue, STURTUP_OBJ_TYPES nStartupObjType, tERROR error)
{
	ERR_MESSAGE_DATA sMess;
	tDWORD dwSize = sizeof(sMess);
	sMess.m_error    = error;
	sMess.m_ObjType  = nStartupObjType;
	sMess.m_sPath    = const_cast<wchar_t*>(pFileName);
	sMess.m_sSection = const_cast<wchar_t*>(pSection);
	sMess.m_sValue   = const_cast<wchar_t*>(pValue);
	return m_pSendToObj->sysSendMsg(pmc_STARTUPENUM2,mc_ERROR_OPERATION,NULL,&sMess,&dwSize);
}


#define MAX_SIZE_OF_BUFFER_PIF		65
#define MAX_SIZE_OF_PIF				0xe6

tERROR EnumContext::ParsDLink(const wchar_t* sFileName)
{
	tERROR		error;
	tERROR		fRet;
	hIO			hIo;
	tQWORD		qsize=0;
	tDWORD		size=0;
	tDWORD		dRealRead=0;
	char        buff[MAX_SIZE_OF_BUFFER_PIF+1];
	
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\tParsing DOS link file <%S>",sFileName));
	error = m_pStartUpEnum->sysCreateObject((hOBJECT*)&hIo, IID_IO, PID_NATIVE_FIO);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot create IO object for <%S> file, %terr",sFileName,error));
		return error;
	}
	hIo->set_pgOBJECT_FULL_NAME((tPTR)sFileName,0,cCP_UNICODE);
	error = hIo->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot done IO object for <%S> file, %terr",sFileName,error));
		return error;
	}
	error=CALL_IO_GetSize(hIo,&qsize,IO_SIZE_TYPE_EXPLICIT);
	if (PR_FAIL(error))
		goto errret;
	size=(tDWORD)qsize;	
	if (size<MAX_SIZE_OF_PIF)
		goto errret;
	error=CALL_IO_SeekRead(hIo,&dRealRead,0x24,buff,sizeof(buff)-1);
	hIo->sysCloseObject();
	if(PR_SUCC(error))
	{
		cPrStrW		str;
		buff[dRealRead] = 0; // ensure zero-terminated
		str = buff;
		fRet=IfFileExist(str);
		if ((fRet==warnOBJECT_FOUND)||(fRet==warnOBJECT_INFECTED))
			error=fRet;
	}
	return error;
//	if ((fRet==warnOBJECT_FOUND)||(fRet==warnOBJECT_INFECTED))
//	{
//		if(PR_SUCC(error=CALL_IO_SeekRead(hIo,&dRealRead,0xa5,buff,sizeof(buff)-1)))
//		{
//			buff[dRealRead] = 0; // ensure zero-terminated
//			str = buff;
//			error=fRet;
//			fRet=IfFileExist(str);
//			if (fRet==warnOBJECT_INFECTED) 
//				error=warnOBJECT_INFECTED;
//			return error;
//		}
//		else
//		{
//			error=fRet;
//		}
//	}

errret:
	hIo->sysCloseObject();
	return error;
}

tDWORD GetLine(tCHAR* ptr, tCHAR** line, tCHAR* EndChar, tBOOL* bToMiss)
{
	tDWORD i=0;
	*line=0;
	
	if (!ptr)
	{
		if (bToMiss)
			*bToMiss=cTRUE;
		return 0;
	}
	if ((*ptr=='#') && (*(ptr+1)=='!'))
	{
		if (MatchWithPatternCh(ptr+2,"*/perl*",false,0x0d))
		{
			*bToMiss=cTRUE;
			return 0;
		}
	}
	for(;*ptr;ptr++)
	{
		i++;
		if (((*ptr=='p') || (*ptr=='P')) && bToMiss)
		{
			if (!_strnicmp(ptr,"perl",4))
			{
				if (MatchWithPatternCh(ptr+4,"*%0*",false,0x0d))
				{
					*bToMiss=cTRUE;
					return 0;
				}
			}
		}
		if ((*ptr==0x0a)||(*ptr==0x0d))
		{
			if (EndChar) *EndChar=*ptr;
			*ptr=0;
			*line=ptr+1;
			return i;
		}
	}
	return i;
}

tERROR EnumContext::EnumByMaskToCheck(const wchar_t* sFileName,const tCHAR* m_sSection,const tCHAR* m_sValue,tDWORD dwFlag, EnumFuncPtr pEnumFuncPtr)
{
	tERROR		error=warnOBJECT_FALSE;
	bool		bMask=false,bNeedFree=true;
	cPrStrW     sString; // to make variable copy of filename
	cPrStrW     sFullName;
	wchar_t		chSimb=0;
	int			chDrive;

	if (!sFileName || !*sFileName) 
		return errPARAMETER_INVALID;

	// check for "any drive" mask
	if ((sFileName[0]==L'*')&&(sFileName[1]==L':'))
	{
		if (sFileName[2] == 0) 
			return error;
		bMask=true;
		chDrive='c';
		chSimb=sFileName[3];
		sString = sFileName; // make a copy of string to modify drive letter
	}

	do
	{
		if (bMask)
		{
			wchar_t* pString = sString;
			pString[0] = chDrive;
			pString[3]=0;
			chDrive++;
			if (pfGetDriveType(pString)!=DRIVE_FIXED)
				continue;
			pString[3]=chSimb;
			sFileName = sString; // point parameter to copy
		}
		error=IfFileExist(sFileName, true);
		if (error==errOPERATION_CANCELED) 
			return error;
		if (error!=warnOBJECT_FOUND)
		{
			if (bMask) 
				continue;
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumByMaskToCheck> <%S> file does not exist",sFileName));
			return error;
		}
		sFullName = m_sFullFilePathName;
		m_sFullFilePathName[0]=0;
		do {
			error = pEnumFuncPtr(sFullName,m_sSection,m_sValue,dwFlag,this);
		} while (PR_SUCC(error) && NeedRescan());
		if (error==errOPERATION_CANCELED) 
			return error;
	}
	while (chDrive<='z' && bMask);
	error=warnOBJECT_FOUND;
	return error;
}
