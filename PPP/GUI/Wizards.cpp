// TrainingWizard.cpp : implementation of the TrainingWizard
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Wizards.h"
// #include "iface/i_loader.h"
// #include "iface/i_cryptohelper.h"
#include "iface/i_inifile.h"
// #include "plugin/p_win32_reg.h"
// #include "plugin/p_win32_nfio.h"
#include "plugin/p_inifile.h"
#include "structs/s_mc_trafficmonitor.h"
// #include "Kav50Settings/ss.h"
// #include "Kav50Settings/Upgrade.h"
#include <plugin/p_miniarc.h>

#include <algorithm>
#include <avzscan/structs/s_avzscan.h>

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#define STRID_LICERR              "LicOperationError"

#define pmc_PDM_RULE_ADDED        0xf031decb //is a crc32 for "pmc_PDM_RULE_ADDED"
#define pmc_JOB_FINISHED          0xf031decd

static cStringObj AvzGetScriptBase(cStringObj& szScriptName)
{
	return szScriptName.substr(0, szScriptName.find_last_of(L"."));
}

static tDWORD AvzGetScriptID(cStringObj& szScriptName)
{
	return atoi((char*)szScriptName.substr(szScriptName.find_last_of(L".")+1).c_str(cCP_ANSI));
}

static tERROR AvzMakeSettings(cAVZScanSettings *pSett, cVector<cGuiAvzScript> &vTroubles)
{
	if( !pSett )
		return errPARAMETER_INVALID;

	pSett->m_szScriptFile.clear();
	pSett->m_aScriptID.clear();
	
	if( !pSett->m_dwScriptMode )
		return errOK;

	pSett->m_szScript.clear();

	if( !vTroubles.size() )
		return errPARAMETER_INVALID;

	for(tDWORD i = 0; i < vTroubles.size(); i++)
	{
		cGuiAvzScript &script = vTroubles[i];

		if( pSett->m_dwScriptMode == 2 && !script.m_bEnabled )
			continue;

		tDWORD dwScriptID = AvzGetScriptID(script.m_sName);
		cStringObj szScriptBase = AvzGetScriptBase(script.m_sName); 
		cAVZCustomScript& hScript = pSett->m_aScriptID.push_back();
		hScript.dwScriptID = dwScriptID;
		hScript.szScriptBase = szScriptBase;
	}
	pSett->m_szScriptBase = "%Bases%\\"; 
	pSett->m_szScriptBackupBase = "%Backup%\\"; 

	if( !pSett->m_aScriptID.size() )
		return errPARAMETER_INVALID;

	return errOK;
}

static tERROR AvzGetResult(LPCSTR strProfile, cVector<cGuiAvzScript> &vTroubles)
{
	cAVZScanStatistics info;
	tERROR err = g_hTM->GetProfileInfo(strProfile, &info);
	if( PR_FAIL(err) )
		return err;
	
	// Если количество выполненных скриптов не совпадает - рапортуем об ошибке
	if( vTroubles.size() != info.m_aXMLRecords.size() )
		return errNOT_OK;
	
	for(tDWORD i = 0; i < info.m_aXMLRecords.size(); i++)
	{
		cGuiAvzScript &script = vTroubles[i];
		script.m_nResult = ((cAVZScanResult*)(info.m_aXMLRecords.at(i)).ptr_ref())->dwResult;
		script.m_bEnabled = (int)script.m_nResult > 1;
	}

	for(tDWORD i = 0; i < vTroubles.size();)
		if( (int)vTroubles[i].m_nResult <= 0 )
			vTroubles.remove(i);
		else
			i++;

	return errOK;
}

//////////////////////////////////////////////////////////////////////////
// CAvzWizard

CAvzWizard::CAvzWizard(CSinkCreateCtx * pCtx) :
	CTaskProfileView(CTaskProfileView::fStat|CTaskProfileView::fEdit, pCtx)
{}

void CAvzWizard::OnInit()
{
	CTaskProfileView::OnInit();
	LoadSection();
}

void CAvzWizard::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	CTaskProfileView::OnEvent(nEventId, pData);
	
	cProfileBase *pProfile = (cProfileBase *)pData;
	if( nEventId == pmPROFILE_STATE_CHANDED && m_pProfile && pProfile && pProfile->isBasedOn(cProfileBase::eIID) && IsMyTask(pProfile) )
	{
		tTaskState ts = pProfile->task_state();
		if( IS_TASK_FINISHED(ts) )
		{
			if( CItemBase * pPage = Item()->GetCurPage() )
				if( ts == TASK_STATE_COMPLETED )
					OnTaskComplete(pPage, pProfile);
			Item()->UpdateData(true);
			Item()->SetNextPage();
		}
	}
}

void CAvzWizard::OnCustomizeData(cSerializable * pData, bool bIn)
{
	AvzMakeSettings((cAVZScanSettings *)pData, m_vTroubles);
	CTaskProfileView::OnCustomizeData(pData, bIn);
}

void CAvzWizard::LoadSection()
{
	m_vTroubles.clear();
	tString section = Item()->m_strSection;
	ReadSection(section);
	section += "-Res";
	ReadSection(section);
}

void CAvzWizard::ReadSection(tString section)
{
	section += GUI_STR_AUTOSECT_SEP;
	section += "Scripts";

	tKeys sectionKeys;
	Root()->GetKeys(PROFILE_LOCALIZE, section.c_str(),  sectionKeys, false, Item());

	for(size_t i = 0, n = sectionKeys.size(); i < n; i++)
	{
		tIniKey &key = sectionKeys[i];
		cGuiAvzScript &trouble = m_vTroubles.push_back();
		trouble.m_bEnabled = cFALSE;
		remove_quotes(key);     Root()->LocalizeStr(trouble.m_sName,        key.c_str(),     key.size());
		remove_quotes(key.val); Root()->LocalizeStr(trouble.m_sDescription, key.val.c_str(), key.val.size());
	}
}

void CAvzWizard::OnTaskComplete(CItemBase *pPage, cProfileBase *pProfile)
{
	if( CAvzWizard::IsMyTask(pProfile) )
		AvzGetResult(pProfile->m_sName.c_str(cCP_ANSI), m_vTroubles);
}

//////////////////////////////////////////////////////////////////////////
// CAvzSecurityAnalyzer

CAvzSecurityAnalyzer::CAvzSecurityAnalyzer(CSinkCreateCtx * pCtx) :
	CAvzWizard(pCtx),
	m_nDetectDanger(0)
{
	m_pScanVulnerab = m_data.GetProfile(AVP_PROFILE_SCAN_VULNERABILITIES);
}

bool CAvzSecurityAnalyzer::OnSelect(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	scase("ScanSolution")
		if( LoadSectionEx() )
		{
			((cAVZScanSettings *)m_pProfile->settings())->m_dwScriptMode = 1;
			SaveSettings();
			m_pProfile->SetState(TASK_REQUEST_RUN);
		}
		else
			Item()->SetCurPage("Script");
		sbreak;
	
	scase("ScanTrouble2")
		LoadSection();
		((cAVZScanSettings *)m_pProfile->settings())->m_dwScriptMode = 1;
		SaveSettings();
		m_pProfile->SetState(TASK_REQUEST_RUN);
		sbreak;

	scase("ScanVulnerab")
		if( m_pScanVulnerab )
			m_pScanVulnerab->SetState(TASK_REQUEST_RUN);
		sbreak;
	send;

	return true;
}

bool CAvzSecurityAnalyzer::IsMyTask(cProfileBase *pProfile)
{
	if( m_pScanVulnerab && m_pScanVulnerab->m_sName == pProfile->m_sName )
		return true;
	return CAvzWizard::IsMyTask(pProfile);
}

void CAvzSecurityAnalyzer::OnTaskComplete(CItemBase *pPage, cProfileBase *pProfile)
{
	CAvzWizard::OnTaskComplete(pPage, pProfile);
	if( pPage->m_strItemId == "ScanTrouble" || pPage->m_strItemId == "ScanTrouble2" )
	{
		m_nDetectDanger = 0;
		for(size_t i = 0, n = m_vTroubles.size(); i < n; i++)
		{
			cGuiAvzScript &script = m_vTroubles[i];
			if( (int)m_nDetectDanger < (int)script.m_nResult )
				m_nDetectDanger = script.m_nResult;
		}
	}
	else
	if( m_pScanVulnerab && pPage->m_strItemId == "ScanVulnerab" )
	{
		m_vThreats.clear();
		if( cAVS * pAvs = g_hGui->m_pAvs )
		{
			cAVSTreats* pThreats;
			if( PR_SUCC(pAvs->GetTreats(m_pScanVulnerab->m_nRuntimeId, &pThreats)) )
			{
				cThreatsInfo numThreats;
				pThreats->GetInfo(&numThreats);

				m_vThreats.resize(numThreats.m_nThreats);
				for(size_t i = 0, n = numThreats.m_nThreats; i < n; i++)
					pThreats->GetTreatInfo(FILTER_NONE, i, &m_vThreats[i]);

				pAvs->ReleaseTreats(pThreats);
			}
		}
	}
}

bool CAvzSecurityAnalyzer::LoadSectionEx()
{
	cVector<cGuiAvzScript> vTroubles;
	swap_objmem(vTroubles, m_vTroubles);
	for(size_t i = 0, n = vTroubles.size(); i < n; i++)
	{
		cGuiAvzScript &script = vTroubles[i];
		if( script.m_bEnabled )
		{
			tString section;
			Root()->LocalizeStr(section, script.m_sDescription.data(), script.m_sDescription.size());
			ReadSection(section);
		}
	}

	return !m_vTroubles.empty();
}


/////////////////////////////////////////////////////////////////////////////
// CTaskRulesFinder

static int GetLongPathNameEx(char *From, char *To)
{
#if defined (_WIN32)
	memcpy(To, From, 3);
	To[3] = 0;
	
	char *pFromLo = &From[3];
	char *pTo = &To[3];
	char *pFromHi;
	
	for(; pFromHi = strchr(pFromLo, '\\'); pFromLo = pFromHi)
	{
		pFromHi++;
		
		int nLen = pFromHi - pFromLo - 1;
		memcpy(pTo, pFromLo, nLen);
		*(pTo + nLen) = 0;
		
		WIN32_FIND_DATA wfd = {0, };
		HANDLE hFind = FindFirstFile(To, &wfd);
		if(hFind == INVALID_HANDLE_VALUE)
			return 0;
		FindClose(hFind);
		
		strcpy(pTo, wfd.cFileName);
		
		pTo += strlen(pTo);
		*pTo++ = '\\';
		*pTo = 0;
	}
	strcpy(pTo, pFromLo);
#else
#warning "Consideration is needed"
#endif	
	return 1;
}

CExecFinder::CExecFinder(CRootItem * pRoot) :
	CRootTaskBase(pRoot, 0x5a6539ae), //is a crc32 for "CTaskRulesFinder"
	m_nEvent(0),
	m_err(errOK),
	m_working(false)
{}

CExecFinder::~CExecFinder()
{
	Stop();
	wait_for_stop();
}

bool CExecFinder::Begin(CItemBase * pParent, tDWORD nEvent)
{
	wait_for_stop();
	
	m_pParent    = pParent;
	m_err        = errOK;
	m_nEvent     = nEvent;
	m_aborted    = false;
	m_working    = true;
	m_TotalFiles = 0;

	ReportCompleted(0);
	return start();
}

void CExecFinder::Stop()
{
	if( m_working )
		m_aborted = true;
}

tERROR CExecFinder::Find(const cStringObj &sRootDir, cProcessMonitor *pPM, tDWORD nDeep)
{
	tERROR err = errOK;
	tDWORD filesEnumerated = 0;
	cAutoObj<cObjPtr> pIoPtr;
	if( PR_SUCC(err) ) err = g_root->sysCreateObject((hOBJECT*)&pIoPtr, IID_OBJPTR, PID_NATIVE_FIO);
	if( PR_SUCC(err) ) err = sRootDir.copy(pIoPtr, pgOBJECT_PATH);
	if( PR_SUCC(err) ) err = pIoPtr->sysCreateObjectDone();
	if( PR_SUCC(err) ) err = pIoPtr->propSetStr(NULL, pgMASK, (tPTR)"*.exe");
	if( PR_SUCC(err) ) err = pIoPtr->Reset(cFALSE);
	if( PR_SUCC(err) )
	{
		while( !m_aborted && PR_SUCC(pIoPtr->Next()))
		{
			cStringObj sFile; sFile.assign(pIoPtr, pgOBJECT_FULL_NAME);
			if( pIoPtr->get_pgIS_FOLDER() )
			{
				if( m_TotalFiles )
					Find(sFile, pPM, nDeep + 1);
			}
			else
			{
				cModuleInfoKLSRL mi;
				tERROR err = pPM->GetFileInfoEx(sFile.data(), &mi);
				PR_TRACE(( g_hGui, PR_SUCC(err) ? prtIMPORTANT : prtERROR, "gui\tCExecFinder: find result %terr, file %S", err, sFile.data()));
			}
			filesEnumerated++;

			if( m_TotalFiles && !nDeep )
				ReportCompleted(100 * filesEnumerated / m_TotalFiles);
		}
	}	
	if( !m_TotalFiles )
	{
		m_TotalFiles = filesEnumerated;	
		return m_TotalFiles ? err : errNOT_FOUND;
	}
	return err;
}

void CExecFinder::do_work()
{
	cProcessMonitor * pPM = NULL;
	if( PR_SUCC(g_hTM->GetService(0, (hOBJECT)g_hTM, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM)) )
	{
		cStringObj sProgramFiles = L"%ProgramFiles%";
		if( g_hGui->ExpandEnvironmentString(sProgramFiles) )
			if( PR_SUCC(Find(sProgramFiles, pPM)) )
				Find(sProgramFiles, pPM);

		if( pPM )
			g_hTM->ReleaseService(0, (cObject *)pPM);
	}
	else
		PR_TRACE(( g_hGui, prtERROR, "gui\tCExecFinder: can't get " AVP_SERVICE_PROCESS_MONITOR " service." ));
}

void CExecFinder::on_exit()
{
	ReportCompleted(100);
	m_pRoot->SendEvent(m_nEvent, NULL, 0, m_pParent);
	m_working = false;
}

void CExecFinder::ReportCompleted(tDWORD nPercent)
{
	cProgressInfo pi; pi.m_nProgressPercent = nPercent;
	m_pRoot->SendEvent(m_nEvent, (cSerializable *)&pi, 0, m_pParent);
}

/////////////////////////////////////////////////////////////////////////////

