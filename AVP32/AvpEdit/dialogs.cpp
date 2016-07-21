// Dialogs.cpp : implementation file
//

#include "stdafx.h"
#include <fcntl.h>
#include <sys\stat.h>

#include "AvpEdit.h"
#include "REdit.h"
#include "Dialogs.h"
#include "DDX_Hex.h"
#include "MainFrm.h"

#if (defined (KL_MULTIPLATFORM_LINKS))
    #include "LinkDlg.h"
#endif

#include "OptDlg.h"

#include <Bases/Method.h>
#include <ScanAPI/scanobj.h>
#include <ScanAPI/avp_dll.h>
#include <Bases/sizes.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* MainFramePtr;
extern COptDlg OptDlg;
/////////////////////////////////////////////////////////////////////////////
// CDInsert dialog

CDInsert::CDInsert(CWnd* pParent /*=NULL*/)
	: CDialog(CDInsert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDInsert)
	m_iType = -1;
	//}}AFX_DATA_INIT
}


void CDInsert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDInsert)
	DDX_Radio(pDX, IDC_INSERT_R1, m_iType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDInsert, CDialog)
	//{{AFX_MSG_MAP(CDInsert)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDInsert message handlers
/////////////////////////////////////////////////////////////////////////////
// CDSeparator dialog


CDSeparator::CDSeparator(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDSeparator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDSeparator)
	m_Comment = _T("");
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDSeparator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSeparator)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSeparator, CDialog)
	//{{AFX_MSG_MAP(CDSeparator)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSeparator message handlers
/////////////////////////////////////////////////////////////////////////////
// CDKernel dialog


CDKernel::CDKernel(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDKernel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDKernel)
	m_Comment = _T("");
	m_Name = _T("");
	m_Link16 = _T("");
	m_Link32 = _T("");
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDKernel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDKernel)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDKernel, CDialog)
	//{{AFX_MSG_MAP(CDKernel)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDKernel message handlers
#include <io.h>

#if (!defined (KL_MULTIPLATFORM_LINKS))
    extern CString PathLink;
    CString record;
#endif

extern CString PathAutoFill;

BOOL CheckDir(CString& name)
{
	char buff[0x200];
	char* fn=0;
	if(! _access(name,4)) return 1;
	if(! GetFullPathName(name,0x200,buff,&fn)) return 0;
	if( fn <= buff ) return 0;
	char* n=fn-1;
	do{
		*n=0;
		n=strrchr(buff,'\\');
		if(n) *(n+1)=0;
		else return 0;
	}while(_access(buff,4));
	strcpy(n+1,fn);
	name=buff;
	return 1;
}

#if (!defined (KL_MULTIPLATFORM_LINKS))
CString GetObjName(CWnd* Wnd, CString record_)
{
	CString name = PathLink;
	CheckDir(name);

	CFileDialog cfd( TRUE ,"obj", NULL,
		OFN_FILEMUSTEXIST
		|OFN_PATHMUSTEXIST
		|OFN_EXPLORER
		|OFN_READONLY
		|OFN_HIDEREADONLY
		|OFN_NOCHANGEDIR, 
		"Obj modules (*.obj,*.o16,*.o32)|*.obj;*.o16;*.o32|16-bit Obj (*.o16)|*.o16|32-bit Obj (*.o32)|*.o32||",Wnd);

	cfd.m_ofn.lpstrTitle="Select OBJ file"; 
//	cfd.m_ofn.lpstrInitialDir=name;
	cfd.m_ofn.lpstrFile=name.GetBuffer(0x200);
	cfd.m_ofn.nMaxFile=0x200;

	int ret=cfd.DoModal();
	
	name.ReleaseBuffer();
	if(ret==IDOK){
		record=record_;
		PathLink=name;
		return PathLink;
	}
	else return "";
}

CString GetObjNameCurrent(CWnd* Wnd, CString record_){
	if(record==record_ && !_access(PathLink,4))	return PathLink;
	return GetObjName(Wnd,record_);
}
#endif

CString GetSumName(CWnd* Wnd)
{
	CString name = PathAutoFill;
	CheckDir(name);

	CFileDialog cfd( TRUE ,"obj", NULL,
		OFN_FILEMUSTEXIST
		|OFN_PATHMUSTEXIST
		|OFN_EXPLORER
		|OFN_READONLY
		|OFN_HIDEREADONLY
		|OFN_NOCHANGEDIR, 
		"All files (*.*)|*.*|Bin files (*.b*,*.e*,*.c*)|*.b*;*.e*;*.c*||",Wnd);

	cfd.m_ofn.lpstrTitle="Select file for Sum calc"; 
//	cfd.m_ofn.lpstrInitialDir=name;
	cfd.m_ofn.lpstrFile=name.GetBuffer(0x200);
	cfd.m_ofn.nMaxFile=0x200;

	int ret=cfd.DoModal();

	name.ReleaseBuffer();
	if(ret==IDOK){
		PathAutoFill=name;
		return PathAutoFill;
	}
	else return "";
}



/////////////////////////////////////////////////////////////////////////////
// CDJump dialog


CDJump::CDJump(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDJump::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDJump)
	m_Name = _T("");
	m_Method = -1;
	m_Comment = _T("");
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_2Bytes=0;
	m_Data1=0;
	m_Data2=0;
	m_Data3=0;
	m_Sum1=0;
	//}}AFX_DATA_INIT

	m_Len1=0;
	m_Mask1=0;
	m_Mask2=0;

	REptr=reptr;
}


void CDJump::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDJump)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_CBIndex(pDX, IDC_METHOD, m_Method);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_TextToWORD(pDX, IDC_2BYTES, m_2Bytes);
	DDX_TextToWORD(pDX, IDC_DATA1, m_Data1);
	DDX_TextToWORD(pDX, IDC_DATA2, m_Data2);
	DDX_TextToWORD(pDX, IDC_DATA3, m_Data3);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	//}}AFX_DATA_MAP
	
	if(pDX->m_bSaveAndValidate)
	{
		BOOL chk;
		m_Len1=0;
		m_Mask1= m_Mask2= 0;
		for(int i=0;i<32;i++){
			DDX_Check(pDX, IDC_BIT0 + i , chk);
			m_Mask1 |=(chk)?(1<<i):0;
			if(chk)	m_Len1=i+1;
		}
		for(i=0;i<32;i++){
			DDX_Check(pDX, IDC_BIT32+ i , chk);
			m_Mask2 |=(chk)?(1<<i):0;
			if(chk)	m_Len1=i+33;
		}
  	}
	else
	{
		BOOL chk;
		m_Len1=0;
		for(int i=0;i<32;i++){
			chk=(m_Mask1 & (1<<i))?1:0;
			DDX_Check(pDX, IDC_BIT0 + i , chk);
			if(chk)	m_Len1=i+1;
		}
		for(i=0;i<32;i++){
			chk=(m_Mask2 & (1<<i))?1:0;
			DDX_Check(pDX, IDC_BIT32+ i , chk);
			if(chk)	m_Len1=i+33;
		}
	}
}


