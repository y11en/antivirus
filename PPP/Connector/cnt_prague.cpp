// -------------------------------------------
// (C) 2002 Kaspersky Lab 
// 
// \file	info.h
// \author	Mezhuev Pavel
// \date	21.01.2003 17:36:14
// \brief	Information about AVS component
// 
// -------------------------------------------

#define PR_IMPEX_DEF

#include "connector.h"
#include "common.h"
#include <version/ver_product.h>

hROOT g_root = NULL;
extern HMODULE g_hModule;

#define PRODUCT_EXE_FILE          _T("avp.exe")
#define PRODUCT_REPORT            "eventlog"

#define REGPATH_SETTINGS_ROOT     "HKEY_LOCAL_MACHINE\\" VER_PRODUCT_REGISTRY_PATH "\\connector"

#define IMPEX_IMPORT_LIB
#include <plugin/p_cryptohelper.h>
#include <plugin/p_base64.h>
#include <plugin/p_win32_reg.h>
#include <plugin/p_qb.h>
#include <common/updater.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <iface/e_loader.h> 
	#include <iface/e_ktrace.h>
IMPORT_TABLE_END

cSerRegistrar     * g_serializable_registrar_head = NULL;
CProductConnector * g_pProduct = NULL;

//////////////////////////////////////////////////////////////////////////

tERROR SerIsBasedOn(tDWORD nSerId, tDWORD nParentSerId)
{
	if( nSerId == nParentSerId )
		return true;

	cSerDescriptor *pDesc;
	tERROR err;
	if( PR_FAIL(err = g_root->FindSerializableDescriptor(&pDesc, nSerId)) )
		return err;

	nSerId = 0;
	for(const cSerDescriptorField* field = pDesc->m_fields; field->m_flags != SDT_FLAG_ENDPOINT; field++)
	{
		if( field->m_flags & SDT_FLAG_BASE )
		{
			nSerId = field->m_id;
			break;
		}
	}
	return nSerId == 0 ? errNOT_OK : SerIsBasedOn(nSerId, nParentSerId);
}

//////////////////////////////////////////////////////////////////////////
// CProductConnector

#define pmc_TIMER 0x416465fc //is a crc32 for "pmc_TIMER"

CProductConnector::CProductConnector() :
	cThreadPoolBase("Connector"),
	m_pNotifier(NULL),
//	m_pLock(NULL),
	m_pReg(NULL),
	m_pEnvironment(NULL),
	m_pReport(NULL),
	m_pBase64Codec(NULL),
	m_pReciever(NULL),
	m_pTM(m_Prague),
	m_bShutdown(false),
	m_pCrHelp(NULL),
	m_pQuarantine(NULL),
	m_pBackup(NULL)
{
	g_pProduct = this;
}

CProductConnector::~CProductConnector()
{
	g_pProduct = NULL;
}

