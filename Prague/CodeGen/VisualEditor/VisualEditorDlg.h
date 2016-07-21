// VisualEditorDlg.h : header file
//
//{{AFX_INCLUDES()
#include "splitterctrl.h"
//}}AFX_INCLUDES

#if !defined(AFX_VISUALEDITORDLG_H__9D0A11D7_C8F2_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_VISUALEDITORDLG_H__9D0A11D7_C8F2_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <limits.h>
#include <avpcontrols/controltree.h>
#include <avpcontrols/wascmess.h>
#include <property/property.h>
#include <stuff/parray.h>
#include <stuff/cpointer.h>
#include <stuff/pathstr.h>
#include <stuff/sarray.h>
#include <stuff/regstorage.h>
#include <stuffio/utils.h>
#include <stuffmfc/xstatusbar.h>
#include <avpcontrols/propertysetget.h>
#include <pr_compl.h>  // compiler dependent macro definitions
#include <pr_types.h>
#include "../ifaceinfo.h"

#include "../avppveid.h"
#include "../idserverinterface.h"
#include "../pgiserverinterface.h"

#include "vecontroltree.h"
#include "resource.h"
#include "selectloadedinterfacesdlg.h"

#include "Options.h"
#include "PVE_DT_UTILS.h"

#define MAX_RECENT_FILES		9
#define ID_FIRSTRECENTFILE		33000
#define CONTENTS_SCROLL_SIZE	5

#define CFG_REG_KEY					_T("PragueVisualEditor")
#define SPLIT_TLR_REG_VALUE			_T("SplitTopLR")
#define SPLIT_LTB_REG_VALUE			_T("SplitLeftTB")
#define WINDOWPLACEMENT_REG_VALUE	_T("WindowPlacement")
#define PRT_INITDIR_REG_VALUE		_T("PRTInitialDir")
#define IMP_INITDIR_REG_VALUE		_T("IMPInitialDir")
#define LCPLACEMENT_REG_VALUE		_T("LargeCommentPlacement")
#define LCFONT_REG_VALUE			_T("LargeCommentFont")
#define SLIPLACEMENT_REG_VALUE		_T("SelectInterfacesPlacement")
#define TECH_EDITING_MODE_REG_VALUE _T("TechnicalEditingMode")
#define LOCAL_EDITING_MODE_REG_VALUE _T("LocalEditingMode")
#define COMM_LANG_REG_VALUE			_T("CommentLang")

#define DEFAULT_VAR_TYPE      tid_DWORD
#define DEFAULT_METHOD_TYPE   tid_ERROR

// ---
struct CNodeUID {
	TCHAR m_UID[41];
	CNodeUID()	{ memset(m_UID, 0, sizeof(m_UID)); }
	CNodeUID( TCHAR *pUID, DWORD dwSize)	{ 
		memset(m_UID, 0, sizeof(m_UID)); 
		memcpy(m_UID, pUID, min(dwSize, sizeof(m_UID) - 1));
		if ( lstrlen(m_UID) < sizeof(m_UID) - 1 )
			memset(m_UID + lstrlen(m_UID), 0, sizeof(m_UID) - lstrlen(m_UID));
	}
	operator LPSTR () { return m_UID; }
	operator LPCSTR () { return m_UID; }

};

// ---
struct TypeID {
	CAPointer<TCHAR> m_pName;
	AVP_dword				 m_nType;
	AVP_dword				 m_nDTType;
	AVP_dword				 m_nSizeOf; 
};

// ---
struct PredefinedProp {
	CAPointer<TCHAR>	m_pName;
	DWORD							m_nID;
	AVP_dword					m_nType;
	CAPointer<TCHAR>	m_pTypeName;
	AVP_dword					m_nDTType;
	AVP_bool					m_bShared;
	CAPointer<TCHAR>	m_pDefValue;
	CAPointer<TCHAR>	m_pShortComment;
	CAPointer<TCHAR>	m_pLargeComment;
	CAPointer<TCHAR>	m_pBehaviorComment;
};

// ---
struct ConstValue {
	CAPointer<TCHAR>	m_pConstName;
	AVP_qword			m_dwValue;
	CAPointer<TCHAR>	m_pValue;
};

// ---
struct TypeConst {
	AVP_dword			m_dwPragueType;
	CPArray<ConstValue>	m_Constants;
};

// ---
enum ENodeTypeMark
{
	sdt_Interface = 1,
	sdt_Type,
	sdt_Constant,
	sdt_Property,
	sdt_Method,
	sdt_MethodParam,
	sdt_SysMethod,
	sdt_DataStructure,
	sdt_DataStructureMember,
	sdt_Desctription,
	sdt_Parameters,
	sdt_PropScopeLocal,
	sdt_PropScopeShared,
	sdt_PropLocalNone,
	sdt_PropLocalRead,
	sdt_PropLocalWrite,
	sdt_PropLocalReadWrite,
	sdt_PropLocalMemberRead,
	sdt_PropLocalMemberWrite,
	sdt_PropLocalMemberReadWrite,
	sdt_Plugin,
	sdt_Includes,
	sdt_ScrollListNewValue,
	sdt_ScrollListValue,
	sdt_ErrorCode,
	sdt_MessageClass,
	sdt_Message,
	sdt_Label = 0x8000,
};