BEGIN_MESSAGE_MAP(CDJump, CDialog)
	//{{AFX_MSG_MAP(CDJump)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_CBN_SELCHANGE(IDC_METHOD, OnSelchangeMethod)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDJump message handlers


void CDJump::OnButtonSum() 
{
	UpdateData();

	CDSum sd(m_Len1,0,0,0,0,this,0,m_Mask1,m_Mask2,1);

	if(IDOK!=sd.DoModal())return;

	int k,m;
	for(k=0; k<32 && k<=m_Len1; k++){
		if(((m_Mask1)&(1<<k))==0) sd.m_Buf1[k]=0;
	}
	for(m=0; m<32 && k<=m_Len1;k++, m++){
		if(((m_Mask2)&(1<<m))==0) sd.m_Buf1[k]=0;
	}

	m_Sum1=CalcSum(sd.m_Buf1,m_Len1);
	
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;

	UpdateData(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CDMemory dialog


CDMemory::CDMemory(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDMemory::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDMemory)
	m_Name = _T("");
	m_Comment = _T("");
	m_Method = -1;
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_Offset=0;
	m_Byte=0;
	m_Len1=0;
	m_Len2=0;
	m_Off2=0;
	m_Segment=0;
	m_Sum1=0;
	//}}AFX_DATA_INIT
	memset(m_Replace,0,5);

	REptr=reptr;
}


void CDMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDMemory)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_CBIndex(pDX, IDC_METHOD, m_Method);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	DDX_TextToWORD(pDX, IDC_OFFSET, m_Offset);
	DDX_TextToBYTE(pDX, IDC_BYTE, m_Byte);
	DDX_TextToBYTE(pDX, IDC_LEN, m_Len1);
	DDX_TextToBYTE(pDX, IDC_LEN2, m_Len2);
	DDX_TextToWORD(pDX, IDC_OFF2, m_Off2);
	DDX_TextToWORD(pDX, IDC_SEGMENT, m_Segment);
	//}}AFX_DATA_MAP
	DDX_TextToBuffer(pDX, IDC_REPLACE, m_Replace, 5);
}


BEGIN_MESSAGE_MAP(CDMemory, CDialog)
	//{{AFX_MSG_MAP(CDMemory)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_CBN_SELCHANGE(IDC_METHOD, OnSelchangeMethod)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDMemory message handlers

void CDMemory::OnButtonSum() 
{
	UpdateData();

	CDSum sd(m_Len1,0,m_Offset,0,0,this,1,(DWORD)-1,(DWORD)-1,3);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Byte=sd.m_Buf1[0];
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CDSector dialog


CDSector::CDSector(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDSector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDSector)
	m_Name = _T("");
	m_Comment = _T("");
	m_Aboot1 = FALSE;
	m_Aboot2 = FALSE;
	m_FDboot1 = FALSE;
	m_FDboot2 = FALSE;
	m_HDboot1 = FALSE;
	m_HDboot2 = FALSE;
	m_MBR1 = FALSE;
	m_MBR2 = FALSE;
	m_Page2 = -1;
	m_Page1 = -1;
	m_Method1 = -1;
	m_Method2 = -1;
	m_Page_C1 = -1;
	m_Page_C2 = -1;
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_2Bytes=0;
	m_Addr11=0;
	m_Addr12=0;
	m_Addr21=0;
	m_Addr22=0;
	m_Data1=0;
	m_Data2=0;
	m_Len1=0;
	m_Len2=0;
	m_Off1=0;
	m_Off2=0;
	m_Sum1=0;
	m_Sum2=0;
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDSector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSector)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Check(pDX, IDC_ABOOT1, m_Aboot1);
	DDX_Check(pDX, IDC_ABOOT2, m_Aboot2);
	DDX_Check(pDX, IDC_FDBOOT1, m_FDboot1);
	DDX_Check(pDX, IDC_FDBOOT2, m_FDboot2);
	DDX_Check(pDX, IDC_HDBOOT1, m_HDboot1);
	DDX_Check(pDX, IDC_HDBOOT2, m_HDboot2);
	DDX_Check(pDX, IDC_MBR1, m_MBR1);
	DDX_Check(pDX, IDC_MBR2, m_MBR2);
	DDX_CBIndex(pDX, IDC_PAGE2, m_Page2);
	DDX_CBIndex(pDX, IDC_PAGE1, m_Page1);
	DDX_CBIndex(pDX, IDC_METHOD1, m_Method1);
	DDX_CBIndex(pDX, IDC_METHOD2, m_Method2);
	DDX_CBIndex(pDX, IDC_PAGE_C1, m_Page_C1);
	DDX_CBIndex(pDX, IDC_PAGE_C2, m_Page_C2);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_TextToWORD(pDX, IDC_2BYTES, m_2Bytes);
	DDX_TextToWORD(pDX, IDC_ADDRESS11, m_Addr11);
	DDX_TextToWORD(pDX, IDC_ADDRESS12, m_Addr12);
	DDX_TextToWORD(pDX, IDC_ADDRESS21, m_Addr21);
	DDX_TextToWORD(pDX, IDC_ADDRESS22, m_Addr22);
	DDX_TextToSignedBYTE(pDX, IDC_DATA1, m_Data1);
	DDX_TextToSignedBYTE(pDX, IDC_DATA2, m_Data2);
	DDX_TextToBYTE(pDX, IDC_LEN1, m_Len1);
	DDX_TextToBYTE(pDX, IDC_LEN2, m_Len2);
	DDX_TextToWORD(pDX, IDC_OFF1, m_Off1);
	DDX_TextToWORD(pDX, IDC_OFF2, m_Off2);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	DDX_TextToDWORD(pDX, IDC_SUM2, m_Sum2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSector, CDialog)
	//{{AFX_MSG_MAP(CDSector)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_CBN_SELCHANGE(IDC_METHOD1, OnSelchangeMethod1)
	ON_CBN_SELCHANGE(IDC_METHOD2, OnSelchangeMethod2)
	ON_BN_CLICKED(IDC_BUTTON_SUM_FILE, OnButtonSumFile)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSector message handlers

extern WORD WorkAreaPage[][2];

