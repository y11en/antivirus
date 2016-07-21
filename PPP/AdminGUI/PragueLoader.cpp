// PragueLoader.cpp: implementation of the CPragueLoader class.
//

#define PR_IMPEX_DEF

#include "stdafx.h"
#include "PragueLoader.h"
#include <pr_loadr.h>
#include <plugin\p_tm.h>
#include <plugin\p_win32_reg.h>
#include <plugin\p_base64.h>
#include <structs\s_profiles.h>
#include <FWRules.h>
#include <ListsView.h>
#include "ComAdminGui.h"

// -------------------------------- for pr_sizeometer function --------------------------------

#define IMPEX_IMPORT_LIB
#  include <plugin/p_win32_nfio.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <iface/e_loader.h>
	#include <iface/e_ktrace.h>
	#include <plugin/p_win32_nfio.h>
IMPORT_TABLE_END

// -------------------------------- for serializable structs --------------------------------

cSerRegistrar *  g_serializable_registrar_head = NULL;

// -------------------------------- prague globals --------------------------------

hROOT                                   g_root = NULL;
cGuiStub *                              g_hGui = NULL;
cIPRESOLVER *                           g_pIPRes = NULL;
CPragueLoader                           g_Prague;
extern cSerObj<CProfileAdm>             g_pProduct = NULL;

void AntispamShowTrainingWizard(class CItemBase *) {}

//////////////////////////////////////////////////////////////////////////
// CPragueLoader

CPragueLoader::CPragueLoader() :
	cThreadPoolBase("PragueLoader"),
	m_hBase64Codec(NULL),
	m_bStarting(false),
	m_bExiting(false),
	m_nComThreadId(0)
{
}

CPragueLoader::~CPragueLoader()
{
	UnLoadPrague();
}

tERROR CPragueLoader::LoadPrague(HINSTANCE hModule)
{
	cAutoLockerCS Locker(*this);

	if( g_root )
		return errOK;

	m_bExiting = false;
	
	tERROR err = errOK;

	if( PR_SUCC(err) ) err = CPRRemotePrague::Init(hModule, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH);

	char *sPlugins[] = {"params.ppl"};
	for(int i = 0; i < countof(sPlugins) && PR_SUCC(err); i++)
		err = g_root->LoadModule(NULL, sPlugins[i], 0, cCP_ANSI);
	
	for(const cSerRegistrar *tmp = g_serializable_registrar_head; tmp && PR_SUCC(err); tmp = tmp->m_next)
		if( (err = g_root->RegisterSerializableDescriptor(tmp->m_dsc)) == errOBJECT_ALREADY_EXISTS )
			err = errOK;

	if( PR_SUCC(err) ) err = g_root->ResolveImportTable(NULL, import_table, PID_APPLICATION);
	if( PR_SUCC(err) ) err = cPrTracer::Init(hModule);
	if( PR_SUCC(err) ) err = LoadTM();
	if( PR_SUCC(err) ) err = InitProfiles();
	if( PR_SUCC(err) )
	{
		g_hGui = new cGuiStub;
		err = g_hGui->Init((cObject *)g_hTM);
	}
	if( PR_SUCC(err) )
	{
		m_bStarting = true;
		if( PR_SUCC(err) ) err = cThreadPoolBase::init(g_root);
		if( PR_SUCC(err) ) err = cThreadTaskBase::start(*(cThreadPoolBase *)this);
		if( PR_SUCC(err) ) 
			for(; m_bStarting; Sleep(50));
	}
	if( PR_FAIL(err) )
		UnLoadPrague();

	PR_TRACE((g_root, prtIMPORTANT, "ADM\tCPragueLoader::LoadPrague. done (%terr)", err));

	return err;
}

tERROR CPragueLoader::UnLoadPrague()
{
	cAutoLockerCS Locker(*this);

	if( !g_root )
		return errOK;

	if( g_hGui )
		delete g_hGui, g_hGui = NULL;

	m_bExiting = true;
	PostThreadMessage(m_nComThreadId, 0, 0, 0);
	cThreadTaskBase::wait_for_stop();
	cThreadPoolBase::de_init();

	if( g_pProduct )
		g_pProduct.clear();

	g_FwPresets.Clear();

	if( m_hBase64Codec )
		m_hBase64Codec->sysCloseObject(), m_hBase64Codec = NULL;
	
	UnloadTM();
	
	cPrTracer::Deinit();
	
	CPRRemotePrague::Deinit(); g_root = NULL;

	return errOK;
}

cDataCodec *CPragueLoader::GetBase64Codec()
{
	if( !m_hBase64Codec && g_root )
		g_root->sysCreateObjectQuick((cObject **)&m_hBase64Codec, IID_DATACODEC, PID_BASE64);
	return m_hBase64Codec;
}

