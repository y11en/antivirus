#include <os2.h>
#include "tv32def.h"

#define Uses_TFileDialog
#define Uses_TChDirDialog
#define Uses_TDirCollection
#define Uses_TListBox
//#define Uses_TSortedListBox
#define Uses_TFileCollection

#define Uses_TKeys
#define Uses_MsgBox
#define Uses_TIndicator
#define Uses_TStaticText
#define Uses_TInputLine
#define Uses_TButton
#define Uses_TResourceCollection
#define Uses_TDeskTop
#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TSubMenu
//#define Uses_TMenuItem
#include "include\tv.h"
#include "include\tvhelp.h"
#include "consts.h"

const MAXSIZE = 80;

#include <conio.h>
#include <ctype.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
                                     //->кнопка
#include "..\\typedef.h"
#include "..\\locale\\resource.h"
#include "../../Common.Fil/_avpio.h"
#include "avp_os2.h"
#include "avp_rc.h"
#include "memalloc.h"
#include "start.h"
#include "disk.h"
#include "profile.h"
#define __TCheckBox
#define __TNotebook
#define __TPanel
#define __TFile
#define __TViewer
#define __TDirListBoxs
#define __TRadioBattons
#define __TListViews
#include "classes.h"
#include "contain.h"

#include "../../Common.Fil/ScanApi/baseapi.h"
#include "../../Common.Fil/stuff/_CARRAY.H"
#include "loadkeys.h"
#include "../../Common.Fil/AvpComIDs.h"
#include "../../Common.Fil/AvpKeyIDs.h"
#include "../../Common.Fil/property.h"
#include "../../Common.Fil/sign/sign.h"

ULONG functionality=0;
CPtrArray KeyHandleArray;

BOOL needs=0;
BOOL flScanAll=FALSE;
unsigned char f_help=0;                   // 1 - out help screen
unsigned char f_daily=0;                  // 1 - daily

// Возвращает количество дней прошедшее с 0-го года
unsigned long vldate_(unsigned short year,unsigned char month,unsigned char date)
 {
  return year*365+year/4+date+(month-1)*30+month/2-(year%4?2:1);
 }

struct _cmdline cmdline;

struct   dosdate_t cda;

#define MSK1_FLOPPY          0x01
#define MSK1_NETWORK         0x02
#define MSK1_LOCAL           0x04

#define MSK2_MEMORY          0x01
#define MSK2_SECTORS         0x02
#define MSK2_FILES           0x04
#define MSK2_PACKED          0x08
#define MSK2_EXTRACT         0x10
#define MSK2_MAILBASES       0x20
#define MSK2_MAILPLAIN       0x40

#define MSK2_SMART              0
#define MSK2_PROGRAMS           1
#define MSK2_ALLFILES           2
#define MSK2_USERDEF            3

#define MSK2_EXCLUDEFILES    0x01

#define MSK3_REPORTONLY         0
#define MSK3_DISINFECTDIALOG    1
#define MSK3_DISINFECTAUTO      2
#define MSK3_DELETE             3

#define MSK3_COPYINFECTED    0x01
#define MSK3_COPYSUSPICIOUS  0x01

#define MSK4_WARNINGS        0x01
#define MSK4_CA              0x02
#define MSK4_REDUNDANT       0x04
#define MSK4_SHOWOK          0x08
#define MSK4_SHOWPACK        0x10
#define MSK4_SOUND           0x20
#define MSK4_TRACK           0x40

#define MSK4_REPORT          0x01
#define MSK4_APPEND          0x02
#define MSK4_LIMIT           0x04

class TClockView : public TView
 {
public:
  TClockView( TRect& r );
  virtual void draw();
  virtual void update();

private:
  char lastTime[9];
  char curTime[9];
 };

// -------- Clock Viewer functions -------------

TClockView::TClockView( TRect& r ) : TView( r )
 {
  strcpy(lastTime, "        ");
  strcpy(curTime, "        ");
 }

void TClockView::draw()
 {
  TDrawBuffer buf;
  char c =(char)getColor((ushort)2);

  buf.moveChar(0,' ',c,(ushort)size.x);
  buf.moveStr(0,curTime,c);
  writeLine(0,0,(ushort)size.x, 1, buf);
 }

void TClockView::update()
 {
  time_t t = time(0);
  char *date = ctime(&t);

  date[19] = '\0';
  strcpy(curTime, &date[11]);        /* Extract time. */
  if( strcmp(lastTime, curTime) )
   {
    drawView();
    strcpy(lastTime, curTime);
   }
 }

// ---------- TAVP32TVPrg -------------

class TAVP32TVPrg:public TApplication
 {
public:
  TAVP32TVPrg(char *argv[],char *ccsopts,char *ccspath);

  static TMenuBar *initMenuBar(TRect);
  static TStatusLine *initStatusLine(TRect);
  static TDeskTop *initDeskTop(TRect);

  virtual void getEvent(TEvent& event);
  virtual void handleEvent(TEvent& event);

  virtual void idle();              // Updates heap and clock views

private:
  TClockView *clock;                // Clock view
  void aboutDlgBox();               // "About" box
  void shell();                     // DOS shell
  //void outOfMemory();               // For validView() function
 };

TListViews    *list;
TCheckBox     *cbScaner,*nb1cb,*nb2cb,*nb3cb1,*nb3cb2,*nb4cb,*nb4cb1,*nb2cb2;
TRadioBattons *nb2rb,*nb3rb;
TInputLine    *nb2il,*nb3il1,*nb3il2,*nb4il1,*nb4il2,*nb2il2;
TNotebook     *NoteBook;

void setPrf(void)
 {
  ushort mask;

  mask=0;
  //mask|=~(MSK1_FLOPPY|MSK1_NETWORK|MSK1_LOCAL);
  nb1cb->setData(&mask);

  mask=0;
  if(prf_data.Memory)    mask|=MSK2_MEMORY;
  if(prf_data.Sectors)   mask|=MSK2_SECTORS;
  if(prf_data.Files)     mask|=MSK2_FILES;
  //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_UNPACK)))    prf_data.Packed=0;
  if(!(functionality & FN_UNPACK))  prf_data.Packed=0;
  if(prf_data.Packed)    mask|=MSK2_PACKED;
  //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_EXTRACT)))   prf_data.Archives=0;
  if(!(functionality & FN_EXTRACT)) prf_data.Archives=0;
  if(prf_data.Archives)  mask|=MSK2_EXTRACT;
  //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_MAILBASES))) prf_data.MailBases=0;
  if(!(functionality & FN_MAIL))    prf_data.MailBases=0;
  if(prf_data.MailBases) mask|=MSK2_MAILBASES;
  //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_MAILPLAIN))) prf_data.MailPlain=0;
  if(!(functionality & FN_MAIL))    prf_data.MailPlain=0;
  if(prf_data.MailPlain) mask|=MSK2_MAILPLAIN;
  nb2cb->setData(&mask);

  mask=0;
  switch (prf_data.FileMask)
   {
    case 0:
     mask=MSK2_SMART;
     //WinEnableWindow(hWndNB2EF,FALSE);
     break;
    case 1:
     mask=MSK2_PROGRAMS;
     //WinEnableWindow(hWndNB2EF,FALSE);
     break;
    case 2:
     mask=MSK2_ALLFILES;
     //WinEnableWindow(hWndNB2EF,FALSE);
     break;
    case 3:
     mask=MSK2_USERDEF;
     //WinEnableWindow(hWndNB2EF,TRUE);
     break;
   }
  nb2rb->setData(&mask);

  nb2il->setData(prf_data.UserMask);
  mask=0;
  if(prf_data.ExcludeFiles)    mask|=MSK2_EXCLUDEFILES;
  nb2cb2->setData(&mask);
  nb2il2->setData(prf_data.ExcludeMask);

  mask=0;
  switch (prf_data.InfectedAction)
   {
    case 0:
nokey:
     mask=MSK3_REPORTONLY;
     break;
    case 1:
     //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_DISINFECT)))
     if(!(functionality & FN_DISINFECT))
      {
       prf_data.InfectedAction=0;
       goto nokey;
      }
     else
       mask=MSK3_DISINFECTDIALOG;
     break;
    case 2:
     //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_DISINFECT)))
     if(!(functionality & FN_DISINFECT))
      {
       prf_data.InfectedAction=0;
       goto nokey;
      }
     else
       mask=MSK3_DISINFECTAUTO;
     break;
    case 3:
     mask=MSK3_DELETE;
     break;
   }
  nb3rb->setData(&mask);

  mask=0;
  if(prf_data.InfectedCopy)   mask|=MSK3_COPYINFECTED;
  nb3cb1->setData(&mask);

  nb3il1->setData(prf_data.InfectedFolder);
//  WinEnableWindow(hWndNB3EF1,prf_data.InfectedCopy);
  mask=0;
  if(prf_data.SuspiciousCopy) mask|=MSK3_COPYSUSPICIOUS;
  nb3cb2->setData(&mask);
  nb3il2->setData(prf_data.SuspiciousFolder);
//  WinEnableWindow(hWndNB3EF2,prf_data.SuspiciousCopy);

  mask=0;
  if(prf_data.Warnings)      mask|=MSK4_WARNINGS;
  //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_CA)))  prf_data.CodeAnalyser=0;
  if(!(functionality & FN_CA)) prf_data.CodeAnalyser=0;
  if(prf_data.CodeAnalyser)  mask|=MSK4_CA;
  if(prf_data.RedundantScan) mask|=MSK4_REDUNDANT;
  if(prf_data.ShowOK)        mask|=MSK4_SHOWOK;
  if(prf_data.ShowPack)      mask|=MSK4_SHOWPACK;
  if(prf_data.Sound)         mask|=MSK4_SOUND;
//  WinEnableWindow(hWndNB4RBTN7,FALSE);
  nb4cb->setData(&mask);

  mask=0;
  if(prf_data.Report)          mask|=MSK4_REPORT;
  nb4il1->setData(prf_data.ReportFileName);
  nb4il2->setData(prf_data.ReportFileSize);
  if(prf_data.Append)          mask|=MSK4_APPEND;
  if(prf_data.ReportFileLimit) mask|=MSK4_LIMIT;
  if(prf_data.Report!=0)
   {
//    WinEnableWindow(hWndNB4EF1,TRUE);
//    WinEnableWindow(hWndNB4RBTNb,TRUE);
//    WinEnableWindow(hWndNB4RBTNc,TRUE);
//    if(prf_data.ReportFileLimit) WinEnableWindow(hWndNB4EF2,TRUE);
//     else WinEnableWindow(hWndNB4EF2,FALSE);
   }
  else
   {
//    WinEnableWindow(hWndNB4EF1,FALSE);
//    WinEnableWindow(hWndNB4RBTNb,FALSE);
//    WinEnableWindow(hWndNB4RBTNc,FALSE);
//    WinEnableWindow(hWndNB4EF2,FALSE);
   }
  nb4cb1->setData(&mask);
 }

