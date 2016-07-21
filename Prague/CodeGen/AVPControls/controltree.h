#if !defined(AFX_CONTROLLIST_H__010F4142_99AB_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_CONTROLLIST_H__010F4142_99AB_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TreeWnd.h"

// ControlList.h : header file
//
/*
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;
*/
// Дескриптор узла листа
// ---
class CControlTreeItem : public CCheckTreeItem {
private :
	uint8			fCanBeRemoved										: 1;		 //	Может быть удален
	union {
		DWORD				m_dwFlags;
		struct {
			uint8     fRadioAlias											: 1;		 // Синоним RadioButton
			uint8     fCheckAlias											: 1;		 // Синоним CheckButton
			uint8     fSimpleEditAlias								: 1;		 // Синоним Edit
			uint8     fUpDownEditAlias								: 1;		 // Синоним UpDownEdit
			uint8     fDropDownComboAlias							: 1;		 // Синоним DropDownComboBox
			uint8     fDropDownListComboAlias					: 1;		 // Синоним DropDownListComboBox
			uint8     fSimpleStringAlias							: 1;		 // Синоним String
			uint8     fExternalEditingAlias						: 1;		 // Синоним ExternalEditing
			uint8     fExclusiveExternalEditingAlias	: 1;		 // Синоним ExclusiveExternalEditing
			uint8     fVScrollUpAlias									: 1;		 // Синоним VScrollUp
			uint8     fVScrollDownAlias								: 1;		 // Синоним VScrollDown
			uint8     fDateAlias											: 1;		 // Синоним Date
			uint8     fTimeAlias											: 1;		 // Синоним Time
			uint8     fDateTimeAlias									: 1;		 // Синоним DateTime
			uint8     fPasswordAlias									: 1;		 // Синоним Password
			uint8     fIPAddressAlias									: 1;		 // Синоним IPAddress
			uint8     fEditableCheckAlias							: 1;		 // Синоним EditableCheckButton
			uint8     fPIDValueAlias									: 1;		 // Синоним PIDValue
		};
	};

	void ClearAliases() { m_dwFlags = 0;	}

public:
    typedef enum {
			ii_RadioUnchecked = CCheckTreeItem::ii_LastIndex,
			ii_RadioChecked,								// 6
			ii_CheckUnchecked,							// 7
			ii_CheckChecked,								// 8
			ii_SimpleEdit,									// 9
			ii_UpDownEdit,									// 10
			ii_DropDownCombo,								// 11
			ii_DropDownListCombo,						// 12
			ii_SimpleStringCollapsed,   		// 13
			ii_SimpleStringExpanded,    		// 14
			ii_ExternalEditingButtonUp,			// 15
			ii_ExternalEditingButtonDown,		// 16
			ii_VScrollUpButtonUp,						// 17
			ii_VScrollUpButtonDown,					// 18
			ii_VScrollDownButtonUp,					// 19
			ii_VScrollDownButtonDown,				// 20
			ii_Date,												// 21
			ii_Time,												// 22
			ii_DateTime,										// 23
			ii_Password,										// 24
			ii_IPAddress,										// 25
			ii_PIDValue,										// 26
			ii_Empty,       								// 27
			ii_LastIndex,										// 28
		} CieImageIndex;

		CControlTreeItem();				   
   ~CControlTreeItem();

		virtual bool operator == ( const CCheckTreeItem &rItem );

		virtual void Assign( const CCheckTreeItem &rItem );

		virtual CCheckTreeItem *Duplicate();

	  void       SetCanBeRemoved( bool set = true ) { fCanBeRemoved = set; }
    bool       IsCanBeRemoved()								    { return !!fCanBeRemoved; } 

    void       SetRadioAlias()               { ClearAliases(); fRadioAlias = true; SetMarkable( false ); SetImageIndex( ii_RadioUnchecked );}
    bool       IsRadioAlias()								 { return !!fRadioAlias; } 

    void       SetCheckAlias()               { ClearAliases(); fCheckAlias = true; SetMarkable( false );  SetImageIndex( ii_CheckUnchecked );}
    bool       IsCheckAlias()								 { return !!fCheckAlias; } 

    void       SetSimpleEditAlias()          { ClearAliases(); fSimpleEditAlias = true; SetMarkable( false ); SetImageIndex( ii_SimpleEdit );}
    bool       IsSimpleEditAlias()					 { return !!fSimpleEditAlias; } 

