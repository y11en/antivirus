#include "enumctx.h"

tERROR __stdcall cbIniEnum(const wchar_t* sFileName,const tCHAR* sSection,const tCHAR* sValue,tDWORD dwFlag, EnumContext* pContext)
{
	return pContext->IniEnum(sFileName, sSection, sValue, dwFlag);
}

tERROR EnumContext::IniEnum(const wchar_t* sFileName,const tCHAR* sSection,const tCHAR* sValue,tDWORD dwFlag)
{
	tBOOL		bCommit=cFALSE;
	tERROR		error;
	tERROR		fRet;
	hINIFILE	hIni;
	tCHAR*		chSection;
	tCHAR*		chValName;
	tCHAR*		chData;
	cPrStrW 	wsData;
	tDWORD		dDataLen=0;
	tDWORD		dValCount=0;
	tDWORD		nSection,nValue;
	tERROR		OpRet=errOK;
	cPrStrW 	sFullFileName;
	const wchar_t* pFullFileName=NULL;

	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\tParsing ini <%S>",sFileName));

	if (_IfFileExist(sFileName,(int*)&nSection, true)!=warnOBJECT_FALSE)
	{
		sFullFileName = m_sFullFilePathName;
		pFullFileName = sFileName;
	}
	else
		pFullFileName = sFileName;

	m_SendData.m_ObjType=OBJECT_INI_TYPE;
	m_sFullFilePathName[0]=0; // ????
	m_SendData.m_sRoot=const_cast<wchar_t*>(pFullFileName);
	if (pFullFileName && !wcsncmp(pFullFileName, L"\\\\?\\", 4))
		m_SendData.m_sRoot=const_cast<wchar_t*>(pFullFileName+4);
	cPrStrW wsSection, wsValue;
	wsSection = sSection;
	wsValue = sValue;
	m_SendData.m_sSection = wsSection;
	m_SendData.m_sValue = wsValue;

	PR_TRACE((m_pStartUpEnum,prtSPAM,"startupenum2\tParsing ini <%S>",pFullFileName));

	error = m_pStartUpEnum->sysCreateObject((hOBJECT*)&hIni, IID_INIFILE, PID_INIFILE);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot create INI object for <%S> file, %terr",pFullFileName,error));
		return error;
	}
	cAutoObj<cIniFile> _ini(hIni);

	error = hIni->set_pgOBJECT_FULL_NAME((tPTR)pFullFileName,0,cCP_UNICODE);
	if (PR_SUCC(error))
	{
		// locked read
		cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
		error = hIni->sysCreateObjectDone();
	}
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot done INI object for <%S> file, %terr",pFullFileName,error));
		return error;
	}

	nSection=0;
	while ((error=hIni->EnumSections(nSection,&chSection,&dValCount))==errOK)
	{
		if (MatchWithPatternCh(chSection,sSection,false,0))
		{
			for (nValue=0;nValue<dValCount;nValue++)
			{
				error=hIni->EnumValues(nSection,nValue,&chValName,&chData);
				if (error==errEND_OF_THE_LIST) 
				{
					error=errOK;
					break;
				}
				if (error!=errOK) 
					continue;
				if (MatchWithPatternCh(chValName,sValue,false,0))
				{
					wsData = chData;
					fRet=IfFileExist(wsData);
					if ((m_dwFlags&CURE_ACTION_ACTIVE) && fRet==warnOBJECT_INFECTED)
					{
						OpRet=errOK;
						m_SendData.m_dAction=GetActionType(m_dwFlags,m_sDefaultValue,m_sOriginalString);
						switch(m_SendData.m_dAction) 
						{
						case CutData:
							if (IsNewStringEmpty(&m_sOriginalString))
								OpRet=hIni->DelValueByIndex(nSection,nValue);
							else
								OpRet=hIni->SetValueByIndex(nSection,nValue,cPrStrA(m_sOriginalString));
							break;
						case RestDef:
							OpRet=hIni->SetValueByIndex(nSection,nValue,cPrStrA(m_sDefaultValue));
							break;
						case DelData:
							OpRet=hIni->DelValueByIndex(nSection,nValue);
							break;
						default:
							OpRet=errNOT_FOUND;
						}
						if (PR_FAIL(OpRet)) 
						{
							PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<IniEnum> error restore %x.",OpRet));
							OpRet = SendError(pFullFileName, cPrStrW(chSection), cPrStrW(chValName), OBJECT_INI_TYPE, OpRet);
							if ((OpRet==ERROR_CANCELLED)||(OpRet==errOPERATION_CANCELED)) 
								return OpRet;
						}
						else 
						{
							cPrStrW curSection(chSection);
							cPrStrW curValue(chValName);
							m_SendData.m_sSection = curSection;
							m_SendData.m_sValue = curValue;
							OpRet=SendFileFound();
							m_SendData.m_sSection = wsSection;
							m_SendData.m_sValue = wsValue;
							bCommit=cTRUE;
							nValue--;
							dValCount--;
						}
					}
					
				}
			}
		}
		nSection++;
	}
	if (bCommit) 
	{
		{
			cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
			OpRet=hIni->Commit();
		}
		if (PR_FAIL(OpRet)) 
		{
			OpRet = SendError(pFullFileName, cPrStrW(chSection), cPrStrW(chValName), OBJECT_INI_TYPE, OpRet);
			if ((OpRet==ERROR_CANCELLED)||(OpRet==errOPERATION_CANCELED)) 
				return OpRet;
		}
	}
	return error;
}

tBOOL EnumContext::IsNewStringEmpty(cPrStrW* m_sOriginalString)
{
	int i=0;
//INT3;
	if (**m_sOriginalString==0) return cTRUE;
	while (((*m_sOriginalString)[i]==' ') || ((*m_sOriginalString)[i]=='\t')) i++;
	if ((*m_sOriginalString)[i]==0) return cTRUE;
	return cFALSE;
}