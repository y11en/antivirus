#include <os2.h>

#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <c:\lang\watcom\h\sys\\types.h>
#include <c:\lang\watcom\h\sys\\stat.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>

#include "memalloc.h"
#include "avp_os2.h"
#include "kernel.h"
#include "start.h"
#include "..\locale\resource.h"
//#include "key.h"
#include "profile.h"
#include "disk.h"
#include "dword.h"

#include "..\\typedef.h"
#include "../../Common.Fil/stuff/_CARRAY.H"
#include "../../Common.Fil/_avpio.h"
#include "../../Common.Fil/ScanApi/baseapi.h"

#ifndef _INTERN_VER_
#include "LoadKeys.h"
extern ULONG functionality;
#include "../../Common.Fil/AvpVndIDs.h"
#include "../../Common.Fil/sign\sign.h"
#include "../../Common.Fil/kldtser.h"
extern void** vndArray;
extern int vndArrayCount;
#endif

void About(void);

extern int  ExitCode;
VOID DisplayMessage(PCHAR szStr,BOOL flStop)
 {
  _printf("\n%s\n\n",szStr);
  if(flStop)
   if(cmdline.skip_dlg!=1)
    {
     _puts(LoadString(IDS_PRESS_KEY_L,"Press any key\n"));
     getch();
    }
 }

// проверка реакции системы на "Ctrl"+"Break"
// return: 1 - система реагирует
//         0 - система не реагирует
UCHAR GetCtrlBreak(void)
 {
  RMI   rmi;                   //REGS r;
                               //r.w.ax=0x3300;
  memset(&rmi,0,sizeof(RMI));  //int386(0x21,&r,&r);
  rmi.EAX = 0x3300;//ax        //return (r.h.dl==0)? FALSE:TRUE;
  if(SimulateRMInt(0x21,&rmi)) return (UCHAR)(rmi.EDX&0xff);
  else                         return 0;
 }

// установка реакции системы на "Ctrl"+"Break"
// State: 1 - система реагирует
//        0 - система не реагирует
BOOL SetCtrlBreak(UCHAR State)
 {
  RMI rmi;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x3301;//ax            //r.w.ax=0x3301;
  rmi.EDX = State; //dl            //r.h.dl=0;
  return SimulateRMInt(0x21,&rmi); //int386(0x21,&r,&r);
 }

BYTE ctrlBreakState;               // реакция системы на "Ctrl"+"Break"
// Указатели на старые обработчики прерываний
void __interrupt (__far *oldvect24)();
void __interrupt (__far *oldvect1b)();

typedef void __interrupt __far (*intvec_p)();
static intvec_p oldint1b=NULL;
static intvec_p oldint23=NULL;
static intvec_p oldint24=NULL;

UCHAR intBody[3]={0xb0,0x3,0xcf};

// Тело обработчика прерывания INT 1bh
static void _interrupt __far int1Bhandler(void)
 {
  if(scan_goes==1)
   {
    scan_goes=0;
    BAvpCancelProcessObject(1);
    _puts("\nAbort\n");
   }

  *(char*)0x471&=0x7F;       // reset DOS Ctrl-Break bit
  //oldint1b();           // вызов старого обработчика INT 1b
  //TSystemError::ctrlBreakHit = True;
 }

static void _interrupt __far int23handler(void)
 {
 }

void set_es(void);
#pragma aux set_es = "push ds" "pop es";

// Это тело обработчика прерывания для прерываний: INT 24h
static void _interrupt __far int24handler(union INTPACK r)
 {
  _enable();
  set_es();
  USHORT code  =r.w.di;
  USHORT drive =r.w.ax;
  USHORT devseg=r.w.bp;
  USHORT devoff=r.w.si;
  if(code==9) drive=0xFF;        // Printer out of paper
  else
   if(drive&0x8000)
    {
     code=13;                    // Bad memory image of FAT
     char *devhd=(char*)MK_FP(devseg,devoff);
     if(devhd[5]&0x80)          // Block device?
      code++;                    // Device access error
    }
  //r.w.ax=1; // retry
  r.w.ax=3;   // fail
  //REGS rr;
  //rr.w.ax = 0x5400;            // Dummy function call to get
  //int386(0x21,&rr,&rr);        // DOS into a stable state
 }

