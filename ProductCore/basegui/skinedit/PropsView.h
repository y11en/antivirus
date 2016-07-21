#if !defined(__PROPSVIEW_H__)
#define __PROPSVIEW_H__

/*
 CPropBrowseWnd custom messages
 */
#define WM_PB_STARTEDIT		(WM_APP + 12)
#define WM_PB_WARNING		(WM_APP + 14)
#define WM_PB_SHOWPOPUP		(WM_APP + 15)
#define WM_PB_PROPCHANGE	(WM_APP + 16)

/*
 Color picker custom message
 */
#define WM_ENDPICK			(WM_USER + 11)

class CPropBrowseWnd;
class CPBProperty;

// used in color picker's 'System' tab
typedef struct
{
	OLE_COLOR c;
	TCHAR *szText;
} PBSystemColorEntry;

class CPBColorPicker : public CWindowImpl<CPBColorPicker>
	{
	public:
		OLE_COLOR m_clr;
 
		DECLARE_WND_CLASS(_T("PBColorPicker_1_0"))
		HRESULT PickColor(CPBProperty *pProp,RECT* r);
		BOOL CreateWnd(CPropBrowseWnd * pParent);
		CPBColorPicker():m_wndTab(WC_TABCONTROL,this,1),m_wndPalette(_T("BUTTON"),this,2),
			m_wndList(_T("LISTBOX"),this,3)
			{
			SetRectEmpty(&m_rcDisplay);
			m_bTrackPaletteColor = FALSE;
			m_nTrackIndex = -1;
			}
		~CPBColorPicker()
			{
			}
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		WORD fActive = LOWORD(wParam);
		if(fActive == WA_INACTIVE)
			{
			ShowWindow(SW_HIDE);// hide window now (if drag other window's caption)
			PostMessage(WM_ENDPICK);
			}
		return 0;
		}


	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		HWND hwndGetFocus = (HWND) wParam; 
		if(::GetParent(hwndGetFocus) != m_hWnd)
			PostMessage(WM_ENDPICK,0,0);
		return 0;
		}
	LRESULT OnKillFocusChild(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
		{
		HWND hwndGetFocus = GetFocus();
		if(hwndGetFocus != m_hWnd && ::GetParent(hwndGetFocus) != m_hWnd)
			PostMessage(WM_ENDPICK,0,0);
		return 0;
		}

	LRESULT OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
		{
		int nSel = TabCtrl_GetCurSel(m_wndTab.m_hWnd);
		m_wndPalette.ShowWindow(!nSel);
		m_wndList.ShowWindow(nSel);
		return 0;
		};

	LRESULT OnSelChangeList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
		{
		int nSel = m_wndList.SendMessage(LB_GETCURSEL);
		if(nSel >= 0)
			{
			m_clr = m_SysColors[nSel].c;
			PostMessage(WM_ENDPICK,1);
			}
		return 0;
		}

	LRESULT OnLButtonDownPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUpPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMovePalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkGndPalette(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		return 1;
		}
	LRESULT OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
		WORD fwEvent = LOWORD(wParam);  // event flags 
		WORD idChild = HIWORD(wParam);  // identifier of child window 
		if(fwEvent == WM_RBUTTONDOWN && m_wndPalette.IsWindowVisible())
			{
			POINT pt;pt.x = (int)LOWORD(lParam);pt.y = (int)HIWORD(lParam);
			ClientToScreen(&pt);
			m_wndPalette.ScreenToClient(&pt);
			int nIdx = HitTestPalette(pt);
			if(nIdx > 47)
				m_nChooseColor = nIdx;
			}
		return 0;
		}


	BEGIN_MSG_MAP(CPBColorPicker)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DRAWITEM,OnDrawItem)
		MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocus)
		MESSAGE_HANDLER(WM_ACTIVATE,OnActivate)
		MESSAGE_HANDLER(WM_PARENTNOTIFY,OnParentNotify)

		NOTIFY_HANDLER(1,TCN_SELCHANGE,OnTabSelChange)
		COMMAND_HANDLER(3,LBN_KILLFOCUS,OnKillFocusChild)
		COMMAND_HANDLER(2,BN_KILLFOCUS,OnKillFocusChild)
		COMMAND_HANDLER(3,LBN_SELCHANGE,OnSelChangeList)

	ALT_MSG_MAP(1)
	ALT_MSG_MAP(2)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkGndPalette)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDownPalette)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUpPalette)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMovePalette)

	ALT_MSG_MAP(3)
	END_MSG_MAP()
	protected:
		int m_nChooseColor;
		void SetStartColor(OLE_COLOR c);
		int m_nTrackIndex;
		int GetPaletteItemRect(int nIdx,RECT *r);
		int HitTestPalette(POINT p);
		void DrawListItem(LPDRAWITEMSTRUCT di);
		void DrawPalette(LPDRAWITEMSTRUCT di);
		void CalcSizes();
		int m_nCellSize;
		BOOL  m_bTrackPaletteColor;
		CContainedWindow m_wndTab;
		CContainedWindow m_wndPalette;
		CContainedWindow m_wndList;
		RECT m_rcDisplay;
		CPropBrowseWnd * m_pParent;
		CPBProperty *m_pProp;
	static PBSystemColorEntry m_SysColors[];
	static OLE_COLOR m_clrPalette[];
	};