tERROR LoadFile(cStringObj& strFileName, tPTR* ppData, tDWORD* pdwSize)
{
	tQWORD qwSize;
	tDWORD dwSize;
	tERROR error;
	if (ppData && !pdwSize)
		return errPARAMETER_INVALID;
	if (ppData)
		*ppData = 0;
	cIOObj hIo((cObject *)g_root, cAutoString(strFileName), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
	if( PR_FAIL(error = hIo.last_error()))
	{
		PR_TRACE((g_root, prtERROR, "gui\tLoadFile(%S) failed %terr", strFileName.data(), error));
		return error;
	}
	if (!ppData)
		return error;
	if( PR_FAIL(error = hIo->GetSize(&qwSize,IO_SIZE_TYPE_EXPLICIT)))
		return error;
	if (qwSize > 10*1000*1024) // >10Mb  - something wrong here!
	{
		PR_TRACE((g_root, prtIMPORTANT, "gui\tLoadFile(%S), size=%d", strFileName.data(), qwSize));
		return error = errUNEXPECTED;
	}
	dwSize = (tDWORD)qwSize;
	if (PR_FAIL(error = g_root->heapRealloc(ppData, *ppData, dwSize)))
		return error;
	if( PR_FAIL(error = hIo->SeekRead(0, 0, *ppData, dwSize)))
	{
		g_root->heapFree(*ppData);
		*ppData = 0;
		*pdwSize = 0;
		return error;
	}
	*pdwSize = dwSize;
	return error;
}

tERROR CheckFolderNotEmpty(cStringObj& strPath)
{
	tERROR error = errOK;
	cAutoObj<cObjPtr> hPtr;
	error = g_root->sysCreateObjectQuick((hOBJECT*)&hPtr, IID_OBJPTR, PID_NATIVE_FIO);
	if (PR_FAIL(error))
		return error;
	if (PR_FAIL(error = hPtr->ChangeTo(cAutoString(strPath))))
		return error;
	if (PR_FAIL(error = hPtr->StepDown()))
		return error;
	while(PR_SUCC(hPtr->Next())) 
	{
		if (hPtr->get_pgOBJECT_ATTRIBUTES() & fATTRIBUTE_DIRECTORY)
			continue;
		return errOK;
	};
	
	return errNOT_FOUND;
}

/////////////////////////////////////////////////////////////////////////////

bool AddProductKey(const cStringObj& strKeyPath, CItemBase * pParent)
{
	if( !g_hGui->m_pLic )
	{
		MsgBoxErr(pParent, &cGuiErrorInfo(errOBJECT_BAD_INTERNAL_STATE));
		return false;
	}

	cLicErrorInfo licErrInfo;
	licErrInfo.m_nOperationCode = LICOPCODE_ADD;
	licErrInfo.m_strKeyFile = strKeyPath;

	if( licErrInfo.m_nErr = g_hGui->m_pLic->AddKey(strKeyPath.c_str(cCP_UNICODE), KAM_ADD) )
	{
		PR_TRACE((g_hGui, prtNOTIFY, "gui\tGUI::AddKey: error %terr", licErrInfo.m_nErr));
		return MsgBoxErr(pParent, &licErrInfo, STRID_LICERR), false;
	}

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tGUI::AddKey: the key was added successfully"));
	return true;
}

CAddProductOnlineKey::CAddProductOnlineKey(CRootItem * pRoot) :
	CRootTaskBase(pRoot, 0xFFFF1230),
	m_err(errOK),
	m_hForm(NULL)
{
	g_hGui->sysCreateObjectQuick((hOBJECT*)&m_hForm, IID_STRING);
}

CAddProductOnlineKey::~CAddProductOnlineKey()
{
	wait_for_stop();
	if (m_hForm)
		m_hForm->sysCloseObject();
}

bool CAddProductOnlineKey::CheckActiveKey(cCheckInfo& pInfo)
{
	if( !g_hGui->m_pLic )
		return false;
	return PR_SUCC(g_hGui->m_pLic->CheckActiveKey(&pInfo));
}

bool CAddProductOnlineKey::Begin(const cUserData& data, CItemBase * pParent)
{
	if( !g_hGui->m_pLic )
	{
		m_err = errOBJECT_BAD_INTERNAL_STATE;
		return false;
	}
	
	wait_for_stop();
	
	m_pData = data;
	m_pParent = pParent;
	
	cUserData * pSendData = (cUserData *)m_pData.ptr_ref();
	pSendData->m_strProdVer  = g_hGui->ExpandEnvironmentString("%ProductVersion%");
	pSendData->m_strAgent    = "Key File Downloader";
	pSendData->m_strUrl      = g_hGui->m_settings->m_LicActivationLink;

	m_err = errOK;

	m_PostData.m_header.clear();
	m_PostData.m_postData.clear();
	start();

	return PR_SUCC(m_err);
}

bool CAddProductOnlineKey::Submit(const tString &strHeaders, tCPTR pPostData, tSIZE_T nPostDataLen)
{
	if( !g_hGui->m_pLic )
{
		m_err = errOBJECT_BAD_INTERNAL_STATE;
		return false;
	}
	
	wait_for_stop();

	m_PostData.m_header = strHeaders.c_str();
	m_PostData.m_postData.resize(nPostDataLen);
	memcpy((tPTR)&m_PostData.m_postData[0], pPostData, nPostDataLen);

	m_err = errOK;
	start();
	
	return PR_SUCC(m_err);
}
	
void CAddProductOnlineKey::Stop()
	{
	if( g_hGui->m_pLic )
		g_hGui->m_pLic->StopOLA();
	wait_for_stop();
	m_err = errOPERATION_CANCELED;
}
		
bool CAddProductOnlineKey::Finish(cCheckInfo& pInfo, cStringObj& htmlForm, cLicErrorInfo& licErrInfo)
{
	wait_for_stop();
		
	if( m_err == errOK )
		{
		tDWORD len = 0;
		m_hForm->Length(&len);
		if (len)
		{
			htmlForm = m_hForm;
			return true;
		}
		return pInfo = m_Info, true;
	}
	if( m_err == errOPERATION_CANCELED )
		return false;
		
	licErrInfo.m_nOperationCode = LICOPCODE_ADDONLINE;
	licErrInfo.m_nErr = m_err;
	if (m_err == errUNKNOWN_OLA_SERVER_ERROR)
	{
		licErrInfo.m_dwOnlineActSrvError = m_dwOLAErrCode;
		licErrInfo.m_dwOnlineActSrvErrorSpecific = m_dwOLAErrCodeSpecific;

	}
	MsgBoxErr(m_pParent, &licErrInfo, STRID_LICERR);
	return false;
}

void CAddProductOnlineKey::do_work()
{
	cMsgReceiver * hMsgRcv = NULL;
	m_err = g_hGui->sysCreateObject((hOBJECT*)&hMsgRcv, IID_MSG_RECEIVER, PID_ANY,SUBTYPE_ANY);
	if( !hMsgRcv )
		return;
	
	m_err = hMsgRcv->propSetPtr(pgRECEIVE_CLIENT_ID, (tPTR)this);
	m_err = hMsgRcv->propSetDWord(pgRECEIVE_PROCEDURE, (tDWORD)_MsgReceive);
	m_err = hMsgRcv->sysCreateObjectDone();
	hMsgRcv->sysRegisterMsgHandler(pmc_ONLINE_LICENSING, rmhDECIDER, g_hGui->m_pLic, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	m_hForm->Cut(cSTRING_WHOLE, 0);
	if (m_PostData.m_header.empty() && m_PostData.m_postData.empty())
		m_err = g_hGui->m_pLic->AddOLAKey(m_pData, KAM_ADD, m_hForm, &m_Info, &m_dwOLAErrCode, &m_dwOLAErrCodeSpecific);
	else
		m_err = g_hGui->m_pLic->SubmitOLAForm(&m_PostData, KAM_ADD, m_hForm, &m_Info, &m_dwOLAErrCode, &m_dwOLAErrCodeSpecific);
	
	g_hGui->m_pLic->sysSendMsg(pmc_ONLINE_LICENSING, pm_COMPLETED, NULL, NULL, NULL);

	hMsgRcv->sysUnregisterMsgHandler(pmc_ONLINE_LICENSING, g_hGui->m_pLic);
	hMsgRcv->sysCloseObject();
}

tERROR CAddProductOnlineKey::OnMsg(tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
{
	if( msg_cls != pmc_ONLINE_LICENSING )
		return errOK;

	if( (msg_id == pm_DOWNLOAD_COMPLETED) && pbuff &&
		blen && (*blen = sizeof(tERROR)) && PR_FAIL(*(tERROR *)pbuff) )
		return errOK;
	
	//PR_TRACE((g_root, prtIMPORTANT, "gui\tCAddProductOnlineKey::OnMsg(msg_id = 0x%x, pData = 0x%p)", msg_id, pData));
	cSerializable * pData = blen == SER_SENDMSG_PSIZE ? (cSerializable *)pbuff : NULL;
	return g_pRoot->SendEvent(msg_cls + msg_id, pData, pData ? EVENT_SEND_SYNC : 0, m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
// CConfigureWizard

CConfigureWizard::CConfigureWizard(tDWORD nFlags, CProfile * pDst) :
	TBase(pDst ? pDst : g_pProduct, (nFlags & fSettings) ? fEdit : 0),
	m_activation(g_pRoot),
	m_bLicActivating(0),
	m_ExecFinder(g_pRoot),
	m_Reseter(&m_data),
	m_bSilent(cFALSE),
	m_bPervSettingsLoaded(0),
	m_bUpgrade(0),
	m_Kav46Type(None),
	m_nPiwFlags(nFlags),

	m_eTriggers((enWisardTriggers)(wzMaskActivationOnline | wzMaskSettings)),
	m_eLicTriggers(wzNone),
	m_bAlreadyTrialled(cFALSE),
//	m_nNetworkState(_fwnzUntrusted),
	m_bInteractiveExtendedMode(cFALSE),
	m_bInteractiveAhLearn(cTRUE),
	m_bInteractiveInvader(cTRUE),
	m_bInteractiveIntegrity(cTRUE),
	m_bInteractivePdmRegistry(cTRUE),
	m_bIsDnsEnabled(cFALSE),
	m_bEnableDns(cTRUE),
	m_bActivationNeeded(cTRUE),
	m_bConnected(cFALSE),
	m_bSentCode(cFALSE),
	m_bRetrievedKey(cFALSE),
	m_bFwRulesAdding(cFALSE),
	m_eInheritTriggers(wzitNone),
	m_eInheritTriggersAvail(wzitNone),
	m_bReboot(cTRUE),
	m_bStartProduct(cTRUE),
	m_bExpress(cFALSE)
{
	m_nActionsMask = DLG_ACTION_WIZARD;
	m_strSection = "ConfigureWizard";
	m_eInitTriggers = m_eTriggers = (enWisardTriggers)-1;
	
	{
		tDWORD nInitMode = g_hGui->GetInitFlags();
		m_bInitMode = !!(nInitMode & BL_INITMODE_INIT);
		m_bLoadPervSettings = !!(nInitMode & BL_INITMODE_LOAD_SETTINGS);
		m_bReboot = !(nInitMode & BL_INITMODE_NOREBOOT);
		m_bExpress = !!(nInitMode & BL_INITMODE_EXPESS);
		m_bUpgrade = !!(nInitMode & BL_INITMODE_UPGRADE);
	}

	m_bInteractiveExtendedMode = cFALSE;
	m_bInteractiveAhLearn = cFALSE;
	m_bInteractiveInvader = cFALSE;
	m_bInteractivePdmRegistry = cFALSE;
	m_bInteractiveIntegrity = cFALSE;

	if( m_bInitMode )
	{
		if( PR_FAIL(LoadCfg("%ProductRoot%\\install.cfg")) )
		{
			if( m_bLoadPervSettings )
				m_bPervSettingsLoaded = PR_SUCC(LoadCfg("%DataFolder%\\avp.cfg"));

			if( !m_bPervSettingsLoaded )
			{
				// т.к. устаревших hidden профилей вроде AVP_PROFILE_RETRANSLATION
				// нет в m_data, pm_SETTINGS_UPGRADE не сможет из них прочитать
				// поэтому апгрейдим здесь
				if (UpgradeSettingsFromHiddenProfiles())
					m_data.SetSettingsDirty(true, true);
			}
		}

		if (CProfile* pProtection = m_data.GetProfile(AVP_PROFILE_PROTECTION))
		{
			// bf 16789: CProfile через sysSendMsg не пролезет в другой процесс!
			cProfileEx protection;
			pProtection->PopSettings();
			pProtection->CopyTo(protection);
			if (errOK_DECIDED == g_hTM->sysSendMsg(pmc_SETTINGS, pm_SETTINGS_UPGRADE, NULL, &protection, SER_SENDMSG_PSIZE))
				pProtection->CopyFrom(protection);
		}
	}

	if( g_hGui->m_pLic )
	{
		cLicInfo licInfo;
		if( PR_SUCC(g_hGui->m_pLic->GetInfo(&licInfo)) )
			m_bAlreadyTrialled = licInfo.m_blTrialPeriodOver;
	}

#ifdef _DEBUG
//	{
//		cGuiLicCheckKeyInfo _key;
//		
//		_key.m_LicInfo.m_strKeyFileName = "test1.key";
//		_key.m_errCheck = errKEY_INST_EXP;
//		m_KeysDiscarted.m_aKeys.push_back(_key);
//		
//		_key.m_LicInfo.m_strKeyFileName = "test2.key";
//		_key.m_errCheck = errLICENSE_INFO_WRONG_FOR_PRODUCT;
//		m_KeysDiscarted.m_aKeys.push_back(_key);
//	}
#endif
}

CConfigureWizard::~CConfigureWizard()
{
	if( m_bLicActivating )
		m_activation.Stop();
}

tERROR CConfigureWizard::LoadCfg(LPCSTR strCfgFile)
{
	cProfileEx oTopProile;
	cMsgParams Params;
	Params.m_strVal1 = g_hGui->ExpandEnvironmentString(strCfgFile);
	Params.m_nVal1 = &oTopProile;

	tERROR err = g_hTM->sysSendMsg(pmc_SETTINGS, pm_LOAD_SETTINGS, NULL, &Params, SER_SENDMSG_PSIZE);
	if( PR_SUCC(err) )
	{
		PR_TRACE((g_hGui, prtERROR, "gui\tTrying to load config file %s...", strCfgFile));
		for(tUINT i = 0; i < oTopProile.count(); i++)
		{
			cProfileEx *pProfile = oTopProile[i];
			if( pProfile->m_eOrigin == cProfile::poUser )
			{
				tERROR err = g_hTM->CloneProfile(NULL, pProfile, 0, NULL);
				PR_TRACE((g_hGui, prtNOTIFY, "gui\tCloning of user profile %S returned %terr", pProfile->m_sName.data(), err));
			}
		}

		// Don't allow to overwrite password settings
		{
			cBLSettings * pSrcCfg = (cBLSettings *)(cTaskSettings *)oTopProile.settings();
			cBLSettings * pDstCfg = (cBLSettings *)m_data.settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
			
			if( pDstCfg && pSrcCfg && pSrcCfg->isBasedOn(cBLSettings::eIID) )
				CSettingsReseter::CopyNotResetableProtSettings(pSrcCfg, pDstCfg);
		}
		
		m_data.CopyFrom(oTopProile, true);
	}
	else
		PR_TRACE((g_hGui, prtERROR, "gui\tFailed to load config file %s (%terr)", strCfgFile, err));

	return err;
}

bool CConfigureWizard::UpgradeSettingsFromHiddenProfiles()
{
	cBLSettings* pBLSettings = (cBLSettings*) m_data.settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
	if (!pBLSettings)
		return false;

	tDWORD v_major = 6, v_minor = 0, v_mp = 0, v_build = 0;
	sscanf((tSTRING)pBLSettings->m_SettingsVersion.c_str(cCP_ANSI), "%d.%d.%d.%d", &v_major, &v_minor, &v_mp, &v_build);

	cUpdaterSettings* pUpdSet = (cUpdaterSettings*)	m_data.settings(AVP_PROFILE_UPDATER, cUpdaterSettings::eIID);
	if (pUpdSet && v_major == 6 && v_minor == 0 && v_mp <= 1)
	{
		// bf 19205
		cAutoObj<cRegistry> hReg;
		tDWORD nResult;
		tCHAR sKey1 [] = "HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\profiles\\Retranslation\\settings";
		if (PR_SUCC(::g_root->sysCreateObject((hOBJECT *)&hReg, IID_REGISTRY, PID_WIN32_REG))
			&& PR_SUCC(hReg->propSetStr(0, pgROOT_POINT, sKey1))
			&& PR_SUCC(hReg->sysCreateObjectDone()))
		{
			tCHAR    sPathName[0x200];
			tTYPE_ID nType = tid_STRING;
			if (PR_SUCC(hReg->GetValue(&nResult, cRegRoot, "RetrPath", &nType, sPathName, countof(sPathName))))
				pUpdSet->m_strRetrPath = sPathName;
			hReg.clean();
		}
		tCHAR sKey2 [] = "HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\profiles\\Retranslation\\schedule";
		if (PR_SUCC(::g_root->sysCreateObject((hOBJECT *)&hReg, IID_REGISTRY, PID_WIN32_REG))
			&& PR_SUCC(hReg->propSetStr(0, pgROOT_POINT, sKey2))
			&& PR_SUCC(hReg->sysCreateObjectDone()))
		{
			tDWORD Mode = schmManual;
			tTYPE_ID nType = tid_DWORD;
			if (PR_SUCC(hReg->GetValue(&nResult, cRegRoot, "Mode", &nType, &Mode, sizeof(Mode))))
				pUpdSet->m_bUseRetr = Mode != schmManual;
		}
	}
	return true;
}

void CConfigureWizard::DoSilent()
{
	m_bSilent = cTRUE;
	m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~wzMaskActivation);
	
	CDialogItem * pItem = NULL;
	g_pRoot->LoadItem(this, (CItemBase *&)pItem, m_strSection.c_str(), m_strItemId.c_str(), LOAD_FLAG_DESTROY_SINK);

	pItem->DoAutomate();
}

bool CConfigureWizard::ReInit(CDialogSink * pFrom)
{
	if( static_cast<CConfigureWizard *>(pFrom)->m_eInitTriggers != m_eInitTriggers )
		return false;
	return true;
}

#if defined (_WIN32)
static int _dump_fprintf(HANDLE output, char* format, ...)
{
	char buffer[0x1000];
	int len;
	int res;
	va_list args;
	va_start(args, format);
	len = wvsprintfA(buffer, format, args);
	res = WriteFile(output, buffer, len, (DWORD*)&len, 0);
	va_end(args);
	return res;
}
#endif //_WIN32

void CConfigureWizard::AddKeysFromProductRoot()
{
	if( !g_hGui->m_pLic )
		return;

	cAutoObj<cObjPtr> pAttachedFiles;
	tERROR err = g_hGui->sysCreateObject((cObject**)&pAttachedFiles, IID_OBJPTR, PID_NATIVE_FIO);
	if( PR_SUCC(err) )
	{
		g_hGui->ExpandEnvironmentString("%ProductRoot%").copy(pAttachedFiles, pgOBJECT_PATH);
		err = pAttachedFiles->sysCreateObjectDone();
	}

	if( PR_SUCC(err) )
	{
		pAttachedFiles->propSetStr(NULL, pgMASK, (tPTR)"*.key");
		pAttachedFiles->Reset(cFALSE);
		if( PR_SUCC(err = pAttachedFiles->Next()) && pAttachedFiles->get_pgIS_FOLDER() )
			err = errNOT_FOUND;
	}

	if( PR_SUCC(err) )
	{
		cStrObj strKeyFile;
		strKeyFile.assign(pAttachedFiles, pgOBJECT_FULL_NAME);
		PR_TRACE((g_hGui, prtIMPORTANT, "gui\tCConfigureWizard key file: %S", strKeyFile.data()));
		
		err = g_hGui->m_pLic->AddKey((tWSTRING)strKeyFile.data(), KAM_REPLACE);

		cIOObj pKeyFile(*g_hGui, cAutoString(strKeyFile), fACCESS_READ);
		if( PR_SUCC(pKeyFile.last_error()) )
			pKeyFile->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
	}		

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tCConfigureWizard add attached key result: %terr", err));
}

void CConfigureWizard::FillRegistryGuardInitialRules()
{
	// Заполним Registry Guard правилами из баз
// 	if( CProfile * pPdm = m_data.GetProfile(AVP_PROFILE_PDM) )
// 		if( cPDMSettings *pSett = (cPDMSettings *)pPdm->settings(NULL, cPDMSettings::eIID) )
// 			if( pSett->m_RegGroup_List.empty() )
// 				pPdm->ResetSettingsLevel(SETTINGS_LEVEL_RESET);
// 	if( CProfile * pRg2 = m_data.GetProfile(AVP_PROFILE_REGGUARD2) )
// 		if( CRegSettings *pSett = (CRegSettings *)pRg2->settings(NULL, CRegSettings::eIID) )
// 			if( pSett->m_vGroupList.empty() )
// 				pRg2->ResetSettingsLevel(SETTINGS_LEVEL_RESET);
}

void CConfigureWizard::SetUserInfo()
{
	cUpdaterSettings * pUpdaterCfg = (cUpdaterSettings *)m_data.settings(AVP_PROFILE_UPDATER, cUpdaterSettings::eIID);
	if( pUpdaterCfg)
	{
		// auto complete user form and find best update region
		CUserInfo ui; Root()->GetUserInfo(&ui);

		if( pUpdaterCfg )
		{
			pUpdaterCfg->m_strRegion = ui.m_strCountryISO.c_str();
		}
	}

	if( m_eTriggers & wzMaskActivation)
	{
		m_strCustomerId = g_hGui->m_settings->m_strCustomerId;
		m_strCustomerPassword = g_hGui->m_settings->m_strCustomerPwd;
		((CRootSink *)(Root()->m_pSink))->CryptDecrypt(m_strCustomerPassword, false);
	}
}

void CConfigureWizard::OnInit()
{
	if( m_bInitMode )
	{
		AddKeysFromProductRoot();
		FillRegistryGuardInitialRules();
	}

	if( m_eTriggers & wzInherit || m_bExpress )
		UpdateInheritTriggers(); // должен быть перед InheritLic!

	UpdateLicensingTriggers();
	// Предыдущую лицензию подхватываем без всяких вопросов
	if (m_bActivationNeeded && InheritLic(false))
		UpdateLicensingTriggers();

	if( m_bExpress )
	{
		(int &)m_eTriggers &= ~(wzMaskSettings | wzInherit);
		(int &)m_eTriggers |= (wzPdmRulesCollect | wzFwRules);

		if( m_bUpgrade )
			(int &)m_eTriggers &= ~wzMaskActivation;

		// Чтобы при экспесс-инсталляции вообще не появлялась кнопка "Список"
		// при сборе правил антихакера, сразу устанавливаем признак того, 
		// что идет сбор правил
		m_bFwRulesAdding = cTRUE;

		InheritConfiguration(m_eInheritTriggers);
	}

	// Для WKS уберём Online активацию
	if( m_nFlags & fActivation && m_eTriggers & wzActivationBegin )
	{
		if( !Item()->GetItem("ActivateCommercial") && !Item()->GetItem("ActivateTrial") )
		{
			(int &)m_eTriggers &= ~wzMaskActivation;
			(int &)m_eTriggers |= wzActivationChooseKey | wzActivationEnd;
		}
	}
	(int &)m_eLicTriggers = m_eTriggers & wzMaskActivation;

	SetUserInfo();

	PR_TRACE((g_root, prtIMPORTANT, "gui\tOnInit: trigg=%08X inherit=%08X", m_eTriggers, m_eInheritTriggers));

//	if (0 == m_eInheritTriggers)
//		CleanupUpgradeFolder();

	if( m_eTriggers & wzResetArea )
	{
		m_Reseter.FindImportantSettings();
		if( m_Reseter.IsEmpty() )
		{
			m_Reseter.Reset();
			m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~wzResetArea);
		}
	}

	if( m_bUpgrade || m_bPervSettingsLoaded )
	{
		m_Reseter.FillSettings();

//		cPDMSettings * pPDMSett = (cPDMSettings *)m_data.settings(AVP_PROFILE_PDM, cPDMSettings::eIID);
// 		cAHFWSettings * pAHFWSett = (cAHFWSettings *)m_data.settings(AVP_PROFILE_FIREWALL, cAHFWSettings::eIID);
// 
// 		if( (!pAHFWSett || !pAHFWSett->m_AppRules.empty()) && 
// 			(!pPDMSett || !pPDMSett->m_TrustedImageList.empty()) )
// 		{
// 			(int &)m_eTriggers &= ~wzFwAdditional;
// 		}
//
// 		if( !pPDMSett || !pPDMSett->m_TrustedImageList.empty() )
// 		{
// 			(int &)m_eTriggers &= ~(wzPdmRules | wzPdmRulesCollect);
// 			m_Reseter.SetProfileReset(AVP_PROFILE_BEHAVIORMONITOR, false);
// 		}

//		if( !pAHFWSett || !pAHFWSett->m_AppRules.empty() )
// 		{
// 			(int &)m_eTriggers &= ~(wzFwNetworks | wzFwRules);
// 			m_Reseter.SetProfileReset(AVP_PROFILE_ANTIHACKER, false);
// 		}
	}

	CTaskProfileView::OnInit();
}

bool CConfigureWizard::OnAddDataSection(LPCTSTR strSect)
{
	if( !strSect || !(*strSect) )
		Item()->AddDataSource(&m_Reseter);
	
	return TBase::OnAddDataSection(strSect);
}

bool CConfigureWizard::OnSelect(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	scase("ActivationBegin")
		if( m_KeysDiscarted.m_aKeys.size() )
		{
			MsgBox(*this, "LicenseKeysDiscarted", "dlg_caption", MB_OK|MB_ICONEXCLAMATION, &m_KeysDiscarted);
			m_KeysDiscarted.m_aKeys.clear();
		}
		sbreak;
	
	scase("ActivationProgress")
		m_bLicActivating = 1;
		sbreak;

	scase("ActivationEnd")
		m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~wzMaskActivation);
		Item()->UpdateData(true);
		if( m_bLicActivating )
		{
			m_activation.Stop();
			m_bLicActivating = 0;
		}		
		sbreak;

	scase("EnumExe")
		m_Progress = 0;
		m_ExecFinder.Begin(*this, pmc_JOB_FINISHED);
		sbreak;
	send;

	return TBase::OnSelect(pItem);
}

bool CConfigureWizard::OnNext()
{
	CItemBase * pItem = Item()->GetCurPage();
	if( !pItem )
		return false;
	
	sswitch(pItem->m_strItemId.c_str())
	scase("ResetArea")
		m_Reseter.Reset();
		m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~wzResetArea);
		
		if( !m_Reseter.IsProfileReset(AVP_PROFILE_UPDATER) )
			m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~(wzUpdate));
		
		if( !m_Reseter.IsProfileReset(AVP_PROFILE_BEHAVIORMONITOR, "pdm.AppsMonitoring_List") &&
			!m_Reseter.IsProfileReset(AVP_PROFILE_BEHAVIORMONITOR, "pdm.TrustedImageList") )
			m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~(wzPdmRulesCollect));
		
		if( !m_Reseter.IsProfileReset(AVP_PROFILE_ANTIHACKER) )
			m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~(wzFwNetworks));

		if( !m_Reseter.IsProfileReset(AVP_PROFILE_ANTIHACKER, "fw.AppRules") &&
			!m_Reseter.IsProfileReset(AVP_PROFILE_ANTIHACKER, "fw.PortRules") )
			m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~(wzFwRules));
		
		Item()->UpdateData(true);
		sbreak;

	scase("ActivationBegin")
		if( (m_eTriggers & wzActivationProgress) && (g_hGui->m_statistics->m_State.m_KeyState == eKeyOk) )
		{
			cStringObj strLocalization(L"%Disable2WeekKeyInst%");
			g_hGui->ExpandEnvironmentString(strLocalization);
			if(strLocalization == L"1")
			{
				MsgBox(*this, "ActivateOnExistingCommercKeyWarning", NULL, MB_OK|MB_ICONEXCLAMATION);
				return false;
			}
//			else if( MsgBox(*this, "ActivateOnExistingCommercKeyWarning", NULL, MB_YESNO|MB_ICONEXCLAMATION) != DLG_ACTION_YES )
//				return false;
		}
		
		if( (m_eTriggers & wzActivationProgress) && !(m_eTriggers & wzActivationForm) )
			if( !ActivationBegin(true) )
				return false;
		sbreak;

	scase("ActivationForm")
			if( !ActivationBegin(false) )
				return false;
		sbreak;
	
	scase("ActivationServerForm")
		CItemBase * pActivationServerFormItem = GetItem("ActivationServerForm", NULL, false);
		if( pActivationServerFormItem )
		{
			CHTMLItem* pForm = (CHTMLItem*)pActivationServerFormItem->GetItem("ActivationHtmlForm");
			if( pForm )
			{
				pForm->Submit();
				return false;
			}
		}
		sbreak;

	scase("ActivationChooseKey")
		if( !m_bInitMode && !g_hGui->CheckPassword(*this, pwdc_Activate) )
			return false;
		if( !AddProductKey(m_strKeyPath, *this) )
			return false;
		sbreak;

	scase("Update")
		if( !m_bSilent && !true )
        {
			return false;
        }
		sbreak;

	scase("Inherit")
		m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~wzInherit);
		InheritConfiguration(m_eInheritTriggers);
		sbreak;

	send;

	return TBase::OnNext();
}