void resume(void)
 {
  // -- save CtrlBreak state ---
  ctrlBreakState=GetCtrlBreak();
  SetCtrlBreak(0);
  if(oldint24==NULL)  // -- Перехват INT 24h
   {
    //oldvect24=_dos_getvect(0x24);  //critical error handler
    //_dos_setvect(0x24,(void __interrupt (__far *)(void))intBody);
    oldint24=_dos_getvect(0x24);
    _dos_setvect(0x24,intvec_p(int24handler));
   }
  if(oldint23==NULL)  // -- Перехват INT 23h
   {
    //oldvect1b=_dos_getvect(0x23);  //ctrl-break
    //_dos_setvect(0x,(void __interrupt (__far *)(void))intBody);

    oldint23=_dos_getvect(0x23);
    //_dos_setvect(0x23,(void __interrupt (__far *)(void))intBody);
    _dos_setvect(0x23,intvec_p(int23handler));
   }
  if(oldint1b==NULL)  // -- Перехват INT 1bh
   {
    //oldvect1b=_dos_getvect(0x1b);  //ctrl-break
    //_dos_setvect(0x1b,(void __interrupt (__far *)(void))intBody);
    oldint1b=_dos_getvect(0x1B);
    _dos_setvect(0x1B,intvec_p(int1Bhandler));
   }
 }

void suspend(void)
 {
  // -- восстановить INT 0x24, INT 0x1b, INT 0x23, INT 0x1c --
  if(oldint1b!=NULL)
   { _dos_setvect(0x1B,oldint1b);  oldint1b=NULL; }
  if(oldint23!=NULL)
   { _dos_setvect(0x23,oldint23);  oldint23=NULL; }
  if(oldint24!=NULL)
   { _dos_setvect(0x24,oldint24);  oldint24=NULL; }
  SetCtrlBreak(ctrlBreakState); // restore CtrlBreak state
 }

BOOL  flgScan=0;

extern char file_name_2[CCHMAXPATH];  // Буфер

unsigned long vldate_(unsigned short year,unsigned char month,unsigned char date);

void setPrf(void)
 {
//  if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_UNPACK)))    prf_data.Packed=0;
//  if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_EXTRACT)))   prf_data.Archives=0;
//  if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_MAILBASES))) prf_data.MailBases=0;
//  if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_MAILPLAIN))) prf_data.MailPlain=0;
#ifndef _INTERN_VER_
  if(!(functionality & FN_UNPACK))  prf_data.Packed=0;
  if(!(functionality & FN_EXTRACT)) prf_data.Archives=0;
  if(!(functionality & FN_MAIL))    prf_data.MailBases=0;
  if(!(functionality & FN_MAIL))    prf_data.MailPlain=0;
#endif

//  if(!(functionality & FN_DISINFECT) && tPage6.m_Action<3)  tPage6.m_Action = 0;
  switch (prf_data.InfectedAction)
   {
    case 0:
    case 3:
     break;
    case 1:
     //if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_DISINFECT)))
#ifndef _INTERN_VER_
     if(!(functionality & FN_DISINFECT))
       prf_data.InfectedAction=0;
#endif
     break;
   }

//  if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_CA))) prf_data.CodeAnalyser=0;
#ifndef _INTERN_VER_
  if(!(functionality & FN_CA)) prf_data.CodeAnalyser=0;
#endif
//   if((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_REMOTESCAN))) WinEnableWindow(hWndNB1RBTN2,FALSE);
 }

void strCpyEnd(char* dst,char* src)
 {
  int i,j;
  for(j=0;j<(STATLEN-6);j++) dst[j]=' ';
  dst[j+1]='\0';
  for(i=strlen(src)-1;(i>=0)&&(j>=0);i--,j--) dst[j]=src[i];
 }

