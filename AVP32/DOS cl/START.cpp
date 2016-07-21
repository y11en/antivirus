#include <os2.h>

#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>
#include <share.h>
#include <dos.h>
#include <direct.h>

#include "start.h"
#include "memalloc.h"
#include "disk.h"
#include "..\locale\resource.h"
#include "avp_os2.h"
#include "avp_rc.h"
//#include "unsqu.h"
//#include "key.h"
#include "profile.h"
#include "report.h"

#include "..\\typedef.h"
#ifndef _INTERN_VER_
#include "../../Common.Fil/stuff/_CARRAY.H"
#include "../../Common.Fil/AvpKeyIDs.h"
#include "../../Common.Fil/sign\sign.h"
#include "../../Common.Fil/property.h"
#include "loadkeys.h"

ULONG functionality=0;
CPtrArray KeyHandleArray;
#endif
#include "contain.h"

char *GNAM(char *_ttop)
 {
  return (strrchr(_ttop,'\\')?
    (1+strrchr(_ttop,'\\')):
      (strrchr(_ttop,':')?(1+strrchr(_ttop,':')):_ttop));
 }

struct _cmdline cmdline;

// ‚®§¢à é ¥â ª®«¨ç¥áâ¢® ¤­¥© ¯à®è¥¤è¥¥ á 0-£® £®¤ 
unsigned long vldate_(unsigned short year,unsigned char month,unsigned char date)
 {
  return year*365+year/4+date+(month-1)*30+month/2-(year%4?2:1);
 }

char scanner_name[9];

unsigned char f_help=0;                   // 1 - out help screen
unsigned char f_daily=0;                  // 1 - daily

// prg_error - åà ­¨â ­®¬¥à ¯à®¨§®è¥¤è¥© ®è¨¡ª¨
// prg_error=0  - ­¥â ®è¨¡ª¨ prg_error>0      - ®è¨¡ª 
unsigned char prg_error=0;

// f_damage - ä« £, ¯®ª §ë¢ îé¨© ¯®¢à¥¦¤¥­/­¥ ¯®¢à¥¦¤¥­ "AVP.EXE"
// f_damage=0 - "AVP.EXE" ­¥ ¯®¢à¥¦¤¥­ f_damage=1  - "AVP.EXE" ¯®¢à¥¦¤¥­
//unsigned char f_damage=0;
ULONG fl_load=0;
BOOL flScanAll=FALSE;
//unsigned char f_was_expir=0;      // =1 - ¢ë¢¥áâ¨ ¯à¥¤ã¯à¥¦¤¥­¨¥, çâ® «¨æ¥­§¨ï
                                  // § ª®­ç¨« áì
//unsigned char f_will_expir=0;     // =1 - ¢ë¢¥áâ¨ ¯à¥¤ã¯à¥¦¤¥­¨¥, ª®£¤ 
                                  // § ª®­ç¨âìáï «¨æ¥­§¨ï
//unsigned char f_i_every_launch=0; // =1 - ¢ë¢¥áâ¨ "Copy Info" ¯à¨ § ¯ãáª¥

char *startPath;//[CCHMAXPATH];   // ®«­ë© ¯ãâì, ®âªã¤  ¡ë« § ¯ãé¥­ AVP
char prgPath[CCHMAXPATH];     // ®«­ë© ¯ãâì, £¤¥ ­ å®¤¨âáï AVP

char file_name[CCHMAXPATH],file_name_2[CCHMAXPATH];  // ãä¥à 

BOOL needs=0;
char nameExe[CCHMAXPATH];
/*
void prnInCenter(char *sour1,char *sour2,int fn)
 {
  char StrNull[]="º                                                     º\n\r";

  switch (fn)
   {
    case 0:
      if(strlen(StrNull)>strlen(sour1))
        strncpy(&StrNull[strlen(StrNull)/2-strlen(sour1)/2],sour1,strlen(sour1));
      PutStrAttr(StrNull,0x4f);
      break;
    case 1:
      if(strlen(sour2)>0)
       {
        if((strlen(StrNull)/2)>strlen(sour1))
          strncpy(&StrNull[strlen(StrNull)/2-strlen(sour1)-1],sour1,strlen(sour1));
        if((strlen(StrNull)/2)>strlen(sour2))
          strncpy(&StrNull[strlen(StrNull)/2+1],sour2,strlen(sour2));
        PutStrAttr(StrNull,0x4f);
       }
      break;
   }
 }
*/
//void OnSupportAboutDlg()
// {
//  int i=m_ListKeys.GetNextItem(-1,LVNI_SELECTED);
//  if(i!=-1)
//   {
//    AVP_dword dseq[5];
//    int l=DATA_Get_Val( (HDATA)KeyHandleArray[i], DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSUPPORTINFO,0), 0, 0, 0);
//    if(l)
//     {
//      char* p=new char[l];
//      if(p)
//       {
//        if(DATA_Get_Val( (HDATA)KeyHandleArray[i], DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSUPPORTINFO,0), 0, p, l))
//         {
//          CSupportDlg sd;
//          sd.m_Support=p;
//          sd.DoModal();
//          delete p;
//          return;
//         }
//        delete p;
//       }
//     }
//   }
//
//  CString n;
//  GetPrivateProfileString(_T("Configuration"),_T("SupportFile"),_T("Readme.txt"),n.GetBuffer(512),512,IniFile);
//  n.ReleaseBuffer();
//  if(_access(IniFile,0)!=0||_access(IniFile,2)==0)
//          WritePrivateProfileString(_T("Configuration"),_T("SupportFile"),n,IniFile);
//  if(_access(n,4))
//   {
//    CString t=StartPath;
//    t+="\\";
//    t+=n;
//    n=t;
//   }

//  ShellExecute(m_hWnd,_T("open"),n,NULL,NULL,SW_SHOWNORMAL);
// }

