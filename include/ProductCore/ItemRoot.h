// ItemRoot.h: interface for the CItemRoot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMROOT_H__C48F2C41_CE67_4D5F_B6A3_8E3D38ECB552__INCLUDED_)
#define AFX_ITEMROOT_H__C48F2C41_CE67_4D5F_B6A3_8E3D38ECB552__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

#include <Prague/iface/i_os.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/iface/i_event.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_semaph.h>

class  CRootItem;
struct CObjectInfo;
struct CUserInfo;
struct CBrowseObjectInfo;
struct CBindProps;
struct CFieldsBinder;
class  CFildFinder;
struct CItemBinding;
struct CSendMailInfo;
class CSkinDebugWindow;

//////////////////////////////////////////////////////////////////////

#define PROFILE_LAYOUT              (-1)
#define PROFILE_LOCALIZE            (-2)
#define PROFILE_RESOURCE            (-3)

#define GETSTRING_RAW               ((CMacroResolver *)-1)

#define PROFILE_SID_COLORS          "Colors"
#define PROFILE_SID_FONTS           "Fonts"
#define PROFILE_SID_ICONS           "Icons"
#define PROFILE_SID_BACKGNDS        "Backgrounds"
#define PROFILE_SID_BORDERS         "Borders"

#define PROFILE_SID_GLOBAL          "Global"
#define PROFILE_SID_TEMPLATES       "Templates"
#define PROFILE_SID_INCLUDE         "Include"
#define PROFILE_SID_LAYOUT          "Layout"
#define PROFILE_SID_LOCALIZE        "Localize"

#define INI_FILE_SKINROOT           "skin.ini"
#define INI_DIR_LAYOUT              "layout"
#define INI_DIR_LOCALIZE            "loc"

#define RES_DEF_FONT                ((LPCSTR)-1)

#ifndef WINAPI
#define MB_OK                       0x00000000L
#define MB_ICONINFORMATION          0x00000040L
#define MB_ICONSTOP                 0x00000010L
#endif //WINAPI

#define ITEM_ACTION_DESTROY         0x0001
#define ITEM_ACTION_CLICK           0x0002
#define ITEM_ACTION_SELECT          0x0003
#define ITEM_ACTION_CLOSEDLG        0x0004

#define EVENT_SEND_SYNC             0x0001
#define EVENT_SEND_TO_ROOT          0x0002
#define EVENT_SEND_WAIT_GLOBAL      0x0004
#define EVENT_SEND_WAIT_DIALOG      0x0008
#define EVENT_SEND_AFTER_TIMEOUT    0x0010
#define EVENT_SEND_OWN_DATA         0x0020
#define EVENT_SEND_OWN_QUEUE        0x0040

#define RES_FONTS                   0x0001
#define RES_ICONS                   0x0002
#define RES_BACKGROUNDS             0x0004
#define RES_BORDERS                 0x0008
#define RES_ALL                     0xFFFF

#define EVENT_ID_DEVICE_CHANGED     0x8bb2cf70
#define EVENT_ID_ICONEXTRUCTED      0x909f3902
#define EVENT_ID_END_LONG_USER_INACTIVITY 0xf8125fd6

#define GUIFLAG_STYLESIMPLE         0x00000001
#define GUIFLAG_ADMIN               0x00000002
#define GUIFLAG_LOCKBUTTONS         0x00000004
#define GUIFLAG_ALLOWCOMPACT        0x00000008
#define GUIFLAG_CONTROLLER_RO       0x00000010
#define GUIFLAG_NOCRYPT             0x00000020
#define GUIFLAG_CFGSTG_RO           0x00000040
#define GUIFLAG_OWNTHREAD           0x00000080
#define GUIFLAG_NOHELP              0x00000100
#define GUIFLAG_EDITOR              0x00000200
#define GUIFLAG_CONTROLLER          0x00000400
#define GUIFLAG_ASSERTS             0x00000800

#define GUI_ASSERT_MSG(_root, expr, msg) if( _root->m_nGuiFlags & GUIFLAG_ASSERTS ) PR_ASSERT_MSG(expr, msg)

#define GUI_ISSTYLE_SIMPLE( _pRoot ) ((_pRoot)->m_nGuiFlags & GUIFLAG_STYLESIMPLE)
#define GUI_COM_RO( _pRoot )         ((_pRoot)->m_nGuiFlags & GUIFLAG_CONTROLLER_RO)

#define REFRESH_PARAMS              0x00000001
#define REFRESH_SKIN_DATA           0x00000002
#define REFRESH_OPEN_DIALOGS        0x00000004
#define REFRESH_DEFLOC              0x00000008
#define REFRESH_ALL                 0x000000FF

#define GUI_OSVER_FAMILY_MASK       0xFF000000
#define GUI_OSVER_VERHIGH_MASK      0x00FF0000
#define GUI_OSVER_VERLOW_MASK       0x0000FF00
#define GUI_OSVER_VER_MASK          (GUI_OSVER_VERHIGH_MASK|GUI_OSVER_VERLOW_MASK)
#define GUI_OSVER_EXTENSION_MASK    0x000000FF
#define GUI_OSVER_BASE_MASK         (GUI_OSVER_FAMILY_MASK|GUI_OSVER_VER_MASK)

