// SettingsDlgs.cpp: implementation of the SettingsDlgs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSIfacesImpl.h"
#include "SettingsDlgsAdm.h"
#include "SettingsPages.h"

#include <iface/i_licensing.h>
#include <structs/s_licensing.h>
#include <structs/s_licmgr.h>
#include <plugin/p_win32_nfio.h>

#define IMPEX_IMPORT_LIB
	#include <plugin/p_win32_nfio.h>

//////////////////////////////////////////////////////////////////////////
// CLicMgrDlg

CLicMgrDlg::CLicMgrDlg(CProfile *pProfileList) : 
	TBase(pProfileList),
	m_pSett(NULL)
{
}

bool CLicMgrDlg::OnAddDataSection(LPCTSTR strSect)
{
	bool bRet = TBase::OnAddDataSection(strSect);
	if( !(strSect && *strSect) )
		m_pBinding->AddDataSource(&m_KeyInfo);
	return bRet;
}

#define EVENTID_SETFOCUS 0x377fa0c4 //is a crc32

void CLicMgrDlg::OnInited()
{
	if( m_pProfile && m_pProfile->settings() && m_pProfile->settings()->isBasedOn(cLicMgrSettings::eIID) )
		m_pSett = (cLicMgrSettings *)m_pProfile->settings();

	CheckAndLoadKeyFile(kfCheckFromMemory);
	m_pBinding->UpdateData(true, CStructData(NULL));

	Root()->SendEvent(EVENTID_SETFOCUS);
}

void CLicMgrDlg::OnEvent(tDWORD nEventId, cSerializable *pData)
{
	if( nEventId == EVENTID_SETFOCUS)
		if( CItemBase *pItem = Item()->GetItem("Browse", GUI_ITEMT_BUTTON) )
			SetFocus(pItem->GetWindow());
	
	TBase::OnEvent(nEventId, pData);
}

bool CLicMgrDlg::OnClicked(CItemBase* pItem)
{
	if( pItem->m_strItemId == "Browse" )
		BrowseForKeyFile();

	return TBase::OnClicked(pItem);
}

bool CLicMgrDlg::OnOk()
{
	if( !m_pSett )
		return false;

	if( m_pSett->m_sFileName.empty() )
	{
		MsgBox(*this, "errNeedKey", NULL, MB_OK|MB_ICONERROR);
		return false;
	}

	if( m_KeyInfo.m_LicInfo.m_dwInvalidReason != ekirValid )
	{
		MsgBox(*this, "KeyInvalidReason", NULL, MB_OK|MB_ICONSTOP, &m_KeyInfo.m_LicInfo);
		return false;
	}

	return TBase::OnOk();
}

bool CLicMgrDlg::BrowseForKeyFile()
{
	if( !(m_pSett && Root()->BrowseFile(*this, NULL, "key", m_pSett->m_sFileName)) )
		return false;

	CheckAndLoadKeyFile(kfCheckFromFile);

	Item()->SetChangedData();
	
	return true;
}