void getPrf(void)
 {
  ushort mask;

  nb2cb->getData(&mask);
  if(mask&MSK2_MEMORY)    prf_data.Memory=1;   else prf_data.Memory=0;
  if(mask&MSK2_SECTORS)   prf_data.Sectors=1;  else prf_data.Sectors=0;
  if(mask&MSK2_FILES)     prf_data.Files=1;    else prf_data.Files=0;
  if(mask&MSK2_PACKED)    prf_data.Packed=1;   else prf_data.Packed=0;
  if(mask&MSK2_EXTRACT)   prf_data.Archives=1; else prf_data.Archives=0;
  if(mask&MSK2_MAILBASES) prf_data.MailBases=1;else prf_data.MailBases=0;
  if(mask&MSK2_MAILPLAIN) prf_data.MailPlain=1;else prf_data.MailPlain=0;

  nb2rb->getData(&mask);
  prf_data.FileMask=0;
  if(mask==MSK2_SMART)    prf_data.FileMask=0;
  if(mask==MSK2_PROGRAMS) prf_data.FileMask=1;
  if(mask==MSK2_ALLFILES) prf_data.FileMask=2;
  if(mask==MSK2_USERDEF)  prf_data.FileMask=3;

  nb2il->getData(prf_data.UserMask);
  nb2cb2->getData(&mask);
  if(mask&MSK2_EXCLUDEFILES) prf_data.ExcludeFiles=1; else prf_data.ExcludeFiles=0;
  nb2il2->getData(prf_data.ExcludeMask);

  nb3rb->getData(&mask);
  prf_data.InfectedAction=0;
  if(mask==MSK3_REPORTONLY)      prf_data.InfectedAction=0;
  if(mask==MSK3_DISINFECTDIALOG) prf_data.InfectedAction=1;
  if(mask==MSK3_DISINFECTAUTO)   prf_data.InfectedAction=2;
  if(mask==MSK3_DELETE)          prf_data.InfectedAction=3;

  nb3cb1->getData(&mask);
  if(mask&MSK3_COPYINFECTED)    prf_data.InfectedCopy=1; else prf_data.InfectedCopy=0;

  nb3il1->getData(prf_data.InfectedFolder);
  nb3cb2->getData(&mask);
  if(mask&MSK3_COPYSUSPICIOUS)  prf_data.SuspiciousCopy=1; else prf_data.SuspiciousCopy=0;
  nb3il2->getData(prf_data.SuspiciousFolder);

  nb4cb->getData(&mask);
  if(mask&MSK4_WARNINGS)      prf_data.Warnings=1;     else  prf_data.Warnings=0;
  if(mask&MSK4_CA)            prf_data.CodeAnalyser=1; else  prf_data.CodeAnalyser=0;
  if(mask&MSK4_REDUNDANT)     prf_data.RedundantScan=1;else  prf_data.RedundantScan=0;
  if(mask&MSK4_SHOWOK)        prf_data.ShowOK=1;       else  prf_data.ShowOK=0;
  if(mask&MSK4_SHOWPACK)      prf_data.ShowPack=1;     else  prf_data.ShowPack=0;
  if(mask&MSK4_SOUND)         prf_data.Sound=1;        else  prf_data.Sound=0;

  nb4cb1->getData(&mask);
  if(mask&MSK4_REPORT)        prf_data.Report=1;          else prf_data.Report=0;
  nb4il1->getData(prf_data.ReportFileName);
  nb4il2->getData(prf_data.ReportFileSize);
  if(mask&MSK4_APPEND)        prf_data.Append=1;          else prf_data.Append=0;
  if(mask&MSK4_LIMIT)        prf_data.ReportFileLimit=1; else prf_data.ReportFileLimit=0;
 }

char scanprofile[CCHMAXPATH]="";