bool CProductConnector::Init(KLCONN::Notifier *pNotifier)
{
	m_bShutdown = false;
	m_pNotifier = pNotifier;
	
	tERROR err = errOK;
	if( PR_SUCC(err) ) err = m_Prague.Init(g_hModule, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH);
	if( PR_SUCC(err) ) err = m_Prague.LoadProxyStub("pxstub.ppl");
	if( PR_SUCC(err) ) m_pTM.Init("TaskManager");
	if( PR_SUCC(err) ) err = cPrTracer::Init(g_hModule);
	if( PR_SUCC(err) ) err = m_Prague->ResolveImportTable(NULL, import_table, PID_APPLICATION);
	if( PR_SUCC(err) )
	{
		for(const cSerRegistrar* tmp = g_serializable_registrar_head; PR_SUCC(err) && tmp; tmp = tmp->m_next)
			err = m_Prague->RegisterSerializableDescriptor(tmp->m_dsc);
	}
	if( PR_SUCC(err) )
	{
		err = g_root->sysCreateObject((hOBJECT *)&m_pReg, IID_REGISTRY, PID_WIN32_REG);
		if( PR_SUCC(err) ) err = m_pReg->propSetStr(0, pgROOT_POINT, REGPATH_SETTINGS_ROOT);
		if( PR_SUCC(err) ) err = m_pReg->propSetBool(pgOBJECT_RO, cFALSE);
		if( PR_SUCC(err) ) err = m_pReg->sysCreateObjectDone();
		if( PR_FAIL(err) )
		{
			m_pReg->sysCloseObject();
			m_pReg = NULL;
		}
		cSerializable *pStr = this;
		g_root->RegDeserialize(&pStr, m_pReg, NULL, cSerializable::eIID);
	}
	if( PR_SUCC(err) )
		err = InitEnvironment();
	if( PR_SUCC(err) )
	{
		err = m_Prague->sysCreateObject((hOBJECT*)&m_pReciever, IID_MSG_RECEIVER);
		if( PR_SUCC(err) ) err = m_pReciever->propSetPtr(pgRECEIVE_PROCEDURE, MsgReceive);
		if( PR_SUCC(err) ) err = m_pReciever->propSetPtr(pgRECEIVE_CLIENT_ID, this);
		if( PR_SUCC(err) ) err = m_pReciever->sysCreateObjectDone();

		tMsgHandlerDescr hdl[] =
		{
			{ (hOBJECT)m_pReciever, rmhLISTENER, pmcPROFILE,                       IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ (hOBJECT)m_pReciever, rmhLISTENER, pmc_PRODUCT,                      IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ (hOBJECT)m_pReciever, rmhLISTENER, pmc_QBSTORAGE_REPLICATED,         IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ (hOBJECT)m_pReciever, rmhDECIDER,  pmc_TASKAHFW_GET_NAGENT_SETTINGS, IID_ANY, PID_AHFW,IID_ANY, PID_ANY },
			{ (hOBJECT)m_pReciever, rmhLISTENER, pmc_LICENSING,                    IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ (hOBJECT)m_pReciever, rmhLISTENER, pmc_UPDATER,                      IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		};
		memcpy(m_hndList, hdl, sizeof(hdl));
	}
//	if( PR_SUCC(err) ) err = m_Prague->sysCreateObjectQuick((cObject **)&m_pLock, IID_CRITICAL_SECTION);

	if( PR_SUCC(err) ) err = cThreadPoolBase::init(m_pReciever, 1, 500);
	if( PR_SUCC(err) ) err = cThreadTaskBase::start(*this);

	return PR_SUCC(err);
}

bool CProductConnector::Deinit()
{
	m_bShutdown = true;
	cThreadPoolBase::de_init();

	DeinitEnvironment();

//	close_object(m_pLock);
	close_object(m_pReport);
	close_object(m_pReciever);
	close_object(m_pReg);
	close_object(m_pBase64Codec);

	for(const cSerRegistrar* tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
		m_Prague->UnregisterSerializableDescriptor(tmp->m_dsc);

	cPrTracer::Deinit();
	
	return true;
}

tERROR CProductConnector::InitEnvironment()
{
	tERROR err = errOK;
	
	if( PR_SUCC(err) ) err = g_root->sysCreateObject((hOBJECT *)&m_pEnvironment, IID_REGISTRY, PID_WIN32_REG);
	if( PR_SUCC(err) )
	{
		if( PR_SUCC(err) ) err = m_pEnvironment->propSetStr(0, pgROOT_POINT, "HKEY_LOCAL_MACHINE\\" VER_PRODUCT_REGISTRY_PATH);
		if( PR_SUCC(err) ) err = m_pEnvironment->propSetBool(pgOBJECT_RO, cTRUE);
		if( PR_SUCC(err) ) err = m_pEnvironment->set_pgROOT_POINT("environment", 12);
		if( PR_SUCC(err) ) err = m_pEnvironment->sysCreateObjectDone();
		if( PR_FAIL(err) )
		{
			m_pEnvironment->sysCloseObject();
			m_pEnvironment = NULL;

			PR_TRACE((g_root, prtERROR, "CON\tCPragueLoader::InitEnvironment. error creating environment registry key (%terr)", err));
		}
		else
			err = PrSetEnviroment((hOBJECT)m_pEnvironment, NULL);
	}

	PR_TRACE((g_root, prtIMPORTANT, "CON\tCPragueLoader::InitEnvironment. done (%terr)", err));
	return err;
}

tERROR CProductConnector::DeinitEnvironment()
{
	if( m_pEnvironment )
		m_pEnvironment->sysCloseObject(), m_pEnvironment = NULL;

	return errOK;
}

void CProductConnector::OnConnect()
{
	m_pTM->sysRegisterMsgHandlerList(m_hndList, countof(m_hndList));

	m_pNotifier->ApplicationStateChange(KLCONN::APP_STATE_RUNNING, NULL);


	// report abnormally terminated tasks
	cProfileEx ProfileTree(0);
	if( GetProfile(NULL, &ProfileTree) )
	{
		for(unsigned i = 0; i < ProfileTree.m_aChildren.size(); i++)
		{
			cProfile *pProfile = ProfileTree.m_aChildren.at(i);
			if( pProfile->isTask() && pProfile->state() == PROFILE_STATE_FAILED && pProfile->m_lastErr == errTASK_ABNORMAL_TERMINATION_ONCE )
				NotifyStateChanged(pProfile->m_sName.data(), pProfile->m_sType.data(), pProfile->state(), pProfile->m_lastErr);
		}
	}
}

void CProductConnector::OnDisconnect()
{
	m_pNotifier->ApplicationStateChange(KLCONN::APP_STATE_INACTIVE, NULL);

	m_pTM->sysUnregisterMsgHandlerList(m_hndList, countof(m_hndList));

	if( m_pQuarantine )
		m_pTM->ReleaseService(0, (hOBJECT)m_pQuarantine), m_pQuarantine = NULL;
	
	if( m_pBackup )
		m_pTM->ReleaseService(0, (hOBJECT)m_pBackup), m_pBackup = NULL;

	if( m_pCrHelp )
		m_pTM->ReleaseService(0, (hOBJECT)m_pCrHelp), m_pCrHelp = NULL;

	close_object(m_pReport);
}

bool CProductConnector::CheckConnection()
{
	if( m_pTM.IsValid() )
		return true;

	if( m_pTM )
	{
		OnDisconnect();
		m_pTM.Detach();
		PR_TRACE((g_root, prtIMPORTANT, "CON\tCProductConnector::CheckConnection. disconnect"));
	}

	if( m_pTM.Attach() )
	{
		if( !CheckCanOperate() )
		{
			m_pTM.Detach();
			return false;
		}
		
		PR_TRACE((g_root, prtIMPORTANT, "CON\tCProductConnector::CheckConnection. connect"));
		OnConnect();
		return true;
	}

	return false;
}

bool CProductConnector::CheckCanOperate()
{
	cProfile Protection(cCfg::fSettings);
	if( PR_FAIL(m_pTM->GetProfileInfo(AVP_PROFILE_PROTECTION, &Protection)) )
		return false;

	cBLSettings *pBLSett = (cBLSettings *)Protection.settings();
	if( !pBLSett || !pBLSett->isBasedOn(cBLSettings::eIID) )
		return false;

	return !(pBLSett->m_Ins_InitMode & BL_INITMODE_INIT);
}

void CProductConnector::do_work()
{
	for(; !m_bShutdown; Sleep(1000))
	{
		if( CheckConnection() )
		{
			static tDWORD nTimeLastRefresh = GetTickCount();
			tDWORD nTimeCurrent = GetTickCount();

			if( nTimeCurrent - nTimeLastRefresh > 10000 )
			{
				nTimeLastRefresh = nTimeCurrent;

				OnEvent(pmc_TIMER, 0, NULL, NULL, NULL, NULL, NULL);
			}
		}
	}
	if( m_pTM )
	{
		OnDisconnect();
		m_pTM.Detach();
	}
}

//-------------------------------------------------------

bool CProductConnector::SetProductState(bool bActive)
{
	tERROR error;
	if( bActive )
	{
		error = m_Prague.CreateProcess(PR_SESSION_LOCAL, PRODUCT_EXE_FILE, "-s", 10000, cFALSE, NULL);
		        m_Prague.CreateProcess(PR_SESSION_ALL, PRODUCT_EXE_FILE, "-gui", 0, cFALSE, NULL);
	}
	else
	{
		if (m_pTM)
			m_pTM->Exit(0);
	}
	
	return PR_SUCC(error);
}

bool CProductConnector::CheckProductRunning()
{
	return m_pTM.IsValid();
}

//-------------------------------------------------------

tERROR CProductConnector::OnEvent(tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen)
{
	switch(msg_cls)
	{
	case pmcPROFILE:
		{
			cProfileBase *pProfile = (cProfileBase *)pbuff;
			if( !(pProfile && pProfile->isBasedOn(cProfileBase::eIID)) )
				break;

			if( !pProfile->m_nAdmFlags & PROFILE_ADMFLAG_ADMINABLE )
				break;

			switch(msg_id)
			{
			case pmPROFILE_STATE_CHANDED:
				{
					if( pProfile->isService() )
						break;
					
					if( pProfile->m_nState == PROFILE_STATE_STARTING ||
						pProfile->m_nState == PROFILE_STATE_PAUSING  ||
						pProfile->m_nState == PROFILE_STATE_STOPPING ||
						pProfile->m_nState == PROFILE_STATE_RESUMING )
						break;

					NotifyStateChanged(pProfile->m_sName.data(), pProfile->m_sType.data(), pProfile->state(), pProfile->m_lastErr);
					break;
				}

			case pmPROFILE_MODIFIED:
			case pmPROFILE_SETTINGS_MODIFIED:
				{
					const wchar_t *strProfile = pProfile->m_sName.data();

					PR_TRACE((g_root, prtIMPORTANT, "CON\tProfile settings changed %S", strProfile));

					if( pProfile->isTask() )
						m_pNotifier->TasksSettingsChange(&strProfile, 1);
					else
						m_pNotifier->SettingsChange(&strProfile, 1);
					
					break;
				}

			case pmPROFILE_CANT_START:
				{
					NotifyStateChanged(pProfile->m_sName.data(), pProfile->m_sType.data(), PROFILE_STATE_RUNNING, errOK);
					NotifyStateChanged(pProfile->m_sName.data(), pProfile->m_sType.data(), PROFILE_STATE_FAILED, pProfile->m_lastErr);
					break;
				}
			}
		}
		break;
	
	case pmc_PRODUCT:
		{
			switch(msg_id)
			{
			case pm_FULLSCAN_COMPLETED:
				if( pbuff && blen && *blen == sizeof(time_t) )
				{
					PR_TRACE((g_root, prtIMPORTANT, "CON\tFull scan completed"));
					time_t time = *(time_t *)pbuff;
					m_pNotifier->Event(KLEVP::c_szwGnrlEvent_FullScanCompleted, NULL, time);
				}
				break;
			}
		}
		break;

	case pmc_QBSTORAGE_REPLICATED:
		{
			if( pbuff && *blen == sizeof(cQbMessageReplicated) )
			{
				cQbMessageReplicated &MsgParams = *(cQbMessageReplicated *)pbuff;

				NotifyQbOperation(msg_id, MsgParams.nListID, MsgParams.nObjectID);
			}
		}
		break;

	case pmc_LICENSING:
		if( msg_id == pm_DATA_CHANGED )
		{
			PR_TRACE((g_root, prtIMPORTANT, "CON\tLicensing data changed"));
			m_pNotifier->ApplicationPropertiesChange();
		}
		break;

	case pmc_UPDATER:
		if( msg_id == pm_COMPONENT_UPDATED )
		{
			cUpdatedComponentName *updatedComp = (cUpdatedComponentName *)pbuff;
			if(blen == SER_SENDMSG_PSIZE && updatedComp && updatedComp->isBasedOn(cUpdatedComponentName::eIID) &&
				updatedComp->m_updatedComponentName.compare(cUPDATE_CATEGORY_AVS, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
			{
				PR_TRACE((g_root, prtIMPORTANT, "CON\tBases updated"));
				m_pNotifier->ApplicationPropertiesChange();
			}
		}
		break;
	
	case pmc_TASKAHFW_GET_NAGENT_SETTINGS:
		{
			cAHFWSettings *pSett = (cAHFWSettings *)pbuff;
			if( blen == SER_SENDMSG_PSIZE && pSett && pSett->isBasedOn(cAHFWSettings::eIID) )
			{
//				cAutoCS cs(m_pLock, true);
				pSett->m_PortRules = m_aRulesForNagent;
				return errOK_DECIDED;
			}
			return errOK;
		}
		break;
	
	case pmc_TIMER:
		{
			// enumerate all running tasks & notify if progress changed
			cProfileEx ProfileTree(0);
			if( !GetProfile(NULL, &ProfileTree) )
				break;

			for(unsigned i = 0; i < ProfileTree.m_aChildren.size(); i++)
			{
				cProfile *pProfile = ProfileTree.m_aChildren.at(i);
				int nPercent = GetCompletion(pProfile);
				if( nPercent != -1 )
					m_pNotifier->TaskCompletionChange(pProfile->m_sName.data(), nPercent);
			}
			
			// publish events (if any)
			PublishEvents();
		}
		break;
	};
	return errOK;
}

tERROR CProductConnector::MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen) 
{
	CProductConnector *pThis = (CProductConnector *)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
	return pThis->OnEvent(msg_cls, msg_id, obj, ctx, receive_point, pbuff, blen);
}

//-------------------------------------------------------

bool CProductConnector::GetProfileNames(KLSTD::AKWSTRARR &vecNames, bool bServives, wstrings *pAdditional)
{
	cProfileEx ProfileTree(0);
	if( !GetProfile(NULL, &ProfileTree) )
		return false;

	wstrings aProfileNames;
	if( bServives )
		aProfileNames.push_back(ProfileTree.m_sName.data());
	
	for(unsigned i = 0; i < ProfileTree.m_aChildren.size(); i++)
	{
		cProfileEx *pProfile = ProfileTree.m_aChildren.at(i);
		if( (pProfile->m_nAdmFlags & PROFILE_ADMFLAG_ADMINABLE) && (pProfile->m_eOrigin != cProfileBase::poTemporary) && (pProfile->isService() == bServives) )
			aProfileNames.push_back(pProfile->m_sName.data());
	}

	if( pAdditional )
		aProfileNames.insert(aProfileNames.end(), pAdditional->begin(), pAdditional->end());
	
	KLSTD::klwstrarr_t vec;
	KLSTD::ConvertVector2Array(aProfileNames, vec);
	vecNames = vec.detach();
	return true;
}

bool CProductConnector::AddProfile(cProfile *pProfile, tDWORD flags)
{
	if( !m_pTM )
		return false;

	tERROR err = m_pTM->CloneProfile(NULL, pProfile, flags, (cObject *)m_pReciever);
	
	PR_TRACE((g_root, prtIMPORTANT, "CNV\tCProductConnector::AddProfile. done (%terr). %S %S", err, pProfile->m_sName.data(), pProfile->m_sDescription.data()));
	
	return PR_SUCC(err);
}

bool CProductConnector::UpdateProfile(cProfile *pProfile, tDWORD flags)
{
	if( !m_pTM )
		return false;

	if( pProfile->isBasedOn(cProfileEx::eIID) )
	{
		if( ((cProfileEx *)pProfile)->count() )
		{
			cCfgEx CfgEx(pProfile->flags());
			if( PR_FAIL(CfgEx.assignProfile(*(cProfileEx *)pProfile, false)) )
				return false;

			return PR_SUCC(m_pTM->SetProfileInfo(pProfile->m_sName.c_str(cCP_ANSI), &CfgEx, (cObject *)m_pReciever, flags));
		}

		cProfile oProfile = *pProfile;
		return UpdateProfile(&oProfile, flags);
	}

	return PR_SUCC(m_pTM->SetProfileInfo(pProfile->m_sName.c_str(cCP_ANSI), pProfile, (cObject *)m_pReciever, flags));
}

bool CProductConnector::GetProfile(const tCHAR *strProfile, cProfile *pProfile)
{
	if( !m_pTM )
		return false;

	return PR_SUCC(m_pTM->GetProfileInfo(strProfile, pProfile));
}

bool CProductConnector::DeleteProfile(const tCHAR *strProfile)
{
	if( !m_pTM )
		return false;
	
	return PR_SUCC(m_pTM->DeleteProfile(strProfile, (cObject *)m_pReciever));
}

bool CProductConnector::SetProfileState(const tCHAR *strProfile, tTaskRequestState nState)
{
	if( !m_pTM )
		return false;
	
	return PR_SUCC(m_pTM->SetProfileState(strProfile, nState, NULL, 0));
}

bool CProductConnector::GetProfileState(const tCHAR *strProfile, tProfileState &nState)
{
	nState = PROFILE_STATE_UNK;

	if( !m_pTM )
		return false;
	
	cProfileBase Profile;
	if( PR_FAIL(m_pTM->GetProfileInfo(strProfile, &Profile)) )
		return false;

	nState = Profile.m_nState;
	return true;
}

bool CProductConnector::SetProfileInfo(const tCHAR *strProfile, cSerializable* info)
{
	if( !m_pTM )
		return false;

	return PR_SUCC(m_pTM->SetProfileInfo(strProfile, info, (cObject *)m_pReciever, 0));
}

bool CProductConnector::GetProfileInfo(const tCHAR *strProfile, cSerializable* info)
{
	if( !m_pTM )
		return false;

	return PR_SUCC(m_pTM->GetProfileInfo(strProfile, info));
}

LPCWSTR CProductConnector::GetProfileLocalizedName(cProfileBase *pProfile)
{
	if( !pProfile->m_sDescription.empty() )
		return pProfile->m_sDescription.data();

	wstrings_map::iterator it = m_mProfileNames.find(pProfile->m_sName.data());
	if( it == m_mProfileNames.end() )
	{
		cSerString m_strLocTaskName(pProfile->m_sName);
		if (m_pTM)
			m_pTM->sysSendMsg(pmc_LOCALIZE_INFO, pm_LOCALIZE_PROFILE_NAME, NULL, &m_strLocTaskName, SER_SENDMSG_PSIZE);

		it = m_mProfileNames.insert(wstrings_map::value_type(pProfile->m_sName.data(), m_strLocTaskName.m_str.data())).first;
	}
	return it->second.c_str();
}

void CProductConnector::ClearPolicyAttr(cProfile *pProfile)
{
	pProfile->m_cfg.m_dwLockedFields.Zero();
	pProfile->m_cfg.m_dwMandatoriedFields.Zero();
	cTaskSettings *pSett = pProfile->settings();
	if( pSett )
	{
		pSett->m_dwLockedFields.Zero();
		pSett->m_dwMandatoriedFields.Zero();
		
		cTaskSettings *pBLSett = (cTaskSettings *)pSett->m_pTaskBLSettings.ptr_ref();
		if( pBLSett && pBLSett->isBasedOn(cTaskSettings::eIID) )
		{
			pBLSett->m_dwLockedFields.Zero();
			pBLSett->m_dwMandatoriedFields.Zero();
		}
	}

	if( pProfile->isBasedOn(cProfileEx::eIID) )
	{
		cProfileEx &Profile = *(cProfileEx *)pProfile;
		for(tUINT i = 0; i < Profile.count(); i++)
			ClearPolicyAttr(Profile[i]);
	}
}

int CProductConnector::GetCompletion(cProfile *pProfile)
{
	if( pProfile->isService() )
		return -1;
	
	if( !(pProfile->m_nAdmFlags & PROFILE_ADMFLAG_ADMINABLE) )
		return -1;
	
	if( pProfile->m_nState != PROFILE_STATE_RUNNING )
		return -1;

	if( PR_FAIL(SerIsBasedOn(pProfile->m_nSerIdStatistics, cODSStatistics::eIID))
        && PR_FAIL(SerIsBasedOn(pProfile->m_nSerIdStatistics, cUpdaterStatistics::eIID)) )
    {
		return -1;
    }
	
	cTaskStatistics *pStat = GetStaistics(pProfile);
	if( !pStat )
		return -1;

	return pStat->m_nPercentCompleted;
}

cTaskStatistics *CProductConnector::GetStaistics(cProfile *pProfile)
{
	pProfile->m_statistics.init(pProfile->m_nSerIdStatistics);
	
	if( !GetProfileInfo( pProfile->m_sName.c_str(cCP_ANSI), pProfile->statistics()) )
		pProfile->m_statistics.clear();
	
	return (cTaskStatistics *)(cSerializable *)pProfile->m_statistics;
}

tERROR DeserializeReportRecord(cReport* pReport, tDWORD nPos, cSerializable *&pStruct, tDWORD nStructId = cSerializable::eIID)
{
	pStruct = NULL;
	tERROR err = errOK;

	tDWORD nSize = 0;
	if( PR_FAIL(err = pReport->GetRecord(&nSize, nPos, NULL, 0, NULL)) )
		return err;

	tPTR pBuffer = alloca(nSize);
	if( PR_FAIL(err = pReport->GetRecord(NULL, nPos, pBuffer, nSize, NULL)) )
		return err;

	if( PR_FAIL(err = g_root->StreamDeserialize(&pStruct, (tBYTE *)pBuffer, nSize, NULL)) )
		return err;

	if( !pStruct->isBasedOn(nStructId) )
	{
		g_root->DestroySerializable(pStruct); pStruct = NULL;
		return errPARAMETER_INVALID;
	}

	return errOK;
}

void CProductConnector::PublishEvents()
{
	if( !m_pTM )
		return;

	if( !m_pReport && PR_FAIL(m_pTM->GetTaskReport(0, PRODUCT_REPORT, (cObject *)m_pReciever, fACCESS_RW, &m_pReport)) )
		return;

	tDWORD nLastReport;
	if( PR_FAIL(m_pReport->GetRecordCount(&nLastReport)) )
		return;

	if( m_nLastReport == nLastReport )
		return;

	if( m_nLastReport > nLastReport )
	{
		m_tmLastReport = (tDWORD)-1;
		m_nLastReport = 0;
	}
	else
	{
		if( m_tmLastReport != (tDWORD)-1 && m_nLastReport )
		{
			cSerObj<cBLNotification> pEvent = NULL;
			if( PR_FAIL(::DeserializeReportRecord(m_pReport, m_nLastReport - 1, pEvent.ptr_ref(), cBLNotification::eIID)) ||
				pEvent->m_tmReport != m_tmLastReport )
			{
				m_tmLastReport = (tDWORD)-1;
				m_nLastReport = 0;
			}
		}
	}

	// „тобы не грузить CPU на 100% ограничимс€ 100 сообщени€ми за раз
	if (nLastReport - m_nLastReport > 100)
		nLastReport = m_nLastReport + 100;

	for(tDWORD nCurRec = m_nLastReport; nCurRec < nLastReport; nCurRec++)
	{
		cSerObj<cBLNotification> pEvent = NULL;
		if (PR_FAIL(::DeserializeReportRecord(m_pReport, nCurRec, pEvent.ptr_ref(), cBLNotification::eIID))
			|| !PublishEvent(pEvent.ptr_ref_type()))
		{
			if (m_pTM.IsValid())
				continue;
			else
				break;
		}

		m_nLastReport = nCurRec + 1;
		m_tmLastReport = pEvent->m_tmReport;
	}
	
	saveOneField(m_pReg, NULL, &m_nLastReport, sizeof(m_nLastReport));
	saveOneField(m_pReg, NULL, &m_tmLastReport, sizeof(m_tmLastReport));
}

tERROR CProductConnector::PublishKnownEvent(KLPAR::Params *pEvent, cBLNotification *pNotification, const wstring &strEventID)
{
	if(
		strEventID == KLEVP::c_szwGnrlEvent_VirusFound ||
		strEventID == KLEVP::c_szwGnrlEvent_SuspiciousFound ||
		strEventID == KLEVP::c_szwGnrlEvent_ObjectNotCured ||
		strEventID == KLEVP::c_szwGnrlEvent_PasswProtectedFound ||
		strEventID == KLEVP::c_szwGnrlEvent_ObjectCured ||
		strEventID == KLEVP::c_szwGnrlEvent_ObjectDeleted ||
		strEventID == KLEVP::c_szwGnrlEvent_Quarantined
		)
	{
		cDetectObjectInfo *pData = (cDetectObjectInfo *)pNotification->m_pData.ptr_ref();
		if( !pData || !pData->isBasedOn(cDetectObjectInfo::eIID) )
			return errPARAMETER_INVALID;

		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par2, StringValue, pData->m_strObjectName.data());
		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par5, StringValue, pData->m_strDetectName.data());
		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par7, StringValue, pData->m_strUserName.data());
		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par8, StringValue, __itow10(PragueDetectType2CscDetectType(pData->m_nDetectType)));
		
		//cStringObj strObjectID;
		//strObjectID.format(cCP_UNICODE, L"%08x", 0);
		//REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par6, StringValue, strObjectID.data());

		pData->m_strObjectName.clear();
		return errOK;
	}
	else
	if( strEventID == KLEVP::c_szwGnrlEvent_AttackDetected )
	{
		cIDSEventReport *pData = (cIDSEventReport *)pNotification->m_pData.ptr_ref();
		if( !pData || !pData->isBasedOn(cIDSEventReport::eIID) )
			return errPARAMETER_INVALID;

		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par1, StringValue, pData->m_AttackDescription.data());
		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par2, StringValue, __itow10(pData->m_Proto));

        if (pData->m_AttackerIP.IsV4())
        {
            REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par3, StringValue, __ultow10(pData->m_AttackerIP.m_V4));
        }
        else if (pData->m_AttackerIP.IsV6())
        {
            REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par3, StringValue, __ultow10(0));
        }
        //TODOv6: Put IPv6 address to the appropriate parameter

		REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par4, StringValue, __itow10(pData->m_LocalPort));
		return errOK;
	}
	
	return errNOT_FOUND;
}