class CPBProperty
	{
	public:
		LPOLESTR GetNextPredefinedString(LPCTSTR szText);
		BOOL SetNewValue(VARIANT *pVar);
		bool ReadOnly() { return m_invkind == INVOKE_PROPERTYGET; }
		CComBSTR m_bstrText;
		BOOL m_bDirty;
		BOOL SetNewText(BSTR bstrNewText);
		CALPOLESTR     m_castr;
		CADWORD        m_cadw; 
		VARIANT * m_pvPredefinedValues;
	CPBProperty()
		{
		::ZeroMemory(&m_castr,sizeof(m_castr));
		::ZeroMemory(&m_cadw,sizeof(m_cadw));
		m_bDirty = FALSE;
		m_ulHelpContext = 0;
		m_DisplayType = PBT_ERROR;
		m_id = 0;
		m_vt = VT_NULL;
		}
	virtual ~CPBProperty()
		{
		ClearPredefinedValues();
		}

	void ClearPredefinedValues()
		{
		if(m_cadw.cElems > 0)
			{
			for(ULONG i=0;i < m_cadw.cElems;i++)
				VariantClear(m_pvPredefinedValues + i);
			CoTaskMemFree((void *)m_cadw.pElems);
			if(m_pvPredefinedValues)
				CoTaskMemFree((void *)m_pvPredefinedValues);
			m_pvPredefinedValues = NULL;
			m_cadw.cElems = 0;
			}
		if(m_castr.cElems > 0)
			{
			for (ULONG i=0; i < m_castr.cElems; i++)
				CoTaskMemFree((void *)m_castr.pElems[i]);  
			CoTaskMemFree((void *)m_castr.pElems); 
			m_castr.cElems = 0;
			}

		}
	HRESULT InitPredefinedValues(IDispatch * pDisp)
		{
		ULONG i = 0;HRESULT hr = S_OK;
		IPerPropertyBrowsing *pPropBrowse = NULL;
		ClearPredefinedValues();
		if(m_castr.cElems <=0 && m_DisplayType == PBT_BOOL)
		{
			m_castr.cElems = 2;
			m_castr.pElems = (LPOLESTR *)CoTaskMemAlloc(2*sizeof(LPOLESTR));
			m_cadw.cElems = 2;
			m_cadw.pElems = (DWORD FAR*)CoTaskMemAlloc(2*sizeof(DWORD FAR*));
			if (NULL==m_castr.pElems || NULL==m_cadw.pElems)
				{
				hr = E_OUTOFMEMORY;goto error;
				}
			for (int i = 0; i < 2; i++)
				{
				m_castr.pElems[i] = (LPOLESTR)CoTaskMemAlloc(128);
				if (!m_castr.pElems[i])
					{
					hr = E_OUTOFMEMORY;goto error;
					}
				}
			ocscpy_s(m_castr.pElems[0], 127, L"False");m_cadw.pElems[0] = 0;
			ocscpy_s(m_castr.pElems[1], 127, L"True");m_cadw.pElems[1] = 1;
			m_pvPredefinedValues = (VARIANT*) CoTaskMemAlloc(m_cadw.cElems*sizeof(VARIANT));
			m_pvPredefinedValues[0].vt = VT_BOOL;m_pvPredefinedValues[0].boolVal = FALSE;
			m_pvPredefinedValues[1].vt = VT_BOOL;m_pvPredefinedValues[1].boolVal = -1;
			return S_OK;
		}
		hr = pDisp->QueryInterface(IID_IPerPropertyBrowsing,(void**)&pPropBrowse);
		if(FAILED(hr)) goto error;
		hr = pPropBrowse->GetPredefinedStrings(m_id,&m_castr,&m_cadw);
		if(FAILED(hr)) goto error;
		if(m_cadw.cElems > 0)
			{
			m_pvPredefinedValues = (VARIANT*) CoTaskMemAlloc(m_cadw.cElems*sizeof(VARIANT));
			if(!m_pvPredefinedValues) { hr = E_OUTOFMEMORY;goto error;}
			}
		for(i=0;i < m_cadw.cElems;i++)
			{
			VariantInit(m_pvPredefinedValues + i);
			hr = pPropBrowse->GetPredefinedValue(m_id,m_cadw.pElems[i],m_pvPredefinedValues + i);
			if(FAILED(hr))
				goto error;
			}
		pPropBrowse->Release();
		return hr;
		error:
		ClearPredefinedValues();
		if(pPropBrowse) pPropBrowse->Release();
		return hr;
		}
	enum PBPropTypes
		{
		PBT_ERROR	=0,
		PBT_STR		=1,
		PBT_COLOR	=2,
		PBT_FONT	=3,
		PBT_ENUM	=4,
		PBT_PICTURE	=5,
		PBT_BOOL	=6
		} m_DisplayType ;

	static CPBProperty::PBPropTypes GetPropType(LPTYPEINFO pTypeInfo,TYPEDESC* lptd,VARTYPE *vt)
		{
		LPTYPEINFO lpUserTI=NULL;
		TYPEATTR* ta=NULL;
		PBPropTypes PropType = PBT_ERROR;
		if(vt)
			*vt = lptd->vt;
		switch(lptd->vt)
			{
				// simple types supported
			case VT_UI1:
			case VT_UI2:
			case VT_UI4:
			case VT_UI8:
			case VT_INT:
			case VT_UINT:
			case VT_I8:
			case VT_I1:
			case VT_I2:
			case VT_I4:
			case VT_R4:
			case VT_R8:
			case VT_BSTR:
			case VT_DATE:
			case VT_CY:
				return PBT_STR;
			case VT_BOOL:
				return PBT_BOOL;
			case VT_USERDEFINED:
				{
				// looks for OLE_COLOR and user-defined enums
				HRESULT hr;
		        hr = pTypeInfo->GetRefTypeInfo(lptd->hreftype,
					&lpUserTI);
		        if (FAILED(hr)) goto error;
				hr = lpUserTI->GetTypeAttr(&ta);
		        if (FAILED(hr)) goto error;
				switch(ta->typekind)
					{
					case TKIND_ALIAS:
						if(InlineIsEqualGUID(ta->guid,GUID_COLOR))
							PropType = PBT_COLOR;
						else
						PropType= GetPropType(lpUserTI,&ta->tdescAlias,vt);
						break;
					case TKIND_ENUM:

						if(InlineIsEqualGUID(ta->guid,GUID_COLOR))
							PropType = PBT_COLOR;
						else
						PropType = PBT_ENUM;
						break;
					case TKIND_DISPATCH:
						if(InlineIsEqualGUID(ta->guid,IID_IFontDisp))
							PropType = PBT_FONT;
						else if(InlineIsEqualGUID(ta->guid,IID_IPictureDisp))
							PropType = PBT_PICTURE;
						else PropType = PBT_ERROR;
						break;
					default:
						PropType = PBT_ERROR;
					}
				lpUserTI->ReleaseTypeAttr(ta);
				lpUserTI->Release();
				}
				break;
			case VT_PTR:
				// looks for IFontDisp* and IPictureDisp*
				PropType = GetPropType(pTypeInfo,lptd->lptdesc,vt);
				break;
			default:
				// any others are unsupported
				ATLTRACE(_T("[CPropertyBrowserPage]:Unsupported property type\n"));
				goto error;
			}
		return PropType;
		error:
		if(ta && lpUserTI)
			lpUserTI->ReleaseTypeAttr(ta);
		if(lpUserTI)
			lpUserTI->Release();
		return PBT_ERROR;
		}
	static HRESULT Create(ITypeInfo *pTypeInfo,unsigned int nIdx, CPBProperty ** ppProp)
		{
		CPBProperty* pProp = new CPBProperty();
		FUNCDESC *fd=NULL;
		PBPropTypes propType = PBT_ERROR;
		LPTYPEINFO ptinfoUserDefined = NULL;
		if(!pProp)
			return E_OUTOFMEMORY;
		_ASSERT(pTypeInfo);
		HRESULT hr = pTypeInfo->GetFuncDesc(nIdx,&fd);
		if(FAILED(hr)) goto error;
		pProp->m_id = fd->memid;
		pProp->m_invkind = fd->invkind;
		hr = pTypeInfo->GetDocumentation(pProp->m_id,
										&pProp->m_bstrName.m_str,
										&pProp->m_bstrDocumentation.m_str,
										&pProp->m_ulHelpContext,
										&pProp->m_bstrHelpFile
										);

		if(FAILED(hr)) goto error;
		if(fd->cParams != 1)
			{
			hr = E_INVALIDARG;
			goto error;
			}
		propType = GetPropType(pTypeInfo,&fd->lprgelemdescParam[0].tdesc,&pProp->m_vt);
		if(propType == PBT_ERROR)
			{
			hr = E_INVALIDARG;
			goto error;
			}
		pProp->m_DisplayType = propType;

		pTypeInfo->ReleaseFuncDesc(fd);
		(*ppProp) = pProp;
		return S_OK;
		error:
		if(pProp) delete pProp;
		if(fd) pTypeInfo->ReleaseFuncDesc(fd);
		if(ptinfoUserDefined) ptinfoUserDefined->Release();
		(*ppProp) = NULL;
		return hr;
		}
	MEMBERID m_id;
	INVOKEKIND m_invkind;
	VARTYPE m_vt;
	CComVariant	m_value; 
	CComBSTR m_bstrName;
	CComBSTR m_bstrDocumentation;
	CComBSTR m_bstrHelpFile;
	unsigned long m_ulHelpContext;
	protected:
	};