#define GUI_OSVER_FAMILY_OTHER      0x00000000
#define GUI_OSVER_FAMILY_9X         0x01000000
#define GUI_OSVER_FAMILY_NT         0x02000000

#define GUI_OSVER_VER_MAKE(_h, _l)  ( ((_h)<<16)|((_l)<<8) )

#define GUI_OSVER_WIN95             (GUI_OSVER_FAMILY_9X|GUI_OSVER_VER_MAKE(4, 0))
#define GUI_OSVER_WIN98             (GUI_OSVER_FAMILY_9X|GUI_OSVER_VER_MAKE(4, 0xA))
#define GUI_OSVER_WINME             (GUI_OSVER_FAMILY_9X|GUI_OSVER_VER_MAKE(4, 0x5A))
#define GUI_OSVER_WINNT351          (GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(3, 33))
#define GUI_OSVER_WINNT4            (GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(4, 0))
#define GUI_OSVER_WIN2000           (GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(5, 0))
#define GUI_OSVER_WINXP             (GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(5, 1))
#define GUI_OSVER_WIN2003           (GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(5, 2))

enum enStrCnv{escDef, escUTF8, escUTF8Ex};

enum enGuiEditPropsMode{guiepmUpdate, guiepmUpdateFromString, guiepmUpdateEditable};

typedef enum tag_SrvMgrRequest
{
	SRVMGR_CHECK_ENABLE,
	SRVMGR_ENABLE,
	SRVMGR_DISABLE,
	SRVMGR_RESTORE,
} tSrvMgrRequest;

//////////////////////////////////////////////////////////////////////

struct CustomHolderData
{
	virtual ~CustomHolderData() {}
	virtual void Init(CItemPropVals& pParams, CRootItem * pRoot) {}
	virtual void Destroy() { delete this; }
};

struct tSyncer : public cAutoObj<cSemaphore>
{
	tSyncer(tDWORD dwInitVal = 1){ Init(dwInitVal); }

	void Init(tDWORD dwInitVal);
};

struct tSyncEventData
{
	tSyncEventData(bool bOwnThread, tDWORD _EventId, tDWORD _Flags,
		cSerializable* _EventData, CItemBase* _SendPoint, tDWORD _nTimeOut);
	virtual ~tSyncEventData();

	void AddRef();
	void Release();

	tLONG            nRef;

	bool             m_bOwnThread;
	tDWORD           nEventId;
	tDWORD           nTimeout;
	tDWORD           nFlags;
	cSerializableObj pData;
	CItemBase *      pSendPoint;
	tSyncer          hWaiter;
	tERROR           errResult;
};				     

class CRootSink : public CItemSink
{
public:
	virtual bool   GetCustomObjectInfo(CObjectInfo& pObject){ return false; }
	virtual bool   CanClearResources(){ return false; }
	virtual bool   IsTransparentEnabled(){ return true; }
	virtual bool   IsGuiSimple(){ return true; }
	virtual bool   IsTooltipTest(){ return false; }
	virtual bool   IsGuiControllerFull(){ return false; }
	virtual tBYTE  GetDlgTransparency(){ return 200; }
	virtual void   ReportError(tTRACE_LEVEL level, LPCSTR strInfo){}
	virtual bool   CryptDecrypt(cStringObj& str, bool bCrypt) { return false; }
	virtual bool   CheckPasswords(cStringObj& strOldCrypted, cStringObj& strNew) { return false; }
	virtual hREGISTRY GetCfg(){ return NULL; }
	virtual void   GetLocalizationId(tString& strLocId){}
	virtual void   GetHelpStorageId(cStrObj& strHelpStorageId){}
	virtual bool   CheckCondition(CItemBase * pParent, LPCSTR strCondition){ return true; }
	virtual bool   ExpandEnvironmentString(cStringObj& strName){ return false; }
	virtual bool   GetAppInfoByPid(tDWORD nPID, CObjectInfo * pObjInfo){ return false; }
	virtual bool   OnAddDataSection(CFieldsBinder *pBinding, LPCSTR strSect) { return false; }
	virtual void   OnHotkey(tDWORD nVK) {}
	virtual void   OnGetSkinPath(tObjPath& strSkinPath){}
	virtual tERROR OnFileSaveError(const cStrObj& strPath, tERROR err) { return errNOT_OK; }
};

class CRootItem : public CItemBase
{
public:
	SINK_MAP_DECLARE();

	CRootItem(tDWORD nFlags, CRootSink * pSink = NULL);
	~CRootItem();

	static void          InitInstance(HINSTANCE hInstance);
	static CRootItem *   CreateInstance(tDWORD nFlags, CRootSink * pSink);
	static void          ExitInstance();
	static void          ThreadDetach();

	virtual void         Clear();
	virtual void         Cleanup();
	virtual bool         InitRoot(const tWCHAR* strSkinPath = NULL);
	virtual void         Refresh(tDWORD nFlags = REFRESH_ALL);

