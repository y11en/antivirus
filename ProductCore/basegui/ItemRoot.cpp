// ItemRoot.cpp: implementation of the CItemRoot class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <ProductCore/ItemRoot.h>
#include <ProductCore/binding.h>
#include <ProductCore/ItemProps.h>
#include "../../Prague/IniLib/IniRead.h"
#include <Extract/plugin/p_miniarc.h>
#include <Prague/plugin/p_win32_reg.h>
#include <Prague/plugin/p_buffer.h>
#include "SScale/ClassMatrix.h"
#include "SysSink.h"
#include <hashutil/hashutil.h>

#define IMPEX_IMPORT_LIB
    #include <Prague/plugin/p_win32_nfio.h>

#define MAX_CRYPTO_BUFFER	   2048

#define OWNEROFMEMBER_PTR( _owner_class, _member )	\
	((_owner_class *)((unsigned char *)this - (unsigned char *)&((reinterpret_cast<_owner_class *>(0))->_member)))

//////////////////////////////////////////////////////////////////////

void tSyncer::Init(tDWORD dwInitVal)
{
	if( dwInitVal == -1 )
		return;

	tERROR err = ::g_root->sysCreateObject((cObj**)&m_obj, IID_SEMAPHORE);
	if( PR_SUCC(err) )
		err = m_obj->set_pgBAND_WIDTH(1);
	if( PR_SUCC(err) )
		err = m_obj->set_pgINITIAL_COUNT(dwInitVal);
	if( PR_SUCC(err) )
		err = m_obj->sysCreateObjectDone();
	if( PR_FAIL(err) )
		relinquish();
}

tSyncEventData::tSyncEventData(bool bOwnThread, tDWORD _EventId, tDWORD _Flags, cSerializable * _Data, CItemBase * _SendPoint, tDWORD _nTimeOut) :
	m_bOwnThread(bOwnThread), nEventId(_EventId), nFlags(_Flags),
	pSendPoint(_SendPoint), errResult(errOK), nRef(1), nTimeout(_nTimeOut), hWaiter(-1)
{
	if( _Data )
	{
		if( nFlags & EVENT_SEND_OWN_DATA )
		{
			pData.ptr_ref() = _Data;
			_Data->addRef();
		}
		else
			pData.assign(_Data, false);

		if( !pData )
		{
			errResult = errUNEXPECTED;
			return;
		}
	}

	if( pSendPoint )
		pSendPoint->AddRef();
}

tSyncEventData::~tSyncEventData()
{
	if( pSendPoint )
		pSendPoint->Release();
}

void tSyncEventData::AddRef()
{
	PrInterlockedIncrement(&nRef);
}

void tSyncEventData::Release()
{
	if( !PrInterlockedDecrement(&nRef) )
		delete this;
}

//////////////////////////////////////////////////////////////////////

bool CRootItem::SkinPtr::IsInited()
{
	return !!m_pSkinPtr;
}

