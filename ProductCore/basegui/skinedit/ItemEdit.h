#if !defined __GUIITEMEDIT_H__
#define __GUIITEMEDIT_H__

class CMainFrame;

// ########################################################################
// CGuiItemEdit

class CGuiItemEdit : public CWindowImpl<CGuiItemEdit, ATL::CWindow>
{
public:
	class CTracker
	{
	public:
		// Флаги, задающие активные части рамки
		enum eTSF
		{
			TSF_HATCHEDBORDER = 1,
			TSF_APPEALBORDER = 2,
			TSF_MIDDLE = 4,
			TSF_TOP = 8,
			TSF_BOTTOM = 16,
			TSF_LEFT = 32,
			TSF_RIGHT = 64,
			TSF_TOPLEFT = 128,
			TSF_TOPRIGHT = 256,
			TSF_BOTTOMLEFT = 512,
			TSF_BOTTOMRIGHT = 1024,
			TSF_TRACKALL = (TSF_MIDDLE|TSF_TOP|TSF_BOTTOM|TSF_LEFT|TSF_RIGHT|TSF_TOPLEFT|TSF_TOPRIGHT|TSF_BOTTOMLEFT|TSF_BOTTOMRIGHT)
		};

		// Вспомогательный тип - перечислитель активных [и не очень]
		// регионов рамки [<!> поддерживает битовые операции <!>]
		typedef enum
		{
			hitNothing = 0, hitMiddle = 1, hitBorder = 3, hitTop = 7,
			hitBottom = 11, hitLeft = 19, hitRight = 35,
			hitTopLeft = 23, hitTopRight = 39,
			hitBottomLeft = 27, hitBottomRight = 43		 
		} TrackerHit;

		// Виды региона обновления
		typedef enum
		{
			rtLeft = 1,
			rtTop = 2,
			rtRight = 4,
			rtBottom = 8,
			rtFull = 15,
			rtOutRect = 31
		} RegionType;

	public:
		CTracker(BYTE width = 6);

		void Draw(CDC& dc);
		bool SetCursor(HWND hWnd, UINT hitTest) const;
		bool SetCursor(TrackerHit hitTest) const;
		bool Track(HWND hWnd, const CPoint& pt, bool bRubberBand = false) const;

	// Operations
	public:
		void		GetEraseRegion(/*[out]*/ HRGN& hRgn, unsigned short rt = rtFull) const;// return hRgn like strong ref
		TrackerHit	HitTest(const CPoint& pt) const;
		BYTE		GetWidth() const {return uWidth;}
		USHORT		SetStyle(USHORT newStyle);                       // задать новый стиль рамки

		static void EnsureSize(CRect& rc, const CSize& szMin, TrackerHit hitTest);  // корректировка размера track-контура
		static void DrawTrackerRect(CDC& dc, const CRect& rc);                      // отрисовка track-контура

	// Private member functions - helpers
	private:
		int getCursorIndex(TrackerHit hitTest) const;

		inline void drawTrackerRect(CDC& dc, const CRect& rc) const;
		inline void drawAppealRect(CDC& dc, const CBrush& br) const;
		inline void setDragCursor(TrackerHit hitTest) const;

		inline static void combineEraseRgn(HRGN& hRgn, HRGN& hTmp, int l, int t, int r, int b);
		inline static void createBackBitmap(CDC& dc, CBitmap& bmp);

		inline static int _x(LPARAM lp);
		inline static int _y(LPARAM lp);

	// Custom data members
	private:
		USHORT m_nStyle;        // заданный пользователем стиль рамки
		CRect m_rect;           // последняя НАРИСОВАННАЯ внешняя рамка
		const BYTE uWidth;      // ширина рамки трекера

		static const BYTE uHatchSpace;  // расстояние между штрихами рамки [2-7]
		static CBitmap _bmpBack;        // фон рамки в режиме TSF_HATCHEDBORDER
		static HCURSOR _hCursors[6];    // используемые во время работы курсоры
	};

