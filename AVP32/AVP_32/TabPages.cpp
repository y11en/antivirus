// TabPages.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include <Stuff/layout.h>
//#include "TabPages.h"
#include "Avp32Dlg.h"
#include "../fn.h"

#include <winnetwk.h>
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CStringArray Mess;

/////////////////////////////////////////////////////////////////////////////
// CTabPage1 dialog


CTabPage1::CTabPage1(CWnd* pParent )
	: CDialog(CTabPage1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabPage1)
	m_cFloppy = FALSE;
	m_cLocal = FALSE;
	m_cNetwork = FALSE;
	//}}AFX_DATA_INIT
	
//	count=0;
	m_Disabled=FALSE;
	m_ShellImageList=0;
	 
	for(int c=0;c<MAX_TYPES;c++){
		c_max[c]=0;
		c_on[c]=0;
	}

}

CTabPage1::~CTabPage1()
{
	;	
}

void CTabPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPage1)
	DDX_Control(pDX, IDC_LIST_DISK, m_lDisk);
	DDX_Check(pDX, IDC_FLOPPY, m_cFloppy);
	DDX_Check(pDX, IDC_LOCAL, m_cLocal);
	DDX_Check(pDX, IDC_NETWORK, m_cNetwork);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTabPage1, CDialog)
	//{{AFX_MSG_MAP(CTabPage1)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_FLOPPY, OnCheckFloppy)
	ON_BN_CLICKED(IDC_LOCAL, OnCheckLocal)
	ON_BN_CLICKED(IDC_NETWORK, OnCheckNetwork)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISK, OnItemchangedListDisk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DISK, OnKeydownListDisk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DISK, OnDblclkListDisk)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DISK, OnRclickListDisk)
	ON_WM_SETFOCUS()
	ON_NOTIFY(NM_CLICK, IDC_LIST_DISK, OnClickListDisk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabPage1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SHFILEINFO sfi;

	CoInitialize(NULL);

	if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS
	||(os.dwPlatformId==VER_PLATFORM_WIN32_NT
	   && os.dwMajorVersion>=4)
	){
		m_ShellImageList = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\", 
                                           0,
                                           &sfi, 
                                           sizeof(SHFILEINFO), 
                                           SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	}
	
    if (m_ShellImageList){
		CImageList il;
		il.Attach(m_ShellImageList);
		ImageList.Create(il,0,il,0,0,0);   //bMask
		il.Detach();
	}else
	{
		ImageList.Create(16,16,TRUE, 13, 4);   //bMask
		for(int idi=IDI_ICON1;idi<=IDI_ICON12;idi++)
			ImageList.Add(AfxGetApp()->LoadIcon(idi));
	}

	ImageList.SetOverlayImage(ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICONOVERLAY)),1);

	m_lDisk.SetImageList(&ImageList,LVSIL_SMALL);
	m_lDisk.SetItemCount(32);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTabPage1::Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd)
{
	Tab=tab;
	Parent=pParentWnd;
	return CDialog::Create(nIDTemplate, (CWnd*)pParentWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CTabPage1 message handlers

void CTabPage1::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	Tab->SetCurSel(0);
	long r;
	Parent->OnSelchangeTab1(NULL,&r);


}	


void CTabPage1::OnButtonBrowse() 
{
	CString s;
	BROWSEINFO bi;
	LPITEMIDLIST pitem;

	bi.hwndOwner=m_hWnd;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=s.GetBuffer(MAX_PATH);
	bi.lpszTitle=(const char*)MS( IDS_ADD_FOLDER );
	bi.ulFlags=BIF_RETURNONLYFSDIRS;
	bi.lpfn=NULL;
	bi.lParam=0;


	if(!CoInitialize(NULL)) return;
	pitem=SHBrowseForFolder(&bi);
	s.ReleaseBuffer();

	if(pitem){
		SHGetPathFromIDList(pitem,s.GetBuffer(MAX_PATH));
		s.ReleaseBuffer();
		if(-1!=GetFileAttributes(s))
			m_lDisk.EnsureVisible( InsertPathString(s, NULL ,0,TRUE,TRUE) ,FALSE);
		
		LPMALLOC pMalloc;
		if(NOERROR==SHGetMalloc(&pMalloc))
			pMalloc->Free(pitem);
	}

}



int CTabPage1::CheckDisks(int type,int status)
{
	int ret=0;
	int i=-1;
	while(-1 != (i = m_lDisk.GetNextItem(i, 0)))
	{
		if((m_lDisk.GetItemData(i)) == (DWORD)type){
			UINT state = status?INDEXTOOVERLAYMASK(1):0;
			m_lDisk.SetItemState(i,state ,LVIS_OVERLAY);
		}
	}
	m_lDisk.UpdateWindow();
	return ret;
}
void CTabPage1::OnCheck_( int *var,int type )
{
	AddDisks();
	UpdateData();
	switch(*var){
	case 1:
		CheckDisks(type,1);
		break;
	case 2: *var=0;
	case 0:
		CheckDisks(type,0);
		break;
	}
	UpdateData(FALSE);
}

void CTabPage1::OnCheckLocal() 
{
	OnCheck_(&m_cLocal,DRIVE_FIXED);
}
void CTabPage1::OnCheckNetwork() 
{
	OnCheck_(&m_cNetwork,DRIVE_REMOTE);
}
void CTabPage1::OnCheckFloppy() 
{
	OnCheck_(&m_cFloppy,DRIVE_REMOVABLE);
}


void CTabPage1::OnItemchangedListDisk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	int i = pNMListView->lParam;
	
	if(((pNMListView->uNewState) & LVIS_OVERLAY) 
	== ((pNMListView->uOldState) & LVIS_OVERLAY)
	|| i<2)
		return;
	
	if(pNMListView->uNewState & LVIS_OVERLAY)c_on[i]++;
	else c_on[i]--;
		
	int ret=2;
	if(!c_on[i])ret=0;
	else if(c_on[i]==c_max[i])ret=1;
	
	int* val;
	switch(i){
	case DRIVE_FIXED:
		val=&m_cLocal;break;
	case DRIVE_REMOTE:
		val=&m_cNetwork;break;
	case DRIVE_REMOVABLE:
		val=&m_cFloppy;break;
	default:
		return;
	}

	if((*val)!=ret){
		*val=ret;
		UpdateData(FALSE);
	}
}


void CTabPage1::OnDblclkListDisk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(m_Disabled)return;

	UINT state=m_lDisk.GetItemState(m_lDisk.GetNextItem(-1,LVNI_FOCUSED),LVIS_OVERLAY);
	if(!Parent->cust.m_CustSingleClick)
		state = state?0:INDEXTOOVERLAYMASK(1);

	int i=-1;
	while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_SELECTED)))
		m_lDisk.SetItemState(i,state ,LVIS_OVERLAY);
}

void CTabPage1::OnRclickListDisk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(m_Disabled)return;

    CPoint curPoint,cp;
    GetCursorPos(&curPoint);
	cp=curPoint;
    m_lDisk.ScreenToClient(&cp);
	UINT nFlags;
    int m_ItemSel = m_lDisk.HitTest(cp, &nFlags);
	if(!(nFlags&LVHT_NOWHERE))
	Parent->popMenu.GetSubMenu(0)->TrackPopupMenu(0,curPoint.x,curPoint.y,this);
}

