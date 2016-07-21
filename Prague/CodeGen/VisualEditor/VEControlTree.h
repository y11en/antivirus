#ifndef __VECONTROLTREE_H__
#define __VECONTROLTREE_H__

#include <avpcontrols/controltree.h>

// Дескриптор узла листа
// ---
class CVEControlTreeItem : public CControlTreeItem {
public:
		CVEControlTreeItem();
   ~CVEControlTreeItem();

public:
    typedef enum {
			ii_EnabledImageIndex = CControlTreeItem::ii_LastIndex,
			ii_DisabledImageIndex,					// 29
			ii_LastIndex,										// 30
		} CieImageIndex;

		virtual int GetImageIndex();
};

/////////////////////////////////////////////////////////////////////////////
// CVEControlTreeBase window
// ---

class CVEControlTreeBase : public CControlTree {
public:
	CVEControlTreeBase();
	
	//{{AFX_VIRTUAL(CControlTree)
	//}}AFX_VIRTUAL

	BOOL m_bTechnicalEditingMode;

	void	SetItemsArray( CCTItemsArray *itemsArray );					
	virtual ~CVEControlTreeBase();
protected:
	BOOL	IsCommentItem(CControlTreeItem *i_Item);
	BOOL	IsNewNodeItem(CControlTreeItem *i_Item);
	void	ShowRoot();
	
	//{{AFX_MSG(CControlTree)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CVEControlTree window
// ---
class CVEControlTree : public CVEControlTreeBase {
public:
	CVEControlTree();

	//{{AFX_VIRTUAL(CControlTree)
	//}}AFX_VIRTUAL

	virtual ~CVEControlTree();
protected:
	virtual void CreateImageList();
	//{{AFX_MSG(CControlTree)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __VECONTROLTREE_H__