#ifndef _INTERN_VER_
BOOL OnInitAboutDlg()
 {
//  CDialog::OnInitDialog();

//  ListView_SetExtendedListViewStyle(m_ListKeys.m_hWnd, ListView_GetExtendedListViewStyle(m_ListKeys.m_hWnd)|LVS_EX_FULLROWSELECT);

//  m_ListKeys.InsertColumn(0,MS(IDS_LIST_KEYS_NUMBER),LVCFMT_LEFT,130,0);
//  m_ListKeys.InsertColumn(1,MS(IDS_LIST_KEYS_PLNAME),LVCFMT_LEFT,100,1);
//  m_ListKeys.InsertColumn(2,MS(IDS_LIST_KEYS_DESCRIPTION),LVCFMT_LEFT,300,2);
//  m_ListKeys.InsertColumn(3,MS(IDS_LIST_KEYS_EXPIRATION),LVCFMT_LEFT,100,3);
//  m_ListKeys.InsertColumn(4,MS(IDS_LIST_KEYS_FILE),LVCFMT_LEFT,100,4);
  _printf("Key name      Ser. number            Price pos.             Exp. date  Trial\r\n");
  for(int i=0;i<KeyHandleArray.GetSize();i++)
   {
    char szScr[0x100];
    memset(szScr,' ',0x100);
    char buf[0x200];
    HDATA hKey=(HDATA)KeyHandleArray[i];
    AVP_dword dseq[5];

    DATA_Get_Val(hKey,DATA_Sequence(dseq,PID_FILENAME,0),0,buf,0x200);
    char* p;
    //GetFullPathName(buf,0x200,buf,&p);
    p=strrchr(buf,'\\');
    if(p) p++;
    else  p=buf;
    if(strlen(p)<=13) memcpy(szScr,p,strlen(p));
    else  { memcpy(szScr,p,10); memcpy(&szScr[10],"...",3); }
    //m_ListKeys.SetItem(i,4,LVIF_TEXT,p,0,0,0,0);

    AVP_dword keynumber[3];
    *buf=0;
    if(DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSERIALNUMBER,0),0,keynumber,sizeof(keynumber)) )
     sprintf(buf,"%04d-%06d-%08d",keynumber[0],keynumber[1],keynumber[2]);
    else DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0),0,buf,0x200);
    if(strlen(buf)<=20) memcpy(&szScr[14],buf,strlen(buf));
    else  { memcpy(&szScr[14],buf,17); memcpy(&szScr[31],"...",3); }
//    m_ListKeys.InsertItem(i,buf);
//    if(i==0)
//      m_ListKeys.SetItem(i,0,LVIF_STATE,0,0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED,0);

    strcpy(buf,"AVP Key v1.0");
    DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOSNAME,0),0,buf,0x200);
    if(strlen(buf)<=20) memcpy(&szScr[37],buf,strlen(buf));
    else  { memcpy(&szScr[37],buf,17); memcpy(&szScr[54],"...",3); }
//    m_ListKeys.SetItem(i,1,LVIF_TEXT,buf,0,0,0,0);

    //SYSTEMTIME stCurrent;
    //GetSystemTime(&stCurrent);
    struct dosdate_t stCurrent;
    _dos_getdate(&stCurrent);
    SYSTEMTIME stExpir;
    memset(&stExpir,0,sizeof(SYSTEMTIME));
    strcpy(buf,"None");
    if(DATA_Get_Val( hKey, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, &stExpir, sizeof(AVP_date)) )
     {
      if(stCurrent.year > stExpir.wYear) goto time_bad;
      if(stCurrent.year < stExpir.wYear) goto time_ok;
      if(stCurrent.month > stExpir.wMonth) goto time_bad;
      if(stCurrent.month < stExpir.wMonth) goto time_ok;
      if(stCurrent.day > stExpir.wDay) goto time_bad;
      if(stCurrent.day < stExpir.wDay) goto time_ok;
time_bad:
      strcpy(buf,"Expired");
      goto ok;
time_ok:
//      strcpy(buf,CTime(stExpir).Format("%x"));
      _sprintf(buf,"%d.%d.%d",stExpir.wDay,stExpir.wMonth,stExpir.wYear);
ok:   ;
     }
    memcpy(&szScr[60],buf,strlen(buf));
//    m_ListKeys.SetItem(i,3,LVIF_TEXT,buf,0,0,0,0);

    *buf=0;
    if(DATA_Find(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))
     memcpy(&szScr[71],"Yes",3);
    else
     memcpy(&szScr[71],"No",2);
    DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLICENCEINFO,0),0,buf+strlen(buf),0x200);
//_printf("LisInfo:%s ",buf);
//    m_ListKeys.SetItem(i,2,LVIF_TEXT,buf,0,0,0,0);
    szScr[74]=0;
    _printf("%s\r\n",szScr);
   }

//  SetWindowText(MS( IDS_ABOUT ));
//  GetDlgItem(IDC_SUPPORT)->SetWindowText(MS( IDS_SUPPORT ));

//  HINSTANCE hiold=AfxGetResourceHandle();
//  HINSTANCE hinew=0;
//  if(LocFile.GetLength())
//   {
//    hinew=LoadLibrary(LocFile);
//    if(hinew>0)
//     {
//      AfxSetResourceHandle(hinew);
//      if(m_David.LoadBitmap(IDB_DAVID))
//              ((CStatic*)GetDlgItem(IDB_DAVID))->SetBitmap(HBITMAP(m_David));
//      FreeLibrary(hinew);
//      AfxSetResourceHandle(hiold);
//     }
//   }

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
 }
#endif