static const int nAllocSize = 32;

class CPBPropertyHolder
	{
	public:
		CPBPropertyHolder()
			{
			m_ppProp = NULL;
			m_nCount = 0;
			m_nAllocCount = 0;
			}
		virtual ~CPBPropertyHolder()
			{
			CleanUp();
			}
		BOOL Init(IDispatch *pDisp)
			{
			ITypeInfo *pTypeInfo = NULL;

			TYPEATTR* ta;
			HRESULT hr =  pDisp->GetTypeInfo(0,NULL,&pTypeInfo);
			if(FAILED(hr)) {_ASSERT(FALSE); return FALSE;}
			hr =pTypeInfo->GetTypeAttr(&ta);
			if(FAILED(hr)) {_ASSERT(FALSE); return FALSE;}
			int i;
			for(i=0;i < ta->cFuncs;i++)
				AddFuncProperty(pTypeInfo,i);
			pTypeInfo->ReleaseTypeAttr(ta);
			pTypeInfo->Release();
			pTypeInfo = NULL;
			CComDispatchDriver dd(pDisp);
			for(i = 0;i < m_nCount;i++)
				{
				CComVariant v;
				hr = dd.GetProperty(m_ppProp[i]->m_id,&v);
				_ASSERT(SUCCEEDED(hr));
				m_ppProp[i]->InitPredefinedValues(pDisp);
				m_ppProp[i]->SetNewValue(&v);
				}
			return TRUE;
			}
		int GetCount() {return m_nCount;}
		CPBProperty* operator[](int nIdx)
			{
			if(nIdx < 0 || nIdx >= m_nCount)
				{
				_ASSERT(FALSE);
				return NULL;
				}
			return m_ppProp[nIdx];
			}
		void CleanUp()
			{
			if(m_ppProp)
				{
				for(int i=0;i < m_nCount;i++)
					delete m_ppProp[i];
				delete[] m_ppProp;
				m_ppProp = NULL;
				m_nCount=0;
				m_nAllocCount = 0;
				}
			}
	protected:
		void AddFuncProperty(ITypeInfo * pTypeInfo,int nIdx)
		{
			HRESULT hr;
			FUNCDESC* fd;
			hr = pTypeInfo->GetFuncDesc(nIdx, &fd);
			if( FAILED(hr)) { _ASSERT(FALSE); return; }
			
			// looking for a property
			if( fd->invkind == INVOKE_PROPERTYPUT || fd->invkind == INVOKE_PROPERTYPUTREF || fd->invkind == INVOKE_PROPERTYGET )
			{
				// don't show hidden and non-browsable elements
				if( fd->wFuncFlags & (FUNCFLAG_FHIDDEN|FUNCFLAG_FNONBROWSABLE) )
					return;
				if( fd->cParams != 1 )
					return;
				if( CPBProperty::GetPropType(pTypeInfo, &fd->lprgelemdescParam[0].tdesc, NULL) == CPBProperty::PBT_ERROR )
					return;
				
				CPBProperty * pProp = Find(fd->memid);
				if( !pProp )
					_Add(pTypeInfo, nIdx);
				else if( fd->invkind != INVOKE_PROPERTYGET )
					pProp->m_invkind = fd->invkind;
			}
		}
		
		CPBProperty** m_ppProp;
		int m_nCount;
		int m_nAllocCount;
		//dumb finder
		CPBProperty* Find(MEMBERID mid)
			{
			for(int i=0;i < m_nCount;i++)
				if(m_ppProp[i]->m_id == mid)
					return m_ppProp[i];
			return NULL;
			}
		static inline int _CmpPropNames(CPBProperty * px,CPBProperty* py)
			{
			return _wcsicmp(px->m_bstrName.m_str,py->m_bstrName.m_str);
			}
		HRESULT _Add(ITypeInfo * pTypeInfo,int nIdx)
			{
			int i=0;
			if(m_nAllocCount == m_nCount)
				if(!_AllocMore())
					return FALSE;
			CPBProperty* pProp = NULL;
			HRESULT hr = CPBProperty::Create(pTypeInfo,nIdx,&pProp);
			if(FAILED(hr)) goto error;
			
			// disable sorting !!!
			/*			for(i=0;i < m_nCount;i++)
				{
				if(_CmpPropNames(m_ppProp[i],pProp) < 0)
					continue;
				memmove(m_ppProp + i + 1,m_ppProp+i,sizeof(m_ppProp[0])*(m_nCount-i));
				m_ppProp[i] = pProp;
				m_nCount++;
				return S_OK;
				}*/
			m_ppProp[m_nCount] = pProp;
			m_nCount++;
			return S_OK;
			error:
			if(pProp)
				delete pProp;
			return hr;
			}
		BOOL _AllocMore()
			{
			CPBProperty** ppTemp = NULL;
			ppTemp = new CPBProperty*[m_nAllocCount + nAllocSize];
			if(!ppTemp)
				{
				_ASSERT(ppTemp);// out of memory?
				return FALSE;
				}
			_ASSERT(m_nCount <= m_nAllocCount);
			m_nAllocCount += nAllocSize;
			for(int i=0;i < m_nCount;i++)
				ppTemp[i]=m_ppProp[i];
			if(m_ppProp)
				delete[] m_ppProp;
			m_ppProp = ppTemp;
			return TRUE;
			}
	};