void CTabPage1::OnKeydownListDisk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(m_Disabled)return;

	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	switch(pLVKeyDow->wVKey){
	case VK_SPACE:	
		{
		int j=-1;
		j=m_lDisk.GetNextItem(j,LVNI_FOCUSED);
		UINT state=m_lDisk.GetItemState(j,LVIS_OVERLAY);
		state = state?0:INDEXTOOVERLAYMASK(1);
		int i=-1;
		i=m_lDisk.GetNextItem(i,LVNI_SELECTED);
		if(i==-1)
			m_lDisk.SetItemState(j,state ,LVIS_OVERLAY);
		else do
			{
				m_lDisk.SetItemState(i,state ,LVIS_OVERLAY);
			}while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_SELECTED)));
		}
		break;
	case VK_INSERT:	OnButtonBrowse();	break;
	case VK_DELETE:
		int i=-1;
		while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_SELECTED))){
			DWORD lParam=m_lDisk.GetItemData(i);
			if(lParam<2){
				int j=m_lDisk.GetNextItem(i,LVNI_ALL);
				m_lDisk.SetItemState(j,LVIS_FOCUSED,LVIS_FOCUSED);
				m_lDisk.DeleteItem(i);
			}
		}

		break;
	}
}



int CTabPage1::InsertPathString(const char*  vn, const char*  descr, int type, BOOL check, BOOL focus )
{
	LV_ITEM			lvitem;
	int count = m_lDisk.GetItemCount();
	CString Label=vn;
	if(descr){
		Label+="   ";
		Label+=descr;
	}
	CString Disk=vn;
	Disk=Disk.Left(2)+"\\";
	
	if(	!(Parent->functionality & FN_NETWORK)){
		if(type == DRIVE_REMOTE)return count;
		if(GetDriveType(Disk) == DRIVE_REMOTE)	return count;
	}

	lvitem.iItem = count;
	lvitem.pszText = (char*)(const char*)Label;
	lvitem.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE | LVIF_PARAM ;
	lvitem.iSubItem = 0;
	lvitem.state = 0;
	lvitem.lParam = type;			
	if(check){
		lvitem.mask |= LVIS_OVERLAY;
		lvitem.state |= INDEXTOOVERLAYMASK(1);
	}
	if(focus || !count){
		lvitem.state |= LVIS_FOCUSED;
	}

	HICON  hGen32Icon;
	HIMAGELIST hSysImageList;
	SHFILEINFO  shfi;
	CString s=vn;
	s+="\\";

	if(m_ShellImageList
	&&	(hSysImageList = (HIMAGELIST)SHGetFileInfo((LPCSTR)s,
									   0,
									   &shfi,
									   sizeof (SHFILEINFO),
									   SHGFI_SYSICONINDEX | SHGFI_SMALLICON)))
	{
		hGen32Icon = ImageList_GetIcon (hSysImageList, shfi.iIcon, ILD_NORMAL);
		lvitem.iImage=ImageList.Add(hGen32Icon);
		DestroyIcon(hGen32Icon);
	}else{
		if(type == DRIVE_FIXED)		lvitem.iImage = 10;//HD
		if(type == DRIVE_REMOTE)	lvitem.iImage = 1; //ND
		if(type == DRIVE_REMOVABLE)	lvitem.iImage = 8; //FD
		if(type == DRIVE_CDROM)		lvitem.iImage = 2; //CD
		if(type == DRIVE_RAMDISK)	lvitem.iImage = 11; //CD

		if(type == 0) lvitem.iImage = 5; //Dir
		if(type == 1) lvitem.iImage = 0; //File
	}

	count = m_lDisk.InsertItem(&lvitem);
	c_max[type]++;
	return count;
}

void CTabPage1::GetPaths(CStringArray& sap)
{
	sap.RemoveAll();
	int i=-1;
	while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_ALL))){
		CString s=m_lDisk.GetItemText(i,0);
		if(m_lDisk.GetItemData(i)>1){
			if( m_lDisk.GetItemState(i,LVIS_OVERLAY))	
				sap.Add(s.Left(2));
		}
		else{
			if( m_lDisk.GetItemState(i,LVIS_OVERLAY))
				sap.Add(s);
			else					
				sap.Add(_T("*")+s);
		}
	}
}


void CTabPage1::SetPaths(CStringArray& sap)
{
	if(!m_lDisk) return;

	int i=-1;
	while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_ALL)))
		m_lDisk.SetItemState(i,0 ,LVIS_OVERLAY);

	
	for(int j=0;j<sap.GetSize();j++){
		CString s=sap[j];
		BOOL chk;
		chk=(s[0]=='*')?TRUE:FALSE;
		s=LPCTSTR(sap[j])+chk;

		s.MakeUpper();
		int i=-1;
		while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_ALL))){
			CString si=m_lDisk.GetItemText(i,0);
			si.MakeUpper();
			if(m_lDisk.GetItemData(i)>1)si=si.Left(2);
			if(si==s){
				if(!chk)
					m_lDisk.SetItemState(i,INDEXTOOVERLAYMASK(1) ,LVIS_OVERLAY);
				goto cont;
			}
		}
		
		s=LPCTSTR(sap[j])+chk;

		if(-1!=GetFileAttributes(s))
			InsertPathString(s, NULL , 0, !chk, FALSE);
cont:	;
	}

}

/////////////////////////////////////////////////////////////////////////////
// CTabPage2 dialog


CTabPage2::CTabPage2(CWnd* pParent )
	: CDialog(CTabPage2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabPage2)
	m_Mask = 0;
	m_Sectors = TRUE;
	m_Files = TRUE;
	m_Packed = FALSE;
	m_Memory = FALSE;
	m_Archives = FALSE;
	m_UserMask = _T("");
	m_MailPlain = FALSE;
	m_MailBases = FALSE;
	m_Exclude = FALSE;
	m_EditExclude = _T("");
	//}}AFX_DATA_INIT

}


void CTabPage2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPage2)
	DDX_Radio(pDX, IDC_SMART, m_Mask);
	DDX_Check(pDX, IDC_SECTORS, m_Sectors);
	DDX_Check(pDX, IDC_FILES, m_Files);
	DDX_Check(pDX, IDC_PACKED, m_Packed);
	DDX_Check(pDX, IDC_MEMORY, m_Memory);
	DDX_Check(pDX, IDC_EXTRACT, m_Archives);
	DDX_Text(pDX, IDC_EDIT_EXT, m_UserMask);
	DDX_Check(pDX, IDC_MAILPLAIN, m_MailPlain);
	DDX_Check(pDX, IDC_MAILBASES, m_MailBases);
	DDX_Check(pDX, IDC_EXCLUDE, m_Exclude);
	DDX_Text(pDX, IDC_EDIT_EXCLUDE, m_EditExclude);
	//}}AFX_DATA_MAP

	CWnd* wndp;
	BOOL re=FALSE;
	if(wndp=GetDlgItem(IDC_USERDEF))	re=wndp->IsWindowEnabled();
	if(wndp=GetDlgItem(IDC_EDIT_EXT)) wndp->EnableWindow(m_Mask==3 && re);
	if(wndp=GetDlgItem(IDC_EXCLUDE))	re=wndp->IsWindowEnabled();
	if(wndp=GetDlgItem(IDC_EDIT_EXCLUDE)) wndp->EnableWindow(m_Exclude && re);
	UpdateWindow();

}