void initWorcStat(void)
 {
  setPrf();

  strCpyEnd(workStat.Sectors  ,LoadString(IDS_TAB5_STATIC_SECTORS    ,"Sector Objects :"));
  strCpyEnd(workStat.Files    ,LoadString(IDS_TAB5_STATIC_FILES      ,"Files :"));
  strCpyEnd(workStat.Folder   ,LoadString(IDS_TAB5_STATIC_FOLDERS    ,"Folders :"));
  strCpyEnd(workStat.Archives ,LoadString(IDS_TAB5_STATIC_ARCHIVES   ,"Archives :"));
  strCpyEnd(workStat.Packed   ,LoadString(IDS_TAB5_STATIC_PACKED     ,"Packed :"));
  strCpyEnd(workStat.countSize,LoadString(IDS_TAB5_STATIC_SPEED      ,"Speed (Kb/sec) :"));
  strCpyEnd(workStat.scanTime ,LoadString(IDS_TAB5_STATIC_TIME       ,"Scan time :"));
  strCpyEnd(workStat.knownVir ,LoadString(IDS_TAB5_STATIC_VIRUSES    ,"Known viruses :"));
  strCpyEnd(workStat.virBodies,LoadString(IDS_TAB5_STATIC_BODIES     ,"Virus bodies :"));
  strCpyEnd(workStat.Disinfect,LoadString(IDS_TAB5_STATIC_DISINFECTED,"Disinfected :"));
  strCpyEnd(workStat.Deleted  ,LoadString(IDS_TAB5_STATIC_DELETED    ,"Deleted :"));
  strCpyEnd(workStat.Warning  ,LoadString(IDS_TAB5_STATIC_WARNINGS   ,"Warnings :"));
  strCpyEnd(workStat.Suspis   ,LoadString(IDS_TAB5_STATIC_SUSPIC     ,"Suspicious :"));
  strCpyEnd(workStat.Corript  ,LoadString(IDS_TAB5_STATIC_CORRUPTED  ,"Corrupted :"));
  strCpyEnd(workStat.IOError  ,LoadString(IDS_TAB5_STATIC_IOERRORS   ,"I/O Errors :"));
  iniWorkStat();
 }

extern CDWordArray VirCountArray;

ULONG start2(void)
 {
  char dat[CCHMAXPATH];      // буфер для вывода даты
  unsigned ret=0;

  LoadIO();
  //BAvpRegisterCallback();
  BAvpInit();  // Allocate internal Buffers

  // -- Загрузка баз && Memory check -----------------------
  char *curpath;
  unsigned drives;

  curpath=getcwd(NULL,0);

  if(prgPath[1]==':') _dos_setdrive(tolower(prgPath[0])-'a'+1,&drives);
  chdir(prgPath);

  if(strlen(BasePath)>0)
   {
    if(BasePath[strlen(BasePath)-1]!='\\')
     sprintf(file_name,"%s\\%s",BasePath,SetFile);
    else
     sprintf(file_name,"%s%s",BasePath,SetFile);
    if(access(file_name,0)==-1)
     {
      if(file_name[1]!=':')
       {
        strcpy(file_name,prgPath);
        if(file_name[strlen(file_name)-1]!='\\')
         { strcat(file_name,"\\");strcat(file_name,BasePath); }
        else
         strcat(file_name,BasePath);
        if(file_name[strlen(file_name)-1]!='\\')
         { strcat(file_name,"\\");strcat(file_name,SetFile); }
        else
         strcat(file_name,SetFile);
        //if(access(file_name,0)==-1) { }
       }
     }
   }
  else
   {
    strcpy(file_name,SetFile);
    if(access(file_name,0)==-1)
     {
      strcpy(file_name,prgPath);
      strcat(file_name,SetFile);
     }
   }

  if(access(file_name,0)==-1)
   {
    ULONG ulCurDisk;
    ulCurDisk=_getdrive();
    if((ulCurDisk==1)||(ulCurDisk==2))
     {
      _puts(LoadString(IDS_INSERT_BASES,"\nInsert disk with Antiviral bases and press <Enter>\n"));
      getch();
     }
   }
  if(access(file_name,0)==-1)
   {
    _puts(LoadString(IDS_BASES_ABSEND,"\nAntiviral bases not found. Press any key to exit\n"));
    getch();
    ret=7;
   }
  else
   if(load_avpbase(file_name)==FALSE)
    {DisplayMessage(LoadString(IDS_ERRORLOAD_AVC,"Error loading *.avc"),1); ret=7;}

  _dos_setdrive(tolower(curpath[0])-'a'+1,&drives);
  chdir(curpath);

  if(ret!=0) return ret;
  fl_load|=FL_LOAD_BASE;

  // -- Проверка баз на устарение --------------------------
  unsigned long nmonth;
  struct dosdate_t currdate;
  int setfil;
  USHORT date,time;
  if((setfil=open(file_name,O_RDONLY))!=(-1))
   {
    _dos_getftime(setfil,&date,&time);
    close(setfil);
   }
  _dos_getdate(&currdate);                       // получить текущую дату
  //if(avpKeyFlag) nmonth=avpKey->BaseDelay; else nmonth=3;
#ifndef _INTERN_VER_
  if(functionality)
#endif
    nmonth=3;
  if(vldate_(currdate.year,currdate.month,currdate.day)>
    vldate_((USHORT)YEAR(date),(UCHAR)MONTH(date),(UCHAR)DAY(date))+nmonth*30)
   DisplayMessage(LoadString(IDS_SET_OUTOFDATE,"The set of AVP antiviral bases is out of date.\nPlease take updates."),1);

  // -- Проверка "AVP.EXE" на вирусы & CRC -----------------
  SelfScan=1;
  unsigned char o_report;
  o_report=prf_data.Report;
  prf_data.Report=0;
  struct stat statbuf;
  sprintf(file_name,"%s\\%s",prgPath,nameExe);
  if(stat(file_name,&statbuf)!=(-1))
    ScanFile(file_name,statbuf.st_size,GetFlags());
  sprintf(file_name,"%s\\avpdos32.exe",startPath);
  if(stat(file_name,&statbuf)!=(-1))
    ScanFile(file_name,statbuf.st_size,GetFlags());
  prf_data.Report=o_report;
  SelfScan=0;

  //if(prf_data.Memory) // Test of Memory
  // {
  //  out_string_(178);
  //  ScanObject.type=_MEMORY;
  //  ProcessObject();
  // }

  //if((ScanObject.r_flags&RF_DETECT&&!(ScanObject.r_flags&RF_CURE))||!CheckCRC())
  // {f_damage=1; return 11;}

  //if(f_csdl) mb_ok_(8);

  //if(f_i_every_launch) info_every_launch_(); // out copy info

  // -- Вывод предупреждений касающихся лицензии -----------
//  if(avpKey!=NULL)
   {
    //send_param_ptr=(unsigned char *)dat;
//    if(f_was_expir)
//     {
//      sprintf(dat,LoadString(IDS_EXPIRIED_L,"Your AVP license was expiried\nat %d/%d/%d"),avpKey->ExpirMonth,avpKey->ExpirDay,avpKey->ExpirYear);
//      DisplayMessage(dat,1);
//     }
//    else
//     {
//      sprintf(dat,LoadString(IDS_EXPIR_ALARM_L,"Your AVP license will be expiried\nat %d/%d/%d"),avpKey->ExpirMonth,avpKey->ExpirDay,avpKey->ExpirYear);
//      if(f_will_expir) DisplayMessage(dat,1);
///     }
   }

  // -- Проверка на запуск программы с сервера -------------
  //if(CheckForRemote(toupper(*prgPath)-'A')
  //   &&(!avpKeyFlag||(avpKeyFlag&&!(avpKey->Options&KEY_O_REMOTELAUNCH))))
  // {mb_ok_(254); return;}

  return 0;
 }

