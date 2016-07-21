// ScanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AvpEdit.h"
#include "MainFrm.h"
//#include "ScanDlg.h"
#include <Stuff/layout.h>
#include "OptDlg.h"
extern COptDlg OptDlg;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../avp_32/ScanDir.h"
#include <ScanAPI/ScanObj.h>
#include <ScanAPI/avp_dll.h>
#include <ScanAPI/avp_msg.h>
#include <Bases/sizes.h>
#include "../basework/workarea.h"
#include <Bases/Retcode3.h>
#include <IOCache/TFileData.h>

const UINT	 wm_AVP = RegisterWindowMessage( AVP_MSGSTRING );
extern CMainFrame* MainFramePtr;
extern CString PageFileName;
BOOL extern save_pages;

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog


CScanDialog::CScanDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScanDialog::IDD, pParent)
{
	CanCancel=0;
	KillScan=0;
	ScanThreadPtr=NULL;
	//{{AFX_DATA_INIT(CScanDialog)
	m_Path = _T("");
	m_CA = TRUE;
	m_AllWarnings = TRUE;
	m_AllEntry = TRUE;
	m_SavePages = FALSE;
	m_Redundant = FALSE;
	m_SavePacked = FALSE;
	m_SaveArchived = FALSE;
	m_Archived = FALSE;
	m_Mail = FALSE;
	m_Packed = FALSE;
	m_Action = -1;
	m_Recurse = FALSE;
	m_PathSave = _T("");
	//}}AFX_DATA_INIT
	m_Path = AfxGetApp()->GetProfileString("Options","PathScan","");
	m_PathSave = AfxGetApp()->GetProfileString("ScanDialog","PathSave","");
	m_CA = AfxGetApp()->GetProfileInt("ScanDialog","CodeAnalyzer",1);
	m_AllWarnings = AfxGetApp()->GetProfileInt("ScanDialog","AllWarnings",1);
	m_AllEntry = AfxGetApp()->GetProfileInt("ScanDialog","AllEntries",1);
	m_SavePages = AfxGetApp()->GetProfileInt("ScanDialog","SavePages",0);
	m_Redundant = AfxGetApp()->GetProfileInt("ScanDialog","Redundant",0);

	m_SavePacked = AfxGetApp()->GetProfileInt("ScanDialog","SavePacked",0);
	m_SaveArchived = AfxGetApp()->GetProfileInt("ScanDialog","SaveArchived",0);
	m_Archived = AfxGetApp()->GetProfileInt("ScanDialog","Archived",1);
	m_Mail = AfxGetApp()->GetProfileInt("ScanDialog","Mail",1);
	m_Packed = AfxGetApp()->GetProfileInt("ScanDialog","Packed",1);
	m_Action = AfxGetApp()->GetProfileInt("ScanDialog","Action",0);
	m_Recurse = AfxGetApp()->GetProfileInt("ScanDialog","Recurse",0);
	
}

void CScanDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanDialog)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Text(pDX, IDC_PATH, m_Path);
	DDX_Check(pDX, IDC_CA, m_CA);
	DDX_Check(pDX, IDC_ALL_WARNINGS, m_AllWarnings);
	DDX_Check(pDX, IDC_ALL_ENTRY, m_AllEntry);
	DDX_Check(pDX, IDC_SAVEPAGES, m_SavePages);
	DDX_Check(pDX, IDC_REDUNDANT, m_Redundant);
	DDX_Check(pDX, IDC_SAVE_PACKED, m_SavePacked);
	DDX_Check(pDX, IDC_SAVE_ARCHIVED, m_SaveArchived);
	DDX_Check(pDX, IDC_ARCHIVED, m_Archived);
	DDX_Check(pDX, IDC_MAIL, m_Mail);
	DDX_Check(pDX, IDC_PACKED, m_Packed);
	DDX_Radio(pDX, IDC_DISINFECT, m_Action);
	DDX_Check(pDX, IDC_RECURSE, m_Recurse);
	DDX_Text(pDX, IDC_PATH_SAVE, m_PathSave);
	//}}AFX_DATA_MAP
}