tERROR CLicMgrDlg::CheckAndLoadKeyFile(tCheckMode eMode)
{
	cObject *pParent = g_hGui->Obj();

	if( !m_pSett || !pParent )
		return errNOT_INITIALIZED;

	
	tERROR err = errOK;

	cAutoObj<cIO> pTmpIo;
	cStringObj strKeyFile;
	if( eMode == kfCheckFromMemory )
	{
		tMemChunk &mem = *(tMemChunk *)&m_pSett->m_aLicBody;
		if( PR_SUCC(err) ) err = PrGetTempFileName(cAutoString(strKeyFile), NULL, cCP_ANSI);
		if( PR_SUCC(err) ) err = pParent->sysCreateObject(pTmpIo, IID_IO, PID_WIN32_NFIO);
		if( PR_SUCC(err) ) err = strKeyFile.copy(pTmpIo, pgOBJECT_FULL_NAME);
		if( PR_SUCC(err) ) err = pTmpIo->set_pgOBJECT_ACCESS_MODE(fACCESS_WRITE);
		if( PR_SUCC(err) ) err = pTmpIo->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
		if( PR_SUCC(err) ) err = pTmpIo->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_DELETE|fOMODE_SHARE_DENY_WRITE);
		if( PR_SUCC(err) ) err = pTmpIo->sysCreateObjectDone();
		if( PR_SUCC(err) ) err = pTmpIo->SeekWrite(NULL, 0, mem.m_ptr, mem.m_used);
	}
	else
	{
		strKeyFile = m_pSett->m_sFileName;
	}
	
	cAutoObj<cIO> pLicIo;
	cAutoObj<cLicensing> pLicSrv;
	if( PR_SUCC(err) ) err = pParent->sysCreateObjectQuick(pLicIo, IID_IO, PID_TMPFILE);
	if( PR_SUCC(err) ) err = pParent->sysCreateObject(pLicSrv, IID_LICENSING);
	if( PR_SUCC(err) ) err = pLicSrv->propSetBool(plUSES_BLACKLIST, cFALSE);
	if( PR_SUCC(err) ) err = pLicSrv->set_pgSTORAGE(pLicIo);
	if( PR_SUCC(err) ) err = pLicSrv->sysCreateObjectDone();
	if( PR_SUCC(err) ) err = pLicSrv->CheckKeyFile((wchar_t *)strKeyFile.data(), KAM_ADD, &m_KeyInfo.m_LicInfo);
	if( PR_SUCC(err) && eMode == kfCheckFromFile )
	{
		cAutoObj<cIO> pIo;
		tQWORD nFileSize;
		tMemChunk &mem = *(tMemChunk *)&m_pSett->m_aLicBody;
		if( PR_SUCC(err) ) err = pParent->sysCreateObject(pIo, IID_IO, PID_WIN32_NFIO);
		if( PR_SUCC(err) ) err = pIo->propSetStr(0, pgOBJECT_FULL_NAME, strKeyFile.c_str(cCP_ANSI));
		if( PR_SUCC(err) ) err = pIo->sysCreateObjectDone();
		if( PR_SUCC(err) ) err = pIo->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT);
		if( PR_SUCC(err) ) err = pParent->heapRealloc(&mem.m_ptr, mem.m_ptr, mem.m_allocated = (tDWORD)nFileSize);
		if( PR_SUCC(err) ) err = pIo->SeekRead(&mem.m_used, 0, mem.m_ptr, mem.m_allocated);
	}
	return err;
}

//////////////////////////////////////////////////////////////////////////
// CPolicyTree

#define SECTID_AKPOLICYWIZARD "AkPolicyWizard"

class CDlgConfigSink : public CPageBase
{
public:
	typedef CPageBase TBase;

	CDlgConfigSink(CProfileAdm *pProfile) :
		m_Profile(*pProfile),
		m_pProfile(&m_Profile),
		m_pProfileOrig(pProfile)
	{}
	
	~CDlgConfigSink() {};
	
	SINK_MAP_BEGIN()
		SINK_DYNAMIC("settingsview", CSettingsViewAdm(m_pProfile))
	SINK_MAP_END(TBase)

protected:
	virtual bool   OnOk();
	virtual void   OnInit();
	virtual void   OnInited();

	CProfileAdm    m_Profile;
	CProfileAdm *  m_pProfile;
	CProfileAdm *  m_pProfileOrig;
};

void CDlgConfigSink::OnInit()
{
	if( CItemBase *pHelp = Item()->GetItem("Help") )
	{
		CItemBinding *pBinding = Root()->CreateItemBinding(pHelp);
		pBinding->Init();
		pBinding->UpdateData(true, CStructData(NULL, STRUCTDATA_ALLSECTIONS));
		pBinding->Destroy();
	}
	
	TBase::OnInit();
}

void CDlgConfigSink::OnInited()
{
	CItemBase *pPage = NULL;
	LoadPage(m_pProfile->GetName().c_str(cCP_ANSI), pPage);
	TBase::OnInited();
}

bool CDlgConfigSink::OnOk()
{
	m_pProfileOrig->CopyFrom(*(cProfileEx*)m_pProfile);
	return TBase::OnOk();
}