	virtual COLORREF     GetColor(LPCSTR strKey, COLORREF nDefVal=-1);
	virtual CFontStyle*  GetFont(LPCSTR strKey);
	virtual CIcon*       GetIcon(LPCSTR strKey);
	virtual LPCSTR       GetIconKey(CIcon* pIcon);
	virtual CIcon*       ExtractIcon(LPCSTR strPath, tDWORD nType, tDWORD nMask, CItemBase *pItemToNotify, bool bExpandPath, CIcon* pDefIcon);
	virtual void         PutIconInCache(LPCSTR strKey, CIcon *pIcon);
	virtual CBackground* GetBackground(LPCSTR strKey);
	virtual LPCSTR       GetBackgroundKey(CBackground* pBackground);
	virtual CBorder*     GetBorder(LPCSTR strKey);
	virtual bool         GetImage(LPCSTR nStrId, hIO* pSource);
	virtual void         ClearResources(tDWORD nFlags = RES_ALL);

	virtual bool         Resolve(tDWORD nFmtId, LPCSTR strMacro, tString& strResult);
	virtual LPCSTR       GetString(tString& str, tDWORD nFmtId, LPCSTR strSection, LPCSTR strKey, LPCSTR nDefVal = "", CMacroResolver* pResolver = NULL);
	virtual LPCWSTR      GetString(cStringObj &str, tDWORD nFmtId, LPCSTR strSection, LPCSTR strKey, LPCSTR nDefVal = "", CMacroResolver* pResolver = NULL);
	virtual bool         GetKeys(tDWORD nFmtId, LPCSTR strSection, tKeys& aKeys, bool bMerge = false, CMacroResolver * pResolver = NULL);
	virtual void         GetSections(tDWORD nFmtId, tSections& aSections);
	virtual bool         IsSectionExists(tDWORD nFmtId, LPCSTR strSection);
	virtual tQWORD       GetIntFromKey(LPCSTR strKey, tTYPE_ID eReqType = tid_QWORD);
	virtual LPCWSTR      GetSectionGroup(tDWORD nFmtId, LPCSTR strSection);
	virtual tERROR       SaveToSkin(CItemBase * pItem);
	virtual tERROR       DeleteFromSkin(LPCSTR strSection);

	virtual cNode*       CreateNode(LPCSTR strFormat, CFieldsBinder *pBinder, CItemBase *owner = NULL);
	virtual cNode*       CreateNode(tTYPE_ID type, tDWORD flags, tPTR data);
	virtual LPCSTR       GetFormatedText(tString &strResult, cNode *pNode, CStructData *pData = NULL);
	virtual LPCWSTR      GetFormatedText(cStrObj &strResult, cNode *pNode, CStructData *pData = NULL);
	virtual LPCSTR       GetFormatedText(tString &strResult, LPCSTR strFormat, CFieldsBinder *pBinder, CStructData *pData = NULL);
	virtual LPCWSTR      GetFormatedText(cStrObj &strResult, LPCSTR strFormat, CFieldsBinder *pBinder, CStructData *pData = NULL);
	virtual LPCSTR       GetFormatedText(tString &strResult, LPCSTR strFormat, cSerializable *pStruct);
	virtual LPCWSTR      GetFormatedText(cStrObj &strResult, LPCSTR strFormat, cSerializable *pStruct);
	virtual tString      GetErrorText(LPCSTR strEnumSect, tERROR err);

	virtual LPCSTR       LocalizeStr(tString &strResult, LPCWSTR strSource, size_t nSize = -1);
	virtual LPCWSTR      LocalizeStr(cStrObj &strResult, LPCSTR strSource, size_t nSize = -1);

	virtual void         ReportError(tTRACE_LEVEL level, LPCSTR strFmt, ...);

	virtual void         AdjustMetrics(CItemBase* pItem);
	virtual bool         DrawRect(HDC dc, RECT& rcRect, tDWORD flags, tDWORD state){ return false; }
	virtual bool         DragImage(CItemBase* pItem, CImageBase* pImage, void*& ctx, POINT& pt, enDragType type, tDWORD ipos = 0){ return false; };

	virtual void         SetTrackItem(CItemBase* pItem);

public:
	virtual bool         Activate(bool bActive);

	virtual COLORREF     GetSysColor(LPCSTR strColor){ return 0; }
	virtual tDWORD       GetSysVersion(tString &strVersion, tDWORD * nSystemInfo = NULL, tBOOL * bWinPE = NULL){ return 0; }
	virtual HDC          GetDesktopDC(){ return NULL; }
	virtual HDC          GetTempDC(RECT& rcFrame, RECT* rcUpdate){ return NULL; }
	virtual void         CopyTempDC(HDC hTempDC, HDC dc, RECT& rcFrame){}
	virtual void         ClearTempDC(){}
	virtual void         ClipRect(HDC dc, RECT* rc, bool bExclude = true){}
	virtual void         EnableToolTip(CItemBase* pItem, bool bEnable){}
	virtual bool         ShowBalloon(CItemBase* pItem, LPCSTR strTextId, LPCSTR strCaptionId = NULL){ return false; }
	virtual bool         IsThemed(){ return false; }

	virtual void         OnAction(tWORD nEventId, CItemBase * pItem, void * pParam);
	virtual void         SwapMemory() {}