// ---
struct CPPDescriptor 
{
	HDATA	m_hData;
	HPROP	m_hProp;
	int		m_nSpecialType;
	int		m_nArrayIndex;
	int		m_nScrollPos;

	CPPDescriptor( 
			HDATA i_hData = NULL, 
			HPROP i_hProp = NULL, 
			int	i_nSpecialType = 0, 
			int i_nArrayIndex = 0, 
			int i_nScrollPos = 0) :
		m_hData( i_hData ),
		m_hProp( i_hProp ),
		m_nSpecialType( i_nSpecialType ),
		m_nArrayIndex( i_nArrayIndex ),
		m_nScrollPos( i_nScrollPos )
		{}

	~CPPDescriptor() {}
};


#define CPPD(item)								((CPPDescriptor *)item->GetData())
#define IS_EMPTY_DATA(item)						(item->GetData() == NULL)
#define IS_NODE_LABEL(item)						(CPPD(item)->m_nSpecialType &  sdt_Label)
#define IS_NODE_INTERFACE(item)					((CPPD(item)->m_nSpecialType & ~sdt_Label) == sdt_Interface)
#define IS_NODE_METHOD(item)					((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_Method)
#define IS_NODE_TYPE(item)						((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_Type)
#define IS_NODE_CONSTANT(item)					((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_Constant)
#define IS_NODE_ERRORCODE(item)					((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_ErrorCode)
#define IS_NODE_PROPERTY(item)					((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_Property)
#define IS_NODE_METHODPARAM(item)				((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_MethodParam)
#define IS_NODE_DATASTRUCTURE(item)				((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_DataStructure)
#define IS_NODE_DATASTRUCTUREMEMBER(item)		((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_DataStructureMember)
#define IS_NODE_EQUAL(item, type)				((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	type)
#define GET_NODE_TYPE(item)						(CPPD(item)->m_nSpecialType & ~sdt_Label)
#define IS_NODE_MESSAGE_CLASS(item)				((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_MessageClass)
#define IS_NODE_MESSAGE(item)					((CPPD(item)->m_nSpecialType & ~sdt_Label) ==	sdt_Message)

#define CONTENTS_SCROLL_SIZE 5

// ---
class CRecentFiles : public CPArray<char> 
{
public :
	CRecentFiles() : CPArray<char>(0, 1, true) {}
	void Shake( const char *pFileName );
	BOOL Find( const char *pFileName );
};

// ---
struct CIObject 
{
	DWORD		m_dwRefCount;
	DWORD		m_dwObjectID;
	CString		m_rcObjectName;
	CString		m_rcIncludeName;
	BOOL		m_bInterface;
	CNodeUID	m_UID;
	CIObject( const TCHAR *pObjectName ) :
		m_dwRefCount( 0 ),
		m_dwObjectID( 0 ),
		m_bInterface( FALSE ),
		m_rcIncludeName(),
		m_rcObjectName( pObjectName ) {}
};

// ---
class CIObjectContainer : public CPArray<CIObject> 
{
public :
	CIObjectContainer();
	CIObject *Find( const TCHAR *pObjectName );
	CIObject *Find( AVP_dword dwID );

	CIObject *Add( const TCHAR *pObjectName );

	CIObject *Update( const TCHAR *pOldObjectName, const TCHAR *pNewObjectName );
	CIObject *Update( AVP_dword dwOldID, AVP_dword dwNewID );
	BOOL			IsIncludeNameUsed( CIObject *pIObject );
};


// ---
enum {
	wmt_Undefined			= -1,
	wmt_IDefinition,
	wmt_IImplementation,
	wmt_CGOptions,
};

namespace MD
{
	enum ETreeNodeType
	{
		eString,
		eDDList,
		eStrDDList,
		eStrExtEdit,
		eCheck,
		eRadio,
		eDate,
		eTime,
		eDatetime,
	};

	typedef ETreeNodeType (*pfnPropGetDisplayType)(HDATA,AVP_dword,ETreeNodeType);
	typedef HPROP (*pfnPropCostructor)(HDATA,AVP_dword,AVP_dword,BOOL&);
	typedef	TCHAR* (*pfnGetDisplayNameHelper)(HDATA,HPROP,int,int);

	struct SPropMetadata
	{
		int				m_nPropId;
		int				m_nPrefixStrId;
		ETreeNodeType	m_eTreeNodeType;
		AVP_dword		m_dwPropDefVal;

		pfnPropCostructor		m_pfnPropConstructor;
		pfnGetDisplayNameHelper m_pfnGetDisplayNameHelper;
		pfnPropGetDisplayType	m_pfncPropGetDisplayType;
	};

	typedef HDATA (*pfnDataCostructor)(HDATA,UINT,UINT,CHAR*);

	struct SDataMetadata
	{
		UINT		  m_uiPropertyType;
		UINT		  m_uiNamePropId;
		ENodeTypeMark m_eTreeDataMark;
		int			  m_nStrNewNodeId;
		pfnDataCostructor m_eDataConstructor;
	};
};