BEGIN_MESSAGE_MAP(CTabPage2, CDialog)
	//{{AFX_MSG_MAP(CTabPage2)
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_USERDEF, OnChanged)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ALLFILES, OnChanged)
	ON_BN_CLICKED(IDC_PROGRAMS, OnChanged)
	ON_BN_CLICKED(IDC_SMART, OnChanged)
	ON_BN_CLICKED(IDC_EXCLUDE, OnExclude)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabPage2::Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd)
{
	Tab=tab;
	Parent=pParentWnd;
	return CDialog::Create(nIDTemplate, (CWnd*)pParentWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CTabPage2 message handlers

void CTabPage2::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	Tab->SetCurSel(1);
	long r;
	Parent->OnSelchangeTab1(NULL,&r);


}	

/////////////////////////////////////////////////////////////////////////////
// CTabPage3 dialog


CTabPage3::CTabPage3(CWnd* pParent )
	: CDialog(CTabPage3::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabPage3)
	m_Warnings = TRUE;
	m_ShowOK = FALSE;
	m_CA = TRUE;
	m_Limit = FALSE;
	m_Append = FALSE;
	m_Redundant = FALSE;
	m_Report = TRUE;
	m_Reportfile = _T("Report.txt");
	m_Size = 500;
	m_ShowPack = FALSE;
	m_Priority = 2;
	//}}AFX_DATA_INIT
}


void CTabPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPage3)
	DDX_Check(pDX, IDC_WARNINGS, m_Warnings);
	DDX_Check(pDX, IDC_SHOWOK, m_ShowOK);
	DDX_Check(pDX, IDC_CA, m_CA);
	DDX_Check(pDX, IDC_LIMIT, m_Limit);
	DDX_Check(pDX, IDC_APPEND, m_Append);
	DDX_Check(pDX, IDC_REDUNDANT, m_Redundant);
	DDX_Check(pDX, IDC_REPORT, m_Report);
	DDX_Text(pDX, IDC_REPORTFILE, m_Reportfile);
	DDX_Text(pDX, IDC_LIMITSIZE, m_Size);
	DDX_Check(pDX, IDC_SHOWPACK, m_ShowPack);
	//}}AFX_DATA_MAP

	
	CWnd* wndp;
	BOOL re=FALSE;
	if(wndp=GetDlgItem(IDC_REPORT))	re=wndp->IsWindowEnabled();
	if(wndp=GetDlgItem(IDC_REPORTFILE))wndp->EnableWindow(m_Report && re);
	if(wndp=GetDlgItem(IDC_LIMITSIZE))wndp->EnableWindow(m_Report && m_Limit && re);
	if(wndp=GetDlgItem(IDC_LIMIT))wndp->EnableWindow(m_Report && re);
	if(wndp=GetDlgItem(IDC_APPEND))wndp->EnableWindow(m_Report && re);
	UpdateWindow();
}


BEGIN_MESSAGE_MAP(CTabPage3, CDialog)
	//{{AFX_MSG_MAP(CTabPage3)
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_CA, OnChanged)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_WARNINGS, OnChanged)
	ON_BN_CLICKED(IDC_SHOWOK, OnChanged)
	ON_BN_CLICKED(IDC_SOUND, OnChanged)
	ON_BN_CLICKED(IDC_LIMIT, OnChanged)
	ON_BN_CLICKED(IDC_REPORT, OnChanged)
	ON_BN_CLICKED(IDC_SHOWPACK, OnChanged)
	ON_BN_CLICKED(IDC_BUTTON_CUST, OnButtonCust)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabPage3::Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd)
{
	Tab=tab;
	Parent=pParentWnd;
	return CDialog::Create(nIDTemplate, (CWnd*)pParentWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CTabPage3 message handlers
void CTabPage3::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	Tab->SetCurSel(3);
	long r;
	Parent->OnSelchangeTab1(NULL,&r);


}
	
/////////////////////////////////////////////////////////////////////////////
// CTabPage5 dialog

void CTabPage5::Flush()
{
	m_Archives = 0;
	m_Bodies = 0;
	m_Directories = 0;
	m_Disinfected = 0;
	m_Deleted = 0;
	m_Files = 0;
	m_Packed = 0;
	m_Sectors = 0;
	m_Suspicious = 0;
	m_Virus = 0;
	m_Warnings = 0;
	m_Corrupted = 0;
	m_Errors = 0;
	m_Time = _T("");
	m_Speed = 0;
	m_CountSize = 0;
	m_RealCountSize = 0;
}

CTabPage5::CTabPage5(CWnd* pParent )
	: CDialog(CTabPage5::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabPage5)
	m_Archives = 0;
	m_Bodies = 0;
	m_Directories = 0;
	m_Disinfected = 0;
	m_Deleted = 0;
	m_Files = 0;
	m_Packed = 0;
	m_Sectors = 0;
	m_Suspicious = 0;
	m_Virus = 0;
	m_Warnings = 0;
	m_Corrupted = 0;
	m_Errors = 0;
	m_Time = _T("");
	m_Speed = 0;
	//}}AFX_DATA_INIT
	m_CountSize = 0;
	m_RealCountSize = 0;

}


void CTabPage5::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if(pDX->m_bSaveAndValidate)return;

	//{{AFX_DATA_MAP(CTabPage5)
	DDX_Text(pDX, IDC_S_ARCHIVES, m_Archives);
	DDX_Text(pDX, IDC_S_BODIES, m_Bodies);
	DDX_Text(pDX, IDC_S_DIRECTORIES, m_Directories);
	DDX_Text(pDX, IDC_S_DISINFECTED, m_Disinfected);
	DDX_Text(pDX, IDC_S_DELETED, m_Deleted);
	DDX_Text(pDX, IDC_S_FILES, m_Files);
	DDX_Text(pDX, IDC_S_PACKED, m_Packed);
	DDX_Text(pDX, IDC_S_SECRORS, m_Sectors);
	DDX_Text(pDX, IDC_S_SUSPICIOUS, m_Suspicious);
	DDX_Text(pDX, IDC_S_VIRUS, m_Virus);
	DDX_Text(pDX, IDC_S_WARNING, m_Warnings);
	DDX_Text(pDX, IDC_S_CORRUPTED, m_Corrupted);
	DDX_Text(pDX, IDC_S_ERRORS, m_Errors);
	DDX_Text(pDX, IDC_TIME, m_Time);
	DDX_Text(pDX, IDC_S_SPEED, m_Speed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabPage5, CDialog)
	//{{AFX_MSG_MAP(CTabPage5)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabPage5::Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd)
{
	Tab=tab;
	Parent=pParentWnd;
	return CDialog::Create(nIDTemplate, (CWnd*)pParentWnd);
}


/////////////////////////////////////////////////////////////////////////////
// CTabPage5 message handlers