void About(void)
 {
//  CHAR  *tmp2;

  attr=GetCharAttr();
  PutStrAttr("\nÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»\n\r"
               "º AntiViral Toolkit Pro by Eugene Kaspersky for DOS32 º\n\r"
               "º           Copyright(C) Kaspersky Lab. 1998          º\n\r"
               "º                Version 3.0  build 132               º\n\r"
               "º                                                     º\n\r",0x4f);
//   _printf(LoadString(IDS_LAST_UPDATE,"®á«¥¤­¥¥ ®¡­®¢«¥­¨¥: %02d.%02d.%04d, %d ¢¨àãá®¢."),SurBase.LastUpdateDay,SurBase.LastUpdateMonth,SurBase.LastUpdateYear,SurBase.RecCount);
//   prnInCenter(LoadString(IDS_REGISTERINFO,"Registration info:"),NULL,0);
//   PutStrAttr("º                                                     º\n\r",0x4f);

//  if((avp_key_present)&&
//   ((avpKey->Flags&KEY_F_REGISTERED)&&(avpKey->Platforms&KEY_P_DOS)) )
    {
//     tmp2=(char*)avpKey;
//     prnInCenter(&tmp2[avpKey->CopyInfoOffs],NULL,0);

//     prnInCenter(LoadString(IDS_NAME,"Name"),&tmp2[avpKey->NameOffs],1);
//     prnInCenter(LoadString(IDS_ORGANIZATION,"Organization"),&tmp2[avpKey->OrgOffs],1);
//     prnInCenter(LoadString(IDS_REGNUMBER,"Registration number"),&tmp2[avpKey->RegNumberOffs],1);
    }
//  else
//     prnInCenter("Evaluation copy",NULL,0);

  PutStrAttr("ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼",0x4f);
//  SetCharAttr(attr,' ');
  PutStrAttr("\n\r",attr);

  _printf("                   %s                  \n\r",LoadString(IDS_REGISTERINFO,"Registration info:"));
#ifndef _INTERN_VER_
  if(functionality)
   {
    OnInitAboutDlg();
   }
  else
#endif
   _puts("                   Evaluation copy                  \n\r");
 }

void usage(void)
 {
_puts(LoadString(IDS_HELP_01,"\nUsage: AvpDos32.exe [options] path[\\name][...]\n"));
_puts(LoadString(IDS_HELP_02,"        Path - any DOS path, * or *: - all local disks\n"));
_puts(LoadString(IDS_HELP_03,"        Name - wildcards * or ?  Default is executable files\n"));
_puts(LoadString(IDS_HELP_04,"Valid options are:\n"                              ));
_puts(LoadString(IDS_HELP_05,"    /-  disinfect                       /D daily\n"));
_puts(LoadString(IDS_HELP_06,"    /E  delete infected files           /F=filename load alternate profile\n"));
_puts(LoadString(IDS_HELP_07,"    /XF=masks exclude files             /@=filename check files by list\n"));
_puts(LoadString(IDS_HELP_08,"    /XD=masks exclude directories        /@! delete list after scanning\n"));
_puts(LoadString(IDS_HELP_09,"    /1  check only one floppy disk      /*[-]  check all files\n"));
_puts(LoadString(IDS_HELP_10,"    /M[-]  skip memory test             /S[-]  sound off\n"));
_puts(LoadString(IDS_HELP_11,"    /P[-]  skip Master Boot Record test /R[-]  do not scan subdirectories\n"));
_puts(LoadString(IDS_HELP_12,"    /B[-]  skip DOS Boot Sector test    /W[T][A][-][+][=filename]  save report\n"));
_puts(LoadString(IDS_HELP_13,"    /U[-]  disable unpack                 /WT or -WT+ truncate existing file\n"));
_puts(LoadString(IDS_HELP_14,"    /A[-]  disable extract                /WA or -WA+ appends to existing file\n"));
_puts(LoadString(IDS_HELP_15,"    /H[-]  disable heuristic analysis     /W+ or -WA+ extended report\n"));
_puts(LoadString(IDS_HELP_16,"    /V[-]  enable redundant scanning    /MD[-] check mail databases\n"));
_puts(LoadString(IDS_HELP_17,"    /K[-]  disable pack info            /MP[-] check plain mail\n"));
_puts(LoadString(IDS_HELP_18,"    /O[-]  write OK messages            /Y[-]  skip all dialogs\n"));
_puts(LoadString(IDS_HELP_19,"    /Z[-]  disable aborting             /?  help screen\n"));
_puts(LoadString(IDS_HELP_20,"For all options '-' inverts the default meaning.\n" ));
_puts(LoadString(IDS_HELP_21,"Press Enter to finish...\n"));//"Press any key to finish...\n");

//"U¾ycie: AvpDos32.exe [opcje] ˜cie¾ka[nazwa][...]\n"
//"        ˜cie¾ka - ˜cie¾ka DOS, * lub *: - wszystkie twarde dyski\n"
//"        Nazwa - znaki zast©pcze * lub ?   Domy˜lne s¥ pliki wykonywalne\n"
//"Dost©pne opcje:\n"
//"   /-  leczenie                        /*  test wszystkich plik¢w\n"
//"   /E  kasuj zainfekowane pliki        /F=nazwa alternatywnego profilu\n"
//"   /XF=maska niesprawdzanych plik¢w    /@=nazwa test plik¢w z listy\n"
//"   /XD=maska niesprawdzanych katalog¢w   /@! skasuj list© po te˜cie\n"
//"   /1  test wyˆ¥cznie jednej dyskietki /D  dzienny\n"
//"   /M[-] pomiä test pami©ci            /S[-]  wyˆ¥czenie d«wi©ku\n"
//"   /P[-] pomiä test MBR                /R[-]  wyˆ¥czenie testu podkatalog¢w\n"
//"   /B[-] pomiä test DOS Boot Sektor¢w  /W[T][A][-][+][=nazwa]  zapisz raport\n"
//"   /U[-] wyˆ¥czenie dearchiwizacji      /WT lub -WT+ utnij istniej¥cy raport\n"
//"   /A[-] wyˆ¥czenie rozpakowywania      /WA lub -WA+ dopisz do pliku\n"
//"   /H[-] wyˆ¥czenie analizy heuryst.    /W+ lub -WA+ rozszerz raport\n"
//"   /V[-] wˆ¥cz tryb redundancyjny      /MD[-] test pocztowe bazy danych\n"
//"   /K[-] wyˆ¥czenie info. o pakowaniu  /MP[-] test proste formaty poczty\n"
//"   /O[-] pisanie komunikatu OK         /Y[-]  pomiä wszystkie dialogi\n"
//"   /Z[-] uniemo¾liwia przerwanie testu /?  ekran pomocy\n"
//"Dla wszystkich opcji '-' neguje domy˜lne znaczenie.\n"
//"Naci˜nij Enter aby zakoäczy†...\n");
  for(;;)
   {
    char c;
    if(kbhit())
     {
//      if(!(c=getch())) getch();
//      if(c==0x0D) break;
      if(((c=(char)getch())!=0)&&(c==0x0D)) break;
     }
   }
//  while(kbhit()==0);
//  flushall();
//  while(!feof(stdin) && (fgetc(stdin)!='\n')) ;
 }

