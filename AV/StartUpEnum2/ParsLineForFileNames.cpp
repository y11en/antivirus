#include "defines.h"
#include "EnumFunc.h"
#include "Initialization.h"
#include "ParsLineForFileNames.h"
#include "regenum.h"

#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_inifile.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <AV/iface/i_startupenum2.h>
#include <AV/structs/s_startupenum2.h>

#define EXTENSION_CHECK_COUNT	6

extern "C" DWORD g_TotalInput = 0;
extern "C" DWORD g_DupInput = 0;
extern "C" DWORD g_TotalParse = 0;
extern "C" DWORD g_DupParse = 0;
extern "C" DWORD g_MissPars = 0;
extern "C" DWORD g_DupMiss = 0;
extern "C" DWORD g_TotalOutput = 0;
extern "C" DWORD g_DupOutput = 0;
extern "C" DWORD g_TotalGetFileAttr = 0;

long __stdcall	RegEnumCallback(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	EnumContext* pContext=(EnumContext*)pEnumCallbackContext;
	return pContext->iRegEnumCallback(pEnumCallbackContext, hKey, sPath, sName, pValue, dwSize, dwType);
}

long EnumContext::iRegEnumCallback(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	LONG		OpRet=ERROR_SUCCESS;
	long		Ret=ERROR_REGOP_TRUE;
	wchar_t*	pData=(wchar_t*)pValue;
	wchar_t*	pNewMultiSZ=NULL;
	DWORD		dNewMultiSZ=0;
	int			iChanged=1;
	DWORD		dCount=dwSize/2;

	wchar_t*	pFixStrBuffer=NULL;
//INT3;

	if (dwSize<2) return ERROR_REGOP_TRUE;

	m_SendData.m_sRoot=const_cast<wchar_t*>(sPath);
	m_SendData.m_sSection=NULL;
	m_SendData.m_sValue=const_cast<wchar_t*>(sName);
	if ((m_dwFlags&FLAG_ENUM_VALUES)|(m_dwFlags&FLAG_ENUM_KEYS)) 
	{
//		if (!(m_dwFlags & CURE_ACTION_ACTIVE))
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> Parsing <%S> key: <%S>.",sPath,sName));
		if (m_dwFlags&FLAG_ENUM_VALUES) m_SendData.m_ObjType=OBJECT_REGISTRY_VALUE_TYPE;
		if (m_dwFlags&FLAG_ENUM_KEYS) m_SendData.m_ObjType=OBJECT_REGISTRY_KEY_TYPE;
		OpRet=IfFileExist(sName);
	}
	else
	{
//		if (!(m_dwFlags & CURE_ACTION_ACTIVE))
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> Parsing key 0x%x key <%S> value: <%S>",
				hKey, sPath, sName));

		m_SendData.m_ObjType=OBJECT_REGISTRY_DATA_TYPE;
		if ((dwType==REG_SZ)|(dwType==REG_EXPAND_SZ))
		{
			if (pData[dCount-1] != 0)
			{
				// нет завершающего нуля в строке, чтобы не переехать память - добавляем его
				PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\t<iRegEnumCallback> Invalid value format: key 0x%x subkey <%S> value: <%S>",
					hKey, sPath, sName));

				dCount++;
				m_pStartUpEnum->heapAlloc( (tPTR*)&pFixStrBuffer, dCount*sizeof(wchar_t) );
				memcpy(pFixStrBuffer, pData, dwSize);
				pFixStrBuffer[dCount-1] = 0;
				pData = pFixStrBuffer;
				dwSize = dCount*2;
			}
			OpRet=IfFileExist(pData);
		}
		if (dwType==REG_MULTI_SZ)
		{
			DWORD dwAddCount = 0;
			if (pData[dCount-1] != 0)
				dwAddCount++;
			if (pData[dCount-2] != 0)
				dwAddCount++;
			if (dwAddCount > 0)
			{
				// нет завершающих нулей в мультистроке, чтобы не переехать память - добавляем их
				PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\t<iRegEnumCallback> Invalid value format: key 0x%x subkey <%S> value: <%S>",
					hKey, sPath, sName));

				dCount += dwAddCount;
				m_pStartUpEnum->heapAlloc( (tPTR*)&pFixStrBuffer, dCount*sizeof(wchar_t) );
				memcpy(pFixStrBuffer, pData, dwSize);
				pFixStrBuffer[dCount-1] = 0;
				pFixStrBuffer[dCount-2] = 0;
				pData = pFixStrBuffer;
				dwSize = dCount*2;
			}

			m_pStartUpEnum->heapAlloc((tPTR*)&pNewMultiSZ,dwSize);
			if (pNewMultiSZ)
			{
				wchar_t* nP=pNewMultiSZ;
				wchar_t* nD=pData;
				while ((DWORD)(nD-pData)<dCount)
				{
					iChanged=1;
					long SubRet;
					SubRet=IfFileExist(nD);
					if (SubRet==warnOBJECT_INFECTED) 
					{
						OpRet=warnOBJECT_INFECTED;
						if (m_sOriginalString) 
						{
							if (*m_sOriginalString) 
								wcscpy(nP,m_sOriginalString);
							else 
								iChanged=0;
						}
						else 
						{
							wcscpy(nP,nD);
						}
					}
					else 
					{
						wcscpy(nP,nD);
					}
					if (iChanged)
					{
						while (*nP!=0) 
						{
							nP++;
							dNewMultiSZ++;
						}
						nP++;
						dNewMultiSZ++;
					}
					while (*nD!=0) nD++;
					nD++;
					if (*nD==0) break;
				}
				*nP=0;
				dNewMultiSZ++;
				if (nP==pNewMultiSZ)
				{
					*(nP+1)=0;
					dNewMultiSZ++;
				}
			}
		}
	}
	if (m_dwFlags&CURE_ACTION_ACTIVE)
	{
		if (OpRet==warnOBJECT_INFECTED)
		{
//INT3;
			wchar_t* sToCopy=m_sDefaultValue;
			OpRet=ERROR_SUCCESS;
			ActionType Action=GetActionType(m_dwFlags,sToCopy,m_sOriginalString);
			m_SendData.m_dAction=Action;
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> use %d action.",Action));
			switch(Action) 
			{
			case CutData:
				sToCopy=m_sOriginalString;
			case RestDef:
				if (m_dwFlags&FLAG_ENUM_VALUES)
				{
					OpRet=m_cRegEnumCtx->pfRegDeleteValue(hKey,sName);
					Ret=ERROR_REGOP_MISS_VALUE;
				}
				else 
				{
					if (dwType==REG_MULTI_SZ) 
					{
						OpRet=m_cRegEnumCtx->pfRegSetValueEx(hKey,sName,dwType,(LPBYTE)pNewMultiSZ,dNewMultiSZ*2);
						PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> restoring MULTI_SZ value '%S' err=%d.", pNewMultiSZ, OpRet));
					}
					else 
					{
						//if ((REG_SZ==dwType) && (wcsstr(sToCopy,L"%\\")))
						//	dwType=REG_EXPAND_SZ;
						OpRet=m_cRegEnumCtx->pfRegSetValueEx(hKey,sName,dwType,(LPBYTE)sToCopy,(wcslen(sToCopy)+1)*2);
						PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> restoring value '%S' err=%d.", sToCopy, OpRet));
					}
				}
				if (OpRet==ERROR_SUCCESS) 
				{
					if (errOPERATION_CANCELED == SendFileFound()) 
						Ret=ERROR_REGOP_CANCELED;
					OpRet=ERROR_SUCCESS;
				}
				else
					PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> error restoring value err=%d.", OpRet));
				break;
			case DelData:
				OpRet=m_cRegEnumCtx->pfRegDeleteValue(hKey,sName);
				Ret=ERROR_REGOP_MISS_VALUE;
				
				if (OpRet==ERROR_SUCCESS) 
				{
					PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> value deleted."));
					if (errOPERATION_CANCELED == SendFileFound()) 
						Ret=ERROR_REGOP_CANCELED;
				}
				else
					PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> error delete value '%S' result 0x%x (hkey 0x%x).",
						sName, OpRet, hKey));
				break;
			case DelKey:
				Ret=ERROR_REGOP_DEL_KEY;
				break;
			case DelThisKey:
				Ret=ERROR_REGOP_DEL_THISKEY;
				break;
			default:
				OpRet=errNOT_FOUND;
			}
			if ((OpRet!=ERROR_SUCCESS)&&(OpRet!=errOPERATION_CANCELED))
			{
				PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<iRegEnumCallback> error restore %x.",OpRet));
				OpRet = SendError(sPath, NULL, sName, OBJECT_REGISTRY_TYPE, OpRet);
				if (OpRet==errOPERATION_CANCELED) 
					Ret=ERROR_REGOP_CANCELED;
			}
			else
				InterlockedIncrement(&m_pStartUpEnum->m_nModifyCounter);
			m_dwFlags |= FLAG_REG_DETECTED;
		}
	}
	else
	{
		if (NeedRescan()) 
			Ret=ERROR_REGOP_RECOUNT_KEY;
	}
	if (pFixStrBuffer)
		m_pStartUpEnum->heapFree((tPTR)pFixStrBuffer);
	if (pNewMultiSZ) 
		m_pStartUpEnum->heapFree((tPTR)pNewMultiSZ);
	if (OpRet==errOPERATION_CANCELED) 
		Ret=ERROR_REGOP_CANCELED;
	return Ret;
}

