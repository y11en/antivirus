// TrayIcon.h: interface for the CTrayIcon class.
//
// main System Tray class. The class is responsible for displaying main tray icon
// (this icon is visible all the time while the product module is loaded)
//
//////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAYICON_H__8FB16FE0_5E9E_41A2_AE17_0B483B52CDBB__INCLUDED_)
#define AFX_TRAYICON_H__8FB16FE0_5E9E_41A2_AE17_0B483B52CDBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////////
// CTray

class CTrayMenu : public CTaskProfileView
{
public:
	CTrayMenu();
	
	bool OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem);
	void OnTimerRefresh(tDWORD nTimerSpin) {}
};

class CTray : public CDialogSink
{
public:
	CTray();
	
	CTrayItem * operator->(){ return (CTrayItem *)Item(); }

	SINK_MAP_BEGIN()
		SINK_DYNAMIC(GUI_ITEMT_MENU, CTrayMenu())
	SINK_MAP_END(CDialogSink)

	void OnCreate();
	void OnDestroy();
	void OnInit();
	void OnEvent(tDWORD nEventId, cSerializable* pData);
	void OnTimerRefresh(tDWORD nTimerSpin);

	void UpdateIcon();

private:
	CIcon *                 m_pIcon;
	tDWORD                  m_nIconState;
	CProfile *              m_pAVSProfile;

	tDWORD                  m_nTicksActiveState;
	unsigned                m_fUpdateActive : 1;
	unsigned                m_bEnableTrayIconAnimation : 1;
};

class CTrayFlash : public CDialogSink
{
public:
	typedef CDialogSink TBase;

	CTrayFlash(LPCSTR strIcon1, LPCSTR strIcon2);

	CTrayItem * operator->(){ return (CTrayItem *)Item(); }

	SINK_MAP_BEGIN()
		SINK_DYNAMIC(GUI_ITEMT_MENU, CTrayMenu())
	SINK_MAP_END(CDialogSink)

	void OnInit();
	void OnDestroy();
	void OnTimerRefresh(tDWORD nTimerSpin);
	void UpdateIcon();
	void Show(bool bShow);

private:
	tString m_strIcon1, m_strIcon2;

	CIcon *  m_pIcon1;
	CIcon *  m_pIcon2;
	bool     m_bShow1st;
};

//////////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TRAYICON_H__8FB16FE0_5E9E_41A2_AE17_0B483B52CDBB__INCLUDED_)
