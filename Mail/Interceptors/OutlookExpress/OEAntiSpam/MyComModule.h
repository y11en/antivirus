// MyComModule.h: interface for the CMyComModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYCOMMODULE_H__740BDE8F_98F5_4361_B340_3B68DC0665B6__INCLUDED_)
#define AFX_MYCOMMODULE_H__740BDE8F_98F5_4361_B340_3B68DC0665B6__INCLUDED_

#include <ProductCore/basegui.h>
#include <ProductCore/GuiLoader.h>

class CMyComModule : public CComModule, public CRootSink
{
public:
	bool LocInit();	// load the localization library
	void LocDeinit();

	bool TeachAntiSpam(bool bIsSpam, const LPBYTE buf, UINT nBufSize);
	bool IsKAVRunning() const;
	bool IsAntiSpamTaskRunning() const;
	bool ShowAntiSpamSettings() const;
	
	// all LPCTSTR identifiers are MAKEINTRESOURCE'd
	HICON   LoadIcon(LPCTSTR szIconName) const;
	HMENU   LoadMenu(LPCTSTR szMenuName) const;
	HBITMAP LoadBitmap(LPCTSTR szBitmapName) const;
	HCURSOR LoadCursor(LPCTSTR szCursorName) const;
	tstring LoadString(UINT nStringID) const;

	tstring LoadString(LPCSTR szSection, LPCSTR szID);	// Prague localization support
	std::wstring LoadStringW(LPCSTR szSection, LPCSTR szID);
	int MessageBox(CItemBase* pParent, LPCTSTR szTitle, LPCTSTR szText, UINT uType);

	static bool _WriteOEVerToLog();

protected:
	void GetHelpStorageId(cStrObj& strHelpStorageId);

public:
	CGuiLoader    m_GuiLoader;
	hREGISTRY     m_hEnvironment;
};

LRESULT CALLBACK CBT_Proc(int nCode, WPARAM wParam, LPARAM lParam);
extern HHOOK g_hHook;

#endif // !defined(AFX_MYCOMMODULE_H__740BDE8F_98F5_4361_B340_3B68DC0665B6__INCLUDED_)