/////////////////////////////////////////////////////////////////////////////
// CVisualEditorDlg dialog
// ---
class CVisualEditorDlg : public CDialog {
// Construction
public:
	CVisualEditorDlg(CWnd* pParent = NULL);	// standard constructor

	void	SetPropTypifiedDefValue( HDATA hData );

	//{{AFX_DATA(CVisualEditorDlg)
	enum { IDD = IDD_VISUALEDITOR_DIALOG };
	AVP_dword			m_nWorkMode;
	CVEControlTree		m_Interfaces;
	CVEControlTreeBase	m_IDescriptions;
	CSplitterCtrl		m_Splitter;
	CRegStorage			m_RegStorage;
	CRecentFiles		m_RecentFiles;
	HACCEL				m_hAccelTable;
	BOOL				m_bStateRestored;
	BOOL				m_bChanged;
	int					m_nAppID;
	HDATA				m_hTree;
	HDATA				m_hDefinitionData;
	HDATA				m_hCCTree;
	HDATA				m_hCutData;
	UINT				m_nInterfacesClipFormat;
	UINT				m_nDescriptionsClipFormat;
	CString				m_FileName;
	CString				m_IDefinitionFileName;
	CString				m_IImplementationFileName;
	CIObjectContainer	m_IOTable;
	XStatusBar			m_StatusBar;
	IPragueIDServer*    m_pIIDServer;
	IPragueIServer*		m_pIIServer;
	BOOL				m_bTechnicalEditting;
	BOOL				m_bLocalEditting;
	CStdTypeInfo		m_types;
	//}}AFX_DATA

	UINT				m_uiIdleTimerId;
	SOptions			m_oOptions;
	HICON				m_hIcon;

