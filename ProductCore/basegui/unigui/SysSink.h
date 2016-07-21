// SysSink.h
//

#if !defined(AFX_SYSSINK_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
#define AFX_SYSSINK_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CShellTreeSink

class CShellTreeSink : public CShellTreeSinkBase
{
public:
	typedef CShellTreeSinkBase TBase;

	CShellTreeSink();
	~CShellTreeSink();

public:
	void OnInited();
	bool SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessPath = NULL);
	bool GetSelObject(CObjectInfo& pObject);

private:
	struct TVITEMDATA : public cTreeItem
	{
		TVITEMDATA(TVITEMDATA * _pParent, const QModelIndex& _idx) :
			pParent(_pParent), nCustomId(0), m_idx(_idx) {}

		TVITEMDATA(TVITEMDATA * _pParent, DWORD _nCustomId = 0) :
			pParent(_pParent), nCustomId(_nCustomId) {}

		TVITEMDATA *    pParent;
		DWORD           nCustomId;
		QModelIndex     m_idx;
	};

protected:
	bool OnItemExpanding(cTreeItem * pItem, tUINT& nChildren);	// before expanding
	void OnUpdateItemProps(cTreeItem * pItem, tDWORD flags);

private:
	bool  GetObjectInfo(CObjectInfo& pObject, TVITEMDATA * pItem);
	tUINT _AddImage(QIcon * qIcon, bool bSelected = false);

	QDirModel  m_dirs;
	CImageList m_images;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SYSSINK_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