	virtual bool         OpenShellObject(const tWCHAR* strUrl, const tWCHAR* strParams = NULL, const tWCHAR* strDir = NULL, CItemBase * pParent = NULL, bool bSync = false){ return false; }
	virtual bool         OpenURL(LPCSTR strURL, CItemBase * pParent = NULL){ return false; }
	virtual bool         OpenMailURL(LPCSTR strURL, CItemBase * pParent = NULL){ return false; }
	virtual bool         GotoFile(LPCSTR strPath, CItemBase * pParent = NULL){ return false; }
	virtual bool         ExecProgram(LPCSTR strPath, LPCSTR strArgs){ return false; }
	virtual tERROR       SendMail(CSendMailInfo& pInfo, CItemBase* pParent = NULL){ return errNOT_IMPLEMENTED; }
	virtual bool         DoSound(LPCSTR strSoundId){ return false; };
	virtual bool         AnimateItem(CItemBase* pItem, bool bAnimate);

	virtual int          ShowMsgBox(CItemBase* pParent, LPCSTR strTextId, LPCSTR strCaptionId, tUINT uType = MB_OK|MB_ICONINFORMATION, cSerializable * pData = NULL, CFieldsBinder * pBinder = NULL);
	virtual bool         ShowCpl(LPCSTR strAppletId){ return false; }
	virtual bool         ShowContextHelp(tDWORD dwTopicID, CItemBase* pParent){ return false; }
	virtual void         ShowDlg(CDialogSink * pSink, bool bModal, bool bSingleton, CItemBase * pParent = NULL);
	virtual void         CloseDlg(tDWORD nId);
	virtual bool         CloseAllDlgs(bool bUser = false);
	virtual tDWORD       GetWaitDlgsCount();

	virtual bool         BrowseObject(CBrowseObjectInfo& pBrowseInfo);
	virtual bool         BrowseFile(CItemBase* pParent, LPCSTR strTitle, LPCSTR strFileType, tObjPath &strPath, bool bSave = false){ return false; }
	virtual bool         BrowseUserAccount(CItemBase* pParent, CObjectInfo& pObj) { return false; }
	virtual bool         GetObjectInfo(CObjectInfo * pObjInfo);
	virtual bool         EnumDrives(tString& strDrive, tDWORD* pCookie){ return false; }
	virtual void         MessageLoop(CItemBase* pItem) = 0;
	virtual tERROR       DoCommand(CItemBase* pItem, LPCSTR strCommand, bool bGuiController = false);

	virtual tERROR       SendEvent(tDWORD nEventId, cSerializable* pEvent = NULL, tDWORD nFlags=0, CItemBase* pSendPoint = NULL, tDWORD nTimeOut = 0);
	virtual void         SendEvent(tSyncEventData * pEvent);
	virtual void         WaitEvent(tSyncEventData* pEvent, tSyncer& pSyncer);
	virtual void         ProcessEvent(tSyncEventData* pEvent);
	virtual void         PostProcessEventQueue() = 0;
	virtual void         ProcessEventQueue();

	virtual void         PostAction(tWORD nActionId, CItemBase * pItem = NULL, void * pParam = NULL, bool bSync = false){}
	virtual tERROR&      GetCurEventResult();

	virtual bool         GetRegSettings(const tCHAR *sPath, cSerializable* pStruct);
	virtual bool         SetRegSettings(const tCHAR *sPath, cSerializable* pStruct);

	virtual tSIZE_T      TrimString(tString& strVal);
	virtual tSIZE_T      TrimString(cStrObj& strVal);
	virtual tERROR       SendSrvMgrRequest(const tWCHAR* strService, tSrvMgrRequest nRequest) { return errNOT_IMPLEMENTED; }
	virtual tERROR       GetUserInfo(CUserInfo *pUserInfo) { return errNOT_IMPLEMENTED; }

	virtual bool         RegisterHotKey(tDWORD nVK) { return false; }

	virtual void         Edit_UpdateItemProps(CItemBase * pTop, CItemBase * pItem, enGuiEditPropsMode eMode = guiepmUpdate);
	virtual CItemBase *  Edit_CreateExtContainer(HWND hParent);
	virtual CItemBase *  Edit_CreateItem(CItemBase * pTop, CItemBase * pWhere, LPCSTR strId, LPCSTR strLoadInfo, LPCSTR strText = NULL, tDWORD nFlags = 0);
	virtual CItemBase *  Edit_CreateFromData(CItemBase * pTop, CItemBase * pWhere, const CItemEditData::Cd& _data, LPCSTR strId);

public:
	enum eImageType {eImageSimple, eImageIcon, eImageImage};
	
	virtual CFontStyle*       CreateFont(){ return NULL; }
	virtual CImageBase*       CreateImage(eImageType eType, hIO pIo, int nImageSizeX = 0, int nImageNotStretch = 0, int nStyle = 0, CImageBase* pImage = NULL){ return NULL; }
	virtual CIcon*            CreateIcon(HICON hIcon, bool bRef=false, CImageBase* pImage = NULL){ return NULL; }
	virtual CBackground*      CreateBackground(COLORREF nBGColor, COLORREF nFrameColor = -1, int nRadius=0, int nStyle = 0, CImageBase* pImage = NULL){ return NULL; }
	virtual CBorder*          CreateBorder(){ return NULL; }
	virtual CFieldsBinder*    CreateFieldsBinder(const cSerDescriptor* pDesc = NULL);
	virtual CItemBase*        CreateItem(LPCSTR strId, LPCSTR strType);
	virtual CItemProps*       CreateProps(LPCSTR strProps, tString& str);
	