int Start(void);

int main(int argc,char **argv)
 {
// char cline[0x50];           // Command Line
//puts("!1\n");
  char ccsopts[0x50];
  char ccspath[0x100];
  unsigned short i,len;
  unsigned char  szT[CCHMAXPATH],
                 realid='0',
                 *pt;
  int           in;          // file handle
  // -- for load base --
  struct dosdate_t cda;
  int ReturnCode=1;

  ccsopts[0]=0;
  ccspath[0]=0;
// cline[0]=0;
  for(unsigned char z=1;z<argc;z++)
   {
    if((strlen(szExtRep)+strlen(argv[z])+2)<0x200)
     {strcat(szExtRep,argv[z]); strcat(szExtRep," "); }
    //strcat(cline,argv[z]); strcat(cline," ");
    if(*argv[z]=='/')
     {
      if((strlen(ccsopts)+strlen(argv[z])+1)<0x50) strcat(ccsopts,argv[z]);
     }
    else
     if((strlen(ccspath)+strlen(argv[z])+2)<0x100)
       {strcat(ccspath,argv[z]); strcat(ccspath,";");}
   }
  if(argc<=1) strcat(szExtRep,"None");
  strcat(szExtRep,"\r\n");
//  if(cline[strlen(cline)-1]==' ') cline[strlen(cline)-1]=0;
//  strcat(cline,"\n\n");

 // -- ¯®«ãç¨âì ¨¬ï áª ­¥à  -------------------------
 pt=(unsigned char*)strrchr(*argv,'\\');
 if(pt==NULL) pt=(unsigned char*)(strrchr(*argv,':'));
 if(pt==NULL) pt=(unsigned char*)(*argv-1);
 strcpy(file_name,(char*)(pt+1));
 *(strchr(file_name,'.'))=0;
 strupr(file_name);
 strcpy(scanner_name,file_name);

 // -- ¯®«ãç¨âì ¯®«­ë© ¯ãâì ®âªã¤  ¡ë« § ¯ãé¥­ AVP --
 startPath=getcwd(NULL,0);

 // -- ¯®«ãç¨âì ¯®«­ë© ¯ãâì ª AVP -------------------
 strcpy(prgPath,*argv);
//strcpy(prgPath,"c:\\lang\\avp\\avp32.dos\\face\\");
 for(i=(unsigned short)(strlen(prgPath)-1);i!=0;i--)
  {
   switch(prgPath[i])
   {
    case '\\':
    case '/':
    case ':': prgPath[i+1]=0; goto avp_path_done;
   }
  }
 prgPath[0]=0;
avp_path_done:

 if(pmAllocMem(SIZEALLOC,&SS.selector,&SS.segment))
  {
   if(load_ini("avpdos32.ini")==0) fl_load|=FL_LOAD_INI;

   if(access(*argv,0)==-1)
    {
     if(prgPath[strlen(prgPath)-1]=='\\')
      sprintf(file_name,"%savpdos32.exe",prgPath);
     else
      sprintf(file_name,"%s\\avpdos32.exe",prgPath);
     if(access(file_name,0)==-1)
       sprintf(file_name,"%s\\avpdos32.exe",startPath);
    }
   else
    strcpy(file_name,*argv);
#ifndef _INTERN_VER_
   if(sign_check_file(file_name,1,0,0,0))
     strcat(szExtRep,"\r\nAvpDos32.exe has been damaged or infected by unknown virus!\r\n\r\n");
   else fl_load|=FL_UNSIGN_EXE;
#endif

   _dos_getdate(&cda);                       // ¯®«ãç¨âì â¥ªãéãî ¤ âã
   // -- ‡ £àã§ª  "avp.key" ---------------------------
#ifndef _INTERN_VER_
   SearchFile(KeyFile,file_name,".key");
//   avpKeyFlag=load_key_file_(file_name);
   DWORD Level=0;
   BOOL Valid=0;

   functionality=0;
   if(strlen(file_name)>0)
    functionality|=LoadKeys(AVP_APID_SCANNER_DOS_32,
                file_name, &Level, &Valid, &KeyHandleArray);
    functionality|=LoadKeys(AVP_APID_SCANNER_DOS_LITE,
                file_name, &Level, &Valid, &KeyHandleArray);
   if(strlen(KeysPath)==0) strcpy(KeysPath,prgPath);
   if(KeysPath[strlen(KeysPath)-1]!='\\') strcat(KeysPath,"\\*.key");
   else strcat(KeysPath,"*.key");
   functionality|=LoadKeys(AVP_APID_SCANNER_DOS_32,
                KeysPath, &Level, &Valid, &KeyHandleArray);
   functionality|=LoadKeys(AVP_APID_SCANNER_DOS_LITE,
                KeysPath, &Level, &Valid, &KeyHandleArray);

   functionality|=LoadKeys(AVP_APID_SCANNER_DOS_32,
                startPath, &Level, &Valid, &KeyHandleArray);
   functionality|=LoadKeys(AVP_APID_SCANNER_DOS_LITE,
                startPath, &Level, &Valid, &KeyHandleArray);

   if(!functionality)
    {
     //tPage2.m_Packed   &= !!(functionality & FN_UNPACK);
     //tPage2.m_Archives &= !!(functionality & FN_EXTRACT);
     //tPage2.m_MailBases&= !!(functionality & FN_MAIL);
     //tPage2.m_MailPlain&= !!(functionality & FN_MAIL);
     //tPage3.m_CA       &= !!(functionality & FN_CA);
     //if(!(functionality & FN_DISINFECT) && tPage6.m_Action<3)  tPage6.m_Action = 0;
//     MessageBox(MS(IDS_KEY_ABSENT),AppName,  MB_ICONINFORMATION|MB_OK);
//   else _puts("\nAVP Key is not loaded.\n");
    }
   else  //avpKeyFlag=1; if(avpKeyFlag)
    {
     fl_load|=FL_LOAD_KEY;
//     if(avpKey->Flags&KEY_F_INFOEVERYLAUNCH) f_i_every_launch=1;
//     if(avpKey->ExpirMonth&&avpKey->ExpirDay)
      {
//       if(vldate_(cda.year,cda.month,cda.day)>
//         vldate_(avpKey->ExpirYear,(unsigned char)avpKey->ExpirMonth,(unsigned char)avpKey->ExpirDay))
        {
//         f_was_expir=1;
//         avpKeyFlag=0;
        }
//       else
//        if(vldate_(cda.year,cda.month,cda.day)+avpKey->AlarmDays>
//          vldate_(avpKey->ExpirYear,(unsigned char)avpKey->ExpirMonth,(unsigned char)avpKey->ExpirDay))
//       f_will_expir=1;
      }
     if(Level==0)
      {
//       MessageBox((os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)?
//            MS(IDS_KEY_PLATHFORM_95):MS(IDS_KEY_PLATHFORM_NT),
//                               AppName,MB_ICONINFORMATION|MB_OK);
      }
     else
      {
       if(Level>=10) functionality |= FN_MINIMAL;
       if(Level>=20) functionality |= FN_OPTIONS;
       if(Level>=30) functionality |= FN_FULL;
       if(!Valid)
        {
         functionality &= ~FN_UPDATES;
//         MessageBox(MS(IDS_UPDATE_EXPIRED), AppName, MB_ICONINFORMATION|MB_OK);
        }
      }
    }

   if(!(functionality & FN_NETWORK))
    {
//     char* p;
//     CString s;
//     DWORD i=GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,s.GetBuffer(512),&p);
//     s.ReleaseBuffer();
//     if(GetDriveType(s.Left(3)) == DRIVE_REMOTE)
      {
//       functionality=0;
//       MessageBox(MS(IDS_REMOTE_LAUNCH),AppName,MB_ICONSTOP|MB_OK);
      }
    }
#endif

//   if(os.dwPlatformId==VER_PLATFORM_WIN32_NT && !IsAdmin()
//      && (-1 == CommandLine.Find(_T("/a "))))
//      if(cust.m_CustOtherMessages)
//    MessageBox(MS(IDS_ADMIN_RIGHTS),AppName,MB_ICONINFORMATION|MB_OK);

   // -- ‡ £àã§ª  "*.prf" -----------------------------
   strcat(szExtRep,"Profile ");
   strcat(szExtRep,DefProfile);
   setDefaultOptions(); // “áâ ­®¢ª  ®¯æ¨© ¯® ã¬®«ç ­¨î
   if(loadProfile(DefProfile,szExtRep)==0)
    fl_load|=FL_LOAD_PRF;
   else
    strcat(szExtRep," (profile is not loaded)");
   strcat(szExtRep,"\r\n");

   cmdline.skip_mbr      =0;
   cmdline.skip_boot     =0;
   cmdline.skip_dlg      =0;
   cmdline.one_floppy    =0;
   cmdline.quit_after    =0;
   cmdline.remote        =0;
   cmdline.abort_disable =0;
   *cmdline.ListFile     =0;
   cmdline.KillList      =0;
   cmdline.start_minumize=0;

   char *tmp_c=ccsopts;

   while(*tmp_c)
    {
     switch(toupper(*++tmp_c))
      {
       case 'I': needs=1; break;                   //yes
       case 'P':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Sectors=1; cmdline.skip_mbr=0;}
        else cmdline.skip_mbr=1;
        break;
       case 'B':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Sectors=1; cmdline.skip_boot=0;}
        else cmdline.skip_boot=1;
        break;
       case '1': cmdline.one_floppy=1; break;      //yes
       case 'R':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.SubDirectories=1;}
        else prf_data.SubDirectories=0;
        break;
       case 'M':                                   //yes
         switch(toupper(*(tmp_c+1)))
          {
           case 'D': tmp_c++;
            tmp_c++;
            if((*(tmp_c+1))=='-') { tmp_c++; prf_data.MailBases=0;}
            else prf_data.MailBases=1;
            break;
           case 'P':
            tmp_c++;
            if((*(tmp_c+1))=='-') { tmp_c++; prf_data.MailPlain=0;}
            else prf_data.MailPlain=1;
            break;
           default:
            if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Memory=1;}
            else prf_data.Memory=0;
            break;
          }
         break;
       case 'U':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Packed=1;}
        else prf_data.Packed=0;
        break;
       case 'A':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Archives=1;}
        else prf_data.Archives=0;
        break;
       case 'O':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.ShowOK=0;}
        else prf_data.ShowOK=1;
        break;
       case 'K':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.ShowPack=1;}
        else prf_data.ShowPack=0;
        break;
       case 'S':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Sound=1;}
        else prf_data.Sound=0;
        break;
       case '*':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Files=0;}
        else
         {
          prf_data.Files=1;
          prf_data.FileMask=2;
         }
        break;       //yes
       case 'X':
         ++tmp_c;
         switch(toupper(*tmp_c))
          {
           case 'F':
             if(*++tmp_c=='=')
              {
               prf_data.ExcludeFiles=1;
               if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
               else len=(unsigned short)strlen(tmp_c);
               if((len>0)&&(tmp_c[0]=='{'))
                {
                 strncpy(prf_data.ExcludeMask,&tmp_c[1],len-1);
                 prf_data.ExcludeMask[len-1]=0;
                }
               else
                {
                 strncpy(prf_data.ExcludeMask,tmp_c,len);
                 prf_data.ExcludeMask[len]=0;
                }
               tmp_c+=len;
               if(prf_data.ExcludeMask[strlen(prf_data.ExcludeMask)-1]=='}')
                prf_data.ExcludeMask[strlen(prf_data.ExcludeMask)-1]=0;
              }
             break;
           case 'D':
             if(*++tmp_c=='=')
              {
               prf_data.ExcludeFiles=1;
               PLIST pList,pDel;
               while((pTail!=NULL)&&(pTail->Selected==2))
                {
                 pDel=pTail;
                 pTail=pTail->pNext;
                 Free(pDel);
                }
               if(pTail!=NULL)
                {
                 if(pTail->pNext!=NULL)
                  {
                   pList=pTail;
                   do
                    {
gtNext:              if(pList->pNext!=NULL)
                      if(pList->pNext->Selected==2)
                       {
                        pDel=pList->pNext;
                        if(pList->pNext==pListTop)
                         {
                          pListTop=pList;
                          pList->pNext=NULL;
                         }
                        if(pList->pNext!=NULL)
                         pList->pNext=pList->pNext->pNext;
                        Free(pDel);
                        goto gtNext;
                       }
                     pList=pList->pNext;
                    } while(pList != NULL);
                  }
                }

               if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
               else len=(unsigned short)strlen(tmp_c);
               char tmpExclDir[CCHMAXPATH],*z0;
               strncpy(tmpExclDir,tmp_c,len);
               tmp_c+=len;
               tmpExclDir[len]=0;
               z0=strtok(tmpExclDir,"; ,{}");
               while(z0)
                {
                 if(z0[strlen(z0)-1]=='\\') z0[strlen(z0)-1]=0;
                 AddRecord(z0,2);
                 z0=strtok(0,"; ,{}");
                }
              }
             break;
          }
         break;