TAVP32TVPrg::TAVP32TVPrg(char *argv[],char *ccsopts,char *ccspath):TProgInit(TAVP32TVPrg::initStatusLine,
                         TAVP32TVPrg::initMenuBar,TAVP32TVPrg::initDeskTop)
 {
  TPalette Pal(cpMyColor,sizeof(cpMyColor)-1);
  getPalette()=Pal;
  deskTop->setState(sfVisible,False);
  deskTop->setState(sfVisible,True);

  TRect r=getExtent();                      // Create the clock view.
  r.a.x=r.b.x-9; r.b.y=r.a.y+1;
  clock=new TClockView( r );
  insert(clock);

  Boolean flEnable;
//  TNotebook *
  lock();
  NoteBook=new TNotebook(TRect(1,1,74,20));
  if(NoteBook!=0)
   {
    TRect rNB(NoteBook->getExtent());  //rNB.grow(-1, -1);
    TPanel *panel=new TPanel(rNB,LoadString(IDS_TAB_LOCATION," ~L~ocation "),0);
    if(panel!=0)
     {
      // Scrollbar
      TScrollBar *sb=new TScrollBar(TRect(3,15,3+rNB.b.x*0.5,16));
      panel->insert(sb);

      // List box
      list=new TListViews(TRect(3,4,3+rNB.b.x*0.5,4+11),sb,NULL);
      //list->newList(dataCollection);
      panel->insert(list);

      unsigned short len;
      unsigned char  szT[CCHMAXPATH];
      int      in;          // file handle
      // -- Загрузка "default.prf" -----------------------------
      set_default_options_(); // Установка опций по умолчанию
      if(loadProfile(DefProfile)!=0)
       _puts("Profile  is not loaded.\n");

      char *tmp_c=ccsopts;

      while(*tmp_c)
       {
        switch(toupper(*++tmp_c))
         {
          case 'I': needs=1; break;                   //yes
          case 'P': cmdline.skip_mbr=1; break;        //yes
          case 'B': cmdline.skip_boot=1; break;       //yes
          case '1': cmdline.one_floppy=1; break;      //yes
          case 'R': prf_data.SubDirectories=0; break; //yes
          case 'M':                                   //yes
            switch(toupper(*(tmp_c+1)))
             {
              case 'D': tmp_c++; prf_data.MailBases=1; break;
              case 'P': tmp_c++; prf_data.MailPlain=1; break;
              default : prf_data.Memory=0;
             }
            break;
          case 'U': prf_data.Packed=0; break;         //yes
          case 'A': prf_data.Archives=0; break;       //yes
          case 'O': prf_data.ShowOK=1; break;         //yes
          case 'K': prf_data.ShowPack=0; break;
          case 'S':
            switch(toupper(*(tmp_c+1)))
             {
              case 'S':
               {
                tmp_c++;
// see in main()
//                cmdline.startScaner=1;
//                scanprofile[0]=0;
//                if(*++tmp_c=='=')
//                 {
//                  if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
//                  else len=(unsigned short)strlen(tmp_c);
//                  strncpy(scanprofile,tmp_c,len);
//                  tmp_c+=len;
//                  scanprofile[len]=0;
//                 }
                break;
               }
              default : prf_data.Sound=0;
             }
            break;
          case '*': prf_data.FileMask=2; break;       //yes
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
                  while((list->pTail!=NULL)&&(list->pTail->Selected==2))
                   {
                    pDel=list->pTail;
                    list->pTail=list->pTail->pNext;
                    Free(pDel);
                   }
                  if(list->pTail!=NULL)
                   {
                    if(list->pTail->pNext!=NULL)
                     {
                      pList=list->pTail;
                      do
                       {
gtNext:                 if(pList->pNext!=NULL)
                         if(pList->pNext->Selected==2)
                          {
                           pDel=pList->pNext;
                           if(pList->pNext==list->pListTop)
                            {
                             list->pListTop=pList;
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
                  tmpExclDir[len]=0;
                  tmp_c+=len;
                  z0=strtok(tmpExclDir,"; ,{}");
                  while(z0)
                   {
                    if(z0[strlen(z0)-1]=='\\') z0[strlen(z0)-1]=0;
                    AddRecord(z0,2,list);
                    z0=strtok(0,"; ,{}");
                   }
                 }
                break;
             }
            break;
//          case '-': if(avpKey!=NULL) prf_data.InfectedAction=2; break; //yes
          case 'E': prf_data.InfectedAction=3; break; //yes
          case 'H': prf_data.CodeAnalyser=0; break;   //yes
          case 'V': prf_data.RedundantScan=1; break;  //yes
          case 'F':                                   //yes
            char defprofile[CCHMAXPATH];
            if(*++tmp_c=='=')
             {
              if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
               else len=(unsigned short)strlen(tmp_c);
              strncpy(defprofile,tmp_c,len);
              defprofile[len]=0;
              tmp_c+=len;
              loadProfile(defprofile);
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
              cmdline.ListFile[len]=0;
              tmp_c+=len;
             }
            break;
        case 'Y': cmdline.skip_dlg=1; break;
        case 'Z': cmdline.abort_disable=1; break;
        case 'L':
          if((*(tmp_c+1))=='-') { tmp_c++; prf_data.ScanRemovable=0;}
          else prf_data.ScanRemovable=1;
          break;
//        case 'N': cmdline.remote=1; break;
//        case 'T':
//         if(*++tmp_c!='=') while(*tmp_c&&*tmp_c!='/') tmp_c++;
//         else
//         {
//          if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
//          else len=(unsigned short)strlen(tmp_c);
//          strncpy(temp_path,tmp_c,len);
//          temp_path[len]=0;
//         }
//         break;
//        case 'L':
//         realid=(unsigned char)toupper(*++tmp_c);
//         if(*++tmp_c=='!') if(set_default_lng(file_name,realid)<=0) return -8;
//         break;
         } // switch(toupper(*++tmp_c))
        while(*tmp_c&&*tmp_c!='/') tmp_c++;
       } // while(*tmp_c)

      if(*ccspath)
       {
        SelectAllRecords(0,list);
        char *_ttop=strtok(ccspath,";");
        while(_ttop)
         {
          //убираем последний слеш если более 1 символа
          if(_ttop[1]&&_ttop[strlen(_ttop)-1]=='\\') _ttop[strlen(_ttop)-1]=0;
          // ситуация * или *:
          if(*_ttop=='*'&&(!_ttop[1]||_ttop[1]==':'&&!(_ttop[2])))
          if(*_ttop=='*'&&(!_ttop[1]||_ttop[1]==':'&&!(_ttop[2])))
           {
            flScanAll=TRUE;
            SelectDiskType(2,1,list);
            if(prf_data.ScanRemovable==1)
             {
              SelectDiskType(1,1,list);
              SelectDiskType(3,1,list);
             }
           }
          else
           if(_ttop[1]==':'&&!_ttop[2]&&
             tolower(*_ttop)>='a'&&tolower(*_ttop)<='z')//&&Disk[toupper(*_ttop)-'A'])
            AddRecord((char*)_ttop,1,list);
           else
            {
             if((!strcmp(_ttop,"."))||(!strcmp(_ttop,".\\"))) AddRecord(start_path,1,list);
             else
              {
               prf_data.Path=1;
               if(access(_ttop,0)!=-1) AddRecord(_ttop,1,list);
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
                    AddRecord((char*)szT,1,list);
                   }
                  else
                   {
                    char *CurrDir;
                    CurrDir=getcwd(NULL,0);
                    AddRecord(CurrDir,1,list);
//                   Free(CurrDir);
                   }
                  prf_data.FileMask=3;
                 }
              }
            }
          _ttop=strtok(NULL,";");
         }
        needs=1;
       }
      list->insertItem(NULL,0);
      list->focusItem(0);

      nb1cb=new TCheckBox(TRect(rNB.b.x*0.6,rNB.b.y*0.25,rNB.b.x*0.6+25,rNB.b.y*(0.25+0.45)),0,3);
      nb1cb->insertChBtn(LoadString(IDS_TAB1_FLOPPY,"Floppy drives"),ID_NB1RBTN1,True,0);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_REMOTESCAN)))
      if(!(functionality & FN_NETWORK)) flEnable=False; else flEnable=True;
      nb1cb->insertChBtn(LoadString(IDS_TAB1_NETWORK,"Network drives"),ID_NB1RBTN2,flEnable,0);
      nb1cb->insertChBtn(LoadString(IDS_TAB1_LOCAL,"Hard drives"),ID_NB1RBTN3,True,0);

      panel->insert(nb1cb);
      TButton *nb1btn3=new TButton(TRect(rNB.b.x*0.6,rNB.b.y*0.75,rNB.b.x*0.6+18,rNB.b.y*0.75+2),LoadString(IDS_TAB1_ADD_FOLDER,"Add folder"),ID_NB1BTN,bfDefault);
      //nb1btn3->options|=ofTopSelect;
      panel->insert(nb1btn3);
      NoteBook->insertPage(panel,"First");
     }

    TPanel *panel2=new TPanel(rNB,LoadString(IDS_TAB_OBJECTS," O~b~jects "),1);
    if(panel2!=0)
     {
      nb2cb=new TCheckBox(TRect(1,3,rNB.b.x*0.5,rNB.b.y-2),0,2);
      nb2cb->insertChBtn(LoadString(IDS_TAB2_MEMORY,"Memory"),ID_NB2RBTN1,True,0);
      nb2cb->insertChBtn(LoadString(IDS_TAB2_SECTORS,"Sectors"),ID_NB2RBTN2,True,0);
      nb2cb->insertChBtn(LoadString(IDS_TAB2_FILES,"Files"),ID_NB2RBTN3,True,0);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_UNPACK)))
      if(!(functionality & FN_UNPACK)) flEnable=False; else flEnable=True;
      nb2cb->insertChBtn(LoadString(IDS_TAB2_PACKED,"Pasked files"),ID_NB2RBTN4,flEnable,0);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_EXTRACT)))
      if(!(functionality & FN_EXTRACT)) flEnable=False; else flEnable=True;
      nb2cb->insertChBtn(LoadString(IDS_TAB2_EXTRACT,"Archives"),ID_NB2RBTN5,flEnable,0);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_MAILBASES)))
      if(!(functionality & FN_MAIL)) flEnable=False; else flEnable=True;
      nb2cb->insertChBtn(LoadString(IDS_TAB2_MAILBASES,"MailDatabases"),ID_NB2RBTN6,flEnable,0);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_MAILPLAIN)))
      if(!(functionality & FN_MAIL)) flEnable=False; else flEnable=True;
      nb2cb->insertChBtn(LoadString(IDS_TAB2_MAILPLAIN,"Plain Mailformats"),ID_NB2RBTN7,flEnable,0);
      panel2->insert(nb2cb);

      nb2rb=new TRadioBattons(TRect(rNB.b.x*0.5+1,4,rNB.b.x-3,11),2);
      nb2rb->insertRbBtn(LoadString(IDS_TAB2_SMART,"Smart"),ID_NB2RB1,True);
      nb2rb->insertRbBtn(LoadString(IDS_TAB2_PROGRAMS,"Programs"),ID_NB2RB2,True);
      nb2rb->insertRbBtn(LoadString(IDS_TAB2_ALLFILES,"All files"),ID_NB2RB3,True);
      nb2rb->insertRbBtn(LoadString(IDS_TAB2_USERDEF,"User defined"),ID_NB2RB4,True);
      panel2->insert(nb2rb);
      nb2il=new TInputLine(TRect(rNB.b.x*0.5+5,12,rNB.b.x-4,13),80);