// ########################################################################
// CPBDropListBox

class CPBDropListBox : public CWindowImpl<CPBDropListBox>
{
public:
	enum {_ID_LIST = 1};

	CPBDropListBox();
	~CPBDropListBox();

	void	Display(RECT * r, CPBProperty * pProp, HWND hwndEdit);
	int		LoadStrings(CPBProperty * pProp);
	
public:
	BEGIN_MSG_MAP(CPBDropListBox)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_HANDLER(1, LBN_SELCHANGE, OnSelChangeList)

		ALT_MSG_MAP(1)
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSelChangeList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	CContainedWindow	m_wndList;
	HWND				m_hwndBuddyEdit;
	CPBProperty *		m_pProp;
};

// ########################################################################
// CPropBrowseWnd

class CPropBrowseWnd : public CWindowImpl<CPropBrowseWnd>
{
public:
	enum {_ID_EDIT=1};
	
	HRESULT TranslateAccelerator(MSG* pMsg);
	BOOL Create(HWND hWndParent);
	void AddProperty(CPBProperty * pProp);
	void Clear();
	
	CPropBrowseWnd() : m_wndEdit(_T("EDIT"),this,1)
	{
		m_pEditProp = NULL;
		m_bEditReadOnly = FALSE;
		m_nColWidth = 0;
		m_clrTextRo = ::GetSysColor(COLOR_GRAYTEXT);
		m_clrHighliteTextRo = ::GetSysColor(COLOR_3DLIGHT);
		m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
		m_clrBack = ::GetSysColor(COLOR_WINDOW);
		m_clrHighlite = ::GetSysColor(COLOR_HIGHLIGHT);
		m_clrHighliteText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		m_hGridPen = CreatePen(PS_SOLID,1,::GetSysColor(COLOR_3DLIGHT));

		m_clrDirtyText = RGB(0xC0,0x00,0x00);// red color
	}
	virtual ~CPropBrowseWnd()
	{
		if(m_hGridPen)
			::DeleteObject(m_hGridPen);
		if(::IsWindow(m_wndColor.m_hWnd))
			m_wndColor.DestroyWindow();
		if(::IsWindow(m_wndLBox.m_hWnd))
			m_wndLBox.DestroyWindow();
	}

public:
	DECLARE_WND_SUPERCLASS(_T("KL.PropertyBrowserList"),_T("LISTBOX"))