BOOL CTabPage5::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	CRect cr(20,20,300,40);
	//m_Progress.Create(WS_VISIBLE,cr,this,IDC_PROGRESS);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabPage5::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	Tab->SetCurSel(4);
	long r;
	Parent->OnSelchangeTab1(NULL,&r);


}
	
/////////////////////////////////////////////////////////////////////////////
// CTabPage6 dialog


CTabPage6::CTabPage6(CWnd* pParent )
	: CDialog(CTabPage6::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabPage6)
	m_CopyInfected = FALSE;
	m_CopySuspicion = FALSE;
	m_sInfectFolder = _T("");
	m_sSuspFolder = _T("");
	m_Action = 1;
	//}}AFX_DATA_INIT

}


void CTabPage6::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPage6)
	DDX_Check(pDX, IDC_COPYINFECTED, m_CopyInfected);
	DDX_Check(pDX, IDC_COPYSUSPICIOUS, m_CopySuspicion);
	DDX_Text(pDX, IDC_INFECTFOLDER, m_sInfectFolder);
	DDX_Text(pDX, IDC_SUSPFOLDER, m_sSuspFolder);
	DDX_Radio(pDX, IDC_REPORTONLY, m_Action);
	//}}AFX_DATA_MAP

	CWnd* wndp;
	BOOL re=FALSE;
	if(wndp=GetDlgItem(IDC_COPYINFECTED)) re=wndp->IsWindowEnabled();
	if(wndp=GetDlgItem(IDC_INFECTFOLDER)) wndp->EnableWindow(m_CopyInfected && re);
	if(wndp=GetDlgItem(IDC_COPYSUSPICIOUS)) re=wndp->IsWindowEnabled();
	if(wndp=GetDlgItem(IDC_SUSPFOLDER)) wndp->EnableWindow(m_CopySuspicion && re);
	UpdateWindow();


}


BEGIN_MESSAGE_MAP(CTabPage6, CDialog)
	//{{AFX_MSG_MAP(CTabPage6)
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_COPYINFECTED, OnChanged)
	ON_BN_CLICKED(IDC_COPYSUSPICIOUS, OnChanged)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabPage6::Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd)
{
	Tab=tab;
	Parent=pParentWnd;
	return CDialog::Create(nIDTemplate, (CWnd*)pParentWnd);
}


/////////////////////////////////////////////////////////////////////////////
// CTabPage6 message handlers

void CTabPage6::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	Tab->SetCurSel(2);
	long r;
	Parent->OnSelchangeTab1(NULL,&r);

}	



BOOL CTabPage1::Enable(BOOL enable)
{
	return m_Disabled=!enable;
}


void CTabPage3::OnChanged() 
{
	UpdateData();
}

void CTabPage6::OnChanged() 
{
	UpdateData();
}

void CTabPage2::OnChanged() 
{
	UpdateData();
}





void CTabPage1::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------

	  {  lMOVE,    lRIGHT	(IDC_LOCAL),	lRIGHT    (lPARENT)       ,-5  },
      {  lMOVE,    lTOP		(IDC_LOCAL),	lTOP      (lPARENT)       ,+5  },
      {  lSTRETCH, lWIDTH	(IDC_LOCAL),	lWIDTHOF  (lPARENT,30)    ,+0  },

      {  lMOVE,    lRIGHT	(IDC_NETWORK),	lRIGHT  (IDC_LOCAL),+0  },
      {  lMOVE,    lTOP		(IDC_NETWORK),	lBOTTOM (IDC_LOCAL),+2  },
      {  lSTRETCH, lWIDTH	(IDC_NETWORK),	lWIDTH  (IDC_LOCAL),+0  },

      {  lMOVE,    lRIGHT	(IDC_FLOPPY),	lRIGHT  (IDC_LOCAL),  +0  },
      {  lMOVE,    lTOP		(IDC_FLOPPY),	lBOTTOM (IDC_NETWORK),+2  },
      {  lSTRETCH, lWIDTH	(IDC_FLOPPY),	lWIDTH  (IDC_LOCAL),  +0  },

	  {  lMOVE,    lLEFT	(IDC_BUTTON_BROWSE),	lLEFT	(IDC_LOCAL)	,+0  },
      {  lMOVE,    lBOTTOM	(IDC_BUTTON_BROWSE),	lBOTTOM	(lPARENT)	,-5  },
      {  lSTRETCH, lRIGHT	(IDC_BUTTON_BROWSE),	lRIGHT	(lPARENT)	,-5  },
      
	  {  lMOVE,    lLEFT	(IDC_LIST_DISK),	lLEFT	(lPARENT)	,+5  },
      {  lMOVE,    lTOP		(IDC_LIST_DISK),	lTOP	(IDC_LOCAL)	,+0  },
      {  lSTRETCH, lRIGHT	(IDC_LIST_DISK),	lLEFT	(IDC_LOCAL)	,-5  },
      {  lSTRETCH, lBOTTOM	(IDC_LIST_DISK),	lBOTTOM	(lPARENT)	,-5  },


      {  lEND                                                          }
   };
   
	Layout_ComputeLayout(m_hWnd, rules);
}