//  hWndNB2EF=WinCreateWindow(hWndNB2Fr,WC_ENTRYFIELD,(PSZ)NULL,
//      WS_VISIBLE|ES_MARGIN,0,0,0,0,hWndNB2Fr,HWND_TOP,ID_NB2EF,NULL,NULL);
      panel2->insert(nb2il);
      nb2cb2=new TCheckBox(TRect(rNB.b.x*0.5+1,14,rNB.b.x-3,15),0,2);
      nb2cb2->insertChBtn(LoadString(ID_EXCLIDE_FILES,"Exclude files"),ID_NB2CB2,True,0);
      panel2->insert(nb2cb2);
      nb2il2=new TInputLine(TRect(rNB.b.x*0.5+5,16,rNB.b.x-4,17),CCHMAXPATH-1);
      panel2->insert(nb2il2);
      NoteBook->insertPage(panel2,"Second");
     }

    TPanel *panel3=new TPanel(rNB,LoadString(IDS_TAB_ACTIONS," ~A~ctions "),2);
    if(panel3!=0)
     {
      nb3rb=new TRadioBattons(TRect(2,4,rNB.b.x*0.5-2,12),2);
      nb3rb->insertRbBtn(LoadString(IDS_TAB6_REPORTONLY,"Report only"),ID_NB3RB1,True);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_DISINFECT)))
      if(!(functionality & FN_DISINFECT)) flEnable=False; else flEnable=True;
      nb3rb->insertRbBtn(LoadString(IDS_TAB6_DISINFECTDIALOG,"Display Disinfect dialog"),ID_NB3RB2,flEnable);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_DISINFECT)))
      if(!(functionality & FN_DISINFECT)) flEnable=False; else flEnable=True;
      nb3rb->insertRbBtn(LoadString(IDS_TAB6_DISINFECTAUTO,"Disinfect automatically"),ID_NB3RB3,flEnable);
      nb3rb->insertRbBtn(LoadString(IDS_TAB6_DELETE,"Delete object automatically"),ID_NB3RB4,True);
      panel3->insert(nb3rb);
      nb3cb1=new TCheckBox(TRect(2,12,rNB.b.x*0.5-2,13),0,2);
      nb3cb1->insertChBtn(LoadString(IDS_TAB6_COPYINFECTED,"Copy to infected folder"),ID_NB3RBTN1,True,0);
      panel3->insert(nb3cb1);
      nb3il1=new TInputLine(TRect(6,14,rNB.b.x*0.5-3,15),CCHMAXPATH-1);
//  hWndNB3EF1=WinCreateWindow(hWndNB3Fr,WC_ENTRYFIELD,(PSZ)NULL,
//    WS_VISIBLE|ES_MARGIN,0,0,0,0,hWndNB3Fr,HWND_TOP,ID_NB3EF1,NULL,NULL);
      panel3->insert(nb3il1);

      nb3cb2=new TCheckBox(TRect(rNB.b.x*0.5+1,12,rNB.b.x-3,13),0,2);
      nb3cb2->insertChBtn(LoadString(IDS_TAB6_COPYSUSPICIOUS,"Copy to infected folder"),ID_NB3RBTN2,True,0);
      panel3->insert(nb3cb2);
      nb3il2=new TInputLine(TRect(rNB.b.x*0.5+5,14,rNB.b.x-4,15),CCHMAXPATH-1);
//  hWndNB3EF2=WinCreateWindow(hWndNB3Fr,WC_ENTRYFIELD,(PSZ)NULL,
//    WS_VISIBLE|ES_MARGIN,0,0,0,0,hWndNB3Fr,HWND_TOP,ID_NB3EF2,NULL,NULL);
      panel3->insert(nb3il2);
      NoteBook->insertPage(panel3,"Therd");
     }

    TPanel *panel4=new TPanel(rNB,LoadString(IDS_TAB_OPTIONS," ~O~ptions "),3);
    if(panel4!=0)
     {
      nb4cb=new TCheckBox(TRect(1,3,rNB.b.x*0.5-1,rNB.b.y-2),0,2);
      nb4cb->insertChBtn(LoadString(IDS_TAB3_WARNINGS,"Warning"),ID_NB4RBTN1,True,0);
      //if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_CA)))
      if(!(functionality & FN_CA)) flEnable=False; else flEnable=True;
      nb4cb->insertChBtn(LoadString(IDS_TAB3_CA,"Code analyser"),ID_NB4RBTN2,flEnable,0);
      nb4cb->insertChBtn(LoadString(IDS_TAB3_REDUNDANT,"Redindant scan"),ID_NB4RBTN3,True,0);
      nb4cb->insertChBtn(LoadString(IDS_TAB3_SHOWOK,"Show clean object in the log"),ID_NB4RBTN4,True,0);
      nb4cb->insertChBtn(LoadString(IDS_TAB3_SHOWPACK,"Show pack info in the log"),ID_NB4RBTN5,True,0);
      nb4cb->insertChBtn(LoadString(IDS_TAB3_SOUND,"Sound effect"),ID_NB4RBTN6,True,0);
//      nb4cb->insertChBtn(LoadString(IDS_TAB3_TRACK,"Tracking"),ID_NB4RBTN7,True,0);
      panel4->insert(nb4cb);

      nb4cb1=new TCheckBox(TRect(rNB.b.x*0.5,4,rNB.b.x-2,rNB.b.y-4),0,2);
      nb4cb1->insertChBtn(LoadString(IDS_TAB3_REPORT,"R~eport file"),ID_NB4RBTNA,True,2);
      nb4il1=new TInputLine(TRect(rNB.b.x*0.5+3,6,rNB.b.x-4,7),CCHMAXPATH-1);
//  hWndNB4EF1=WinCreateWindow(hWndNB4Fr,WC_ENTRYFIELD,(PSZ)NULL,
//    WS_VISIBLE|ES_MARGIN,0,0,0,0,hWndNB4Fr,HWND_TOP,ID_NB4EF1,NULL,NULL);
      nb4cb1->insertChBtn(LoadString(IDS_TAB3_APPEND,"Appen~d"),ID_NB4RBTNB,True,0);
      nb4cb1->insertChBtn(LoadString(IDS_TAB3_LIMIT,"Limit ~size of report file"),ID_NB4RBTNC,True,2);
      nb4il2= new TInputLine(TRect(rNB.b.x*0.5+3,12,rNB.b.x-4,13),8);
//  hWndNB4EF2=WinCreateWindow(hWndNB4Fr,WC_ENTRYFIELD,(PSZ)NULL,
//    WS_VISIBLE|ES_MARGIN,0,0,0,0,hWndNB4Fr,HWND_TOP,ID_NB4EF2,NULL,NULL);
      panel4->insert(nb4cb1);
      panel4->insert(nb4il1);
      panel4->insert(nb4il2);

      NoteBook->insertPage(panel4,"Fourth");
     }

    setPrf();

    if(cmdline.startScaner==1)
     {
      ushort mask=0;
      TButton *bScaner=new TButton(TRect(2,21,20,23),LoadString(IDS_STARTSCAN,"Start scanner"),ID_CHSCANER,bfDefault);
      TPalette MyButtonPal(cpMyButton,sizeof(cpMyButton)-1);
      bScaner->getPalette()=MyButtonPal;
      bScaner->setState(sfVisible,False);
      bScaner->setState(sfVisible,True);
      deskTop->insert(bScaner);
      //cbScaner=new TCheckBox(TRect(2,21,35,22),0,2);
      //cbScaner->insertChBtn("Start scaner after close.",ID_CHSCANER,False,0);
      //deskTop->insert(cbScaner);
      cmdline.startScaner=0;
     } //deskTop->selectNext(False);

    deskTop->insert(NoteBook);
//    NoteBook->setCurrent(panel,normalSelect);
//    ((TGroup*)(NoteBook->NotPages[1].Page))->select();
//    NoteBook->setCurrent((TGroup*)(NoteBook->NotPages[0].Page),normalSelect);
//    NoteBook->movedTo(0);
    NoteBook->sel=0;
//    NoteBook->setState(sfVisible|sfActive|sfFocused|sfExposed,True);
    NoteBook->NotPages[NoteBook->sel].Page->makeFirst();
    NoteBook->setCurrent((TGroup*)(NoteBook->NotPages[NoteBook->sel].Page),normalSelect);
   }
  unlock();
 }

// "Out of Memory" function ( called by validView() )
//void TAVP32TVPrg::outOfMemory()
// {
//  messageBox("Not enough memory available to complete operation.",mfError|mfOKButton);
// }

//  DOS Shell Command.
void TAVP32TVPrg::shell()
 {
  hideCursor();
  suspend();
  system("cls");
//  cout << "Type EXIT to return...";
  system(getenv("COMSPEC"));
  resume();
  redraw();
  showCursor();
 }

// idle() function ( updates heap and clock views for this program. )
void TAVP32TVPrg::idle()
 {
  TProgram::idle();
  clock->update();
  //heap->update();
  //if(deskTop->firstThat(isTileable, 0) != 0 )
  // {
  //  enableCommand(cmTile);
  //  enableCommand(cmCascade);
  // }
  //else
  // {
  //  disableCommand(cmTile);
  //  disableCommand(cmCascade);
  // }
 }

