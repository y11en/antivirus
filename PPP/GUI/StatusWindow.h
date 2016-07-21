//////////////////////////////////////////////////////////////////////
// StatusWindow.h: interface for the SettingsWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUS_WINDOW_H__6A4B16E2_4BFB_4AF5_94C4_978E8F567B2B__INCLUDED_)
#define AFX_STATUS_WINDOW_H__6A4B16E2_4BFB_4AF5_94C4_978E8F567B2B__INCLUDED_

//////////////////////////////////////////////////////////////////////
// CNewsWindow

class CNewsWindow : public CDialogBindingViewT<>
{
public:
	typedef CDialogBindingViewT<> TBase;

	CNewsWindow(bool bShowOldNews = true);

protected:
	void OnCreate();
	void OnInit();
	bool OnClose(tDWORD& nResult);
	bool OnClicked(CItemBase* pItem);

	void UpdateNews(int nDir);

protected:
	bool			m_bShowOldNews;
	cStringObj		m_NewsItemId;
	cGuiNewsItem	m_NewsItem;
	cVector<cStringObj> m_ReadNews;
};


//////////////////////////////////////////////////////////////////////
// CStatusWindow

class CStatusWindow : public CItemSinkT<>
{
public:
	typedef CItemSinkT<> TBase;
	
	CStatusWindow();
	void UpdateStatus(int nDir = 0, bool bUpdView = true);
	void HideStatus();

public:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_nStateId, tid_DWORD, "ProdStateId")
		BIND_VALUE(m_nStateIdNext, tid_DWORD, "ProdStateIdNext")
		BIND_VALUE(m_nStateIdPrev, tid_DWORD, "ProdStateIdPrev")
	BIND_MEMBER_MAP_END(CItemSink)

	cNode * CreateOperator(const char *name, cNode **args, cAlloc *al);

	void OnInitProps(CItemProps& pProps);
	void OnEvent(tDWORD nEventId, cSerializable * pData);

	tKeys           m_vecStates;
	tQWORD			m_nStatesMask;
	tDWORD          m_nStateId;
	tDWORD          m_nStateIdNext;
	tDWORD          m_nStateIdPrev;
};

//////////////////////////////////////////////////////////////////////
// CStatusBaloon

class CStatusBaloon : public CDialogBindingT<cBLNotification>
{
public:
	typedef CDialogBindingT<cBLNotification> TBase;

	CStatusBaloon() : m_pNodeDisplayText(NULL), m_bFreeze(0), m_nViewTimeout(0) {}
	
	void OnInitProps(CItemProps& pProps);
	bool OnClicked(CItemBase* pItem);
	bool OnClose(tDWORD& nResult);
	void OnDestroy();

	void PushMsg(cBLNotification* pData);
	bool PopMsg(bool bDisable = false);
	bool CheckMsgEqual(cBLNotification * pExst, cBLNotification * pTest);
	bool RefreshMsg();

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_strDisplayText, tid_STRING_OBJ, "DisplayText")
	BIND_MEMBER_MAP_END(CItemSink)

private:
	std::vector<cBLNotification> m_queue;
	tDWORD         m_nViewTimeout;
	
	cStrObj        m_strDisplayText;
	CGuiPtr<cNode> m_pNodeDisplayText;
	unsigned       m_bFreeze : 1;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_STATUS_WINDOW_H__6A4B16E2_4BFB_4AF5_94C4_978E8F567B2B__INCLUDED_)