// pack struct pNotification to base-64 string & save to 'c_er_par1', ... 'c_er_par9'
// each of 'c_er_parx' can contain string of 512-symblols max
tERROR CProductConnector::PackEventStruct(KLPAR::Params *pEvent, cBLNotification *pNotification, const wchar_t **pFreeParams)
{
	tERROR err = errOK;

	const tDWORD nPackedStrParamSize = 512 * sizeof(wchar_t) - sizeof(wchar_t);
	const tDWORD nStrParamSize = 512 * sizeof(wchar_t) * 6 / 8 - sizeof(wchar_t);

	tDWORD nPackedEventMaxSize = 0;
	tDWORD nEventMaxSize = 0;
	for(const wchar_t **strParamIt = pFreeParams; *strParamIt; strParamIt++)
	{
		nEventMaxSize += nStrParamSize;
		nPackedEventMaxSize += nPackedStrParamSize;
	}

	tDWORD buffUsed = 0;
	tBYTE *buff = (tBYTE *)alloca(nEventMaxSize);
	err = ::g_root->StreamSerialize(pNotification, SERID_UNKNOWN, buff, nEventMaxSize, &buffUsed);
	if( PR_FAIL(err) )
	{
		PR_TRACE((g_root, prtERROR, "CON\tCProductConnector::PublishEvent. Can't serialize event. %terr", err));
		return err;
	}

	tDWORD buff64Used = 0;
	tBYTE *buff64 = (tBYTE *)alloca(nPackedEventMaxSize);
	err = m_pBase64Codec->Encode(&buff64Used, buff, buffUsed, buff64, nPackedEventMaxSize);
	if( PR_FAIL(err) )
	{
		PR_TRACE((g_root, prtERROR, "CON\tCProductConnector::PublishEvent. Can't pack event to base64 format. %terr", err));
		return err;
	}

	tDWORD nSize = buff64Used, nPos = 0;
	for(const wchar_t **strParamIt = pFreeParams; nSize && *strParamIt; strParamIt++)
	{
		char strFrame[nPackedStrParamSize + sizeof(wchar_t)];
		tDWORD nFrameSize = nSize > nPackedStrParamSize ? nPackedStrParamSize : nSize;
		memcpy(strFrame, buff64 + nPos, nFrameSize);
		char *pTerm = strFrame + nFrameSize;
		if( nFrameSize & 1 )
			*pTerm++ = 'A';
		*(wchar_t *)pTerm = 0;
		nSize -= nFrameSize;
		nPos += nFrameSize;

		ADD_PARAMS_VALUE(pEvent, *strParamIt, StringValue, (wchar_t *)strFrame);
	}
	return errOK;
}