// About Box function()
void TAVP32TVPrg::aboutDlgBox()
 {
  static TDialog *aboutBox=0;
  if(aboutBox==0)
   {
    //aboutBox=new TDialog(TRect(0, 0,62,17),"About AVP32 for DOS");
    aboutBox=new TDialog(TRect(0, 0,62,19),LoadString(IDS_ABOUT,"About AVP"));

    aboutBox->insert(new TStaticText(TRect(2, 2,60, 5),
      "\003AntiViral Toolkit Pro by Eugene Kaspersky for DOS32\n" // These strings will be
      "\003Copyright(C) Kaspersky Lab. 1998\n" // concatenated by the compiler.
      "\003Version 3.0 build 130"              // The Ctrl-C centers the line.
      ) );
                                       //32,16,43
    aboutBox->insert(new TButton(TRect(26,16,37,18),"~O~k",cmOK,bfDefault) );
//    aboutBox->insert(new TButton(TRect(19,16,30,18),"~S~upport",cmOK,bfDefault) );

    char tmp2[80];
    sprintf(tmp2,"\003%s",LoadString(IDS_REGISTERINFO,"Регистрационные данные:"));
    aboutBox->insert(new TStaticText(TRect(2, 6,60, 7),tmp2 ) );

//    if(avp_key_present)
    if(functionality)
     {
      TListViews *KeyList;

      TScrollBar *KeySB=new TScrollBar(TRect(2,14,60,15));
      aboutBox->insert(KeySB);

      KeyList=new TListViews(TRect(2,8,60,14),KeySB,(short)(KeyHandleArray.GetSize()+2));
      aboutBox->insert(KeyList);
//      KeyList->insertItem(NULL,0);
      char buf[0x200];
      int i;

      KeyList->insertItem("Key name",0xff);
      KeyList->insertItem("________",0xff);
      for(i=0;i<KeyHandleArray.GetSize();i++)
       {
        HDATA hKey=(HDATA)KeyHandleArray[i];
        AVP_dword dseq[5];

        DATA_Get_Val(hKey,DATA_Sequence(dseq,PID_FILENAME,0),0,buf,0x200);
        char* p;
        //GetFullPathName(buf,0x200,buf,&p);
        p=strrchr(buf,'\\');
        if(p) p++;
        else  p=buf;
        //if(strlen(p)<=13) memcpy(szScr,p,strlen(p));
        //else  { memcpy(szScr,p,10); memcpy(&szScr[10],"...",3); }
        KeyList->insertItem(p,0xff);
       }

      KeyList->insertItem("Ser. number",0xff);
      KeyList->insertItem("___________",0xff);
      for(i=0;i<KeyHandleArray.GetSize();i++)
       {
        HDATA hKey=(HDATA)KeyHandleArray[i];
        AVP_dword dseq[5];
        AVP_dword keynumber[3];
        *buf=0;
        if(DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSERIALNUMBER,0),0,keynumber,sizeof(keynumber)) )
         sprintf(buf,"%04d-%06d-%08d",keynumber[0],keynumber[1],keynumber[2]);
        else DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0),0,buf,0x200);
        KeyList->insertItem(buf,0xff);
       }

      KeyList->insertItem("Price pos.",0xff);
      KeyList->insertItem("__________",0xff);
      for(i=0;i<KeyHandleArray.GetSize();i++)
       {
        HDATA hKey=(HDATA)KeyHandleArray[i];
        AVP_dword dseq[5];
        strcpy(buf,"AVP Key v1.0");
        DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOSNAME,0),0,buf,0x200);
        KeyList->insertItem(buf,0xff);
       }

      KeyList->insertItem("Exp. date",0xff);
      KeyList->insertItem("_________",0xff);
      for(i=0;i<KeyHandleArray.GetSize();i++)
       {
        HDATA hKey=(HDATA)KeyHandleArray[i];
        AVP_dword dseq[5];
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
          _sprintf(buf,"%d:%d:%d",stExpir.wDay,stExpir.wMonth,stExpir.wYear);
ok:       ;
         }
        KeyList->insertItem(buf,0xff);
       }

      KeyList->insertItem("Trial",0xff);
      KeyList->insertItem("_____",0xff);
      for(i=0;i<KeyHandleArray.GetSize();i++)
       {
        HDATA hKey=(HDATA)KeyHandleArray[i];
        AVP_dword dseq[5];
        *buf=0;
        if(DATA_Find(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))
         KeyList->insertItem("Yes",0xff);
        else
         KeyList->insertItem("No",0xff);
       }
       //DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLICENCEINFO,0),0,buf+strlen(buf),0x200);
       //_printf("LisInfo:%s ",buf);

//      char *tmpKey=(char*)avp_key;
//      sprintf(tmp2,"\003%s",&tmpKey[avp_key->CopyInfoOffs]);
//      aboutBox->insert(new TStaticText(TRect(2,8,60,9),tmp2 ) );

//      sprintf(tmp2,"\003%s : %s",LoadString(IDS_NAME,"Имя"),&tmpKey[avp_key->NameOffs]);
//      aboutBox->insert(new TStaticText(TRect(2,10,60,11),tmp2 ) );
//      sprintf(tmp2,"\003%s : %s",LoadString(IDS_ORGANIZATION,"Фирма"),&tmpKey[avp_key->OrgOffs]);
//      aboutBox->insert(new TStaticText(TRect(2,11,60,12),tmp2 ) );
//      sprintf(tmp2,"\003%s : %s",LoadString(IDS_REGNUMBER,"Номер"),&tmpKey[avp_key->RegNumberOffs]);
//      aboutBox->insert(new TStaticText(TRect(2,12,60,13),tmp2 ) );
      KeyList->insertItem(NULL,0);
      KeyList->focusItem(1);
     }
    else
     aboutBox->insert(new TStaticText(TRect(2,8, 60,9),"\003Evaluation copy") );

    aboutBox->options |= ofCentered;
   }

  deskTop->execView(aboutBox);
  //destroy( aboutBox );
 }

Boolean fileExists(char *name)
 {
  struct find_t sr;

  if(_dos_findfirst(name,0,&sr)==0) return True;
  else       return False;
 }