void CDSector::OnButtonSum() 
{
	UpdateData();

	BYTE SubType=ST_COM;

	CDSum sd(m_Len1,m_Len2, 
		WorkAreaPage[m_Page1][0]+ m_Off1, 
		WorkAreaPage[m_Page2][0]+ m_Off2,SubType,this,1);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CDFile dialog


CDFile::CDFile(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFile)
	m_Comment = _T("");
	m_Name = _T("");
	m_Method = 0;
	m_Com = FALSE;
	m_Exe = FALSE;
	m_NE = FALSE;
	m_Ole2 = FALSE;
	m_Page_C = -1;
	m_Page1 = -1;
	m_Page2 = -1;
	m_Sys = FALSE;
	m_2Bytes=0;
	m_Data1=0;
	m_Data2=0;
	m_Data3=0;
	m_Data4=0;
	m_Data5=0;
	m_Len1=0;
	m_Len2=0;
	m_Off1=0;
	m_Off2=0;
	m_Sum1=0;
	m_Sum2=0;
	m_Script = FALSE;
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_Hlp = FALSE;
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CDFile)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_CBIndex(pDX, IDC_METHOD, m_Method);
	DDX_Check(pDX, IDC_COM, m_Com);
	DDX_Check(pDX, IDC_EXE, m_Exe);
	DDX_Check(pDX, IDC_NE, m_NE);
	DDX_Check(pDX, IDC_OLE2, m_Ole2);
	DDX_CBIndex(pDX, IDC_PAGE_C, m_Page_C);
	DDX_CBIndex(pDX, IDC_PAGE1, m_Page1);
	DDX_CBIndex(pDX, IDC_PAGE2, m_Page2);
	DDX_Check(pDX, IDC_SYS, m_Sys);
	DDX_TextToWORD(pDX, IDC_2BYTES, m_2Bytes);
	DDX_TextToSignedWORD(pDX, IDC_DATA1, m_Data1);
	DDX_TextToSignedWORD(pDX, IDC_DATA2, m_Data2);
	DDX_TextToSignedWORD(pDX, IDC_DATA3, m_Data3);
	DDX_TextToSignedWORD(pDX, IDC_DATA4, m_Data4);
	DDX_TextToSignedWORD(pDX, IDC_DATA5, m_Data5);
	DDX_TextToBYTE(pDX, IDC_LEN1, m_Len1);
	DDX_TextToBYTE(pDX, IDC_LEN2, m_Len2);
	DDX_TextToWORD(pDX, IDC_OFF1, m_Off1);
	DDX_TextToWORD(pDX, IDC_OFF2, m_Off2);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	DDX_TextToDWORD(pDX, IDC_SUM2, m_Sum2);
	DDX_Check(pDX, IDC_SCRIPT, m_Script);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_Check(pDX, IDC_HLP, m_Hlp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFile, CDialog)
	//{{AFX_MSG_MAP(CDFile)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_CBN_SELCHANGE(IDC_METHOD, OnSelchangeMethod)
	ON_BN_CLICKED(IDC_BUTTON_SUM_FILE, OnButtonSumFile)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	ON_BN_CLICKED(IDC_OLE2, OnOle2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFile message handlers


void CDFile::OnButtonSum() 
{
	UpdateData();
	
	BYTE SubType=0;
	SubType|=(m_Com)? ST_COM: 0;
	SubType|=(m_Exe)? ST_EXE: 0;
	SubType|=(m_Sys)? ST_SYS: 0;
	SubType|=(m_NE)?  ST_NE: 0;
	SubType|=(m_Ole2)?ST_OLE2: 0;
	SubType|=(m_Script)?  ST_SCRIPT: 0;
	SubType|=(m_Hlp)?  ST_HLP: 0;
	
	int Off1=WorkAreaPage[m_Page1][0]+ m_Off1;
	int Off2=WorkAreaPage[m_Page2][0]+ m_Off2;
	DWORD disflags=0;
	
	
	if(m_Page1>=3){
		disflags|=0x100;
		disflags|=1;
	}
	if(m_Page2>=3){
		disflags|=0x200;
		disflags|=2;
	}
	
	CDSum sd(m_Len1,m_Len2, Off1, Off2,
		SubType,this,1,(DWORD)-1,(DWORD)-1,disflags);

	if(IDOK!=sd.DoModal())return;


	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);

	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CDCA dialog


CDCA::CDCA(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDCA::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDCA)
	m_Comment = _T("");
	m_Name = _T("");
	m_Com = FALSE;
	m_Exe = FALSE;
	m_NE = FALSE;
	m_Ole2 = FALSE;
	m_Page1 = -1;
	m_Page2 = -1;
	m_Sys = FALSE;
	m_Len1=0;
	m_Len2=0;
	m_Off1=0;
	m_Off2=0;
	m_Sum1=0;
	m_Sum2=0;
	m_FalseAlarm = FALSE;
	m_Script = FALSE;
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_Hlp = FALSE;
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDCA::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDCA)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_Check(pDX, IDC_COM, m_Com);
	DDX_Check(pDX, IDC_EXE, m_Exe);
	DDX_Check(pDX, IDC_NE, m_NE);
	DDX_Check(pDX, IDC_OLE2, m_Ole2);
	DDX_CBIndex(pDX, IDC_PAGE1, m_Page1);
	DDX_CBIndex(pDX, IDC_PAGE2, m_Page2);
	DDX_Check(pDX, IDC_SYS, m_Sys);
	DDX_TextToBYTE(pDX, IDC_LEN1, m_Len1);
	DDX_TextToBYTE(pDX, IDC_LEN2, m_Len2);
	DDX_TextToWORD(pDX, IDC_OFF1, m_Off1);
	DDX_TextToWORD(pDX, IDC_OFF2, m_Off2);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	DDX_TextToDWORD(pDX, IDC_SUM2, m_Sum2);
	DDX_TextToWORD(pDX, IDC_2BYTES, m_2Bytes);
	DDX_Check(pDX, IDC_FALSEALARM, m_FalseAlarm);
	DDX_Check(pDX, IDC_SCRIPT, m_Script);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_Check(pDX, IDC_HLP, m_Hlp);
	//}}AFX_DATA_MAP

	CWnd* wndp;
	if(wndp=GetDlgItem(IDC_SECOND))wndp->EnableWindow(m_FalseAlarm);
	if(wndp=GetDlgItem(IDC_PAGE2))wndp->EnableWindow(m_FalseAlarm);
	if(wndp=GetDlgItem(IDC_LEN2))wndp->EnableWindow(m_FalseAlarm);
	if(wndp=GetDlgItem(IDC_OFF2))wndp->EnableWindow(m_FalseAlarm);
	if(wndp=GetDlgItem(IDC_SUM2))wndp->EnableWindow(m_FalseAlarm);
	UpdateWindow();
}


BEGIN_MESSAGE_MAP(CDCA, CDialog)
	//{{AFX_MSG_MAP(CDCA)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_BN_CLICKED(IDC_FALSEALARM, OnFalsealarm)
	ON_BN_CLICKED(IDC_BUTTON_SUM_FILE, OnButtonSumFile)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCA message handlers