#ifndef _INTERN_VER_
       //case '-': if(avpKey!=NULL) prf_data.InfectedAction=2; break;
       case '-': if(functionality) prf_data.InfectedAction=2; break;
#else
       case '-': prf_data.InfectedAction=2; break;
#endif
       case 'E': prf_data.InfectedAction=3; break; //yes
       case 'T': prf_data.InfectedAction=0; break; //yes
       case 'H':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.CodeAnalyser=1;}
        else prf_data.CodeAnalyser=0;
        break;
       case 'V':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.RedundantScan=0;}
        else prf_data.RedundantScan=1;
        break;
       case 'F':                                   //yes
         char defprofile[CCHMAXPATH];
         if(*++tmp_c=='=')
          {
           if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
            else len=(unsigned short)strlen(tmp_c);
           strncpy(defprofile,tmp_c,len);
           tmp_c+=len;
           defprofile[len]=0;
           loadProfile(defprofile,NULL);
          }
        break;
       case 'D':                                   //yes
         if((in=open(*argv,O_RDWR|O_BINARY))!=-1)
          {
           USHORT date,time;
           _dos_getftime(in,&date,&time);
           if(vldate_((USHORT)YEAR(date),(UCHAR)MONTH(date),(UCHAR)DAY(date))==
             vldate_(cda.year,cda.month,cda.day))
            f_daily=1;
           else
            _dos_setftime(in,(USHORT)(((cda.year-1980) << 9)+(cda.month << 5)+cda.day),time);
           close(in);
          }
         break;
       case '?':
         f_help=1;
         break;
       case 'W':                                     //yes
         prf_data.Report=1;
         while((*tmp_c)&&(*tmp_c!='/'))
          switch(toupper(*++tmp_c))
           {
            case 'T': prf_data.Append=0; break;
            case 'A': prf_data.Append=1; break;
            case '-': prf_data.ExtReport=0; break;
            case '+': prf_data.ExtReport=1; break;
            case '=':
              if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
              else len=(unsigned char)strlen(tmp_c);
              strncpy(prf_data.ReportFileName,tmp_c,len);
              prf_data.ReportFileName[len]=0;
              tmp_c+=len;
              break;
           }
         break;
       case '@':                                     //yes
         needs=1;
         if(*++tmp_c=='!') {cmdline.KillList=1; tmp_c++;}
         if(*tmp_c=='=')
          {
           if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
           else len=(unsigned short)strlen(tmp_c);
           strncpy(cmdline.ListFile,tmp_c,len);
           tmp_c+=len;
           cmdline.ListFile[len]=0;
          }
         break;
       case 'Y':
        if((*(tmp_c+1))=='-') { tmp_c++; cmdline.skip_dlg=0;}
        else cmdline.skip_dlg=1;
        break;
       case 'Z':
        if((*(tmp_c+1))=='-') { tmp_c++; cmdline.abort_disable=0;}
        else cmdline.abort_disable=1;
        break;
       case 'L':
        if((*(tmp_c+1))=='-') { tmp_c++; prf_data.ScanRemovable=0;}
        else prf_data.ScanRemovable=1;
        break;