#include <stdlib.h>
#include <share.h>
typedef char *PSTR;
typedef USHORT WCHAR;
typedef WCHAR *LPWSTR;
typedef USHORT WORD;
typedef WORD *PWORD;
typedef DWORD *PDWORD;
typedef BYTE BOOLEAN;
typedef void *HANDLE;

#include "PE_Res.H"
PIMAGE_RESOURCE_DIRECTORY pStrs=0;
PIMAGE_RESOURCE_DIRECTORY pRsData=0;
DWORD                           m_rsrcStartRVA;
DWORD                           m_loadAddress;
DWORD                           m_fileDelta;
PIMAGE_RESOURCE_DIRECTORY       m_pRsrcDirHdr;
PIMAGE_RESOURCE_DIRECTORY_ENTRY m_pRsrcIdDirEntries;

PIMAGE_RESOURCE_DIRECTORY GetResourceTypeById(WORD Id)
 {
//  if(!m_rsrcStartRVA ) return 0;

  PIMAGE_RESOURCE_DIRECTORY_ENTRY pCurr = m_pRsrcIdDirEntries;

  // Compare each resource instance Id to the input Id...
  for(unsigned i=0; i<m_pRsrcDirHdr->NumberOfIdEntries; i++, pCurr++ )
    if(Id == pCurr->Id)
     {
      if(pCurr->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY )
        return (PIMAGE_RESOURCE_DIRECTORY)
           ((pCurr->OffsetToData & 0x7FFFFFFF)+(DWORD)m_pRsrcDirHdr);
     }
  return 0;
 }

PIMAGE_RESOURCE_DATA_ENTRY GetNextResourceInst(PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry)
 {
  PIMAGE_RESOURCE_DIRECTORY presDir;
  PIMAGE_RESOURCE_DIRECTORY_ENTRY presDirEntry;

  if(resDirEntry->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY )
   {
    presDir=(PIMAGE_RESOURCE_DIRECTORY)((resDirEntry->OffsetToData & 0x7FFFFFFF)+(DWORD)m_pRsrcDirHdr);
    presDirEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(presDir+1);
    return (PIMAGE_RESOURCE_DATA_ENTRY)
      (presDirEntry->OffsetToData+m_rsrcStartRVA+m_loadAddress+m_fileDelta);
   }
  return 0;
 }