bool CConfigureWizard::OnPrev()
{
	CItemBase * pItem = Item()->GetCurPage();
	if( !pItem )
		return false;

	if (pItem->m_strItemId == "ActivationServerForm"
		|| pItem->m_strItemId == "ActivationProgress")
	{
		if( m_bLicActivating )
		{
			m_activation.Stop();
			m_bLicActivating = 0;
		}		
		CItemBase * pActivationFormItem = GetItem("ActivationForm", NULL, false);
		if (pActivationFormItem)
			Item()->SetCurPage(pActivationFormItem);
	}

	return TBase::OnPrev();
}

void CConfigureWizard::OnChangedData(CItemBase * pItem)
{
	TBase::OnChangedData(pItem);

	sswitch(pItem->m_strItemId.c_str())
	scase("Controls")
		if( m_LicInfo.m_dwInvalidReason != ekirValid )
		{
			(int &)m_eTriggers &= ~wzMaskActivation;
			(int &)m_eTriggers |= (m_eLicTriggers & wzMaskActivation);
		}
		sbreak;

	scase("KeyBrowse")
		PR_TRACE((g_hGui, prtNOTIFY, "gui\tKey file \"%S\" selected", m_strKeyPath.data()));
		if( g_hGui->m_pLic )
	{
			tERROR err = g_hGui->m_pLic->CheckKeyFile((tWSTRING)m_strKeyPath.data(), KAM_ADD, &m_LicInfo);
			if (PR_SUCC(err))
				PR_TRACE((g_hGui, prtNOTIFY, "gui\tKey check succeded (CustomerInfo: \"%S\")", m_LicInfo.m_KeyInfo.m_strCustomerInfo.data()));
			else
				PR_TRACE((g_hGui, prtERROR, "gui\tKey check failed: %terr", err));
	}
		else
			PR_TRACE((g_hGui, prtERROR, "gui\tNo lic component"));
	
		Item()->UpdateData(true);
		sbreak;
	send;
}