//     case 'N': cmdline.remote=1; break;
//     case 'T':
//      if(*++tmp_c!='=') while(*tmp_c&&*tmp_c!='/') tmp_c++;
//      else
//      {
//       if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
//       else len=(unsigned short)strlen(tmp_c);
//       strncpy(temp_path,tmp_c,len);
//       temp_path[len]=0;
//      }
//      break;
//     case 'L':
//      realid=(unsigned char)toupper(*++tmp_c);
//      if(*++tmp_c=='!') if(set_default_lng(file_name,realid)<=0) return -8;
//      break;
      } // switch(toupper(*++tmp_c))
     while(*tmp_c&&*tmp_c!='/') tmp_c++;
    } // while(*tmp_c)

   // -------------------------------------------------------
   if(*ccspath)
   {
//    prf_data.FileMask=2;
    SelectAllRecords(0);
    char *_ttop=strtok(ccspath,";");
    while(_ttop)
     {
      //ã¡¨à ¥¬ ¯®á«¥¤­¨© á«¥è ¥á«¨ ¡®«¥¥ 1 á¨¬¢®« 
      if(_ttop[1]&&_ttop[strlen(_ttop)-1]=='\\') _ttop[strlen(_ttop)-1]=0;
      // á¨âã æ¨ï * ¨«¨ *:
      if(*_ttop=='*'&&(!_ttop[1]||_ttop[1]==':'&&!(_ttop[2])))
       {
        flScanAll=TRUE;
        SelectDiskType(2,1);
        if(prf_data.ScanRemovable==1)
         {
          SelectDiskType(1,1);
          SelectDiskType(3,1);
         }
       }
      else
       if(_ttop[1]==':'&&!_ttop[2]&&
          tolower(*_ttop)>='a'&&tolower(*_ttop)<='z')//&&Disk[toupper(*_ttop)-'A'])
         AddRecord((char*)_ttop,1);
       else
        {
         if((!strcmp(_ttop,"."))||(!strcmp(_ttop,".\\"))) AddRecord(startPath,1);
         else
          {
           prf_data.Path=1;
           if(access(_ttop,0)!=-1)  AddRecord(_ttop,1);
           else
            if(strlen(GNAM(_ttop))<13)
             {
              char *szTmpMsk=GNAM(_ttop);
              strcpy(prf_data.UserMask,szTmpMsk);

              if((strlen(szTmpMsk)>0)&&(szTmpMsk[0]=='{'))
                {
                 strncpy(prf_data.UserMask,&szTmpMsk[1],strlen(szTmpMsk)-1);
                 prf_data.UserMask[strlen(szTmpMsk)-1]=0;
                }
               else
                {
                 strncpy(prf_data.UserMask,szTmpMsk,strlen(szTmpMsk));
                 prf_data.UserMask[strlen(szTmpMsk)]=0;
                }
               if(prf_data.UserMask[strlen(prf_data.UserMask)-1]=='}')
                prf_data.UserMask[strlen(prf_data.UserMask)-1]=0;
              if(szTmpMsk!=_ttop)
               {
                strncat((char*)szT,_ttop,GNAM(_ttop)-_ttop);
                if((strlen((char*)szT)<=3)&&(szT[1]==':')&&
                  (tolower(szT[0])>='a') && (tolower(szT[0])<='z') )
                 {if(szT[2]!='\\') strcat((char*)szT,"\\");}
                else
                 if(szT[strlen((char*)szT)-1]=='\\') szT[strlen((char*)szT)-1]=0;
                AddRecord((char*)szT,1);
               }
              else
               {
                char *CurrDir;
                CurrDir=getcwd(NULL,0);
                AddRecord(CurrDir,1);
//                Free(CurrDir);
               }
              prf_data.FileMask=3;
             }
          }
        }
      _ttop=strtok(NULL,";");
     }
    needs=1;
   }