long __stdcall RegEnumCallbackErrMessage(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error)
{
	EnumContext* pContext = (EnumContext*)pEnumCallbackContext;
	return pContext->iRegEnumCallbackErrMessage(pEnumCallbackContext, sPath, sName, dError, Error);
}

long EnumContext::iRegEnumCallbackErrMessage(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error)
{
	pERR_MESSAGE_DATA	pMess=NULL;
	LONG				OpRet;
	if (Error)
	{
		OpRet = SendError(sPath, NULL, sName, OBJECT_REGISTRY_TYPE, dError);
		if (OpRet==errOPERATION_CANCELED) 
			return ERROR_REGOP_CANCELED;
	}
	else
	{
		m_SendData.m_sRoot=const_cast<wchar_t*>(sPath);
		m_SendData.m_sValue=const_cast<wchar_t*>(sName);
		OpRet=SendFileFound();
		if (OpRet==errOPERATION_CANCELED) 
			return ERROR_REGOP_CANCELED;
	}
	return ERROR_REGOP_TRUE;
}


long EnumContext::IfFileExist(const wchar_t* sString, bool bWithoutSending, bool bDontLowercase)
{
	tERROR error;
	wchar_t* ptr;
	error = intIfFileExist(sString, bWithoutSending, bDontLowercase);
	if (warnOBJECT_FALSE != error)
		return error;
	if (NULL == wcschr(sString, '^'))
		return error;
	cPrStrW cTempStr; cTempStr = sString;
	do
	{
		ptr = wcschr(cTempStr, '^');
		if (!ptr)
			break;
		wcscpy(ptr, ptr+1);
	} while (ptr);
	error = intIfFileExist(cTempStr, bWithoutSending, bDontLowercase);
	return error;
}

tERROR EnumContext::ScanCLSID(const wchar_t* wPoint)
{
	LONG	lRet=warnOBJECT_FALSE;
	tERROR	fRet=warnOBJECT_FALSE;
	wchar_t	chCLSID[40];
	if (errHASH_FOUND != Hash_AddItem_Quick(m_pHashClsid, wPoint, 38))
	{
		EnumContext ContextCopy(this);
		
		ContextCopy.m_sEnumRootObject = L"*\\Software\\Classes\\CLSID\\";
		if (!ContextCopy.m_sEnumRootObject.reserve_len(100))
			return ERROR_NOT_ENOUGH_MEMORY;
		wcsncat(ContextCopy.m_sEnumRootObject,wPoint,38);
		wcsncpy(chCLSID,wPoint,38);
		ContextCopy.m_sEnumRootObject[63]=0;
		chCLSID[38]=0;
		wcscat(ContextCopy.m_sEnumRootObject,L"\\*Server*");
		ContextCopy.m_dwFlags &= FLAG_CLEAR_ACTIONS_SENDING;
//		ContextCopy.m_dwFlags &= FLAG_CLEAR_ENUM_KEYS;
		ContextCopy.m_dwFlags |= FLAG_CUT_DATA;
		ContextCopy.m_SendData.m_sRoot=ContextCopy.m_sEnumRootObject;
		ContextCopy.m_SendData.m_sValue=NULL;
		ContextCopy.m_SendData.m_sSection=NULL;
		ContextCopy.m_SendData.m_ObjType=OBJECT_REGISTRY_DATA_TYPE;
		ContextCopy.m_bCLSIDScanning = true;
		m_cRegEnumCtx->CLSIDInit();
		lRet=m_cRegEnumCtx->CLSIDScan(chCLSID,(LPVOID)&ContextCopy,RegEnumCallback,RegEnumCallbackErrMessage);
//		lRet=m_cRegEnumCtx->RegEnumWW(ContextCopy.m_sEnumRootObject,L"*",FLAG_REG_WITH_SUBKEYS,(LPVOID)&ContextCopy,RegEnumCallback,RegEnumCallbackErrMessage);
		
		if (ContextCopy.m_dwFlags&FLAG_REG_DETECTED) 
		{
			if (m_bAdvancedDisinfection)
			{
				cStringObj name(ContextCopy.m_sEnumRootObject);
				tERROR error = m_pStartUpEnum->sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_LOCK_REGKEY, (hOBJECT)cAutoString(name), NULL, NULL);
				PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tNotify pm_ADVANCED_DISINFECTION_LOCK_REGKEY <%S>, result %terr", name.data(), error));
			}
			fRet=warnOBJECT_INFECTED;
		}
		else 
			fRet=warnOBJECT_FOUND;
	}
	else
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\tIfFileExist duplicate CLSID"));
	if (lRet==errOPERATION_CANCELED) 
		fRet=errOPERATION_CANCELED;
	return fRet;
}

