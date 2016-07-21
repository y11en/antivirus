#include "HtmlCtrl.h"

// Microsoft Systems Journal -- January 2000
 // If this code works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 // Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT too.
 //
 #include "StdAfx.h"
#include "resource.h"
 #include "HtmlCtrl.h"
#include "OlaTestDlg.h"

#include <fstream>
#include <mshtml.h>
#include <comdef.h>
 
 #ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
 #endif
 
 IMPLEMENT_DYNAMIC(CHtmlCtrl, CHtmlView)
 BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
    ON_WM_DESTROY()
    ON_WM_MOUSEACTIVATE()
 END_MESSAGE_MAP()

 
CHtmlCtrl::CHtmlCtrl(COlaTestDlg* pDialog) : m_pDialog(pDialog)
{
}

 //////////////////
 // Create control in same position as an existing static control with
 // the same ID (could be any kind of control, really)
 //
 BOOL CHtmlCtrl::CreateFromStatic(UINT nID, CWnd* pParent)
 {
    CStatic wndStatic;
    if (!wndStatic.SubclassDlgItem(nID, pParent))
       return FALSE;
 
    // Get static control rect, convert to parent's client coords.
    CRect rc;
    wndStatic.GetWindowRect(&rc);
    pParent->ScreenToClient(&rc);
    wndStatic.DestroyWindow();
 
    // create HTML control (CHtmlView)
    return Create(NULL,                  // class name
       NULL,                             // title
       (WS_CHILD | WS_VISIBLE ),         // style
       rc,                               // rectangle
       pParent,                          // parent
       nID,                              // control ID
       NULL);                            // frame/doc context not used
 }
 
 ////////////////
 // Override to avoid CView stuff that assumes a frame.
 //
 void CHtmlCtrl::OnDestroy()
 {
    // This is probably unecessary since ~CHtmlView does it, but
    // safer to mimic CHtmlView::OnDestroy.
//    if (m_pBrowserApp) {
//     m_pBrowserApp->Release();
//       m_pBrowserApp = NULL;
//    }
    CWnd::OnDestroy(); // bypass CView doc/frame stuff
 }
 
 ////////////////
 // Override to avoid CView stuff that assumes a frame.
 //
 int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
 {
    // bypass CView doc/frame stuff
    return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, msg);
 }
 
void CHtmlCtrl::OnBeforeNavigate2
(
	LPCTSTR lpszURL,
	DWORD nFlags,
	LPCTSTR lpszTargetFrameName,
	CByteArray& baPostedData,
	LPCTSTR lpszHeaders,
	BOOL* pbCancel
)
{
	// intercept posts
	if (baPostedData.GetSize())
	{
		*pbCancel = TRUE;
		m_pDialog->PostReply(lpszHeaders, baPostedData.GetData(), baPostedData.GetSize());
	}
}
 
void CHtmlCtrl::LoadFromBuffer(const char* szData, size_t nSize)
{
	IHTMLDocument2Ptr spDoc = GetHtmlDocument();

	if (spDoc)
	{
		SAFEARRAY* safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		
		VARIANT* variant;
		
		SafeArrayAccessData(safe_array,(LPVOID*)&variant);
		
		variant->vt      = VT_BSTR;
		variant->bstrVal = CString(szData, nSize).AllocSysString();
		
		SafeArrayUnaccessData(safe_array);

		// write SAFEARRAY to browser document

		spDoc->write(safe_array);

		SafeArrayDestroy(safe_array);
	}
}

bool CHtmlCtrl::Submit()
{
	IHTMLDocument2Ptr spDoc = GetHtmlDocument();
	if (!spDoc)
		return false;

	IHTMLElementCollectionPtr spForms;
	if (FAILED(spDoc->get_forms(&spForms)))
		return false;

	long length = 0;
	if (FAILED(spForms->get_length(&length)))
		return false;

	IHTMLFormElementPtr spForm;

	for (long i = 0; i < length; ++i)
	{
		IDispatchPtr spDisp;
		CComVariant varIdx(i);
		if FAILED(spForms->item(varIdx, varIdx, &spDisp))
			continue;
		IHTMLElementPtr spElem(spDisp);
		IHTMLStylePtr spStyle;
		if(SUCCEEDED(spElem->get_style(&spStyle)) && spStyle)
		{
			CComBSTR bstrDisplay;
			if(SUCCEEDED(spStyle->get_display(&bstrDisplay)) && bstrDisplay)
			{
				bstrDisplay.ToLower();
				if(wcsstr(bstrDisplay, L"none"))
					continue;
			}
		}
		spForm = spDisp;
		break;
	}
	if (!spForm)
		return false;
	if FAILED(spForm->submit())
		return false;

	return true;
}