#include <Dos.h>
void TAVP32TVPrg::handleEvent(TEvent& event)
 {
  //TEvent em;
  static TFileDialogs *SaveFileDialog=NULL,*LoadFileDialog=NULL,*ViewLofDialog=NULL;
  static TChDirDialogs *ChDirDialog=NULL;
  ULONG mask;

  TApplication::handleEvent(event);
  if(event.what==evCommand)
   {
    unsigned int total;
    switch(event.message.command)
     {
      case ID_FILE_SAVEPROFILEASDEFAULT: //"~S~ave profile as default"
        if(start_path[1] == ':' )
          _dos_setdrive(toupper(start_path[0])-'A'+1,&total);
        chdir(start_path);
        save_profile_(DefProfile);
        clearEvent( event );
        break;
      case ID_FILE_LOADPROFILE:          //"~L~oad profile..."
        if(start_path[1] == ':' )
          _dos_setdrive(toupper(start_path[0])-'A'+1,&total);
        chdir(start_path);
        if(LoadFileDialog==NULL)
          LoadFileDialog=(TFileDialogs *)validView(
            new TFileDialogs("*.prf",LoadString(IDS_LOAD_PROFILE,"Load profile..."),LoadString(ID_PROFILE_NAME,"~P~rofile name"),fdOpenButton,100));
        else LoadFileDialog->readDirectory();
        if(LoadFileDialog!=NULL && deskTop->execView(LoadFileDialog)!=cmCancel)
         {
          char *szFullFile=new char[MAXPATH];
          LoadFileDialog->getFileName(szFullFile);
          if(!fileExists(szFullFile))
           {
            char errorMsg[MAXSIZE];
            strcpy(errorMsg,"Cannot find profile\n");
            strcat(errorMsg,szFullFile);
            messageBox(errorMsg,mfError|mfOKButton);
           }
          else
           {
            set_default_options_(); // Установка опций по умолчанию
            loadProfile(szFullFile);
            setPrf();
            if(list!=0)
             {
              //list->deleteList();
              list->insertItem(NULL,0);
             }
           }
          delete szFullFile;
         }
        //destroy(d);
        clearEvent( event );
        break;
      case ID_FILE_SAVEPROFILE:          //"Save profile ~A~s..."
        if(start_path[1]==':')
          _dos_setdrive(toupper(start_path[0])-'A'+1,&total);
        chdir(start_path);
        if(SaveFileDialog==NULL)
          SaveFileDialog=(TFileDialogs *)validView(
            new TFileDialogs("*.prf",LoadString(IDS_SAVE_PROFILE,"Save profile..."),LoadString(ID_PROFILE_NAME,"~P~rofile name"),fdSaveButton,101));
        else SaveFileDialog->readDirectory();
        if(SaveFileDialog!=NULL && deskTop->execView(SaveFileDialog)!=cmCancel)
         {
          char *fileName=new char[MAXPATH];
          SaveFileDialog->getFileName(fileName);
          if(fileExists(fileName))
           {
            char errorMsg[MAXSIZE];
            strcpy(errorMsg,"Profile exist. Overwrite it?\n");
            strcat(errorMsg,fileName);
            if(messageBox(errorMsg, mfError|mfYesNoCancel)==cmYes)
              save_profile_(fileName);
           }
          else
           save_profile_(fileName);

          delete fileName;
         }
        //destroy( d );
        clearEvent( event );
        break;
      case ID_VIEW_LOG:
        if(start_path[1]==':') _dos_setdrive(toupper(start_path[0])-'A'+1,&total);
        chdir(start_path);
        if(ViewLofDialog==NULL)
          ViewLofDialog=(TFileDialogs *)validView(
            new TFileDialogs("*.txt",LoadString(ID_VIEW_LOG,"Log file..."),LoadString(ID_LOC_FILE_NAME,"~L~og file name"),fdOpenButton,102));
        else ViewLofDialog->readDirectory();
        if(ViewLofDialog!=NULL && deskTop->execView(ViewLofDialog)!=cmCancel)
         {
          char *szLogFile=new char[MAXPATH];
          ViewLofDialog->getFileName(szLogFile);
          if(!fileExists(szLogFile))
           {
            char errorMsg[MAXSIZE];
            strcpy(errorMsg,"Cannot find log file\n");
            strcat(errorMsg,szLogFile);
            messageBox(errorMsg,mfError|mfOKButton);
           }
          else
           {
            TViewWindow *ViewsLog;
            TRect vr=deskTop->getExtent();
            //vr.grow(-2,-3);
            vr.a.x+=2; vr.a.y+=4; vr.b.x-=2; vr.b.y--;
            ViewsLog=(TViewWindow*)validView(new TViewWindow(vr,szLogFile,wnNoNumber));
            //if( !visible) ViewsLog->hide();
            deskTop->insert(ViewsLog);
           }
          delete szLogFile;
         }
        //destroy( d );
        clearEvent( event );
        break;
      case ID_HELP_ABOUTAVP32:    //  About Dialog Box
        aboutDlgBox();
        clearEvent( event );
        break;
      case cmDOS_Cmd:             //  DOS shell
//struct stat statbuf;
//stat("a:\\",&statbuf);
        shell();
        clearEvent( event );
        break;
      case ID_NB1BTN:
        if(start_path[1]==':')
          _dos_setdrive(toupper(start_path[0])-'A'+1,&total);
        chdir(start_path);
        if(ChDirDialog==NULL) ChDirDialog=new TChDirDialogs(LoadString(ID_ADD_DIRECTORY,"Add folder"),cdNormal,102);
        ChDirDialog->setData(start_path);
        if(ChDirDialog!=NULL && deskTop->execView(ChDirDialog)!=cmCancel)
         {
          char *szDir=new char[MAXPATH+1];
          ChDirDialog->getData(szDir);
          list->insertItem(szDir,1);
          delete szDir;
         }
        //destroy(ChDirDialog);
        clearEvent( event );
        break;
      case ID_NB1RBTN1:             //Floppy drives
        nb1cb->getData(&mask);
        if((mask&MSK1_FLOPPY)==MSK1_FLOPPY) SelectDiskType(1,TRUE,list);
        else                      SelectDiskType(1,FALSE,list);
        list->draw();
        clearEvent( event );
        break;
      case ID_NB1RBTN2:             //Network drives
        nb1cb->getData(&mask);
        if((mask&MSK1_NETWORK)==MSK1_NETWORK)SelectDiskType(3,TRUE,list);
        else                      SelectDiskType(3,FALSE,list);
        list->draw();
        clearEvent(event);
        break;
      case ID_NB1RBTN3:             //Hard drives
        nb1cb->getData(&mask);
        if((mask&MSK1_LOCAL)==MSK1_LOCAL)  SelectDiskType(2,TRUE,list);
        else                      SelectDiskType(2,FALSE,list);
        list->draw();
        clearEvent( event );
        break;
      case ID_SETTINGS_LOCATION_L:
        //em.what=evMouseDown;
        //em.mouse.buttons=mbLeftButton;
        //em.mouse.doubleClick=False;//True;
        //em.mouse.where.x=10;
        //em.mouse.where.y=3;
        //putEvent(em);
        NoteBook->sel=0;
        //NoteBook->NotPages[NoteBook->sel].Page->options|=ofTopSelect;
        NoteBook->NotPages[NoteBook->sel].Page->makeFirst();//select();
//        ((TGroup*)(NoteBook->NotPages[0].Page))->putInFrontOf(first());
        NoteBook->setCurrent((TGroup*)(NoteBook->NotPages[NoteBook->sel].Page),normalSelect);
        //NoteBook->movedTo();
        clearEvent(event);
        break;
      case ID_SETTINGS_OBJECTS_L:
        NoteBook->sel=1;
        NoteBook->NotPages[NoteBook->sel].Page->makeFirst();//select();
//        ((TGroup*)(NoteBook->NotPages[0].Page))->putInFrontOf(first());
        NoteBook->setCurrent((TGroup*)(NoteBook->NotPages[NoteBook->sel].Page),normalSelect);
        clearEvent(event);
        break;
      case ID_SETTINGS_ACTIONS_L:
        NoteBook->sel=2;
        NoteBook->NotPages[NoteBook->sel].Page->makeFirst();//select();
//        ((TGroup*)(NoteBook->NotPages[0].Page))->putInFrontOf(first());
        NoteBook->setCurrent((TGroup*)(NoteBook->NotPages[NoteBook->sel].Page),normalSelect);
        clearEvent(event);
        break;
      case ID_SETTINGS_OPTIONS_L:
        NoteBook->sel=3;
        NoteBook->NotPages[NoteBook->sel].Page->makeFirst();//select();
//        ((TGroup*)(NoteBook->NotPages[0].Page))->putInFrontOf(first());
        NoteBook->setCurrent((TGroup*)(NoteBook->NotPages[NoteBook->sel].Page),normalSelect);
        clearEvent(event);
        break;
      case ID_SETTINGS_NEXT_L:
        TEvent en;
        en.what=evKeyDown;
        en.keyDown.keyCode=kbRight;
        putEvent(en);
        clearEvent(event);
        break;
      case ID_SETTINGS_PREV_L:
        TEvent ep;
        ep.what=evKeyDown;
        ep.keyDown.keyCode=kbLeft;
        putEvent(ep);
        clearEvent(event);
        break;
      case ID_SCAN_NOW:
      case ID_CHSCANER: //cbScaner
        if(cmdline.startScaner==1) cmdline.startScaner=0;
        else
         {
          if(fileExists(scanprofile)) unlink(scanprofile);
          save_profile_(scanprofile);

          TEvent e;
          e.what=evCommand;
          e.message.command=cmQuit;
          e.message.infoPtr=this;
          putEvent(e);
          cmdline.startScaner=1;
         }
        clearEvent(event);
        break;
      default: break;
     }
   }
 }

// DemoApp::getEvent()  Event loop to check for context help request

void TAVP32TVPrg::getEvent(TEvent &event)
 {
  TWindow *w;
  THelpFile *hFile;
//  fpstream *helpStrm;
  static Boolean helpInUse = False;

  TApplication::getEvent(event);
  switch (event.what)
   {
    case evCommand:
      if((event.message.command == cmHelp) && ( helpInUse == False))
       {
        helpInUse = True;
//        helpStrm = new fpstream("DEMOHELP.HLP", ios::in|ios::binary);
//        hFile = new THelpFile(*helpStrm);
//        if(!helpStrm)
//         {
//          messageBox("Could not open help file", mfError | mfOKButton);
//          delete hFile;
//         }
//        else
         {
          w = new THelpWindow(hFile,getHelpCtx());
          if(validView(w) != 0)
           {
            execView(w);
            destroy( w );
           }
          clearEvent(event);
         }
        helpInUse = False;
       }
      break;
    case  evKeyDown:
      switch( ctrlToArrow(event.keyDown.keyCode) )
       {
        case  kbF10:
          TEvent e;
          e.what=evCommand;
          e.message.command=cmMenu;
          e.message.infoPtr=this;
          putEvent(e);
          break;
       }
      break;
    case evMouseDown:
      if(event.mouse.buttons != 1)
        event.what = evNothing;
      break;
   }
 }

TDeskTop *TAVP32TVPrg::initDeskTop(TRect r)
 {
  r.a.y++;
  //r.b.y--;
  //TMyDeskTop* TMD=new TMyDeskTop(r);
  return new TDeskTop(r);
 }

#define ID_APP_EXIT                     0xE141
#define ID_CONTEXT_HELP                 0xE145      // shift-F1
#define ID_HELP                         0xE146      // first attempt for F1
#define ID_DEFAULT_HELP                 0xE147      // last attempt