	BEGIN_MSG_MAP(CPropBrowseWnd)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		MESSAGE_HANDLER(WM_PB_STARTEDIT,OnStartEdit)
		MESSAGE_HANDLER(WM_PB_WARNING,OnDisplayWarning)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_PB_SHOWPOPUP,OnShowPopup)
		MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtlColorStatic)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocusEdit)
		MESSAGE_HANDLER(WM_PASTE,OnCutPasteEdit)
		MESSAGE_HANDLER(WM_CUT,OnCutPasteEdit)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnLDoubleClickEdit)
		MESSAGE_HANDLER(WM_SYSKEYDOWN,OnSysKeyDownEdit)
		MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDownEdit)
	END_MSG_MAP()

	LRESULT DrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDoubleClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// mesage handler for start edit
	LRESULT OnStartEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// message handler to safe display a message box 
	LRESULT OnDisplayWarning(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnKillFocusEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLDoubleClickEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysKeyDownEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDownEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCutPasteEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = m_bEditReadOnly;
		return 0;
	}

protected:
	HRESULT AddMetaFile(CPBProperty * pProp,LPCTSTR szFileName);
	HRESULT AddBitmapFile(CPBProperty * pProp,LPCTSTR szFileName);
	HRESULT AddIconFile(CPBProperty * pProp,LPCTSTR szFileName);
	HRESULT SelectPicture(CPBProperty * pProp);
	HRESULT SelectFont(CPBProperty *pProp);
	void DrawItemArrow(HDC hDC,LPRECT prcItem,CPBProperty * pProp,BOOL bSelected);
	BOOL NeedArrow(CPBProperty *pProp);
	int GetItemWidth(CPBProperty* pProp);
	
	void _DoReturnClick(int nIdx);
	
	static int m_nDeltaX;

	BOOL m_bEditReadOnly;
	int m_nColWidth;
	COLORREF	m_clrText;
	COLORREF	m_clrBack;
	COLORREF	m_clrTextRo;
	COLORREF	m_clrHighliteTextRo;
	COLORREF	m_clrHighlite;
	COLORREF	m_clrHighliteText;
	COLORREF	m_clrDirtyText;
	HPEN		m_hGridPen;
	CPBProperty * m_pEditProp;	// current editing property
	CContainedWindow m_wndEdit;
	CPBColorPicker m_wndColor;
	CPBDropListBox m_wndLBox;
	RECT m_rcEdit;
};