	class CDesigner : public CWindowImpl<CDesigner, CWindow, CWinTraits<WS_CHILDWINDOW|WS_VISIBLE|WS_OVERLAPPED, WS_EX_TRANSPARENT|WS_EX_LEFT|WS_EX_RIGHTSCROLLBAR> >
	{
	public:
		DECLARE_WND_CLASS_EX(_T("KL.EditOverlay"), CS_DBLCLKS, -1)    // [-1 - отсутствие brush]

		BEGIN_MSG_MAP(CDesigner)
			MESSAGE_HANDLER(WM_PAINT,			OnPaint)
			MESSAGE_HANDLER(WM_LBUTTONDOWN,		OnLButtonDown)
			MESSAGE_HANDLER(WM_RBUTTONDOWN,		OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONDBLCLK,	OnLButtonDblClk)
			MESSAGE_HANDLER(WM_SETCURSOR,		OnSetCursor)
			MESSAGE_HANDLER(WM_TIMER,			OnTimer)
			MESSAGE_HANDLER(WM_KEYDOWN,			OnKeyDown)
			MESSAGE_HANDLER(WM_CONTEXTMENU,		OnCtxMenu)
		END_MSG_MAP()

		LRESULT OnPaint(UINT, WPARAM wParam, LPARAM, BOOL& bHandled);
		LRESULT OnLButtonDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled);
		LRESULT OnLButtonDblClk(UINT, WPARAM wParam, LPARAM, BOOL& bHandled);
		LRESULT OnSetCursor(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnCtxMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	public: // Tracker
		// Вызывается в процессе изменения размера track-контура до его отрисовки [из функции Track]
		void Tracker_OnChangingRect(/*[in][out]*/ CRect& rc, CTracker::TrackerHit hitTest);
		// Вызывается при отрисовке рамки
		void Tracker_OnGetDrawingRect(/*[out]*/ CRect& rc);
		// Вызывается после изменения размеров рамки
		void Tracker_OnChangedRect(const CRect& rcOld, const CRect& rcNew);
		// Вызывается перед отображением/стиранием track-контура
		void Tracker_OnDrawTrackerRect(CDC& dc, const CRect& rc);

	public:
		CTracker	m_Tracker;
	};

	friend class CComGuiItemImpl;

public:
	typedef CWindowImpl<CGuiItemEdit, ATL::CWindow> TBase;
	
	DECLARE_WND_CLASS(_T("KL.GuiItemEditor"));
	
	BEGIN_MSG_MAP(CGuiItemEdit)
		MESSAGE_HANDLER(WM_CLOSE,		OnClose)
		MESSAGE_HANDLER(WM_SIZE,		OnSize)
		MESSAGE_HANDLER(WM_DESTROY,		OnDestroy)
		MESSAGE_HANDLER(WM_PAINT,		OnPaint)
		MESSAGE_HANDLER(WM_CHILDACTIVATE,	OnChildActivate)
	END_MSG_MAP()

	// DockFloatSplitTab Framework sends WM_CLOSE message
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT, WPARAM wParam, LPARAM, BOOL& bHandled);
	LRESULT OnChildActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	CGuiItemEdit(CMainFrame * pFrame);
	~CGuiItemEdit();
	
	HWND	Create(HWND hWndParent, CRootItem * pRoot, LPCSTR strSection = NULL, bool bExt = false);
	void	Update();
	void	ItemPropsChanged(CItemBase * pItem, bool bSetDirty = true, bool bPropStr = false);
	void	GetSelection(CItemBase::Items& pItems);
	void	SetSelection(tPTR pItem);
	bool	DeleteItem(CItemBase * pItem);
	void	ResizeItem(CItemBase * pItem, const RECT& rcOld, const RECT& rcNew);
	void	CreateItem(tDWORD nIdx);
	void	CreateTopItemFrom(tDWORD nIdx);
	void	InsertExtItem(LPCSTR strSection);
	bool	Save();
	void	Copy(CItemEditData::Cd& _data);
	void	Paste(CItemEditData::Cd& _data);

	CMainFrame *	m_pFrame;
	CRootItem *		m_pRoot;
	CItemBase *		m_pItem;
	CItemBase *		m_pItemOwner;
	CItemBase *		m_pItemCur;
	unsigned		m_bDirty : 1;
	unsigned		m_bUpdateTracker : 1;

	CDesigner		m_Designer;

