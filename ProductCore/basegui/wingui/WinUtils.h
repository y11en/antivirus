// WinUtils.h: interface for the CWinUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINUTILS_H__D0B7A8EC_30E1_4316_804A_EA77F11DFBA9__INCLUDED_)
#define AFX_WINUTILS_H__D0B7A8EC_30E1_4316_804A_EA77F11DFBA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "..\png\pngimg.h"
#include "xpbrush.h"

//////////////////////////////////////////////////////////////////////
// Data Time formats

LPCSTR GetDateTimeFormat(DTT_TYPE eType);
LPCSTR GetDateTimeStr(CRootItem* pRoot, cDateTime& prDt, tString& str, DTT_TYPE eType, bool bSeconds, bool bLocal);

//////////////////////////////////////////////////////////////////////
// CEnsureCleanup

#define OWNEROFMEMBER_PTR( _owner_class, _member )	\
	((_owner_class *)((unsigned char *)this - (unsigned char *)&((reinterpret_cast<_owner_class *>(0))->_member)))

typedef VOID WINAPI ENSURECLEANUP_FUNC(UINT_PTR);

template<class T, ENSURECLEANUP_FUNC * pfn, UINT_PTR tInvalid>
class CEnsureCleanup
{
public:
	CEnsureCleanup()    { m_t = (T)tInvalid; }
	CEnsureCleanup(T t) : m_t(t) {}
	~CEnsureCleanup()   { Cleanup(); }

	T    operator=(T t) { Cleanup(); m_t = t; return *this; }
	T *  operator &()   { return &m_t; }
	     operator T()   { return m_t; }
	
	bool IsValid()      { return m_t != (T)tInvalid; }
	T    Detach()       { T t = m_t; m_t = (T)tInvalid; return t; }
	void Cleanup()      { if( IsValid() ) pfn((UINT_PTR)m_t), m_t = (T)tInvalid; }

	T    m_t;
};

#define MAKECLEANUPCLASS_EX(className, tData, pfnCleanup, tInvalid) \
	typedef CEnsureCleanup<tData, (ENSURECLEANUP_FUNC *)pfnCleanup, (UINT_PTR)tInvalid> className;
#define MAKECLEANUPCLASS(className, tData, pfnCleanup) \
	MAKECLEANUPCLASS_EX(className, tData, pfnCleanup, NULL)

MAKECLEANUPCLASS(CSysHnd,           HANDLE,    CloseHandle)
MAKECLEANUPCLASS(CLibHnd,           HMODULE,   FreeLibrary)
MAKECLEANUPCLASS(CLocalHnd,         HLOCAL,    LocalFree)
MAKECLEANUPCLASS(CGlobalHnd,        HGLOBAL,   GlobalFree)
MAKECLEANUPCLASS(CRegHnd,           HKEY,      RegCloseKey)
MAKECLEANUPCLASS(CServiceHnd,       SC_HANDLE, CloseServiceHandle)
MAKECLEANUPCLASS(CWindowStationHnd, HWINSTA,   CloseWindowStation)
MAKECLEANUPCLASS(CDesktopHnd,       HDESK,     CloseDesktop)
MAKECLEANUPCLASS(CViewOfFilePtr,    PVOID,     UnmapViewOfFile)
MAKECLEANUPCLASS(CSidPtr,           PSID,      FreeSid)

MAKECLEANUPCLASS(CDcHnd,            HDC,       DeleteDC)
MAKECLEANUPCLASS(CMenuHnd,          HMENU,     DestroyMenu)
MAKECLEANUPCLASS(CIconHnd,          HICON,     DestroyIcon)
MAKECLEANUPCLASS(CBitmapHnd,        HBITMAP,   DeleteObject)
MAKECLEANUPCLASS(CBrushHnd,         HBRUSH,    DeleteObject)
MAKECLEANUPCLASS(CPenHnd,           HPEN,      DeleteObject)

MAKECLEANUPCLASS_EX(CFileHnd,       HANDLE,    CloseHandle, INVALID_HANDLE_VALUE);
MAKECLEANUPCLASS_EX(CFindHnd,       HANDLE,    FindClose,   INVALID_HANDLE_VALUE);

class CDesktopDC : public CDcHnd
{
public:
	CDesktopDC(bool bCreate = true) { if( bCreate ) Create(); }
	HDC Create()
	{
		CDcHnd &hDC = *(CDcHnd *)this;
		if( !hDC )
			hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
		return hDC;
	}
};

class IconInfo : public ICONINFO
{
public:
	IconInfo() { memset(this, 0, sizeof(*this)); }
	~IconInfo()
	{
		if( hbmMask )  ::DeleteObject((HGDIOBJ)hbmMask);
		if( hbmColor ) ::DeleteObject((HGDIOBJ)hbmColor);
	}
};

struct CTls
{
	CTls() : m_id(::TlsAlloc()) {}
	~CTls() { ::TlsFree(m_id); }

	bool operator =(tPTR val) { return !!::TlsSetValue(m_id, val); }
	     operator tPTR()      { return (tPTR)::TlsGetValue(m_id); }
	
	tDWORD m_id;
};



/////////////////////////////////////////////////////////////////////////////

#define GetTopBrush()     CAlphaBrush(m_clBg, m_clFrame, 0.1)
#define GetLeftBrush()    CAlphaBrush(m_clBg, m_clFrame, 0.1)
#define GetBottomBrush()  CAlphaBrush(m_clBg, 0x00FFFFFF, 0.6)
#define GetRightBrush()   CAlphaBrush(m_clBg, 0x00FFFFFF, 0.4)