//   if(!((f_daily==1)||(f_help==1)) )
   if(f_daily==0)
    {
     pt=(unsigned char*)strrchr(*argv,'\\');
     if(pt==NULL) pt=(unsigned char*)strrchr(*argv,':');
     if(pt==NULL) pt=(unsigned char*)(*argv-1);
     strcpy(nameExe,(char*)(pt+1));
//     HANDLE AvpCreateFile("C:\\FSAV_000.NUL",GENERIC_READ,FILE_SHARE_READ,
//      NULL,OPEN_EXISTING,0,NULL);
     sopen("C:\\FSAV_000.NUL",O_RDONLY|O_BINARY,SH_DENYNO,S_IWRITE);
     //¯®¯ëâª  ®âªàëâì â ª®© ä ©« à¥£¨áâà¨àã¥â ä ©« ã ¬®­¨â®à (win 9x)
     //ä ©« ¬®¦¥â ­¥ áãé¥áâ¢®¢ âì
     // fsav_001.nul - ¤¥à¥£¨áâà æ¨ï
     ReturnCode=Start();
    }

//   if(f_help==1) usage();
   // exit_();
   }
  pmFreeMem(SS.selector);
  return ReturnCode;
 }

// Ž¡à ¡®âç¨ª ¢ëå®¤  ¨§ ¯à®£à ¬¬ë
/*void exit_()
{
 unsigned char i,y=1;

 if(prg_error!=6)
 {
  OutText(prg_name); OutText("  "); OutText(version);
  if(prg_error)
  {
   switch(prg_error)
   {
    case 1:
     OutText("Not enough memory");
     break;
    case 4:
     OutText("Language file not found");
     break;
    case 5:
     OutText("Language file error");
     break;
    case 7:
     out_string_(203); // "SET file not found"
     break;
    case 8:
     OutText("Language selection failure");
     break;
    case 9:
     OutText("Temporary file error");
     break;
    case 10:
     OutText("Kernel procedure absent");
     break;
    case 11:
     OutText(scanner_name);
     OutText(" has been damaged or infected by unknown virus!");
   } // switch(prg_error)
  }
//  else
//  {
//   if(f_help)
//   {
//    for(i=2;i<7;i++) out_string_2_(string_ptr[i]);
//    scan_lng_file_(file_name);
//   }
//   else
//    if(f_daily)
//    {
//     out_string_2_(string_ptr[19]);
//    }
//  }
 } // if(prg_error!=6)

 // -- set errorlevel & exit to DOS -------------------------
// if(prg_error&&!f_damage)
//  exit(ERR_LEVEL_AVP_CRASH);
// if(f_damage)
//  exit(ERR_LEVEL_AVP_DAMAGE);
// if(statistics[INC_CURED]+statistics[INC_DELETED]==statistics[INC_INFECTED]
// &&statistics[INC_INFECTED])
//  exit(ERR_LEVEL_ALL_DISINFECTED);
// if(statistics[INC_INFECTED])
//  exit(ERR_LEVEL_VIRUS);
// if(statistics[INC_WARNING]||statistics[INC_SUSPICION])
//  exit(ERR_LEVEL_WARNINGS);
// if(scan_end)
//  exit(ERR_LEVEL_NOT_COMPLETED);
// exit(ERR_LEVEL_NO_VIRUS);
}*/