	virtual cNode *           CreateOperand(const char *name, cNode *container, cAlloc *al);

public:
	cAutoRegistry        m_cfg;
	tString              m_strLocName;
	tDWORD               m_nLocaleId;
	tDWORD               m_nOsVer;
	cStringObj           m_strTemp1;
	cStringObj           m_strTemp2;
	cThreadPoolBase      m_ThreadPool;
	tDWORD               m_nGuiFlags;
	tBYTE                m_nDlgTransparency;
	CItemBase*           m_pClicking;
	CItemBase*           m_pClicked;
	CItemBase*           m_pSelected;
	CItemBase *          m_pTrackItem;
	CItemBase*           m_pDrawExcl;
	bool                 m_bActive;
	bool                 m_bGuiActive;
	tDWORD               m_tmLastTimer;
	tDWORD               m_tmLastUserInput;
	tDWORD               m_nTimerSpin;
	bool                 m_bRefreshRequest;
	tDWORD               m_dwScaleY;

public:
	std::vector<CDialogItem *>   m_vecDialogs;
	std::vector<tSyncEventData*> m_vecQueue;
	std::vector<CItemBase*>      m_vecAniItems;
	tDWORD                       m_nWaitDialogs;
	tSyncEventData *             m_pCurEvent;
	CSkinDebugWindow *           m_pDebugWnd;

	void                 AddDialog(CDialogItem * pItem);
	void                 RemoveDialog(CDialogItem * pItem);
	void                 SetDialogErrResult(CDialogItem* pItem, tERROR err);
	void                 DoModalDialog(CDialogItem* pItem, bool bForce = false);
	void                 RefreshDialog(CDialogItem* pItem);
	void                 ThreadLoop();
	virtual bool         IsOwnThread() { return false; }

protected:
	friend class CItemBase;
	friend class CItemEditData;
	friend class cNodeSwitch;
	friend struct cNodeFileSize;
	friend struct cNodeSkinFunc;

	struct SkinPtr
	{
		~SkinPtr() { Cleanup(); }

		bool IsInited();
		bool Init(const tWCHAR* strSkinPath);
		bool InitAlt(const tWCHAR* strSkinPath);
		bool InitPath(const tWCHAR* strSkinPath);
		bool InitZip(const tWCHAR* strSkinPath);
		void Cleanup();
		
		cAutoObj<cObjPtr>    m_pSkinPtr;
		cAutoObj<cOS>        m_pSkinOS;
		cAutoObj<cIO>        m_pSkinIO;
	};

	SkinPtr              m_pSkin;
	SkinPtr              m_pSkinAlt;
				        
	virtual void         OnInit();
	virtual void         OnInitLocale() {}
	virtual void         OnGetModulePath(tObjPath& strModulePath) = 0;
	virtual void         OnGetSysDefFont(tString& strFont) {}
	virtual void         OnTimer(UINT_PTR nTimerId);
				        
	virtual void         ReinitResources(tDWORD nFlags = RES_ALL);
	virtual bool         ActivateData(bool bActive) { return true; }
	virtual bool         SetAnimationTimer(tDWORD nTimeout) { return false; }
				        
	hREGISTRY            _GetStorage(LPCSTR strType, LPCSTR strPath, bool bReadOnly = false);
	LPCSTR               _GetString(tString& sValue, tDWORD nFmtId, LPCSTR strSection, LPCSTR nDefVal = "", CMacroResolver * pResolver = NULL);

private:
	struct tGroupSections
	{
		tGroupSections() : s_first(NULL), s_last(NULL) {}
		
		tObjPath				s_name;
		struct tagIniSection *	s_first;
		struct tagIniSection *	s_last;
	};
	
	struct tIniFile
	{
		typedef struct tagIniLine * L;
		
		struct S
		{
			struct tagIniSection * m_sec;
			std::vector<L>         m_data;
		};
		
		static int key_cmp_S(const void * key, const void * e);
		static int key_cmp_L(const void * key, const void * e);
        static int sort_cmp_S(const void * e1, const void * e2);
        static int sort_cmp_L(const void * e1, const void * e2);

		tIniFile() : m_first(NULL) {}

		void clear();
		void buildCache();
		S *  find(LPCSTR section_name);
		L *  find(S& s, LPCSTR line_name);
		
		std::vector<S>            m_data;
		struct tagIniSection *	  m_first;
	};

	typedef std::map<tString, CFontStyle*>  tMapFonts;
	typedef std::map<tString, CIcon*>       tMapIcons;
	typedef std::map<tString, CBackground*> tMapBackgrounds;
	typedef std::map<tString, CBorder*>     tMapBorders;
	typedef std::vector<tGroupSections>		tGroups;

	bool                 AddIniFile(tDWORD nFmtId, LPCSTR strFile);
	void                 AddIniFiles(tDWORD nFmtId, LPCSTR strSection, LPCSTR strPath);
	bool                 OpenLocFile(LPCSTR strFile, cIO *&pIO);
	tERROR				 IniIOCreate(hIO * result, const tObjPath& strPath, tDWORD access_mode, bool bForceRo = false);