void CTabPage2::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------

	  {  lMOVE,    lRIGHT	(IDC_MASK_FRAME),	lRIGHT    (lPARENT)       ,-5  },
      {  lMOVE,    lTOP		(IDC_MASK_FRAME),	lTOP      (lPARENT)       ,+2  },
      {  lSTRETCH, lLEFT	(IDC_MASK_FRAME),	lWIDTHOF  (lPARENT,50)    ,+2  },
      {  lSTRETCH, lBOTTOM	(IDC_MASK_FRAME),	lBOTTOM   (lPARENT)       ,-5  },

      {  lMOVE,    lLEFT	(IDC_SMART),	lLEFT   (IDC_MASK_FRAME),+8  },
      {  lMOVE,    lTOP		(IDC_SMART),	lTOP	(IDC_MASK_FRAME),+12  },
      {  lSTRETCH, lRIGHT	(IDC_SMART),	lRIGHT  (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_PROGRAMS),	lLEFT   (IDC_SMART),+0  },
      {  lMOVE,    lTOP		(IDC_PROGRAMS),	lTOP	(IDC_SMART),+12  },
      {  lSTRETCH, lRIGHT	(IDC_PROGRAMS),	lRIGHT  (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_ALLFILES),	lLEFT   (IDC_PROGRAMS),+0  },
      {  lMOVE,    lTOP		(IDC_ALLFILES),	lTOP	(IDC_PROGRAMS),+12  },
      {  lSTRETCH, lRIGHT	(IDC_ALLFILES),	lRIGHT  (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_USERDEF),	lLEFT   (IDC_ALLFILES),+0  },
      {  lMOVE,    lTOP		(IDC_USERDEF),	lTOP	(IDC_ALLFILES),+12  },
      {  lSTRETCH, lRIGHT	(IDC_USERDEF),	lRIGHT  (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_EDIT_EXT),	lLEFT   (IDC_MASK_FRAME),+15  },
      {  lMOVE,    lTOP		(IDC_EDIT_EXT),	lTOP	(IDC_USERDEF),+12  },
      {  lSTRETCH, lRIGHT	(IDC_EDIT_EXT),	lRIGHT  (IDC_MASK_FRAME),-10 },

      {  lMOVE,    lLEFT	(IDC_EXCLUDE),	lLEFT   (IDC_USERDEF), +0  },
      {  lMOVE,    lTOP		(IDC_EXCLUDE),	lBOTTOM	(IDC_EDIT_EXT),+4  },
      {  lSTRETCH, lRIGHT	(IDC_EXCLUDE),	lRIGHT  (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_EDIT_EXCLUDE),	lLEFT   (IDC_EDIT_EXT), +0  },
      {  lMOVE,    lTOP		(IDC_EDIT_EXCLUDE),	lTOP	(IDC_EXCLUDE),+12  },
      {  lSTRETCH, lRIGHT	(IDC_EDIT_EXCLUDE),	lRIGHT  (IDC_MASK_FRAME),-10 },

      {  lMOVE,    lLEFT	(IDC_MEMORY),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_MEMORY),	lTOP	(lPARENT),+6  },
      {  lSTRETCH, lRIGHT	(IDC_MEMORY),	lLEFT   (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_SECTORS),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_SECTORS),	lTOP	(IDC_MEMORY),+12  },
      {  lSTRETCH, lRIGHT	(IDC_SECTORS),	lLEFT   (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_FILES),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_FILES),	lTOP	(IDC_SECTORS),+12  },
      {  lSTRETCH, lRIGHT	(IDC_FILES),	lLEFT   (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_PACKED),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_PACKED),	lTOP	(IDC_FILES),+12  },
      {  lSTRETCH, lRIGHT	(IDC_PACKED),	lLEFT   (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_EXTRACT),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_EXTRACT),	lTOP	(IDC_PACKED),+12  },
      {  lSTRETCH, lRIGHT	(IDC_EXTRACT),	lLEFT   (IDC_MASK_FRAME),-5 },

      {  lMOVE,    lLEFT	(IDC_MAILBASES),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_MAILBASES),	lTOP	(IDC_EXTRACT),+12  },
      {  lSTRETCH, lRIGHT	(IDC_MAILBASES),	lLEFT   (IDC_MASK_FRAME),-5 },
      
	  {  lMOVE,    lLEFT	(IDC_MAILPLAIN),	lLEFT   (lPARENT),+15  },
      {  lMOVE,    lTOP		(IDC_MAILPLAIN),	lTOP	(IDC_MAILBASES),+12  },
      {  lSTRETCH, lRIGHT	(IDC_MAILPLAIN),	lLEFT   (IDC_MASK_FRAME),-5 },

      {  lEND                                                          }
   };
   
	Layout_ComputeLayout(m_hWnd, rules);
	
}

void CTabPage3::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------
	  {  lMOVE,    lLEFT	(IDC_WARNINGS),	lLEFT	  (lPARENT)		  ,+15  },
      {  lMOVE,    lTOP		(IDC_WARNINGS),	lTOP      (lPARENT)       ,+6  },
      {  lSTRETCH, lRIGHT	(IDC_WARNINGS),	lWIDTHOF  (lPARENT,50)	  ,-3  },

	  {  lMOVE,    lLEFT	(IDC_CA		),	lLEFT     (IDC_WARNINGS)  ,+0  },
      {  lMOVE,    lTOP		(IDC_CA		),	lTOP      (IDC_WARNINGS)  ,+12 },
      {  lSTRETCH, lRIGHT	(IDC_CA		),	lRIGHT	  (IDC_WARNINGS)  ,+0  },
      
	  {  lMOVE,    lLEFT	(IDC_REDUNDANT),lLEFT     (IDC_CA)  ,+0  },
      {  lMOVE,    lTOP		(IDC_REDUNDANT),lTOP      (IDC_CA)  ,+12 },
      {  lSTRETCH, lRIGHT	(IDC_REDUNDANT),lRIGHT	  (IDC_CA)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_SHOWOK),	lLEFT     (IDC_REDUNDANT)  ,+0  },	
      {  lMOVE,    lTOP		(IDC_SHOWOK),	lTOP      (IDC_REDUNDANT)  ,+12 },	
      {  lSTRETCH, lRIGHT	(IDC_SHOWOK),	lRIGHT	  (IDC_REDUNDANT)  ,+0  },	

	  {  lMOVE,    lLEFT	(IDC_SHOWPACK),	lLEFT     (IDC_SHOWOK)    ,+0  },
      {  lMOVE,    lTOP		(IDC_SHOWPACK),	lTOP      (IDC_SHOWOK)    ,+12 },
      {  lSTRETCH, lRIGHT	(IDC_SHOWPACK),	lRIGHT	  (IDC_SHOWOK)	  ,+0  },
/*
	  {  lMOVE,    lLEFT	(IDC_SOUND),	lLEFT     (IDC_SHOWPACK)  ,+0  },
      {  lMOVE,    lTOP		(IDC_SOUND),	lTOP      (IDC_SHOWPACK)  ,+12 },
      {  lSTRETCH, lRIGHT	(IDC_SOUND),	lRIGHT	  (IDC_SHOWPACK)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_TRACK),	lLEFT     (IDC_SOUND)     ,+0  },
      {  lMOVE,    lTOP		(IDC_TRACK),	lTOP      (IDC_SOUND)     ,+12 },
      {  lSTRETCH, lRIGHT	(IDC_TRACK),	lRIGHT	  (IDC_SOUND)	  ,+0  },
*/
	  {  lMOVE,    lLEFT	(IDC_REPORT_FRAME),	lRIGHT  (IDC_WARNINGS)   ,+5  },
      {  lMOVE,    lTOP		(IDC_REPORT_FRAME),	lTOP    (lPARENT)        ,+2  },
      {  lSTRETCH, lRIGHT	(IDC_REPORT_FRAME),	lRIGHT  (lPARENT)	     ,-5  },
      {  lSTRETCH, lBOTTOM	(IDC_REPORT_FRAME),	lBOTTOM   (lPARENT)       ,-5  },

	  {  lMOVE,    lLEFT	(IDC_REPORT),	lLEFT  (IDC_REPORT_FRAME),+10  },
      {  lMOVE,    lTOP		(IDC_REPORT),	lTOP   (IDC_REPORT_FRAME) ,+10 },
      {  lSTRETCH, lRIGHT	(IDC_REPORT),	lRIGHT (IDC_REPORT_FRAME) ,-5 },

	  {  lMOVE,    lLEFT	(IDC_REPORTFILE),	lLEFT  (IDC_REPORT) ,+0  },
      {  lMOVE,    lTOP		(IDC_REPORTFILE),	lTOP   (IDC_REPORT) ,+13 },
      {  lSTRETCH, lRIGHT	(IDC_REPORTFILE),	lRIGHT (IDC_REPORT_FRAME) ,-10 },

	  {  lMOVE,    lLEFT	(IDC_APPEND),	lLEFT  (IDC_REPORTFILE) ,+0  },
      {  lMOVE,    lTOP		(IDC_APPEND),	lTOP   (IDC_REPORTFILE) ,+18 },
      {  lSTRETCH, lRIGHT	(IDC_APPEND),	lRIGHT (IDC_REPORT) ,+0 },
	  
	  {  lMOVE,    lLEFT	(IDC_LIMIT),	lLEFT  (IDC_APPEND) ,+0  },
      {  lMOVE,    lTOP		(IDC_LIMIT),	lTOP   (IDC_APPEND) ,+12 },
      {  lSTRETCH, lRIGHT	(IDC_LIMIT),	lRIGHT (IDC_APPEND) ,+0 },

	  {  lMOVE,    lLEFT	(IDC_LIMITSIZE),	lLEFT  (IDC_LIMIT) ,+10  },
      {  lMOVE,    lTOP		(IDC_LIMITSIZE),	lTOP   (IDC_LIMIT) ,+12 },
	  
      {  lMOVE,    lBOTTOM	(IDC_BUTTON_CUST),	lBOTTOM   (lPARENT) ,-5 },

      {  lEND                                                          }
   };
   
	Layout_ComputeLayout(m_hWnd, rules);

}