    void       SetPasswordAlias()						 { ClearAliases(); fPasswordAlias = true; SetMarkable( false );  SetImageIndex( ii_Password );}
    bool       IsPasswordAlias()					   { return !!fPasswordAlias; } 

    void       SetIPAddressAlias()					 { ClearAliases(); fIPAddressAlias = true; SetMarkable( false );  SetImageIndex( ii_IPAddress );}
    bool       IsIPAddressAlias()					   { return !!fIPAddressAlias; } 

    void       SetUpDownEditAlias()          { ClearAliases(); fUpDownEditAlias = true; SetMarkable( false );  SetImageIndex( ii_UpDownEdit );}
    bool       IsUpDownEditAlias()           { return !!fUpDownEditAlias; } 

    void       SetDropDownComboAlias()       { ClearAliases(); fDropDownComboAlias = true; SetMarkable( false );  SetImageIndex( ii_DropDownCombo );}
    bool       IsDropDownComboAlias()        { return !!fDropDownComboAlias; } 

    void       SetDropDownListComboAlias()   { ClearAliases(); fDropDownListComboAlias = true; SetMarkable( false );  SetImageIndex( ii_DropDownListCombo );}
    bool       IsDropDownListComboAlias()    { return !!fDropDownListComboAlias; } 

    void       SetSimpleStringAlias()        { ClearAliases(); fSimpleStringAlias = true; SetMarkable( false );  SetImageIndex( ii_SimpleStringCollapsed );}
    bool       IsSimpleStringAlias()				 { return !!fSimpleStringAlias; } 

    void       SetExternalEditingAlias()     { ClearAliases(); fExternalEditingAlias = true; SetMarkable( false );  SetImageIndex( ii_ExternalEditingButtonUp );}
    bool       IsExternalEditingAlias()			 { return !!fExternalEditingAlias; } 

    void       SetExclusiveExternalEditingAlias()     { ClearAliases(); fExclusiveExternalEditingAlias = true; SetMarkable( false );  SetImageIndex( ii_ExternalEditingButtonUp );}
    bool       IsExclusiveExternalEditingAlias()			 { return !!fExclusiveExternalEditingAlias; } 

    void       SetVScrollUpAlias()           { ClearAliases(); fVScrollUpAlias = true; SetMarkable( false );  SetImageIndex( ii_VScrollUpButtonUp );}
    bool       IsVScrollUpAlias()						 { return !!fVScrollUpAlias; } 

    void       SetVScrollDownAlias()				 { ClearAliases(); fVScrollDownAlias = true; SetMarkable( false );  SetImageIndex( ii_VScrollDownButtonUp );}
    bool       IsVScrollDownAlias()					 { return !!fVScrollDownAlias; } 

    void       SetDateAlias()								 { ClearAliases(); fDateAlias = true; SetMarkable( false );  SetImageIndex( ii_Date );}
    bool       IsDateAlias()					       { return !!fDateAlias; } 

    void       SetTimeAlias()								 { ClearAliases(); fTimeAlias = true; SetMarkable( false );  SetImageIndex( ii_Time );}
    bool       IsTimeAlias()					       { return !!fTimeAlias; } 

    void       SetDateTimeAlias()							{ ClearAliases(); fDateTimeAlias = true; SetMarkable( false );  SetImageIndex( ii_DateTime );}
    bool       IsDateTimeAlias()					    { return !!fDateTimeAlias; } 

    void       SetEditableCheckAlias()        { ClearAliases(); fEditableCheckAlias = true; fSimpleEditAlias = true; fCheckAlias = true; SetMarkable( false );  SetImageIndex( ii_CheckUnchecked );}
    bool       IsEditableCheckAlias()					{ return !!fEditableCheckAlias; } 

    void       SetPIDValueAlias()							{ ClearAliases(); fPIDValueAlias = true; SetMarkable( false );  SetImageIndex( ii_PIDValue );}
    bool       IsPIDValueAlias()							{ return !!fPIDValueAlias; } 

    void       SetVScrollIsDown( bool set )	 { SetImageIndex( set ? IsVScrollUpAlias() ? ii_VScrollUpButtonDown : ii_VScrollDownButtonDown : IsVScrollUpAlias() ? ii_VScrollUpButtonUp : ii_VScrollDownButtonUp);}