//---------
/*
void DumpResourceDirectory(PIMAGE_RESOURCE_DIRECTORY resDir,
        DWORD resourceBase,DWORD level, DWORD resourceType);

// The predefined resource types
char *SzResourceTypes[] = {
"???_0", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG", "STRING", "FONTDIR",
"FONT", "ACCELERATORS", "RCDATA", "MESSAGETABLE", "GROUP_CURSOR",
"???_13", "GROUP_ICON", "???_15", "VERSION"};

// Get an ASCII string representing a resource type
void GetResourceTypeName(DWORD type, PSTR buffer, UINT cBytes)
 {
  if( type <= 16 )
   strncpy(buffer, SzResourceTypes[type], cBytes);
  else
   sprintf(buffer, "%X", type);
 }

//
// If a resource entry has a string name (rather than an ID), go find
// the string and convert it from unicode to ascii.
//
void GetResourceNameFromId(DWORD id,DWORD resourceBase,PSTR buffer,UINT cBytes)
 {
  PIMAGE_RESOURCE_DIR_STRING_U prdsu;

  // If it's a regular ID, just format it.
  if ( !(id & IMAGE_RESOURCE_NAME_IS_STRING) )
   {
    sprintf(buffer, "%X", id);
    return;
   }

  id &= 0x7FFFFFFF;
  prdsu = (PIMAGE_RESOURCE_DIR_STRING_U)(resourceBase + id);

  // prdsu->Length is the number of unicode characters
  //WideCharToMultiByte(CP_ACP, 0, prdsu->NameString, prdsu->Length,
  //                                        buffer, cBytes, 0, 0);
  //buffer[min(cBytes-1,prdsu->Length) ] = 0;      // Null terminate it!!!
 }

// Dump the information about one resource directory entry.  If the
// entry is for a subdirectory, call the directory dumping routine
// instead of printing information in this routine.
void DumpResourceEntry(PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry,
        DWORD resourceBase,DWORD level)
 {
  UINT i;
  char nameBuffer[128];

//  if(resDirEntry->m_pRrsrDataEntry->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY )
  if(resDirEntry->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY )
   {
    DumpResourceDirectory( (PIMAGE_RESOURCE_DIRECTORY)
            ((resDirEntry->OffsetToData & 0x7FFFFFFF) + resourceBase),
            resourceBase, level, resDirEntry->Name);
    return;
   }

  // Spit out the spacing for the level indentation
  for(i=0; i < level; i++ ) printf("    ");

  if(resDirEntry->Name & IMAGE_RESOURCE_NAME_IS_STRING )
   {
    GetResourceNameFromId(resDirEntry->Name, resourceBase, nameBuffer,
                                              sizeof(nameBuffer));
    printf("Name: %s  Offset: %08X\n",nameBuffer,resDirEntry->OffsetToData);
   }
  else
   printf("ID: %08X  Offset: %08X\n",resDirEntry->Name,resDirEntry->OffsetToData);
 }

// Dump the information about one resource directory.
void DumpResourceDirectory(PIMAGE_RESOURCE_DIRECTORY resDir,
        DWORD resourceBase,DWORD level,DWORD resourceType)
 {
  PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry;
  char szType[64];
  UINT i;

  // Spit out the spacing for the level indentation
  for(i=0;i < level;i++ )  printf("    ");

  // Level 1 resources are the resource types
  if(level==1 && !(resourceType&IMAGE_RESOURCE_NAME_IS_STRING) )
   {
    GetResourceTypeName(resourceType, szType, sizeof(szType) );
   }
  else    // Just print out the regular id or name
   {
    GetResourceNameFromId(resourceType,resourceBase,szType,sizeof(szType) );
   }

  printf("ResDir (%s) Named:%02X ID:%02X TimeDate:%08X Vers:%u.%02u Char:%X\n",
          szType, resDir->NumberOfNamedEntries, resDir->NumberOfIdEntries,
          resDir->TimeDateStamp, resDir->MajorVersion,
          resDir->MinorVersion,resDir->Characteristics);

  resDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(resDir+1);

  for(i=0;i<resDir->NumberOfNamedEntries;i++,resDirEntry++ )
          DumpResourceEntry(resDirEntry,resourceBase,level+1);

  for(i=0;i<resDir->NumberOfIdEntries;   i++,resDirEntry++ )
          DumpResourceEntry(resDirEntry,resourceBase,level+1);
 }

// Given a section name, look it up in the section table and return a
// pointer to the start of its raw data area.
LPVOID GetSectionPtr(PSTR name, PIMAGE_NT_HEADERS pNTHeader, DWORD imageBase)
 {
  PIMAGE_SECTION_HEADER section;
  unsigned i;

  section=(PIMAGE_SECTION_HEADER)(pNTHeader+1);

  for(i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
   {
    if(strnicmp((char*)section->Name,name, IMAGE_SIZEOF_SHORT_NAME) == 0 )
            return (LPVOID)(section->PointerToRawData + imageBase);
   }

  return 0;
 }

// Top level routine called to dump out the entire resource hierarchy
void DumpResourceSection(DWORD base, PIMAGE_NT_HEADERS pNTHeader)
 {
  PIMAGE_RESOURCE_DIRECTORY resDir;

  resDir=(PIMAGE_RESOURCE_DIRECTORY)GetSectionPtr(".rsrc",pNTHeader,(DWORD)base);
  if(!resDir) return;

  printf("Resources\n");
  DumpResourceDirectory(resDir,(DWORD)resDir,0,0);
 }
*/
//---------
typedef struct LOCTYPE
 {
  USHORT   UniType;
  ULONG    ulSize;
  unsigned char* locStr;
 } LOCTYPE;