#define SSTEP 10

void CTabPage5::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------

	  {  lMOVE,    lLEFT	(IDC_SCANNED_FRAME),	lLEFT    (lPARENT)       ,+5  },
      {  lMOVE,    lTOP		(IDC_SCANNED_FRAME),	lTOP      (lPARENT)       ,+2  },
      {  lSTRETCH, lBOTTOM	(IDC_SCANNED_FRAME),	lBOTTOM   (lPARENT)  ,-5  },
      {  lSTRETCH, lRIGHT	(IDC_SCANNED_FRAME),	lWIDTHOF  (lPARENT,50)    ,-3  },

	  {  lMOVE,    lLEFT	(IDC_FOUND_FRAME),	lRIGHT    (IDC_SCANNED_FRAME)  ,+5  },
      {  lMOVE,    lTOP		(IDC_FOUND_FRAME),	lTOP      (IDC_SCANNED_FRAME)  ,+0  },
      {  lSTRETCH, lBOTTOM	(IDC_FOUND_FRAME),	lBOTTOM   (IDC_SCANNED_FRAME)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_FOUND_FRAME),	lRIGHT    (lPARENT)           ,-5  },

	  {  lMOVE,    lLEFT	(IDC_S_SECTORS),	lWIDTHOF  (IDC_SCANNED_FRAME,70)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_SECTORS),	lTOP      (IDC_SCANNED_FRAME)  ,+10  },
      {  lSTRETCH, lRIGHT	(IDC_S_SECTORS),	lRIGHT    (IDC_SCANNED_FRAME)   ,-10  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_SECTORS),	lLEFT  (IDC_SCANNED_FRAME)  ,+5  },
      {  lMOVE,    lTOP		(IDC_STATIC_SECTORS),	lTOP   (IDC_S_SECTORS)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_SECTORS),	lLEFT  (IDC_S_SECTORS)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_S_FILES),	lLEFT     (IDC_S_SECTORS)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_FILES),	lTOP      (IDC_S_SECTORS)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_FILES),	lRIGHT    (IDC_S_SECTORS)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_FILES),	lLEFT  (IDC_STATIC_SECTORS)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_FILES),	lTOP   (IDC_S_FILES)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_FILES),	lLEFT  (IDC_S_FILES)  ,+0  },

	  {  lMOVE,    lLEFT     (IDC_S_DIRECTORIES)  ,lLEFT	(IDC_S_FILES),	+0  },
      {  lMOVE,    lTOP      (IDC_S_DIRECTORIES)  ,lTOP		(IDC_S_FILES),	+SSTEP  },
      {  lSTRETCH, lRIGHT    (IDC_S_DIRECTORIES)  ,lRIGHT	(IDC_S_FILES),	+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_FOLDERS),	lLEFT  (IDC_STATIC_SECTORS) ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_FOLDERS),	lTOP   (IDC_S_DIRECTORIES)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_FOLDERS),	lLEFT  (IDC_S_DIRECTORIES)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_S_ARCHIVES),lLEFT     (IDC_S_DIRECTORIES)  ,	+0  },
      {  lMOVE,    lTOP		(IDC_S_ARCHIVES),lTOP      (IDC_S_DIRECTORIES)  ,	+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_ARCHIVES),lRIGHT    (IDC_S_DIRECTORIES)  ,	+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_ARCHIVES),	lLEFT  (IDC_STATIC_SECTORS) ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_ARCHIVES),	lTOP   (IDC_S_ARCHIVES)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_ARCHIVES),	lLEFT  (IDC_S_ARCHIVES)  ,+0  },

	  {  lMOVE,    lLEFT     (IDC_S_PACKED)  ,lLEFT	(IDC_S_ARCHIVES),	+0  },
      {  lMOVE,    lTOP      (IDC_S_PACKED)  ,lTOP		(IDC_S_ARCHIVES),	+SSTEP  },
      {  lSTRETCH, lRIGHT    (IDC_S_PACKED)  ,lRIGHT	(IDC_S_ARCHIVES),	+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_PACKED),	lLEFT  (IDC_STATIC_SECTORS) ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_PACKED),	lTOP   (IDC_S_PACKED)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_PACKED),	lLEFT  (IDC_S_PACKED)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_SPEED),lLEFT     (IDC_S_PACKED)  ,	+0  },
      {  lMOVE,    lTOP		(IDC_S_SPEED),lTOP      (IDC_S_PACKED)  ,	+SSTEP*2  },
      {  lSTRETCH, lRIGHT	(IDC_S_SPEED),lRIGHT    (IDC_S_PACKED)  ,	+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_SPEED),	    lLEFT  (IDC_STATIC_SECTORS) ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_SPEED),	    lTOP   (IDC_S_SPEED)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_SPEED),	    lLEFT  (IDC_S_SPEED)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_TIME),lLEFT       (IDC_S_SPEED)  ,	+0  },
      {  lMOVE,    lTOP		(IDC_TIME),lTOP        (IDC_S_SPEED)  ,	+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_TIME),lRIGHT      (IDC_S_SPEED)  ,	+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_TIME), 	   lLEFT  (IDC_STATIC_SECTORS) ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_TIME),	   lTOP   (IDC_TIME)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_TIME),	   lLEFT  (IDC_TIME)  ,+0  },



	  {  lMOVE,    lRIGHT	(IDC_S_VIRUS),	lRIGHT    (IDC_FOUND_FRAME)   ,-10  },
      {  lMOVE,    lTOP		(IDC_S_VIRUS),	lTOP      (IDC_S_SECTORS)     ,+0  },
      {  lSTRETCH, lWIDTH	(IDC_S_VIRUS),	lWIDTH    (IDC_S_SECTORS)     ,+0  },


	  {  lMOVE,    lLEFT	(IDC_STATIC_VIRUSES),	lLEFT  (IDC_FOUND_FRAME)  ,+5  },
      {  lMOVE,    lTOP		(IDC_STATIC_VIRUSES),	lTOP   (IDC_S_VIRUS)      ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_VIRUSES),	lLEFT  (IDC_S_VIRUS)      ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_BODIES),	lLEFT     (IDC_S_VIRUS)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_BODIES),	lTOP      (IDC_S_VIRUS)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_BODIES),	lRIGHT    (IDC_S_VIRUS)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_BODIES),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_BODIES),	lTOP   (IDC_S_BODIES)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_BODIES),	lLEFT  (IDC_S_BODIES)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_DISINFECTED),	lLEFT     (IDC_S_BODIES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_DISINFECTED),	lTOP      (IDC_S_BODIES)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_DISINFECTED),	lRIGHT    (IDC_S_BODIES)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_DISINFECTED),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_DISINFECTED),	lTOP   (IDC_S_DISINFECTED)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_DISINFECTED),	lLEFT  (IDC_S_DISINFECTED)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_DELETED),	lLEFT     (IDC_S_DISINFECTED)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_DELETED),	lTOP      (IDC_S_DISINFECTED)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_DELETED),	lRIGHT    (IDC_S_DISINFECTED)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_DELETED),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_DELETED),	lTOP   (IDC_S_DELETED)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_DELETED),	lLEFT  (IDC_S_DELETED)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_WARNING),	lLEFT     (IDC_S_DELETED)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_WARNING),	lTOP      (IDC_S_DELETED)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_WARNING),	lRIGHT    (IDC_S_DELETED)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_WARNINGS),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_WARNINGS),	lTOP   (IDC_S_WARNING)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_WARNINGS),	lLEFT  (IDC_S_WARNING)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_SUSPIC),	lLEFT     (IDC_S_WARNING)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_SUSPIC),	lTOP      (IDC_S_WARNING)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_SUSPIC),	lRIGHT    (IDC_S_WARNING)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_SUSPIC),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_SUSPIC),	lTOP   (IDC_S_SUSPIC)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_SUSPIC),	lLEFT  (IDC_S_SUSPIC)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_CORRUPTED),	lLEFT     (IDC_S_SUSPIC)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_CORRUPTED),	lTOP      (IDC_S_SUSPIC)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_CORRUPTED),	lRIGHT    (IDC_S_SUSPIC)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_CORRUPTED),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_CORRUPTED),	lTOP   (IDC_S_CORRUPTED)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_CORRUPTED),	lLEFT  (IDC_S_CORRUPTED)  ,+0  },


	  {  lMOVE,    lLEFT	(IDC_S_IOERRORS),	lLEFT     (IDC_S_CORRUPTED)  ,+0  },
      {  lMOVE,    lTOP		(IDC_S_IOERRORS),	lTOP      (IDC_S_CORRUPTED)  ,+SSTEP  },
      {  lSTRETCH, lRIGHT	(IDC_S_IOERRORS),	lRIGHT    (IDC_S_CORRUPTED)  ,+0  },

	  {  lMOVE,    lLEFT	(IDC_STATIC_IOERRORS),	lLEFT  (IDC_STATIC_VIRUSES)  ,+0  },
      {  lMOVE,    lTOP		(IDC_STATIC_IOERRORS),	lTOP   (IDC_S_IOERRORS)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_STATIC_IOERRORS),	lLEFT  (IDC_S_IOERRORS)  ,+0  },
  

	  {  lEND                                                          }
   };
   
	Layout_ComputeLayout(m_hWnd, rules);
	
}