// ########################################################################
// CPropBrowser

class CPropBrowser : public CWindowImpl<CPropBrowser, ATL::CWindow>
{
public:
	CPropBrowser() : m_bDirty(0) {}
	
	HRESULT SetObjects(ULONG nObjects, IDispatch ** ppUnk)
	{
		m_ppUnk.clear();
		m_Array.CleanUp();
		
		m_ppUnk.resize(nObjects);
		for(ULONG i = 0; i < nObjects; i++)
			m_ppUnk[i] = ppUnk[i];
		
		if( nObjects > 0 )
			m_Array.Init(m_ppUnk[0]);
	
		m_wndList.Clear(); // clear list box
		for(int i=0;i < m_Array.GetCount();i++)
			m_wndList.AddProperty(m_Array[i]);
		
		return S_OK;
	}

	HRESULT Apply(DISPID pid = -1)
	{
		ATLTRACE(_T("CPropertyBrowserPage::Apply\n"));
		for(UINT i = 0; i < m_ppUnk.size(); i++)
		{
			CComDispatchDriver& dd = m_ppUnk[i];
			for(int j = 0; j < m_Array.GetCount(); j++)
			{
				CPBProperty * pProp = m_Array[j];
				if( pid != -1 && pProp->m_id != pid )
					continue;
				
				HRESULT hr = dd.PutProperty(pProp->m_id, &(pProp->m_value));
				if( hr != S_OK )
				{
					if( FAILED(hr) )
					{
						CString strErr; strErr.Format(_T("Error apply property \"%s\": 0x%08X"), W2T(pProp->m_bstrName), hr);
						MessageBox(strErr, _T("Error"), MB_OK|MB_ICONSTOP);
					}

					Update(j);
				}
				
				pProp->m_bDirty = FALSE;

				if( pid != -1 )
					break;
			}
		}
		
		m_bDirty = 0;
		m_wndList.RedrawWindow();
		return S_OK;
	}