LOCTYPE pLocType;

char  szLoadStr[CCHMAXPATH];
char* LoadString(ULONG strId,char *defaultStr)
 {
  static ULONG prevId;
  if(pStrs) // No String?  We're done!
   {
    int i;
    if(prevId==strId) return szLoadStr;
    prevId=strId;

    PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry;
    resDirEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pStrs+1);
    ULONG Id;
    PIMAGE_RESOURCE_DATA_ENTRY pStrInst = 0;
    for(i=0;i<pStrs->NumberOfIdEntries;i++,resDirEntry++)
     if((Id=(resDirEntry->NameOffset-1))==(strId&0xffff)>>4)
     if((pStrInst=GetNextResourceInst(resDirEntry))!=0)
      {
       Id=Id<<4;
       PVOID pStrData = (PVOID)(m_loadAddress +pStrInst->OffsetToData +m_fileDelta);

       WORD *Str=(WORD*)pStrData;
       ULONG Len=pStrInst->Size;
       USHORT ii=0;
s:     if(Str[ii]!=0)
        {
         if((USHORT)strId==Id)
          {
           int  j,k;
           for(j=0,k=0;j<Str[ii];j++)
            if((char)Str[ii+j+1]!='&')
             {
              szLoadStr[j-k]=(char)Str[ii+j+1];
              if(pLocType.locStr!=NULL)
               {
                if(((Str[ii+j+1]&0xff00)==pLocType.UniType)&&(szLoadStr[j-k]<=(pLocType.ulSize-2)))//0x400)
                  szLoadStr[j-k]=pLocType.locStr[szLoadStr[j-k]];
               }
//              if((Str[ii+j+1]&0xff00)==0x400)
//               if(szLoadStr[j-k]<0x40)  szLoadStr[j-k]+=0x70;
//               else                     szLoadStr[j-k]+=0xa0;
             }
            else k++;
           szLoadStr[j-k]=0;
           return szLoadStr;
          }
         ii+=Str[ii];
        }
       ii++;
       Id++;
       if(((char*)&Str[ii]-(char*)pStrData)<Len) goto s;
      }
   }
  return defaultStr;
 }

ULONG LoadLocStr(void)
 {
  ULONG ret=1;
  int i;

  pLocType.UniType=0;//x0400;
  pLocType.ulSize=0;
  pLocType.locStr=NULL;
  PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry;
  resDirEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pRsData+1);
  PIMAGE_RESOURCE_DATA_ENTRY pStrInst = 0;
  for(i=0;i<pRsData->NumberOfIdEntries;i++,resDirEntry++)
   {
   if((pStrInst=GetNextResourceInst(resDirEntry))!=0)
    {
     PVOID pStrData=(PVOID)(m_loadAddress +pStrInst->OffsetToData +m_fileDelta);
     pLocType.ulSize=pStrInst->Size;
     if(pLocType.ulSize>=2)
      {
       memcpy(&pLocType.UniType,pStrData,2);
       if((pLocType.locStr=(unsigned char*)Malloc(pStrInst->Size-1))!=0)
       memcpy(pLocType.locStr,(char*)pStrData+2,pStrInst->Size-2);
       pLocType.locStr[pStrInst->Size-2]=0;
       ret=0;
      }
    }
   }
  return ret;
 }