void CTabPage6::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------
	  {  lMOVE,    lLEFT	(IDC_INFECTED_OBJ),	lLEFT    (lPARENT)       ,+5  },
      {  lMOVE,    lTOP		(IDC_INFECTED_OBJ),	lTOP      (lPARENT)       ,+2  },
      {  lSTRETCH, lRIGHT	(IDC_INFECTED_OBJ),	lWIDTHOF  (lPARENT,50)    ,-3  },
      {  lSTRETCH, lBOTTOM	(IDC_INFECTED_OBJ),	lBOTTOM   (lPARENT)       ,-5  },

	  {  lMOVE,    lLEFT	(IDC_SUSP_OBJ),	lRIGHT    (IDC_INFECTED_OBJ)  ,+5  },
      {  lMOVE,    lTOP		(IDC_SUSP_OBJ),	lTOP      (IDC_INFECTED_OBJ)  ,+0  },
      {  lSTRETCH, lRIGHT	(IDC_SUSP_OBJ),	lRIGHT    (lPARENT)           ,-5  },
      {  lSTRETCH, lBOTTOM	(IDC_SUSP_OBJ),	lBOTTOM   (lPARENT)       ,-5  },

      {  lMOVE,    lLEFT	(IDC_REPORTONLY),	lLEFT   (IDC_INFECTED_OBJ),+8  },
      {  lMOVE,    lTOP		(IDC_REPORTONLY),	lTOP	(IDC_INFECTED_OBJ),+12  },
      {  lSTRETCH, lRIGHT	(IDC_REPORTONLY),	lRIGHT  (IDC_INFECTED_OBJ),-5 },

      {  lMOVE,    lLEFT	(IDC_DISINFECTDIALOG),	lLEFT   (IDC_REPORTONLY),+0  },
      {  lMOVE,    lTOP		(IDC_DISINFECTDIALOG),	lTOP	(IDC_REPORTONLY),+12  },
      {  lSTRETCH, lRIGHT	(IDC_DISINFECTDIALOG),	lRIGHT  (IDC_REPORTONLY),+0 },

      {  lMOVE,    lLEFT	(IDC_DISINFECTAUTO),	lLEFT   (IDC_DISINFECTDIALOG),+0  },
      {  lMOVE,    lTOP		(IDC_DISINFECTAUTO),	lTOP	(IDC_DISINFECTDIALOG),+12  },
      {  lSTRETCH, lRIGHT	(IDC_DISINFECTAUTO),	lRIGHT  (IDC_DISINFECTDIALOG),+0 },

      {  lMOVE,    lLEFT	(IDC_DELETE),	lLEFT   (IDC_DISINFECTAUTO),+0  },
      {  lMOVE,    lTOP		(IDC_DELETE),	lTOP	(IDC_DISINFECTAUTO),+12  },
      {  lSTRETCH, lRIGHT	(IDC_DELETE),	lRIGHT  (IDC_DISINFECTAUTO),+0 },

      {  lMOVE,    lLEFT	(IDC_COPYINFECTED),	lLEFT   (IDC_DELETE),+0  },
      {  lMOVE,    lTOP		(IDC_COPYINFECTED),	lBOTTOM	(lPARENT),-35  },
      {  lSTRETCH, lRIGHT	(IDC_COPYINFECTED),	lRIGHT  (IDC_DELETE),+0 },

      {  lMOVE,    lLEFT	(IDC_INFECTFOLDER),	lLEFT   (IDC_COPYINFECTED),+10  },
      {  lMOVE,    lTOP		(IDC_INFECTFOLDER),	lTOP	(IDC_COPYINFECTED),+12  },
      {  lSTRETCH, lRIGHT	(IDC_INFECTFOLDER),	lRIGHT  (IDC_INFECTED_OBJ),-10 },

      {  lMOVE,    lLEFT	(IDC_COPYSUSPICIOUS),	lLEFT   (IDC_SUSP_OBJ),+8  },
      {  lMOVE,    lTOP		(IDC_COPYSUSPICIOUS),	lTOP	(IDC_COPYINFECTED),+0  },
      {  lSTRETCH, lRIGHT	(IDC_COPYSUSPICIOUS),	lRIGHT  (IDC_SUSP_OBJ), -5 },

      {  lMOVE,    lLEFT	(IDC_SUSPFOLDER),	lLEFT   (IDC_COPYSUSPICIOUS),+10  },
      {  lMOVE,    lTOP		(IDC_SUSPFOLDER),	lTOP	(IDC_COPYSUSPICIOUS),+12  },
      {  lSTRETCH, lRIGHT	(IDC_SUSPFOLDER),	lRIGHT  (IDC_SUSP_OBJ),-10 },

      {  lEND                                                          }
   };
   
	Layout_ComputeLayout(m_hWnd, rules);
	
}