	//{{AFX_VIRTUAL(CVisualEditorDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:		 
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CVisualEditorDlg)
	afx_msg void OnOK();
	afx_msg void OnAppExit();
	afx_msg void OnSysCommand( UINT nID, LONG lParam );
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup( CMenu* pMenu, UINT nIndex, BOOL bSysMenu );
	afx_msg LRESULT OnRestoreDlgState( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTPostLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTLoadComboContents( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTWindowShutDown( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTLoadUpDownContext( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTBeginLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTCancelLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTCheckedChanging( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTRemoveItem( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTItemSelChanging( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTStateChanged( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTEndLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTQueryDate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTQueryDateFormat( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTEndDateLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTQueryTime( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTQueryTimeFormat( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTEndTimeLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTQueryDateTime( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTQueryDateTimeFormat( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTEndDateTimeLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTPreptocessContextMenu( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTDoContextMenuCommand( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTItemExpanding( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTLoadIPAddress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTEndAddressLabelEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTGetMenuToolbar( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTDoExternalEditing( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTVScrollUp( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTVScrollDown( WPARAM wParam, LPARAM lParam );
	afx_msg void OnFileExit();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileNewImp(CCmdUI* pCmdUI);
	afx_msg void OnFileNewImp();
	afx_msg void OnFileSrcgen();
	afx_msg void OnUpdateFileImportDef(CCmdUI* pCmdUI);
	afx_msg void OnFileImportDef();
	afx_msg void OnFileSynchronize();
	afx_msg void OnFileReinitIServer();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSrcgen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSynchronize(CCmdUI* pCmdUI);
	afx_msg void OnFileAddDef();
	afx_msg void OnUpdateFileAddDef(CCmdUI* pCmdUI);
	afx_msg void OnFileNewdefByBase();
	afx_msg void OnUpdateFileNewdefByBase(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// ------------------------------------------------------------------------------------------
	//		tree macro instruments
public:

	struct	STreeNodeInfo
	{
		HDATA			m_hData; 
		int				m_nPropId; 
		int				m_nArrayPosition;
		int				m_nSpecialTypeId; 
		int				m_nItemLevel;
		BOOL			m_bEditable; 
		BOOL			m_bCanBeRemoved;
		LPCTSTR			m_strTitle;
		int				m_nPrefixId;
		CCTItemsArray&	m_newItems;
		MD::ETreeNodeType m_eTreeNodeType;
		DWORD			m_dwInitData;
		CControlTreeItem* m_pExportedItem;

		STreeNodeInfo
		(
			CCTItemsArray&	i_newItems,
			int				i_nItemLevel,
			HDATA			i_hData, 
			int				i_nPropId, 
			BOOL			i_bEditable = TRUE, 
			BOOL			i_bCanBeRemoved = FALSE,
			int				i_nSpecialTypeId = 0
		);
	};

	// ------------------------------------------------------------------------------------------
	static MD::SPropMetadata	s_arrPropMetadata[];
	static MD::SPropMetadata*	FindPropMetadata(int i_nPropId, BOOL i_bPropUniqueId = FALSE);
	static MD::SDataMetadata	s_arrDataMetadata[];
	static MD::SDataMetadata*	FindDataMetadata(UINT i_nTypeId);

	static HPROP CreateDataProp(HDATA i_hData, int i_nPropId);
	HDATA	CreateDataData(HDATA i_hData, UINT i_uiDataId, CHAR* i_strName);

	void	CreateTreeNode(STreeNodeInfo& i_oTreeNodeInfo);

	void	CreatePropertyNode(CCTItemsArray & o_newItems, int i_nFirstItemLevel, 
				HDATA i_hData, int i_nPropId, 
				BOOL i_bEditable = TRUE, BOOL i_bCanBeRemoved=FALSE, int i_nSpecialTypeId = 0, 
				DWORD i_dwInitData = 0, int i_nPrefixId=0);

	void	CreateStringNode(CCTItemsArray & o_newItems, int i_nFirstItemLevel, 
				HDATA i_hData, UINT i_StrResId, 
				BOOL i_bEditable = TRUE, BOOL i_bRemovable = FALSE, 
				int i_nSpecialTypeId = 0, MD::ETreeNodeType i_eTreeNodeType=MD::eString);

	void	CreateStringNode(CCTItemsArray & o_newItems, int i_nFirstItemLevel, 
				HDATA i_hData, LPCTSTR i_strTitle, 
				BOOL i_bEditable = TRUE, BOOL i_bRemovable = FALSE, 
				int i_nSpecialTypeId = 0, MD::ETreeNodeType i_eTreeNodeType=MD::eString);

	void	PrepareDisplayString(CTTMessageInfo* i_pInfo, HDATA i_hData, HPROP i_hProp, 
		int i_nArrayPosition, BOOL i_bWithPrefix);

	BOOL	GetPropDisplayString(CString& o_strStr, HDATA i_hData, HPROP i_hProp, 
		int i_nArrayPosition, BOOL i_bWithPrefix);
	// ------------------------------------------------------------------------------------------
	struct SExtEditTitle
	{
		int m_nPropId;
		int m_nStrId;
	};

	static SExtEditTitle s_arrExtEditTitles[];
	static int GetExtEditTitleId(int i_nPropId);

	// ------------------------------------------------------------------------------------------

	void GetNotificationData(LPARAM i_lParam,
		CTTMessageInfo*& o_info,
		CVEControlTreeBase*& o_pTreeCtrl,
		CCTItemsArray*&	o_pCtrlItems,
		int&			o_nItemIndex,
		CCheckTreeItem *& o_pItem,
		HDATA& o_hData,
		HPROP& o_hProp);

	// -----------------------------------------------------------------------------------------//
	//																							//
	//											LOADING											//
	//																							//
	// -----------------------------------------------------------------------------------------//

	//	interface / plugin properties

	void	LoadInterfaceDescriptionNodes( HDATA hRootData );
	void	LoadOnePluginStuff( HDATA hPluginData, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadOneInterfaceStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );

 	void	LoadInterfaceMnemonicIDStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceDigitalIDStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceRevisionIDStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceCreationDateStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceObjectTypeStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceIncludeFileStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceOutputFileStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceAutorNameStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfaceVendorIDStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );

	//	interface / plugin descrioption
	void	LoadInterfaceDescriptionTreeNodes( HDATA hRootData );

	void	LoadInterfaceSubTypeStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadInterfaceFlagsStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadInterfaceIncludesNodes( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );

	// interface types
	void	LoadInterfaceTypesNodes( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceTypeStuff( HDATA hTypeData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );

	// interface constants
	void	LoadInterfaceConstantsNodes( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceConstantStuff( HDATA hTypeData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );

	// interface messages
	void	LoadInterfaceMessagesNodes( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceMessagesClassStuff( HDATA i_hTypeData, CCTItemsArray &i_oNewItems, int i_nFirstItemLevel, BOOL i_bLinkedToFile); 
	void	LoadOneInterfaceMessagesClassMessagesStuff( HDATA i_hTypeData, CCTItemsArray &i_oNewItems, int i_nFirstItemLevel, BOOL i_bEditable, BOOL i_bLinkedToFile); 

	// interface error codes
	void	LoadInterfaceErrorCodeNodes( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceErrorCodeStuff( HDATA hErrorCodeData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );

	// interface data structures
	void	LoadInterfaceDataStructuresNodes( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceDataStructureStuff(HDATA hDataStructureData, CCTItemsArray &newItems, int nFirstItemLevel);
	void	LoadOneInterfaceDataStructureMemberStuff(HDATA hDataStructureMemberData, CCTItemsArray &newItems, int nFirstItemLevel);

	// interface properties
	void	LoadInterfacePropertiesNodes( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadNewHardPropertiesDescriptionTreeStuff( HDATA hParentData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadNewHardPropertyDescriptionTreeStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );

	void	LoadOneInterfacePropertyStuff( HDATA hPropData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadInterfacePropertyStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bNativeProperty, BOOL bLinkedToFile );
	void	LoadInterfacePropertyLocalMemberStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL i_bEditable );
	void	LoadInterfacePropertyLocalModeStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL i_bEditable );

	// interface methods
	void	LoadInterfaceMethodsNodes( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceMethodStuff( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile);
	void	LoadInterfaceMethodResultNameStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable );
	void	LoadInterfaceMethodResultDefValueStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable );
	void	LoadInterfaceMethodParametersDescriptionNodes( HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable);
	void	LoadOneInterfaceMethodParameterStuff(HDATA hMethodData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable );
	void	LoadInterfaceMethodParamConstStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable);

	// system methods
	void	LoadInterfaceSystemMethodsNodes( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile );
	void	LoadOneInterfaceSystemMethodStuff( HDATA hMethodData, CCTItemsArray &newItems, int nFirstItemLevel );

	// something
	void	LoadPluginDescriptionTreeNodes( HDATA hRootData, BOOL bSetToWindow = true );

	// common loading helpers
	void	LoadShortCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile = false);
	void	LoadLargeCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile = false);
	void	LoadBehaviorCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile = false );
	void	LoadValueCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile = false );
	void	LoadImplCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile = false );

	void	CreateValueCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );
	void	CreateBehaviorCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );
	void	CreateLargeCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );
	void	CreateShortCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel );

	// -----------------------------------------------------------------------------------------//
	//																							//
	//											CREATING										//
	//																							//
	// -----------------------------------------------------------------------------------------//
	void	OnCreateData(HDATA i_hData);
	void	OnLoadData(HDATA i_hData, BOOL i_bLinkedToFile);

	void	InitCreationParams(CCTItemsArray& items, int nItemInd, 
		HDATA& o_hParent, int& o_nParentIndex, CString& o_strItemText, int& o_nItemLevel);

	void	CreateData(CCTItemsArray& items, int nItemInd, UINT i_nDataType);
	void	PopulateData(HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel);

	void	CreateInterfaceTypeNodeContents( CCTItemsArray &items, int nItemInd );
	void	CreateInterfaceConstantNodeContents( CCTItemsArray &items, int nItemInd );
	void	CreateInterfaceMessageClassNodeContents( CCTItemsArray &items, int nItemInd ); 
	void	CreateInterfaceMessageNodeContents( CCTItemsArray &items, int nItemInd ); 
	void	CreateInterfaceErrorCodeNodeContents( CCTItemsArray &items, int nItemInd );
	void	CreateInterfacePropertyNodeContents( CCTItemsArray &items, int nItemInd );

	void	CreateInterfaceNodeContents( CCTItemsArray &items, int nItemInd );
	void	CreateNewDescriptionTreeStuff();

	void	CreateInterfaceMethodNodeContents( CCTItemsArray &items, int nItemInd );
	void	CreateInterfaceMethodParamNodeContents( CCTItemsArray &items, int nItemInd );

	// data structures
	void	CreateInterfaceDataStructureNodeContents( CCTItemsArray &items, int nItemInd );
	void	CreateInterfaceDataStructureMemberNodeContents( CCTItemsArray &items, int nItemInd );

	void	CreateNewInterfaceTreeStuff();

	// -----------------------------------------------------------------------------------------//
	//																							//
	//											SYNCRONIZING									//
	//																							//
	// -----------------------------------------------------------------------------------------//
	void	SetImpTreePrototypeFile( HDATA hImpTreeData, CString &rcFileName );
	HDATA	SynchronizeImpAndDef( HDATA hSynchroData );
	void	SynchronizeImpAndDef( CString &rcFileName );

	void	SyncDatasCommon(HDATA io_hNewInt, HDATA i_hProtoInt, HDATA i_hImpInt, UINT i_nDataId);



	// -----------------------------------------------------------------------------------------//
	BOOL	IsModified();
	void	SetModified( BOOL bModified );
	BOOL	IsPrototype();
	BOOL	IsImplementation();
	BOOL	IsEditableData(HDATA i_hData, BOOL i_bLinkedToFile=false);

	void	LoadTypesTable();
	void	ScanTreeForSetIDInUse();
	void	DisplayPIDSError( HRESULT hr );
	void	DisplayPISError( HRESULT hr );

	BOOL	SaveChangedRequest( BOOL bStrictSave = false );
	BOOL	PerformFileOpenDlg( DWORD dwExtID, DWORD dwFilterID, CString &rcString );
	BOOL	PerformFileSave();
	BOOL	PerformFileSaveAs();
	BOOL	SaveBakFile();
	BOOL	SaveInterfaces( BOOL bSaveAs, LPCTSTR pszFileName = NULL );
	void	SaveDescriptionDB();
	void	CheckLinkedInterfaces( HDATA hRootData, BOOL bRecursiveImported = FALSE );
	BOOL	LoadInterfaces( CString rcFileName, BOOL bEpenOrLoad );
	void	CheckAndRemoveLinkedInterfaces( HDATA hData, int nItemInd );

	void	ExchangeIncludes( const char *pOldIncludeName, const char *pNewIncludeName );
	void	SetObjectIncludeName( HDATA hData, const char *pIncludeName, BOOL bUpdate = false );

	void	FlushDataTree( HDATA hRootData );
	void	InitNewTree( HDATA hRootData = NULL );
	void	ReloadInterfaceCCTree( HDATA hRootData );
	void	ReloadPluginCCTree( HDATA hRootData );

	BOOL	CheckInterfaceMessageClassName( HDATA hData, int nItemInd, const char *pText ); 
	BOOL	CheckInterfaceMessageName( HDATA hData, int nItemInd, const char *pText ); 
		
	BOOL	EditLargeComment( CTTMessageInfo *info, DWORD dwTitleID, DWORD dwPrefixID, HDATA hData, HPROP hProp, BOOL bReadOnly, CWnd *pParent = NULL );
	BOOL	EditLocalScopeMember( CTTMessageInfo *info, DWORD dwPrefixID, HDATA hData, HPROP hProp );

	void	FillAndSetClipboard( HDATA hRootData, UINT nClipFormat );
	HDATA	DeserializeClipboardData( UINT nClipFormat );

	BOOL	CheckTypeIsEqual( HDATA hAttachData, CCheckTreeItem *item );
	void	AttachInterfaceDataTree( HDATA hAttachData );
	void	AttachDescriptionDataTree( HDATA hAttachData );

	BOOL	IsEditPasteEnable();
	BOOL	IsEditCopyEnable();
	BOOL	IsEditCutEnable();

	void	PreProcessMenu();
	void	ShakeRecentMenu();
	BOOL	LoadRecentFile( int nIndex );

	void	LinkInterfaceDefinition( HDATA hTargetData, HDATA hInterfacesToLink );
	void	AttachLinkedInterface( HDATA hRootData, HDATA hInterfaceData );
	void	CheckDuplicateIntName( HDATA hRootData, const char *pIntName );
	BOOL	CheckAlreadyImported( HDATA hRootData, const char *pFileName );
	BOOL	CheckAlreadyImported( HDATA hRootData, CNodeUID &rcUID );
	BOOL	LoadImportInterfacesDefinition( const char *pFileName, const char *pRelativeFileName, HDATA hRootData, BOOL bLinkToFile );
	BOOL	LoadImportInterfacesDefinitionByUID( CNodeUID &rcUID, HDATA hRootData );
	void	LoadImportDefinitionFile( CString &rcFileName, BOOL bLinkToFile );
	BOOL	UpdateInterfacesIncludesByUID( CNodeUID &rcUID, HDATA hRootData );

	BOOL	CheckPluginID( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceName( HDATA hData );
	BOOL	CheckInterfaceName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceID( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceRevisionID( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceTypeName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceConstantName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceErrorCodeName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceMethodName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceMethodParamName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfacePropertyName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfacePropertyID( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckInterfaceObjectTypeName( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckPropertyDefValue( HDATA hData, int nItemInd, const char *pText );
	BOOL	CheckPropType( HDATA hData, int nItemInd, AVP_dword dwNewType );
	BOOL	CheckTypeNameIsUsed( const char *pText );
	BOOL	CheckTypeNameIsUsedForDelete( const char *pText, BOOL* pbUsed=NULL);

	void	ExchangeTypesByInterfaceID( CIObject *pObject );
	void	ExchangeTypesByInterfaceObject( CIObject *pObject );
	void	ExchangeTypesByTypeName( LPCTSTR pOldTypeName, LPCTSTR pNewTypeName);
	void	ExchangeConstantsByConstantName( LPCTSTR pOldConstantName, LPCTSTR pNewConstantName );
		
	void	LoadInterfaceDataStructureMemberShortCommentStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel );
	void	LoadObjectsTable( HDATA hRootData );
	void	CreateOrUpdateInterfaceMnemonicIDStuff( HDATA hData );
	void	CreateOrUpdateInterfaceDigitalIDStuff( HDATA hData );
	void	CreateOrUpdateInterfaceRevisionIDStuff( HDATA hData );
	void	CreateOrUpdateInterfaceCreationDateStuff( HDATA hData );
	void	CreateOrUpdateInterfaceObjectTypeStuff( HDATA hData );
	void	CreateOrUpdateInterfaceIncludeFileStuff( HDATA hData );
	void	CreateOrUpdateInterfaceAutorNameStuff( HDATA hData );
	void	CreateOrUpdateInterfaceVendorIDStuff( HDATA hData );
	void	CreateOrUpdateInterfaceStuff( HDATA hData );

	void	EnableEditableStuff();

	void	SetObjectIsUsed( const TCHAR *pObjectName );
	AVP_dword AcceptUsedObject( const TCHAR *pObjectName, AVP_dword &dwType );
	void	RefuseUsedObject( HDATA hData, AVP_dword dwTypePropID );
	void	ReloadIncludesTree();

	void	ProcessObjectTable( HDATA hData, AVP_dword dwRootPID, AVP_dword dwNamePID );

	void	LoadContentsList( CCTItemsArray &items, HDATA hData, HPROP hProp, int nFirstItemLevel, int nScrollPos, BOOL bReadOnly );
	void	ReloadContentsList( CCTItemsArray &items, int nParentInd, HDATA hData, HPROP hProp );
	void	UpdateContentsScrollItems( HDATA hData, HPROP hProp, int nScrollPos, LPARAM lParam );

	BOOL	GetImportFileName( CString &rcFileName, BOOL &bLinked );
	BOOL	GetSynchonizeFileName( CString &rcFileName );

	void	SetErrorCodeDisplayString( HDATA hErrorCodeData );
	void	SetMethodDisplayString( HDATA hMethodData );
	void	SetPropertyDisplayString( HDATA hPropData );
	void	SetConstantDisplayString( HDATA hConstantData );

	void	PreparePredefinedPropComments( PredefinedProp *pProp, HDATA hData );
	void	PreparePredefinedProp( CPArray<PredefinedProp> &rcProps, const TCHAR *pName, HDATA hData );

	void	SetWindowTitle(BOOL i_bUpdateRegistry = TRUE);

	typedef void (CVisualEditorDlg::*PreprocesEnumedInterfacesFunc)(HDATA hRootData);
	BOOL	EnumAndSelectInterfaces( HDATA *hResultData, PreprocesEnumedInterfacesFunc pPreprocessFunc = NULL, HDATA hSourceData = NULL, BOOL bFlushSelected = TRUE );

	void	PreprocessForAddDef( HDATA hRootData );

	// filling oTypeNames list with available type names, excluding pExcludeThisType
	void	FillTypeNamesList(CStringList& oTypeNames, TCHAR* pExcludeThisType);
	BOOL	CheckPluginDefaultName();

public :
	void	EnableInterfacePropertyLocalWriteStuff( HDATA hNodeData, HPROP hProp, int nIndex );

	friend class CSelectLoadedInterfacesDlg;
	friend class CMethDlg;
};

// inlines
#include "VisualEditorDlg.inl"

#define NUMERIC_TYPE(type) \
				(	type == avpt_short		|| \
					type == avpt_long			|| \
					type == avpt_byte			|| \
					type == avpt_word			|| \
					type == avpt_dword		|| \
					type == avpt_qword		|| \
					type == avpt_longlong	|| \
					type == avpt_bool 		|| \
					type == avpt_int			|| \
					type == avpt_uint)    

void TruncateName( char *pString );

//------------------------------------------------------------------------------------------------//
//																								  //
//										display string helpers									  //
//																								  //
//------------------------------------------------------------------------------------------------//
char *	GetConstantNameDisplayString( HDATA hData, HPROP hProp, int, int );
char *	GetConstantNameDisplayString( HDATA hData, HPROP hProp, AVP_dword dwWorkMode, BOOL i_bForEditing = FALSE );
void	GetConstantNameDisplayString( CTTMessageInfo *info, HDATA hData, HPROP hProp, AVP_dword dwWorkMode );

char *	GetErrorCodeNameDisplayString( HDATA hData, HPROP hProp, int, int );
char *	GetErrorCodeNameDisplayString( HDATA hData, HPROP hProp, BOOL i_bForEditing = FALSE );
void	GetErrorCodeNameDisplayString( CTTMessageInfo *info, HDATA hData, HPROP hProp );

char*	GetMethodNameDisplayString( CMethodInfo& method, BOOL i_bForEditing = FALSE );
char*	GetMethodNameDisplayString( HDATA hData );
void	GetMethodNameDisplayString( CTTMessageInfo *info, HDATA hData );

char*	GetPropertyNameDisplayString( HDATA hData, HPROP hProp, AVP_dword dwWorkMode, BOOL i_bForEditing = FALSE );
void	GetPropertyNameDisplayString( CTTMessageInfo *info, HDATA hData, HPROP hProp, AVP_dword dwWorkMode );

char*	CreateDisplayLabelSTR( DWORD dwPrefixID, LPCTSTR lpszStr );
char*	CreateDisplayLabelSTRExt( DWORD dwPrefixID, LPCTSTR lpszStr );
char*	CreateDisplayLabelINT( DWORD dwPrefixID, int nValue, BOOL i_bHex = TRUE);

char*	CreateLargeCommentLabel( HDATA hData, AVP_dword dwCommentID );

char*	CreatePluginMnemonicIDLabel( HDATA hData );
char*	CreateMnemonicIDLabel( HDATA hData );
char*	CreateObjectTypeLabel( HDATA hData );
char*	CreateIncludeFileLabel( HDATA hData );
char*	CreateOutputFileLabel( HDATA hData );

TCHAR	*StrDuplicate( const TCHAR *pStr );

void	FullyDeleteItem( CControlTree &rcTree, int nItemInd, BOOL bRemoveData = true );
void	DeleteNodesData( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );
bool	FindForNodeID( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );
bool	FindForNodeType( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );
bool	FindForNodeHDATA( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );

int		A2I( const TCHAR *pS );
__int64		A2I64( const TCHAR *pS, TCHAR **ppLast, int nBase );
HDATA	FindParentHData( CCheckTree *pTree, int nIndex );
int		GetConvertBase( const TCHAR *pS );
BOOL	CheckConvertable( const TCHAR *pString );

void	SetParentChildNodeText( CControlTree &rcTree, int nItemInd, int nParentType, DWORD dwSiblingID, char *pText );
int		FindParentChildNodeIndex( CControlTree &rcTree, int nItemInd, int nParentType, DWORD dwSiblingID );

void	AjustDisplayString( CPathStr &pDispStr, int nAjustLen );

char	*GetPureValueString( HPROP hProp );
void	GetPureInfoValueString( CTTMessageInfo *info, HPROP hProp );
void	GetCodePageValueString( CTTMessageInfo *info, HPROP i_hProp ); 
//void	GetCodePageValueDisplayString( CTTMessageInfo *info, DWORD dwPrefixID, HPROP i_hProp ); 

void	GetValueDisplayString( CTTMessageInfo *info, DWORD dwPrefixID, HPROP hProp );
void	GetLargeCommentValueDisplayString( CTTMessageInfo *info, DWORD dwPrefixID, HDATA hData, AVP_dword dwCommentID );


AVP_dword GetTreeNodePropId(CControlTree &rcTree, int nItemInd);
BOOL	IsIDProlog( CControlTree &rcTree, int nItemInd, AVP_dword nID );
BOOL	IsTypeProlog( CControlTree &rcTree, int nItemInd, int nDescrType );
void	SaveUsedObject( CIObject *pIObject, HDATA hRootData );
void	RemoveUsedObject( CIObject *pIObject, HDATA hRootData );

AVP_dword MaxTypeID();

char	*GTypeID2TypeName( DWORD nType );
DWORD	GTypeName2TypeID( const char *pTypeName );
DWORD	PropTypeName2PropTypeID( const char *pTypeName );
char	*PropTypeID2PropTypeName( DWORD nType );
DWORD	PropName2PropID( const char *pPropName, BOOL bHard = false );
char	*PropName2PropTypeName( const char *pPropName, BOOL bHard = false );
DWORD	PropName2PropType( const char *pPropName, BOOL bHard = false );
void	AddGlobalConst(AVP_dword dwType, LPCSTR szName, LPCSTR szValue );

BOOL	CheckEditedValue( CWnd *pOwner, const TCHAR *pText, BOOL bDigital );

HDATA	FindPropDataByName( HDATA hRootData, LPCTSTR pPropName );
BOOL	CheckHardPropExists( HDATA hPropData );

AVP_dword CreateDataId( HDATA hParentData );

BOOL	IsLinkedToFile( HDATA hData );
BOOL	IsParentLinkedToFile( HDATA hData );
BOOL	IsImported( HDATA hData );
BOOL	IsSelected( HDATA hData );

BOOL	IsEllipsis( const char *pName );

BOOL	IsSeparator( HDATA hData );
BOOL	IsSeparator( LPCTSTR pszNodeText );
BOOL	SetSeparator( HDATA hData, LPTSTR pszNodeText );
BOOL	SetSeparator( HDATA hData, CString& strNodeText );

void	AddHeaderGuidProp( HDATA hData );

AVP_dword GetPropDTTypeByPragueType( HDATA hData );

void	CollectConstantNames( HDATA hRootData, CStringList &pContList, DWORD dwPragueType );
void	CollectCodePages(CStringList &rContList);

char	*GetConstantValueAsString( HDATA hRootData, const char *pConstName );
char	*GetConstantNameOnValue(const char *pConstValue );

void	SetItemEditType( HPROP hProp, CControlTreeItem *item );

void	SetNodeUID( HDATA hData, BOOL bDirectSet = FALSE );
BOOL	GetNodeUID( HDATA hData, CNodeUID &pvUID );
BOOL	CompareNodesUID( CNodeUID &pvFUID, CNodeUID &pvSUID );

BOOL	SetDataPropWithCheck( HDATA hData, AVP_dword dwPID, AVP_dword dwValue );
BOOL	SetDataPropWithCheck( HDATA hData, AVP_dword dwPID, LPCTSTR pValue );
BOOL	SetDataPropArrayWithCheck( HPROP hProp, int nPos, LPCTSTR pValue );

void GetSeparatorDisplayString( CPathStr &pDispStr, LPCTSTR pValueStr ); 

ItType	WType2IType( int iwType );
BOOL	IsAlreadyAttached( HDATA hDataToCheck, HDATA hRootData );

HDATA	FindFirstImportedData( HDATA hRootData );

// ---
extern CPArray<TypeID>			gTypes;
extern CPArray<TypeID>			gPropTypes;
extern CPArray<PredefinedProp>	gHardProps;
extern CPArray<PredefinedProp>	gSoftProps;
extern HDATA					ghSMRootData;
extern CVisualEditorDlg*		gpMainDlg; 

enum tLangId { li_all, li_rus, li_eng, li_last };
extern tLangId gLang; 
extern tCHAR*  gLangId[]; 

#define TYPE_IS_POINTER(t) (((t)==tid_PTR) || ((t)==tid_FUNC_PTR) || ((t)==tid_STRING) || ((t)==tid_WSTRING) || ((t)==tid_BINARY) || ((t)==tid_IFACEPTR) || ((t)==tid_OBJECT))

//{{AFX_INSERT_LOCATION}}

#endif // 