	void                 LoadIni(bool bInit = true);
	tIniFile&            GetIni(tDWORD nFmtId);
	struct tagIniSection * GetIniSection(tDWORD nFmtId, LPCSTR name);
	LPCSTR               GetIniValue(tDWORD nFmtId, LPCSTR section_name, LPCSTR line_name, bool bSafe = false);

	bool                 InitFont(tMapFonts::iterator& pFont);
	bool                 InitIcon(tMapIcons::iterator& pIcon);
	bool                 InitBackground(tMapBackgrounds::iterator& pBackground);
	bool                 InitBorder(tMapBorders::iterator& pBorder);

	bool				 GetImageHlp(const cStringObj &strPath, hIO* pSource);

protected:
	struct CSaveCtx;
	
	tERROR				_Edit_GetIniEx(CSaveCtx& _ctx, struct tagIni ** ppIni, cStrObj& strPath);
	tERROR				_Edit_GetIni(CSaveCtx& _ctx, struct tagIni ** ppIni, LPCWSTR strGroup, bool bLocalize = false);
	struct tagIniSection *_Edit_GetSection(CSaveCtx& _ctx, struct tagIni* pIni, const tCHAR* section_name, bool bLocalize = false);
	tERROR				_Edit_Commit(CSaveCtx& _ctx);
	tERROR				_Save_Item(CSaveCtx& _ctx, CItemBase * pTop, CItemBase * pItem);
	tERROR				_Save_SectItem(CSaveCtx& _ctx, CItemBase * pTop, CItemBase * pItem);
	tERROR				_Save_ExtOverrides(CItemBase * pExtItem, tString& strProps, struct tagIni * pLoc);
	void				_Edit_DelSections(CSaveCtx& _ctx, CItemBase * pItem, bool bErase = false);
	tERROR				_Edit_CleanItemsSection(CSaveCtx& _ctx, CItemBase * pItem);
	void				_Edit_CreateFromData(CItemBase * pTop, CItemBase * pParent, const CItemEditData::Cd& _data);
	void				_Edit_PostCreateFromData(CItemBase * pTop, CItemBase * pParent);
	void				_Save_Finalize(CItemBase * pTop, CItemBase * pItem);

private:
	tMapFonts            m_mapFonts;
	tMapIcons            m_mapIcons;
	tMapBackgrounds      m_mapBackgrounds;
	tMapBorders          m_mapBorders;

	tIniFile             m_iniLayout;
	tIniFile             m_iniLocalize;
	tIniFile             m_iniResource;
	struct tagIniMm *    m_iniMm;
	tGroups				 m_iniGroups;

	CFontStyle *         m_pDefFont;

public:
	virtual cVariant * GetGlobalVar(const char *sName, bool bCreateNew = false);

	struct cCache
	{
		cCache(){ memset(this, 0, sizeof(*this)); }
		void get(cVariant &value, cNodeExecCtx &ctx, cNode** args);

	private:
		struct cItem
		{
			tQWORD    m_key;
			tDWORD    m_hit;
			cVariant  m_val;
			cSerializableObj m_obj;
		};
		cItem m_d[0x100];
	};

	typedef std::map<tString, cVariant>   tMapVariants;
	typedef std::map<tString, cCache>     tMapCaches;

	tMapVariants         m_mapVariants;
	tMapCaches           m_mapCaches;

protected:
	class Thr : public cThreadPoolBase, public cThreadTaskBase
	{
	public:
		Thr(const char* name) : cThreadPoolBase(name) { init(g_root); }
		~Thr() { de_init(); }
		
		bool start() { return PR_SUCC(cThreadTaskBase::start(*this)); }
		void do_work();
	}; friend class Thr;
	
	Thr                  m_Thread;
	cAutoObj<cEvent>     m_hEvent;
	tSyncer              m_hGSyncer;
	tSyncer              m_hESyncer;
	cAutoObj<cCriticalSection>   m_hELocker;
	cAutoObj<cCriticalSection>   m_hQLocker;
	cAutoObj<cCriticalSection>   m_accessor;

	UINT_PTR             m_idTimerRefresh;
	UINT_PTR             m_idTimerAnimation;

	CDialogItem *        m_pTopModalDialog;

protected:
	struct cNodeIsThemed : public cNode
	{
		void Destroy() {}
		bool IsConst() const { return false; }
		void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	};

	cNodeIsThemed m_nodeIsThemed;
};

//////////////////////////////////////////////////////////////////////

// For Windows start with 9x and NT 40
#define CPLID_DESKTOP            "desktop"        // Launches the Display Properties window. 
#define CPLID_COLOR              "color"          // Launches the Display Properties window with the Appearance tab preselected. 
#define CPLID_DATETIME           "date/time"      // Launches the Date and Time Properties window. 
#define CPLID_INTERNATIONAL      "international"  // Launches the Regional and Language Options window. 
#define CPLID_MOUSE              "mouse"          // Launches the Mouse Properties window. 
#define CPLID_KEYBOARD           "keyboard"       // Launches the Keyboard Properties window. 
#define CPLID_PRINTERS           "printers"       // Displays the Printers and Faxes folder. 
#define CPLID_FONTS              "fonts"          // Displays the Fonts folder. 
#define CPLID_SYSTEMINFO         "systeminfo"     // Launches the System properties window. 
#define CPLID_SOUNDS             "sounds"         // Launches the Sounds properties window. 