void CPolicyTree::OnInited()
{
	if( !m_pBinding->Bind(Item(), CProfile::eIID) )
		return;

	m_btnConfig = Item()->m_pParent->GetItem("Configure");
	if( m_btnConfig )
		m_btnConfig->AttachSink(new CConfigSink(this), true);

	m_icoLocked = Item()->AddIcon(Root()->GetIcon("locked"));
	m_icoUnlocked = Item()->AddIcon(Root()->GetIcon("unlocked"));

	tKeys aKeys;
	Root()->GetKeys(PROFILE_LOCALIZE, SECTID_AKPOLICYWIZARD, aKeys);

	tDWORD nCount = aKeys.size();
	m_aItems.alloc(nCount);
	for(tDWORD i = 0; i < nCount; i++)
	{
		tString &strKey = aKeys[i];
		tDWORD nLevel = 0;
		CProfileAdm *pProfile = FindProfile(m_pProfileList, strKey.c_str(), nLevel);
		if( !pProfile )
			continue;

		CProfilePolicy &el = m_aItems.push_back();
		el.m_nLevel = nLevel;
		el.m_bLocked = false;
		el.m_pProfile = pProfile;
		el.m_pProfile->m_sDescription = Root()->GetString(tString(), PROFILE_LOCALIZE, SECTID_AKPOLICYWIZARD, strKey.c_str());
		
		tString strFields; Root()->GetString(strFields, PROFILE_LAYOUT, SECTID_AKPOLICYWIZARD, strKey.c_str());
		CItemProps *prp = Root()->CreateProps(strFields.c_str(), tString());
		el.m_LbHolder.Init(prp->Get(HOLDERID_FIELDS), Root());
		prp->Destroy();
	}
	
	Item()->InitData();
	OnChangedState(NULL);
}

CProfileAdm* CPolicyTree::FindProfile(CProfileAdm* pProfileList, LPCSTR strProfile, tDWORD& nLevel)
{
	if( pProfileList->m_sName == strProfile )
		return pProfileList;

	nLevel++;
	tDWORD i, nCount = pProfileList->GetChildrenCount();
	for(i = 0; i < nCount; i++)
		if( CProfileAdm* pProfile = FindProfile((CProfileAdm*)pProfileList->GetChild(i), strProfile, nLevel) )
			return pProfile;
	nLevel--;
	return NULL;
}

bool CPolicyTree::GetRecord(CRecord &pRecord)
{
	CProfilePolicy& el = m_aItems.at(pRecord.m_nPos);
	if( !m_pBinding->InitRecord(el.m_pProfile, pRecord) )
		return false;

	pRecord.m_nIndent = el.m_nLevel;
	
	tString strResult;
	el.m_LbHolder.Resolve(false, el.m_pProfile, strResult);
	if( el.m_bLocked = (strResult == "1") )
		pRecord.m_pData->m_Status = m_icoLocked;
	else
		pRecord.m_pData->m_Status = m_icoUnlocked;

	return true;
}

long CPolicyTree::GetRecordsCount()
{
	return m_aItems.size();
}

bool CPolicyTree::UpdateRecord(CRecord &pRecord)
{
	CProfilePolicy& el = m_aItems.at(pRecord.m_nPos);
	return m_pBinding->UpdateRecord(el.m_pProfile, pRecord);
}

bool CPolicyTree::OnListEvent(int nItem, int nColumn, int nEvent)
{
	if( nEvent == VLISTCTRL_EVENT_CHECKED )
		ProcessCheck(nItem, nColumn);
	
	return CListBindingSink::OnListEvent(nItem, nColumn, nEvent);
}

void CPolicyTree::OnClickIcon(int nItem)
{
	CProfilePolicy& el = m_aItems.at(nItem);

	el.m_bLocked = !el.m_bLocked;
	el.m_LbHolder.Resolve(true, el.m_pProfile, tString(el.m_bLocked ? "1" : ""));

	Item()->UpdateItem(nItem);
}

void CPolicyTree::OnChangedState(CItemBase * pItem, tDWORD nStateMask)
{
	if( m_btnConfig )
		m_btnConfig->Enable(Item()->GetSelectedCount() != 0);
}

bool CPolicyTree::OnConfigure()
{
	int nSel = Item()->GetNextSelectedItem();
	if( nSel == -1 )
		return false;

	CProfilePolicy& el = m_aItems.at(nSel);
	CDlgConfigSink(el.m_pProfile).DoModal(*this, "PolicyConfigureAK");
	Item()->UpdateView(UPDATE_FLAG_SOURCE);
	return true;
}

tDWORD CPolicyTree::OnGetRecordIndent(tDWORD nSrcPos)
{
	return m_aItems.at(nSrcPos).m_nLevel;
}

//////////////////////////////////////////////////////////////////////////