void CConfigureWizard::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	TBase::OnEvent(nEventId, pData);

	switch( nEventId )
	{
	case pmc_ONLINE_LICENSING + pm_POSTING_DATA:
		m_bConnected = cTRUE;
		Item()->UpdateData(true);
		break;
	case pmc_ONLINE_LICENSING + pm_SENT:
		m_bConnected = cTRUE;
		m_bSentCode = cTRUE;
		Item()->UpdateData(true);
		break;
	case pmc_ONLINE_LICENSING + pm_DOWNLOAD_COMPLETED:
		m_bConnected = cTRUE;
		m_bSentCode = cTRUE;
		m_bRetrievedKey = cTRUE;
		Item()->UpdateData(true);
		break;
	
	case pmc_ONLINE_LICENSING + pm_COMPLETED:
		{
			cLicErrorInfo licErrInfo;
			cStringObj htmlForm;
			bool bSucc = m_activation.Finish(m_LicInfo, htmlForm, licErrInfo);
			if( bSucc )
				Item()->UpdateData(true);
			
			if( m_bLicActivating )
			{
				CItemBase * pActivationBeginItem = GetItem("ActivationBegin", NULL, false);
				if (!bSucc && licErrInfo.IsProxyError() && pActivationBeginItem)
					Item()->SetCurPage(pActivationBeginItem);
				else if( htmlForm.empty() )
					{
					cBLSettings* pSet = (cBLSettings *)m_data.settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
						if( pSet && g_hGui->m_pLic && g_hGui->m_pCrHelp)
						{
							g_hGui->m_pLic->GetCustomerCredentials(cAutoString(pSet->m_strCustomerId),
								cAutoString(pSet->m_strCustomerPwd));
							g_hGui->m_pCrHelp->CryptPassword(&cSerString(pSet->m_strCustomerPwd), 0);
						}
						m_bLicActivating = 0;
						Item()->SetNextPage(!bSucc);
					}
				else if( CItemBase * pActivationServerFormItem = GetItem("ActivationServerForm", NULL, false) )
					{
					Item()->SetCurPage(pActivationServerFormItem);
						CItemBase * pForm = pActivationServerFormItem->GetItem("ActivationHtmlForm");
						if (pForm)
						{
							tDWORD styleBeg = htmlForm.find(L"<style>");
							tDWORD styleEnd = htmlForm.find(L"</style>");
							if (styleBeg != cStrObj::npos && styleEnd != cStrObj::npos)
							{
								cStringObj strCss(L"%Skin%");
								g_hGui->ExpandEnvironmentString(strCss);
								strCss.add_path_sect(g_hGui->IsGuiSimple() ? L"actsystm.css" : L"actskin.css");
								if (FILE* f = fopen(strCss.c_str(cCP_ANSI), "rt"))
								{
									char buf[16*1024];
									int n = fread(buf, 1, sizeof(buf) - 1, f);
									buf[n] = 0;
									fclose(f);
									cStringObj newForm = htmlForm.substr(0, styleBeg);
									newForm += buf;
									newForm += htmlForm.substr(styleEnd + sizeof("</style>"));
									htmlForm = newForm;
								}
							}

							pForm->SetText("");
							pForm->SetText(htmlForm.c_str(cCP_ANSI));
						}
					}
				}
			}
		break;

	case pmc_ONLINE_LICENSING + pm_GET_PASS_FOR_PROXY:
		CResultDialogT<CDialogBindingViewT<> >(pData, DLG_ACTION_OKCANCEL|DLG_MODE_ALWAYSMODAL).DoModal(*this, "OnlineActivationProxyAuth");
		break;

	case pmc_JOB_FINISHED:
		if( pData && pData->isBasedOn(cProgressInfo::eIID) )
			m_Progress = ((cProgressInfo *)pData)->m_nProgressPercent;
		else
		{
			if( CItemBase *pPage = Item()->GetCurPage() )
				pPage->Show(false);
			Item()->SetNextPage(false);
		}
		break;
	}
}

void CConfigureWizard::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( m_ExecFinder.m_working )
		Item()->UpdateData(true);
}

bool CConfigureWizard::OnNavigate(CItemBase * pItem,
								  const tString &strURL,
								  int Flags,
								  tCPTR pPostData,
								  tSIZE_T nPostDataLen,
								  const tString &strHeaders,
								  bool &bCancel)
{
	bCancel = true;

	CItemBase * pActivationProgressItem = GetItem("ActivationProgress", NULL, false);
	if (pActivationProgressItem)
		Item()->SetCurPage(pActivationProgressItem);

	m_bConnected = cFALSE; 
	m_bSentCode = cFALSE;
	m_bRetrievedKey = cFALSE;
	Item()->UpdateData(true);

	m_activation.Submit(strHeaders, pPostData, nPostDataLen);
	return true;
}


bool CConfigureWizard::OnClicked(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	stcase(UpdateNow)
		CProfile *pUpdater = m_data.GetProfile(AVP_PROFILE_UPDATER);
		if( !pUpdater )
			return false;
		
		cUpdaterSettings * pSett = (cUpdaterSettings *)pUpdater->settings(NULL, cUpdaterSettings::eIID);
		if( !pSett )
			return false;
		
		if( !true )
			return false;

		if( !g_hTM || PR_FAIL(g_hTM->SetProfileInfo(AVP_PROFILE_UPDATER, pSett, *g_hGui, 0)) )
			return false;

		// proxy settings
		cBLSettings* pBlSett = (cBLSettings*)m_data.settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
		if( !pBlSett || !g_hTM || PR_FAIL(g_hTM->SetProfileInfo(AVP_PROFILE_PROTECTION, pBlSett, *g_hGui, 0)) )
			return false;

		g_hGui->ProcessEvent(cProfileAction::START_TASK, &cProfileAction(AVP_PROFILE_UPDATER), pItem, GUI_PROCEVT_PROCESS|GUI_PROCEVT_FORCESYNC);
		sbreak;
	send;

	return TBase::OnClicked(pItem);
}

class CCfgWizFinishTask : public CRootTaskBase
{
public:
	CCfgWizFinishTask(CRootItem * pRoot, cTaskManager * hTM, bool bNeedReboot, bool bReboot, bool bStartProduct, bool bSilent) :
		CRootTaskBase(pRoot, 0xdb0947b9), m_hTM(hTM),
		m_bNeedReboot(bNeedReboot), m_bReboot(bReboot), m_bStartProduct(bStartProduct), m_bSilent(bSilent)
	{
		if( m_hTM )
			g_hGui->AddRefObjectProxy(*m_hTM);
	}

	~CCfgWizFinishTask()
	{
		if( m_hTM )
			g_hGui->ReleaseObjectProxy(*m_hTM);
	}
	
	void do_work()
	{
		PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAVP\tGUI - CCfgWizFinishTask..."));

		if( m_bNeedReboot )
		{
			if( m_bReboot )
			{
				if( m_hTM && g_hGui->IsValidProxy && PR_SUCC(g_hGui->IsValidProxy(*m_hTM)) )
					m_hTM->Exit(cCloseRequest::cNone);
				else
					g_hGui->sysSendMsg(pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
				
				PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAVP\tGUI - sending reboot..."));
				g_hGui->sysSendMsg(pmc_REBOOT, pm_REBOOT_OS, NULL, NULL, NULL);
				PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAVP\tGUI - reboot ok"));
			}
			else
			{
				if( m_hTM && g_hGui->IsValidProxy && PR_SUCC(g_hGui->IsValidProxy(*m_hTM)) )
					m_hTM->Exit(cCloseRequest::cByUser);
				else
					g_hGui->sysSendMsg(pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
			}
		}
		else
		{
			if( m_bStartProduct )
			{
				PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAVP\tGUI - start product"));

				tDWORD dwFlags = cTM_INIT_FLAG_DEFAULT;
				if( PR_SUCC(g_hGui->IsSystemProcess(PR_PROCESS_LOCAL)) || m_bSilent )
				{
					PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAVP\tGUI - start gui by user"));
					CALL_SYS_SendMsg(g_root, pmc_REMOTE_CLOSE_REQUEST, pm_TO_RESTART_GUI, NULL, NULL, NULL);
					dwFlags |= cTM_INIT_FLAG_STARTGUI;
				}

				if( m_hTM )
					m_hTM->Init(dwFlags);
			}
			else
			{
				if( m_hTM && g_hGui->IsValidProxy && PR_SUCC(g_hGui->IsValidProxy(*m_hTM)) )
					m_hTM->Exit(cCloseRequest::cByUser);
				else
					g_hGui->sysSendMsg(pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
			}
		}

		PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAVP\tGUI - CCfgWizFinishTask done"));
	}

	cTaskManager * m_hTM;
	bool           m_bNeedReboot;
	bool           m_bReboot;
	bool           m_bStartProduct;
	bool           m_bSilent;
};

bool CConfigureWizard::OnClose(tDWORD& nResult)
{
	PR_TRACE((g_root, prtIMPORTANT, "gui\tAVP\tCConfigureWizard::OnClose..."));

	m_ExecFinder.Stop();

	if( m_bInitMode )
	{
		if( nResult == DLG_ACTION_CANCEL )
		{
			if( g_hTM && g_hGui->IsValidProxy && PR_SUCC(g_hGui->IsValidProxy(*g_hTM)) )
				g_hTM->Exit(cCloseRequest::cByUser);
			else
				g_hGui->sysSendMsg(pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
		}
		else
		{
			bool bNeedReboot = g_hGui->m_statistics && (g_hGui->m_statistics->m_State.m_UpdateState & eUpdateNeedReboot);
			
			if( !bNeedReboot && m_bStartProduct )
			{
				g_hGui->m_fShowTray = 1;
				g_hGui->ShowTray();
			}

			(new CCfgWizFinishTask(Root(), g_hTM,
				bNeedReboot, !!m_bReboot, !!m_bStartProduct, !!m_bSilent))->start(); 
		}
	}

	return TBase::OnClose(nResult);
}

bool CConfigureWizard::OnApply()
{
	if( (m_nPiwFlags & fSettings) || m_bExpress )
	{
		if( !m_bSilent && !g_hGui->CheckPassword(Item(), pwdc_SaveSettings) )
			return false;

		m_data.SetSettingsDirty(true, true);

/*		if( m_Reseter.IsProfileReset(AVP_PROFILE_ANTIHACKER) )
		{
			if( cAHFWSettings * pAhCfg = (cAHFWSettings *)m_data.settings(AVP_PROFILE_FIREWALL, cAHFWSettings::eIID) )
				pAhCfg->m_ProtectionLevel = (m_bInteractiveExtendedMode && m_bInteractiveAhLearn) ? _fwwmAskUser : _fwwmAllowNotFiltered;
		}*/

		if( m_Reseter.IsProfileReset(AVP_PROFILE_BEHAVIORMONITOR) )
		{
// 			if( cPDMSettings * pPdmCfg = (cPDMSettings *)m_data.settings(AVP_PROFILE_PDM, cPDMSettings::eIID) )
// 			{
// 				tBOOL bInteractiveInvader = (m_bInteractiveExtendedMode && m_bInteractiveInvader) ? cTRUE : cFALSE;
// 				for(tDWORD i = 0; i < pPdmCfg->m_Set.size(); i++)
// 				{
// 					cPDMSettings_Item& _item = pPdmCfg->m_Set[i];
// 					switch( _item.m_SetType )
// 					{
// 					case _eSet_Browser:    _item.m_bEnabled = bInteractiveInvader; break;
// 					//case _eSet_Invader:    _item.m_bEnabled = bInteractiveInvader; break;
// 					case _eSet_SetWndHook: _item.m_bEnabled = bInteractiveInvader; break;
// 					case _eSet_AntiKeyLog: _item.m_bEnabled = m_bInteractiveExtendedMode; break;
// 					}
// 				}
// 				
// 				pPdmCfg->m_bAppMonitoring_Enabled = !!(m_bInteractiveExtendedMode && m_bInteractiveIntegrity);
// 				pPdmCfg->m_bRegMonitoring_Enabled = !!(m_bInteractiveExtendedMode && m_bInteractivePdmRegistry);
// 			}
// 			CProfile * pRG2 = m_data.GetProfile(AVP_PROFILE_REGGUARD2);
// 			if( pRG2 && !m_bUpgrade && !m_bPervSettingsLoaded )
// 				pRG2->m_cfg.m_bEnabled = !!(m_bInteractiveExtendedMode && m_bInteractivePdmRegistry);
		}

		if( !m_bUpgrade && !m_bPervSettingsLoaded )
		{
			const cStringObj &strProductType = g_hGui->ExpandEnvironmentString("%ProductType%");
			bool bIsWksOrFs = strProductType == "wks" || strProductType == "fs";
			if( cTrafficMonitorSettings * pTmCfg = (cTrafficMonitorSettings *)m_data.settings(AVP_PROFILE_TRAFFICMONITOR, cTrafficMonitorSettings::eIID) )
				pTmCfg->m_nDecodeSSL = m_bInteractiveExtendedMode && !bIsWksOrFs ? dssl_AskMe : dssl_NO;
		}
		
		if( cBLSettings * pBlCfg = (cBLSettings *)m_data.settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID) )
			pBlCfg->m_Ins_InitMode = 0;

		if( !m_bEnableDns )
			g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_SRVMGR_REQUEST, NULL,
				&cSvrMgrRequest(L"Dnscache", SRVMGR_DISABLE), SER_SENDMSG_PSIZE);

		// На случай, елси где-то перебили настройки применим политику
		m_data.ApplyPolicy();
}

	return TBase::OnApply();
}

bool CConfigureWizard::ActivationBegin(bool bTrial)
{
	if( !m_bInitMode && !g_hGui->CheckPassword(*this, pwdc_Activate) )
		return false;

	if( !g_hGui->m_pLic )
	{
		MsgBoxErr(*this, &cGuiErrorInfo(errOBJECT_BAD_INTERNAL_STATE));
		return false;
	}
	
	cUserData _Data;
	_Data.m_bTrial = bTrial ? cTRUE : cFALSE;
		_Data.m_strCustomerId = m_strCustomerId;
		_Data.m_strCustomerPwd = m_strCustomerPassword;
	if( !bTrial )
		_Data.m_strKeyNum = m_strActivationCode;
	
	m_bConnected = cFALSE; 
	m_bSentCode = cFALSE;
	m_bRetrievedKey = cFALSE;
	Item()->UpdateData(true);

	cBLSettings *pSett = (cBLSettings *)m_data.settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
	if( pSett )
	{
		_Data.m_ProxySettings = pSett->m_ProxySettings;
		((CRootSink *)(Root()->m_pSink))->CryptDecrypt(_Data.m_ProxySettings.m_strProxyPassword, false);
	}

	PR_TRACE((g_hGui, prtIMPORTANT , "gui\tact\tActivation code: %S", _Data.m_strKeyNum.data()));

	return m_activation.Begin(_Data, *this);
}

void CConfigureWizard::InheritConfiguration(tDWORD nInheritTriggers)
{
	}
		
bool CConfigureWizard::InheritLic(bool bCheckOnly)
	{
	return true;
}

void CConfigureWizard::UpdateLicensingTriggers()
{
	if( !m_bInitMode || !g_hGui->m_pLic )
		return;

	m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers | (wzMaskActivationOnline));
	m_bActivationNeeded = cTRUE;
	
	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tUpdateLicensingTriggers()"));

	tERROR errCheck = errOK;
	while( errCheck = m_activation.CheckActiveKey(m_LicInfo) )
	{
		if( m_LicInfo.m_dwInvalidReason == ekirValid )
		{
			m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~(wzActivationBegin|wzActivationForm|wzActivationProgress|wzActivationChooseKey));
			m_bActivationNeeded = cFALSE;
			break;
		}

		cGuiLicCheckKeyInfo& pDiscard = m_KeysDiscarted.m_aKeys.push_back();
		pDiscard.m_LicInfo = m_LicInfo;
		pDiscard.m_errCheck = errCheck;
		
		tERROR err = g_hGui->m_pLic->RevokeKey(&m_LicInfo.m_KeyInfo.m_KeySerNum);
		PR_TRACE((g_hGui, prtIMPORTANT, "gui\tUpdateLicensingTriggers() RevokeKey result: %terr", err));
		if( PR_FAIL(err) )
			break;
		
		m_LicInfo = cCheckInfo();
	}

	if( m_bSilent )
		m_eTriggers = (enWisardTriggers)((tDWORD)m_eTriggers & ~(wzMaskActivation));
}

void CConfigureWizard::UpdateInheritTriggers()
{
}

/////////////////////////////////////////////////////////////////////////////
// CFolderExpander