TMenuBar *TAVP32TVPrg::initMenuBar( TRect r )
 {
  r.b.y = r.a.y+1;

  TSubMenu& sub1=
   *new TSubMenu(LoadString(ID_MENU_FILE,"~F~ile"),0,hcNoContext)+
     *new TMenuItem(LoadMenuString(ID_FILE_SAVEPROFILEASDEFAULT,"~S~ave profile as default"),ID_FILE_SAVEPROFILEASDEFAULT,getAltCode(hotKey(LoadMenuString(ID_FILE_SAVEPROFILEASDEFAULT,"~S~ave profile as default")))/*kbAltS*/,hcNoContext)+
     newLine() +
     *new TMenuItem(LoadMenuString(ID_FILE_LOADPROFILE,"~L~oad profile..."),ID_FILE_LOADPROFILE,getAltCode(hotKey(LoadMenuString(ID_FILE_LOADPROFILE,"~L~oad profile...")))/*kbAltL*/,hcNoContext)+
     *new TMenuItem(LoadMenuString(ID_FILE_SAVEPROFILE,"Save profile ~A~s..."),ID_FILE_SAVEPROFILE,getAltCode(hotKey(LoadMenuString(ID_FILE_SAVEPROFILE,"Save profile ~A~s...")))/*kbAltA*/,hcNoContext)+
     newLine() +
     *new TMenuItem(LoadString(ID_MENU_VIEWLOG,"~V~iew log..."),ID_VIEW_LOG,getAltCode(hotKey(LoadString(ID_MENU_VIEWLOG,"~V~iew log...")))/*kbAltV*/,hcNoContext)+
     newLine() +
     *new TMenuItem("~D~OS Shell", cmDOS_Cmd,getAltCode(hotKey("~D~OS Shell"))/*kbAltD*/, hcFDosShell)+
     newLine() +
     *new TMenuItem(LoadMenuString(ID_APP_EXIT,"E~x~it"),cmQuit,getAltCode(hotKey(LoadMenuString(ID_APP_EXIT,"E~x~it")))/*kbAltX*/,hcFExit,"Alt-X");

  TSubMenu& sub2 =
   *new TSubMenu(LoadString(ID_MENU_SCAN,"~S~can"),0,hcHelp)+
     *new TMenuItem(LoadMenuString(ID_SCAN_NOW,"~S~can now"),ID_SCAN_NOW,getAltCode(hotKey(LoadMenuString(ID_SCAN_NOW,"~S~can now")))/*kbAltS*/,hcNoContext)
//     +*new TMenuItem(LoadMenuString(ID_STOP,"Sto~p~"),ID_STOP,kbAltA*/,hcHAbout)
     ;

  TSubMenu& sub3 =
   *new TSubMenu(LoadString(ID_MENU_SETTINGS,"S~e~ttings"),0,hcHelp)+
     *new TMenuItem(LoadString(ID_SETTINGS_LOCATION_L,"~L~ocation"),ID_SETTINGS_LOCATION_L,getAltCode(hotKey(LoadString(ID_SETTINGS_LOCATION_L,"~L~ocation")))/*kbAltL*/,hcNoContext)+
     *new TMenuItem(LoadString(ID_SETTINGS_OBJECTS_L,"O~b~jects"),ID_SETTINGS_OBJECTS_L,getAltCode(hotKey(LoadString(ID_SETTINGS_OBJECTS_L,"O~b~jects")))/*kbAltB*/,hcNoContext)+
     *new TMenuItem(LoadString(ID_SETTINGS_ACTIONS_L,"~A~ctions"),ID_SETTINGS_ACTIONS_L,getAltCode(hotKey(LoadString(ID_SETTINGS_ACTIONS_L,"~A~ctions")))/*kbAltA*/,hcNoContext)+
     *new TMenuItem(LoadString(ID_SETTINGS_OPTIONS_L,"~O~ptions"),ID_SETTINGS_OPTIONS_L,getAltCode(hotKey(LoadString(ID_SETTINGS_OPTIONS_L,"~O~ptions")))/*kbAltO*/,hcNoContext)+
//     *new TMenuItem(LoadMenuString(ID_SETTINGS_STATISTICS,"S~t~atistics"),ID_SETTINGS_STATISTICS,kbAltT*/,hcNoContext)+
     newLine() +
     *new TMenuItem(LoadString(ID_SETTINGS_NEXT_L,"~N~ext page"),ID_SETTINGS_NEXT_L,getAltCode(hotKey(LoadString(ID_SETTINGS_NEXT_L,"~N~ext page")))/*kbAltN*/,hcNoContext)+
     *new TMenuItem(LoadString(ID_SETTINGS_PREV_L,"P~r~evious page"),ID_SETTINGS_PREV_L,getAltCode(hotKey(LoadString(ID_SETTINGS_PREV_L,"P~r~evious page")))/*kbAltR*/,hcNoContext)
     ;

  TSubMenu& sub4 =
   *new TSubMenu(LoadString(ID_MENU_HELP,"~H~elp"),0,hcHelp)+
//     *new TMenuItem(LoadMenuString(ID_DEFAULT_HELP,"~C~ontents"),ID_HELP_CONTENTS,kbAltC,hcNoContext)+
//     newLine() +
     *new TMenuItem(LoadMenuString(ID_HELP_ABOUTAVP32,"~A~bout AVP..."),ID_HELP_ABOUTAVP32,getAltCode(hotKey(LoadMenuString(ID_HELP_ABOUTAVP32,"~A~bout AVP...")))/*kbAltA*/,hcHAbout)
     //+ *new TMenuItem( "~F~ilebox...", FileBoxCmd, kbAltB ) +
     //*new TMenuItem( "Ca~l~endar", cmCalendarCmd, kbNoKey,hcHCalendar
     ;

    r.b.y =r.a.y+1;
    if(cmdline.startScaner==1)
     return (new TMenuBar(r,sub1+sub2+sub3+sub4));
    else
     return (new TMenuBar(r,sub1+sub3+sub4));
 }

TStatusLine *TAVP32TVPrg::initStatusLine(TRect r)
 {
  r=r;
//  r.a.y = r.b.y-1;
  return 0;//new TStatusLine( r,
//      *new TStatusDef( 0, 0xFFFF ) +
//          *new TStatusItem( "~Ctrl-Ins~ Filebox", kbCtrlIns, FileBoxCmd ) +
//          *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
//          *new TStatusItem( 0, kbF10, cmMenu )
//          );
 }

// prg_error - хранит номер произошедшей ошибки
// prg_error=0  - нет ошибки prg_error>0      - ошибка
unsigned char prg_error=0;

//unsigned char f_was_expir=0;      // =1 - вывести предупреждение, что лицензия
                                  // закончилась
//unsigned char f_will_expir=0;     // =1 - вывести предупреждение, когда
                                  // закончиться лицензия
//unsigned char f_i_every_launch=0; // =1 - вывести "Copy Info" при запуске

char *start_path;//[CCHMAXPATH];  // Полный путь, откуда был запущен AVP
char prg_path[CCHMAXPATH];        // Полный путь, где находится AVP

char file_name[CCHMAXPATH],file_name_2[CCHMAXPATH];  // Буфера

char LocaleStr[128];

VOID DisplayMessage(PCHAR szStr,BOOL flStop)
 {
  _printf("\n%s\n\n",szStr);
  if(flStop)
   if(cmdline.skip_dlg!=1)
    {
     _puts("Press any key.\n");
     getch();
    }
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
PIMAGE_RESOURCE_DIRECTORY pMenuData=0;
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
              szLoadStr[j+k]=(char)Str[ii+j+1];
              if(pLocType.locStr!=NULL)
               {
                if(((Str[ii+j+1]&0xff00)==pLocType.UniType)&&(szLoadStr[j+k]<=(pLocType.ulSize-2)))//0x400)
                  szLoadStr[j+k]=pLocType.locStr[szLoadStr[j+k]];
               }
//              if((Str[ii+j+1]&0xff00)==0x400)
//               if(szLoadStr[j-k]<0x40)  szLoadStr[j-k]+=0x70;
//               else                     szLoadStr[j-k]+=0xa0;
             }
            else
             {
              szLoadStr[j+k]='~';
              j++;
              szLoadStr[j+k]=(char)Str[ii+j+1];
              if(pLocType.locStr!=NULL)
               {
                if(((Str[ii+j+1]&0xff00)==pLocType.UniType)&&(szLoadStr[j+k]<=(pLocType.ulSize-2)))//0x400)
                  szLoadStr[j+k]=pLocType.locStr[szLoadStr[j+k]];
               }
              k++;
              szLoadStr[j+k]='~';
             }
           szLoadStr[j+k]=0;
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

