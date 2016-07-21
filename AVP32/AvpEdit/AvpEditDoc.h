// AvpEditDoc.h : interface of the CAvpEditDoc class
//
/////////////////////////////////////////////////////////////////////////////
#include <Bases/Format/Base.h>
#include "REdit.h"

class CAvpEditDoc : public CDocument
{
protected: // create from serialization only
	BOOL InitHeader();
	void ReadOldBase(CArchive& ar);
	CAvpEditDoc();
	DECLARE_DYNCREATE(CAvpEditDoc)


// Attributes
public:
	const char* MakeFullName(int i);
	char* ColumnName[10];
	int ColumnCx[10];
	int ColumnCount;
	BOOL LoadSuccess;
	int Compression;
	int ClipboardPaste(int i);
	int ClipboardCopy(int i);
	void ClipboardFree();
	int ClipboardCount();
	void InsertRecord(int i,CRecordEdit* re);
	int EditRecord(int i);
	int DeleteRecord(int i);
//	void InitialUpdateView(CView* view);
	CPtrArray EditArray;
	AVP_BaseHeader BaseHeader;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvpEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAvpEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAvpEditDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#define UV_ADD		0x100
#define UV_INSERT	0x200
#define UV_CHANGE	0x300
#define UV_DELETE	0x400

class UVStruct : public CObject 
{
public:
	CWnd* Sender;
	int Index;
	CRecordEdit* REptr;
};