tERROR CPragueLoader::InitProfiles()
{
	g_pProduct.init(CProfileAdm::eIID);
	g_pProduct->m_sName = AVP_PROFILE_PRODUCT;
	g_pProduct->Update(true);
	//g_pProduct->ResetSettingsLevel(SETTINGS_LEVEL_RESET, true);
	for(tDWORD i = 0; i < g_pProduct->m_aChildren.size(); i++)
	{
		cProfileEx *pProfile = g_pProduct->m_aChildren.at(i);
		if( !(pProfile->m_nAdmFlags & PROFILE_ADMFLAG_ADMINABLE) || pProfile->m_sType.empty() )
		{
			g_pProduct->m_aChildren.remove(pProfile);
			i--;
			continue;
		}
		
		cSerializable * pSett = pProfile->settings();
		if( pSett && pSett->isBasedOn(cTaskSettingsPerUser::eIID) )
			((cTaskSettingsPerUser *)pSett)->m_users.clear();
	}
	
	return errOK;
}


tERROR CPragueLoader::LoadTM()
{
	tERROR err = errOK;

	if( PR_SUCC(err) ) err = g_root->sysCreateObject((hOBJECT*)&g_hTM, IID_TASKMANAGER, PID_TM);
	if( PR_SUCC(err) )
	{
		cRegistry *pEnvironment = NULL;
		if( PR_SUCC(err) ) err = g_hTM->sysCreateObject((hOBJECT *)&pEnvironment, IID_REGISTRY, PID_WIN32_REG);
		if( PR_SUCC(err) )
		{
			if( PR_SUCC(err) ) err = pEnvironment->propSetStr(0, pgROOT_POINT, GetProductRegistryPath());
			if( PR_SUCC(err) ) err = pEnvironment->propSetBool(pgOBJECT_RO, cTRUE);
			if( PR_SUCC(err) ) err = pEnvironment->set_pgROOT_POINT("environment", 12);
			if( PR_SUCC(err) ) err = pEnvironment->sysCreateObjectDone();
			if( PR_FAIL(err) )
			{
				pEnvironment->sysCloseObject();
				pEnvironment = NULL;

				PR_TRACE((g_root, prtERROR, "ADM\tCPragueLoader::InitEnvironment. error creating environment registry key (%terr)", err));
			}
			else
				err = PrSetEnviroment((hOBJECT)pEnvironment, (hOBJECT)g_hTM);

		}

		cRegistry *pConfig = NULL;
		if( PR_SUCC(err) ) err = g_hTM->sysCreateObject((hOBJECT *)&pConfig, IID_REGISTRY, PID_WIN32_REG);
		if( PR_SUCC(err) )
		{
			if( PR_SUCC(err) ) err = pConfig->propSetStr(0, pgROOT_POINT, GetProductRegistryPath());
			if( PR_SUCC(err) ) err = pConfig->propSetBool(pgOBJECT_RO, cTRUE);
			if( PR_SUCC(err) ) err = pConfig->sysCreateObjectDone();
			if( PR_FAIL(err) )
			{
				pConfig->sysCloseObject();
				pConfig = NULL;

				PR_TRACE((g_root, prtERROR, "ADM\tCPragueLoader::LoadTM. error creating config registry key (%terr)", err));
			}
		}

	
		if( PR_SUCC(err) ) err = g_hTM->propSetObj(pgTM_PRODUCT_CONFIG, *pConfig);
		if( PR_SUCC(err) ) err = g_hTM->sysCreateObjectDone();
		if( PR_SUCC(err) ) err = g_hTM->Init(cTM_INIT_FLAG_PROFILES);
		if( PR_FAIL(err) )
		{
			g_hTM->sysCloseObject();
			g_hTM = NULL;

			PR_TRACE((g_root, prtERROR, "ADM\tCPragueLoader::LoadTM. error creating TM (%terr)", err));
		}
	}
	
	PR_TRACE((g_root, prtIMPORTANT, "ADM\tCPragueLoader::LoadTM. done (%terr)", err));
	return err;
}

tERROR CPragueLoader::UnloadTM()
{
	if( g_hTM )
		g_hTM->sysCloseObject(), g_hTM = NULL;

	return errOK;
}