protected:
	bool	_SelectItem(CItemBase * pItem, tPTR pSelItem);
	bool	_IsSectExist(LPCTSTR strSection);
	void	_GetNewId(CItemBase * pParent, LPCTSTR strPrefix, bool bTrySelf, CString& strItemId);
	void	_GetNewSection(LPCTSTR strPrefix, CString& strSection);
};

// ########################################################################
// CComGuiItem

template <class T, class TUnk = IUnknown>
class _CUnkImpl : public T
{
public:
	LONG m_ref;
	
	_CUnkImpl() : m_ref(1) {}

	TUnk * p() { return (TUnk *)this; }

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
	{
		if( riid == __uuidof(IUnknown) )
			return *(IUnknown **)ppvObject = p(), S_OK;
		return T::QueryInterface(riid, ppvObject);
	}
	ULONG STDMETHODCALLTYPE AddRef()	{ return InterlockedIncrement(&m_ref); }
	ULONG STDMETHODCALLTYPE Release()	{ LONG r = InterlockedDecrement(&m_ref); if( !r ) delete this; return r; }
};

// ########################################################################
// CComGuiItem

class CComGuiItemImpl : public IDispatch, public ITypeInfo
{
public:
	typedef HRESULT FnInvokeProp(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	
	enum eFnGetPropPredefVal{gpvGetCount, gpvGetNextItem, gpvFinish};
	typedef DWORD FnGetPropPredefVal(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);

	struct PropDesc : public FUNCDESC
	{
		PropDesc()
		{
			memset(this, 0, sizeof(FUNCDESC));
			memset(&m_ElemDesc, 0, sizeof(m_ElemDesc));
			m_pFunc = NULL;
			m_pVals = NULL;
			
			invkind = INVOKE_PROPERTYPUT;
			cParams = 1;
		}

		void			Fixup() { lprgelemdescParam = &m_ElemDesc; }
		DISPID&			Id() { return memid; }
		VARTYPE&		Type() { return m_ElemDesc.tdesc.vt; }
		bool			ReadOnly() { return invkind == INVOKE_PROPERTYGET; }

		ELEMDESC				m_ElemDesc;
		std::string				m_Name;
		FnInvokeProp *			m_pFunc;
		FnGetPropPredefVal *	m_pVals;
	};

	class _PredefVals : public IPerPropertyBrowsing
	{
	public:
		_PredefVals() : m_ref(0) {}
			
		HRESULT	STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject) { return E_NOTIMPL; }
		ULONG	STDMETHODCALLTYPE AddRef()
		{
			CComGuiItemImpl& p = *_OWNEROFMEMBER_PTR(CComGuiItemImpl, m_PrefVals);
			return ((IDispatch *)&p)->AddRef(), InterlockedIncrement(&m_ref);
		}
		
		ULONG	STDMETHODCALLTYPE Release()
		{
			CComGuiItemImpl& p = *_OWNEROFMEMBER_PTR(CComGuiItemImpl, m_PrefVals);
			LONG r = InterlockedDecrement(&m_ref); if( !r ) m_Vals.clear();
			return ((IDispatch *)&p)->Release(), r;
		}

	// IPerPropertyBrowsing
		HRESULT STDMETHODCALLTYPE GetDisplayString(DISPID dispID, BSTR * pBstr) { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE MapPropertyToPage(DISPID dispID, CLSID * pClsid) { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetPredefinedStrings(DISPID dispID, CALPOLESTR * pCaStringsOut, CADWORD * pCaCookiesOut);
		HRESULT STDMETHODCALLTYPE GetPredefinedValue(DISPID dispID, DWORD dwCookie, VARIANT * pVarOut);

	protected:
		LONG						m_ref;
		std::vector<CComVariant>	m_Vals;
	};

public:
	CGuiItemEdit *			m_pEditor;
	CItemBase *				m_pItem;
	TYPEATTR				m_TypeAttr;
	std::vector<PropDesc>	m_Props;
	DISPID					m_nPropStrId;
	DISPID					m_nTextId;
	_PredefVals				m_PrefVals;

	CComGuiItemImpl() : m_pEditor(NULL), m_pItem(NULL), m_nPropStrId(-1), m_nTextId(-1) {}
	~CComGuiItemImpl() { Init(NULL); }

	void	Init(CItemBase * pItem);

public:
// IDispatch
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT * pctinfo) { return *pctinfo = 1, S_OK; }
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo) { return iTInfo ? E_FAIL : (*ppTInfo = this, ((IDispatch *)this)->AddRef(), S_OK); }
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

// ITypeInfo
	HRESULT STDMETHODCALLTYPE GetTypeAttr(TYPEATTR **ppTypeAttr) { return *ppTypeAttr = &m_TypeAttr, S_OK; }
	HRESULT STDMETHODCALLTYPE GetTypeComp(ITypeComp **ppTComp) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetFuncDesc(UINT index, FUNCDESC **ppFuncDesc);
	HRESULT STDMETHODCALLTYPE GetVarDesc(UINT index, VARDESC **ppVarDesc) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetNames(MEMBERID memid, BSTR *rgBstrNames, UINT cMaxNames, UINT *pcNames) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetRefTypeOfImplType(UINT index, HREFTYPE *pRefType) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetImplTypeFlags(UINT index, INT *pImplTypeFlags) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(LPOLESTR *rgszNames, UINT cNames, MEMBERID *pMemId) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetDocumentation(MEMBERID memid, BSTR *pBstrName, BSTR *pBstrDocString, DWORD *pdwHelpContext, BSTR *pBstrHelpFile);
	HRESULT STDMETHODCALLTYPE GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR *pBstrDllName, BSTR *pBstrName, WORD *pwOrdinal) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo **ppTInfo) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID *ppv) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter, REFIID riid, PVOID *ppvObj) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetMops(MEMBERID memid, BSTR *pBstrMops) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetContainingTypeLib(ITypeLib **ppTLib, UINT *pIndex) { return E_NOTIMPL; }
	void STDMETHODCALLTYPE ReleaseTypeAttr(TYPEATTR * pTypeAttr) {}
	void STDMETHODCALLTYPE ReleaseFuncDesc(FUNCDESC * pFuncDesc) {}
	void STDMETHODCALLTYPE ReleaseVarDesc(VARDESC * pVarDesc) {}

public:
	static HRESULT _InvokeProp_Group(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_ExternalId(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Id(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_External(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Overridable(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Props(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Type(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Alias(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Text(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_TextIsSimple(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	
	static HRESULT _InvokeProp_QuotedParamsProp(LPCSTR strProp, CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	
	static HRESULT _InvokeProp_Font(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Icon(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_Background(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);

	static HRESULT _InvokeProp_Border(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_SizeHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_SizeVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_MarginLeft(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_MarginTop(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_MarginRight(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_MarginBottom(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_OffsetBeforeHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_OffsetBeforeVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_OffsetAfterHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_OffsetAfterVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_PreviousItem(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_AlignHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);
	static HRESULT _InvokeProp_AlignVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v);

	static DWORD _GetPropPredefVal_Group(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_Font(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_Icon(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_Background(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_Border(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_Size(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_PreviousItem(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_AlignHorizontal(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
	static DWORD _GetPropPredefVal_AlignVertical(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v);
};

typedef _CUnkImpl<CComGuiItemImpl, IDispatch>	CComGuiItem;

// ########################################################################
// ########################################################################

#endif // __GUIITEMEDIT_H__