BOOL LoadPEDll(char* DllName)
 {
  int         m_hFile;
  PVOID       m_pMemoryMappedFileBase;

  if((m_hFile=sopen(DllName,O_BINARY|O_RDONLY,SH_DENYNO|SH_COMPAT,S_IWRITE))==-1 )
   {
//    _printf("Couldn't open Dll file\n");
    return FALSE;
   }

  if((m_pMemoryMappedFileBase=Malloc(filelength(m_hFile)))==NULL) return FALSE;
  lseek(m_hFile,0,SEEK_SET);
  if(read(m_hFile,m_pMemoryMappedFileBase,filelength(m_hFile))==-1) return FALSE;

  // Finally... We have a memory mapped file.  Let's now check to make
  // sure it's a valid PE file.
  PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)m_pMemoryMappedFileBase;
  if(IMAGE_DOS_SIGNATURE != pDosHdr->e_magic )  return FALSE;

  if(pDosHdr->e_lfarlc < 0x40 ) // Theoretically, this field must be >=
    return FALSE;                     // 0x40 for it to be a non-DOS executable

  PIMAGE_NT_HEADERS pNTHdr =
    (PIMAGE_NT_HEADERS)((DWORD)m_pMemoryMappedFileBase + pDosHdr->e_lfanew);
  if(IMAGE_NT_SIGNATURE != pNTHdr->Signature ) return FALSE;

  // It's a valid PE file.  Now make sure that there's resouces, and if
  // so, set up the remaining member variables.
  m_loadAddress = (DWORD)m_pMemoryMappedFileBase;

  m_rsrcStartRVA = pNTHdr->OptionalHeader
                      .DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE]
                      .VirtualAddress;
  if(!m_rsrcStartRVA) return FALSE;

  DWORD                           m_rsrcDirRVA;
  m_rsrcDirRVA = m_rsrcStartRVA;

  // Find the section header for the resource section.  We need to know
  // the section header info so that we can calculate the delta between
  // the resource RVA, and its actual offset in the memory mapped file.
  //PIMAGE_SECTION_HEADER section=(PIMAGE_SECTION_HEADER)(pNTHdr+1);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNTHdr );

  //PIMAGE_RESOURCE_DIRECTORY resDir=0;

  for(unsigned i=0;i< pNTHdr->FileHeader.NumberOfSections; i++ )
   {
    DWORD sectStartRVA = pSection->VirtualAddress;
    DWORD sectEndRVA = sectStartRVA + pSection->Misc.VirtualSize;

    if ( (m_rsrcStartRVA >= sectStartRVA) && (m_rsrcStartRVA < sectEndRVA))
      break;

    //if(strnicmp(pSection->Name,".rsrc",IMAGE_SIZEOF_SHORT_NAME) == 0)
    // {
    //  resDir=(PIMAGE_RESOURCE_DIRECTORY)(pSection->PointerToRawData+m_loadAddress);
    //  break;
    // }
    pSection++;
   }

  m_fileDelta = pSection->PointerToRawData - pSection->VirtualAddress;

  // m_pRsrcDirHdr points to the IMAGE_RESOURCE_DIRECTORY
  m_pRsrcDirHdr=
      (PIMAGE_RESOURCE_DIRECTORY)(m_loadAddress+m_rsrcDirRVA+m_fileDelta);

  // m_pRsrcDirEntries points to the first element in the array of
  // IMAGE_RESOURCE_DIRECTORY_ENTRY's
  PIMAGE_RESOURCE_DIRECTORY_ENTRY m_pRsrcDirEntries;
  m_pRsrcDirEntries=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(m_pRsrcDirHdr+1);

  // m_pRsrcDirEntries points to the first non-named element in the array of
  // IMAGE_RESOURCE_DIRECTORY_ENTRY's.  These entries always follow the
  // named entries
  m_pRsrcIdDirEntries = m_pRsrcDirEntries
                      + m_pRsrcDirHdr->NumberOfNamedEntries;

  DWORD                           m_nEntries;
  m_nEntries = m_pRsrcDirHdr->NumberOfNamedEntries
             + m_pRsrcDirHdr->NumberOfIdEntries;

  //Free(m_pMemoryMappedFileBase);
  if(m_hFile != -1) close(m_hFile);

  //if(!resDir) return ;

  // Locate the String resources
  pStrs=GetResourceTypeById((WORD)PE_RT_STRING);
  pRsData=GetResourceTypeById((WORD)PE_RT_RCDATA);
  //DumpResourceSection((DWORD)pDosHdr,pNTHdr);
  if((pStrs!=0)&&(pRsData!=0)) return (BOOL)!LoadLocStr();
  else                         return FALSE;
 }