void CPragueLoader::do_work()
{
	GUIControllerRegister();

	m_nComThreadId = GetCurrentThreadId();
	m_bStarting = false;
	
	MSG msg;
	while( !m_bExiting )
	{ 
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	GUIControllerUnregister();
}

//////////////////////////////////////////////////////////////////////////
// cGuiStub

tERROR cGuiStub::Init(cObject *pParent)
{
	tERROR err = pParent->sysCreateObject((hOBJECT*)&m_pReceiver, IID_MSG_RECEIVER);
	if( PR_SUCC(err) ) err = m_pReceiver->propSetPtr(pgRECEIVE_PROCEDURE, MsgReceiveLink);
	if( PR_SUCC(err) ) err = m_pReceiver->propSetPtr(pgRECEIVE_CLIENT_ID, this);
	if( PR_SUCC(err) ) err = m_pReceiver->sysCreateObjectDone();
	if( PR_SUCC(err) )
	{
		tMsgHandlerDescr hdl[] =
		{
			{ (hOBJECT)m_pReceiver, rmhLISTENER, pmc_IPRESOLVER_RESULT,    IID_ANY, PID_RESIP, IID_ANY, PID_ANY },
		};
		err = m_pReceiver->sysRegisterMsgHandlerList(hdl, countof(hdl));
	}
	return err;
}

cGuiStub::~cGuiStub()
{
	DestroyAllRoots();
	
	if( m_pIpRes )
		m_pIpRes->sysCloseObject(), m_pIpRes = NULL;

	if( m_pReceiver )
		m_pReceiver->sysCloseObject(), m_pReceiver = NULL;
}

cIPRESOLVER *cGuiStub::GetIpResolver()
{
	if( !m_pIpRes )
		Obj()->sysCreateObjectQuick((hOBJECT *)&m_pIpRes, IID_IPRESOLVER);
	return m_pIpRes;
}

tERROR cGuiStub::MsgReceive(tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen)
{
	bool bProcess = false;
	bool bSync = false;
	bool bWait = false;
	cSerializable * pSer = blen == SER_SENDMSG_PSIZE ? (cSerializable *)pbuff : NULL;

	switch(msg_cls)
	{
	case pmc_IPRESOLVER_RESULT:
		bProcess = msg_id == pm_RESOLVE_INFO;
		break;
	}

	tERROR err = errOK;
	if( bProcess )
		err = SendEvent2AllRoots(msg_id, pSer, (bSync ? EVENT_SEND_SYNC : 0) | (bWait ? EVENT_SEND_WAIT_DIALOG : 0));
	return err;
}

tERROR cGuiStub::MsgReceiveLink(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen) 
{
	cGuiStub *pThis = (cGuiStub *)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
	return pThis->MsgReceive(msg_cls, msg_id, obj, ctx, receive_point, pbuff, blen);
}

CRootItem *cGuiStub::GetLocRoot()
{
	cAutoLockerCS Locker(*this);

	CRootItem *pRoot = m_LocGui.GetRoot();
	if( !pRoot )
	{
		CRootSinkAdm *pSink = new CRootSinkAdm;
		if( pRoot = m_LocGui.CreateAndInitRoot(g_root, 0, pSink, NULL, false) )
		{
			pRoot->m_nFlags |= STYLE_DESTROYSINK;
			pSink->m_nOSVersion = pRoot->GetSysVersion(tString());
		}
		else
			delete pSink;
	}
	return pRoot;
}

CRootItem *cGuiStub::GetRoot()
{
	cAutoLockerCS Locker(*this);

	CRootItem *pRoot = g_pRoot;
	if( pRoot )
	{
		for(IGuiLoader iGui = m_aGuis.begin(); iGui != m_aGuis.end(); iGui++)
			if( pRoot == (*iGui)->GetRoot() )
			{
				(*iGui)->m_nRef++;
				return pRoot;
			}

		pRoot = NULL;
	}

	CGuiLoaderRef *pLoader = new CGuiLoaderRef;
	CRootSinkAdm *pSink = new CRootSinkAdm;
	if( pRoot = pLoader->CreateAndInitRoot(g_root, GUIFLAG_ADMIN|GUIFLAG_STYLESIMPLE, pSink) )
	{
		pRoot->m_nFlags |= STYLE_DESTROYSINK;
		
		TlsSetValue(g_nTlsIndex, pRoot);
				
		pSink->m_nOSVersion = pRoot->GetSysVersion(tString());

		GUIControllerAddRoot(pRoot, L"");

		m_aGuis.push_back(pLoader);
	}
	else
	{
		delete pSink;
		delete pLoader;
	}

	return pRoot;
}

void cGuiStub::DestroyRoot()
{
	CRootItem *pRoot = g_pRoot;
	if( !pRoot )
		return;

	cAutoLockerCS Locker(*this);
	
	for(IGuiLoader iGui = m_aGuis.begin(); iGui != m_aGuis.end(); iGui++)
	{
		CGuiLoaderRef *pLoader = *iGui;
		if( pRoot == pLoader->GetRoot() )
		{
			if( !(--pLoader->m_nRef)  )
			{
				GUIControllerRemoveRoot(pRoot);

				pLoader->Unload();
				delete pLoader;
				
				TlsSetValue(g_nTlsIndex, NULL);
				
				m_aGuis.erase(iGui);
				break;
			}
		}
	}	
}

void cGuiStub::DestroyAllRoots()
{
	cAutoLockerCS Locker(*this);
	
	for(IGuiLoader iGui = m_aGuis.begin(); iGui != m_aGuis.end(); iGui++)
	{
		CGuiLoaderRef *pLoader = *iGui;

		GUIControllerRemoveRoot(pLoader->GetRoot());
		
		pLoader->Unload();
		delete pLoader;
	}
	m_aGuis.clear();

	m_LocGui.Unload();
}

tERROR cGuiStub::SendEvent2AllRoots(tDWORD nEventId, cSerializable* pEvent, tDWORD nFlags, CItemBase* pSendPoint, tDWORD nTimeOut)
{
	cAutoLockerCS Locker(*this);

	for(IGuiLoader iGui = m_aGuis.begin(); iGui != m_aGuis.end(); iGui++)
		(*iGui)->GetRoot()->SendEvent(nEventId, pEvent, nFlags, pSendPoint, nTimeOut);

	return errOK;
}

////////////////////////////////////////////////////////////////////////////