bool CProductConnector::PublishEvent(cBLNotification *pNotification)
{
	if( !pNotification )
		return false;

	if( !m_pBase64Codec && PR_FAIL(g_root->sysCreateObjectQuick((cObject **)&m_pBase64Codec, IID_DATACODEC, PID_BASE64)) )
		return false;

	PParams pEvent;	KLPAR_CreateParams(&pEvent);
	wstring &strEventType = GetEventType(pNotification->m_EventID, pNotification->m_Severity);

	cBLLocalizedNotification LocNot; *(cBLNotification *)(&LocNot) = *pNotification;
	if (errOK_DECIDED != m_pTM->sysSendMsg(pmc_LOCALIZE_INFO, pm_LOCALIZE_NOTIFICATION, NULL, &LocNot, SER_SENDMSG_PSIZE))
		return false;

	tERROR err = PublishKnownEvent(pEvent, pNotification, strEventType);
	if( PR_FAIL(err) && err != errNOT_FOUND )
		return false;
	
	if( PR_FAIL(PackEventStruct(pEvent, pNotification, GetEventFreeParams(strEventType))) )
		return false;

	ADD_PARAMS_VALUE(pEvent, KLEVP::c_er_locid,                   IntValue,    EVENT_KAV6_LOCID);
	ADD_PARAMS_VALUE(pEvent, KLEVP::c_er_severity,                IntValue,    PragueSeveriry2CscSeveriry(pNotification->m_Severity));
	ADD_PARAMS_VALUE(pEvent, KLEVP::c_er_descr,                   StringValue, LocNot.m_sEventText.data());
	ADD_PARAMS_VALUE(pEvent, KLEVP::c_er_event_type_display_name, StringValue, LocNot.m_sEventType.data());
	ADD_PARAMS_VALUE(pEvent, KLEVP::c_er_task_display_name,       StringValue, LocNot.m_sTaskName.data());

	PR_TRACE((g_root, prtIMPORTANT, "CON\tCProductConnector::PublishEvent. successed. %S -- %S -- %S -- %S", strEventType.c_str(), LocNot.m_sTaskName.data(), LocNot.m_sEventType.data(), LocNot.m_sEventText.data()));

	m_pNotifier->Event(strEventType.c_str(), pEvent, pNotification->m_tmReport);

	return true;
}