HACCEL acc;
extern COptDlg OptDlg;
extern CFont cf;

BOOL CScanDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(OptDlg.m_FontName.GetLength()){
		cf.CreatePointFont(OptDlg.m_FontSize,OptDlg.m_FontName);
		m_List.SetFont(&cf);
	}

	ListView_SetExtendedListViewStyle(m_List.m_hWnd, ListView_GetExtendedListViewStyle(m_List.m_hWnd)|LVS_EX_FULLROWSELECT);
	
	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	lvcolumn.pszText = "Filename";
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 400;
	m_List.InsertColumn(0, &lvcolumn);

	lvcolumn.pszText = "Result";
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = 270;
	m_List.InsertColumn(1, &lvcolumn);

	AVP_RegisterCallback(m_hWnd);

	acc=LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CScanDialog, CDialog)
	//{{AFX_MSG_MAP(CScanDialog)
	ON_BN_CLICKED(IDC_SCAN, OnScan)
    ON_REGISTERED_MESSAGE( wm_AVP, OnAVPMessage )
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FINDAGAIN, OnEditFindagain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDialog message handlers
UINT CScanDialog::CheckMBRS(DWORD MFlags)
{
	DWORD ret=0;
	for(int i=0;i<16;i++)
	{
		if(KillScan)break;
		CString Name;
		AVPScanObject ScanObject;
		memset(&ScanObject,0,sizeof(AVPScanObject));
		ScanObject.SType=OT_MBR;
		ScanObject.Size=512; 
		ScanObject.MFlags=MFlags;
		ScanObject.MFlags|=MF_ALLENTRY;
		ScanObject.Drive=2;
		
		Name.Format("MBR of HDD%d",i+1);
		strcpy(ScanObject.Name,Name);
		ScanObject.Disk=0x80+i;

		ret&=AVP_ProcessObject(&ScanObject);
	}
	return ret;
}


UINT CScanDialog::CheckBoot(CString& path, DWORD MFlags)
{
	AVPScanObject ScanObject;
	if(path[1]!=':')return 0;
	CString Name="Boot sector of drive";
	Name+=" ";
	Name+=path.Left(2);

	memset(&ScanObject,0,sizeof(AVPScanObject));
	ScanObject.SType=OT_BOOT; 
	ScanObject.Size=512; 
	ScanObject.MFlags=MFlags;

	strcpy(ScanObject.Name,Name);
	ScanObject.Drive=(path[0]|0x20)-'a';
	ScanObject.Disk=ScanObject.Drive;

	switch(GetDriveType(path+"\\"))
	{
	case DRIVE_REMOTE:
		ScanObject.Disk=0x20; return 0;
	case DRIVE_RAMDISK:
		ScanObject.Disk=0x40;  return 0;//break;
	case DRIVE_CDROM:
		ScanObject.Disk=0x60 | ScanObject.Drive;  return 0;//break;
	case DRIVE_FIXED:  return 0;
//		CheckMBRS(MFlags);
//		ScanObject.Disk=0x80; break;
	case DRIVE_REMOVABLE:
	default:
		ScanObject.Disk=ScanObject.Drive;	break;
		return 0;
	}
	return AVP_ProcessObject(&ScanObject);
}


UINT Scan( LPVOID pParam )
{
	UINT ret=((CScanDialog*)pParam)->ScanPath();
	return	ret;
}