void CopyToPlainVector(TrainFolderInfoList_t &aFolders, CMapiFolderTree &FolderTree)
{
	if(FolderTree.m_FolderState != TrainFolderInfo_t::FolderUndefined)
		aFolders.push_back(*(TrainFolderInfo_t *)&FolderTree);

	for(size_t i = 0; i < FolderTree.m_aChildren.size(); i++)
		CopyToPlainVector(aFolders, FolderTree.m_aChildren[i]);
}

tERROR CFolderExpander::Expand(void *pContext, CMapiFolderTree *pFolder, CItemBase *pItem)
{
	m_pContext = pContext;
	m_pFolder = pFolder;
	m_pItem = pItem;
//	асинхронный expand приводит к падениям в PSTPRX32 при expand-е IMAP папок
//	cThreadTaskBase::start(*this);
	do_work();
	return errOK;
}

void CFolderExpander::do_work()
{
	((CFolderTree *)m_pContext)->GetFolderInfoAsync(m_pFolder, m_pItem);
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

void CFolderTree::OnInited()
{
	Item()->AddStateIcon(Root()->GetIcon("mail"));

	ExpandItem(NULL);
}

bool CFolderTree::OnItemExpanding(cTreeItem * pItem, tUINT& nChildren)
{
	return ExpandItem(pItem);
}

bool CFolderTree::OnItemChecking(cTreeItem * pItem)
{
	CMapiFolderTree * pFolder = CFolderTree::GetFolder(pItem);
	if( pFolder && CanBeChecked(pFolder) )
	{
		pFolder->m_FolderState = pFolder->m_FolderState == TrainFolderInfo_t::FolderUndefined ? m_CheckedState : TrainFolderInfo_t::FolderUndefined;
		return true;
	}
	return false;
}

void CFolderTree::GetFolderInfo(CMapiFolderTree *pFolder)
{
//	асинхронный expand приводит к падениям в PSTPRX32 при expand-е IMAP папок

//	CItemBase *pOwner = Item()->GetOwner(true);
//	CItemBase *pLoopItem = Root()->CreateItem(NULL, NULL);
//	pLoopItem->m_pRoot = Root();
//	m_FolderExpander.Expand(this, pFolder, pLoopItem);
//	pOwner->Enable(false);
//	Root()->MessageLoop(pLoopItem);
//	pOwner->Enable(true);
//	pLoopItem->Destroy();

	m_FolderExpander.Expand(this, pFolder, NULL);
}

void CFolderTree::GetFolderInfoAsync(CMapiFolderTree *pFolder, CItemBase *pItem)
{
	if(NULL == m_pAS )
		return;
	FolderInfoList_t m_aFolders;
	if(PR_SUCC(m_pAS->GetFolderInfoList(pFolder == &m_Tree ? NULL : pFolder, &m_aFolders)))
		for(tDWORD i = 0; i < m_aFolders.size(); i++)
			pFolder->m_aChildren.push_back(m_aFolders[i]);

	if(pItem)
		pItem->m_pRoot = NULL;
}

bool CFolderTree::ExpandItem(cTreeItem * hItem)
{
	CMapiFolderTree * pFolder = &m_Tree;
	if( hItem )
	{
		if( Item()->GetChildItem(hItem) )
			return true;
		pFolder = GetFolder(hItem);
	}
	else
		hItem = NULL;
	
	std::vector<CMapiFolderTree> &aChildren = pFolder->m_aChildren;
	
	if( pFolder->bHasChildren && !aChildren.size() )
		GetFolderInfo(pFolder);
	
	for(size_t i = 0; i < aChildren.size(); i++)
	{
		pFolder = &aChildren[i];
		Item()->AppendItem(hItem, new TVIDATA(pFolder));
	}
	
	return true;
}

void CFolderTree::ApplyState(cTreeItem * hItem, eFolderState State, bool bApplyToChildren, bool bApplyToSiblings)
{
	if( !hItem )
		return;
	
	CMapiFolderTree *pFolder = GetFolder(hItem);
	if( pFolder && CanBeChecked(pFolder) && pFolder->m_FolderState != State )
		Item()->SetItemCheckState(hItem, (pFolder->m_FolderState = State) != TrainFolderInfo_t::FolderUndefined);
	
	if( bApplyToChildren )
		ApplyState(Item()->GetChildItem(hItem), State, bApplyToChildren, true);
	
	if( bApplyToSiblings )
		ApplyState(Item()->GetNextItem(hItem), State, bApplyToChildren, bApplyToSiblings);
}

void CFolderTree::OnUpdateItemProps(cTreeItem * pItem, tDWORD flags)
{
	CMapiFolderTree *pFolder = GetFolder(pItem);
	if( pFolder && pFolder->CanBeChecked() )
	{
		pItem->m_stateMask |= cTreeItem::fStateDisabled;
		pItem->m_state &= ~cTreeItem::fStateDisabled;
		if( !CanBeChecked(pFolder) )
			pItem->m_state |= cTreeItem::fStateDisabled;
	}
	else
	{
		pItem->m_stateMask |= cTreeItem::fIconStateMask;
		pItem->m_state &= ~cTreeItem::fIconStateMask;
		pItem->m_state |= cTreeItem::fIconStateUser0;
	}
}

CMapiFolderTree * CFolderTree::GetFolder(cTreeItem * hItem)
{
	return hItem ? ((TVIDATA *)hItem)->m_pFolder : NULL;
}

bool CFolderTree::CanBeChecked(CMapiFolderTree * pFolder)
{
	return pFolder->m_FolderState == TrainFolderInfo_t::FolderUndefined || pFolder->m_FolderState == m_CheckedState;
}


/////////////////////////////////////////////////////////////////////////////
// CWzTrainig

CWzTrainig::CWzTrainig() : TBase(&m_Status), m_pAS(NULL), m_bTraining(false)
{
	m_nActionsMask = DLG_ACTION_WIZARD;

#if defined (_WIN32)
	tERROR err = g_hTM->sysCreateObject((hOBJECT*)&m_pAS, IID_AS_TRAINSUPPORT, PID_AS_TRAINSUPPORT);
    if (PR_SUCC(err))
         err = m_pAS->set_pgTrainSupp_TM((hOBJECT)g_hTM);
    if (PR_SUCC(err))
       err = m_pAS->sysCreateObjectDone();            
#endif



	m_FolderExpander.init(g_hGui);
	m_pTree = new CMapiFolderTree;
};

CWzTrainig::~CWzTrainig()
{
	m_FolderExpander.wait_for_stop();
	m_FolderExpander.de_init();
	
	TwStop();
	
	delete m_pTree;
	
	if( m_pAS )
	m_pAS->sysCloseObject();
	
//	g_pTW = NULL;
}

void CWzTrainig::OnInited()
{
	if( CItemBase *pItem = GetItem("KeepExisting") )
		pItem->SetInt(1);
}

bool CWzTrainig::OnClicked(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	sswitch("Clear")
		CFolderTree * pTree = (CFolderTree *)pItem->m_pParent->GetSink("FolderTree", GUI_ITEMT_TREE);
		if( pTree )
			pTree->ApplyState(pTree->Item()->GetChildItem(NULL), TrainFolderInfo_t::FolderUndefined, true, true);
		sbreak;
	sswitch("Skip")
		TwSkip();
		sbreak;
	send;
	
	return TBase::OnClicked(pItem);
}

bool CWzTrainig::OnCanClose(bool bUser)
{
	if(m_bTraining)
		TwStop();
	else
		TwCancelResults();
	return true;
}

bool CWzTrainig::OnOk()
{
	TwApplyResults();
	return true;
}

bool CWzTrainig::OnSelect(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	scase("Page4")
		StartTraining();
		sbreak;
	send;

	return TBase::OnSelect(pItem);
}

void CWzTrainig::StartTraining()
{
	TrainSettings TrSett;
	CopyToPlainVector(TrSett.m_Folders, *m_pTree);

	m_nStartTime = GetTickCount();

	m_bTraining = true;
	if(m_pAS)
	m_pAS->TrainAsync(&TrSett);
}

void CWzTrainig::OnTrainComplete()
{
	m_bTraining = false;
	
	Item()->SetNextPage();
	Item()->UpdateView(0);
}

void CWzTrainig::OnTrainStatus(TrainStatus_t *pStatus)
{
	DWORD nCurTime = GetTickCount();

	pStatus->ulPercentCompleted = pStatus->ulMessagesCount ? pStatus->ulMessagesProcessed * 100 / pStatus->ulMessagesCount : pStatus->ulPercentCompleted;
	pStatus->ulTimeToFinish = pStatus->ulMessagesProcessed ? (nCurTime - m_nStartTime) * (pStatus->ulMessagesCount - pStatus->ulMessagesProcessed) / pStatus->ulMessagesProcessed / 1000 : pStatus->ulTimeToFinish;
	pStatus->bTrainComplete = cFALSE;

	m_ser->assign(*pStatus, true);
	UpdateData(true);
}

void CWzTrainig::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	TBase::OnEvent(nEventId, pData);

	if(nEventId == pm_AS_TRAIN_STATUS)
	{
		if(pData)
			OnTrainStatus((TrainStatus_t *)pData);
		else
			OnTrainComplete();
	}
}

void CWzTrainig::TwCancelResults()
{
	if(m_pAS)
	m_pAS->sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_CANCEL_RESULTS, 0, 0, 0);
}

void CWzTrainig::TwApplyResults()
{
	if(NULL == m_pAS)
		return;
	CItemBase *pItem;
	if(!!(pItem = GetItem("KeepExisting")) && !!pItem->GetInt())
		m_pAS->sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_MERGE_RESULTS, 0, 0, 0);
	else
		m_pAS->sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_APPLY_RESULTS, 0, 0, 0);
}

void CWzTrainig::TwStop()
{
	if(m_pAS)
	m_pAS->sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_CANCEL_TRAINING, 0, 0, 0);
}

void CWzTrainig::TwSkip()
{
	if(m_pAS)
	m_pAS->sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_SKIP_TRAINING, 0, 0, 0);
}
/*
void AntispamShowTrainingWizard()
{
#if defined (_WIN32)
	if( !g_pTW )
	{
		cAS_TrainSupport *pAS = NULL;
		if( PR_SUCC(g_hTM->sysCreateObjectQuick((hOBJECT*)&pAS, IID_AS_TRAINSUPPORT, PID_AS_TRAINSUPPORT)) )
			g_pRoot->LoadItem(new CWzTrainig(pAS), g_pTW, "OutlookPlugun.TrainingWizard", "TrainingWizard", LOAD_FLAG_DESTROY_SINK);
		else
			MsgBox(g_pRoot, "NoMailerPresent", NULL, MB_OK);
	}

	if( g_pTW )
		((CDialogItem *)g_pTW)->Activate();
#endif
}
*/

/////////////////////////////////////////////////////////////////////////////
// CMakeRescueDiskWizard
#if defined (_WIN32)
#define RCD_OPCODE_PREPAREDATA         1
#define RCD_OPCODE_TOOLEXECUTE         2
#define RCD_OPCODE_CLEANUP             3
#define RCD_OPCODE_ISOCREATE           4
#define RCD_OPCODE_ISOOPEN             5
#define RCD_OPCODE_PEBUILDER_NOT_FOUND 6
#define RCD_OPCODE_WINXP_NOT_FOUND     7
#define RCD_OPCODE_BARTPE_NOT_FOUND    8
#define RCD_OPCODE_CANT_CREATE_OUTPUT  9

#define pmc_RDISK_COMPLETE_STAGE			0xf6de2e9d
#define pmc_RDISK_COMPLETE_DETECT_STAGE		0xf6de2e9e

CMakeRescueDiskWizard::CMakeRescueDiskWizard() :
	TBase(this),
	CRootTaskBase(g_pRoot, 0xd59a602e),
	m_pRecorders(NULL),
	m_pMkIsoViewer(NULL),
	m_pBurnCdViewer(NULL)
{
	m_nActionsMask = DLG_ACTION_WIZARD;
	m_eTriggers = (enRdiskTriggers)(0xFFFFFFFF & ~wzRdisk_Progress);

	cStringObj &strProductType = g_hGui->ExpandEnvironmentString("%ProductType%");
	m_UseIAMT = strProductType == L"wks" || strProductType == L"fs";
}

CMakeRescueDiskWizard::~CMakeRescueDiskWizard()
{
	if( m_pMkIsoViewer )
		m_pMkIsoViewer->Destroy();
	if( m_pBurnCdViewer )
		m_pBurnCdViewer->Destroy();
}

void CMakeRescueDiskWizard::OnInit()
{
	m_eTriggers = (enRdiskTriggers)(m_eTriggers & ~(wzRdiskBartPE_BurnCd));
//		if(  )
//		{
//			m_eTriggers = (enRdiskTriggers)(m_eTriggers & ~(wzRdiskBartPE_BurnCd));
//			UpdateData(true);
//		}

	if( CComboItem * pMkIsoViewer = (CComboItem *)GetItem("MakeIsoViewer", "procoutput") )
	{
		CItemSink * pSink = pMkIsoViewer->m_pSink;
		if( m_pMkIsoViewer )
			pMkIsoViewer->AttachSink(m_pMkIsoViewer, false);
		else
		{
			m_pMkIsoViewer = (CStdinRedirectedSinkBase *)pSink;
			pMkIsoViewer->m_nFlags &= ~STYLE_DESTROYSINK;
		}
	}
	
	if( CComboItem * pBurnCdViewer = (CComboItem *)GetItem("BurnCdViewer", "procoutput") )
	{
		CItemSink * pSink = pBurnCdViewer->m_pSink;
		if( m_pBurnCdViewer )
			pBurnCdViewer->AttachSink(m_pBurnCdViewer, false);
		else
		{
			m_pBurnCdViewer = (CStdinRedirectedSinkBase *)pSink;
			pBurnCdViewer->m_nFlags &= ~STYLE_DESTROYSINK;
		}
	}
	
	if( CComboItem * pRecorders = (CComboItem *)GetItem(NULL, "cdrdrives") )
		m_pRecorders = (CStdinRedirectedSinkBase *)pRecorders->m_pSink;
	
	TBase::OnInit();

}