//-------------------------------------------------------

bool CProductConnector::CryptDecrypt(cStringObj &str, tDWORD bCrypt, tBOOL bRecrypt)
{
	if( !m_pCrHelp && m_pTM.IsValid() )
        m_pTM->GetService(0, NULL, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&m_pCrHelp);

	if( !m_pCrHelp )
		return false;

	if( !str.length() )
		return true;

	return PR_SUCC(m_pCrHelp->CryptPassword(&cSerString(str), (bCrypt ? 0 : PSWD_DECRYPT)|(bRecrypt ? PSWD_RECRYPT:0)));
}

#define errTASK_WAS_STOPPED 0x80380fbb //is a crc32

void CProductConnector::NotifyStateChanged(const wchar_t *strProfile, const wchar_t *strType, tProfileState nState, tERROR err)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tCProductConnector::NotifyStateChanged. Profile state changed profile: %S, type: %S, state: %08x, err: %08x", strProfile, strType, nState, err));
	
	if( !wcscmp(strProfile, AVP_PROFILE_PROTECTION_L) )
		m_pNotifier->RtpStateChanges(PragueProfileState2RtpState(nState), err);
	else
	{
		PParams pEvent;
		if( nState == TASK_STATE_STOPPED )
			err = errTASK_WAS_STOPPED;
		else if( nState != TASK_STATE_FAILED && PR_FAIL(err) )
			err = errOK;
		KLSTD_USES_CONVERSION;
		if( PR_FAIL(err) || (!wcscmp(strType, KLSTD_A2CW(AVP_TASK_UPDATER)) && err != errOK) )
		{
			KLPAR_CreateParams(&pEvent);
			REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_locid, IntValue, EVENT_KAV6_LOCID);
			
			tWCHAR strPar1[50]; swprintf(strPar1, L"%08x", err);
			REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par1, StringValue, strPar1);
			REPLACE_PARAMS_VALUE(pEvent, KLEVP::c_er_par2, StringValue, strType);
		}
		m_pNotifier->TaskStateChange(strProfile, PragueProfileState2CscTaskState(nState), pEvent);
	}
}