// For Windows 2000 and later systems:
#define CPLID_FOLDERS            "folders"        // Launches the Folder Options window. 
#define CPLID_NETWARE            "netware"        // Launches the Novell NetWare window (if installed). 
#define CPLID_TELEPHONY          "telephony"      // Launches the Phone and Modem Options window. 
#define CPLID_ADMINTOOLS         "admintools"     // Displays the Administrative Tools folder. 
#define CPLID_SCHEDTASKS         "schedtasks"     // Displays the Scheduled Tasks folder. 
#define CPLID_NETCONNECTIONS     "netconnections" // Displays the Network Connections folder. 
#define CPLID_INFRARED           "infrared"       // Launches the Infrared Monitor window (if installed). 
#define CPLID_USERPASSWORDS      "userpasswords"  // Launches the User Accounts window.

#define SHELL_OBJTYPE_UNKNOWN       0x0000
#define SHELL_OBJTYPE_DRIVE         0x0001
#define SHELL_OBJTYPE_FOLDER        0x0002
#define SHELL_OBJTYPE_FILE          0x0004
#define SHELL_OBJTYPE_MYCOMPUTER    0x0008
#define SHELL_OBJTYPE_CUSTOM        0x0020
#define SHELL_OBJTYPE_MASK          0x0040
#define SHELL_OBJTYPE_REGKEY        0x0080
#define SHELL_OBJTYPE_REGVALUE      0x0100
#define SHELL_OBJTYPE_FILTERED      0x0200
#define SHELL_OBJTYPE_URL           0x0400
#define SHELL_OBJTYPE_PERSONAL      0x0800
#define SHELL_OBJTYPE_PROCESS       0x1000
#define SHELL_OBJTYPE_USERACCOUNT   0x2000

#define OBJINFO_DISPLAYNAME         0x0001
#define OBJINFO_ICON                0x0002
#define OBJINFO_PRODUCTVERSION      0x0004
#define OBJINFO_FILEVERSION         0x0008
#define OBJINFO_COMPANYNAME         0x0010
#define OBJINFO_DESCRIPTION         0x0020
#define OBJINFO_LARGEICON           0x0040
#define OBJINFO_ALL                 (OBJINFO_DISPLAYNAME | OBJINFO_ICON | OBJINFO_PRODUCTVERSION | OBJINFO_FILEVERSION | OBJINFO_COMPANYNAME | OBJINFO_DESCRIPTION)

enum PATHOP_TYPE{PATHOP_ORIGINAL, PATHOP_FILE, PATHOP_DIR};

struct CObjectInfo : public cSerializable
{
	CObjectInfo(LPCSTR strName = NULL, tDWORD nType = SHELL_OBJTYPE_UNKNOWN) :
		m_nQueryMask(OBJINFO_ALL),
		m_strName(strName ? strName : ""),
		m_nType(nType),
		m_pIcon(NULL),
		m_nCustomId(0) {}
	
	tDWORD   m_nQueryMask;
	tDWORD   m_nType;
	tDWORD   m_nCustomId;
	
	tObjPath m_strName;
	tObjPath m_strShortName;
	tObjPath m_strDisplayName;
	tObjPath m_strShortDisplayName;
	
	cStrObj  m_strProductVersion;
	cStrObj  m_strFileVersion;
	cStrObj  m_strCompanyName;
	cStrObj  m_strDescription;
	CIcon *  m_pIcon;
};

struct CUserInfo
{
	tString  m_strName;
	tString  m_strCompany;
	tString  m_strEmail;
	tString  m_strCountry;
	tString  m_strCountryISO;
};


struct CBrowseCallBack
{
	virtual bool NextObject(tDWORD& nCustomId) { return false; }
	virtual bool FilterObject(CObjectInfo& pObject) { return true; }
	virtual void OnAddObject(CBrowseObjectInfo& pObject) {}
	virtual bool OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject) { return false; }
	virtual bool OnIsValidObject(CBrowseObjectInfo& pObject) { return true; }
	virtual bool OnCanAddObject(CBrowseObjectInfo& pObject) { return true; }
};

struct CBrowseObjectInfo : public CObjectInfo
{
	CBrowseObjectInfo(LPCSTR strSection, tDWORD nTypeMask, CItemBase* pParent = NULL,
			cSerializable* pData = NULL, CBrowseCallBack* pEnum = NULL) :
		m_pParent(pParent),
		m_pDlg(NULL),
		m_strSection(strSection),
		m_nTypeMask(nTypeMask),
		m_pData(pData),
		m_pEnum(pEnum) {}

	CItemBase *       m_pParent;
	CItemBase *       m_pDlg;
	LPCSTR            m_strSection;
	tDWORD            m_nTypeMask;
	cSerializable *   m_pData;
	cVector<cSerializable *> m_aDataEx;
	CBrowseCallBack * m_pEnum;
};

//////////////////////////////////////////////////////////////////////

class CRootTaskBase : protected cThreadTaskBase
{
public:
	CRootTaskBase(CRootItem *pRoot, tDWORD nType) : m_pRoot(pRoot), m_nType(nType){}
	virtual ~CRootTaskBase(){}