bool CMakeRescueDiskWizard::OnSelect(CItemBase * pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	scase("BartPE_Prepare_Progress")
		m_eOp = BartPE_Prepare, start();
		sbreak;
	
	scase("BartPE_MakeIso_Progress")
		m_eOp = BartPE_MakeIso;
		
		if( m_pMkIsoViewer )
		{
			m_strBartToolPath.check_last_slash();
			m_strBartDestPath.check_last_slash(false);

			m_err.m_nErr = errOK;
			m_err.m_nOperationCode = RCD_OPCODE_TOOLEXECUTE;
			m_err.m_strVal1 = L"mkisofs.exe";
			
			if( (m_strBartIsoFile.length() < 4) || (m_strBartIsoFile.compare(m_strBartIsoFile.length() - 4, cSTRING_WHOLE_LENGTH, ".iso", fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ) )
				m_strBartIsoFile.append(L".iso");
			
			cStringObj strParams;
			strParams.format(cCP_UNICODE, L"\"%S%S\" -iso-level 4 -force-uppercase -volid \"BartPEwithKAV6\" -b bootsect.bin -no-emul-boot -boot-load-size 4 -hide bootsect.bin -hide boot.catalog -o \"%S\" \"%S\"",
				m_strBartToolPath.data(), m_err.m_strVal1.data(),
				m_strBartIsoFile.data(), m_strBartDestPath.data());
			m_pMkIsoViewer->CreateProcess(strParams.data(), m_strBartToolPath.data(), pmc_RDISK_COMPLETE_STAGE);
		}
		sbreak;
	
	scase("BartPE_BurnCd_Progress")
		m_eOp = BartPE_BurnCd;

		if( m_pBurnCdViewer )
		{
			m_strBartToolPath.check_last_slash();
			
			m_err.m_nErr = errOK;
			m_err.m_nOperationCode = RCD_OPCODE_TOOLEXECUTE;
			m_err.m_strVal1 = L"cdrecord.exe";
			
			cStringObj strParams;
			strParams.format(cCP_UNICODE, L"\"%S%S\" dev=%S%S driveropts=burnfree gracetime=0 -force -s -v \"%S\"",
				m_strBartToolPath.data(), m_err.m_strVal1.data(), m_ScsiBurnDeviceId.data(),
				m_EraseDisk ? L" blank=fast" : L"", m_strBartIsoFile.data());
			m_pBurnCdViewer->CreateProcess(strParams.data(), m_strBartToolPath.data(), pmc_RDISK_COMPLETE_STAGE);
		}
		sbreak;
	send;

	return TBase::OnSelect(pItem);
}

bool CMakeRescueDiskWizard::OnNext()
{
	CItemBase * pItem = Item()->GetCurPage();
	if( !pItem )
		return false;
	
	m_eTriggers = (enRdiskTriggers)(m_eTriggers & ~(wzRdisk_Progress));

	sswitch(pItem->m_strItemId.c_str())
	scase("BartPE_Prepare")
		if( !ValidateData(BartPE_Prepare) )
			return false;

		if( m_pRecorders )
		{
			m_strBartToolPath.check_last_slash();
			
			cStringObj strParams;
			strParams.format(cCP_UNICODE, L"\"%S%S\" -scanbus",
				m_strBartToolPath.data(), L"cdrecord.exe");
			m_pRecorders->CreateProcess(strParams.data(), m_strBartToolPath.data(), pmc_RDISK_COMPLETE_DETECT_STAGE);
		}

		if( m_Prepare )
		{
			m_eTriggers = (enRdiskTriggers)(m_eTriggers|wzRdiskBartPE_Prepare_Progress);
			m_bProcessing = cTRUE;
			UpdateData(true);
		}
		sbreak;
	
	scase("BartPE_MakeIso")
		if( m_MakeIso )
		{
			if( !ValidateData(BartPE_MakeIso) )
				return false;

			m_eTriggers = (enRdiskTriggers)(m_eTriggers|wzRdiskBartPE_MakeIso_Progress);
			m_bProcessing = cTRUE;
			UpdateData(true);
		}
		else
		{
			if( m_pRecorders )
				m_pRecorders->CheckProcessOutput();
		}


		sbreak;
	
	scase("BartPE_BurnCd")
		if( m_BurnIso )
		{
			m_eTriggers = (enRdiskTriggers)(m_eTriggers|wzRdiskBartPE_BurnCd_Progress);
			m_bProcessing = cTRUE;
			UpdateData(true);
		}
		sbreak;
	
	scase("BartPE_MakeIso_Progress")
		if( m_pRecorders )
			m_pRecorders->CheckProcessOutput();
		sbreak;
	send;

	return TBase::OnNext();
}

bool CMakeRescueDiskWizard::OnPrev()
{
	CItemBase * pItem = Item()->GetCurPage();
	if( !pItem )
		return false;
	
	if( m_bProcessing && !CanInterruptProcess() )
		return false;

	m_eTriggers = (enRdiskTriggers)(m_eTriggers & ~(wzRdisk_Progress));
	return TBase::OnPrev();
}

#include <atlbase.h>
#include <setupapi.h>
#include <devguid.h>
#include <winioctl.h>
#include <ntddscsi.h>

#define CDB6GENERIC_LENGTH		   6
#define CDB10GENERIC_LENGTH 	   10
#define SCSIOP_INQUIRY			   0x12
#define SCSIOP_MODE_SENSE		   0x1A
#define MODE_PAGE_CAPABILITIES	   0x2A

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
	SCSI_PASS_THROUGH Spt;
	ULONG			  Filler;	   // realign buffers to double word boundary
	UCHAR			  SenseBuf[32];
	UCHAR			  DataBuf[512];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

struct SCSI_ADDRESS_LETTER
{
	CHAR         Letter;
	SCSI_ADDRESS Addr;
	cStrObj      Device;
	tString		 szVendorId;
	tString		 szProductId;
	tString		 szRevision;
	tString		 szDeviceName;
	tString		 szSCSIAddr;
};

//////////////////////////////////////////////////////////////////////

struct SCSI_HELPER 
{
	SCSI_HELPER()
	{
		m_hSetupApi = ::LoadLibrary("SETUPAPI.DLL");
		if (!m_hSetupApi)
			return;

		*(void**)&_SetupDiEnumDeviceInterfaces = GetProcAddress(m_hSetupApi, "SetupDiEnumDeviceInterfaces");
		*(void**)&_SetupDiDestroyDeviceInfoList = GetProcAddress(m_hSetupApi, "SetupDiDestroyDeviceInfoList");
		*(void**)&_SetupDiGetDeviceInterfaceDetailA = GetProcAddress(m_hSetupApi, "SetupDiGetDeviceInterfaceDetailA");
		*(void**)&_SetupDiGetClassDevsA = GetProcAddress(m_hSetupApi, "SetupDiGetClassDevsA");
		*(void**)&_SetupDiEnumDeviceInfo = GetProcAddress(m_hSetupApi, "SetupDiEnumDeviceInfo");
		*(void**)&_SetupDiGetDeviceRegistryPropertyA = GetProcAddress(m_hSetupApi, "SetupDiGetDeviceRegistryPropertyA");
	}

	~SCSI_HELPER() 
	{
		if (m_hSetupApi)
			FreeLibrary(m_hSetupApi);

		m_hSetupApi = NULL;
	}

	HMODULE m_hSetupApi;

	BOOL     (WINAPI * _SetupDiEnumDeviceInterfaces)(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, LPGUID InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
	BOOL     (WINAPI * _SetupDiDestroyDeviceInfoList)(HDEVINFO DeviceInfoSet);
	BOOL     (WINAPI * _SetupDiGetDeviceInterfaceDetailA)(HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize, PSP_DEVINFO_DATA DeviceInfoData);
	HDEVINFO (WINAPI * _SetupDiGetClassDevsA)(LPGUID ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags);
	BOOL     (WINAPI * _SetupDiEnumDeviceInfo)( IN HDEVINFO DeviceInfoSet, IN DWORD MemberIndex, OUT PSP_DEVINFO_DATA DeviceInfoData );
	BOOL     (WINAPI * _SetupDiGetDeviceRegistryPropertyA)( IN HDEVINFO DeviceInfoSet, IN PSP_DEVINFO_DATA DeviceInfoData, IN DWORD Property, OUT PDWORD PropertyRegDataType, OPTIONAL OUT PBYTE PropertyBuffer, IN DWORD PropertyBufferSize, OUT PDWORD RequiredSize );

protected:
		tERROR EnumCDRDevices_Item(HDEVINFO hIntDevInfo, DWORD nIndex, tPTR pDrivesList)
		{
			cVector<SCSI_ADDRESS_LETTER>& aDrives = *(cVector<SCSI_ADDRESS_LETTER> *)pDrivesList;
			
			DWORD   errWin;
			tString strDevice;
			tString strDeviceName;
			
			{
				tChunckBuff 			 buff;
				DWORD					 reqSize;
				
				SP_DEVICE_INTERFACE_DATA interfaceData; interfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
				if( !_SetupDiEnumDeviceInterfaces(hIntDevInfo, 0, (GUID *)&CdRomClassGuid, nIndex, &interfaceData) )
				{
					errWin = ::GetLastError();
					if( errWin == ERROR_NO_MORE_ITEMS )
						return errEND_OF_THE_LIST;
					PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiEnumDeviceInterfaces() error: 0x%08X", errWin));
					return errNOT_OK;
				}
				
				if( !_SetupDiGetDeviceInterfaceDetailA(hIntDevInfo, &interfaceData, NULL, 0, &reqSize, NULL) )
				{
					errWin = ::GetLastError();
					if( errWin != ERROR_INSUFFICIENT_BUFFER )
					{
						PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiGetDeviceInterfaceDetailA() error: 0x%08X", errWin));
						return errNOT_OK;
					}
				}
				
				PSP_DEVICE_INTERFACE_DETAIL_DATA interfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buff.get(reqSize, false);
				if( !interfaceDetailData )
					return errNOT_ENOUGH_MEMORY;
				
				interfaceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
				
				if( !_SetupDiGetDeviceInterfaceDetailA(hIntDevInfo, &interfaceData, interfaceDetailData,
					reqSize, &reqSize, NULL) )
				{
					errWin = ::GetLastError();
					PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiGetDeviceInterfaceDetailA(buf) error: 0x%08X", errWin));
					return errNOT_OK;
				}
				
				strDevice = interfaceDetailData->DevicePath;
				
				SP_DEVINFO_DATA devinfoData; devinfoData.cbSize = sizeof(devinfoData);
				if( !_SetupDiEnumDeviceInfo(hIntDevInfo, nIndex, &devinfoData) )
				{
					errWin = ::GetLastError();
					PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiEnumDeviceInfo() error: 0x%08X", errWin));
					return errNOT_OK;
				}
				
				if( !_SetupDiGetDeviceRegistryPropertyA(hIntDevInfo, &devinfoData, SPDRP_FRIENDLYNAME, NULL, NULL, 0, &reqSize) )
				{
					errWin = ::GetLastError();
					if( errWin != ERROR_INSUFFICIENT_BUFFER )
					{
						PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiGetDeviceRegistryPropertyA() error: 0x%08X", errWin));
						return errNOT_OK;
					}
				}
				
				strDeviceName.resize(reqSize);
				
				if( !_SetupDiGetDeviceRegistryPropertyA(hIntDevInfo, &devinfoData, SPDRP_FRIENDLYNAME, NULL, (BYTE *)&strDeviceName[0], strDeviceName.size(), NULL) )
				{
					errWin = ::GetLastError();
					PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiGetDeviceRegistryPropertyA(buf) error: 0x%08X", errWin));
					return errNOT_OK;
				}
			}
			
			HANDLE hDevice = ::CreateFile(strDevice.c_str(), GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
			
			if( hDevice == INVALID_HANDLE_VALUE )
			{
				errWin = ::GetLastError();
				PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: CreateFile(%s) error: 0x%08X", strDevice.c_str(), errWin));
				return errNOT_OK;
			}
			
			typedef enum _STORAGE_PROPERTY_ID { 
				StorageDeviceProperty = 0, 
				StorageAdapterProperty, 
				StorageDeviceIdProperty 
			} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID; 
			
			
			typedef enum _STORAGE_QUERY_TYPE { 
				PropertyStandardQuery = 0, 
				PropertyExistsQuery, 
				PropertyMaskQuery, 
				PropertyQueryMaxDefined 
			} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE; 

			#define IOCTL_STORAGE_QUERY_PROPERTY \
CTL_CODE(IOCTL_STORAGE_BASE, 0x0500,METHOD_BUFFERED, FILE_ANY_ACCESS)

#pragma pack(push)			
#pragma pack(4)
			typedef struct _STORAGE_PROPERTY_QUERY { 
				STORAGE_PROPERTY_ID  PropertyId; 
				STORAGE_QUERY_TYPE  QueryType; 
				UCHAR  AdditionalParameters[1]; 
			} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

			typedef enum _STORAGE_BUS_TYPE {
				BusTypeUnknown = 0x00,
					BusTypeScsi,
					BusTypeAtapi,
					BusTypeAta,
					BusType1394,
					BusTypeSsa,
					BusTypeFibre,
					BusTypeUsb,
					BusTypeRAID,
					BusTypeiSCSI,
					BusTypeSATA,
					BusTypeSAS,
					BusTypeMaxReserved = 0x7F
			} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;
			
			


			typedef struct _STORAGE_DEVICE_DESCRIPTOR {
				
				//
				// Sizeof(STORAGE_DEVICE_DESCRIPTOR)
				//
				
				ULONG Version;
				
				//
				// Total size of the descriptor, including the space for additional
				// data and id strings
				//
				
				ULONG Size;
				
				//
				// The SCSI-2 device type
				//
				
				UCHAR DeviceType;
				
				//
				// The SCSI-2 device type modifier (if any) - this may be zero
				//
				
				UCHAR DeviceTypeModifier;
				
				//
				// Flag indicating whether the device's media (if any) is removable.  This
				// field should be ignored for media-less devices
				//
				
				BOOLEAN RemovableMedia;
				
				//
				// Flag indicating whether the device can support mulitple outstanding
				// commands.  The actual synchronization in this case is the responsibility
				// of the port driver.
				//
				
				BOOLEAN CommandQueueing;
				
				//
				// Byte offset to the zero-terminated ascii string containing the device's
				// vendor id string.  For devices with no such ID this will be zero
				//
				
				ULONG VendorIdOffset;
				
				//
				// Byte offset to the zero-terminated ascii string containing the device's
				// product id string.  For devices with no such ID this will be zero
				//
				
				ULONG ProductIdOffset;
				
				//
				// Byte offset to the zero-terminated ascii string containing the device's
				// product revision string.  For devices with no such string this will be
				// zero
				//
				
				ULONG ProductRevisionOffset;
				
				//
				// Byte offset to the zero-terminated ascii string containing the device's
				// serial number.  For devices with no serial number this will be zero
				//
				
				ULONG SerialNumberOffset;
				
				//
				// Contains the bus type (as defined above) of the device.  It should be
				// used to interpret the raw device properties at the end of this structure
				// (if any)
				//
				
				STORAGE_BUS_TYPE BusType;
				
				//
				// The number of bytes of bus-specific data which have been appended to
				// this descriptor
				//
				
				ULONG RawPropertiesLength;
				
				//
				// Place holder for the first byte of the bus specific property data
				//
				
				UCHAR RawDeviceProperties[1];
				
			} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

#pragma pack(pop)
			STORAGE_PROPERTY_QUERY query;
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;
			
			UCHAR outBuf[512];
			ULONG returnedLength;
			BOOL status = DeviceIoControl(
				hDevice,                
				IOCTL_STORAGE_QUERY_PROPERTY,
				&query,
				sizeof( STORAGE_PROPERTY_QUERY ),
				&outBuf,                   
				512,                      
				&returnedLength,
				NULL                    
				);

			
			
			if ( !status ){
				
				PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: IOCTL_STORAGE_QUERY_PROPERTY(%s) error: 0x%08X", strDevice.c_str(), ::GetLastError()));
				::CloseHandle(hDevice);
				return errNOT_OK;
			}

			SCSI_ADDRESS_LETTER * pDrive = NULL;
		
			SCSI_ADDRESS scsi_address = { 0 };
			tCHAR strScsiId[100] = {0};
			{
				ULONG  returnedLength = 0;
				if( !::DeviceIoControl(hDevice, IOCTL_SCSI_GET_ADDRESS, NULL, 0, &scsi_address, sizeof(scsi_address), &returnedLength, FALSE) )
				{
					errWin = ::GetLastError();
					PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: DeviceIoControl(%s, IOCTL_SCSI_GET_ADDRESS) error: 0x%08X", strDevice.c_str(), errWin));
				}
				else
					_snprintf(strScsiId, countof(strScsiId), "%u,%u,%u", scsi_address.PortNumber, scsi_address.TargetId, scsi_address.Lun);
			}

			::CloseHandle(hDevice);
			
			{
				
				cStrObj strDeviceW = strDevice.c_str();
				
				for(unsigned int i = 0; i < aDrives.size(); i++)
				{
					SCSI_ADDRESS_LETTER& _Item = aDrives[i];
					
					if( strDeviceW.find(_Item.Device.data(), cCP_UNICODE, 0, cStrObj::whole, fSTRING_FIND_FORWARD|fSTRING_COMPARE_CASE_INSENSITIVE) != cStrObj::npos )
					{
						pDrive = &_Item;
						break;
					}
				}
			}

			if (pDrive)
			{
				PSTORAGE_DEVICE_DESCRIPTOR devDesc = (PSTORAGE_DEVICE_DESCRIPTOR) outBuf;
				
				PUCHAR p = (PUCHAR) outBuf;
				ULONG i;
				if ( devDesc->VendorIdOffset && p[devDesc->VendorIdOffset] )
					for ( i = devDesc->VendorIdOffset; p[i] != (UCHAR) NULL && i < returnedLength; i++ )
						pDrive->szVendorId+=p[i];
					
				if ( devDesc->ProductIdOffset && p[devDesc->ProductIdOffset] )
					for ( i = devDesc->ProductIdOffset; p[i] != (UCHAR) NULL && i < returnedLength; i++ )
						pDrive->szProductId += p[i];
					
					
				if ( devDesc->ProductRevisionOffset && p[devDesc->ProductRevisionOffset] )
					for ( i = devDesc->ProductRevisionOffset; p[i] != (UCHAR) NULL && i < returnedLength; i++ )
						pDrive->szRevision += p[i];


				pDrive->szVendorId.erase(std::remove(pDrive->szVendorId.begin(), pDrive->szVendorId.end(), ' '), pDrive->szVendorId.end());
				pDrive->szVendorId.erase(std::remove(pDrive->szVendorId.begin(), pDrive->szVendorId.end(), '\t'), pDrive->szVendorId.end());

				pDrive->szProductId.erase(std::remove(pDrive->szProductId.begin(), pDrive->szProductId.end(), ' '), pDrive->szProductId.end());
				pDrive->szProductId.erase(std::remove(pDrive->szProductId.begin(), pDrive->szProductId.end(), '\t'), pDrive->szProductId.end());

				pDrive->szRevision.erase(std::remove(pDrive->szRevision.begin(), pDrive->szRevision.end(), ' '), pDrive->szRevision.end());
				pDrive->szRevision.erase(std::remove(pDrive->szRevision.begin(), pDrive->szRevision.end(), '\t'), pDrive->szRevision.end());


				pDrive->szDeviceName += pDrive->Letter;
				pDrive->szDeviceName += ": ";
				pDrive->szDeviceName += strDeviceName;

				pDrive->szSCSIAddr = strScsiId;
					
			}
			
			return errOK;
	}

public:
	bool MapID2Letter(cVector<SCSI_ADDRESS_LETTER>& aDrives)
	{
		if( !_SetupDiEnumDeviceInterfaces || !_SetupDiDestroyDeviceInfoList ||
			!_SetupDiGetDeviceInterfaceDetailA || !_SetupDiGetClassDevsA ||
			!_SetupDiEnumDeviceInfo || !_SetupDiGetDeviceRegistryPropertyA )
		{
			PR_TRACE((g_root, prtIMPORTANT,
				"GUI(Win)::enum CD-R drives: one of SetupDi functions are not found: "
				"EnumDeviceInterfaces: 0x%p, DestroyDeviceInfoList: 0x%p, "
				"GetDeviceInterfaceDetailA: 0x%p, GetClassDevsA: 0x%p, "
				"SetupDiEnumDeviceInfo: 0x%p, SetupDiGetDeviceRegistryPropertyA: 0x%p",
				_SetupDiEnumDeviceInterfaces, _SetupDiDestroyDeviceInfoList,
				_SetupDiGetDeviceInterfaceDetailA, _SetupDiGetClassDevsA,
				_SetupDiEnumDeviceInfo, _SetupDiGetDeviceRegistryPropertyA));
			return false;
		}
		
		// HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\Scsi
		
		// HKEY_LOCAL_MACHINE\SYSTEM\MountedDevices
		// Values: \DosDevices\<x>:
		
		{
			CHAR  cLetter = 'A';
			DWORD nDrives = ::GetLogicalDrives();
			if( !nDrives )
			{
				PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: GetLogicalDrives() == 0"));
				return false;
			}
			
			CRegKey hMountedDevices;
			DWORD errWin = hMountedDevices.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\MountedDevices", KEY_READ);
			if( errWin == ERROR_SUCCESS )
			{
				for(int i = 0; i < 26; i++, cLetter++)
				{
					if( nDrives & (1 << i) )
					{
						{
							CHAR driveName[MAX_PATH]; _snprintf(driveName, countof(driveName), "%c:", cLetter);
							if( ::GetDriveType(driveName) != DRIVE_CDROM )
								continue;
						}
						
						CHAR deviceMountName[MAX_PATH]; _snprintf(deviceMountName, countof(deviceMountName), "\\DosDevices\\%c:", cLetter);
						
						DWORD nSize = 0;
						if( ::RegQueryValueExA(hMountedDevices, deviceMountName, 0, NULL, NULL, &nSize) != ERROR_SUCCESS )
							continue;
						
						tStringW strDevice; strDevice.resize(nSize/2);
						nSize = strDevice.size()*2;
						
						if( ::RegQueryValueExA(hMountedDevices, deviceMountName, 0, NULL, (LPBYTE)&strDevice[0], &nSize) != ERROR_SUCCESS )
							continue;
						
						if( strDevice.size() > 1 && strDevice[1] == '?' )
							strDevice[1] = '\\';
						
						SCSI_ADDRESS_LETTER& _Item = aDrives.push_back();
						_Item.Letter = cLetter;
						//_Item.Addr = scsi_address;
						_Item.Device = strDevice.c_str();
						
						tDWORD nPos = _Item.Device.find_last_of("#");
						if( nPos != cStrObj::npos )
							_Item.Device.erase(nPos);
					}
				}
			}
			else
				PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: OpenRegKey(SYSTEM\\MountedDevices) error: 0x%08X", errWin));
		}
		
		HDEVINFO hIntDevInfo = _SetupDiGetClassDevsA((GUID *)&CdRomClassGuid, NULL, NULL, (DIGCF_PRESENT|DIGCF_INTERFACEDEVICE));
		if( hIntDevInfo == INVALID_HANDLE_VALUE )
		{
			PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI(Win)::enum CD-R drives: SetupDiGetClassDevsA() == INVALID_HANDLE_VALUE"));
			return false;
		}
		
		for(int i = 0; PR_SUCC(EnumCDRDevices_Item(hIntDevInfo, i, (tPTR)&aDrives)); i++);
		
		_SetupDiDestroyDeviceInfoList(hIntDevInfo);
		
		return true;
		
	}
};

void CMakeRescueDiskWizard::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	TBase::OnEvent(nEventId, pData);

	switch(nEventId)
	{
	case pmc_RDISK_COMPLETE_STAGE:
	{
		if( m_pMkIsoViewer && (m_eOp == BartPE_MakeIso) )
		{
			m_err.m_nErr = m_pMkIsoViewer->m_err;
			m_err.m_nVal1 = (tPTR)m_pMkIsoViewer->m_nProcessExitCode;
		}
		
		if( m_pBurnCdViewer && (m_eOp == BartPE_BurnCd) )
		{
			m_err.m_nErr = m_pBurnCdViewer->m_err;
			m_err.m_nVal1 = (tPTR)m_pBurnCdViewer->m_nProcessExitCode;
		}
		
		bool bFail = PR_FAIL(m_err.m_nErr) || m_err.m_nVal1;
		if( bFail )
			MsgBoxErr(*this, &m_err, "RescueDiskErrorDescr");
		
		bool bNextPage = !bFail || (m_eOp == BartPE_Prepare);
		
		m_bProcessing = cFALSE;
		if( bNextPage )
			m_eTriggers = (enRdiskTriggers)(m_eTriggers & ~(wzRdisk_Progress));
		UpdateData(true);
		Item()->UpdateView();
		if( bNextPage )
			Item()->SetNextPage(bFail);
	} break;

	case pmc_RDISK_COMPLETE_DETECT_STAGE:
	{
		CComboItem * pRecorders = m_pRecorders ? (CComboItem *)m_pRecorders->m_pItem : NULL;
		if( pRecorders )
		{
			pRecorders->DeleteAllItems();

			tString str; m_pRecorders->GetResultText(str);
			
			SCSI_HELPER hlp;
			cVector<SCSI_ADDRESS_LETTER> aDrives;
			hlp.MapID2Letter(aDrives);
			size_t pos = 0;
			while((pos = str.find("Removable CD-ROM", pos))!=tString::npos)
			{
				size_t bpos = str.rfind("\r\n", pos);
				if (bpos!=tString::npos && bpos+2<pos)
				{
					tString disp(str.begin()+bpos+2, str.begin()+pos);
					size_t scsi_address = disp.find('\t');
					if (scsi_address!=tString::npos)
					{
						size_t scsi_address_end = disp.find('\t', scsi_address+1);
						if (scsi_address_end!=tString::npos)
						{
							tString scsi(disp.begin()+scsi_address+1, disp.begin()+scsi_address_end);

							size_t vendor_id = disp.find('\'', scsi_address_end);
							size_t vendor_id_end = disp.find('\'', vendor_id+1);
							tString szVendorID(disp.begin()+vendor_id+1, disp.begin()+vendor_id_end);
			
							szVendorID.erase(std::remove(szVendorID.begin(), szVendorID.end(), ' '), szVendorID.end());
							szVendorID.erase(std::remove(szVendorID.begin(), szVendorID.end(), '\t'), szVendorID.end());


							size_t product_id = disp.find('\'', vendor_id_end+1);
							size_t product_id_end = disp.find('\'', product_id+1);
							tString szProductID(disp.begin()+product_id+1, disp.begin()+product_id_end);

							szProductID.erase(std::remove(szProductID.begin(), szProductID.end(), ' '), szProductID.end());
							szProductID.erase(std::remove(szProductID.begin(), szProductID.end(), '\t'), szProductID.end());

							size_t revision = disp.find('\'', product_id_end+1);
							size_t revision_end = disp.find('\'', revision+1);
							tString szRevision(disp.begin()+revision+1, disp.begin()+revision_end);

							szRevision.erase(std::remove(szRevision.begin(), szRevision.end(), ' '), szRevision.end());
							szRevision.erase(std::remove(szRevision.begin(), szRevision.end(), '\t'), szRevision.end());


							tString szResult = disp;
							if (vendor_id != tString::npos)
								szResult.erase(szResult.begin(), szResult.begin()+vendor_id+1);

							int nMatch = 0;
							unsigned int match_i = 0;
							unsigned int i = 0;

							for(i = 0; i < aDrives.size(); i++)
							{
								SCSI_ADDRESS_LETTER& _Item = aDrives[i];
								
								if (!_Item.szSCSIAddr.empty() && _Item.szSCSIAddr == scsi)
								{
									++nMatch;
									match_i = i;
									break;
								}
							}

							if (nMatch == 0)
							{
								for(i = 0; i < aDrives.size(); i++)
								{
									SCSI_ADDRESS_LETTER& _Item = aDrives[i];
									
									if (((_Item.szVendorId.empty() || szVendorID.find(_Item.szVendorId)!=tString::npos) ||
										(szVendorID.empty() || _Item.szVendorId.find(szVendorID)!=tString::npos)) &&
										((_Item.szProductId.empty() || szProductID.find(_Item.szProductId)!=tString::npos) ||
										(szProductID.empty() || _Item.szProductId.find(szProductID)!=tString::npos)) &&
										((_Item.szRevision.empty() || szRevision.find(_Item.szRevision)!=tString::npos) ||
										(szRevision.empty() || _Item.szRevision.find(szRevision)!=tString::npos)))
									{
										++nMatch;
										match_i = i;
									}
								}
							}



							if (nMatch == 1)
								szResult = 	aDrives[match_i].szDeviceName;
							else
							{
								szResult.erase(std::remove(szResult.begin(), szResult.end (), '\''), szResult.end());
								szResult.erase(std::remove(szResult.begin(), szResult.end (), '\t'), szResult.end());
							}
							
							pRecorders->AddItem(Root()->LocalizeStr(TOR(cStrObj,()), szResult.c_str(), szResult.size()), cVar(scsi));
						}
					}
				}
				++pos;
			}

			str.erase(str.begin(), str.end());

			if (pRecorders->GetItemsCount())
			{
				m_eTriggers = (enRdiskTriggers)(m_eTriggers | wzRdiskBartPE_BurnCd);
				pRecorders->SetDef(0);
			}

			UpdateData(true);
		}

	}
	break;
	}
}

void CMakeRescueDiskWizard::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( m_pRecorders )
		m_pRecorders->OnTimerRefresh(nTimerSpin);
	
	TBase::OnTimerRefresh(nTimerSpin);
}

bool CMakeRescueDiskWizard::CanInterruptProcess()
{
	return false;
}

bool CMakeRescueDiskWizard::OnCanClose(bool bUser)
{
	if( !TBase::OnCanClose(bUser) )
		return false;
	
	if( !m_bProcessing )
		return true;

	return CanInterruptProcess();
}

bool CMakeRescueDiskWizard::OnClose(tDWORD& nResult)
{
	wait_for_stop();
	return TBase::OnClose(nResult);
}

void CMakeRescueDiskWizard::OnDestroy()
{
	if (m_pMkIsoViewer)
		m_pMkIsoViewer->DoneProxyProcess();

	m_pRecorders = NULL;
	TBase::OnDestroy();
}
void CMakeRescueDiskWizard::do_work()
{
	m_err.m_nErr = errOK;
	m_err.m_nOperationCode = 0;
	m_err.m_nVal1 = NULL;
	m_err.m_strVal1.clear();
	
	switch( m_eOp )
	{
	case BartPE_Prepare:
	{
		tERROR err = errOK;
		
		m_strBartToolPath.check_last_slash();
		
		cStringObj strExe = m_strBartToolPath;
		strExe += L"pebuilder.exe";

		cStringObj strParams = L"-auto -overwrite -exit";
		
		cStringObj strInfBak = m_strBartToolPath; strInfBak += L"input.avp6_bak.inf";
		cStringObj strInf = m_strBartToolPath; strInf += L"input.inf";

		if( PR_SUCC(err) )
			if( PR_FAIL(err = ApplyAVP6(BartPE_Prepare, false)) )
				PR_TRACE((g_hGui, prtIMPORTANT, "gui\tMakeRescueDisk - clean previous preparation failed"));
		
		if( PR_SUCC(err) )
			err = ApplyAVP6(BartPE_Prepare, true);

		if( PR_SUCC(err) )
		{
			err = PrCopy(cAutoString(strInf), cAutoString(strInfBak), 0);
			if( PR_FAIL(err) && err == errOBJECT_ALREADY_EXISTS )
				err = errOK;

			bool bShouldCreateFile = PR_FAIL(err);
			
			cAutoObj<cIO> pInfIo;
			err = g_hGui->sysCreateObject(pInfIo, IID_IO, PID_WIN32_NFIO);
			if( PR_SUCC(err) ) err = strInf.copy(pInfIo, pgOBJECT_FULL_NAME);
			if( bShouldCreateFile && PR_SUCC(err) )
				err = pInfIo->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS);
			if( PR_SUCC(err) ) err = pInfIo->sysCreateObjectDone();

			if( PR_SUCC(err) )
			{
				// Apply parameters to inf
				cAutoObj<cIniFile> pInfFile;
				err = g_hGui->sysCreateObject(pInfFile, IID_INIFILE, PID_INIFILE);
				if( PR_SUCC(err) ) err = pInfFile->set_pgm_hIO(pInfIo);
				if( PR_SUCC(err) ) err = pInfFile->sysCreateObjectDone();
				
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "buildiso",          "0");
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "burncd",            "0");
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "useAbsoluteOutput", "1");
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "fulloutdir",        m_strBartDestPath.c_str(cCP_ANSI));
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "outdir",            "");
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "source",            m_strXPCDPath.c_str(cCP_ANSI));
				if( PR_SUCC(err) ) err = pInfFile->SetValue("input", "isofile",           "");
				
				if( PR_SUCC(err) ) err = pInfFile->Commit();
			}
		}
		
		if( !PR_SUCC(err) )
		{
			m_err.m_nErr = err;
			m_err.m_nOperationCode = RCD_OPCODE_PREPAREDATA;
		}

		if( PR_SUCC(err) )
		{
			tDWORD nRes = 0;
		
			if( m_pMkIsoViewer && PR_SUCC(m_pMkIsoViewer->ShellExec(Item()->GetWindow(), strExe.data(), strParams.data(), m_strBartToolPath.data(), 0)) )
			{
				::WaitForSingleObject(m_pMkIsoViewer->GetChildHandle(), INFINITE);
				::GetExitCodeProcess(m_pMkIsoViewer->GetChildHandle(), (PDWORD)&nRes);
			}
			else
				err = errUNEXPECTED;

	
			if( !PR_SUCC(err) || nRes )
			{
				m_err.m_nErr = err;
				m_err.m_nOperationCode = RCD_OPCODE_TOOLEXECUTE;
				m_err.m_nVal1 = (tPTR)nRes;
				m_err.m_strVal1 = strExe;
			}
		}

		{
			bool bClean = PR_SUCC(err);
			tERROR errClean;
			tERROR errTmp;
			
			errClean = ApplyAVP6(BartPE_Prepare, false);
			
			errTmp = PrMoveFileEx(cAutoString(strInfBak), cAutoString(strInf), fPR_FILE_REPLACE_EXISTING);
			if( errTmp == errOBJECT_NOT_FOUND )
				errTmp = errOK;
			
			if( PR_SUCC(errClean) )
				errClean = errTmp;
			
			if( bClean && !PR_SUCC(errClean) )
			{
				m_err.m_nErr = errClean;
				m_err.m_nOperationCode = RCD_OPCODE_CLEANUP;
			}

			if( PR_SUCC(err) )
				err = errClean;
		}
		
		if( !PR_SUCC(err) )
			return;
	} break;
	
	}
}