char* LoadMenuString(ULONG strId,char *defaultStr)
 {
  if(pMenuData) // No String?  We're done!
   {
    int i;

    PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry;
    resDirEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pMenuData+1);
    PIMAGE_RESOURCE_DATA_ENTRY pStrInst = 0;
    for(i=0;i<pMenuData->NumberOfIdEntries;i++,resDirEntry++)
     if((pStrInst=GetNextResourceInst(resDirEntry))!=0)
      {
       PVOID pStrData=(PVOID)(m_loadAddress+pStrInst->OffsetToData+m_fileDelta);

       WORD *Str=(WORD*)pStrData;
       ULONG Len=pStrInst->Size;
       USHORT ii=0;
s:     if(Str[ii]!=0)
        {
         if((USHORT)strId==Str[ii])
          {
           int  j,k;
           for(j=0,k=0;(char)Str[ii+j+1]!=0;j++)
            if((char)Str[ii+j+1]!='&')
             {
              szLoadStr[j+k]=(char)Str[ii+j+1];
              if(pLocType.locStr!=NULL)
               {
                if(((Str[ii+j+1]&0xff00)==pLocType.UniType)&&(szLoadStr[j+k]<=(pLocType.ulSize-2)))//0x400)
                  szLoadStr[j+k]=pLocType.locStr[szLoadStr[j+k]];
               }
             }
            else
             {
              szLoadStr[j+k]='~';
              j++;
              szLoadStr[j+k]=(char)Str[ii+j+1];
              if(pLocType.locStr!=NULL)
               {
                if(((Str[ii+j+1]&0xff00)==pLocType.UniType)&&(szLoadStr[j+k]<=(pLocType.ulSize-2)))//0x400)
                  szLoadStr[j+k]=pLocType.locStr[szLoadStr[j+k]];
               }
              k++;
              szLoadStr[j+k]='~';
             }
           szLoadStr[j+k]=0;
           return szLoadStr;
          }
         else
          {
           if(Str[ii]==0x0080) ii++; //separator
           else while(Str[ii]!=0) ii++;
          }
         //ii+=Str[ii];
        }
       else
        ii++;
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
    DWORD sectStartRVA=pSection->VirtualAddress;
    DWORD sectEndRVA=sectStartRVA+pSection->Misc.VirtualSize;

    if( (m_rsrcStartRVA>=sectStartRVA) && (m_rsrcStartRVA < sectEndRVA))
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
  pMenuData=GetResourceTypeById((WORD)PE_RT_MENU);
  //DumpResourceSection((DWORD)pDosHdr,pNTHdr);
  if((pStrs!=0)&&(pRsData!=0)&&(pMenuData!=0)) return (BOOL)!LoadLocStr();
  else                         return FALSE;
 }

#include <time.h>

#ifdef __cplusplus
   extern "C" {
#endif
void *_memcpy(void *__s1, const void *__s2, size_t __n );
void *_memmove( void *__s1, const void *__s2, size_t __n );
char *_strcpy( char *__s1, const char *__s2 );
char *_strncpy( char *__s1, const char *__s2, size_t __n );
time_t _time(time_t *__timer );
void _free( void *__ptr );
void *_malloc( size_t __size );
size_t _wcslen( const wchar_t *string );
wchar_t *_wcscpy( wchar_t *strDestination, const wchar_t *strSource );
#ifdef __cplusplus
   }
#endif
void *_memcpy(void *__s1, const void *__s2, size_t __n )
 { return memcpy(__s1,__s2,__n); }
void *_memmove( void *__s1, const void *__s2, size_t __n )
 { return memmove(__s1,__s2,__n); }
char *_strcpy( char *__s1, const char *__s2 )
 { return strcpy(__s1,__s2 ); }
char *_strncpy( char *__s1, const char *__s2, size_t __n )
 { return strncpy(__s1,__s2,__n ); }
time_t _time(time_t *__timer )
 { return time(__timer);}
void _free( void *__ptr )
 { Free(__ptr); }
void *_malloc( size_t __size)
 { return Malloc(__size);}
size_t _wcslen(const wchar_t *string)
 {
  string=string;
  return 0;
 }
wchar_t *_wcscpy(wchar_t *strDestination, const wchar_t *strSource)
 {
  strDestination=strDestination; strSource=strSource;
  return 0;
 }

char *GNAM(char *_ttop)
 {
  return (strrchr(_ttop,'\\')?
    (1+strrchr(_ttop,'\\')):
      (strrchr(_ttop,':')?(1+strrchr(_ttop,':')):_ttop));
 }

int main(int argc,char *argv[])
 {
  char ccsopts[0x50];
  char ccspath[0x100];

  ccsopts[0]=0;
  ccspath[0]=0;

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
  cmdline.startScaner=0;

  strcpy(prg_path,*argv);
  for(unsigned char z=1;z<argc;z++)
   {
    if(*argv[z]=='/')
     {
      if( (toupper(argv[z][1])=='S')&&(toupper(argv[z][2])=='S'))
       {
        cmdline.startScaner=1;
        scanprofile[0]=0;
        if(argv[z][3]=='=')
         {
          int len;
          if(strchr(&argv[z][4],'/')) len=(unsigned char)(strchr(&argv[z][4],'/')-&argv[z][4]);
          else len=(unsigned short)strlen(&argv[z][4]);
          strncpy(scanprofile,&argv[z][4],len);
          scanprofile[len]=0;
         }
       }
      else
       if((strlen(ccsopts)+strlen(argv[z])+1)<0x50) strcat(ccsopts,argv[z]);
     }
    else
     if((strlen(ccspath)+strlen(argv[z])+2)<0x100)
       {strcat(ccspath,argv[z]); strcat(ccspath,";");}
   }

  // -- получить полный путь откуда был запущен AVP --
  start_path=getcwd(NULL,0);

  LoadIO();
  if(pmAllocMem(SIZEALLOC,&SS.selector,&SS.segment))
   {
    if(load_ini("avpdos32.ini")!=0)
      _puts("INI-file is not loaded.\n");

    _dos_getdate(&cda);                       // получить текущую дату

    // -- Загрузка "avp.key" ---------------------------
    //strcpy(file_name_2,KeyFile);
    //if(access(file_name_2,0)==-1)
    // strcpy(file_name,prg_path);
    //else
    // *file_name=0;
    //strcat(file_name,file_name_2);
    SearchFile(KeyFile,file_name,".key");
    //avp_key_flag=load_key_file_(file_name);
    DWORD Level=0;
    BOOL Valid=0;

    functionality=0;

    if(strlen(file_name)>0)
     functionality|=LoadKeys(AVP_APID_SCANNER_DOS_32,
                file_name, &Level, &Valid, &KeyHandleArray);
     functionality|=LoadKeys(AVP_APID_SCANNER_DOS_LITE,
                file_name, &Level, &Valid, &KeyHandleArray);
    if(strlen(KeysPath)==0) strcpy(KeysPath,prg_path);
    if(KeysPath[strlen(KeysPath)-1]!='\\') strcat(KeysPath,"\\*.key");
    else strcat(KeysPath,"*.key");
    functionality|=LoadKeys(AVP_APID_SCANNER_DOS_32,
                KeysPath, &Level, &Valid, &KeyHandleArray);
    functionality|=LoadKeys(AVP_APID_SCANNER_DOS_LITE,
                KeysPath, &Level, &Valid, &KeyHandleArray);

    //functionality|=LoadKeys(start_path,&KeyHandleArray,AVP_APID_SCANNER_DOS);
    functionality|=LoadKeys(AVP_APID_SCANNER_DOS_32,
                start_path, &Level, &Valid, &KeyHandleArray);
    functionality|=LoadKeys(AVP_APID_SCANNER_DOS_LITE,
                start_path, &Level, &Valid, &KeyHandleArray);
    if(!functionality)
     {
     //tPage2.m_Packed   &= !!(functionality & FN_UNPACK);
     //tPage2.m_Archives &= !!(functionality & FN_EXTRACT);
     //tPage2.m_MailBases&= !!(functionality & FN_MAIL);
     //tPage2.m_MailPlain&= !!(functionality & FN_MAIL);
     //tPage3.m_CA       &= !!(functionality & FN_CA);
     //if(!(functionality & FN_DISINFECT) && tPage6.m_Action<3)  tPage6.m_Action = 0;
     _puts("\nAVP Key is not loaded or your AVP license was expiried\n");
    }
   else  //avpKeyFlag=1;
//    if(avp_key_flag)
     {
//      if(avp_key->Flags&KEY_F_INFOEVERYLAUNCH) f_i_every_launch=1;
//      if(avp_key->ExpirMonth&&avp_key->ExpirDay)
       {
//        if(vldate_(cda.year,cda.month,cda.day)>
//          vldate_(avp_key->ExpirYear,(unsigned char)avp_key->ExpirMonth,(unsigned char)avp_key->ExpirDay))
         {
//          f_was_expir=1;
//          avp_key_flag=0;
         }
//        else
//         if(vldate_(cda.year,cda.month,cda.day)+avp_key->AlarmDays>
//           vldate_(avp_key->ExpirYear,(unsigned char)avp_key->ExpirMonth,(unsigned char)avp_key->ExpirDay))
//        f_will_expir=1;
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

    // initial hook up to the resources
    strcpy(file_name_2,LocFile);
    if(access(file_name_2,0)==-1) strcpy(file_name,prg_path);
    else    *file_name=0;
    strcat(file_name,file_name_2);
    if(!LoadPEDll(file_name)) //освобождать!
      DisplayMessage("Error loading NLS(*.dlk file with localization message)",0);//Ошибка загрузки

    char *curpath;
    unsigned drives;

    curpath=getcwd(NULL,0);

    if(prg_path[1]==':') _dos_setdrive(tolower(prg_path[0])-'a'+1,&drives);
    chdir(prg_path);

    TAVP32TVPrg AVP32TVPrg(argv,ccsopts,ccspath);
//    if((fl_load & FL_LOAD_INI)==0) if(prf_data.OtherMessages)DisplayMessage("Ini-file is not loaded.",0);
//    if((fl_load & FL_LOAD_KEY)==0) if(prf_data.OtherMessages)DisplayMessage("AVP Key is not loaded or your AVP license was expiried.",0);
//    if((fl_load & FL_LOAD_PRF)==0) if(prf_data.OtherMessages)DisplayMessage("Profile is not loaded.",0);
//    if((fl_load & FL_UNSIGN_EXE)==0) if(prf_data.OtherMessages)DisplayMessage("AvpDos32.exe has been damaged or infected by unknown virus!",0);
    AVP32TVPrg.run();

    _dos_setdrive(tolower(curpath[0])-'a'+1,&drives);
    chdir(curpath);
   }

  puts("");
  return cmdline.startScaner;
 }