BOOL GetUniversalName( char szUniv[], char szDrive[] )
{
   // get the local drive letter
   char chLocal = toupper( szDrive[0] );
 
   // cursory validation
   if ( chLocal < 'A' || chLocal > 'Z' )
      return FALSE;
 
   if ( szDrive[1] != ':' || szDrive[2] != '\\' )
      return FALSE;
 
   HANDLE hEnum;
   DWORD dwResult = WNetOpenEnum( RESOURCE_CONNECTED, RESOURCETYPE_DISK,
                                  0, NULL, &hEnum );
 
   if ( dwResult != NO_ERROR )
      return FALSE;
 
   // request all available entries
   const int    c_cEntries   = 0xFFFFFFFF;
   // start with a reasonable buffer size
   DWORD        cbBuffer     = 50 * sizeof( NETRESOURCE );
   NETRESOURCE *pNetResource = (NETRESOURCE*) malloc( cbBuffer );
 
   BOOL fResult = FALSE;
 
   while ( TRUE )
   {
      DWORD dwSize   = cbBuffer,
            cEntries = c_cEntries;
 
      dwResult = WNetEnumResource( hEnum, &cEntries, pNetResource,
                                   &dwSize );
 
      if ( dwResult == ERROR_MORE_DATA )
      {
         // the buffer was too small, enlarge
         cbBuffer = dwSize;
         pNetResource = (NETRESOURCE*) realloc( pNetResource, cbBuffer );
         continue;
      }
 
      if ( dwResult != NO_ERROR )
         goto done;
 
      // search for the specified drive letter
      for ( int i = 0; i < (int) cEntries; i++ )
         if ( pNetResource[i].lpLocalName &&
              chLocal == toupper(pNetResource[i].lpLocalName[0]) )
         {
            // match
            fResult = TRUE;
 
            // build a UNC name
            strcpy( szUniv, pNetResource[i].lpRemoteName );
            strcat( szUniv, szDrive + 2 );
            _strupr( szUniv );
            goto done;
         }
   }
 
done:
   // cleanup
   WNetCloseEnum( hEnum );
   free( pNetResource );
 
   return fResult;
 
}

BOOL	FindDiskInList(CListCtrl& m_lDisk,char* p)
{ 
	CString s=p;
	s.SetAt(2,' ');
	s.MakeUpper();
	int i=-1;
	while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_ALL))){
		CString si=m_lDisk.GetItemText(i,0);
		si=si.Left(3);
		si.MakeUpper();
		if(si==s)return TRUE;
	}
	return FALSE;
}

void	CheckDisksInList(CListCtrl& m_lDisk,char* str)
{
	int i=-1;
	while(-1 != (i=m_lDisk.GetNextItem(i,LVNI_ALL))){
		CString si=m_lDisk.GetItemText(i,0);
		si=si.Left(3);
		si.MakeUpper();
		if(si[2]==' '){
			char* p=str;
			while(*p){
				CString s=p;
				s.SetAt(2,' ');
				s.MakeUpper();
				if(s==si) goto next;
				p=strchr(p,0);
				if(p==NULL)goto del;
				p++;
			}
del:		m_lDisk.DeleteItem(i);
		}
next:
		;
	}
}

int CTabPage1::AddDisks()
{
	char buf[0x1000];
	UINT bread=GetLogicalDriveStrings(0x1000,buf);
	char* p=buf;
	UINT type;
	CString Volume;
	CString VolumeName;
	CString FileSystemName;
	DWORD VolumeSerialNumber;
	DWORD MaximumComponentLength;
	DWORD FileSystemFlags;
	int i=0;
	
//	m_lDisk.DeleteAllItems();
	CheckDisksInList(m_lDisk,p);

	while(*p){

		if(FindDiskInList(m_lDisk,p))goto next;

		Volume=p;
		Volume=Volume.Left(2);
		Volume.MakeUpper();

		VolumeName="";
		type=GetDriveType(p);
		switch(type){
		case 0:
		case 1:
			// Error type.
			continue;

		case DRIVE_REMOVABLE:
			VolumeName=MS( IDS_FLOPPY_DRIVE );
			break;
		
		case DRIVE_REMOTE:
			{
				DWORD l=512;
				int j=GetUniversalName(VolumeName.GetBuffer(l),p);
				VolumeName.ReleaseBuffer();
				VolumeName.MakeLower();
				if(j)	break;
			}

		case DRIVE_CDROM:
		case DRIVE_FIXED:
		default:
			VolumeName.Empty();
			GetVolumeInformation(p,//LPCTSTR   address of root directory of the file system 
			VolumeName.GetBuffer(0x100),	// address of name of the volume 
			0x100,	// length of lpVolumeNameBuffer 
			&VolumeSerialNumber,	// address of volume serial number 
			&MaximumComponentLength,	// address of system's maximum filename length
			&FileSystemFlags,	// address of file system flags 
			FileSystemName.GetBuffer(0x100),	// address of name of file system 
			0x100 	// length of lpFileSystemNameBuffer 
			);
			FileSystemName.ReleaseBuffer();
			VolumeName.ReleaseBuffer();
			if(VolumeName.IsEmpty())
				if(type==DRIVE_CDROM)		VolumeName=MS( IDS_CDROM_DRIVE );
				else VolumeName=" ";
			VolumeName.MakeLower();
			break;
		}
		if(VolumeName.GetLength())VolumeName.SetAt(0,toupper(VolumeName.GetAt(0)));

		InsertPathString(Volume,VolumeName,type,FALSE,FALSE);	
next:
		i++;
		p=strchr(p,0);
		if(p==NULL)break;
		p++;
	}

	return i;
}

void CTabPage3::OnButtonCust() 
{
	Parent->cust.DoModal();
}

void CTabPage1::OnClickListDisk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(!Parent->cust.m_CustSingleClick)return;
	if(m_Disabled)return;
	
	int i=-1;;
	if(-1 != (i=m_lDisk.GetNextItem(i,LVNI_FOCUSED))){
		int k=0;
		int j=-1;
		while(-1 != (j=m_lDisk.GetNextItem(j,LVNI_SELECTED))) k++;
		if(k==1)
			m_lDisk.SetItemState(i,(m_lDisk.GetItemState(i,LVIS_OVERLAY))?0:INDEXTOOVERLAYMASK(1) ,LVIS_OVERLAY);
	}

}

void CTabPage2::OnExclude() 
{
	GetDlgItem(IDC_EDIT_EXCLUDE)->EnableWindow(IsDlgButtonChecked(IDC_EXCLUDE));
}