/////////////////////////////////////////////////////////////////////////////

class CBitmapHndEx : public CImageBase
{
public:
	~CBitmapHndEx(){ Clear(); }

	void Clear();
	bool Init(HBITMAP hBmp, DWORD nImageSize, bool bMask);
	void InitMask();
	void Draw(HDC dc, RECT& rc, int nOffset=0);
	bool Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx);
	bool IsTransparent();

	SIZE  Size() { return m_szSize; }
	HDC   DcHnd(){ return m_hDC; }

	virtual void DrawCustom(HDC dc, RECT& rc, HDC dcTemp, int offset){}

protected:
	SIZE        m_szSize;
	COLORREF    m_clTransparent;
	CDcHnd      m_hDC;
	CDcHnd      m_hMaskDC;
	CBitmapHnd  m_hBitmap;
	CBitmapHnd  m_hMask;
	HBITMAP     m_hOldBitmap;
	HBITMAP     m_hOldMask;
};

class CIconImpl : public CImageBase
{
public:
	CIconImpl() : m_hIcon(NULL){ m_szDef.cx = m_szDef.cy = 0; }
	virtual ~CIconImpl(){ Clear(); }

	virtual void Clear();
	virtual void InitSize(IconInfo& iInfo);

	bool Init(HICON hIcon, bool bRef, bool bRepare = false);
	bool Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx);
	bool Merge(CImageBase * pIcon, tDWORD nIdx = 0);

	SIZE  Size() { return m_szDef; }
	HICON IconHnd(){ return m_hIcon; }
	
protected:
	HICON      m_hIcon;
	SIZE       m_szDef;
};

class CIconList : public CIconImpl
{
public:
	CIconList() : m_hIL(NULL), m_nCount(0), m_nImageSizeX(0){}
	~CIconList(){ Clear(); }

	virtual void Clear();
	virtual void InitSize(IconInfo& iInfo);

	bool Init(HICON hIcon, int nImageSizeX);
	bool Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx);

	tPTR  ImageList() { return m_hIL; }

private:
	HIMAGELIST m_hIL;
	DWORD      m_nCount;
	DWORD      m_nImageSizeX;
};

//////////////////////////////////////////////////////////////////////
// CLocker, CAutoLock

class CLocker
{
public:
	CLocker(){ InitializeCriticalSection(&m_cs); }
	~CLocker(){ DeleteCriticalSection(&m_cs); }

	void Enter(){ EnterCriticalSection(&m_cs); }
	void Leave(){ LeaveCriticalSection(&m_cs); }

private:
	CRITICAL_SECTION m_cs;
};

class CAutoLock
{
public:
	CAutoLock(CLocker &locker) : m_locker(locker){ m_locker.Enter(); };
	~CAutoLock() { m_locker.Leave(); };
	
private:
	CLocker &m_locker;
};

class CLibWrapper
{
public:
	CLibWrapper(LPCSTR strLib) : m_hDll(NULL), m_bLoadFail(false), m_sLib(strLib){}
	~CLibWrapper() { Close(); }

	bool GetFunc(FARPROC *pFunc, LPCSTR strFunc)
	{
		if( m_bLoadFail )
			return false;

		if( !m_hDll )
			m_hDll = ::LoadLibrary(m_sLib.c_str());

		if( !m_hDll )
		{
			m_bLoadFail = true;
			return false;
		}

		*pFunc = GetProcAddress(m_hDll, strFunc);
		return *pFunc != NULL;
	}

	void Close()
	{
		if( !m_hDll )
			return;
		
		::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}

	HMODULE Detach()
	{
		HMODULE hDll = m_hDll;
		m_hDll = NULL;
		return hDll;
	}

private:
	tString   m_sLib;
	HMODULE   m_hDll;
	bool      m_bLoadFail;
};

//////////////////////////////////////////////////////////////////////

struct CWindowLocker
{
	CWindowLocker(HWND hwnd, bool isLocked = true): m_hwnd(hwnd), m_bIsLocked(isLocked)
	{
		if (m_bIsLocked)
			SetWindowRedraw(m_hwnd, FALSE);
	}
	~CWindowLocker()
	{
		if (m_bIsLocked)
			SetWindowRedraw(m_hwnd, TRUE);
	}

protected:
	HWND	m_hwnd;
	bool	m_bIsLocked;
};

//////////////////////////////////////////////////////////////////////

void    ConvertPath(PATHOP_TYPE eType, LPCSTR strText, tString& strRes);
void    CompressText(LPWSTR strDst, int nDstSize, LPCWSTR strSrc, int nSrcLen = 0);

PVOID   GetProcImportPtr(PVOID hModule, PCHAR FuncName);
PVOID   GetProcDelayedPtr(PVOID hModule, PCHAR FuncName);
void    HookDrawBegin(CItemBase * pItem);
void    HookDrawEnd();
HWND    GetParentHWND(CItemBase* pItem, bool bTop = false);

BOOL    _EnableFSRedirector(BOOL bEnable);
bool    ResolveShortcat(cStringObj &strShortcat);

//////////////////////////////////////////////////////////////////////

BOOL    ShowContextHelp(CRootItem *pRoot, tDWORD nTopicId);

//////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) tERROR SendSrvMgrRequest(LPCWSTR strService, tSrvMgrRequest nRequest);

#endif // !defined(AFX_WINUTILS_H__D0B7A8EC_30E1_4316_804A_EA77F11DFBA9__INCLUDED_)