void CProductConnector::NotifyQbOperation(tDWORD nMsgID, tDWORD nListID, tOBJECT_ID nObjectID)
{
	const wchar_t* strEventType = NULL;
	switch(nMsgID)
	{
	case pm_QB_IO_DELETE:   strEventType = KLCONN::c_ev_ListItemDeleted;  break;
	case pm_QB_IO_STORE:    strEventType = KLCONN::c_ev_ListItemAdded;    break;
	case pm_QB_IO_MODIFIED: strEventType = KLCONN::c_ev_ListItemModified; break;
	}
	if( !strEventType )
		return;
	
	const wchar_t *strListName = NULL;
	switch(nListID)
	{
	case cQB_OBJECT_ID_QUARANTINE: strListName = KLCONN_NLST::QUARANTINE_LIST_NAME; break;
	case cQB_OBJECT_ID_BACKUP:     strListName = KLCONN_NLST::BACKUP_LIST_NAME;     break;
	}
	if( !strListName )
		return;

	wstring &strObjId = CNetworkListImpl::id2str(nObjectID);

	PR_TRACE((g_root, prtIMPORTANT, "CNV\tCProductConnector::OnEvent. QB notification. storage: %S, action: %S, object %S", strListName, strEventType, strObjId.c_str()));

	PParams pEvent;
	KLPAR_CreateParams(&pEvent);
	
	ADD_PARAMS_VALUE(pEvent, KLCONN::c_szwListName, StringValue, strListName);
	ADD_PARAMS_VALUE(pEvent, KLCONN::c_szwItemId,   StringValue, strObjId.c_str());
	
	m_pNotifier->Event(strEventType, pEvent);
}
 