	bool start(bool bCheckInQueue = false)
	{
		if( !bCheckInQueue || !m_pRoot->m_ThreadPool.check_task_in_queue(this) )
			return PR_SUCC(cThreadTaskBase::start(m_pRoot->m_ThreadPool));
		on_exit();
		return false;
	}

	virtual void on_exit() { delete this; }
	virtual bool is_equal(CRootTaskBase* pTask){ return false; }
	virtual bool is_equal(cThreadTaskBase* pTask)
	{
		CRootTaskBase* pT = (CRootTaskBase*)pTask;
		return ( m_nType == pT->m_nType ) ? is_equal(pT) : false;
	}

protected:
	CRootItem * m_pRoot;
	tDWORD      m_nType;
};

class CIconExctructTask : public CRootTaskBase, public CObjectInfo
{
public:
	CIconExctructTask(CItemBase *pItemToNotify, const tWCHAR* strPath, tDWORD nType, tDWORD nMask, bool bExpandPath, CIcon *pDefIcon) :
		CRootTaskBase(pItemToNotify->m_pRoot, 0x33885223), m_pItemToNotify(pItemToNotify), m_bExpandPath(bExpandPath)
	{
		m_pIcon = pDefIcon;
		m_strName = strPath;
		m_nQueryMask = nMask;
		CObjectInfo::m_nType = nType;
	}

	virtual void do_work();
	virtual bool is_equal(CRootTaskBase* pTask) { return m_strName == ((CIconExctructTask *)pTask)->m_strName; }

protected:
	CItemBase * m_pItemToNotify;
	bool        m_bExpandPath;
};

class CPlaySoundTask : public CRootTaskBase
{
public:
	CPlaySoundTask(CRootItem *pRoot, const tWCHAR* strPath) : CRootTaskBase(pRoot, 0x22b77e37), m_strPath(strPath) {}

	virtual void do_work();
	virtual bool is_equal(cThreadTaskBase* pTask) { return m_strPath == ((CPlaySoundTask *)pTask)->m_strPath; }

protected:
	cStrObj m_strPath;
};

template <class T>
class CGuiPtr
{
public:
	CGuiPtr(T * p = NULL) : m_p(p) {}
	~CGuiPtr() { if( m_p ) m_p->Destroy(); }
	
	T * operator->() { return m_p; }
	operator T * ()  { return m_p; }
	T * operator =(T * p)  { return m_p = p; }

	T * m_p;
};

//////////////////////////////////////////////////////////////////////

struct cGuiParams : public cSerializable
{
	cGuiParams() :
		m_nRes(0),
		m_nVal1(0),
		m_nVal2(0),
		m_nVal3(0),
		m_nVal4(0),
		m_nVal5(0)
	{}

	DECLARE_IID( cGuiParams, cSerializable, PID_SYSTEM, bguist_cGuiParams );

	tPTR        m_nRes;
	tPTR        m_nVal1;
	tPTR        m_nVal2;
	tPTR        m_nVal3;
	tPTR        m_nVal4;
	tPTR        m_nVal5;
	cStrObj     m_strVal1;
	cStrObj     m_strVal2;
	cStrObj     m_strVal3;
	cStrObj     m_strVal4;
	cStrObj     m_strVal5;
	cStrObj     m_strVal6;
	cStrObj     m_strVal7;
};

//////////////////////////////////////////////////////////////////////

struct CAttachedFile : public cSerializable
{
	CAttachedFile(){}
	CAttachedFile(const tWCHAR* strObjPath, const tWCHAR* strName = NULL) :
		m_strObjPath(strObjPath), m_strName(strName) {}
	
	DECLARE_IID( CAttachedFile, cSerializable, PID_SYSTEM, bguist_CAttachedFile );

	cStringObj m_strObjPath;
	cStringObj m_strName;
};

//////////////////////////////////////////////////////////////////////

struct CDeviceAction : public cSerializable
{
	DECLARE_IID( CDeviceAction, cSerializable, PID_SYSTEM, bguist_CDeviceAction );

	tCHAR          m_cDevice;
	enum{ Add, Delete } m_nAction;
};

//////////////////////////////////////////////////////////////////////

typedef cVector<cStringObj>    tMailAddresses;
typedef cVector<CAttachedFile> tAttachedFiles;

struct CSendMailInfo : public cSerializable
{
	DECLARE_IID( CSendMailInfo, cSerializable, PID_SYSTEM, bguist_CSendMailInfo );

	tMailAddresses m_aAddresses;
	cStringObj     m_strSubject;
	cStringObj     m_strBody;
	tAttachedFiles m_aAattaches;
	
	cStringObj     m_strFrom;
	cStringObj     m_strUserName;
	cStringObj     m_strPassword;
	cStringObj     m_strSMTPServer;
	tDWORD         m_nSMPTPort;
};

struct cGuiMsgBox : public cSerializable
{
	DECLARE_IID( cGuiMsgBox, cSerializable, PID_SYSTEM, bguist_cGuiMsgBox );

	cGuiMsgBox() {}

	tDWORD     m_dwIcon;
	cStringObj m_strCaption;
	cStringObj m_strText;
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_ITEMROOT_H__C48F2C41_CE67_4D5F_B6A3_8E3D38ECB552__INCLUDED_)