void CDCA::OnButtonSum() 
{
// Copy from CDFile!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	UpdateData();
	
	BYTE SubType=0;
	SubType|=(m_Com)? ST_COM: 0;
	SubType|=(m_Exe)? ST_EXE: 0;
	SubType|=(m_Sys)? ST_SYS: 0;
	SubType|=(m_NE)?  ST_NE: 0;
	SubType|=(m_Ole2)?ST_OLE2: 0;
	SubType|=(m_Script)?  ST_SCRIPT: 0;
	SubType|=(m_Hlp)?  ST_HLP: 0;
	


	int Off1=WorkAreaPage[m_Page1][0]+ m_Off1;
	int Off2=WorkAreaPage[m_Page2][0]+ m_Off2;
	DWORD disflags=0;
	
	
	if(m_Page1>=3){
		disflags|=0x100;
		disflags|=1;
/*
		Off1=m_Off1;
		CDOffset ofs;
		ofs.csno=1;
		ofs.m_Offset=Off1;
		if(IDOK==ofs.DoModal())
			Off1=ofs.m_Offset;
*/
	}
	if(m_Page2>=3){
		disflags|=0x200;
		disflags|=2;
/*
		Off2=m_Off2;
		CDOffset ofs;
		ofs.csno=2;
		ofs.m_Offset=Off2;
		if(IDOK==ofs.DoModal())
			Off2=ofs.m_Offset;
*/
	}
/*
	if(m_Ole2==TRUE)
		if(((CButton*)GetDlgItem(IDC_AUTO))->GetCheck()==1)	disflags|=4;
		else disflags|=8;
*/
	
	CDSum sd(m_Len1,m_Len2, Off1, Off2,
		SubType,this,1,(DWORD)-1,(DWORD)-1,disflags);
	
	if(IDOK!=sd.DoModal())return;

/*
	if(m_Ole2==TRUE){
		char tmp[20];
		sprintf(tmp,"%.4X ",sd.c_Off1);
		((CEdit*)GetDlgItem(IDC_OFF1))->SetWindowText(tmp);
		sprintf(tmp,"%.4X ",sd.c_Off2);
		((CEdit*)GetDlgItem(IDC_OFF2))->SetWindowText(tmp);
		sprintf(tmp,"%.4X ",sd.m_Len2);
		((CEdit*)GetDlgItem(IDC_LEN2))->SetWindowText(tmp);
		UpdateData();
	}
*/
	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);

	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;

	UpdateData(FALSE);

}
/////////////////////////////////////////////////////////////////////////////
// CDUnpack dialog


CDUnpack::CDUnpack(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDUnpack::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDUnpack)
	m_Comment = _T("");
	m_Name = _T("");
	m_Com = FALSE;
	m_Exe = FALSE;
	m_NE = FALSE;
	m_Ole2 = FALSE;
	m_Page1 = -1;
	m_Page2 = -1;
	m_Sys = FALSE;
	m_2Bytes=0;
	m_Len1=0;
	m_Len2=0;
	m_Off1=0;
	m_Off2=0;
	m_Sum1=0;
	m_Sum2=0;
	m_Script = FALSE;
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_Hlp = FALSE;
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDUnpack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDUnpack)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_Check(pDX, IDC_COM, m_Com);
	DDX_Check(pDX, IDC_EXE, m_Exe);
	DDX_Check(pDX, IDC_NE, m_NE);
	DDX_Check(pDX, IDC_OLE2, m_Ole2);
	DDX_CBIndex(pDX, IDC_PAGE1, m_Page1);
	DDX_CBIndex(pDX, IDC_PAGE2, m_Page2);
	DDX_Check(pDX, IDC_SYS, m_Sys);
	DDX_TextToWORD(pDX, IDC_2BYTES, m_2Bytes);
	DDX_TextToBYTE(pDX, IDC_LEN1, m_Len1);
	DDX_TextToBYTE(pDX, IDC_LEN2, m_Len2);
	DDX_TextToWORD(pDX, IDC_OFF1, m_Off1);
	DDX_TextToWORD(pDX, IDC_OFF2, m_Off2);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	DDX_TextToDWORD(pDX, IDC_SUM2, m_Sum2);
	DDX_Check(pDX, IDC_SCRIPT, m_Script);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_Check(pDX, IDC_HLP, m_Hlp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDUnpack, CDialog)
	//{{AFX_MSG_MAP(CDUnpack)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	ON_BN_CLICKED(IDC_BUTTON_SUM_FILE, OnButtonSumFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDUnpack message handlers


void CDUnpack::OnButtonSum() 
{
	UpdateData();

	BYTE SubType=0;
	SubType|=(m_Com)? ST_COM: 0;
	SubType|=(m_Exe)? ST_EXE: 0;
	SubType|=(m_Sys)? ST_SYS: 0;
	SubType|=(m_NE)?  ST_NE: 0;
	SubType|=(m_Ole2)?ST_OLE2: 0;
	SubType|=(m_Script)?  ST_SCRIPT: 0;
	SubType|=(m_Hlp)?  ST_HLP: 0;
	

	CDSum sd(m_Len1,m_Len2, 
		WorkAreaPage[m_Page1][0]+ m_Off1,  
		WorkAreaPage[m_Page2][0]+ m_Off2, SubType,this,1);


	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CDExtract dialog


CDExtract::CDExtract(CRecordEdit* reptr, CWnd* pParent /*=NULL*/)
	: CDForm(CDExtract::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDExtract)
	m_Comment = _T("");
	m_Name = _T("");
	m_Com = FALSE;
	m_Exe = FALSE;
	m_Ole2 = FALSE;
	m_Page1 = -1;
	m_Page2 = -1;
	m_Sys = FALSE;
	m_NE = FALSE;
	m_2Bytes=0;
	m_Len1=0;
	m_Len2=0;
	m_Off1=0;
	m_Off2=0;
	m_Sum1=0;
	m_Sum2=0;
	m_Script = FALSE;
	m_Link16 = _T("");
	m_Link32 = _T("");
	m_Hlp = FALSE;
	//}}AFX_DATA_INIT

	REptr=reptr;
}