long EnumContext::intIfFileExist(const wchar_t* sString, bool bWithoutSending, bool bDontLowercase)
{
	long	fRet=warnOBJECT_FALSE;
	long	SubRet=warnOBJECT_FOUND;
	BOOL	Found=FALSE;
	int		iCount;
	int		iPoint=0;
	pRESTORE_ORIGIN_STRING	fRest=NULL;
	tDWORD	fRestCount=0;
	tDWORD	SubLen;
//	cPrStrW sRoot;
	cPrStrW sStringLowercase;

	cAutoPrMemFree _free_rest(*m_pStartUpEnum, (void**)&fRest);

	if (!sString || !*sString)
		return warnOBJECT_FALSE;

	if (!bDontLowercase)
	{
		// make lowercase copy of the source string
		sStringLowercase = sString;
		_wcslwr(sStringLowercase);
		sString = sStringLowercase;
	}

	BYTE	cFound=0;
	if((m_dwFlags & CURE_ACTION_ACTIVE) && (!bWithoutSending) && m_pStartUpEnum->m_bHashKnownFilesExist)
	{
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<IfFileExist> Parsing string <%S> try to find short name <%S>",sString,(wchar_t*)m_sShortInfectedName));
		// fast filtering on cleanup
		if(m_sShortInfectedName && !wcsstr(sString, m_sShortInfectedName))
		{
//			fRet=warnOBJECT_FALSE;
//			if (!m_bCLSIDScanning)
//			{
//				tBOOL bOrigInited=cFALSE;
//				int Delta=0;
//				int OrigDelta=0;
//				wchar_t* wPoint=sString;
//				iPoint=0;
//				while ((wPoint=wcschr(wPoint, '{'))!=NULL)
//				{
//					if (wcslen(wPoint)<38)
//						break;
//					// CLSID processing
//					if (((wPoint)[0]=='{')&((wPoint)[9]=='-')&((wPoint)[14]=='-')&((wPoint)[19]=='-')&((wPoint)[24]=='-')&((wPoint)[37]=='}'))
//					{
//						Delta=38;
//						fRet=ScanCLSID(wPoint);
//						if (warnOBJECT_INFECTED==fRet)
//						{
//							if (!bOrigInited)
//							{
//								m_sOriginalString = sString;
//								bOrigInited=cTRUE;
//							}
//							if ((wPoint+Delta)[0]==' ') Delta++;
//							if (m_sOriginalString.getlen()>(tUINT)((wPoint-sString)+Delta-OrigDelta))
//								wcscpy(m_sOriginalString+(wPoint-sString)-OrigDelta,wPoint+Delta);
//							else
//								((wchar_t*)m_sOriginalString)[(wPoint-sString)-OrigDelta]=0;
//							OrigDelta=Delta;
//						}
//						if (errOPERATION_CANCELED==fRet)
//							break;
//						wPoint=wPoint+Delta;
//					}
//					else 
//						iPoint++;
//				}
//			}
//			return fRet;
			return warnOBJECT_FALSE;
		}
	}
//INT3;

	if ( !bWithoutSending && m_pHashInput )
	{
		if (m_dwFlags&CURE_ACTION_ACTIVE)
		{
			if (Hash_FindItem_Quick(m_pHashInput,sString,wcslen(sString))==errHASH_FOUND)
			{
				g_DupMiss++;
				return warnOBJECT_FALSE;
			}
		}
		else
		{
			if (Hash_AddItem_Quick(m_pHashInput,sString,wcslen(sString))==errHASH_FOUND)
			{
				g_DupInput++;
				PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<IfFileExist> Dublicate input <%S>",sString));
				return warnOBJECT_FALSE;
			}
		}
	}

	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<IfFileExist> Parsing string <%S>",sString));

//if (wcsstr(sString,L"test.exe")!=0)
//{
//	INT3;
//}
//
	if (m_dwFlags&CURE_ACTION_ACTIVE)
	{
		m_sOriginalString = sString;
		fRet=ParsEnviroment(sString,m_sFileName,&fRest,&fRestCount);
	}
	else
		fRet=ParsEnviroment(sString,m_sFileName);

	if ((fRet==ERROR_NOT_ENOUGH_MEMORY) || (!(wchar_t*)m_sFileName))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	iCount=wcslen(m_sFileName);
	while (iPoint<iCount)
	{
		g_TotalParse++;
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<IfFileExist> Parsing part of string <%S>",m_sFileName+iPoint));
		Found=FALSE;
		int Delta=0;
		if (iCount - iPoint>=38)
		{
			// CLSID processing
			if (((m_sFileName+iPoint)[0]=='{')&((m_sFileName+iPoint)[9]=='-')&((m_sFileName+iPoint)[14]=='-')&((m_sFileName+iPoint)[19]=='-')&((m_sFileName+iPoint)[24]=='-')&((m_sFileName+iPoint)[37]=='}'))
			{
				if (!m_bCLSIDScanning)
				{
					fRet=ScanCLSID(m_sFileName+iPoint);
					if (errOPERATION_CANCELED==fRet)
						break;
				}
				Found=TRUE;
				Delta=38;
			}
		}
		if (!Found) 
		{
			if (m_sFileName[iPoint] && m_sFileName[iPoint+1]==':' && m_sFileName[iPoint+2] != '\\')
				iPoint+=2;
			fRet=_IfFileExist(m_sFileName+iPoint,&Delta, bWithoutSending);
		}
		if (PR_FAIL(fRet)) 
			return fRet;
		if ((fRet==warnOBJECT_INFECTED)&&(m_dwFlags&CURE_ACTION_ACTIVE))
		{
			tDWORD	Start=0;
			tDWORD	End=0;
			SubRet=warnOBJECT_INFECTED;
			if ((m_sFileName+iPoint+Delta)[0]==' ') Delta++;
			wcscpy(m_sFileName+iPoint,m_sFileName+iPoint+Delta);
			if (fRestCount==0) 
			{
				if (m_sOriginalString.getlen()>(tUINT)(iPoint+Delta))
					wcscpy(m_sOriginalString+iPoint,m_sOriginalString+iPoint+Delta);
				else
					((wchar_t*)m_sOriginalString)[iPoint]=0;
			}
			else
			{
				SubLen=0;
				Start=iPoint;
				while (((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->nPoint<(tDWORD)iPoint)
				{
					Start=((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->oPoint+((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->oSize+(
						iPoint-((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->nPoint-((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->nSize);
					SubLen++;
					if (fRestCount<=SubLen) break;
				}
				End=iPoint+Delta;
				SubLen--;
				if (fRestCount<=SubLen) SubLen=fRestCount-1;
				while (((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->nPoint<(tDWORD)(iPoint+Delta))
				{
					End=((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->oPoint+((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->oSize+(
						iPoint+Delta-((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->nPoint-((pRESTORE_ORIGIN_STRING)(fRest+sizeof(RESTORE_ORIGIN_STRING)*SubLen))->nSize);
					SubLen++;
					if (fRestCount<=SubLen) break;
				}
				wcscpy(m_sOriginalString+Start,m_sOriginalString+End);
			}
			iCount=iCount-Delta;
		}
		else iPoint=iPoint+Delta;
		if (CALL_SYS_SendMsg(m_pStartUpEnum,pmc_PROCESSING,pm_PROCESSING_YIELD,0,0,0)==errOPERATION_CANCELED) 
		{
			fRet=errOPERATION_CANCELED;
			break;
		}
		if (fRet==warnOBJECT_FALSE)
			g_MissPars++;
	}
	if (SubRet==warnOBJECT_INFECTED) 
		fRet=warnOBJECT_INFECTED;
	else
	{
		if ( !bWithoutSending && m_pHashInput && m_dwFlags&CURE_ACTION_ACTIVE)
			Hash_AddItem_Quick(m_pHashInput,sString,wcslen(sString));
	}
	return fRet;
}

const wchar_t pszTerminatorB[] = L"%";
const wchar_t pszWhiteSpace[] = L" \t,/=-";

tERROR EnumContext::_IfFileExist(const wchar_t* ParsString,int* iIndex, bool bWithoutSending)
{
	cPrStrW     sParsString(ParsString);
	tERROR		fRet=warnOBJECT_FALSE;;
	wchar_t*    wCurPoint=sParsString;
	int			iCurLen=0;
	int			iDelta = 0;
	int         niIndex = 0;
	if (!ParsString) 
		return errPARAMETER_INVALID;
	if (iIndex)
		iDelta = niIndex = *iIndex;
	PR_TRACE((m_pStartUpEnum,prtSPAM,"startupenum2\t<_IfFileExist> Parsing string <%S>",ParsString));
	while ((wCurPoint[0]==L' ')|(wCurPoint[0]==L'\t')) 
	{
		wCurPoint++;
		niIndex++;
		iDelta++;
	}
	iCurLen=wcslen(wCurPoint);
	while (iCurLen>0)
	{
		 if ((wCurPoint[iCurLen-1]==L'\n')||(wCurPoint[iCurLen-1]==L'\r')||(wCurPoint[iCurLen-1]==L' ')) 
		 {
			 wCurPoint[iCurLen-1]=0;
			 iCurLen--;
		 }
		 else 
			 break;
	}
	if (iCurLen==0)
	{
		niIndex++;
		if (iIndex)
			*iIndex = niIndex;
		return warnOBJECT_FALSE;
	}
	if (wCurPoint[0]==L'\"') 
	{
		wCurPoint++;
		niIndex++;
		wchar_t* pEnd=wcschr(wCurPoint,'\"');
		int iLen;
		if (pEnd)
		{
			*pEnd = 0;
			iLen = pEnd - wCurPoint;
			niIndex++;
		}
		else
		{
			iLen = wcslen(wCurPoint);
		}
		niIndex += iLen;
		while (iLen)
		{
			if ((wCurPoint[iLen-1]==L' ')||(wCurPoint[iLen-1]==L'\t')) 
			{
				iLen--;
				wCurPoint[iLen]=L'\0';
			}
			else 
				break;
		}
		fRet=IOSFindFilePath(wCurPoint, bWithoutSending);
	}
	else 
	{
		int nLength = 1;
//		int nLength = 0;
		wchar_t *pEnd=wCurPoint;
		wchar_t	cReplace;
		if (wCurPoint!=NULL)
		{
			tERROR	fSRet=warnOBJECT_FALSE;
			int		SubIndex;
			while ((pEnd)&&((int)wcslen(wCurPoint)>nLength)) 
			{
				int step=wcscspn(wCurPoint+nLength/*pEnd*/,pszWhiteSpace);
				if (!step)
				{
					nLength++;
					continue;
				}
				nLength+=step;
//				nLength+=wcscspn(wCurPoint+nLength+1/*pEnd*/,pszWhiteSpace);
				pEnd = wCurPoint+nLength;
//				pEnd = wCurPoint+nLength+1;
				if (pEnd) 
				{
					cReplace=pEnd[0];
					pEnd[0]=0;
					fRet=IOSFindFilePath(wCurPoint, bWithoutSending);
					if (fRet==ERROR_NOT_ENOUGH_MEMORY)
						return fRet;
					nLength++;
					pEnd[0]=cReplace;
					if (fRet!=warnOBJECT_FALSE)
					{
						fSRet=fRet;
						SubIndex=niIndex+nLength;
					}
				}
			}
			if (fSRet!=warnOBJECT_FALSE)
			{
				fRet=fSRet;
				niIndex=SubIndex;
			}
		}
	}
	if (iDelta==niIndex) 
		niIndex=wcscspn(wCurPoint,pszWhiteSpace)+1;
	if (iIndex)
		*iIndex = niIndex;
	return fRet;
}

tERROR EnumContext::IOSFindFilePath(const wchar_t* pszFileName, bool bWithoutSending) 
{
	tERROR	fRet=warnOBJECT_FALSE;
	DWORD	len;
	DWORD	i;

	if (!pszFileName) 
		return FALSE;
	len=wcslen(pszFileName);
	if (len==0) 
		return FALSE;
	if (m_dwFlags&CURE_ACTION_ACTIVE)
	{
		if ( !bWithoutSending && m_pHashInput )
		{
			if (Hash_FindItem_Quick(m_pHashInput,pszFileName,len)==errHASH_FOUND)
			{
				g_DupMiss++;
				return warnOBJECT_FALSE;
			}
		}
	}
	else
	{
		if ( !bWithoutSending && m_pHashParse )
		{
			if (Hash_AddItem_Quick(m_pHashParse,pszFileName,len)==errHASH_FOUND)
			{
				PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<IOSFindFilePath> Dublicate parse <%S>",pszFileName));
				return warnOBJECT_FALSE;
			}
		}
	}
	if ((pszFileName[1]==0 && (*pszFileName==L'*' || *pszFileName==L'?')) || *pszFileName==0) return fRet;
	if ((fRet=_IOSFindFilePath(pszFileName,L"\0\0\0\0", bWithoutSending))==warnOBJECT_FALSE)
	{
		const wchar_t* pName = wcsrchr(pszFileName, '\\');
		if (!wcschr((pName ? pName : pszFileName), '.'))
		{
			wchar_t	chExt[EXTENSION_CHECK_COUNT][5]={
				L".exe",
				L".dll",
				L".bat",
				L".cmd",
				L".pif",
				L".com",
			};
			for (i=0;i<EXTENSION_CHECK_COUNT;i++)
			{
				fRet=_IOSFindFilePath(pszFileName,chExt[i], bWithoutSending);
				if (fRet!=warnOBJECT_FALSE) 
					break;
			}
		}
	}
	if (fRet!=warnOBJECT_INFECTED && m_pHashInput && m_dwFlags&CURE_ACTION_ACTIVE)
		Hash_AddItem_Quick(m_pHashInput,pszFileName,len);
	return fRet;
}

#if defined(_MSC_VER) && _MSC_VER >= 1400 
extern "C" errno_t __cdecl _wsearchenv_s_fixed(const wchar_t *fname,const wchar_t *env_var,wchar_t *path,size_t sz);
#endif

tERROR EnumContext::_IOSFindFilePath(const wchar_t* pszFileName,const wchar_t sExt[5], bool bWithoutSending) 
{
	tERROR		fRet=errPARAMETER_INVALID;
	wchar_t*	psExt=(wchar_t*)sExt;
	if (!pszFileName) 
		return warnOBJECT_FALSE;
	DWORD sLen=wcslen(pszFileName);
	if (!sLen)
		return warnOBJECT_FALSE;
	if (pszFileName[sLen-1]==L'.') 
		psExt=psExt+1;
	DWORD extLen=wcslen(psExt);
	int i;
	tBOOL bPipeName=cFALSE;

	bPipeName=IfPipeName((wchar_t*)pszFileName);
		
	m_sFileNameWithExt = pszFileName;
	if (!m_sFileNameWithExt.append(psExt))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	if (sLen>2)
	{
		tBOOL bFullPath=cFALSE;
		if (m_sFileNameWithExt[0]=='\\' && m_sFileNameWithExt[1]=='\\')
		{
			//INT3;
			tDWORD	Count=0,i=2;
			while (m_sFileNameWithExt[i]!=0)
			{
				if (m_sFileNameWithExt[i]=='\\')
				{
					if (Count)
					{
						bFullPath=cTRUE;
						break;
					}
					Count++;
				}
				i++;
			}
			if (!bFullPath)
				return warnOBJECT_FALSE;
		}  
		if (m_sFileNameWithExt[1]==':' && m_sFileNameWithExt[2]=='\\')
			bFullPath=cTRUE;
		if (bFullPath)
		{
			m_sFullFilePathName = m_sFileNameWithExt;
			return _IOSFindFile(bWithoutSending);
		}
	}

	wchar_t* sDirs[] = {
		m_pStartUpEnum->m_sWindowsDirectory,
		m_pStartUpEnum->m_sSystemDirectory,
		m_pStartUpEnum->m_sSystem32DriversDirectory,
		m_pStartUpEnum->m_sSystem32Directory,
		m_pCurrentWorkingDir,
		(m_sFileNameWithExt[0]=='\\' ? (wchar_t*)m_pStartUpEnum->m_sSystemDrive : (wchar_t*)NULL),
		(((g_bIsWin9x == cFALSE) && (bPipeName == cFALSE)) ? L"\\\\?\\" : (wchar_t*)NULL),
	};

	for (i=0; i<countof(sDirs); i++)
	{
		if (NULL == sDirs[i])
			continue;
		m_sFullFilePathName = sDirs[i];
		m_sFullFilePathName.append_path(m_sFileNameWithExt);
		fRet=_IOSFindFile(bWithoutSending);
		if (fRet!=warnOBJECT_FALSE) return fRet;
	}

	if (m_pStartUpEnum->m_nPathEnvLen)
	{
		tDWORD FullNameSize = max(MAX_PATH, m_pStartUpEnum->m_nPathEnvLen + m_sFileNameWithExt.allocated_len());
		if (!m_sFullFilePathName.reserve_len(FullNameSize))
			return ERROR_NOT_ENOUGH_MEMORY;
#if defined(_MSC_VER) && _MSC_VER >= 1400 
		_wsearchenv_s_fixed(m_sFileNameWithExt, L"PATH", m_sFullFilePathName, FullNameSize);
#else
		_wsearchenv(m_sFileNameWithExt, L"PATH", m_sFullFilePathName);
#endif
		if (m_sFullFilePathName.allocated_len() && *m_sFullFilePathName)
		{
			fRet=_IOSFindFile(bWithoutSending);
			if (fRet!=warnOBJECT_FALSE) return fRet;
		}
		m_sFullFilePathName = pszFileName;
	}
	if (bPipeName)
		return warnOBJECT_FALSE;
	else
		return _IOSFindFile(bWithoutSending);
}

tERROR EnumContext::_IOSFindFile(bool bWithoutSending) 
{
	tERROR	fRet=warnOBJECT_FALSE; //no file
	tERROR	error=errOK;
	wchar_t*	Ext=NULL;
	hash_t	CRC64;
	tBOOL	bMD5Created=0;
	
	if (!g_bIsWin9x)
	{
//		if (wcsncmp(m_sFullFilePathName,L"\\??\\",4)==0) 
//			m_sFullFilePathName[1] = '\\';

		//PR_TRACE((m_pStartUpEnum, prtIMPORTANT,"startupenum2\t<_IOSFindFile> before reparse name %S", (wchar_t*)m_sFullFilePathName));

		cStringObj strtmp( m_sFullFilePathName );
		strtmp.replace_all( L"\\??\\", fSTRING_COMPARE_SLASH_SENSITIVE, L"" );
		strtmp.replace_all( L"\\\\?\\", fSTRING_COMPARE_SLASH_SENSITIVE, L"" );
		strtmp.replace_all( L"\\\\", fSTRING_COMPARE_SLASH_SENSITIVE, L"\\" );
		
		cStringObj strtmp2;
		strtmp2.append( L"\\\\?\\" );
		strtmp2.append( strtmp );

		if (m_sFullFilePathName.reserve_len(strtmp2.length()))
		{
			wcscpy((wchar_t*) m_sFullFilePathName, (PWCHAR) strtmp2.data());
		//	PR_TRACE((m_pStartUpEnum, prtIMPORTANT,"startupenum2\t<_IOSFindFile> reparsed name %S", (wchar_t*)m_sFullFilePathName));
		}
	}

	g_TotalGetFileAttr++;
	if (!(wchar_t*)m_sFullFilePathName)
		return warnOBJECT_FALSE;
	if ((m_dwFlags&CURE_ACTION_ACTIVE) && m_wFileNames) 
	{
		cStringObj wFileNames(m_wFileNames);
		const wchar_t* sFullFilePathName = m_sFullFilePathName;
		const wchar_t* sFileNames = wFileNames.data();
		if (wcsncmp(sFileNames, L"\\\\?\\",4) != 0)
		{
			if (wcsncmp(sFullFilePathName,L"\\\\?\\",4)==0)  // skip unicode prefix
				sFullFilePathName+=4;
		}
		//PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> comparing my '%S' with given '%S'.", sFullFilePathName, sFileNames));
		if (m_wFileNames->CompareBuff(STR_RANGE(0,cSTRING_WHOLE_LENGTH), (tPTR)sFullFilePathName,0,cCP_UNICODE,fSTRING_COMPARE_CASE_INSENSITIVE)==cSTRING_COMP_EQ)
		{
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> prev detected."));
			return warnOBJECT_INFECTED; //detected
		}
	}
	DWORD dAttr=pfGetFileAttributes(m_sFullFilePathName);
	if ((dAttr==-1) || (dAttr&FILE_ATTRIBUTE_DIRECTORY))
	{
		m_sFullFilePathName[0]=0;
		return warnOBJECT_FALSE;
	}
	ConvertToLongName(m_sFullFilePathName);
	if (bWithoutSending)
		return warnOBJECT_FOUND;
	fRet=warnOBJECT_FOUND; //file

	Ext=wcsrchr(m_sFullFilePathName,L'.');
	if (Ext)
	{
		if (!_wcsicmp(Ext,L".lnk")||!_wcsicmp(Ext,L".pif")||!_wcsicmp(Ext,L".bat")) 
		{
			tDWORD	Found=FALSE;
			EnumContext*	PrevContext=m_pPrevContext;
			while (PrevContext!=NULL)
			{
				if (_wcsicmp(PrevContext->m_sFullFilePathName,m_sFullFilePathName)==0)
				{
					Found=TRUE;
					break;
				}
				PrevContext=PrevContext->m_pPrevContext;
			}
			if (!Found)
			{
				EnumContext ContextCopy(this);
				ContextCopy.m_sEnumRootObject=m_sFullFilePathName; 
				ContextCopy.m_SendData.m_sRoot=m_sFullFilePathName;
				ContextCopy.m_SendData.m_sValue=NULL;
				ContextCopy.m_SendData.m_sSection=NULL;
				if (_wcsicmp(Ext,L".lnk")==0) 
				{
					ContextCopy.m_SendData.m_ObjType=OBJECT_LINK_FILE_TYPE;
					fRet=ContextCopy.ParsWLink(m_sFullFilePathName);
					if ((fRet==warnOBJECT_INFECTED)&&(ContextCopy.m_dwFlags&CURE_ACTION_ACTIVE))
					{
						if (pFileToDel)
						{
							tBOOL	bFound=cFALSE;
							DWORD i;
							DWORD Count=pFileToDel->Count;
							for (i=0;i<Count;i++)
							{
								if (!wcscmp((wchar_t*)m_sFullFilePathName,(wchar_t*)pFileToDel->cProfileName[i]))
								{
									bFound=cTRUE;
									break;
								}
							}
							if (!bFound)
							{
								PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> <%S> link file added to queue to delete.",(wchar_t*)m_sFullFilePathName));
								m_pStartUpEnum->heapRealloc((tPTR*)&pFileToDel,pFileToDel,sizeof(UNLOAD_PROFILE_NAMES)+pFileToDel->Count*sizeof(DWORD));
								if (pFileToDel)
								{
									m_pStartUpEnum->heapAlloc((tPTR*)&(pFileToDel->cProfileName[pFileToDel->Count]),(m_sFullFilePathName.getlen()+1)*sizeof(wchar_t));
									if (pFileToDel->cProfileName[pFileToDel->Count])
									{
										wcscpy((wchar_t*)pFileToDel->cProfileName[pFileToDel->Count],(wchar_t*)m_sFullFilePathName);
										pFileToDel->Count++;
									}
								}
							}
							else
								PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> <%S> link file already added earlier.",(wchar_t*)m_sFullFilePathName));
						}
//						tERROR	fRetS;
//						ContextCopy.m_SendData.m_dAction=FileToDel;
//						fRetS=ContextCopy.SendFileFound();
//						if ((fRetS==ERROR_CANCELLED)||(fRetS==errOPERATION_CANCELED)) 
//							return fRet;
					}
				}
				if (_wcsicmp(Ext,L".pif")==0) 
				{
					ContextCopy.m_SendData.m_ObjType=OBJECT_LINK_FILE_TYPE;
					fRet=ContextCopy.ParsDLink(m_sFullFilePathName);
					if ((fRet==warnOBJECT_INFECTED)&&(ContextCopy.m_dwFlags&CURE_ACTION_ACTIVE))
					{
						if (pFileToDel)
						{
							tBOOL	bFound=cFALSE;
							DWORD i;
							DWORD Count=pFileToDel->Count;
							for (i=0;i<Count;i++)
							{
								if (!wcscmp((wchar_t*)m_sFullFilePathName,(wchar_t*)pFileToDel->cProfileName[i]))
								{
									bFound=cTRUE;
									break;
								}
							}
							if (!bFound)
							{
								PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> <%S> link file added to queue to delete.",(wchar_t*)m_sFullFilePathName));
								m_pStartUpEnum->heapRealloc((tPTR*)&pFileToDel,pFileToDel,sizeof(UNLOAD_PROFILE_NAMES)+pFileToDel->Count*sizeof(DWORD));
								if (pFileToDel)
								{
									m_pStartUpEnum->heapAlloc((tPTR*)&(pFileToDel->cProfileName[pFileToDel->Count]),(m_sFullFilePathName.getlen()+1)*sizeof(wchar_t));
									if (pFileToDel->cProfileName[pFileToDel->Count])
									{
										wcscpy((wchar_t*)pFileToDel->cProfileName[pFileToDel->Count],(wchar_t*)m_sFullFilePathName);
										pFileToDel->Count++;
									}
								}
							}
							else
								PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> <%S> link file already added earlier.",(wchar_t*)m_sFullFilePathName));
						}
//						tERROR	fRetS;
//						ContextCopy.m_SendData.m_dAction=FileToDel;
//						fRetS=ContextCopy.SendFileFound();
//						if ((fRetS==ERROR_CANCELLED)|(fRetS==errOPERATION_CANCELED)) 
//							return fRetS;
					}
				}
				if (_wcsicmp(Ext,L".bat")==0) 
				{
					ContextCopy.m_SendData.m_ObjType=OBJECT_BAT_TYPE;
					do {
						fRet=ContextCopy.BatEnum(m_sFullFilePathName,NULL,NULL,0);
					} while(PR_SUCC(fRet) && ContextCopy.NeedRescan());
				}
				if ((fRet==errOPERATION_CANCELED) || (fRet==ERROR_NOT_ENOUGH_MEMORY)) 
					return fRet;
			}
		}
	}

	error=Hash_Calc_Quick(&CRC64, m_sFullFilePathName,wcslen(m_sFullFilePathName));
	if (PR_SUCC( error ))
	{
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> add to m_pHashKnownFiles <%S>",(wchar_t*)m_sFullFilePathName));
		cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
		_Hash_AddItem(*m_pStartUpEnum,m_pStartUpEnum->m_pHashKnownFiles,CRC64.h1,CRC64.h2,!!m_pPrevContext);
		bMD5Created=1;
	}
	else
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> error adding to m_pHashKnownFiles <%S> = %d,err",(wchar_t*)m_sFullFilePathName,error));
//INT3;
	if ((m_dwFlags&CURE_ACTION_ACTIVE) && m_wFileNames) 
	{
		cStringObj wFileNames(m_wFileNames);
		const wchar_t* sFullFilePathName = m_sFullFilePathName;
		const wchar_t* sFileNames = wFileNames.data();
		if (wcsncmp(sFileNames, L"\\\\?\\",4) != 0)
		{
			if (wcsncmp(sFullFilePathName,L"\\\\?\\",4)==0)  // skip unicode prefix
				sFullFilePathName+=4;
		}
		//PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> comparing my '%S' with given '%S'.", sFullFilePathName, sFileNames));
		if (m_wFileNames->CompareBuff(STR_RANGE(0,cSTRING_WHOLE_LENGTH), (tPTR)sFullFilePathName,0,cCP_UNICODE,fSTRING_COMPARE_CASE_INSENSITIVE)==cSTRING_COMP_EQ)
		{
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> detected."));
			fRet=warnOBJECT_INFECTED; //detected
		}

//		if (m_wFileNames->CompareBuff(STR_RANGE(0,cSTRING_WHOLE_LENGTH),(tPTR)m_sFullFilePathName,0,cCP_UNICODE,fSTRING_COMPARE_CASE_INSENSITIVE)==cSTRING_COMP_EQ)
//		{
//			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> detected."));
//			fRet=warnOBJECT_INFECTED; //detected
//		}
	}
	else 
	{
		tBOOL cFound=0;
		if (bMD5Created)
		{
			g_TotalOutput++;
			if (_Hash_AddItem(*m_pStartUpEnum,m_pHashClsid,CRC64.h1,CRC64.h2,!!m_pPrevContext)==errHASH_FOUND)
			{
				g_DupOutput++;
				PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> Dublicate output <%S>", (wchar_t*)m_sFullFilePathName));
				return fRet;
			}
		}
		fRet=SendFileFound();
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<_IOSFindFile> Found file <%S>",(wchar_t*)m_sFullFilePathName));
	}
	return fRet;
}

void EnumContext::ConvertToLongName(cPrStrW& sFilePath)
{
	int  i,j,shortSize,point;
	bool bWasTilda = false;
	
	if (sFilePath==NULL || *sFilePath==0) 
		return;
	// ??? network paths - skipped
	if(sFilePath[1]!=L':' || sFilePath[2]!=L'\\') 
		return;
	if (NULL == wcschr(sFilePath, '~'))
		return;
	if (!m_sLongNameConvert.reserve_len(4))
		return;
	m_sLongNameConvert[0]=sFilePath[0];
	m_sLongNameConvert[1]=':';
	m_sLongNameConvert[2]='\\';
	m_sLongNameConvert[3]=0;
	shortSize=wcslen(sFilePath);
	for(i=3,j=3,point=3;i<=shortSize;i++)
	{
		wchar_t last_char = sFilePath[i];
		if(last_char=='\\' || last_char==0)
		{
			HANDLE hFind = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW fd;
			wchar_t* name;
			sFilePath[i] = 0;
			if (!m_sLongNameConvert.reserve_len(point+MAX_PATH+2))
				return;
			if (bWasTilda && pfFindFirstFile(sFilePath,&hFind, &fd))
			{
				_wcslwr(fd.cFileName);
				name = fd.cFileName;
				FindClose(hFind);
			}
			else
			{
				name = sFilePath+j;
			}
			bWasTilda = false;
			wcscpy(m_sLongNameConvert+point, name);
			point += wcslen(name);
			m_sLongNameConvert[point] = last_char;
			point++;
			sFilePath[i] = last_char;
			j=i+1;
		}
		else if (last_char == '~')
			bWasTilda = true;

	}
	sFilePath = m_sLongNameConvert;
	return;
}

tERROR EnumContext::AddRestPoint(pRESTORE_ORIGIN_STRING* fRest,tDWORD* fRestCount, tDWORD nOrigPos, tDWORD nOrigSize, tDWORD nNewPos, tDWORD nNewSize)
{
	tERROR error;
	pRESTORE_ORIGIN_STRING pRestPoint;
	if (!fRest)
		return errOK;
	if (PR_FAIL(error=m_pStartUpEnum->heapRealloc((tPTR*)fRest,(tPTR)*fRest,(*fRestCount+1)*sizeof(RESTORE_ORIGIN_STRING))))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<AddRestPoint> Cannot allocate memory, %terr",error));
		return error;
	}
	pRestPoint = &(*fRest)[*fRestCount];
	pRestPoint->oPoint = nOrigPos;
	pRestPoint->oSize  = nOrigSize;
	pRestPoint->nPoint = nNewPos;
	pRestPoint->nSize  = nNewSize;
	*fRestCount++;
	return errOK;
}

tERROR EnumContext::ParsEnviroment(const wchar_t* sString, cPrStrW& sTarget,pRESTORE_ORIGIN_STRING* fRest,tDWORD* fRestCount)
{
	tDWORD		SubLen;
	tERROR		fRet=errOK;
	wchar_t*	wEnv=NULL;
	const wchar_t* sCurPoint;
	const wchar_t* sNextPoint=NULL;
	wchar_t     local_buff[0x80];
	wchar_t     local_src_buff[0x80];
	cPrStrW     sSourceCopy(local_src_buff, countof(local_src_buff));
	cPrStrW     sTempStr(local_buff, countof(local_buff));
	int         nSourceLen = wcslen(sString);
	int         nVarLen;
	tERROR      error;
	int         sp_char;

	if (fRest && fRestCount)
	{
		*fRest = NULL;
		*fRestCount = 0;
	}

	if (sString == sTarget)
	{
		sSourceCopy = sString; // make a copy
//		sString = sSourceCopy; // point to copy
		sCurPoint=(wchar_t*)sSourceCopy;
	}
	else
		sCurPoint=sString;

	if(!sTarget.reserve_len(1))
		return errNOT_ENOUGH_MEMORY;
	sTarget[0] = 0;
	tDWORD nTargetUsed = 0;

	if (_wcsnicmp(sCurPoint,L"\\SystemRoot\\",12)==0)
	{
		sTempStr = "SystemRoot";
		wEnv=_wgetenv(sTempStr);
		if (wEnv!=NULL)
		{
			nVarLen = wcslen(wEnv);
			if (!sTarget.reserve_len(nVarLen + 2))
				return ERROR_NOT_ENOUGH_MEMORY;
			wcscpy(sTarget,wEnv);
			if (sTarget[nVarLen-1]!=L'\\') 
			{
				sTarget[nVarLen] = '\\';
				nVarLen++;
			}
			sTarget[nVarLen] = 0;
			nTargetUsed = nVarLen;
			if (sCurPoint[12]==L'\\') 
				sCurPoint=sCurPoint+13;
			else 
				sCurPoint=sCurPoint+12;
			
//			if (PR_FAIL(error = AddRestPoint(fRest, fRestCount, 0, sCurPoint-sString-1, 0, nVarLen)))
			if (PR_FAIL(error = AddRestPoint(fRest, fRestCount, 0, sCurPoint-(wchar_t*)sSourceCopy-1, 0, nVarLen)))
				return error;
		}
	}
	
	const char sp_c[2] = {'%', '!'};
	for (sp_char=0; sp_char<2; sp_char++)
	{
		while (NULL != (sNextPoint=wcschr(sCurPoint, sp_c[sp_char])))
		{
			int Len=nTargetUsed;
			SubLen=sNextPoint-sCurPoint;
			if (!sTarget.reserve_len(Len+SubLen+2))
				return ERROR_NOT_ENOUGH_MEMORY;
			wcsncat(sTarget,sCurPoint,SubLen);
			nTargetUsed = Len+SubLen;
			sTarget[Len+SubLen]=0;
			sCurPoint=sNextPoint;
			sNextPoint=wcschr(sCurPoint+1, sp_c[sp_char]);
			if (sNextPoint==NULL) 
				break;
			nVarLen = sNextPoint-sCurPoint-1;
			if (!sTempStr.reserve_len(nVarLen+10))
				return ERROR_NOT_ENOUGH_MEMORY;
			wcsncpy(sTempStr,sCurPoint+1,nVarLen);
			sTempStr[nVarLen]=0;
			wEnv=_wgetenv(sTempStr);
			if (wEnv!=NULL)
			{
				nVarLen = wcslen(wEnv);
				if (!sTarget.reserve_len(nTargetUsed + nVarLen + 2))
					return ERROR_NOT_ENOUGH_MEMORY;
				wcscpy(sTarget+nTargetUsed, wEnv);
//				if (PR_FAIL(error = AddRestPoint(fRest, fRestCount, sCurPoint-sString, sNextPoint-sCurPoint+1, nTargetUsed, nVarLen)))
				if (PR_FAIL(error = AddRestPoint(fRest, fRestCount, sCurPoint-(wchar_t*)sSourceCopy, sNextPoint-sCurPoint+1, nTargetUsed, nVarLen)))
					return error;
				nTargetUsed += nVarLen;
			}
			else
			{
				nVarLen = sNextPoint-sCurPoint+1;
				if (!sTarget.reserve_len(nTargetUsed + nVarLen + 2))
					return ERROR_NOT_ENOUGH_MEMORY;
				wcsncpy(sTarget+nTargetUsed, sCurPoint, nVarLen);
				nTargetUsed += nVarLen;
				sTarget[nTargetUsed] = 0;
			}
			sCurPoint=sNextPoint+1;
		}
	}
	
	if (!sTarget.reserve_len(nTargetUsed + wcslen(sCurPoint) + 1))
		return ERROR_NOT_ENOUGH_MEMORY;
	wcscpy(sTarget+nTargetUsed,sCurPoint);
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<ParsEnv> result: <%S>, used=%d", (wchar_t*)sTarget, nTargetUsed));
	return fRet;
}

tBOOL EnumContext::IfPipeName(/*tDWORD size, */const wchar_t* name)
{
	if (wcsncmp(name,L"\\\\?\\",4)==0)
		name=name+4;
	if (wcschr(name,'\\')!=0)
		return cFALSE;
	if (wcschr(name,'.')!=0)
		return cFALSE;
	return cTRUE;
//	if (name[size-1]=='.') size--;
//	if (size==3)
//	{
//		if (wcsnicmp(name,L"prn",3)==0) 
//			return cTRUE;
//		if (wcsnicmp(name,L"aux",3)==0) 
//			return cTRUE;
//	}
//	if (size==4)
//	{
//		if (wcsnicmp(name,L"lpt",3)==0) 
//		{
//			if (name[3]>=L'1' && name[3]<=L'9')
//				return cTRUE;
//			else
//				return cFALSE;
//		}
//		if (wcsnicmp(name,L"com",3)==0) 
//		{
//			if (name[3]>=L'1' && name[3]<=L'4')
//				return cTRUE;
//			else
//				return cFALSE;
//		}
//	}
//	return cFALSE;
}