//-------------------------------------------------------
// TasksSynchronizer::SendTaskAction

void CProductConnector::SendTaskAction(const wchar_t *szwTaskId, KLCONN::TskAction nAction)
{
	tTaskRequestState newState = TASK_REQUEST_RUN;
	switch(nAction)
	{
	case KLCONN::TSK_ACTION_START:
	case KLCONN::TSK_ACTION_RESUME:  newState = TASK_REQUEST_RUN;   break;
	case KLCONN::TSK_ACTION_STOP:    newState = TASK_REQUEST_STOP;  break;
    case KLCONN::TSK_ACTION_SUSPEND: newState = TASK_REQUEST_PAUSE; break;
	}

	tERROR err = errNOT_SUPPORTED;
	if( m_pTM )
	{
		KLSTD_USES_CONVERSION;
		err = m_pTM->SetProfileState(KLSTD_W2CA(szwTaskId), newState, NULL, 0);
	}
	PR_TRACE((g_root, prtIMPORTANT, "CON\tCProductConnector::SendTaskAction. %S, requested state %08x, %terr", szwTaskId, newState, err));
}

//-------------------------------------------------------

cQB *CProductConnector::QbGetService(const wchar_t *szwListName)
{
	if( m_pTM )
	{
		if( !wcscmp(szwListName, KLCONN_NLST::QUARANTINE_LIST_NAME) )
		{
			if( !m_pQuarantine )
			{
				tERROR err = m_pTM->GetService(0, (cObject *)m_pReciever, sid_TM_QUARANTINE, (hOBJECT*)&m_pQuarantine);
				if( !m_pQuarantine )
					PR_TRACE((g_root, prtDANGER, "CON\nCProductConnector::QbGetService. Can't get sid_TM_QUARANTINE service: %terr", err));
			}
			return m_pQuarantine;
		}
		if( !wcscmp(szwListName, KLCONN_NLST::BACKUP_LIST_NAME) )
		{
			if( !m_pBackup )
			{
				tERROR err = m_pTM->GetService(0, (cObject *)m_pReciever, sid_TM_BACKUP, (hOBJECT*)&m_pBackup);
				if( !m_pBackup )
					PR_TRACE((g_root, prtDANGER, "CON\nCProductConnector::QbGetService. Can't get sid_TM_BACKUP service: %terr", err));
			}
			return m_pBackup;
		}
	}
	return NULL;
}

void CProductConnector::QbGetItemsList(cQB *pQb, KLSTD::klwstrarr_t &vecItemsIds, tDWORD nItemsStartPos, tDWORD nItemsCount)
{
	tDWORD nCount = 0;
	if( PR_SUCC(pQb->GetCount(&nCount, cCOUNT_TOTAL|cCOUNT_REFRESH)) )
	{
		wstrings aObjectIDs;
		for(; nItemsStartPos < nCount && nItemsCount; nItemsStartPos++, nItemsCount--)
		{
			tOBJECT_ID nObjID;
			if( PR_SUCC(pQb->GetObjectByIndex(NULL, &nObjID, nItemsStartPos, cFALSE, NULL)) )
				aObjectIDs.push_back(CNetworkListImpl::id2str(nObjID));
		}
		KLSTD::ConvertVector2Array(aObjectIDs, vecItemsIds);
	}
}

bool CProductConnector::QbScan(cQB *pQb, const KLSTD::AKWSTRARR &vecItemsIds)
{
	KLSTD::klwstrarr_t aAllQbObjects;

	const KLSTD::AKWSTRARR *pItemsIds = &vecItemsIds;

	if( !pItemsIds->m_nwstr )
	{
		QbGetItemsList(pQb, aAllQbObjects);
		pItemsIds = &aAllQbObjects.inref();
	}

	if( !m_pTM )
		return false;
	
//  incorrect
	cProfile qbProfile;
	if( PR_FAIL(m_pTM->CloneProfile(AVP_PROFILE_SCANQUARANTINE, &qbProfile, fPROFILE_TEMPORARY, (cObject *)m_pReciever)) )
		return false;

	cStrBuff &strProfileName = qbProfile.m_sName.c_str(cCP_ANSI);

	if( !qbProfile.settings() )
		return false;
	cScanObjects *pSett = (cScanObjects*)qbProfile.settings();
	pSett->m_nScanAction = SCAN_ACTION_DISINFECT;
	
	scanobjarr_t& aScanObjects = pSett->m_aScanObjects;
	if( aScanObjects.size() != 1 )
		return false;

	cStringObj &strScanObjects = aScanObjects[0].m_strObjName;
	strScanObjects.clear();
	size_t i;
	for(i = 0; i < pItemsIds->m_nwstr; i++)
		strScanObjects += pItemsIds->m_pwstr[i];

	qbProfile.m_nAdmFlags = PROFILE_ADMFLAG_NONE;
	if( PR_FAIL(m_pTM->SetProfileInfo(strProfileName, &qbProfile, (cObject *)m_pReciever, 0)) )
		return false;

	if( !SetProfileState(strProfileName, TASK_REQUEST_RUN) )
		return false;

//  correct
//
//	cProfileEx qbProfile;
//	if( !GetProfile(AVP_PROFILE_SCANQUARANTINE, &qbProfile) )
//		return false;
//
//	if( !qbProfile.settings() )
//		return false;
//
//	cScanObjects *pSett = (cScanObjects*)qbProfile.settings();
//	pSett->m_nScanAction = SCAN_ACTION_DISINFECT;
//	
//	scanobjarr_t& aScanObjects = pSett->m_aScanObjects;
//	if( aScanObjects.size() != 1 )
//		return false;
//
//	cStringObj &strScanObjects = aScanObjects[0].m_strObjName;
//	strScanObjects.clear();
//	for(size_t i = 0; i < pItemsIds->m_nwstr; i++)
//		strScanObjects += pItemsIds->m_pwstr[i];
//
//	qbProfile.m_nAdmFlags = PROFILE_ADMFLAG_NONE;
//	qbProfile.m_sName = CreateNewTaskId(qbProfile.m_sType.data()).c_str();
//	if( !AddProfile(&qbProfile, fPROFILE_TEMPORARY) )
//		return false;
//
//	if( !SetProfileState(qbProfile.m_sName.c_str(cCP_ANSI), TASK_REQUEST_RUN) )
//		return false;

	for(i = 0; i < pItemsIds->m_nwstr; i++)
		NotifyQbOperation(pm_QB_IO_MODIFIED, pQb->propGetDWord(plQB_STORAGE_ID), CNetworkListImpl::str2id(pItemsIds->m_pwstr[i]));

	return true;
}