	void	Update(DWORD nIdx = -1)
	{
		if( nIdx == -1 )
		{
			for(DWORD j = 0; j < (DWORD)m_Array.GetCount(); j++)
				Update(j);
			
			m_wndList.RedrawWindow();
			return;
		}

		if( !m_ppUnk.size() )
			return;
		if( nIdx >= (DWORD)m_Array.GetCount() )
			return;
		
		CPBProperty * pProp = m_Array[nIdx];

		CComVariant v; m_ppUnk[0].GetProperty(pProp->m_id, &v);
		pProp->SetNewValue(&v);
	}

public:
	DECLARE_WND_CLASS(_T("KL.PropertyBrowser"));

	BEGIN_MSG_MAP(CPropBrowser)
		MESSAGE_HANDLER(WM_CREATE,			OnCreate)
		MESSAGE_HANDLER(WM_DRAWITEM,		OnDrawItem)
		MESSAGE_HANDLER(WM_SIZE,			OnSize)
		MESSAGE_HANDLER(WM_CLOSE,			OnClose)
		MESSAGE_HANDLER(WM_PB_PROPCHANGE,	OnPropertyChange)
		MESSAGE_HANDLER(WM_GETFONT,			OnGetFont)
	END_MSG_MAP()

	std::vector<CComDispatchDriver>	m_ppUnk;
	unsigned						m_bDirty : 1;
	CPBPropertyHolder				m_Array;
	CPropBrowseWnd					m_wndList;
	CFont							m_font;

	LRESULT OnGetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return (LRESULT)m_font.m_hFont;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LOGFONT lfIcon = { 0 }; ::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0);
		m_font.CreateFontIndirect(&lfIcon);

		m_wndList.Create(m_hWnd);
		return bHandled = FALSE, 0;
	}
	
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// just delegate
		return m_wndList.DrawItem(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WORD cx = LOWORD(lParam);
		WORD cy = HIWORD(lParam);
		
		if( m_wndList.m_hWnd )
			m_wndList.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnPropertyChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// receive notification from listbox about property has been changed
		Apply((DISPID)wParam);
		return 0;
	}
};

// ########################################################################
// CSingleTextPropEditor

class CSingleTextPropEditor : public CWindowImpl<CSingleTextPropEditor, WTL::CRichEditCtrl>
{
public:
	typedef CWindowImpl<CSingleTextPropEditor, WTL::CRichEditCtrl> TBase;
	
	CSingleTextPropEditor();
	
	HRESULT SetObject(IDispatch * pDisp, DISPID pid);
	void	Update();
	HRESULT	Apply();
	void	SetWordWrap(bool bWrap);
	bool	GetWordWrap();

	BOOL	Create(HWND hParentWnd);

public:
	BEGIN_MSG_MAP(CSingleTextPropEditor)
		MESSAGE_HANDLER(WM_CLOSE,			OnClose)
		MESSAGE_HANDLER(WM_CREATE,			OnCreate)
		MESSAGE_HANDLER(WM_KILLFOCUS,		OnKillFocus)
		MESSAGE_HANDLER(WM_KEYDOWN,			OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP,			OnKeyUp)
		MESSAGE_HANDLER(WM_CHAR,			OnChar)
	END_MSG_MAP()

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CComPtr<IDispatch>	m_pDisp;
	DISPID				m_nPid;
	unsigned			m_bWordWrap : 1;
};

// ########################################################################
// ########################################################################

#endif // !defined(__PROPSVIEW_H__)