void CMakeRescueDiskWizard::on_exit()
{
	m_pRoot->SendEvent(pmc_RDISK_COMPLETE_STAGE, NULL, 0, *this);
}

static tERROR RegSetValue(cIniFile *pRegFile, tSTRING sSectionName, tSTRING sValueName, cStringObj &strValue)
{
	cStringObj strVal; strVal.format(cCP_UNICODE, L"\"%S\"", strValue.data());
	return pRegFile->SetValue(sSectionName, sValueName, strVal.c_str(cCP_ANSI));
}

static BOOL CALLBACK EnumLangFunc (HANDLE hModule,
								   LPCTSTR lpszType,
								   LPCTSTR lpszName,
								   WORD wIDLanguage,
								   LONG_PTR lParam)
{
	return ((std::set<WORD>*)lParam)->insert(wIDLanguage), TRUE;
}

static bool IsModuleLangEqual(const cStringObj& cPath, LPCWSTR szwFile, WORD wLngID)
{
	cStringObj cFullPath = cPath;
	cFullPath.check_last_slash(true);
	cFullPath += szwFile;
	HMODULE hMod = LoadLibraryExW(cFullPath.data(), NULL, LOAD_LIBRARY_AS_DATAFILE);

	if(hMod == NULL)
		return false;

	std::set<WORD> lngIDs;
	EnumResourceLanguages(hMod, RT_VERSION, MAKEINTRESOURCE(1), (ENUMRESLANGPROC)EnumLangFunc, (LONG_PTR)&lngIDs);

	FreeLibrary (hMod);

	return lngIDs.find(wLngID)!=lngIDs.end();
}