//-------------------------------------------------------

void CProductConnector::SetPortsToOpenList(KLPAR::Params* pData)
{
//	cAutoCS cs(m_pLock, true);
	
	m_aRulesForNagent.clear();
	
	if( KLPAR::ArrayValue *pArray = (KLPAR::ArrayValue *)GetParamsValue(pData, KLCONN::c_szwConnFwPort_Ports, KLPAR::Value::ARRAY_T) )
	{
		for(size_t i = 0, n = pArray->GetSize(); i < n; i++)
		{
			KLPAR::Params *pPortCfg = NULL;
			GET_PARAMS_ARRAY_VALUE_EX(pArray, i, ParamsValue, PARAMS_T, pPortCfg)
			if( pPortCfg )
			{
				tDWORD nProto = 0, nPort = 0, nDirection = 0;
				GET_PARAMS_VALUE_EX(pPortCfg, KLCONN::c_szwConnFwPort_PortType,  IntValue, INT_T, nProto);
				GET_PARAMS_VALUE_EX(pPortCfg, KLCONN::c_szwConnFwPort_Port,      IntValue, INT_T, nPort);
				GET_PARAMS_VALUE_EX(pPortCfg, KLCONN::c_szwConnFwPort_Direction, IntValue, INT_T, nDirection);
				
				// create rule
				cFwPacketRule &Rule = m_aRulesForNagent.push_back();

				Rule.m_nProto = nProto == KLCONN::ConnFwPT_TCP ? _fwPROTO_TCP : _fwPROTO_UDP;
				
				Rule.m_eDirection = _fwInboundOutbound;
				switch(nDirection)
				{
				case KLCONN::ConnFwPT_Dir_Inbound:  Rule.m_eDirection = Rule.m_nProto == _fwPROTO_TCP ? _fwInboundStream  : _fwInbound;  break;
				case KLCONN::ConnFwPT_Dir_Outbound: Rule.m_eDirection = Rule.m_nProto == _fwPROTO_TCP ? _fwOutboundStream : _fwOutbound; break;
				}
				
				cFWPort &Port = Rule.m_aLocalPorts.push_back();
				Port.m_PortLo = Port.m_PortHi = nPort;
			}
		}
	}
	
	cAHFWSettings Sett;
	Sett.m_PortRules = m_aRulesForNagent;
	if( m_pTM )
		m_pTM->sysSendMsg(pmc_TASKAHFW_NAGENT_SETTINGS_CHANGED, 0, NULL, &Sett, SER_SENDMSG_PSIZE);
}

//-------------------------------------------------------

wstring CProductConnector::CreateNewTaskId(const wchar_t *szwTaskType)
{
	const char *szwProfilePrefix = NULL;

	KLSTD_USES_CONVERSION;
	sswitch(KLSTD_W2CA(szwTaskType))
		scase(AVP_TASK_LICENCE)           szwProfilePrefix = AVP_PROFILE_LICENCE;        sbreak;
		scase(AVP_TASK_ONDEMANDSCANNER)   szwProfilePrefix = AVP_PROFILE_SCANOBJECTS;    sbreak;
		scase(AVP_TASK_QUARANTINESCANNER) szwProfilePrefix = AVP_PROFILE_SCANQUARANTINE; sbreak;
		scase(AVP_TASK_UPDATER)           szwProfilePrefix = AVP_PROFILE_UPDATER;        sbreak;
		scase(AVP_TASK_ROLLBACK)          szwProfilePrefix = AVP_PROFILE_ROLLBACK;       sbreak;
		scase(AVP_TASK_RETRANSLATION)     szwProfilePrefix = AVP_PROFILE_RETRANSLATION;  sbreak;
	send

	m_nTaskUnique++;
	saveOneField(m_pReg, NULL, &m_nTaskUnique, sizeof(m_nTaskUnique));
	
	wstring strTaskId;
	strTaskId.resize(0x100);
	strTaskId.resize(swprintf((wchar_t *)strTaskId.c_str(), L"%S$ak%06d", szwProfilePrefix, m_nTaskUnique));
	return strTaskId;
}

//-------------------------------------------------------

void CProductConnector::GetComponentsList(wstrings &aComponents)
{
	aComponents.clear();

	cProfileEx ProfileTree(0);
	if( GetProfileInfo(AVP_PROFILE_PROTECTION, &ProfileTree) )
		GetComponentsList(aComponents, ProfileTree);
}

void CProductConnector::GetComponentsList(wstrings &aComponents, cProfileEx &pProfile)
{
	const wchar_t *str = pProfile.m_sCategory.data();
	const wchar_t *last;
	for( last = str; *str; str++ )
	{
		if( *str == L';' )
		{
			aComponents.push_back(wstring(last, str - last));
			last = str + 1;
		}
	}
	if( *last )
		aComponents.push_back(wstring(last));

	for(tUINT i = 0; i < pProfile.count(); i++)
		GetComponentsList(aComponents, *pProfile[i]);
}

bool CProductConnector::GetLicInfo(cCheckInfoList& keys)
{
	bool res = false;

	hLICENSING pLic = NULL;
	if (m_pTM && PR_SUCC(m_pTM->GetService(0, NULL, sid_TM_LIC, (hOBJECT*) &pLic)))
	{
		res = PR_SUCC(pLic->GetInstalledKeysInfo(&keys));
		m_pTM->ReleaseService(NULL, (hOBJECT) pLic);
	}

	return res;
}

//-------------------------------------------------------

cStringObj CProductConnector::ExpandEnvStr(const wchar_t *str)
{
	cStringObj strExpanded = str;
	g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strExpanded), 0, 0);
	return strExpanded;
}

int CProductConnector::ExpandEnvInt(const wchar_t *str)
{
	cStringObj strExpanded = str;
	g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strExpanded), 0, 0);
	return _wtol(strExpanded.data());
}

//////////////////////////////////////////////////////////////////////////

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <pr_serializable.h>

IMPLEMENT_SERIALIZABLE_BEGIN( cConnectorSettings, 0 )
	SER_VALUE( m_tmLastReport, tid_DWORD, "LastReportTime" )
	SER_VALUE( m_nLastReport,  tid_DWORD, "LastReportPos" )
	SER_VALUE( m_nTaskUnique,  tid_DWORD, "TaskUnique" )
IMPLEMENT_SERIALIZABLE_END()

#undef __DECLARE_SERIALIZABLE
