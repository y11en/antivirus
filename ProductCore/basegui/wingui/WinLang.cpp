// WinLang.cpp: implementation of the CWinLang class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinLang.h"
#include "WinUtils.h"
#include "ShellCtrls.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

static CTls g_TlsBuff;

inline BYTE * GetBuff(DWORD len)
{
	BLOB* pData = (BLOB*)(tPTR)g_TlsBuff;
	if( !pData )
	{
		pData = new BLOB();
		g_TlsBuff = pData;
		if( !pData )
			return NULL;
		memset(pData, 0, sizeof(BLOB));
	}

	if( pData->cbSize < len )
		pData->pBlobData = (BYTE*)realloc(pData->pBlobData, pData->cbSize = len);

	return pData->pBlobData;
}

void CRootItem::ThreadDetach()
{
	BLOB * pData = (BLOB *)(tPTR)g_TlsBuff;
	if( pData )
	{
		if( pData->pBlobData )
			free(pData->pBlobData);
		delete pData;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

DWORD Uni2Str(LPCWSTR strW, DWORD lenW, LPSTR strA, DWORD lenA, enStrCnv sct, UINT cpSpec)
{
	if( sct != escUTF8Ex || CWindowL::m_bAnsi )
		return ::WideCharToMultiByte(sct == escUTF8 ? CP_UTF8 : cpSpec,
			0, strW, lenW, strA, lenA, NULL, NULL);
	
	BOOL bUsedDefaultChars = FALSE;
	DWORD nRes = ::WideCharToMultiByte(cpSpec, 0, strW, lenW, strA, lenA, NULL, &bUsedDefaultChars);
	if( !bUsedDefaultChars )
		return nRes;

	if( lenA < 2 )
		return 0;
	
	nRes = ::WideCharToMultiByte(CP_UTF8, 0, strW, lenW, strA + 1, lenA - 2, NULL, NULL);
	if( nRes )
	{
		strA[0]        = MBCS_UTF8_SEP;
		strA[nRes + 1] = MBCS_UTF8_SEP;
		nRes += 2;
	}
	
	return nRes;
}

//////////////////////////////////////////////////////////////////////

void CRootItem::AdjustMetrics(CItemBase* pItem)
{
	AdjustSize(pItem->m_rcShift.left);
	AdjustSize(pItem->m_rcShift.top);
	AdjustSize(pItem->m_rcShift.right);
	AdjustSize(pItem->m_rcShift.bottom);

	AdjustSize(pItem->m_rcMargin.left);
	AdjustSize(pItem->m_rcMargin.top);
	AdjustSize(pItem->m_rcMargin.right);
	AdjustSize(pItem->m_rcMargin.bottom);

	AdjustSize(pItem->m_szSizeMin.cx);
	AdjustSize(pItem->m_szSizeMin.cy);
	AdjustSize(pItem->m_szSizeMax.cx);
	AdjustSize(pItem->m_szSizeMax.cy);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool CWindowL::m_bAnsi;
bool CWindowL::m_bAnsiOperate;
CPointerCleaner CWindowL::m_ClassNamesHolder;

//////////////////////////////////////////////////////////////////////

void CWindowL::Init(tDWORD nOsVer)
{
	m_bAnsi = !(nOsVer & GUI_OSVER_FAMILY_NT);
	m_bAnsiOperate = m_bAnsi ? (nOsVer != GUI_OSVER_WINME) : false;
}

int CWindowL::DrawText(CRootItem * pRoot, HDC dc, LPCSTR strText, RECT& rc, DWORD flags)
{
	pRoot->LocalizeStr(pRoot->m_strTemp1, strText);

	int len = pRoot->m_strTemp1.size();
	if( flags & DT_MODIFYSTRING )
		pRoot->m_strTemp1.resize(len+4);

	int ret = ::DrawTextW(dc, (LPWSTR)pRoot->m_strTemp1.data(), len, &rc, flags);

	if( flags & DT_MODIFYSTRING )
	{
		tString str; pRoot->LocalizeStr(str, pRoot->m_strTemp1.data());
		strcpy((char*)strText, str.c_str());
	}

	return ret;
}

HWND CWindowL::CreateWnd(DWORD dwExStyle, LPVOID lpClassName, DWORD dwStyle, RECT& rcPos, HWND hWndParent, DWORD nID, LPVOID lpParam)
{
	return ::CreateWindowExW(dwExStyle, (LPCWSTR)lpClassName,
			L"", dwStyle, rcPos.left, rcPos.top, RECT_CX(rcPos), RECT_CY(rcPos),
			hWndParent, (HMENU)nID, _Module.GetModuleInstance(), lpParam);
}

BOOL CWindowL::BrowseFile(CRootItem * pRoot, OPENFILENAMEA * ofn, tObjPath& sPath, bool bSave)
{
	std::vector<BYTE> buf;
	{
		tDWORD nSize = 0x4000 * (m_bAnsi ? 1 : 2);
		buf.resize(nSize);
		if( buf.size() < nSize )
			return ::SetLastError(ERROR_OUTOFMEMORY), FALSE;

		buf[0] = 0;
		buf[1] = 0;
	}

	LPCSTR  strTitleOld = ofn->lpstrTitle;
	LPCSTR  strDefExtOld = ofn->lpstrDefExt;
	LPCSTR  strFilterOld = ofn->lpstrFilter;
	
	cStrObj strTitleW;
	cStrObj strDefExtW;
	
	tString strFilterA;
	cStrObj strFilterW;
	
	tString strInitDirA;
	cStrObj strInitDirW;
	
	bool bFileName = true;
	{
		tDWORD nPos = sPath.find_last_of(L"\\/");
		if( cStrObj::npos == nPos )
		{
			LPITEMIDLIST pPersonalId = NULL;
			::SHGetSpecialFolderLocation(ofn->hwndOwner, CSIDL_PERSONAL, &pPersonalId);
			if( pPersonalId )
			{
				if( m_bAnsi )
				{
					CHAR szPersonalDir[2 * MAX_PATH];
					if( ::SHGetPathFromIDListA(pPersonalId, szPersonalDir) )
						strInitDirA = szPersonalDir;
				}
				else
				{
					WCHAR szPersonalDir[2 * MAX_PATH];
					if( ::SHGetPathFromIDListW(pPersonalId, szPersonalDir) )
						strInitDirW = szPersonalDir;
				}
				
				::CoTaskMemFree(pPersonalId);
			}
		}
		else if( nPos + 1 == sPath.size() )
		{
			if( m_bAnsi )
				pRoot->LocalizeStr(strInitDirA, sPath.data(), sPath.size());
			else
				strInitDirW = sPath;
			
			bFileName = false;
		}
	}
	
	// Copy sPath to temp buffer
	if( bFileName )
	{
		tPTR pScr;
		size_t nSrcSize;
		tString ansiBuff;
		if( m_bAnsi )
		{
			pScr = (tPTR)pRoot->LocalizeStr(ansiBuff, sPath.data(), sPath.size());
			nSrcSize = (strlen((LPCSTR)pScr) + 1) * sizeof(CHAR);
		}
		else
		{
			pScr = (tPTR)sPath.data();
			nSrcSize = (wcslen((LPCWSTR)pScr) + 1) * sizeof(WCHAR);
		}

		memcpy(&buf[0], pScr, min(nSrcSize, buf.size()));
		buf[buf.size() - 1] = 0;
		buf[buf.size() - 2] = 0;
	}

	ofn->lpstrFile = (LPSTR)&buf[0];
	ofn->nMaxFile = (DWORD)buf.size() - 2;
	
	if( m_bAnsi )
	{
		if( ofn->lpstrFilter )
		{
			strFilterA = ofn->lpstrFilter;
			ofn->lpstrFilter = (LPCSTR)strFilterA.c_str();
			
			LPSTR strFltModify = (LPSTR)strFilterA.c_str();
			for(size_t i = 0, n = strFilterA.size(); i < n; i++)
				if( strFltModify[i] == '\n' ) strFltModify[i] = '\0';
		}
	
		if( !strInitDirA.empty() )
			ofn->lpstrInitialDir = strInitDirA.c_str();
	}
	else
	{
		if( ofn->lpstrFilter )
		{
			pRoot->LocalizeStr(strFilterW, ofn->lpstrFilter);
			ofn->lpstrFilter = (LPCSTR)strFilterW.data();
			
			LPWSTR strFltModify = (LPWSTR)strFilterW.data();
			for(int i = 0, n = strFilterW.size(); i < n; i++)
				if( strFltModify[i] == L'\n' ) strFltModify[i] = L'\0';
		}
	
		if( !strInitDirW.empty() )
			ofn->lpstrInitialDir = (LPCSTR)strInitDirW.data();

		if( ofn->lpstrTitle )  { pRoot->LocalizeStr(strTitleW, ofn->lpstrTitle);  ofn->lpstrTitle  = (LPCSTR)strTitleW.data(); }
		if( ofn->lpstrDefExt ) { pRoot->LocalizeStr(strDefExtW, ofn->lpstrDefExt); ofn->lpstrDefExt = (LPCSTR)strDefExtW.data(); }
	}

	BOOL bRes;
	if( m_bAnsi )
	{
		BOOL (APIENTRY * _GetFileNameA)(LPOPENFILENAMEA);
		_GetFileNameA = bSave ? GetSaveFileNameA : GetOpenFileNameA;
		bRes = _GetFileNameA((LPOPENFILENAMEA)ofn);
	}
	else
	{
		BOOL (APIENTRY * _GetFileNameW)(LPOPENFILENAMEW);
		_GetFileNameW = bSave ? GetSaveFileNameW : GetOpenFileNameW;
		bRes = _GetFileNameW((LPOPENFILENAMEW)ofn);
	}

	ofn->lpstrTitle  = strTitleOld;
	ofn->lpstrDefExt = strDefExtOld;
	ofn->lpstrFilter = strFilterOld;
	
	if( !bRes )
		return FALSE;

	if( m_bAnsi )
	{
		LPCSTR strA = (LPCSTR)&buf[0];
		pRoot->LocalizeStr(sPath, strA);
	}
	else
	{
		LPCWSTR strW = (LPCWSTR)&buf[0];
		sPath = strW;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

void CWindowL::ListView_GetItemTextL(CRootItem * pRoot, HWND hWnd, DWORD item, DWORD col, cStrObj& strText)
{
	LVITEMA lvi = { 0 };
	lvi.iSubItem = (int)col;

	LPSTR pTextBuf = NULL;
	int nRes = 0;
	for(int nLen = 256; ; nLen *= 2)
	{
		pTextBuf = new CHAR[nLen * (m_bAnsi ? 1 : 2)];
		if( pTextBuf == NULL )
			break;
		lvi.cchTextMax = nLen;
		lvi.pszText = pTextBuf;
		
		if( m_bAnsi )
		{
			pTextBuf[0] = 0;
			nRes = (int)::SendMessageA(hWnd, LVM_GETITEMTEXTA, (WPARAM)item, (LPARAM)&lvi);
		}
		else
		{
			((LPWSTR)pTextBuf)[0] = 0;
			nRes = (int)::SendMessageW(hWnd, LVM_GETITEMTEXTW, (WPARAM)item, (LPARAM)&lvi);
		}
		
		if( nRes < nLen - 1 )
			break;
		delete [] pTextBuf, pTextBuf = NULL;
	}

	if( !pTextBuf )
		return;

	if( m_bAnsi )
		pRoot->LocalizeStr(strText, pTextBuf);
	else
		strText = (LPCWSTR)pTextBuf;
	delete [] pTextBuf;
}

//////////////////////////////////////////////////////////////////////

ATOM CWindowL::RegisterWindowA(CWndClassInfo* p, bool bEnableReRegister)
{
	if( p->m_atom )
		return p->m_atom;

	p->m_wc.hInstance = _Module.GetModuleInstance();
	if( p->m_lpszOrigName != NULL )
	{
		WNDCLASSEXA wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		// Try global class
		if(!::GetClassInfoExA(NULL, p->m_lpszOrigName, &wc))
			if(!::GetClassInfoExA(p->m_wc.hInstance, p->m_lpszOrigName, &wc))
				return 0;

		LPCSTR lpsz = p->m_wc.lpszClassName;
		WNDPROC proc = p->m_wc.lpfnWndProc;

		memcpy(&p->m_wc, &wc, sizeof(WNDCLASSEX));
		p->pWndProc = p->m_wc.lpfnWndProc;
		p->m_wc.lpszClassName = lpsz;
		p->m_wc.lpfnWndProc = proc;
	}
	else
		p->m_wc.hCursor = ::LoadCursorA(p->m_bSystemCursor ? NULL : p->m_wc.hInstance, p->m_lpszCursorID);

	p->m_wc.hInstance = _Module.GetModuleInstance();;
	p->m_wc.style &= ~CS_GLOBALCLASS;

	WNDCLASSEXA wcTemp;
	memcpy(&wcTemp, &p->m_wc, sizeof(WNDCLASSEXW));
	p->m_atom = (ATOM)::GetClassInfoExA(p->m_wc.hInstance, p->m_wc.lpszClassName, &wcTemp);
	if( p->m_atom == 0 || bEnableReRegister )
	{
		p->m_atom = ::RegisterClassExA(&p->m_wc);
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "CWindowL::RegisterClassExA(%s): atom: 0x%x", p->m_wc.lpszClassName, p->m_atom));
	}
	return p->m_atom;
}

ATOM CWindowL::RegisterWindowW(CWndClassInfo* p, bool bEnableReRegister)
{
	if( p->m_atom )
		return p->m_atom;

	USES_CONVERSION;
	p->m_wc.hInstance = _Module.GetModuleInstance();
	if( p->m_lpszOrigName != NULL )
	{
		size_t cLast = strlen(p->m_lpszOrigName)-1;

		LPWSTR szClassName = A2W(p->m_lpszOrigName);
		if( p->m_lpszOrigName[cLast] == 'A' )
			szClassName[cLast] = 'W';

		WNDCLASSEXW wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		// Try global class
		if(!::GetClassInfoExW(NULL, szClassName, &wc))
			if(!::GetClassInfoExW(p->m_wc.hInstance, szClassName, &wc))
				return 0;

		LPCSTR lpsz = p->m_wc.lpszClassName;
		WNDPROC proc = p->m_wc.lpfnWndProc;

		UINT style = p->m_wc.style;
		memcpy(&p->m_wc, &wc, sizeof(WNDCLASSEX));
		p->pWndProc = p->m_wc.lpfnWndProc;
		p->m_wc.lpszClassName = lpsz;
		p->m_wc.lpfnWndProc = proc;
		p->m_wc.lpfnWndProc = proc;
		
		if( style )
			p->m_wc.style = style;
	}
	else
		p->m_wc.hCursor = ::LoadCursorW(p->m_bSystemCursor ? NULL : p->m_wc.hInstance, (LPCWSTR)p->m_lpszCursorID);

	LPCWSTR lpszwClassName = _wcsdup(A2W(p->m_wc.lpszClassName));
	m_ClassNamesHolder.AddPointer((void *)lpszwClassName);
	p->m_wc.lpszClassName = (LPCSTR)lpszwClassName;
	p->m_wc.hInstance = _Module.GetModuleInstance();;
	p->m_wc.style &= ~CS_GLOBALCLASS;
	
	WNDCLASSEXW wcTemp;
	memcpy(&wcTemp, &p->m_wc, sizeof(WNDCLASSEXW));
	p->m_atom = (ATOM)::GetClassInfoExW(p->m_wc.hInstance, (LPCWSTR)p->m_wc.lpszClassName, &wcTemp);
	if( p->m_atom == 0 || bEnableReRegister )
	{
		p->m_atom = ::RegisterClassExW((WNDCLASSEXW*)&p->m_wc);
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "CWindowL::RegisterClassExW(%S): atom: 0x%x", p->m_wc.lpszClassName, p->m_atom));
	}
	return p->m_atom;
}

//////////////////////////////////////////////////////////////////////

BOOL CWindowL::SetWindowTextW(CRootItem * pRoot, LPCWSTR strText)
{
	USES_CONVERSION;
	
	if( m_bAnsi )
		return ::SetWindowTextA(m_hWnd, W2A(strText));

	return ::SetWindowTextW(m_hWnd, strText);
}

void CWindowL::GetWindowTextW(CRootItem * pRoot, cStrObj& strText) const
{
	USES_CONVERSION;

	int len = ::GetWindowTextLength(m_hWnd);
	
	if( m_bAnsi )
	{
		LPSTR strBuff = (LPSTR)alloca(len + 1);
		if( ::GetWindowTextA(m_hWnd, strBuff, len + 1) )
			strText = A2W(strBuff);
		return;
	}

	strText.resize(len);
	::GetWindowTextW(m_hWnd, (LPWSTR)strText.data(), strText.size() + 1);
}

HTREEITEM CWindowL::TreeView_InsertItemL(CRootItem * pRoot, LPTVINSERTSTRUCT ts)
{
	if( m_bAnsiOperate && m_bAnsi )
		return (HTREEITEM)SNDMSG((m_hWnd), TVM_INSERTITEMA, 0, (LPARAM)(LPTV_INSERTSTRUCT)(ts));

	cStrObj wBuff;
	if( (ts->item.mask & TVIF_TEXT) && (ts->item.pszText != LPSTR_TEXTCALLBACK) )
		ts->item.pszText = (LPSTR)pRoot->LocalizeStr(wBuff, ts->item.pszText);
	return (HTREEITEM)SNDMSG((m_hWnd), TVM_INSERTITEMW, 0, (LPARAM)(LPTV_INSERTSTRUCT)(ts));
}

BOOL CWindowL::TreeView_SetItemL(CRootItem * pRoot, LPTVITEM ti)
{
	if( m_bAnsiOperate && m_bAnsi )
		return (BOOL)SNDMSG((m_hWnd), TVM_SETITEMA, 0, (LPARAM)(LPTVITEM)(ti));

	_USES_CONVERSION(((CWinRoot *)pRoot)->m_nCodePage);
	if( ti->mask & TVIF_TEXT )
		ti->pszText = (LPSTR)A2W(ti->pszText);
	return (BOOL)SNDMSG((m_hWnd), TVM_SETITEMW, 0, (LPARAM)(LPTVITEM)(ti));
}

BOOL CWindowL::ListView_SetColumnL(CRootItem * pRoot, tSIZE_T i, LPLVCOLUMN pcol)
{
	if( m_bAnsiOperate && m_bAnsi )
		return (BOOL)SNDMSG((m_hWnd), LVM_SETCOLUMNA, (WPARAM)i, (LPARAM)(const LV_COLUMN FAR*)(pcol));

	_USES_CONVERSION(((CWinRoot *)pRoot)->m_nCodePage);
	if( pcol->mask & LVCF_TEXT )
		pcol->pszText = (LPSTR)A2W(pcol->pszText);
	return (BOOL)SNDMSG((m_hWnd), LVM_SETCOLUMNW, (WPARAM)i, (LPARAM)(const LV_COLUMN FAR*)(pcol));
}

DWORD CWindowL::ListView_InsertColumnL(CRootItem * pRoot, tSIZE_T i, LPLVCOLUMN pcol)
{
	if( m_bAnsiOperate && m_bAnsi )
		return (BOOL)SNDMSG((m_hWnd), LVM_INSERTCOLUMNA, (WPARAM)i, (LPARAM)(const LV_COLUMN FAR*)(pcol));

	_USES_CONVERSION(((CWinRoot *)pRoot)->m_nCodePage);
	if( pcol->mask & LVCF_TEXT )
		pcol->pszText = (LPSTR)A2W(pcol->pszText);
	return (BOOL)SNDMSG((m_hWnd), LVM_INSERTCOLUMNW, (WPARAM)i, (LPARAM)(const LV_COLUMN FAR*)(pcol));
}

//////////////////////////////////////////////////////////////////////

HWND CDialogBase::Create(CRootItem * pRoot, HWND hWndParent, CWndClassInfo& pCI, RECT& rcPos, DWORD dwStyle, DWORD dwExStyle, UINT nID)
{
#pragma pack(push, 1)
	struct AVP_DLGTEMPLATE
	{
		WORD   dlgVer;
		WORD   signature;
		DWORD  helpID;
		DWORD  exStyle;
		DWORD  style;
		WORD   cDlgItems;
		short  x;
		short  y;
		short  cx;
		short  cy;
		WCHAR  szMenu[1];
		WCHAR  szClass[1];
	};
	
	struct AVP_DLGTEMPLATE_TERMINATOR
	{
		WCHAR  swTitle[1];
		WORD   pointsize;
		WORD   weight;
		BYTE   italic;
		BYTE   charset;
		WCHAR  szFont[1];
	};
#pragma pack(pop)

	tString strClass;
	if( pCI.m_wc.lpszClassName )
		strClass = "AVP.", strClass += (LPCSTR)nID;
	else
		strClass = "#32770";
	
	USES_CONVERSION;
	LPCWSTR szClassName = A2W(strClass.c_str());

	// Register window superclass
	WNDCLASSEXA wcDlg;
	wcDlg.cbSize = sizeof(wcDlg);
	if( m_bAnsi )
	{
		::GetClassInfoExA(NULL, WC_DIALOG, &wcDlg);
		wcDlg.lpszClassName = strClass.c_str();
	}
	else
	{
		::GetClassInfoExW(NULL, (LPCWSTR)WC_DIALOG, (WNDCLASSEXW*)&wcDlg);
		wcDlg.lpszClassName = (LPCSTR)szClassName;
	}
	wcDlg.style |= CS_VREDRAW|CS_HREDRAW;
	wcDlg.hInstance = _Module.GetModuleInstance();
	wcDlg.hbrBackground = NULL;

	if( m_bAnsi )
		::RegisterClassExA(&wcDlg);
	else
		::RegisterClassExW((CONST WNDCLASSEXW*)&wcDlg);

	// Prepare dialog template info
	size_t nClassLen = sizeof(WCHAR)*strClass.size();
	size_t nDlgTplTermOffset = sizeof(AVP_DLGTEMPLATE) + nClassLen;
	size_t nDlgTplSize = nDlgTplTermOffset + sizeof(AVP_DLGTEMPLATE_TERMINATOR);
	AVP_DLGTEMPLATE* pDlgTpl = (AVP_DLGTEMPLATE *)alloca(nDlgTplSize); memset(pDlgTpl, 0, nDlgTplSize);
	AVP_DLGTEMPLATE_TERMINATOR* pDlgTplTerm = (AVP_DLGTEMPLATE_TERMINATOR*)((unsigned char*)pDlgTpl + nDlgTplTermOffset);
	
	pDlgTpl->dlgVer = 1;
	pDlgTpl->signature = 0xFFFF;
	pDlgTpl->style = dwStyle|DS_SETFONT;
	if( !(pDlgTpl->style & WS_CHILD) )
		pDlgTpl->style |= WS_POPUP;
	pDlgTpl->exStyle = dwExStyle;
	pDlgTpl->x = (short)rcPos.left;
	pDlgTpl->y = (short)rcPos.top;
	pDlgTpl->cx = (short)RECT_CX(rcPos);
	pDlgTpl->cy = (short)RECT_CY(rcPos);
	
	memcpy(pDlgTpl->szClass, szClassName, nClassLen+sizeof(WCHAR));
	pDlgTplTerm->charset = (BYTE)((CWinRoot *)pRoot)->m_nCharSet;

	_Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT<CWindowL>*)this);

	if( m_bAnsi )
		m_hWnd = ::CreateDialogIndirectParamA(wcDlg.hInstance, (LPDLGTEMPLATE)pDlgTpl,
					hWndParent, (DLGPROC)StartDialogProc, NULL);
	else
		m_hWnd = ::CreateDialogIndirectParamW(wcDlg.hInstance, (LPDLGTEMPLATE)pDlgTpl,
					hWndParent, (DLGPROC)StartDialogProc, NULL);

	ATLASSERT(m_hWnd != NULL);
	return m_hWnd;
}

//////////////////////////////////////////////////////////////////////