bool CRootItem::SkinPtr::Init(const tWCHAR* strSkinPath)
{
	if( !InitPath(strSkinPath) )
		return false;

	cAutoObj<cIO> pIO;
	if( PR_SUCC(m_pSkinPtr->IOCreate(&pIO, cAutoString(cStringObj(INI_FILE_SKINROOT)), fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
		return true;

	m_pSkinPtr = NULL;
	return InitZip(strSkinPath);
}

bool CRootItem::SkinPtr::InitAlt(const tWCHAR* strSkinPath)
{
	if( InitZip(strSkinPath) )
		return true;
	return InitPath(strSkinPath);
}

bool CRootItem::SkinPtr::InitPath(const tWCHAR* strSkinPath)
{
	cAutoObj<cObjPtr> pSkinPtr;
	if( PR_FAIL(g_root->sysCreateObject((hOBJECT*)&pSkinPtr, IID_OBJPTR, PID_NATIVE_FIO)) )
		return false;
	pSkinPtr->propSetStr(NULL, pgOBJECT_PATH, (tPTR)strSkinPath, 0, cCP_UNICODE);
	if( PR_FAIL(pSkinPtr->sysCreateObjectDone()) )
		return false;

	m_pSkinPtr = pSkinPtr.relinquish();
	return true;
}

bool CRootItem::SkinPtr::InitZip(const tWCHAR* strSkinPath)
{
	cIOObj pZipIO((hOBJECT)g_root, cAutoString(cStringObj(strSkinPath)), fACCESS_READ);
	if( PR_FAIL(pZipIO.last_error()) )
		return false;

	cAutoObj<cOS> pZipOS;
	cAutoObj<cObjPtr> pZipPtr;
	
	if( PR_SUCC(pZipIO->sysCreateObjectQuick((hOBJECT*)&pZipOS, IID_OS, PID_UNIVERSAL_ARCHIVER)) )
		pZipOS->sysCreateObjectQuick((hOBJECT*)&pZipPtr, IID_OBJPTR, PID_UNIVERSAL_ARCHIVER);

	if( !pZipPtr )
		return false;

	m_pSkinIO = pZipIO.relinquish();
	m_pSkinOS = pZipOS.relinquish();
	m_pSkinPtr = pZipPtr.relinquish();
	return true;
}

void CRootItem::SkinPtr::Cleanup()
{
	m_pSkinPtr = NULL;
	m_pSkinOS = NULL;
	m_pSkinIO = NULL;
}

//////////////////////////////////////////////////////////////////////

SINK_MAP_BEGINEX(CRootItem)
	SINK_DYNAMIC("navigator",    CNavigatorSink())
SINK_MAP_END(CItemBase)

//////////////////////////////////////////////////////////////////////

CRootItem::CRootItem(tDWORD nFlags, CRootSink * pSink) :
//	m_iniLayout(NULL),
//	m_iniLocalize(NULL),
//	m_iniResource(NULL),
	m_iniMm(NULL),
	m_nLocaleId(0),
	m_nGuiFlags(nFlags),
	m_pClicking(NULL),
	m_pClicked(NULL),
	m_pSelected(NULL),
	m_pTrackItem(NULL),
	m_pDrawExcl(NULL),
	m_nWaitDialogs(0),
	m_pCurEvent(NULL),
	m_pDefFont(NULL),
	m_bRefreshRequest(false),
	m_nDlgTransparency(200),
	m_bActive(false),
	m_hESyncer(0),
	m_nOsVer(0),
	m_idTimerRefresh(0),
	m_idTimerAnimation(0),
	m_pTopModalDialog(NULL),
	m_pDebugWnd(NULL),
	m_Thread("GUI"),
	m_ThreadPool("GUI2")
{
	m_pRoot = this;
	m_nState |= ISTATE_INITED;
	m_pSink = pSink;
	m_bGuiActive = true;
	m_tmLastTimer = PrGetTickCount();
	m_tmLastUserInput = 0;

	if( nFlags & GUIFLAG_OWNTHREAD )
		::g_root->sysCreateObjectQuick((hOBJECT*)&m_hEvent, IID_EVENT);
	::g_root->sysCreateObjectQuick((hOBJECT*)&m_accessor, IID_CRITICAL_SECTION);
	::g_root->sysCreateObjectQuick((hOBJECT*)&m_hQLocker, IID_CRITICAL_SECTION);
	::g_root->sysCreateObjectQuick((hOBJECT*)&m_hELocker, IID_CRITICAL_SECTION);

	AddDataSource(NULL);

	m_iniMm = IniReadMmCreate();
}

CRootItem::~CRootItem()
{
	if( m_iniMm )
		IniReadMmDestroy(m_iniMm);
}

void CRootItem::Clear()
{
	// RM - SetTrackItem MUST be before freeing, otherwise debugging memory manager could cause crash 
	SetTrackItem(NULL);
	
	size_t n;
	while( n = m_vecDialogs.size() )
	{
		CDialogItem * pDialog = m_vecDialogs[n - 1];
		pDialog->Close();
		RemoveDialog(pDialog);
	}

	CItemBase::Clear();
}

void CRootItem::Cleanup()
{
	Activate(false);

	m_ThreadPool.de_init();
	LoadIni(false);
	ClearResources(RES_ALL);
	
	m_pSkinAlt.Cleanup();
	m_pSkin.Cleanup();

	CItemBase::Cleanup();

	if( m_cfg.ownership() )
		m_cfg.clean();
	else
		m_cfg.relinquish();
	AttachSink(NULL, false);

	cAutoCS locker(m_hQLocker, true);
	for(std::vector<tSyncEventData*>::iterator it = m_vecQueue.begin(); it != m_vecQueue.end(); ++it)
		(*it)->Release();
	m_vecQueue.clear();
}

void CRootItem::OnInit()
{
	CItemSink * pSink = m_pSink; m_pSink = NULL;
	AttachSink(pSink, false);
	
	{
		tObjPath strSkinPathDef;
		if( m_pSink )
			((CRootSink *)m_pSink)->OnGetSkinPath(strSkinPathDef);
		
		if( strSkinPathDef.empty() )
		{
			OnGetModulePath(strSkinPathDef);
			tDWORD nSlash = strSkinPathDef.find_last_of(L"\\/");
			if( nSlash != tObjPath::npos )
				strSkinPathDef.erase(nSlash + 1);
			strSkinPathDef.add_path_sect("skin");
		}
		
		if( !m_pSkin.Init(strSkinPathDef.data()) )
		{
			strSkinPathDef = "%ProductRoot%";
			
			bool bPathAcquired = pSink && ((CRootSink *)pSink)->ExpandEnvironmentString(strSkinPathDef) || 
				g_root && g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strSkinPathDef), 0, 0) == errOK_DECIDED;
			if( bPathAcquired && !strSkinPathDef.empty() && strSkinPathDef[0] != '%' )
			{
				strSkinPathDef.add_path_sect("skin");
				m_pSkin.Init(strSkinPathDef.data());
			}
		}
	}

	AddIniFile(PROFILE_RESOURCE, INI_FILE_SKINROOT);
	GetIni(PROFILE_RESOURCE).buildCache();

	if( !(m_nGuiFlags & GUIFLAG_EDITOR) )
	{
		hREGISTRY pCfg = NULL;
		bool      bCfgDestroy = false;

		if( m_pSink )
			pCfg = ((CRootSink *)m_pSink)->GetCfg();
		
		if( !pCfg )
		{
			tString strStg; GetString(strStg, PROFILE_RESOURCE, "Parameters", "CfgStorage");
			LPSTR strStgPath = (LPSTR)strchr(strStg.c_str(), ':');
			if( strStgPath )
			{
				*strStgPath = 0, strStgPath++;
				
				pCfg = _GetStorage(strStg.c_str(), strStgPath, !!(m_nGuiFlags & GUIFLAG_CFGSTG_RO));
				bCfgDestroy = true;
			}
		}

		if( pCfg )
		{
			m_cfg.obj() = pCfg;
			m_cfg.ownership(bCfgDestroy);
		}
	}
}

hREGISTRY CRootItem::_GetStorage(LPCSTR strType, LPCSTR strPath, bool bReadOnly)
{
	sswitch(strType)
	stcase(registry)
		return cAutoRegistry((hOBJECT)g_root, strPath, PID_WIN32_REG, bReadOnly ? cTRUE : cFALSE).relinquish();
		sbreak;
	stcase(ini)
		sbreak;
	send;

	return NULL;
}

bool CRootItem::InitRoot(const tWCHAR* strSkinPath)
{
	if( !m_pSkin.IsInited() )
		return false;
	
	m_pSkinAlt.Cleanup();
	m_strLocName.erase();

	if( strSkinPath && *strSkinPath )
		m_pSkinAlt.InitAlt(strSkinPath);

	tDWORD nRefreshFlags = REFRESH_PARAMS|REFRESH_SKIN_DATA;

	if( !m_ThreadPool.is_inited() )
		m_ThreadPool.init(g_root, !m_hEvent ? 2 : 3, 60000);
	else
		nRefreshFlags |= REFRESH_OPEN_DIALOGS;

	Refresh(nRefreshFlags);

	if( m_pSink && ((CRootSink *)m_pSink)->IsGuiControllerFull() )
		m_nGuiFlags &= ~GUIFLAG_CONTROLLER_RO;
	else
		m_nGuiFlags |= GUIFLAG_CONTROLLER_RO;
	
	return true;
}

void CRootItem::ThreadLoop()
{
	ActivateData(true);
	m_hEvent->SetState(cTRUE);
	
	SwapMemory();
	MessageLoop(NULL);

	ActivateData(false);
}

void CRootItem::Thr::do_work()
{
	OWNEROFMEMBER_PTR(CRootItem, m_Thread)->ThreadLoop();
}

bool CRootItem::Activate(bool bActive)
{
	if( !m_hEvent )
	{
		if( m_bActive == bActive )
			return true;
		
		m_bActive = bActive;
		return ActivateData(bActive);
	}
	
	{
		cAutoCS locker(m_accessor, true);

		if( bActive == m_bActive )
			return true;

		if( bActive )
		{
			m_bActive = true;

			m_hEvent->SetState(cFALSE);
			if( !m_Thread.start() )
				return false;

			m_hEvent->Wait(-1);
			return true;
		}

		m_bActive = false;
		PostAction(0);
	}

	if( !IsOwnThread() )
		m_Thread.wait_for_stop();

	m_Thread.de_init();
	return true;
}

bool CRootItem::GetRegSettings(const tCHAR *sPath, cSerializable* pStruct)
{
	if( !g_root || !m_cfg )
		return false;

	return PR_SUCC(g_root->RegDeserialize(&pStruct, m_cfg, sPath, SERID_UNKNOWN));
}

bool CRootItem::SetRegSettings(const tCHAR *sPath, cSerializable* pStruct)
{
	if( !g_root || !m_cfg )
		return false;

	return PR_SUCC(g_root->RegSerialize(pStruct, SERID_UNKNOWN, m_cfg, sPath));
}

void CRootItem::LoadIni(bool bInit)
{
	sIniMm * iniMm = m_iniMm;
	m_iniLayout.clear();
	m_iniLocalize.clear();
	m_iniResource.clear();
	if( m_iniMm ) m_iniMm = NULL;
	m_iniGroups.clear();

	if( bInit )
	{
		m_iniMm = IniReadMmCreate();

		tString strLocPath = INI_DIR_LOCALIZE "/"; strLocPath += m_strLocName;
		
		AddIniFile(PROFILE_RESOURCE, INI_FILE_SKINROOT);
		GetIni(PROFILE_RESOURCE).buildCache();

		AddIniFiles(PROFILE_LAYOUT, PROFILE_SID_LAYOUT, INI_DIR_LAYOUT);
		AddIniFiles(PROFILE_LOCALIZE, PROFILE_SID_LOCALIZE, strLocPath.c_str());
		m_nLocaleId = atoi(GetString(TOR(tString,()), PROFILE_LOCALIZE, "Locale", "Value"));
		OnInitLocale();
	}

	if( iniMm )
		IniReadMmDestroy(iniMm);

//#ifdef _DEBUG
	// TEST !!!
/*	
	tDWORD nTime;
	tDWORD nCount = 0;
	{
		nTime = ::GetTickCount();
		
		int i = 1001;
		while( --i )
		{
			sIniSection * p = GetIni(PROFILE_LAYOUT).m_first;
			while( p )
			{
				sIniSection * f = GetIniSection(PROFILE_LAYOUT, p->name);
				nCount++;
				p = p->next;
			}
		}

		nTime = ::GetTickCount() - nTime;
		tCHAR szTxt[100]; _snprintf(szTxt, countof(szTxt), "%u ms", nTime);
		::MessageBox(NULL, szTxt, "BaseGui GetIniSection Test", MB_OK);
	}
*/
//#endif // _DEBUG
}

bool CRootItem::OpenLocFile(LPCSTR strFile, cIO *&pIO)
{
	if( !m_pSkin.m_pSkinPtr )
		return false;

	cStringObj sFile(INI_DIR_LOCALIZE "/");
	sFile += m_strLocName.c_str();
	sFile += '/';
	sFile += strFile;
	return PR_SUCC(IniIOCreate(&pIO, sFile, fACCESS_READ));
}

tERROR CRootItem::IniIOCreate(hIO * result, const tObjPath& strPath, tDWORD access_mode, bool bForceRo)
{
	cAutoString sFile(strPath);
	tERROR err = errOK;

	if( access_mode & fACCESS_WRITE )
	{
		cObjPtr * pSkinPtr = !!m_pSkinAlt.m_pSkinPtr ? m_pSkinAlt.m_pSkinPtr : m_pSkin.m_pSkinPtr;
		
		// Check for readonly
		if( *result )
		{
			if( !bForceRo && ((*result)->get_pgOBJECT_ATTRIBUTES() & fATTRIBUTE_READONLY) )
				return errOBJECT_READ_ONLY;

			(*result)->sysCloseObject();
			*result = NULL;
		}
		else
		{
			cAutoObj<cIO> pIo; err = pSkinPtr->IOCreate(&pIo, sFile, 0, fOMODE_OPEN_IF_EXIST|fOMODE_FILE_NAME_PREPARSING);
			if( !!pIo && !bForceRo && (pIo->get_pgOBJECT_ATTRIBUTES() & fATTRIBUTE_READONLY) )
				return errOBJECT_READ_ONLY;
		}
		
		err = pSkinPtr->IOCreate(result, sFile, access_mode, fOMODE_OPEN_ALWAYS|fOMODE_FILE_NAME_PREPARSING);
		if( *result )
			(*result)->propSetBool(plRESTORE_ATTRIBUTES, cFALSE);
	}
	else
	{
		if( !m_pSkinAlt.m_pSkinPtr ||
			PR_FAIL(m_pSkinAlt.m_pSkinPtr->IOCreate(result, sFile, access_mode, fOMODE_OPEN_IF_EXIST|fOMODE_FILE_NAME_PREPARSING)) )
		{
			err = m_pSkin.m_pSkinPtr->IOCreate(result, sFile, access_mode, fOMODE_OPEN_IF_EXIST|fOMODE_FILE_NAME_PREPARSING);
		}
	}

	return err;
}

void CRootItem::tIniFile::clear()
{
	m_data.clear();
	m_first = NULL;
}

#define _IniStrCmp	strcmp

int CRootItem::tIniFile::key_cmp_S(const void * key, const void * e)
{
	return _IniStrCmp((LPCSTR)key, ((S *)e)->m_sec->name);
}

int CRootItem::tIniFile::key_cmp_L(const void * key, const void * e)
{
	return _IniStrCmp((LPCSTR)key, (*(L *)e)->name);
}

int CRootItem::tIniFile::sort_cmp_S(const void * e1, const void * e2)
{
	return _IniStrCmp(((S *)e1)->m_sec->name, ((S *)e2)->m_sec->name);
}

int CRootItem::tIniFile::sort_cmp_L(const void * e1, const void * e2)
{
	return _IniStrCmp((*(L*)e1)->name, (*(L*)e2)->name);
}

void CRootItem::tIniFile::buildCache()
{
	if( !m_first )
		return;

	for(sIniSection * p = m_first; p;  p = p->next)
	{
		size_t i, n = m_data.size();
		for(i = 0; i < n ; i++)
			if( !strcmp(m_data[i].m_sec->name, p->name) )
				break;

		if( i == n )
		{
			S s = {p};
			m_data.push_back(s);
		}

		S& s = m_data[i];
		for(sIniLine * pl = p->line_head; pl; pl = pl->next)
			s.m_data.push_back(pl);
	}

	for(size_t i = 0, n = m_data.size(); i < n ; i++)
	{
		S& s = m_data[i];
		if( size_t n = s.m_data.size() )
			qsort(&*s.m_data.begin(), n, sizeof(L), sort_cmp_L);
	}

	if( size_t n = m_data.size() )
		qsort(&*m_data.begin(), n, sizeof(S), sort_cmp_S);
}

CRootItem::tIniFile::S * CRootItem::tIniFile::find(LPCSTR section_name)
{
	int size = m_data.size(); if( !size ) return 0;
	return (S *)bsearch(section_name, &*m_data.begin(), size, sizeof(S), key_cmp_S);
}

CRootItem::tIniFile::L * CRootItem::tIniFile::find(S& s, LPCSTR line_name)
{
	int size = s.m_data.size(); if( !size ) return 0;
	return (L *)bsearch(line_name, &*s.m_data.begin(), size, sizeof(L), key_cmp_L);
}

sIniSection * CRootItem::GetIniSection(tDWORD nFmtId, LPCSTR name)
{
	tIniFile& pIni = GetIni(nFmtId);
/**/
	tIniFile::S * s = pIni.find(name);
	return s ? s->m_sec : NULL;
/*/
	return IniGetSection(pIni.m_first, name);
/**/
}

LPCSTR CRootItem::GetIniValue(tDWORD nFmtId, LPCSTR section_name, LPCSTR line_name, bool bSafe)
{
	tIniFile& pIni = GetIni(nFmtId);
/**/
	sIniLine *line = NULL;
	if( tIniFile::S * s = pIni.find(section_name) )
	{
		if( tIniFile::L * l = pIni.find(*s, line_name) )
			line = *l;
	}
	else if( nFmtId == PROFILE_LAYOUT )
		ReportError(prtIMPORTANT, "layout section [%s] not found", section_name);

	return IniGetValueEx(line, bSafe);
/*/
	return IniGetValue(pIni.m_first, section_name, line_name, bSafe);
/**/
}

bool CRootItem::AddIniFile(tDWORD nFmtId, LPCSTR strFile)
{
	if( !m_pSkin.m_pSkinPtr )
		return false;

	cAutoObj<cIO> pIO;
	if( PR_FAIL(IniIOCreate(&pIO, cStringObj(strFile), fACCESS_READ)) )
		return false;

	tIniFile& pIni = GetIni(nFmtId);
	if( PR_FAIL(IniReadMmLoad(&pIni.m_first, m_iniMm, pIO, INI_LOAD_MULTILINEVALS)) )
		return false;
	
	return true;
}

void CRootItem::AddIniFiles(tDWORD nFmtId, LPCSTR strSection, LPCSTR strPath)
{
	sIniSection * pSec = GetIniSection(PROFILE_RESOURCE, strSection);
	if( !pSec )
		return;
	
	for(sIniLine * pLine = pSec->line_head; pLine; pLine = pLine->next)
	{
		tIniFile& pIni = GetIni(nFmtId);
		sIniSection * pLast = pIni.m_first;
		if( pLast ) while(pLast->next) pLast = pLast->next;
		
		tString strIniPath = strPath;
		strIniPath += '/';
		strIniPath += pLine->name;
		AddIniFile(nFmtId, strIniPath.c_str());

		if( nFmtId != PROFILE_LAYOUT )
			continue;
		
		tGroupSections& _iniFile = *m_iniGroups.insert(m_iniGroups.end(), tGroupSections());
		_iniFile.s_name = pLine->name;
		{
			tDWORD nPos = _iniFile.s_name.find_last_of(".");
			if( nPos != cStrObj::npos )
				_iniFile.s_name.erase(nPos);
		}
		_iniFile.s_first = pLast ? pLast->next : pIni.m_first;
		_iniFile.s_last = _iniFile.s_first;
		if( _iniFile.s_last ) while(_iniFile.s_last->next) _iniFile.s_last = _iniFile.s_last->next;
	}

	GetIni(nFmtId).buildCache();
}

void CRootItem::ReinitResources(tDWORD nFlags)
{
	if( nFlags & RES_FONTS )
	{
		for(tMapFonts::iterator it = m_mapFonts.begin(); it != m_mapFonts.end(); )
		{
			if(InitFont(it))
				++it;
			else
			{
				// RM This is not standard, MS-only extension to STL
				//it = m_mapFonts.erase(it);
				tMapFonts::iterator it_copy = it++;
				m_mapFonts.erase(it_copy);
			}
		}
		
		m_pDefFont = NULL;
	}
	
	if( nFlags & RES_ICONS )
	{
		for(tMapIcons::iterator it = m_mapIcons.begin(); it != m_mapIcons.end(); )
		{
			if(InitIcon(it))
				++it;
			else
			{
				//it = m_mapIcons.erase(it);
				tMapIcons::iterator it_copy = it++;
				m_mapIcons.erase(it_copy);
			}
		}
	}
	
	if( nFlags & RES_BACKGROUNDS )
	{
		for(tMapBackgrounds::iterator it = m_mapBackgrounds.begin(); it != m_mapBackgrounds.end(); )
		{
			if(InitBackground(it))
				++it;
			else
			{
				//it = m_mapBackgrounds.erase(it);
				tMapBackgrounds::iterator it_copy = it++;
				m_mapBackgrounds.erase(it_copy);
			}
		}
	}
	
	if( nFlags & RES_BORDERS )
	{
		for(tMapBorders::iterator it = m_mapBorders.begin(); it != m_mapBorders.end(); )
		{
			if(InitBorder(it))
				++it;
			else
			{
				//it = m_mapBorders.erase(it);
				tMapBorders::iterator it_copy = it++;
				m_mapBorders.erase(it_copy);
			}
		}
	}
}

void CRootItem::Refresh(tDWORD nFlags)
{
	if( nFlags & REFRESH_DEFLOC )
	{
		if(m_pSink)
		{
			cStringObj sEnvSkinSwitchDisabled(L"%SkinSwitchDisabled%");
			((CRootSink *)m_pSink)->ExpandEnvironmentString(sEnvSkinSwitchDisabled);
			if(sEnvSkinSwitchDisabled == L"1")
				return;
		}
		m_strLocName.erase();
	}
	else if( m_strLocName.empty() || (nFlags & REFRESH_SKIN_DATA) )
	{
		if( m_pSink )
			((CRootSink *)m_pSink)->GetLocalizationId(m_strLocName);
		
		if( m_strLocName.empty() )
		{
			tString strLocIdCfg; GetString(strLocIdCfg, PROFILE_RESOURCE, "Parameters", "LocIdCfg");
			LPSTR strStgPath = (LPSTR)strchr(strLocIdCfg.c_str(), ':');
			if( strStgPath )
			{
				*strStgPath = 0, strStgPath++;
				
				LPSTR strStgVal = strrchr(strStgPath, ':');
				if( strStgVal )
				{
					*strStgVal = 0, strStgVal++;
					cAutoRegistry _cfg; _cfg.obj() = _GetStorage(strLocIdCfg.c_str(), strStgPath, true);
					cStrObj strLocId; cAutoRegKey(_cfg, cRegRoot, "").get_strobj(strStgVal, strLocId);
					m_strLocName = strLocId.c_str(cCP_ANSI);
				}
			}
		}
	}

	if( m_strLocName.empty() || m_strLocName[0] == '%' )
		m_strLocName = "en";

	if( nFlags & REFRESH_PARAMS )
	{
		if( !m_pSink || ((CRootSink *)m_pSink)->IsGuiSimple() )
			m_nGuiFlags |= GUIFLAG_STYLESIMPLE;
		else
			m_nGuiFlags &= ~GUIFLAG_STYLESIMPLE;
	}

	if( nFlags & REFRESH_SKIN_DATA )
	{
		LoadIni();
		ReinitResources();
		UpdateProps(UPDATE_FLAG_FONT|UPDATE_FLAG_ICON|UPDATE_FLAG_BACKGROUND, NULL);
	}

	if( nFlags & REFRESH_PARAMS )
	{
		if( m_pSink )
			m_nDlgTransparency = ((CRootSink *)m_pSink)->GetDlgTransparency();
	}

	if( nFlags & REFRESH_OPEN_DIALOGS )
		m_bRefreshRequest = true;
}

COLORREF CRootItem::GetColor(LPCSTR strKey, COLORREF nDefVal)
{
	if( !strKey || !*strKey )
		return nDefVal;

	tString strVal; GetString(strVal, PROFILE_RESOURCE, PROFILE_SID_COLORS, strKey);
	if( strVal.empty() )
	{
		COLORREF ret = GetSysColor(strKey);
		return( ret == -1 ? nDefVal : ret);
	}

	if( strVal[ 0 ] == 'R' )
	{
		tDWORD r,g,b;
		if( sscanf( strVal.c_str(),  "RGB(%d,%d,%d)", &r, &g, &b ) != 3 )
			return( nDefVal );
		return ((COLORREF)(((tBYTE)(r)|((tWORD)((tBYTE)(g))<<8))|(((tDWORD)(tBYTE)(b))<<16)));
	}

	return GetSysColor(strVal.c_str());
}

CFontStyle * CRootItem::GetFont(LPCSTR strKey)
{
	if( strKey == RES_DEF_FONT )
	{
		if( m_pDefFont )
			return m_pDefFont;
		return m_pDefFont = GetFont(FONTID_DEFAULT);
	}
	
	if( !strKey || !*strKey )
		return NULL;

	tMapFonts::iterator it = m_mapFonts.find(strKey);
	if( it == m_mapFonts.end() )
	{
		it = m_mapFonts.insert(tMapFonts::value_type(strKey, NULL)).first;
		InitFont(it);
	}
	
	return it->second;
}

CIcon * CRootItem::GetIcon(LPCSTR strKey)
{
	if( !strKey || !*strKey )
		return NULL;

	tMapIcons::iterator it = m_mapIcons.find(strKey);
	if( it == m_mapIcons.end() )
	{
		it = m_mapIcons.insert(tMapIcons::value_type(strKey, NULL)).first;
		InitIcon(it);
	}
	
	return it->second;
}

LPCSTR CRootItem::GetIconKey(CIcon * pIcon)
{
	if( !pIcon )
		return NULL;
	
	for(tMapIcons::iterator i = m_mapIcons.begin(); i != m_mapIcons.end(); i++)
		if( i->second == pIcon )
			return i->first.c_str();

	return NULL;
}

CIcon* CRootItem::ExtractIcon(LPCSTR strPath, tDWORD nType, tDWORD nMask, CItemBase *pItemToNotify, bool bExpandPath, CIcon* pDefIcon)
{
	LPCSTR strIconKey = strPath;
	tString strTmp;
	
	bool bIsPath = strPath && *strPath;
	if( bIsPath && (nMask & OBJINFO_LARGEICON) )
	{
		strTmp = strIconKey;
		strTmp += ":L";
		strIconKey = strTmp.c_str();
	}

	if( bIsPath )
	{
		tMapIcons::iterator it = m_mapIcons.find(strIconKey);
		if( it != m_mapIcons.end() )
			return it->second;

		if( pItemToNotify )
		{
			pItemToNotify->AddRef();
			CIconExctructTask * pTask = new CIconExctructTask(pItemToNotify, LocalizeStr(TOR_cStrObj, strPath), nType, nMask, bExpandPath, pDefIcon);
			if( !pTask->start(true) )
				pItemToNotify->Release();
			return pDefIcon;
		}
	}

	CObjectInfo oi;
	oi.m_strName = strPath;
	oi.m_nType = nType;
	oi.m_nQueryMask = OBJINFO_ICON|nMask;
	
	if( bIsPath && bExpandPath && m_pSink && ((CRootSink *)m_pSink)->ExpandEnvironmentString(oi.m_strName) )
	{
		GetObjectInfo(&oi);
		oi.m_strName = strPath;
	}
	else
		GetObjectInfo(&oi);

	if( !oi.m_pIcon )
		if( (oi.m_pIcon = pDefIcon) && bIsPath )
			pDefIcon->AddRef();

	if( bIsPath )
		PutIconInCache(strIconKey, oi.m_pIcon);

	return oi.m_pIcon;
}

void CRootItem::PutIconInCache(LPCSTR strKey, CIcon *pIcon)
{
	const pair<tMapIcons::iterator, bool>& res = m_mapIcons.insert(tMapIcons::value_type(strKey, pIcon));
	if( pIcon && !res.second && !res.first->second )
		res.first->second = pIcon;
}

CBackground * CRootItem::GetBackground(LPCSTR strKey)
{
	if( !strKey || !*strKey )
		return NULL;

	tMapBackgrounds::iterator it = m_mapBackgrounds.find(strKey);
	if( it == m_mapBackgrounds.end() )
	{
		it = m_mapBackgrounds.insert(tMapBackgrounds::value_type(strKey, NULL)).first;
		InitBackground(it);
	}
	
	return it->second;
}

LPCSTR CRootItem::GetBackgroundKey(CBackground* pBackground)
{
	if( !pBackground )
		return NULL;
	
	for(tMapIcons::iterator i = m_mapBackgrounds.begin(); i != m_mapBackgrounds.end(); i++)
		if( i->second == pBackground )
			return i->first.c_str();

	return NULL;
}

CBorder * CRootItem::GetBorder(LPCSTR strKey)
{
	if( !strKey || !*strKey )
		return NULL;

	tMapBorders::iterator it = m_mapBorders.find(strKey);
	if( it == m_mapBorders.end() )
	{
		it = m_mapBorders.insert(tMapBorders::value_type(strKey, NULL)).first;
		InitBorder(it);
	}
	
	return it->second;
}

bool CRootItem::InitFont(tMapFonts::iterator& p)
{
	CItemPropVals l_props(GetString(TOR(tString,()), PROFILE_RESOURCE, PROFILE_SID_FONTS, p->first.c_str()));

	tString&        strFont    = l_props.Get(0);
	tDWORD          nHeight    = (tDWORD)l_props.GetLong(1);
	tDWORD          nBold      = (tDWORD)l_props.GetLong(2);
	const tString&  strStyle   = l_props.Get(3);
	COLORREF        nColor     = GetColor(l_props.GetStr(4));
	tDWORD          nCharExtra = (tDWORD)l_props.GetLong(5);
	
	bool bUnderline = false, bItalic = false;
	for(size_t i = 0; i < strStyle.size(); i++)
		switch( strStyle[i] )
		{
		case 'i': bItalic = true; break;
		case 'u': bUnderline = true; break;
		}

	if( strFont == "sysdef" )
		OnGetSysDefFont(strFont);
	
	CFontStyle *& pFont = p->second;
	if( !pFont )
		if( !(pFont = CreateFont()) )
			return false;

	if( pFont->Init(this, strFont.c_str(), nHeight, nColor, nBold, bItalic, bUnderline, nCharExtra) )
		return true;

	pFont->Destroy(), pFont = NULL;
	return false;
}

bool CRootItem::InitIcon(tMapIcons::iterator& p)
{
	CItemPropVals l_props(GetString(TOR(tString,()), PROFILE_RESOURCE, PROFILE_SID_ICONS, p->first.c_str()));

	cAutoObj<cIO> pSource;
	if( !GetImage(l_props.GetStr(), &pSource) )
		return false;

	CIcon *& pIcon = p->second;
	if( !pIcon && !(pIcon = CreateImage(eImageIcon, (cIO*)pSource, (int)l_props.GetLong(1), (int)l_props.GetLong(2), true, pIcon)) )
		return false;

	return true;
}

bool CRootItem::InitBackground(tMapBackgrounds::iterator& p)
{
	tString strProps; GetString(strProps, PROFILE_RESOURCE, PROFILE_SID_BACKGNDS, p->first.c_str());
	CItemPropVals l_props(strProps.c_str());

	COLORREF      clColor = GetColor(l_props.GetStr());
	int           nRadius = 0;
	int           nStyle = CBackground::None;
	int			  clFrame = -1;
	cAutoObj<cIO> pImage;

	tString& strGrad = l_props.Get(2);
	
	if( /*!GUI_ISSTYLE_SIMPLE(this) &&*/ !strGrad.empty() )
		switch( strGrad[0] )
		{
		case 'l': nStyle = CBackground::Left; break;
		case 'r': nStyle = CBackground::Right; break;
		case 't': nStyle = CBackground::Top; break;
		case 'b': nStyle = CBackground::Bottom; break;
		case 'm': nStyle = CBackground::Mozaic; break;
		case 'c': nStyle = CBackground::Transparent; break;
		case 'f': clFrame = -2; break;
		}

	if( strGrad.size() > 1 && strGrad[1] == 's' )
		nStyle |= CBackground::System;

	if( clColor != -1 )
	{
		nRadius = (int)l_props.GetLong(1);
		if( nRadius && GUI_ISSTYLE_SIMPLE(this) && !IsThemed() )
			nRadius = -1;

		if (l_props.Count()==4)
			clFrame = GetColor(l_props.Get(3).c_str());
	}
	else
	{
		const tString& strFile = l_props.Get();
		if( !strFile.empty() && !GetImage(l_props.GetStr(), &pImage) )
			return false;
	}

	CBackground *& pBackground = p->second;
	if( clColor != -1 )
		pBackground = CreateBackground(clColor, clFrame, nRadius, nStyle, pBackground);
	else
		pBackground = CreateImage(eImageSimple, pImage, (int)l_props.GetLong(1), 0, nStyle, pBackground);

	if( !pBackground )
		return false;
	return true;
}

bool CRootItem::InitBorder(tMapBorders::iterator& p)
{
	tString strProps; GetString(strProps, PROFILE_RESOURCE, PROFILE_SID_BORDERS, p->first.c_str());
	CItemPropVals l_props(strProps.c_str());

	COLORREF       clColor = GetColor(l_props.GetStr());
	tDWORD         nStyle = 0;
	const tString& strLines = l_props.Get(1);
	
	for(size_t i = 0; i < strLines.size(); i++)
	{
		switch( strLines[i] )
		{
		case 'l': nStyle |= CBorder::Left; break;
		case 'r': nStyle |= CBorder::Right; break;
		case 't': nStyle |= CBorder::Top; break;
		case 'b': nStyle |= CBorder::Bottom; break;
		case 'x': nStyle |= CBorder::XPBrush; break;
		}
	}

	tDWORD nWidth = (tDWORD)l_props.GetLong(2);
	if( !nWidth )
		nWidth = 1;
	
	CBorder *& pBorder = p->second;
	if( !pBorder )
		if( !(pBorder = CreateBorder()) )
			return false;

	if( pBorder->Init(nStyle, clColor, nWidth) )
		return true;

	pBorder->Destroy(), pBorder = NULL;
	return false;
}

void CRootItem::ClearResources(tDWORD nFlags)
{
	if( nFlags & RES_FONTS )
	{
		for(tMapFonts::iterator it = m_mapFonts.begin(); it != m_mapFonts.end(); it++)
			if( it->second )
				it->second->Destroy();
		m_mapFonts.clear();

		m_pDefFont = NULL;
	}

	if( nFlags & RES_ICONS )
	{
		for(tMapIcons::iterator it = m_mapIcons.begin(); it != m_mapIcons.end(); it++)
			if( it->second )
				it->second->Destroy();
		m_mapIcons.clear();
	}

	if( nFlags & RES_BACKGROUNDS )
	{
		for(tMapBackgrounds::iterator it = m_mapBackgrounds.begin(); it != m_mapBackgrounds.end(); it++)
			if( it->second )
				it->second->Destroy();
		m_mapBackgrounds.clear();
	}

	if( nFlags & RES_BORDERS )
	{
		for(tMapBorders::iterator it = m_mapBorders.begin(); it != m_mapBorders.end(); it++)
			if( it->second )
				it->second->Destroy();
		m_mapBorders.clear();
	}
}

//////////////////////////////////////////////////////////////////////

bool CRootItem::Resolve(tDWORD nFmtId, LPCSTR strMacro, tString& strResult)
{
	GetString(strResult, nFmtId, nFmtId != PROFILE_LOCALIZE ? PROFILE_SID_TEMPLATES : PROFILE_SID_GLOBAL, strMacro);
	return true;
}

LPCSTR CRootItem::GetString(tString& str, tDWORD nFmtId, LPCSTR strSection, LPCSTR strKey, LPCSTR nDefVal, CMacroResolver * pResolver)
{
	if( !strKey || !*strKey )
		strKey = ENUM_STREMPTY;

	if( !strSection )
		strSection = PROFILE_SID_GLOBAL;

	if( !*strSection )
		return str.c_str();

	LPCSTR strRaw = NULL;
	if( (pResolver != GETSTRING_RAW) && (nFmtId == PROFILE_RESOURCE) )
		strRaw = GetIniValue(PROFILE_LOCALIZE, strSection, strKey);
	if( !strRaw )
		strRaw = GetIniValue(nFmtId, strSection, strKey);

	str = strRaw ? strRaw : nDefVal;
	if( pResolver != GETSTRING_RAW )
		_GetString(str, nFmtId, strSection, nDefVal, pResolver);
	else
		TrimString(str);
	return str.c_str();
}

LPCWSTR CRootItem::GetString(cStringObj &str, tDWORD nFmtId, LPCSTR strSection, LPCSTR strKey, LPCSTR nDefVal, CMacroResolver* pResolver)
{
	std::string s;
	LPCSTR sTarget = GetString(s, nFmtId, strSection, strKey, nDefVal, pResolver);
	if(!sTarget)
		return NULL;
	return LocalizeStr(str, sTarget);
}

LPCSTR CRootItem::_GetString(tString& sValue, tDWORD nFmtId, LPCSTR strSection, LPCSTR nDefVal, CMacroResolver * pResolver)
{
	TrimString(sValue);

	tString strMacro; bool bMacro = false; bool bStrOpt = false;
	for(size_t c = 0; c < sValue.size() + 1;)
	{
		tCHAR& ch = sValue[c];
		
		if( bStrOpt )
		{
			bool bIgnore = false;
			switch( ch )
			{
			case 'n':  ch = '\n';      break;
			case 't':  ch = '\t';      break;
			case 'r':  ch = '\r';      break;
			case 'e':  bIgnore = true; break;
			case '\0': bIgnore = true; break;
			case ' ':                  break;
			case '\\':                 break;
			default :
				// В языках с иероглифами код 0x5c (символ '\') может являться частью иероглифа.
				// Если после '\' не встретился ни один спец. символ - возвращаем этот код обратно
				sValue.insert(&ch - sValue.c_str(), 1, '\\');
				c++;
				break;
			}
			
			bStrOpt = false;
			
			if( bIgnore )
				sValue.erase(&ch - sValue.c_str(), 1);
			else
				c++;
			
			continue;
		}

		if( !bMacro )
		{
			switch( ch )
			{
			case '$':
			case '#':
				bMacro = true;
				strMacro.erase();
				strMacro += ch;
				break;
			
			case '\\':
				sValue.erase(&ch - sValue.c_str(), 1);
				bStrOpt = true;
				continue;
			}
			
			c++; continue;
		}

		if( _kav_iscsym(ch) )
		{
			strMacro += ch;
			c++; continue;
		}

		if( strMacro.size() > 1 )
		{
			tString strHolder; GetString(strHolder, nFmtId, strSection, strMacro.c_str(), nDefVal, pResolver);
			if( strHolder.empty() )
			{
				if( !pResolver )
					pResolver = this;

				pResolver->Resolve(nFmtId, strMacro.c_str(), strHolder);
				
				if( strHolder.empty() && nFmtId == PROFILE_LAYOUT )
				{
					GetString(strHolder, PROFILE_LOCALIZE, strSection, strMacro.c_str(), nDefVal, pResolver);
					if( strHolder.empty() )
						pResolver->Resolve(PROFILE_LOCALIZE, strMacro.c_str(), strHolder);
				}
			
				if( strHolder.empty() )
					ReportError(prtERROR, "Can't find macro \"%s\"", strMacro.c_str());
			}

			tNATIVE_INT nHolderLen = strHolder.size(), nMacroLen = strMacro.size();
			tNATIVE_INT	nResize = - nMacroLen + nHolderLen;
			
			if( nResize >= 0 )
			{
				sValue.resize(sValue.size() + nResize);
				memmove(&sValue[c + nResize], &sValue[c],
					sizeof(tCHAR)*(strlen(&sValue[c]) + 1));
			}
			else
			{
				memmove(&sValue[c + nResize], &sValue[c],
					sizeof(tCHAR)*(strlen(&sValue[c]) + 1));
				sValue.resize(sValue.size() + nResize);
			}
			
			memcpy(&sValue[c - nMacroLen], strHolder.c_str(), sizeof(tCHAR)*nHolderLen);
			c += nResize;
		}
		else if( ch == '$' || ch == '#' )
			sValue.erase(&ch - sValue.c_str(), 1);
		
		bMacro = false;
	}

	return sValue.c_str();
}

CRootItem::tIniFile& CRootItem::GetIni(tDWORD nFmtId)
{
	switch(nFmtId)
	{
	case PROFILE_LAYOUT:   return m_iniLayout;
	case PROFILE_LOCALIZE: return m_iniLocalize;
	case PROFILE_RESOURCE: return m_iniResource;
	}
	
	return m_iniLayout;
}

bool CRootItem::GetKeys(tDWORD nFmtId, LPCSTR strSection, tKeys& aKeys, bool bMerge, CMacroResolver * pResolver)
{
	if( !strSection || !*strSection )
		return false;

	sIniSection * pSection = GetIniSection(nFmtId, strSection);
	if( !pSection )
		return false;

	for(sIniLine * pLine = pSection->line_head; pLine; pLine = pLine->next )
		if( pLine->name && *pLine->name != '$' )
		{
			aKeys.push_back(tIniKey());
			tIniKey& _key = aKeys[aKeys.size() - 1];
			_key.assign(pLine->name);
			
			if( pResolver )
			{
				_key.val = pLine->value;
				_GetString(_key.val, nFmtId, strSection, "", pResolver);
			}
		}
	
	return true;
}

void CRootItem::GetSections(tDWORD nFmtId, tSections& aSections)
{
	sIniSection * s = GetIni(nFmtId).m_first;
	while( s )
	{
		*aSections.insert(aSections.end(), tString()) = s->name;
		s = s->next;
	}
}

bool CRootItem::IsSectionExists(tDWORD nFmtId, LPCSTR strSection)
{
	if( !strSection || !*strSection )
		return false;

	return !!GetIniSection(nFmtId, strSection);
}

tQWORD CRootItem::GetIntFromKey(LPCSTR strKey, tTYPE_ID eReqType)
{
	if( strKey && *strKey == '#' )
	{
		tString strVal; m_pRoot->GetString(strVal, PROFILE_LOCALIZE, NULL, strKey);
		return GetIntFromString(strVal.c_str());
	}
	return GetIntFromString(strKey);
}

LPCWSTR CRootItem::GetSectionGroup(tDWORD nFmtId, LPCSTR strSection)
{
	if( nFmtId != PROFILE_LAYOUT )
		return NULL;

	for(tDWORD i = 0; i < m_iniGroups.size(); i++)
	{
		tGroupSections& _iniFile = m_iniGroups[i];
		sIniSection * pSect = _iniFile.s_first;
		if( pSect )
		{
			for(;;)
			{
				if( !strcmp(pSect->name, strSection) )
					return _iniFile.s_name.data();

				if( pSect == _iniFile.s_last )
					break;

				pSect = pSect->next;
			}
		}
	}

	return NULL;
}

bool CRootItem::GetImageHlp(const cStringObj &strPath, hIO* pSource)
{
	cAutoObj<cIO> pIO;
	if( PR_FAIL(IniIOCreate(&pIO, strPath, fACCESS_READ)) )
		return false;

	*pSource = pIO.relinquish();
	return true;
}

bool CRootItem::GetImage(LPCSTR nStrId, hIO* pSource)
{
	cStringObj strPath;
	if( m_strLocName.size() )
	{
		strPath = INI_DIR_LOCALIZE "/";
		strPath += m_strLocName.c_str();
		strPath += "/images/";
		strPath += nStrId;
		if( GetImageHlp(strPath, pSource) )
			return true;
	}
	
	strPath = "images/";
	strPath += nStrId;
	return GetImageHlp(strPath, pSource);
}

//////////////////////////////////////////////////////////////////////

bool CRootItem::BrowseObject(CBrowseObjectInfo& pBrowseInfo)
{
	return CBrowseShellObject(pBrowseInfo).DoModal(pBrowseInfo.m_pParent, pBrowseInfo.m_strSection, DLG_ACTION_OKCANCEL) == DLG_ACTION_OK;
}

int CRootItem::ShowMsgBox(CItemBase* pItem, LPCSTR strTextId, LPCSTR strCaptionId, tUINT uType, cSerializable * pData, CFieldsBinder * pBinder)
{
	// for test
	//return ::MessageBoxA(pItem ? pItem->GetWindow() : NULL, strText, strCaption, uType);
	
	return CMsgBox::Show(this, pItem, strTextId ? strTextId : "AnyError", strCaptionId ? strCaptionId : "ProductName", uType, pData, pBinder);
}

//////////////////////////////////////////////////////////////////////

tERROR CRootItem::DoCommand(CItemBase * pItem, LPCSTR strCommand, bool bGuiController)
{
	if( !pItem )
		pItem = this;
	
	cNode * pNode = CreateNode(strCommand, NULL, pItem);
	if( !pNode )
		return errNOT_OK;

	pNode->Exec(TOR(cVariant, ()), TOR(cNodeExecCtx, (this, pItem, NULL, bGuiController)));
	pNode->Destroy();
	return errOK;
}

//////////////////////////////////////////////////////////////////////

tString CRootItem::GetErrorText(LPCSTR strEnumSect, tERROR err)
{
	tCHAR strKey[40]; _snprintf(strKey, countof(strKey), "%u", err);
	tString strVal; GetString(strVal, PROFILE_LOCALIZE, strEnumSect, strKey);
	return strVal;
}

//////////////////////////////////////////////////////////////////////

void CRootItem::ReportError(tTRACE_LEVEL level, LPCSTR strFmt, ...)
{
	if( !m_pSink )
		return;

	char szText[1024];
	
	va_list args; va_start(args, strFmt);
	_vsnprintf(szText, countof(szText), strFmt, args);
	va_end(args);
	
	szText[countof(szText) - 1] = 0;
	
	((CRootSink *)m_pSink)->ReportError(level, szText);
}

//////////////////////////////////////////////////////////////////////

void CRootItem::OnTimer(UINT_PTR nTimerId)
{
	if( nTimerId == m_idTimerRefresh )
	{
		TimerRefresh(m_nTimerSpin++);
		m_tmLastTimer = PrGetTickCount();
	}
	else if( nTimerId == m_idTimerAnimation )
	{
		for(size_t i = 0; i < m_vecAniItems.size(); i++)
		{
			CItemBase* pItem = m_vecAniItems[i];
			if( pItem->m_pBgCtx )
			{
				tDWORD nCount = 1;
				if( pItem->m_pBackground )
					nCount = pItem->m_pBackground->Count();

				long& nFrame = pItem->m_pBgCtx->m_nCurFrame;
				nFrame++;
				if( (tDWORD)nFrame >= nCount )
					nFrame = 0;

				pItem->Update();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////

tERROR CRootItem::SendEvent(tDWORD nEventId, cSerializable* pData, tDWORD nFlags, CItemBase* pSendPoint, tDWORD nTimeOut)
{
	tSyncEventData * pEvent = new tSyncEventData(IsOwnThread(), nEventId, nFlags, pData, pSendPoint, nTimeOut);

	if( nFlags & EVENT_SEND_WAIT_GLOBAL )
		WaitEvent(pEvent, m_hGSyncer);
	
	if( PR_SUCC(pEvent->errResult) )
		SendEvent(pEvent);

	if( PR_SUCC(pEvent->errResult) )
		WaitEvent(pEvent, pEvent->hWaiter);

	if( nFlags & EVENT_SEND_WAIT_GLOBAL )
		m_hGSyncer->Release();

	if( (nFlags & EVENT_SEND_SYNC) && !(nFlags & EVENT_SEND_OWN_DATA) && pEvent->pData )
		pData->assign(*pEvent->pData, false);

	tERROR nResult = pEvent->errResult;
	pEvent->Release();
	return m_bActive ? nResult : errOBJECT_BAD_INTERNAL_STATE;
}

void CRootItem::SendEvent(tSyncEventData * pEvent)
{
	if( pEvent->m_bOwnThread && (pEvent->nFlags & EVENT_SEND_SYNC) )
	{
		ProcessEvent(pEvent);
		return;
	}

	pEvent->AddRef();

	if( pEvent->nFlags & EVENT_SEND_SYNC )
		m_hELocker->Enter(SHARE_LEVEL_WRITE);

	{
		cAutoCS locker(m_hQLocker, true);
		m_vecQueue.push_back(pEvent);
	}

	PostProcessEventQueue();

	if( pEvent->nFlags & EVENT_SEND_SYNC )
	{
		WaitEvent(pEvent, m_hESyncer);
		m_hELocker->Leave(NULL);
	}
}

void CRootItem::WaitEvent(tSyncEventData* pEvent, tSyncer& pSyncer)
{
	if( !pSyncer )
		return;

	if( !pEvent->nTimeout )
	{
		pSyncer->Lock(-1);
		return;
	}

	tLONG tmWaitBegin = PrGetTickCount();
	while(1)
	{
		PR_TRACE((g_root, prtIMPORTANT, "GUI:: WaitEvent..."));
		if( !m_bGuiActive )
		{
			PR_TRACE((g_root, prtERROR, "GUI:: Console is inactive => Release AskAction"));
			break;
		}

		tLONG tmNow = PrGetTickCount();

		tLONG dwDiff = tmNow - m_tmLastTimer;
		if( (tDWORD)dwDiff > pEvent->nTimeout )
		{
			PR_TRACE((g_root, prtERROR, "GUI:: Possible deadlock => Release AskAction"));
			break;
		}

		if( (tDWORD)tmWaitBegin > m_tmLastUserInput )
			dwDiff = tmNow - tmWaitBegin;
		else
			dwDiff = tmNow - m_tmLastUserInput;

		if( (tDWORD)dwDiff > 30 * pEvent->nTimeout )
		{
			PR_TRACE((g_root, prtERROR, "GUI:: No user input (now(%d),lastinput(%d),timeout(%d))=> Release AskAction",
				tmNow, m_tmLastUserInput, pEvent->nTimeout));
			break;
		}

		if( PR_SUCC(pSyncer->Lock(500)) )
			return;
	}

	pEvent->errResult = errTIMEOUT;
}

void CRootItem::ProcessEvent(tSyncEventData* pEvent)
{
	if( !pEvent )
		return;

	tSyncEventData * pOldCurEvent;
	if( pEvent->nFlags & EVENT_SEND_SYNC )
	{
		pOldCurEvent = m_pCurEvent;
		m_pCurEvent = pEvent;
	}

	CItemBase * pSendPoint = NULL;
	if( pEvent->pSendPoint )
	{
		if( pEvent->pSendPoint->m_pRoot )
			pSendPoint = pEvent->pSendPoint;
	}
	else
		pSendPoint = this;
	
	if( pEvent->nEventId == EVENT_ID_ICONEXTRUCTED )
	{
		cGuiParams &ctx = *(cGuiParams *)pEvent->pData.ptr_ref();
		CItemBase *pItemToNotify = (CItemBase *)ctx.m_nVal1;
		CObjectInfo *pObjInfo = (CObjectInfo *)ctx.m_nVal2;
		
		tString strIconKey; LocalizeStr(strIconKey, pObjInfo->m_strName.data(), pObjInfo->m_strName.size());
		if( pObjInfo->m_nQueryMask & OBJINFO_LARGEICON )
			strIconKey += ":L";
		
		PutIconInCache(strIconKey.c_str(), pObjInfo->m_pIcon);

		bool bAlive = !!pItemToNotify->m_pRoot;
		pItemToNotify->Release();
		if( bAlive )
			pSendPoint = pItemToNotify;
	}

	if( pSendPoint )
		pSendPoint->SendEvent(pEvent->nEventId, pEvent->pData, !(pEvent->nFlags & EVENT_SEND_TO_ROOT));

	if( pEvent->nFlags & EVENT_SEND_SYNC )
		m_pCurEvent = pOldCurEvent;
}

void CRootItem::ProcessEventQueue()
{
	while( m_vecQueue.size() )
	{
		tSyncEventData * pEvent = NULL;
		{
			cAutoCS locker(m_hQLocker, true);
			pEvent = m_vecQueue.at(0);
			m_vecQueue.erase(m_vecQueue.begin());
		}

		ProcessEvent(pEvent);

		if( (pEvent->nFlags & EVENT_SEND_SYNC) && (pEvent->errResult != errTIMEOUT) )
			m_hESyncer->Release();

		pEvent->Release();
	}
}

void CRootItem::OnAction(tWORD nEventId, CItemBase * pItem, void * pParam)
{
	switch( nEventId )
	{
	case ITEM_ACTION_DESTROY:
		{
			RemoveDialog((CDialogItem *)pItem);

			if( !m_vecDialogs.size() )
			{
				if( m_pSink && ((CRootSink*)m_pSink)->CanClearResources() )
					ClearResources();
				SwapMemory();
			}
		}
		break;

	case ITEM_ACTION_CLOSEDLG:
		{
			tDWORD nId = (tDWORD)pParam;
			for(size_t i = 0; i < m_vecDialogs.size(); i++)
			{
				CDialogItem * pDlg = m_vecDialogs[i];
				if( pDlg->m_pSink && ((CDialogSink *)pDlg->m_pSink)->OnGetId() == nId )
				{
					pDlg->Close();
					break;
				}
			}
		}
		break;
	
	case ITEM_ACTION_CLICK:    pItem->Click(); break;
	case ITEM_ACTION_SELECT:   pItem->Select(); break;
	}
}

void CRootItem::SetTrackItem(CItemBase* pItem)
{
	if( m_pTrackItem == pItem )
		return;

	CItemBase *pOldHotlight = NULL;
	CItemBase *pNewHotlight = NULL;

	if( m_pTrackItem )
	{
		if( m_pTrackItem->m_pRoot )
		{
			pOldHotlight = m_pTrackItem->GetByFlagsAndState(STYLE_CLICKABLE, ISTATE_ALL, NULL, gbfasUpToParent);
			OnMouseLeave(m_pTrackItem);
		}
	}

	m_pTrackItem = pItem;

	if( m_pTrackItem )
	{
		if( m_pTrackItem->m_pRoot )
		{
			pNewHotlight = m_pTrackItem->GetByFlagsAndState(STYLE_CLICKABLE, ISTATE_ALL, NULL, gbfasUpToParent);
			OnMouseHover(m_pTrackItem);
		}
	}

	if( pOldHotlight != pNewHotlight )
	{
		if( pOldHotlight )
			pOldHotlight->Hotlight(false);
		if( pNewHotlight )
			pNewHotlight->Hotlight(true);
	}
}

//////////////////////////////////////////////////////////////////////

void CRootItem::AddDialog(CDialogItem* pItem)
{
	m_vecDialogs.push_back(pItem);
	pItem->m_pUserData = m_pCurEvent;
	if( m_pCurEvent )
		m_pCurEvent->AddRef();
}

void CRootItem::RemoveDialog(CDialogItem* pItem)
{
	if( pItem->m_pUserData )
	{
		tSyncEventData* pData = (tSyncEventData*)pItem->m_pUserData;
		if( !!pData->hWaiter )
		{
			m_nWaitDialogs--;
			pData->hWaiter->Release();
		}

		pData->Release();
	}

	pItem->Destroy();

	for(size_t i = 0; i < m_vecDialogs.size(); i++)
		if( m_vecDialogs[i] == pItem )
		{
			m_vecDialogs.erase(m_vecDialogs.begin()+i);
			break;
		}
}

void CRootItem::DoModalDialog(CDialogItem * pItem, bool bForce)
{
	if( !bForce )
	{
		tSyncEventData * pData = (tSyncEventData *)pItem->m_pUserData;
		if( pData && !pData->m_bOwnThread )
		{
			if( pData->nFlags & EVENT_SEND_WAIT_DIALOG )
			{
				pData->hWaiter.Init(0);
				m_nWaitDialogs++;
			}
			
			return;
		}
	}

	CDialogItem * pOldTopModalDialog = m_pTopModalDialog;
	m_pTopModalDialog = pItem;
	
	bool bParent = m_pTopModalDialog->m_pParent == pOldTopModalDialog;
	if( pOldTopModalDialog && !bParent )
		pOldTopModalDialog->Enable(false);
	
	pItem->ModalEnableParent(false);
	
	pItem->m_fModal = 1;
	MessageLoop(pItem);
	pItem->m_fModal = 0;

	m_pTopModalDialog = pOldTopModalDialog;
	if( pOldTopModalDialog && !bParent )
		pOldTopModalDialog->Enable(true);
}

bool CRootItem::CloseAllDlgs(bool bUser)
{
	bool ok = true;
	for(size_t i = 0; i < m_vecDialogs.size(); i++)
	{
		CDialogItem * pDlg = m_vecDialogs[i];
		if( pDlg->m_pSink && !pDlg->m_pSink->OnCanClose(bUser) )
			ok = false;
		else
			pDlg->Close();
	}

	return ok;
}

void CRootItem::CloseDlg(tDWORD nId)
{
	PostAction(ITEM_ACTION_CLOSEDLG, NULL, (void *)nId);
}

void CRootItem::SetDialogErrResult(CDialogItem* pItem, tERROR err)
{
	if( tSyncEventData* pData = (tSyncEventData*)pItem->m_pUserData )
		pData->errResult = err;
}

void CRootItem::RefreshDialog(CDialogItem* pItem)
{
	if( !pItem )
	{
		size_t i;
		
		for(i = 0; i < m_vecDialogs.size(); i++)
			RefreshDialog(m_vecDialogs[i]);

		for(i = 0; i < m_aItems.size(); i++)
		{
			CItemBase * pItem = m_aItems[i];
			if( !(pItem->m_nFlags & STYLE_TOP_ITEM) )
				pItem->ReLoad();
		}
		
		return;
	}

	if( pItem->m_nState & ISTATE_CLOSING )
		return;

	bool bActive = pItem->IsActive();

	pItem->ReLoad();

	if( bActive )
		pItem->Activate();
}

void CRootItem::ShowDlg(CDialogSink* pSink, bool bModal, bool bSingleton, CItemBase * pParent)
{
	if( !pSink )
		return;

	if( pParent )
		pParent = pParent->GetOwner(true);
	if( !pParent )
		pParent = this;
	
	LPCSTR strId = pSink->m_strItemId.empty() ? pSink->m_strSection.c_str() : pSink->m_strItemId.c_str();
	CDialogItem* pItem = NULL;
	if( bSingleton )
	{
		while( pItem = (CDialogItem*)pParent->GetNextItem(pItem, strId, NULL, false) )
		{
			if( pItem->m_strItemType != GUI_ITEMT_DIALOG )
				continue;
			if( pItem->m_pSink && !((CDialogSink*)pItem->m_pSink)->ReInit(pSink) )
				continue;
			
			pItem->Activate();
			delete pSink;
			return;
		}
	
		pItem = NULL;
	}
	
	pParent->LoadItem(pSink, (CItemBase*&)pItem, pSink->m_strSection.c_str(),
		pSink->m_strItemId.c_str(), LOAD_FLAG_DESTROY_SINK);

	if( !pItem )
	{
		delete pSink;
		return;
	}

	if( pItem->m_strItemType != GUI_ITEMT_DIALOG )
	{
		pItem->Destroy();
		return;
	}

	pItem->Activate();
	if( pItem && bModal )
		pItem->DoModal();
}

tDWORD CRootItem::GetWaitDlgsCount()
{
	return m_nWaitDialogs;
}

tERROR& CRootItem::GetCurEventResult()
{
	if( m_pCurEvent )
		return m_pCurEvent->errResult;
	
	static tERROR g_errRes = errOK; return g_errRes;
}

//////////////////////////////////////////////////////////////////////

tSIZE_T CRootItem::TrimString(tString& strVal)
{
	if( strVal.empty() )
		return 0;
	
	size_t i, s;
	LPCSTR strTrim;
	
	strTrim = strVal.c_str();
	for(s = 0, i = 0;                 _kav_isspace(strTrim[i]); s++, i++);
	if( s ) strVal.erase(0, s);

	if( strVal.empty() )
		return 0;

	strTrim = strVal.c_str();
	for(s = 0, i = strVal.size() - 1; _kav_isspace(strTrim[i]); s++, i--);
	if( s ) strVal.erase(strVal.size() - s, s);

	return strVal.size();
}

tSIZE_T CRootItem::TrimString(cStrObj& strVal)
{
	if( strVal.empty() )
		return 0;
	
	int i, s;
	const tWCHAR* strTrim;
	
	strTrim = strVal.data();
	for(s = 0, i = 0;                 _kav_isspace((unsigned char)strTrim[i]); s++, i++);
	if( s ) strVal.erase(0, s);

	if( strVal.empty() )
		return 0;

	strTrim = strVal.data();
	for(s = 0, i = strVal.size() - 1; _kav_isspace((unsigned char)strTrim[i]); s++, i--);
	if( s ) strVal.erase(strVal.size() - s, s);

	return strVal.size();
}

//////////////////////////////////////////////////////////////////////

void CRootItem::cNodeIsThemed::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get ) return;
	CRootItem * p = OWNEROFMEMBER_PTR(CRootItem, m_nodeIsThemed);
	value = p->IsThemed();
}

cNode * CRootItem::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	sswitch(name)
	stcase(isOsGuiThemed) return &m_nodeIsThemed; sbreak;
	send;
	
	return CItemBase::CreateOperand(name, container, al);
}

//////////////////////////////////////////////////////////////////////

void CIconExctructTask::do_work()
{
	CIcon * pDefIcon = m_pIcon; m_pIcon = NULL;

	m_nQueryMask |= OBJINFO_ICON;

	if( m_bExpandPath && m_pRoot->m_pSink )
	{
		cStrObj strName = m_strName;
		if( ((CRootSink *)m_pRoot->m_pSink)->ExpandEnvironmentString(m_strName) )
		{
			m_pRoot->GetObjectInfo(this);
			m_strName = strName;
		}
	}
	else
		m_pRoot->GetObjectInfo(this);

	if( !m_pIcon )
		if( m_pIcon = pDefIcon )
			pDefIcon->AddRef();

	cGuiParams ctx;
	ctx.m_nVal1 = m_pItemToNotify;
	ctx.m_nVal2 = (CObjectInfo *)this;
	m_pRoot->SendEvent(EVENT_ID_ICONEXTRUCTED, &ctx, EVENT_SEND_SYNC|EVENT_SEND_OWN_DATA|EVENT_SEND_TO_ROOT);
}

//////////////////////////////////////////////////////////////////////

bool CRootItem::GetObjectInfo(CObjectInfo * pObjInfo)
{
	tDWORD& nMask = pObjInfo->m_nQueryMask;
	tDWORD& nObjType = pObjInfo->m_nType;

	if( nObjType == SHELL_OBJTYPE_CUSTOM )
		return m_pSink ? ((CRootSink *)m_pSink)->GetCustomObjectInfo(*pObjInfo) : false;

	if( nObjType == SHELL_OBJTYPE_UNKNOWN )
	{
		if( pObjInfo->m_strName.find(L"://") != cStrObj::npos )
			return nObjType = SHELL_OBJTYPE_URL, true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////

bool CRootItem::AnimateItem(CItemBase* pItem, bool bAnimate)
{
	if( !!(pItem->m_nState & ISTATE_ANIMATE) == bAnimate )
		return true;
	
	for(size_t i = 0; i < m_vecAniItems.size(); i++)
	{
		if( m_vecAniItems[i] == pItem )
		{
			if( bAnimate )
				return true;
			
			pItem->m_nState &= ~ISTATE_ANIMATE;
			m_vecAniItems.erase(m_vecAniItems.begin()+i);
			if( !m_vecAniItems.size() )
				SetAnimationTimer(0);
			return true;
		}
	}

	if( !bAnimate )
		return false;

	if( !m_vecAniItems.size() )
		SetAnimationTimer(100);

	m_vecAniItems.push_back(pItem);
	pItem->m_nState |= ISTATE_ANIMATE;
	return true;
}

cVariant * CRootItem::GetGlobalVar(const char *sName, bool bCreateNew)
{
	if( bCreateNew )
		return &m_mapVariants.insert(tMapVariants::value_type(sName, cVariant())).first->second;

	tMapVariants::iterator i = m_mapVariants.find(sName);
	if( i == m_mapVariants.end() )
		return NULL;
	return &i->second;
}

//////////////////////////////////////////////////////////////////////

LPCSTR CRootItem::LocalizeStr(tString &strResult, LPCWSTR strSource, size_t nSize)
{
	utf16_to_utf8(strSource, nSize, strResult);
	return strResult.c_str();
}

LPCWSTR CRootItem::LocalizeStr(cStrObj &strResult, LPCSTR strSource, size_t nSize)
{
	utf8_to_utf16(strSource, nSize, strResult);
	return strResult.data();
}

//////////////////////////////////////////////////////////////////////

CItemBase * CRootItem::CreateItem(LPCSTR strId, LPCSTR strType)
{
	return new CItemBase();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
