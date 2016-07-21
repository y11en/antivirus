/**************************************************************************

   File:          ExplorerBar.h
   
   Description:   CExplorerBar definitions.

**************************************************************************/

#include <windows.h>
#include <shlobj.h>

#include "Globals.h"


#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>
#include <atlctrls.h>
#include <atlctrlx.h>
using namespace WTL;

#include <Prague/pr_remote.h>
#include <Prague/iface/i_root.h>
#include <ProductCore/iface/i_taskmanager.h>
#include <ProductCore/iface/i_task.h>

#include <ProductCore/structs/s_profiles.h>
#include <PPP/structs/s_gui.h>
#include <AV/structs/s_avs.h>

#include "Localizer.h"
extern CAppModule _Module;


#ifndef EXPLORERBAR_H
#define EXPLORERBAR_H

#define MIN_SIZE_X   10
#define MIN_SIZE_Y   10

/**************************************************************************

   CExplorerBar class definition

**************************************************************************/

class CExplorerBar : public IDeskBand, 
                     public IInputObject, 
                     public IObjectWithSite,
                     public IPersistStream
{
protected:
   DWORD m_ObjRefCount;

public:
   CExplorerBar();
   ~CExplorerBar();

   //IUnknown methods
   STDMETHODIMP QueryInterface(REFIID, LPVOID*);
   STDMETHODIMP_(DWORD) AddRef();
   STDMETHODIMP_(DWORD) Release();

   //IOleWindow methods
   STDMETHOD (GetWindow) (HWND*);
   STDMETHOD (ContextSensitiveHelp) (BOOL);

   //IDockingWindow methods
   STDMETHOD (ShowDW) (BOOL fShow);
   STDMETHOD (CloseDW) (DWORD dwReserved);
   STDMETHOD (ResizeBorderDW) (LPCRECT prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);

   //IDeskBand methods
   STDMETHOD (GetBandInfo) (DWORD, DWORD, DESKBANDINFO*);

   //IInputObject methods
   STDMETHOD (UIActivateIO) (BOOL, LPMSG);
   STDMETHOD (HasFocusIO) (void);
   STDMETHOD (TranslateAcceleratorIO) (LPMSG);

   //IObjectWithSite methods
   STDMETHOD (SetSite) (IUnknown*);
   STDMETHOD (GetSite) (REFIID, LPVOID*);

   //IPersistStream methods
   STDMETHOD (GetClassID) (LPCLSID);
   STDMETHOD (IsDirty) (void);
   STDMETHOD (Load) (LPSTREAM);
   STDMETHOD (Save) (LPSTREAM, BOOL);
   STDMETHOD (GetSizeMax) (ULARGE_INTEGER*);

private:
	BOOL				m_bFocus;
	HWND				m_hwndParent;
	HWND				m_hWnd;
	DWORD				m_dwViewMode;
	DWORD				m_dwBandID;
	IInputObjectSite *	m_pSite;
	IWebBrowser2 *		m_pWebBrowser;

	CStatic				m_StaticText;
	CHyperLink			m_StaticLink;
	CFont				m_Font;
	HBITMAP				m_hBmp;
	HBRUSH				m_hBgBrush;
	SIZE				m_szBmp;
	TCHAR				m_szPath[MAX_PATH];
	
	CPRRemotePrague		m_pRemote;
	CPRRemoteProxy<cTask *> m_pSCProxy;
	
	cLocalizedResources m_Res;

protected:
	void				FocusChange(BOOL);
	BOOL				RegisterAndCreateWindow(void);
	
	LRESULT				OnKillFocus(void);
	LRESULT				OnSetFocus(void);
	LRESULT				OnPaint(void);
	LRESULT				OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT				OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT				OnTimer(WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK	WndProcLink(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	LRESULT				WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	BOOL				IsProductLoaded(void);

};

#endif   //EXPLORERBAR_H
