// CS AdminKit interfaces implementation
//

#pragma once

// -------------------------------- IKLConfiguratorPage --------------------------------

template <class TItemSink = CDialogSink>
class CPageConfigurator : public KLSTD::KLBaseImpl<IKLConfiguratorPage, true>, public TItemSink
{
public:
	CPageConfigurator() :
		m_hParentWnd(NULL),
		m_eCfgType(IKLConfigurator::GROUP_POLICY_CONFIGURATOR),
		m_ePageType(IKLConfigurator::PROPERTY_PAGE)
	{}
	
	// interface IKLConfiguratorPage
public:
	virtual void                SetInterfaces(IEnumInterfaces* pIfaces)          {}
	virtual HWND                GetPageHWND() const                              { return Item()->GetWindow(); }
	virtual BOOL                IsPageDirty() const                              { return false; }
	virtual void                Apply()                                          { OnApply(); }
	virtual void                ApplyDefaultData()                               {}
	virtual BOOL                ValidateUserInput()                              { return OnOk(); }
	virtual void                NotifyDataUpdated()                              {}
	virtual BOOL                NotifyPageSelecting()                            { return true; }
	virtual void                NotifyPageSelected()                             {}
	virtual BOOL                NotifyPageDeselecting()                          { return true; }
	void                        SetModified()                                    { ::SendMessage(m_hParentWnd, ::RegisterWindowMessage(RWM_KLAKCON_PPN_PAGE_MODIFIED), 0, 0); }

	// interface CItemSink
	// note:
	// this sink should be created without LOAD_FLAG_DESTROY_SINK flag
	// beacouse it contains the GUI root as member
public:
	virtual HWND                OnGetParent()                                    { return m_hParentWnd; }
	virtual void                OnChangedData(CItemBase* pItem)                  { TItemSink::OnChangedData(pItem); SetModified(); }

public:
	virtual bool                CreatePage(CONFIGURATOR_TYPES eCfgType, PAGE_TYPES ePageType, LPCSTR sTaskID, IEnumInterfaces* pIfaces, HWND hParentWnd) = 0;

protected:
	HWND                        m_hParentWnd;
	CONFIGURATOR_TYPES          m_eCfgType;
	PAGE_TYPES                  m_ePageType;
	std::string                 m_sTaskID;
};



// -------------------------------- IKLConfigurator --------------------------------

class CConfigurator : public KLSTD::KLBaseImpl<IKLConfigurator, true>
{
public:
	CConfigurator(CONFIGURATOR_TYPES eCfgType, PAGE_TYPES ePageType, LPCSTR sTaskID);
	virtual ~CConfigurator();

	bool                        Init();

	// interface IKLConfigurator
public:
	virtual void                SetInterfaces(IEnumInterfaces* pIfaces);
	virtual void                ApplyDefaultData();
	virtual unsigned long       GetPagesCount() const;
	virtual IKLConfiguratorPagePtr CreatePage(HWND hParent, unsigned long nIndex);
	virtual void                GetPageHelpInfo(unsigned long nIndex, LPWSTR* pbstrHelpFilePath, long* pnHelpID) const;
	virtual LPCWSTR             GetPageTitle(unsigned long nIndex) const;
	virtual LPCWSTR             GetPageSubtitle(unsigned long nIndex) const;
	virtual HICON               GetPageIcon(unsigned long nIndex) const;

protected:
	class CPageInfo
	{
	public:
		CPageInfo(LPCSTR sTaskId, LPCSTR sPageTitle, LPCSTR sPageSubTitle = "");
		
		std::string  m_sTaskId;
		std::wstring m_sPageTitle;
		std::wstring m_sPageSubTitle;
	};

	void                        AddPageInfo(const CPageInfo &PageInfo);
	bool                        IsPageIndexValid(unsigned long nIndex) const;
	void                        AddEventDefaultSettings(tPTR pParamsOrValue, cBLNotifySettings *pNotifySettings, CRootItem *pRoot);
	
protected:
	CONFIGURATOR_TYPES          m_eCfgType;
	PAGE_TYPES                  m_ePageType; 
	std::string                 m_sTaskID;
	IEnumInterfaces *           m_pIfaces;
	std::vector<CPageInfo>      m_aPageInfos;
	CProfileAdm                 m_Profile;
};



// -------------------------------- IKLLocalizer --------------------------------

class CLocalizer : public IKLLocalizer
{
public:
	CLocalizer(){}
	virtual ~CLocalizer(){}

	// interface IKLLocalizer
public:
	virtual void                LocalizeEvent(UINT nResourceID, KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams1, KLPAR::Params* pTrgParams2);
	virtual void                LocalizeEvent(LPCWSTR lpszFormat, KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams1, KLPAR::Params* pTrgParams2);
	virtual std::wstring        GetLocalizedProductName(LPCWSTR lpszProductName, LPCWSTR lpszProductVersion);
	virtual void                LocalizeStatistic(KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams);

protected:
	void                        LocalizeStatistic(CRootItem *pRoot, LPCSTR strSection, cProfileEx &pProfile, KLPAR::Params* pParams, bool bTask = false);
	void                        LocalizeStatistic(CItemBase *pItem, CViewBinding *pBinding, KLPAR::Params* pParams, bool bTask = false);
	void                        AddSources(CViewBinding *pBinding, cProfileEx &pProfie);
	KLPAR::Params *             GetStatistics(KLPAR::Params *pParams);
};



// -------------------------------- CAccessor --------------------------------

class CAccessor : public IKLAccessor
{
	// interface IKLAccessor
public:
	virtual IKLConfiguratorPtr  CreateConfigurator(CONFIGURATOR_TYPES eCfgType, PAGE_TYPES ePageType, LPCWSTR lpszTaskID = NULL);
	virtual IKLLocalizer*       GetLocalizer();
	virtual void                GetTasks(KLPAR::Params* pParams);
	virtual void                GetEvents(KLPAR::Params* pParams);
	virtual LPCWSTR             GetProductName() const;
	virtual LPCWSTR             GetProductVersion() const;

protected:
	void                        AddEventInfoParamsValue(KLPAR::ArrayValue* pArrayValue, cBLNotifySettings *pNotifySettings, CRootItem *pRoot);

protected:
	CLocalizer                  m_Localizer;
};