// WinRoot.h: interface for the CUniRoot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINROOT_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
#define AFX_WINROOT_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_

#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_mutex.h>

#define _S2Q(_s)     ((CUniRoot *)m_pRoot)->_2QS(_s)
#define _Q2S(_q, _s) ((CUniRoot *)m_pRoot)->_2PS(_q, _s)

//////////////////////////////////////////////////////////////////////

class CUniRoot :
	public CRootItem,
	public QObject
{
public:
	SINK_MAP_DECLARE();

	CUniRoot(tDWORD nFlags, CRootSink * pSink = NULL);
	~CUniRoot();

	void         SendEvent(tSyncEventData * pEvent);
	void         PostProcessEventQueue();
	void         PostAction(tWORD nActionId, CItemBase * pItem, void * pParam, bool bSync = false);
	bool         SetAnimationTimer(tDWORD nTimeout);

public:
	void         MessageLoop(CItemBase * pItem);
	bool         ActivateData(bool bActive);
	bool         IsOwnThread();

	QString&     _2QS(LPCSTR str);
	QString&     _2QS(LPCSTR str, QString& q);
	QString&     _2QS(cStrObj& str, QString& q);
	QString&     _2QS(LPCWSTR str);
	void         _2PS(const QString& q, cStrObj& s);
	void         _2PS(const QString& q, tString& s);

public:
	CPlaceHolder *   CreateHolder(tDWORD nId, LPCSTR strType);
	CFontStyle *     CreateFont();
	CImageBase *     CreateImage(eImageType eType, hIO pIo, int nImageSizeX = 0, int nImageNotStretch = 0, int nStyle = 0, CImageBase* pImage = NULL);
	CIcon *          CreateIcon(HICON hIcon, bool bRef = false, CImageBase * pImage = NULL);
	CBackground *    CreateBackground(COLORREF nBGColor, COLORREF nFrameColor = -1, int nRadius = 0, int nStyle = 0, CImageBase* pImage = NULL);
	CItemBase *      CreateItem(LPCSTR strId, LPCSTR strType);
	HDC              GetDesktopDC();

	bool             BrowseFile(CItemBase * pParent, LPCSTR strTitle, LPCSTR strFileType, tObjPath& strPath, bool bSave = false);
	bool             BrowseApp(CItemBase * pItem, tObjPath& strAppPath);
	bool             GetObjectInfo(CObjectInfo * pObjInfo);
	bool             EnumDrives(tString& strDrive, tDWORD * pCookie);
	
	bool             IsThemed(){ return true; }

protected slots:
	bool event(QEvent *);
	void timerEvent(QTimerEvent *);

	void OnEventHandler(tPTR wParam, tPTR lParam);
	void OnActionHandler(tPTR wParam, tPTR lParam);

protected:
	void    OnGetSysDefFont(tString& strFont);
	void    OnGetModulePath(tObjPath& strModulePath);

protected:
	void _InitDrivesList();

public:
	tDWORD              m_nTimerSpin;
	QApplication *      m_app;
	QString             m_qsTemp;

	QPainter *          m_pDesktopDC;
	QPen *              m_pPenGrayed;
	QPen *              m_pPenHotlight;
	QFileIconProvider * m_pIconProvider;

	QFileInfoList       m_setDrives;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_WINROOT_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