UINT CScanDialog::ScanPath() 
{
	KillScan=0;
	UINT ret=0;
	
	DWORD MFlags=2; //All Files
	MFlags|=MF_WARNINGS;
	if(m_Archived)		MFlags|=MF_ARCHIVED;
	if(m_Packed)		MFlags|=MF_PACKED;
	if(m_Mail)			MFlags|=MF_MAILBASES;
	if(m_Mail)			MFlags|=MF_MAILPLAIN;
	if(m_CA)			MFlags|=MF_CA;
	if(m_AllEntry)		MFlags|=MF_ALLENTRY;
	if(m_AllWarnings)	MFlags|=MF_ALLWARNINGS;
	if(m_SavePages)		MFlags|=MF_CALLBACK_SAVE;
	if(m_SavePacked)	MFlags|=MF_CALLBACK_SAVE;
	if(m_SaveArchived)	MFlags|=MF_CALLBACK_SAVE;
	if(m_Redundant)		MFlags|=MF_REDUNDANT;

	if(m_Path.Mid(1)==":"){
		CheckBoot(m_Path,MFlags);
	}

	CScanDir SD(m_Path);

	if(!KillScan) ret=SD.Dispatch(this,&KillScan,MFlags,m_Recurse);

	GetDlgItem(IDC_SCAN)->SetWindowText("&Scan");
	MainFramePtr->SetMessageText(KillScan?"Scan pocess was interrupted.":"Scan pocess is complete.");
	ScanThreadPtr=NULL;
	return ret;
}


void CScanDialog::OnScan() 
{
	if(ScanThreadPtr){
		OnCancel();
		return;
	}
	UpdateData();

	if(!m_Path.GetLength())
		return;
	if(m_Action && IDYES!=AfxMessageBox(m_Action==1?"Really DISINFECT ALL?":"Really DELETE ALL?",MB_YESNO|MB_ICONQUESTION))
		return;
	
	m_Path.MakeLower();

	if(m_Path[m_Path.GetLength()-1]=='\\')
		m_Path=m_Path.Left(m_Path.GetLength()-1);
	
	m_List.DeleteAllItems();

	GetDlgItem(IDC_SCAN)->SetWindowText("&Stop");
	SetDefID(IDC_SCAN);

	AVP_RegisterCallback(m_hWnd);
	ScanThreadPtr=AfxBeginThread(Scan,this);
	m_List.SetFocus();
}

void CScanDialog::OnCancel()
{
	if(	ScanThreadPtr){
		ScanThreadPtr->SuspendThread();
		if(IDYES==AfxMessageBox("Stop scan process?",MB_YESNO|MB_ICONQUESTION))
		{
			AVP_CancelProcessObject(1); 
			KillScan=1;
		}
		ScanThreadPtr->ResumeThread();
		return;
	}
	ShowWindow(SW_HIDE);
//	GetOwner()->SetForegroundWindow();
}

void CScanDialog::OnClose() 
{
	if(ScanThreadPtr){
		AVP_CancelProcessObject(1);
		KillScan=1;
		PostMessage(WM_CLOSE);
		return;
	}

	CDialog::OnClose();
}

char* If_MakeFullName(char* tempb, char* VirusName)
{
	AVP_MakeFullName(tempb, VirusName);
	if(OptDlg.m_MakeFullName)
	{
		strcat(tempb, " (");
		strcat(tempb, VirusName);
		strcat(tempb, ")");
	}
	return tempb;
}


class OMS
{
public:
	CString Name;
	CString Message;
};

void CScanDialog::ObjectMessage(void* SO,const char* s) 
{
	AVPScanObject* ScanObject=(AVPScanObject*)SO;
	OMS* oms=new OMS;
	oms->Message=s;
	oms->Name=ScanObject->Name;
	if(ScanObject->Entry){
		oms->Name.Format("%s  Entry #%d",ScanObject->Name,ScanObject->Entry);
	}
	PostMessage(wm_AVP, AVP_INTERNAL_MESSAGE,(long)oms);
}