void CDExtract::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDExtract)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 256);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 50);
	DDX_Check(pDX, IDC_COM, m_Com);
	DDX_Check(pDX, IDC_EXE, m_Exe);
	DDX_Check(pDX, IDC_OLE2, m_Ole2);
	DDX_CBIndex(pDX, IDC_PAGE1, m_Page1);
	DDX_CBIndex(pDX, IDC_PAGE2, m_Page2);
	DDX_Check(pDX, IDC_SYS, m_Sys);
	DDX_Check(pDX, IDC_NE, m_NE);
	DDX_TextToWORD(pDX, IDC_2BYTES, m_2Bytes);
	DDX_TextToBYTE(pDX, IDC_LEN1, m_Len1);
	DDX_TextToBYTE(pDX, IDC_LEN2, m_Len2);
	DDX_TextToWORD(pDX, IDC_OFF1, m_Off1);
	DDX_TextToWORD(pDX, IDC_OFF2, m_Off2);
	DDX_TextToDWORD(pDX, IDC_SUM, m_Sum1);
	DDX_TextToDWORD(pDX, IDC_SUM2, m_Sum2);
	DDX_Check(pDX, IDC_SCRIPT, m_Script);
	DDX_Text(pDX, IDC_LINK16, m_Link16);
	DDX_Text(pDX, IDC_LINK32, m_Link32);
	DDX_Check(pDX, IDC_HLP, m_Hlp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDExtract, CDialog)
	//{{AFX_MSG_MAP(CDExtract)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_SUM, OnButtonSum)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_BN_CLICKED(IDC_BUTTON_SUM_FILE, OnButtonSumFile)
	ON_BN_CLICKED(IDC_BUTTON_RELINK, OnButtonRelink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDExtract message handlers


void CDExtract::OnButtonSum() 
{
	UpdateData();

	BYTE SubType=0;
	SubType|=(m_Com)? ST_COM: 0;
	SubType|=(m_Exe)? ST_EXE: 0;
	SubType|=(m_Sys)? ST_SYS: 0;
	SubType|=(m_NE)?  ST_NE: 0;
	SubType|=(m_Ole2)?ST_OLE2: 0;
	SubType|=(m_Script)?  ST_SCRIPT: 0;
	SubType|=(m_Hlp)?  ST_HLP: 0;
	

	CDSum sd(m_Len1,m_Len2, 
		WorkAreaPage[m_Page1][0]+ m_Off1,  
		WorkAreaPage[m_Page2][0]+ m_Off2, SubType,this,1);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}




//!!!!!!!!!!!!!!


BOOL CDCA::OnInitDialog() 
{
	CDialog::OnInitDialog();

	p_2Bytes.SubclassEdit(IDC_2BYTES, this, "HHHH" );
	p_Len1.SubclassEdit(IDC_LEN1, this, "HH" );
	p_Len2.SubclassEdit(IDC_LEN2, this, "HH" );
	p_Off1.SubclassEdit(IDC_OFF1, this, "HHHH" );
	p_Off2.SubclassEdit(IDC_OFF2, this, "HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );
	p_Sum2.SubclassEdit(IDC_SUM2, this,"HHHHHHHH" );
	
	return TRUE; 
}

BOOL CDExtract::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	p_2Bytes.SubclassEdit(IDC_2BYTES, this, "HHHH" );
	p_Len1.SubclassEdit(IDC_LEN1, this, "HH" );
	p_Len2.SubclassEdit(IDC_LEN2, this, "HH" );
	p_Off1.SubclassEdit(IDC_OFF1, this, "HHHH" );
	p_Off2.SubclassEdit(IDC_OFF2, this, "HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );
	p_Sum2.SubclassEdit(IDC_SUM2, this,"HHHHHHHH" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




BOOL CDFile::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* w=GetDlgItem(IDC_METHOD);
	for(int i=0;i<DLL_GetMethodCount(1);i++){
		w->SendMessage(CB_ADDSTRING,0,(LPARAM)DLL_GetMethodText(1,i)[0]);
	}
	w->SendMessage(CB_SETCURSEL,m_Method,0);
	OnSelchangeMethod();
//	((CButton*)GetDlgItem(IDC_AUTO))->ShowWindow((((CButton*)GetDlgItem(IDC_OLE2))->GetCheck()==1)?SW_SHOW:SW_HIDE);

	p_2Bytes.SubclassEdit(IDC_2BYTES, this,"HHHH" );
	p_Data1.SubclassEdit(IDC_DATA1, this,"-HHHH" );
	p_Data2.SubclassEdit(IDC_DATA2, this,"-HHHH" );
	p_Data3.SubclassEdit(IDC_DATA3, this,"-HHHH" );
	p_Data4.SubclassEdit(IDC_DATA4, this,"-HHHH" );
	p_Data5.SubclassEdit(IDC_DATA5, this,"-HHHH" );
	p_Len1.SubclassEdit(IDC_LEN1, this,"HH" );
	p_Len2.SubclassEdit(IDC_LEN2, this,"HH" );
	p_Off1.SubclassEdit(IDC_OFF1, this,"HHHH" );
	p_Off2.SubclassEdit(IDC_OFF2, this,"HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );
	p_Sum2.SubclassEdit(IDC_SUM2, this,"HHHHHHHH" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDJump::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* w=GetDlgItem(IDC_METHOD);
	for(int i=0;i<DLL_GetMethodCount(0);i++){
		w->SendMessage(CB_ADDSTRING,0,(LPARAM)DLL_GetMethodText(0,i)[0]);
	}
	w->SendMessage(CB_SETCURSEL,m_Method,0);
	OnSelchangeMethod();
	
	p_2Bytes.SubclassEdit(IDC_2BYTES, this,"HHHH" );
	p_Data1.SubclassEdit(IDC_DATA1, this,"HHHH" );
	p_Data2.SubclassEdit(IDC_DATA2, this,"HHHH" );
	p_Data3.SubclassEdit(IDC_DATA3, this,"-HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDMemory::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* w=GetDlgItem(IDC_METHOD);
	char** ptr=DLL_GetMethodText(3,0);
	for(int i=0;i<DLL_GetMethodCount(3);i++){
		w->SendMessage(CB_ADDSTRING,0,(LPARAM)ptr[i]);
	}
	w->SendMessage(CB_SETCURSEL,m_Method,0);
	OnSelchangeMethod();
	
	p_Offset.SubclassEdit(IDC_OFFSET, this,"HHHH" );
	p_Byte.SubclassEdit(IDC_BYTE, this,"HH" );
	p_Len.SubclassEdit(IDC_LEN, this,"HH" );
	p_Len2.SubclassEdit(IDC_LEN2, this,"HH" );
	p_Off2.SubclassEdit(IDC_OFF2, this,"HHHH" );
	p_Replace.SubclassEdit(IDC_REPLACE, this,"HH HH HH HH HH" );
	p_Segment.SubclassEdit(IDC_SEGMENT, this,"HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDSector::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* w1=GetDlgItem(IDC_METHOD1);
	CWnd* w2=GetDlgItem(IDC_METHOD2);
	for(int i=0;i<DLL_GetMethodCount(2);i++){
		w1->SendMessage(CB_ADDSTRING,0,(LPARAM)DLL_GetMethodText(2,i)[0]);
		w2->SendMessage(CB_ADDSTRING,0,(LPARAM)DLL_GetMethodText(2,i)[0]);
	}
	w1->SendMessage(CB_SETCURSEL,m_Method1,0);
	w2->SendMessage(CB_SETCURSEL,m_Method2,0);

	OnSelchangeMethod1();
	OnSelchangeMethod2();
	
	p_2Bytes.SubclassEdit(IDC_2BYTES, this,"HHHH" );
	p_Addr11.SubclassEdit(IDC_ADDRESS11, this,"HHHH" );
	p_Addr12.SubclassEdit(IDC_ADDRESS12, this,"HHHH" );
	p_Addr21.SubclassEdit(IDC_ADDRESS21, this,"HHHH" );
	p_Addr22.SubclassEdit(IDC_ADDRESS22, this,"HHHH" );
	p_Data1.SubclassEdit(IDC_DATA1, this,"-HH" );
	p_Data2.SubclassEdit(IDC_DATA2, this,"-HH" );
	p_Len1.SubclassEdit(IDC_LEN1, this,"HH" );
	p_Len2.SubclassEdit(IDC_LEN2, this,"HH" );
	p_Off1.SubclassEdit(IDC_OFF1, this,"HHHH" );
	p_Off2.SubclassEdit(IDC_OFF2, this,"HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );
	p_Sum2.SubclassEdit(IDC_SUM2, this,"HHHHHHHH" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDUnpack::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	p_2Bytes.SubclassEdit(IDC_2BYTES, this, "HHHH" );
	p_Len1.SubclassEdit(IDC_LEN1, this, "HH" );
	p_Len2.SubclassEdit(IDC_LEN2, this, "HH" );
	p_Off1.SubclassEdit(IDC_OFF1, this, "HHHH" );
	p_Off2.SubclassEdit(IDC_OFF2, this, "HHHH" );
	p_Sum.SubclassEdit(IDC_SUM, this,"HHHHHHHH" );
	p_Sum2.SubclassEdit(IDC_SUM2, this,"HHHHHHHH" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDSum dialog


CDSum::CDSum(int len1, int len2, int off1, int off2, BYTE st ,  CWnd* pParent /*=NULL*/, int autofill, DWORD mask1, DWORD mask2, DWORD disflags)
	: CDialog(CDSum::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDSum)
	//}}AFX_DATA_INIT
	m_Disflags=disflags;
	m_Auto=autofill;
	m_Mask1=mask1;
	m_Mask2=mask2;
	m_Len1=len1;
	m_Len2=len2;
	m_Off1=off1;
	m_Off2=off2;
	SubType=st;
/*	
	if(m_Off1>=HEADER_SIZE) 
		if(m_Off1>=(HEADER_SIZE+PAGE_A_SIZE)) 
			if(m_Off1>=(HEADER_SIZE+PAGE_A_SIZE+PAGE_B_SIZE)) c_Off1=off1-(HEADER_SIZE+PAGE_A_SIZE+PAGE_B_SIZE);
			else c_Off1=off1-(HEADER_SIZE+PAGE_A_SIZE);
		else c_Off1=off1-HEADER_SIZE;
	else c_Off1=off1;

	if(m_Off2>=HEADER_SIZE) 
		if(m_Off2>=(HEADER_SIZE+PAGE_A_SIZE)) 
			if(m_Off2>=(HEADER_SIZE+PAGE_A_SIZE+PAGE_B_SIZE)) c_Off2=off2-(HEADER_SIZE+PAGE_A_SIZE+PAGE_B_SIZE);
			else c_Off2=off2-(HEADER_SIZE+PAGE_A_SIZE);
		else c_Off2=off2-(HEADER_SIZE);
	else c_Off2=off2;
*/
	m_Buf1 = NULL;
	m_Buf2 = NULL;
	
	
	if(m_Disflags&1)
		m_Buf1=len1?(new BYTE[len1]):NULL;
	else	
		m_Buf1=AVP_GetPagePtr()+m_Off1;
	
	if(m_Disflags&2)
		m_Buf2=len2?(new BYTE[len2]):NULL;
	else
		m_Buf2=AVP_GetPagePtr()+m_Off2;
/*
	if(m_Len1)memset(m_Buf1,0,m_Len1);
	if(m_Len2)memset(m_Buf2,0,m_Len2);
*/
}
CDSum::~CDSum()
{	
	if((m_Disflags&1) && m_Buf1)delete m_Buf1;
	if((m_Disflags&2) && m_Buf2)delete m_Buf2;
}

void CDSum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_TextToBuffer(pDX, IDC_AREA1, m_Buf1, m_Len1);
	DDX_TextToBuffer(pDX, IDC_AREA2, m_Buf2, m_Len2);
	
	if(p_Area1.m_hWnd)p_Area1.Check();
	if(p_Area2.m_hWnd)p_Area2.Check();

	//{{AFX_DATA_MAP(CDSum)
	//}}AFX_DATA_MAP
}
 


BEGIN_MESSAGE_MAP(CDSum, CDialog)
	//{{AFX_MSG_MAP(CDSum)
	ON_BN_CLICKED(IDC_AUTOFILL, OnAutofill)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSum message handlers
/*
void CDSum::OnAutofill() 
{
	CString name=GetSumName(this);
	if(name.GetLength()){
		if((m_Disflags&3) != 3){
			AVPScanObject ScanObject;
			ScanObject.Mode=Entry;
			strcpy(ScanObject.Name,name);
			AVP_Dispatch(&ScanObject);
		}
		CFile f;
		f.Open(name,CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone);
		if((m_Disflags&1) && m_Len1){
			f.Seek(m_Off1,CFile::begin);
			f.Read(m_Buf1,m_Len1);
		}
		if((m_Disflags&2) && m_Len2){
			f.Seek(m_Off2,CFile::begin);
			f.Read(m_Buf2,m_Len2);
		}
		f.Close();
		UpdateData(FALSE);
		
		if(p_Area1.m_hWnd)p_Area1.SetSel(0xFFFF0000);
		if(p_Area2.m_hWnd)p_Area2.SetSel(0xFFFF0000);

	}
}
*/

BOOL save_pages=0;
CString PageFileName;

void CDSum::OnAutofill()
{
	CString name=GetSumName(this);
	if(name.GetLength()){
		CFile f;
/*
		char* p;
		char pa[512];
		GetFullPathName(name,512,pa,&p);


		CString nam=MainFramePtr->ScanDialog.m_PathSave;
		if(nam.Right(1)!="\\")nam+="\\";
		nam+=p;
		nam+=".pag";
*/
/*		if(MainFramePtr->ScanDialog.m_PathSave.Right(1)!="\\")MainFramePtr->ScanDialog.m_PathSave+="\\";
		CString nam=MainFramePtr->ScanDialog.m_PathSave;
		CString n=WorkArea->ScanObject->Name;
		nam+=((const char*)n) + n.ReverseFind('\\') + 1;
		nam.Replace('/','.');
		nam+=".0.pag";
*/

//		int delpag=0;
		CString save_nam=PageFileName;

		if((m_Disflags&1) && m_Len1){
			CString n=name;
			if((m_Disflags&0x100)){
//				if(_access(nam,4))delpag=1;
				AVPScanObject so;
				so.Type=OT_FILE;
				memset(&so,0,sizeof(AVPScanObject));
				so.MFlags=2 | MF_CALLBACK_SAVE;
				strcpy(so.Name,name);
				save_pages=1;
				AVP_ProcessObject(&so);
				save_pages=0;

				if(0==_access(PageFileName,4))
					n=PageFileName;
/*
				if(0==_access(nam,4))
					n=nam;
*/
				else
					MessageBox("Processing of Page_C failed.\nFile used instead.");
			}
			if(f.Open(n,CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone))
			{
				if(f.GetLength()<(unsigned)(m_Off1+m_Len1))
				{
					memset(m_Buf1,0,m_Len1);
					MessageBox("Buf1 read failed: EOF.");
				}
				else{
					f.Seek(m_Off1,CFile::begin);
					f.Read(m_Buf1,m_Len1);
				}
				f.Close();
			}
		}

		if((m_Disflags&2) && m_Len2){
			CString n=name;
			if((m_Disflags&0x200)){
				if(!(m_Disflags&0x100))
				{
//				if(_access(nam,4))delpag=1;
				AVPScanObject so;
				so.Type=OT_FILE;
				memset(&so,0,sizeof(AVPScanObject));
				so.MFlags=2 | MF_CALLBACK_SAVE;
				strcpy(so.Name,name);
				save_pages=1;
				AVP_ProcessObject(&so);
				save_pages=0;
				}
				if(0==_access(PageFileName,4))
					n=PageFileName;
/*
				if(0==_access(nam,4))
					n=nam;
*/
				else
					MessageBox("Processing of Page_C failed.\nFile used instead.");
			}
			if(f.Open(n,CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone))
			{
				if(f.GetLength()<(unsigned)(m_Off2+m_Len2))
				{
					memset(m_Buf2,0,m_Len2);
					MessageBox("Buf2 read failed: EOF.");
				}
				else{
					f.Seek(m_Off2,CFile::begin);
					f.Read(m_Buf2,m_Len2);
				}
				f.Close();
			}
		}
		if(save_nam!=PageFileName){
			DeleteFile(PageFileName);
			PageFileName="";
		}
//		if(delpag)DeleteFile(nam);

		if((m_Disflags&3) != 3){
			AVPScanObject ScanObject;
			memset(&ScanObject,0,sizeof(AVPScanObject));
			strcpy(ScanObject.Name,name);
			AVP_Dispatch(&ScanObject, SubType);
		}
		
		UpdateData(FALSE);
		
		if(p_Area1.m_hWnd)p_Area1.SetSel(0xFFFF0000);
		if(p_Area2.m_hWnd)p_Area2.SetSel(0xFFFF0000);

	}
}

BOOL CDSum::OnInitDialog() 
{
	CRect pos;
	GetParent()->GetWindowRect(&pos);
	CRect p;
	GetWindowRect(&p);
	SetWindowPos(0,pos.left,pos.bottom-(p.bottom-p.top),0,0,
		SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);

	CDialog::OnInitDialog();
	CString s;
	int i;
	if(m_Len1){
		s.Empty();
		for(i=0;i<m_Len1;i++)
		{
			if(i)s+=" ";
			if(i<32){
				if((m_Mask1 & (1<<i))==0){
					s+="SS";
					continue;
				}
			}
			else if(i<64){
				if((m_Mask2 & (1<<(i-32)))==0){
					s+="SS";
					continue;
				}
			}
			s+="HH";
		}
		p_Area1.SubclassEdit(IDC_AREA1, this, (char*)(const char*)s );
		p_Area1.EnableWindow();

	}
	if(m_Len2){
		s.Empty();
		for(i=0;i<m_Len2;i++)	
		{
			if(i)s+=" ";
			if(i<32){
				if((m_Mask1 & (1<<i))==0){
					s+="SS";
					continue;
				}
			}
			else if(i<64){
				if((m_Mask2 & (1<<(i-32)))==0){
					s+="SS";
					continue;
				}
			}
			s+="HH";
		}
		p_Area2.SubclassEdit(IDC_AREA2, this, (char*)(const char*)s );
		p_Area2.EnableWindow();
	}

	if(m_Auto)OnAutofill();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDFile::OnSelchangeMethod() 
{
	CWnd* w=GetDlgItem(IDC_METHOD);
	int i=w->SendMessage(CB_GETCURSEL,0,0);
	char** ptr=DLL_GetMethodText(1,i);
	if(ptr==NULL)return;
	SetDlgItemText(IDC_LABEL_DATA1,ptr[1]+((*ptr[1])?1:0));
	SetDlgItemText(IDC_LABEL_DATA2,ptr[2]+((*ptr[2])?1:0));
	SetDlgItemText(IDC_LABEL_DATA3,ptr[3]+((*ptr[3])?1:0));
	SetDlgItemText(IDC_LABEL_DATA4,ptr[4]+((*ptr[4])?1:0));
	SetDlgItemText(IDC_LABEL_DATA5,ptr[5]+((*ptr[5])?1:0));
}

void CDJump::OnSelchangeMethod() 
{
	CWnd* w=GetDlgItem(IDC_METHOD);
	int i=w->SendMessage(CB_GETCURSEL,0,0);
	char** ptr=DLL_GetMethodText(0,i);
	if(ptr==NULL)return;
	SetDlgItemText(IDC_LABEL_DATA1,ptr[1]);
	SetDlgItemText(IDC_LABEL_DATA2,ptr[2]);
	SetDlgItemText(IDC_LABEL_DATA3,ptr[3]);
}

void CDMemory::OnSelchangeMethod() 
{
	
}

void CDSector::OnSelchangeMethod1() 
{
	CWnd* w=GetDlgItem(IDC_METHOD1);
	int i=w->SendMessage(CB_GETCURSEL,0,0);
	char** ptr=DLL_GetMethodText(2,i);
	if(ptr==NULL)return;
	SetDlgItemText(IDC_LABEL_DATA11,ptr[1]+((*ptr[1])?1:0));
	SetDlgItemText(IDC_LABEL_DATA12,ptr[2]+((*ptr[2])?1:0));
	SetDlgItemText(IDC_LABEL_DATA1,ptr[3]+((*ptr[3])?1:0));
}

void CDSector::OnSelchangeMethod2() 
{
	CWnd* w=GetDlgItem(IDC_METHOD2);
	int i=w->SendMessage(CB_GETCURSEL,0,0);
	char** ptr=DLL_GetMethodText(2,i);
	if(ptr==NULL)return;
	SetDlgItemText(IDC_LABEL_DATA21,ptr[1]+((*ptr[1])?1:0));
	SetDlgItemText(IDC_LABEL_DATA22,ptr[2]+((*ptr[2])?1:0));
	SetDlgItemText(IDC_LABEL_DATA2,ptr[3]+((*ptr[3])?1:0));
}


void CDCA::OnFalsealarm() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
// CDForm dialog


CDForm::CDForm(UINT idd, CWnd* pParent /*=NULL*/)
	: CDialog(idd, pParent)
{
	m_Modified=TRUE;
	//{{AFX_DATA_INIT(CDForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDForm, CDialog)
	//{{AFX_MSG_MAP(CDForm)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDForm message handlers
void CDForm::OnCancel() 
{
	if(OptDlg.m_EscSaveRec)
	{
		if(m_Modified)switch(AfxMessageBox("Save record?",MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION))
		{
		case IDYES: 
			PostMessage(WM_COMMAND,BN_CLICKED|IDOK,NULL);
		case IDNO:
			break;
		default:
			return;
		}
	}
	CDialog::OnCancel();
}

void CDFile::OnOK() 
{
	UpdateData();
	if(m_NE|m_Ole2|m_Sys|m_Com|m_Exe|m_NE|m_Script|m_Hlp)
		CDForm::OnOK();
	else AfxMessageBox("You must specify at least one object.");
}

void CDCA::OnOK() 
{
	UpdateData();
	if(m_NE|m_Ole2|m_Sys|m_Com|m_Exe|m_NE|m_Script|m_Hlp)
		CDForm::OnOK();
	else AfxMessageBox("You must specify at least one object.");
}
/////////////////////////////////////////////////////////////////////////////
// CDOffset dialog


CDOffset::CDOffset(CWnd* pParent /*=NULL*/)
	: CDialog(CDOffset::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDOffset)
	m_Offset = 0;
	//}}AFX_DATA_INIT
	csno=0;
}


void CDOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDOffset)
	DDX_TextToDWORD(pDX, IDC_EDIT1, m_Offset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDOffset, CDialog)
	//{{AFX_MSG_MAP(CDOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDOffset message handlers

BOOL CDOffset::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(csno==1)	SetWindowText("Check Sum 1");
	if(csno==2)	SetWindowText("Check Sum 2");
	p_Offset.SubclassEdit(IDC_EDIT1, this,"HHHHHHHH" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////


/************************************************************************/

void CDCA::OnButtonSumFile() 
{
	UpdateData();

	CDSum sd(m_Len1,m_Len2, 
		m_Off1,m_Off2, 0,this,1, -1,-1, 3);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}

void CDExtract::OnButtonSumFile() 
{
	UpdateData();

	CDSum sd(m_Len1,m_Len2, 
		m_Off1,m_Off2, 0,this,1, -1,-1, 3);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}

void CDFile::OnButtonSumFile() 
{
	UpdateData();

	CDSum sd(m_Len1,m_Len2, 
		m_Off1,m_Off2, 0,this,1, -1,-1, 3);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}

void CDSector::OnButtonSumFile() 
{
	UpdateData();

	CDSum sd(m_Len1,m_Len2, 
		m_Off1,m_Off2, 0,this,1, -1,-1, 3);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}

void CDUnpack::OnButtonSumFile() 
{
	UpdateData();

	CDSum sd(m_Len1,m_Len2, 
		m_Off1,m_Off2, 0,this,1, -1,-1, 3);

	if(IDOK!=sd.DoModal())return;

	m_Sum1=CalcSum(sd.m_Buf1,sd.m_Len1);
	m_Sum2=CalcSum(sd.m_Buf2,sd.m_Len2);
	if(m_Len1==1)m_2Bytes=*(BYTE*)sd.m_Buf1;
	else if(m_Len1)	m_2Bytes=*(WORD*)sd.m_Buf1;
	else m_2Bytes=0;
	UpdateData(FALSE);
}

/*************************************************************************/

#if (!defined (KL_MULTIPLATFORM_LINKS))
void CDForm::OnButtonLink()
{
	BYTE b=REptr->AddLink(GetObjName(this,REptr->Name));

	if(b){
		UpdateData();
		m_Link16=REptr->GetLink16String();
		m_Link32=REptr->GetLink32String();
		UpdateData(FALSE);

		MainFramePtr->SetMessageText("Linked.");

	}
	else AfxMessageBox("Entry point not defined.");
}

void CDForm::OnButtonUnlink()
{
	if(REptr->Unlink())
	{
		UpdateData();
		m_Link16=REptr->GetLink16String();
		m_Link32=REptr->GetLink32String();
		UpdateData(FALSE);

		MainFramePtr->SetMessageText("Unlinked.");

	}

}

void CDForm::OnButtonRelink()
{
	CString s;
	BYTE b=REptr->AddLink(GetObjNameCurrent(this,REptr->Name),1); //noconfirm

	if(b){
		UpdateData();
		m_Link16=REptr->GetLink16String();
		m_Link32=REptr->GetLink32String();
		UpdateData(FALSE);

		MainFramePtr->SetMessageText("Relinked.");
	}
	else AfxMessageBox("Entry point not defined.");
}
#else
void CDForm::OnButtonLink()
{
    CLinkDlg linkEditorDlg (REptr, false, this);

	if(linkEditorDlg.DoModal() == IDOK)
    {
		UpdateData();
		m_Link16=REptr->GetLink16String();
		m_Link32=REptr->GetLink32String();
		UpdateData(FALSE);

		MainFramePtr->SetMessageText("Linked.");
	}
}

void CDForm::OnButtonUnlink()
{
	if(REptr->Unlink())
	{
		UpdateData();
		m_Link16=REptr->GetLink16String();
		m_Link32=REptr->GetLink32String();
		UpdateData(FALSE);

		MainFramePtr->SetMessageText("Unlinked.");
	}
}

void CDForm::OnButtonRelink()
{
    CLinkDlg linkEditorDlg (REptr, true, this);

	if(linkEditorDlg.DoModal() == IDOK){
		UpdateData();
		m_Link16=REptr->GetLink16String();
		m_Link32=REptr->GetLink32String();
		UpdateData(FALSE);

		MainFramePtr->SetMessageText("Relinked.");
	}
}
#endif
/*************************************************************************/

void CDFile::OnOle2() 
{

//	((CButton*)GetDlgItem(IDC_AUTO))->ShowWindow((((CButton*)GetDlgItem(IDC_OLE2))->GetCheck()==1)?SW_SHOW:SW_HIDE);

	if(((CButton*)GetDlgItem(IDC_OLE2))->GetCheck()==1)
	{
//		((CButton*)GetDlgItem(IDC_AUTO))->SetCheck(1);

		((CButton*)GetDlgItem(IDC_COM))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_EXE))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_SYS))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_NE))->SetCheck(0);
		((CComboBox*)GetDlgItem(IDC_METHOD))->SetCurSel(FILE_OLE2);
		((CComboBox*)GetDlgItem(IDC_PAGE1))->SetCurSel(2);
		((CComboBox*)GetDlgItem(IDC_PAGE2))->SetCurSel(2);

		char tmp[20];
		((CEdit*)GetDlgItem(IDC_DATA1))->GetWindowText(tmp,20);
		if(!strcmp(tmp,"0000"))
			((CEdit*)GetDlgItem(IDC_DATA1))->SetWindowText("0001");
	}
}


