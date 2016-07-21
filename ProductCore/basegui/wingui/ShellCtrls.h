#if !defined(AFX_SHELLTREECTRL_H__98BDBB7B_E2C3_4145_A5D5_693274C6B99B__INCLUDED_)
#define AFX_SHELLTREECTRL_H__98BDBB7B_E2C3_4145_A5D5_693274C6B99B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShellTreeCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include <shlobj.h>
#include <string>

/////////////////////////////////////////////////////////////////////////////
// CShellTreeSink window

extern IMalloc* shell_GetMalloc();
extern void     shell_FreeMalloc();

// item flags
#define STCF_DEFAULT		0x00
#define STCF_INCLUDEFILES	0x01
#define STCF_INCLUDEHIDDEN	0x02
#define STCF_INCLUDEALL		(STCF_INCLUDEFILES|STCF_INCLUDEHIDDEN)
#define STCF_INCLUDEMASK	0x0F

/////////////////////////////////////////////////////////////////////////////

class CShellPidl
{
public:
	CShellPidl(LPITEMIDLIST pidl=NULL) { m_pObj = pidl; }
	CShellPidl(const CShellPidl& c)    { Copy(c.m_pObj); }
	CShellPidl(UINT nSpecialFolder, HWND hOwner = NULL)
	{
		SHGetSpecialFolderLocation(hOwner, nSpecialFolder, &m_pObj);
	}
	~CShellPidl()                      { Free(); }

	const CShellPidl& operator = (const CShellPidl& obj) { Free(); Copy(obj.m_pObj); return *this; }
	
	LPITEMIDLIST operator -> ()        { return m_pObj; }
	LPCITEMIDLIST operator -> () const { return m_pObj; }
	operator LPITEMIDLIST ()           { return m_pObj; }
	operator LPCITEMIDLIST* ()         { return (LPCITEMIDLIST*)&m_pObj; }
	LPITEMIDLIST* operator & ()        { return &m_pObj; }

	operator LPCTSTR const ()          { return (LPCTSTR)m_pObj; }

	BOOL IsValid() const               { return m_pObj != NULL; }
	void Attach(LPITEMIDLIST pObj)     { m_pObj = pObj; }
	LPITEMIDLIST Detach()              { LPITEMIDLIST pObj = m_pObj; m_pObj = NULL; return pObj; }

	void Copy(LPCITEMIDLIST pidl)
	{
		m_pObj = (LPITEMIDLIST)shell_GetMalloc()->Alloc(pidl->mkid.cb);
		memcpy(m_pObj, pidl, pidl->mkid.cb);
	}

	void Free() 
	{
		if( m_pObj )
			shell_GetMalloc()->Free(m_pObj);
		m_pObj = NULL;
	}

protected:
	LPITEMIDLIST m_pObj;
};

typedef CComPtr<IShellFolder> SShellFolderPtr;
typedef CComPtr<IEnumIDList>  SEnumIDListPtr;

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
	bool SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessSel = NULL);
	bool GetSelObject(CObjectInfo& pObject);

private:
	struct TVITEMDATA : public cTreeItem
	{
		TVITEMDATA(TVITEMDATA* _pParent, IShellFolder* folder, LPITEMIDLIST _pidl, UINT _nFlags) :
			pParent(_pParent), pFolder(folder), pidl(_pidl), nFlags(_nFlags), dwAttributes(-1), nCustomId(0), pPersonal(0){}

		TVITEMDATA(TVITEMDATA* _pParent, DWORD _nCustomId) :
			pParent(_pParent), nCustomId(_nCustomId), pPersonal(0){}

		BOOL Init();
		BOOL IsValid() { return (this != NULL) && pidl.IsValid(); }

		TVITEMDATA *    pParent;
		CShellPidl      pidl;
		SShellFolderPtr pFolder;
		DWORD           nCustomId;
		DWORD           pPersonal;
		UINT            nFlags;
		DWORD           dwAttributes;
	};

protected:
	bool OnItemExpanding(cTreeItem * pItem, tUINT& nChildren);	// before expanding
	void OnUpdateItemProps(cTreeItem * pItem, tDWORD flags);

private:
	struct _success_path_int
	{
		cTreeItem * m_pLastSuccess;
		tDWORD      m_nSrcSuccSize;
	};
	
	bool        IsMyComputer(TVITEMDATA *pTreeItem);
	bool        IsPersonalFolder(TVITEMDATA * pTreeItem, TVITEMDATA ** pRootPersonalItem = NULL);
	CShellPidl  GetFullPIDL(TVITEMDATA* pData);
	tUINT       GetImageIndex(int nId, HICON hIcon);
	cTreeItem * GotoPIDL(cTreeItem * pParent, LPITEMIDLIST pidl, bool bSelect = true);
	bool        GotoDisplayName(cTreeItem * pParent, tObjPath& strName, tDWORD nNamePos = 0, _success_path_int * pLastSuccess = NULL);
	cTreeItem * GotoCustom(cTreeItem * pParent, tDWORD nCustomId);
	void        InitItemImage(TVITEMDATA * pData, bool bSelected);

	bool       GetObjectInfo(CObjectInfo& pObject, TVITEMDATA* pItem);
	void       GetItemInfo(TVITEM& item);

	static int __cdecl SortCompare(const void *elem1, const void *elem2);

private:
	std::vector<int>  m_vecImages;
	SShellFolderPtr   m_pDesktopFolder;
	HIMAGELIST        m_hSysImageList;
	HIMAGELIST        m_hImageList;
};

/////////////////////////////////////////////////////////////////////////////
// CRegTreeSink

class CRegTreeSink : public CShellTreeSinkBase
{
public:
	void OnInited();
	bool SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessSel = NULL);
	bool GetSelObject(CObjectInfo& pObject);

private:
	struct TVITEMDATA : public cTreeItem
	{
		TVITEMDATA(TVITEMDATA* _pParent, DWORD _nType) :
			pParent(_pParent), nType(_nType), pRootKey(NULL), nSubKeys(0), nValues(0){}

		TVITEMDATA *    pParent;
		HKEY            pRootKey;
		DWORD           nType;
		DWORD           nSubKeys;
		DWORD           nValues;
		DWORD           nMaxLen;
	};

	bool OnItemExpanding(cTreeItem * pItem, tUINT& nChildren);	// before expanding
	int  OnSortCompare(cTreeItem * pItem1, cTreeItem * pItem2);

	void AddKey(TVITEMDATA * pParent, HKEY pKey, LPCSTR strKey);
	void AddValue(TVITEMDATA * pParent, LPCSTR strName, DWORD nValType);

	bool GetFullName(TVITEMDATA * pData, bool bWithRoot, tObjPath& strName);
	bool GotoKey(cTreeItem * pParent, LPCSTR strKeyPath);
	
	void ShortRegRoot2LongRegRoot(tString &strKey);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLTREECTRL_H__98BDBB7B_E2C3_4145_A5D5_693274C6B99B__INCLUDED_)
