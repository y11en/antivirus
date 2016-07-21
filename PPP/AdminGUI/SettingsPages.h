
#pragma once

#include "stdafx.h"
#include "SettingsWindow.h"
#include "SettingsDlgsAdm.h"

#define SECTID_POLICYSETTINGSAK         "PolicySettingsAK"
#define SECTID_PRODUCTSETTINGSAK        "ProductSettingsAK"
#define SECTID_TASKSETTINGSAK           "TaskSettingsAK"
#define SECTID_INSTALLPREPARE           "InstallPrepare"
 
//////////////////////////////////////////////////////////////////////////
// CSettingsViewAdm

class CSettingsViewAdm : public CSettingsView
{
public:
	typedef CSettingsView TBase;

	CSettingsViewAdm(CProfile * pProfilesList) : CSettingsView(pProfilesList) {}
};

//////////////////////////////////////////////////////////////////////////
// CPageBase

class CPageBase : public CPageConfigurator<CDialogSink>
{
public:
	typedef CPageConfigurator<CDialogSink> TBase;
	
	~CPageBase();
	
	bool           CreatePage(CONFIGURATOR_TYPES eCfgType, PAGE_TYPES ePageType, LPCSTR sTaskID, IEnumInterfaces* pIfaces, HWND hParentWnd);

protected:
	void           OnEvent(tDWORD nEventId, cSerializable* pData);

protected:
	void           LoadPage(LPCSTR strItemId, CItemBase *&pPage, LPCSTR strProfile = NULL);
	virtual bool   OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces);
};

//////////////////////////////////////////////////////////////////////////
// CTaskSettingsBase

class CTaskSettingsBase : public CPageBase
{
public:
	typedef CPageBase TBase;

	CTaskSettingsBase();
	~CTaskSettingsBase();
	
	SINK_MAP_BEGIN()
		SINK_DYNAMIC("settingsview", CSettingsViewAdm(m_pProfile))
	SINK_MAP_END(TBase)

protected:
	bool           OnOk();
	bool           OnApply();
	virtual bool   CheckSettings();

protected:
	bool           OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces);
	BOOL           IsPageDirty();

	CProfileAdm *  m_pProfile;
	CProfileAdm *  m_pProfilesList;

private:
	bool           m_bAlreadyValidated;
	CProfileAdm    m_Profile;
};

//////////////////////////////////////////////////////////////////////////
// CTaskSettingsPage

class CTaskSettingsPage : public CTaskSettingsBase
{
public:
	typedef CTaskSettingsBase TBase;
	
	CTaskSettingsPage();

	SINK_MAP_BEGIN()
		SINK_DYNAMIC("licmgr", CLicMgrDlg(m_pProfile))
	SINK_MAP_END(TBase)

protected:
	virtual void   OnInited();
};

//////////////////////////////////////////////////////////////////////////
// CPolicySettingsPage

class CPolicySettingsPage : public CTaskSettingsBase
{
public:
	typedef CTaskSettingsBase TBase;

	CPolicySettingsPage(LPCSTR srtSection);
	CPolicySettingsPage(CProfileAdm * pProfile);
	
protected:
	virtual void   OnInit();
	virtual void   OnInited();
	virtual void   OnChangedData(CItemBase* pItem);

protected:
	virtual bool   OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces);

protected:
	CComboItem *   m_pPageCombo;
	CItemBase *    m_pActivePanel;
	tString        m_strCurrent;
};

//////////////////////////////////////////////////////////////////////////
// CWizardPage

class CWizardPage : public CTaskSettingsBase
{
public:
	CWizardPage(CProfileAdm* pProfile, int nPage, int nTotal);

protected:
	bool           OnOk();
	bool           OnApply();
	void           OnInited();
	bool           OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces);

	SINK_MAP_BEGIN()
		SINK_DYNAMIC("policytree", CPolicyTree(m_pProfile))
	SINK_MAP_END(CTaskSettingsBase)

protected:
	int            m_nPage;
	int            m_nTotal;
};

//////////////////////////////////////////////////////////////////////////
// CInstallPrepareDlg

class CInstallPrepareDlg : public CPageBase
{
public:
	typedef CPageBase TBase;
	typedef KLSTD::CAutoPtr<KLPKG::PackageConfigurator> PackagePtr;

	CInstallPrepareDlg();

	SINK_MAP_BEGIN()
		SINK_DYNAMIC("wksinstall", CDialogBindingViewT<>(&m_Params))
	SINK_MAP_END(TBase)

protected:
	virtual void   OnInited();
	virtual bool   OnApply();
	virtual bool   OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces);

	bool           GetSetSettings(bool bGet);

	cMsgParams     m_Params;
	PackagePtr     m_pPackage;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