tERROR CMakeRescueDiskWizard::ApplyAVP6(OpType eOp, bool bApply)
{
	tERROR err;

	if( !bApply )
	{
		if( eOp == BartPE_Prepare )
		{
			cStrObj strDstPath = m_strBartToolPath; strDstPath.add_path_sect(L"plugin" PATH_DELIMETER_WIDE L"avp", cCP_UNICODE);
			if( PR_FAIL(err = PrRemoveDir(cAutoString(strDstPath))) && err == errEND_OF_THE_LIST )
				err = errOK;

			strDstPath = m_strBartToolPath;
			strDstPath.add_path_sect(L"plugin" PATH_DELIMETER_WIDE L"lng_fix", cCP_UNICODE);
			if( PR_FAIL(err = PrRemoveDir(cAutoString(strDstPath))) && err == errEND_OF_THE_LIST )
				err = errOK;

			return err;
		}
		
		return errOK;
	}
	
	cStringObj strProductPath = g_hGui->ExpandEnvironmentString("%ProductRoot%");
	
	cStringObj strPackageFile = strProductPath;
	strPackageFile.add_path_sect(L"rescuecd.zip", cCP_UNICODE);

	if( eOp == BartPE_Prepare )
	{
		cStrObj strDstPath = m_strBartToolPath; strDstPath.add_path_sect(L"plugin" PATH_DELIMETER_WIDE L"avp", cCP_UNICODE);
		
		if( PR_FAIL(err = CopyFromPackage(strPackageFile, cStrObj(L"BartPE"), strDstPath)) )
		{
			PR_TRACE((g_hGui, prtIMPORTANT, "gui\tMakeRescueDisk::CopyFromPackage - %terr", err));
			m_err.m_strVal1 = strPackageFile;
			return err;
		}

		// Apply parameters to avp6.inf
		{
			cStrObj strInfFile = strDstPath; strInfFile.add_path_sect(L"avp.inf", cCP_UNICODE);
			
			cIOObj pInfFile(*g_hGui, cAutoString(strInfFile), fACCESS_RW);
			if( PR_FAIL(err = pInfFile.last_error()))
				return err;
			
			tQWORD nDataSize = 0;
			if( PR_FAIL(err = pInfFile->GetSize(&nDataSize, IO_SIZE_TYPE_EXPLICIT)))
				return err;
			
			tString strFileData; strFileData.resize((unsigned int)nDataSize);
			tDWORD nSizeProcessed;
			
			if( PR_FAIL(err = pInfFile->SeekRead(&nSizeProcessed, 0, &strFileData[0], strFileData.size())))
				return err;
			
			strFileData = g_hGui->ExpandEnvironmentString(strFileData.c_str()).c_str(cCP_ANSI);
			
			if( PR_FAIL(err = pInfFile->SeekWrite(&nSizeProcessed, 0, &strFileData[0], strFileData.size())))
				return err;
			if( PR_FAIL(err = pInfFile->SetSize(strFileData.size())))
				return err;
		}

		// Apply parameters to avp.reg
		if( PR_SUCC(err) )
		{
			cStrObj strRegFile = strDstPath; strRegFile.add_path_sect(L"avp.reg", cCP_UNICODE);
			cAutoObj<cIniFile> pRegFile;
			err = g_hGui->sysCreateObject(pRegFile, IID_INIFILE, PID_INIFILE);
			if( PR_SUCC(err) ) err = strRegFile.copy(pRegFile, pgOBJECT_FULL_NAME);
			if( PR_SUCC(err) ) err = pRegFile->sysCreateObjectDone();
			if( PR_SUCC(err) ) err = RegSetValue(pRegFile, "HKEY_LOCAL_MACHINE\\" VER_PRODUCT_REGISTRY_PATH "\\environment", "\"Localization\"",   g_hGui->ExpandEnvironmentString("%Localization%"));
			if( PR_SUCC(err) ) err = RegSetValue(pRegFile, "HKEY_LOCAL_MACHINE\\" VER_PRODUCT_REGISTRY_PATH "\\environment", "\"ProductVersion\"",   g_hGui->ExpandEnvironmentString("%ProductVersion%"));
			if( PR_SUCC(err) ) err = RegSetValue(pRegFile, "HKEY_LOCAL_MACHINE\\" VER_PRODUCT_REGISTRY_PATH "\\environment", "\"ProductStatus\"",   g_hGui->ExpandEnvironmentString("%ProductStatus%"));
 			if( PR_SUCC(err) ) err = pRegFile->Commit();

			if( PR_FAIL(err) )
				return err;
		}

		//bartPE russian symbols fixup
		if ( IsModuleLangEqual(m_strXPCDPath, L"i386\\NTDLL.dll", (WORD)0x0419) )
		{
			strDstPath = m_strBartToolPath; strDstPath.add_path_sect(L"plugin" PATH_DELIMETER_WIDE L"lng_fix", cCP_UNICODE);
			
			if( PR_FAIL(err = CopyFromPackage(strPackageFile, cStrObj(L"FixUP"), strDstPath)) )
			{
				PR_TRACE((g_hGui, prtIMPORTANT, "gui\tMakeRescueDisk::CopyFromPackage - %terr", err));
			}
		}

		if( !m_UseIAMT )
		{
			cStrObj strFile;
			strFile = strDstPath; strFile.add_path_sect(L"devcon.exe",  cCP_UNICODE); PrDeleteFile(cAutoString(strFile), 0);
			strFile = strDstPath; strFile.add_path_sect(L"iresol.inf",  cCP_UNICODE); PrDeleteFile(cAutoString(strFile), 0);
		}
	}
	
	return errOK;
}

#include <plugin/p_minizip.h>
tERROR CMakeRescueDiskWizard::CopyFromPackage(cStringObj& strPackageFile, cStringObj& strScrDir, cStringObj& strDestPath)
{
	tERROR err;
	
	cIOObj pPackage(*g_hGui, cAutoString(strPackageFile), fACCESS_READ);
	if( PR_FAIL(err = pPackage.last_error()) )
		return err;
	
	cAutoObj<cOS> pPackageOS;
	if( PR_FAIL(err = pPackage->sysCreateObject((hOBJECT*)&pPackageOS, IID_OS, PID_UNIVERSAL_ARCHIVER)) )
		return err;

	pPackageOS->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP);
	if( PR_FAIL(err = pPackageOS->sysCreateObjectDone() ) )
		return err;
	
	cAutoObj<cObjPtr> pPackagePtr;
	if( PR_FAIL(err = pPackageOS->sysCreateObjectQuick((hOBJECT*)&pPackagePtr, IID_OBJPTR, PID_UNIVERSAL_ARCHIVER)) )
		return err;

	if( PR_FAIL(err = PrCreateDir(cAutoString(strDestPath))) )
		return err;
	
	while( PR_SUCC(err = pPackagePtr->Next()) )
	{
		cStringObj strFile; strFile.assign(pPackagePtr, pgOBJECT_FULL_NAME);
		tDWORD l_pos = strFile.find_first_of(PATH_DELIMETER_STRING);
		if( l_pos == cStringObj::npos )
			continue;

		cStrObj strFileDir; strFileDir.assign(strFile, 0, l_pos);
		if( strFileDir.compare(strScrDir, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ )
			continue;

		cAutoObj<cIO> pSrcFile;
		if( PR_FAIL(err = pPackagePtr->IOCreate(&pSrcFile, cAutoString(strFile), fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
			return err;

		cStringObj strDstFile = strDestPath; strDstFile.add_path_sect(&strFile.data()[l_pos + 1], cCP_UNICODE);
		cIOObj pDstFile(*g_hGui, cAutoString(strDstFile), fACCESS_WRITE, fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_WRITE);
		if( PR_FAIL(err = pDstFile.last_error()) )
			return err;

		if( PR_FAIL(err = cIOObj::copy(pDstFile, pSrcFile)) )
			return err;
	}
		
	if( err != errEND_OF_THE_LIST )
		return err;
	
	return errOK;
}

bool IsFileExist(cStringObj &strFileOrFolder, LPCWSTR strFile = NULL)
{
	if( strFile && !wcscmp(strFile, L"*.*") )
	{
		cAutoObj<cObjPtr> pIoPtr;
		tERROR err = errOK;
		if( PR_SUCC(err) ) err = g_root->sysCreateObject((hOBJECT*)&pIoPtr, IID_OBJPTR, PID_NATIVE_FIO);
		if( PR_SUCC(err) ) err = strFileOrFolder.copy(pIoPtr, pgOBJECT_PATH);
		if( PR_SUCC(err) ) err = pIoPtr->sysCreateObjectDone();
		if( PR_SUCC(err) ) err = pIoPtr->propSetStr(NULL, pgMASK, (tPTR)"*.*");
		if( PR_SUCC(err) ) err = pIoPtr->Reset(cFALSE);
		if( PR_SUCC(err) ) err = pIoPtr->Next();
		return PR_SUCC(err);
	}
	
	if( strFile && *strFile )
	{
		cStringObj strPath = strFileOrFolder;
		strPath.check_last_slash();
		strPath += strFile;

		cIOObj pIo(*g_hGui, cAutoString(strPath), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		return PR_SUCC(pIo.last_error());
	}
	
	cIOObj pIo(*g_hGui, cAutoString(strFileOrFolder), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
	return PR_SUCC(pIo.last_error());
}

bool CMakeRescueDiskWizard::ValidateData(OpType eOp)
{
	switch( eOp )
	{
	case BartPE_Prepare:
	{
		if( !IsFileExist(m_strBartToolPath, L"pebuilder.exe") ||
			!IsFileExist(m_strBartToolPath, L"mkisofs.exe") ||
			!IsFileExist(m_strBartToolPath, L"cdrecord.exe") )
			return MsgBoxErr(*this, &cGuiErrorInfo(errNOT_FOUND, RCD_OPCODE_PEBUILDER_NOT_FOUND), "RescueDiskErrorDescr"), false;

		if( m_Prepare )
		{
			if( !IsFileExist(m_strXPCDPath, L"i386\\setupldr.bin") )
				return MsgBoxErr(*this, &cGuiErrorInfo(errNOT_FOUND, RCD_OPCODE_WINXP_NOT_FOUND), "RescueDiskErrorDescr"), false;

			if( IsFileExist(m_strBartDestPath, L"*.*") )	{
				return MsgBox(*this, "ForderNotEmpty", NULL, MB_YESNO|MB_ICONEXCLAMATION) == DLG_ACTION_YES;
			}

			cIOObj pIo(*g_hGui, cAutoString(m_strBartDestPath), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
			tERROR err = pIo.last_error();
			if( err != errIO_FOLDER_NOT_FILE )	{
				if( err == errOBJECT_NOT_FOUND )
					return MsgBox(*this, "ForderNotExist", NULL, MB_YESNO) == DLG_ACTION_YES;
				else
					return MsgBoxErr(*this, &cGuiErrorInfo(errUNEXPECTED, RCD_OPCODE_CANT_CREATE_OUTPUT), "RescueDiskErrorDescr"), false;
			}
		}
		else
		{
			if( !IsFileExist(m_strBartDestPath, L"i386\\setupldr.bin") )
				return MsgBoxErr(*this, &cGuiErrorInfo(errNOT_FOUND, RCD_OPCODE_BARTPE_NOT_FOUND), "RescueDiskErrorDescr"), false;
		}
	} break;
	
	case BartPE_MakeIso:
	{
		if( (m_strBartIsoFile.length() < 4) || (m_strBartIsoFile.compare(m_strBartIsoFile.length() - 4, cSTRING_WHOLE_LENGTH, ".iso", fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ) )
			m_strBartIsoFile.append(L".iso");

		if( m_MakeIso )
		{
			cIOObj pIso(*g_hGui, cAutoString(m_strBartIsoFile), fACCESS_RW, fOMODE_CREATE_ALWAYS);
			tERROR err = pIso.last_error();
			if( PR_FAIL(err) )
				return MsgBoxErr(*this, &cGuiErrorInfo(err, RCD_OPCODE_ISOCREATE), "RescueDiskErrorDescr"), false;
		}
		else
		{
			cIOObj pIso(*g_hGui, cAutoString(m_strBartIsoFile), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
			tERROR err = pIso.last_error();
			if( PR_FAIL(err) )
				return MsgBoxErr(*this, &cGuiErrorInfo(err, RCD_OPCODE_ISOOPEN), "RescueDiskErrorDescr"), false;
		}
	} break;
	}

	return true;
}

#endif //_WIN32

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