void usage(void);
int Start(void)
 {
  // initial hook up to the resources
  strcpy(file_name_2,LocFile);
  if(access(file_name_2,0)==-1) strcpy(file_name,prgPath);
  else                          *file_name=0;
  strcat(file_name,file_name_2);
  if(!LoadPEDll(file_name)) //освобождать!
    DisplayMessage(LoadString(IDS_ERRORLOAD_NLS,"Error loading NLS(*.dlk file with localization message)"),0);//Ошибка загрузки

  if(f_help==1) { usage();return 1;}

  About();
#ifndef _INTERN_VER_
  if((fl_load & FL_LOAD_INI)==0) if(prf_data.OtherMessages)DisplayMessage(LoadString(IDS_ERRORLOAD_INI,"Ini-file is not loaded."),0);
  if((fl_load & FL_LOAD_KEY)==0)
   {
    if(prf_data.OtherMessages)
     {
      DisplayMessage(LoadString(IDS_ERRORLOAD_KEY,"AVP Key is not loaded or your AVP license was expiried."),0);
      _puts("\n");
      _puts(LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
      _puts(" (Yes/No)");
      SetCursorOff(TRUE);
      char Chr;
      Chr=(char)getch();
      cprintf("\r\n");
      SetCursorOff(FALSE);
      if((Chr=='y')||(Chr=='Y')) goto abret;
     }
    else goto abret;
   }
  if((fl_load & FL_LOAD_PRF)==0) if(prf_data.OtherMessages) DisplayMessage(LoadString(IDS_ERRORLOAD_PROFILE,"Profile is not loaded."),0);
  if((fl_load & FL_UNSIGN_EXE)==0)
   {
    if(prf_data.OtherMessages)
     {
      DisplayMessage(LoadString(IDS_EXE_DAMAGED,"AvpDos32.exe has been damaged or infected by unknown virus!"),0);
      _puts("\n");
      _puts(LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
      _puts(" (Yes/No)");
      SetCursorOff(TRUE);
      char Chr;
      Chr=(char)getch();
      cprintf("\r\n");
      SetCursorOff(FALSE);
      if((Chr=='y')||(Chr=='Y')) goto abret;
     }
    else goto abret;
   }
#endif
  resume();

  SetCursorOff(FALSE);
  //поднимаем количество открываемыз файлов
  if(prf_data.OpenHandles>=20)
   {
    RMI rmi;
    memset(&rmi,0,sizeof(RMI));
    rmi.EAX=0x6700;
    rmi.EBX=prf_data.OpenHandles;
    SimulateRMInt(0x21,&rmi);
   }


  initWorcStat();
  VirCountArray.SetSize(0,0x200);
  if(!start2())
   {
    flgScan=1;

    ScanAllPaths();
    SetCursorOff(TRUE);

    if(f_help!=1)
     {
      _puts("\r                                                      \r");
      if(flgScan==1)
        _puts(LoadString(IDS_SCANPROC_INCOMPLETE,"Scan process incompleted.\n"));
      else
        _puts(LoadString(IDS_SCANPROC_COMPLETE,"Scan process completed.\n"));

      if(workStat.cek==0) workStat.cek=1;
      _printf("\n%s %6d    %s %6d\n",workStat.Sectors,workStat.sect,workStat.knownVir,workStat.kv);
      _printf("%s %6d    %s %6d\n",workStat.Files,workStat.fil,workStat.virBodies,workStat.vb);
      _printf("%s %6d    %s %6d\n",workStat.Folder,workStat.fold,workStat.Disinfect,workStat.df);
      _printf("%s %6d    %s %6d\n",workStat.Archives,workStat.arch,workStat.Deleted,workStat.deleted);
      _printf("%s %6d    %s %6d\n",workStat.Packed,workStat.pack,workStat.Warning,workStat.warn);
      _printf("                                    %s %6d\n",workStat.Suspis,workStat.susp);
      _printf("%s %6d    %s %6d\n",workStat.countSize,(int)(workStat.CountSize/workStat.cek),workStat.Corript,workStat.cor);
      _printf("%s  %#02d:%#02d:%#02d %s %6d\n\n",workStat.scanTime,workStat.st.hour,workStat.st.minute,workStat.st.second,workStat.IOError,workStat.err);
      //while(getch()!=27);
     }
   }
abret:
  suspend();
//  DosFreeModule(modHandleNLS);
//  WinTerminate(hab);
  return ExitCode;
 }