    void       SetExternalEditingIsDown( bool set )	 { SetImageIndex( set ? ii_ExternalEditingButtonDown : ii_ExternalEditingButtonUp ); }

		bool       IsCanBeEdited() { return IsSimpleEditAlias() ||
																				IsPasswordAlias() ||
																				IsIPAddressAlias() ||
																				IsPIDValueAlias() ||
																				IsUpDownEditAlias() ||
																				IsDropDownComboAlias() ||
																				IsDropDownListComboAlias() ||
																				IsExternalEditingAlias() ||
																				IsDateAlias() ||
																				IsTimeAlias() ||
																				IsDateTimeAlias(); }


		bool       IsCanBeChecked() { return IsMarkable() || IsRadioAlias() || IsCheckAlias(); }
		
		virtual int GetImageIndex();
};


/////////////////////////////////////////////////////////////////////////////
// CControlTree window

class CControlTree : public CCheckTree {
	CString		m_rcOldItemText;
	int				m_nItemBeingEditInd;
	HTREEITEM m_hDeferredStartEditNode;
	HTREEITEM m_hDeferredStartExtEditNode;
	uint			scrollTimerId;
	uint			scrollFirstTime;
	uint			scrollSecondTime;
	uint8			fPreventSetFocus				 : 1;
	uint8			fShowSelectionAlways		 : 1;
	uint8			fEnableAutoScanForEnable : 1;
	uint8			fEditNodeStartInProgress : 1;
	uint8			fAcceptNotChangedText    : 1;
	// Construction
public:
	CControlTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTree)
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTree();

					void SetEnableAutoScanForEnable( bool set = true ) { fEnableAutoScanForEnable = set; }
					bool IsEnableAutoScanForEnable()								    { return !!fEnableAutoScanForEnable; } 
					
					void SetAcceptNotChangedText( bool set = true ) { fAcceptNotChangedText = set; }
					bool IsAcceptNotChangedText()								    { return !!fAcceptNotChangedText; } 
					
					void  SetShowSelectionAlways( bool bShowAlways );

					void  SetItemEnable( int iItemIndex, bool bEnable = true, bool bScanDown = true );
					void  SetItemStrictlyDisabled( int iItemIndex, bool bDisable = true, bool bScanDown = true );

					BOOL	PreTranslateEditMessage( MSG* pMsg );

					int   GetIndexRadioInGroup( int nParentInd, int nItemInd );

	virtual void	BeginModalState();
	virtual void	EndModalState();

protected:
	virtual void CreateImageList();
					void ScrollAlongControl( CWnd *control, CRect &controlEncloseRect, HTREEITEM node );
	virtual void PostLoadProcessing();
	virtual void CheckNode( HTREEITEM hNode );
					void ScanForEnableItems( int firstIndex, bool mustBeEnabled );
					void ScanForStrictDisableItems( int firstIndex, bool bDisable );
					void CheckAndScanForEnableItems( CControlTreeItem *clItem, int nIndex );
					void CreateAndTrackPopupMenu( CPoint& point );
					bool IsEnabled( CControlTreeItem *clItem ) { return !clItem->IsStrictlyDisabled() && (clItem->IsEnabled() || IsEnableOpOnDisabled()); }
					void InsideImageClickImitate( HTREEITEM hItemOnWhich );
					void CheckRadioSiblings( HTREEITEM hNode );
					void ScrollProcessing( CControlTreeItem *clItem, HTREEITEM hNode, int ind );
					void ProcessScrollSelection( UINT nChar );
					void PerformExternalEditing( HTREEITEM hNode );

	// Generated message map functions
protected:
	//{{AFX_MSG(CControlTree)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void		OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void		OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void		OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void		OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnTimer(UINT nIDEvent);
	afx_msg void		OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnAfterStartEditing( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAfterEndEditing( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAfterChangeState( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnWmUser7424( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetIndent( WPARAM wParam, LPARAM lParam );
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnDestroy();
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void		OnKillFocus(CWnd* pNewWnd);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChildInvalid(WPARAM wParam, LPARAM lParam);
	afx_msg int			OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLIST_H__010F4142_99AB_11D2_96B0_00104B5B66AA__INCLUDED_)