LONG CScanDialog::OnAVPMessage( UINT uParam, LONG lParam){
	static LOAD_PROGRESS_STRUCT lps;
	LONG ret=0;
	CString s;
	static CString ObjName;
//	int i;
	char tempb[0x200];
	AVPScanObject* ScanObject=(AVPScanObject*)lParam;

	switch(uParam){
	case AVP_CALLBACK: 
//		h_AVP_KERNEL = (HWND)lParam;
//		ret=1;
		break;
		
	case AVP_CALLBACK_MB_OK:
		return MessageBox((char*)lParam,"AVP Editor");

	case AVP_CALLBACK_MB_OKCANCEL:
		return MessageBox((char*)lParam,"AVP Editor",MB_OKCANCEL);

	case AVP_CALLBACK_MB_YESNO:
		return MessageBox((char*)lParam,"AVP Editor",MB_YESNO);

	case AVP_CALLBACK_PUT_STRING: 
		ShowWindow(SW_SHOW);
		PutLogString((char*)lParam,"");
		break;

	case AVP_CALLBACK_ROTATE: 
	case AVP_CALLBACK_ROTATE_OFF: 
		break;

	case AVP_CALLBACK_LOAD_PROGRESS:
		{
			memcpy(&lps,(LOAD_PROGRESS_STRUCT*)lParam,sizeof(LOAD_PROGRESS_STRUCT));
			if(lps.curLength){
				s.Format("Load base %s %d%%",lps.curName,lps.curPos*100/lps.curLength);
				MainFramePtr->SetMessageText(s);
				MainFramePtr->UpdateWindow();
			}
		}
		break;

	case AVP_INTERNAL_MESSAGE:
		{
			OMS* oms=(OMS*)lParam;
			PutLogString(oms->Name,oms->Message);
			delete oms;
		}
		break;

	case AVP_CALLBACK_OBJ_NAME:
		if(m_SavePages || OptDlg.m_SkipPag)
		{
			CString nam;
			nam=(char*)lParam;
			nam=nam.Right(4);
			nam.MakeLower();
			if(nam==".pag"){
				ret=17;
				break;
			}
		}
		if(m_SavePacked || m_SaveArchived || OptDlg.m_SkipPag)
		{
			CString nam;
			nam=(char*)lParam;
			nam=nam.Right(4);
			nam.MakeLower();
			if(nam==".tmp"){
				ret=17;
				break;
			}
		}

		MainFramePtr->SetMessageText((char*)lParam);
		break;
	case AVP_INTERNAL_CHECK_DIR:
		MainFramePtr->SetMessageText((char*)lParam);
		break;
	
	case AVP_CALLBACK_ASK_DELETE:
		return 2;

	case AVP_CALLBACK_ASK_CURE:
		return m_Action;

	case AVP_CALLBACK_OBJECT_DONE:


		if(ScanObject->RFlags & RF_INTERFACE_FAULT){
			ObjectMessage(ScanObject,"intrface fault.");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_KERNEL_FAULT){
			ObjectMessage(ScanObject,"kernel fault.");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}

		if(ScanObject->RFlags & RF_NOT_A_PROGRAMM) break;
		if(ScanObject->RFlags & RF_NOT_OPEN) return KillScan;


if(ScanObject->RFlags & RF_REPORT_DONE) return KillScan;
		
		if(ScanObject->RFlags & RF_DISK_OUT_OF_SPACE){
			ObjectMessage(ScanObject,"disk out of space.");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_CORRUPT){
			ObjectMessage(ScanObject,"corrupted.");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_IO_ERROR){
			ObjectMessage(ScanObject,"I/O error.");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_CURE_FAIL){
//			s.Format("disinfection failed: %s",If_MakeFullName(tempb,ScanObject->VirusName));
//			ObjectMessage(ScanObject,s);	
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		if(ScanObject->RFlags & RF_PROTECTED){
			ObjectMessage(ScanObject,"password protected.");
			ScanObject->RFlags|=RF_REPORT_DONE;
		}

		if(!(ScanObject->RFlags & RF_REPORT_DONE))
		{

			if( !(ScanObject->RFlags & ~(RF_CURE|RF_ARCHIVE|RF_PACKED)) ){
					ObjectMessage(ScanObject,"ok.");
			}
			else{
				s.Format("RFlags: %08X",ScanObject->RFlags);
				ObjectMessage(ScanObject,s);	
			}
			ScanObject->RFlags|=RF_REPORT_DONE;
		}
		return KillScan;

	case AVP_CALLBACK_OBJECT_CURE_FAIL:
		s.Format("disinfection failed: %s",If_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s);	
		break;

	case AVP_CALLBACK_OBJECT_CURE:
		s.Format("disinfected: %s",If_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s);	
		ScanObject->RFlags|=RF_REPORT_DONE;
//		tPage5.m_Disinfected++;
		break;

	case AVP_CALLBACK_OBJECT_DELETE:
		s.Format("deleted: %s",If_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s);	
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;

	case AVP_CALLBACK_OBJECT_DETECT:
		s.Format("infected: %s",If_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s);	
		ScanObject->RFlags|=RF_REPORT_DONE;
		break;
	
	case AVP_CALLBACK_OBJECT_PACKED:
//		if(tPage3.m_ShowPack){
			s.Format("packed: %s",If_MakeFullName(tempb,ScanObject->VirusName));
			ObjectMessage(ScanObject,s);
//		}
//		if(!ScanObject->InPack)		tPage5.m_Packed++;
		break;

	case AVP_CALLBACK_OBJECT_ARCHIVE:
//		if(tPage3.m_ShowPack){
			s.Format("archive: %s",If_MakeFullName(tempb,ScanObject->VirusName));
			ObjectMessage(ScanObject,s);
//		}
//		tPage5.m_Archives++;
		break;
	
	case AVP_CALLBACK_OBJECT_WARNING:
		s.Format("warning: %s",If_MakeFullName(tempb,ScanObject->WarningName));
		ObjectMessage(ScanObject,s);	
		ScanObject->RFlags|=RF_REPORT_DONE;
//		tPage5.m_Warnings++;
		break;

	case AVP_CALLBACK_OBJECT_SUSP:
		s.Format("suspicion: %s",If_MakeFullName(tempb,ScanObject->SuspicionName));
		ObjectMessage(ScanObject,s);	
		ScanObject->RFlags|=RF_REPORT_DONE;
//		tPage5.m_Suspicious++;
//		if(tPage6.m_CopySuspicion)CopyToFolder(ScanObject->Name,tPage6.m_sSuspFolder);
		break;

	case AVP_CALLBACK_OBJECT_UNKNOWN:
		s.Format("%s: unknown format.",If_MakeFullName(tempb,ScanObject->VirusName));
		ScanObject->RFlags|=RF_REPORT_DONE;
		ObjectMessage(ScanObject,s);	
		break;
	
	case AVP_CALLBACK_OBJECT_MESS:
		ObjectMessage(ScanObject,ScanObject->VirusName);	
		break;

	case AVP_CALLBACK_LINK_NAME:
		ObjName=(char*)lParam;
		break;

	case AVP_CALLBACK_ERROR_FILE_OPEN:
		s.Format("Can't open file: %s",(char*)lParam);
		return MessageBox(s,"AVP Editor");

	case AVP_CALLBACK_ERROR_SET:
		s.Format("Wrong set file: %s",(char*)lParam);
		return MessageBox(s,"AVP Editor");

	case AVP_CALLBACK_ERROR_BASE:
		ShowWindow(SW_SHOW);
		PutLogString((char*)lParam,"ERROR:    Old or corrupted antiviral base");
		break;

	case AVP_CALLBACK_ERROR_KERNEL_PROC:
		ShowWindow(SW_SHOW);
		PutLogString((char*)lParam,"ERROR:    Base kernel proc absent");
		break;

	case AVP_CALLBACK_ERROR_LINK:
		s.Format("ERROR:    Object code not linked (%s)",lParam?(char*)lParam:"");
		ShowWindow(SW_SHOW);
		PutLogString(lps.curName ,s);
		break;

	case AVP_CALLBACK_ERROR_SYMBOL_DUP:
		s.Format("ERROR:    Duplicate symbol %s (%s)",(char*)lParam,(const char*)ObjName);
		ShowWindow(SW_SHOW);
		PutLogString(lps.curName,s);
		break;

	case AVP_CALLBACK_ERROR_SYMBOL:
		s.Format("ERROR:    Symbol %s not defined (%s)",(char*)lParam,(const char*)ObjName);
		ShowWindow(SW_SHOW);
		PutLogString(lps.curName,s);
		break;

	case AVP_CALLBACK_ERROR_FIXUPP:
		s.Format("ERROR:    Symbol %s fixupp error (%s)",(char*)lParam,(const char*)ObjName);
		ShowWindow(SW_SHOW);
		PutLogString(lps.curName,s);
		break;

	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
		ShowWindow(SW_SHOW);
		PutLogString((char*)lParam,"ERROR:    Authentication failed.");
		break;

		
	case AVP_CALLBACK_SAVE_PAGES:
		if(m_SavePages || save_pages)
		{
			CWorkArea* WorkArea= (CWorkArea*)lParam;

			s.Format(".%02d.pag",WorkArea->ScanObject->Entry);

			if(m_PathSave.Right(1)!="\\")m_PathSave+="\\";
			CString pag=m_PathSave;
			CString n=WorkArea->ScanObject->Name;
			pag+=((const char*)n) + n.ReverseFind('\\') + 1;
			pag.Replace('/','.');
			pag+=s;

			PageFileName="";

			HANDLE f=CreateFile(pag,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
			if(f!=INVALID_HANDLE_VALUE){
					ULONG count;
					WriteFile(f,WorkArea->Header, PAGE_C_END, &count, NULL);
					CloseHandle(f);
					PageFileName=pag;
			}
		}
		break;

	case AVP_CALLBACK_SAVE_PACKED:
		if(!m_SavePacked) break;
		s.Format(".%d.unp",
			((CWorkArea*)lParam)->ScanObject->InPack+1);

		goto sav;
		break;

	case AVP_CALLBACK_SAVE_ARCHIVED:
		if(!m_SaveArchived) break;
		s.Format(".%s",
			((CWorkArea*)lParam)->ZipName);
sav:
		{
			CWorkArea* WorkArea= (CWorkArea*)lParam;
			HANDLE Handle=WorkArea->Handle;
			DWORD size=GetFileSize(Handle,0);
			if(size==-1){
				Handle=((TFileData*)(WorkArea->Handle))->Handle;
				size=((TFileData*)(WorkArea->Handle))->CurrLen;
			}
			if(size==-1) break;

			if(m_PathSave.Right(1)!="\\")m_PathSave+="\\";
			CString pag=m_PathSave;
			CString n=WorkArea->ScanObject->Name;
			n=((const char*)n) + n.ReverseFind('\\') + 1;
			n+=s;
			
			n.Replace('\\','.');
			n.Replace(':',';');
			n.Replace('/','.');
			n.Replace('?','_');
			n.Replace('*','_');
			n.Replace('>','}');
			n.Replace('<','{');
			n.Replace('\"','`');
			n.Replace('|','!');
			pag+=n;



			HANDLE f=CreateFile(pag,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
			if(f!=INVALID_HANDLE_VALUE){
				CString s;
				BOOL del=1;
				char* buf=new char[size];
				if(buf){
					ULONG count;
					SetFilePointer(Handle,0,0,0);
					if(ReadFile(Handle,buf, size, &count, NULL))
						if(WriteFile(f,buf, size, &count, NULL))
							del=0;
					delete buf;
				}
				CloseHandle(f);
				if(del){
					DeleteFile(pag);
				}else
				{
					s.Format("%s entry saved as %s",uParam==AVP_CALLBACK_SAVE_ARCHIVED?"Archive":"Pack",pag);
					PutLogString(WorkArea->ScanObject->Name,s);
				}
			}
		}
		break;

	default:
		break;
	}
	return ret;
}


void CScanDialog::PutLogString( const char* objname, const char* message)
{
	LV_ITEM			lvitem;
	int i=m_List.GetItemCount();
	if((i%200)==0)m_List.SetItemCount(i+200);

	lvitem.mask = LVIF_TEXT;
	if(i == 0){
		lvitem.state = LVIS_FOCUSED;
		lvitem.mask |= LVIF_STATE;
	}
	lvitem.iItem = i;
	lvitem.iSubItem = 0;
	lvitem.pszText = (char*)objname;
	i = m_List.InsertItem(&lvitem);
	
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = i;
	lvitem.iSubItem = 1;
	lvitem.pszText = (char*)message;
	m_List.SetItem(&lvitem);

}


void CScanDialog::OnDestroy() 
{

	AfxGetApp()->WriteProfileString("Options","PathScan",m_Path);
	AfxGetApp()->WriteProfileString("ScanDialog","PathSave",m_PathSave);
	AfxGetApp()->WriteProfileInt("ScanDialog","CodeAnalyzer",m_CA);
	AfxGetApp()->WriteProfileInt("ScanDialog","AllWarnings",m_AllWarnings);
	AfxGetApp()->WriteProfileInt("ScanDialog","AllEntries",m_AllEntry);
	AfxGetApp()->WriteProfileInt("ScanDialog","SavePages",m_SavePages);
	AfxGetApp()->WriteProfileInt("ScanDialog","Redundant",m_Redundant);
	AfxGetApp()->WriteProfileInt("ScanDialog","SavePacked",m_SavePacked);
	AfxGetApp()->WriteProfileInt("ScanDialog","SaveArchived",m_SaveArchived);
	AfxGetApp()->WriteProfileInt("ScanDialog","Archived",m_Archived);
	AfxGetApp()->WriteProfileInt("ScanDialog","Mail",m_Mail);
	AfxGetApp()->WriteProfileInt("ScanDialog","Packed",m_Packed);
	AfxGetApp()->WriteProfileInt("ScanDialog","Action",m_Action);
	AfxGetApp()->WriteProfileInt("ScanDialog","Recurse",m_Recurse);

	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CScanDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------

      {  lSTRETCH, lRIGHT	(IDC_LIST),	lRIGHT	(lPARENT)	,-7  },
      {  lSTRETCH, lBOTTOM	(IDC_LIST),	lBOTTOM	(lPARENT)	,-7  },


      {  lEND                                                          }
   };
   
	Layout_ComputeLayout(m_hWnd, rules);
	
}

void CScanDialog::OnEditFind() 
{
	while(IDOK==FindDlg.DoModal()){
		if(FindDlg.m_String.GetLength()){
			OnEditFindagain();
			return;
		}
	}
}

void CScanDialog::OnEditFindagain() 
{
	if(!FindDlg.m_String.GetLength()) {
		OnEditFind();
		return;
	}
	BeginWaitCursor();

	int i=m_List.GetNextItem(-1,LVNI_FOCUSED);
	if(i == -1) i=0;
	CString s;
	CString f=FindDlg.m_String;
	if(!FindDlg.m_Case)f.MakeLower();

	while(-1 != (i=m_List.GetNextItem(i,LVNI_ALL)))
	{
		for(int j=0;j<=1;j++){
			m_List.GetItemText(i,j,s.GetBuffer(0x200),0x200);
			s.ReleaseBuffer();
			if(s.GetLength()){
				if(!FindDlg.m_Case)s.MakeLower();
				if(-1!=s.Find(f)){
					while(-1 != (j=m_List.GetNextItem(-1,LVNI_FOCUSED)))
							m_List.SetItemState(j,0,LVIS_FOCUSED);
					while(-1 != (j=m_List.GetNextItem(-1,LVNI_SELECTED)))
							m_List.SetItemState(j,0,LVIS_SELECTED);
							
					m_List.SetItemState(i,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
					m_List.EnsureVisible(i,FALSE);
					goto ret;
				}
			}
		}
	}
ret:
	EndWaitCursor();
	return;
}

static in_acc=0;

BOOL CScanDialog::PreTranslateMessage(MSG* pMsg) 
{
/*	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_TAB){
		if(0x80&GetKeyState(VK_CONTROL)){
			m_pMainWnd->PostMessage(WM_COMMAND,ID_AVP_CHANGETAB);
			return TRUE;
		}
	}
*/	if(!in_acc){
		in_acc++;
		int ret=TranslateAccelerator(m_hWnd ,acc,pMsg);
		in_acc--;
		if(ret)	return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