/*
BOOL CAvp32Dlg::DisableControls(BOOL dis)
 {
  CWnd* wp=GetFocus();

  GetDlgItem(IDC_TRACK)->ShowWindow(dis?SW_SHOW:SW_HIDE);

  GetMenu()->EnableMenuItem(ID_SCAN_NOW,dis?MF_GRAYED:MF_ENABLED);
  GetMenu()->EnableMenuItem(ID_STOP,dis?MF_ENABLED:MF_GRAYED);

  GetMenu()->EnableMenuItem(ID_TOOLS_UPDATENOW,dis?MF_GRAYED:MF_ENABLED);
  GetMenu()->EnableMenuItem(ID_FIND,(dis || m_List.GetItemCount())?MF_ENABLED:MF_GRAYED);
  GetMenu()->EnableMenuItem(ID_VIRLIST,dis?MF_GRAYED:MF_ENABLED);


  GetDlgItem(IDC_SCAN_NOW)->SetWindowText(MS( (dis?IDS_STOP:IDS_SCAN_NOW) ));
  SetDefID(IDC_SCAN_NOW);

//tPage1.GetDlgItem(IDC_LIST_DISK)->EnableWindow(!dis);
  tPage1.Enable(!dis);
  tPage1.GetDlgItem(IDC_LOCAL)->EnableWindow(!dis);
  tPage1.GetDlgItem(IDC_NETWORK)->EnableWindow(!dis && (functionality & FN_NETWORK));
  tPage1.GetDlgItem(IDC_FLOPPY)->EnableWindow(!dis);
  tPage1.GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(!dis);

  tPage2.GetDlgItem(IDC_MEMORY)->EnableWindow(!dis && RecCountArray[RT_MEMORY]);
  tPage2.GetDlgItem(IDC_SECTORS)->EnableWindow(!dis && RecCountArray[RT_SECTOR]);
  tPage2.GetDlgItem(IDC_FILES)->EnableWindow(!dis && RecCountArray[RT_FILE]);
  tPage2.GetDlgItem(IDC_PACKED)->EnableWindow(!dis && RecCountArray[RT_UNPACK] && (functionality & FN_UNPACK));
  tPage2.GetDlgItem(IDC_EXTRACT)->EnableWindow(!dis && RecCountArray[RT_EXTRACT] && (functionality & FN_EXTRACT));
  tPage2.GetDlgItem(IDC_MAILBASES)->EnableWindow(!dis && RecCountArray[RT_EXTRACT] && (functionality & FN_MAIL));
  tPage2.GetDlgItem(IDC_MAILPLAIN)->EnableWindow(!dis && RecCountArray[RT_EXTRACT] && (functionality & FN_MAIL));

  if(!(functionality & FN_MAIL)){
          tPage2.GetDlgItem(IDC_MAILBASES)->ShowWindow(SW_HIDE);
          tPage2.GetDlgItem(IDC_MAILPLAIN)->ShowWindow(SW_HIDE);
  }

  tPage2.GetDlgItem(IDC_SMART)->EnableWindow(!dis);
  tPage2.GetDlgItem(IDC_PROGRAMS)->EnableWindow(!dis);
  tPage2.GetDlgItem(IDC_ALLFILES)->EnableWindow(!dis);
  tPage2.GetDlgItem(IDC_USERDEF)->EnableWindow(!dis);
  tPage2.GetDlgItem(IDC_EDIT_EXT)->EnableWindow(!dis);
  tPage2.GetDlgItem(IDC_EXCLUDE)->EnableWindow(!dis);
  tPage2.GetDlgItem(IDC_EDIT_EXCLUDE)->EnableWindow(!dis);

  tPage3.GetDlgItem(IDC_WARNINGS)->EnableWindow(!dis);
  tPage3.GetDlgItem(IDC_CA)->EnableWindow(!dis && RecCountArray[RT_CA] && (functionality & FN_CA));
  tPage3.GetDlgItem(IDC_REDUNDANT)->EnableWindow(!dis);
  tPage3.GetDlgItem(IDC_SHOWOK)->EnableWindow(!dis);
  tPage3.GetDlgItem(IDC_SHOWPACK)->EnableWindow(!dis);
  tPage3.GetDlgItem(IDC_REPORT)->EnableWindow(!dis);
//tPage3.GetDlgItem(IDC_BUTTON_CUST)->EnableWindow(!dis);

  tPage6.GetDlgItem(IDC_REPORTONLY)->EnableWindow(!dis);
  tPage6.GetDlgItem(IDC_DISINFECTDIALOG)->EnableWindow(!dis && (functionality & FN_DISINFECT));
  tPage6.GetDlgItem(IDC_DISINFECTAUTO)->EnableWindow(!dis && (functionality & FN_DISINFECT));
  tPage6.GetDlgItem(IDC_DELETE)->EnableWindow(!dis );
  tPage6.GetDlgItem(IDC_COPYINFECTED)->EnableWindow(!dis);
  tPage6.GetDlgItem(IDC_COPYSUSPICIOUS)->EnableWindow(!dis);

  if(wp){
          while(!wp->IsWindowEnabled())   wp=GetNextDlgTabItem(wp);
          wp->SetFocus();
  }

//UpdateData();
  return dis;
 }

  if(!(functionality & FN_UPDATES))
   {
    char s[0x300];
    sprintf(s,MS(IDS_UPDATE_EXPIRED));
    MessageBox(s,AppName);
    return;
   }
*/
