#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <c:\lang\watcom\h\sys\types.h>
#include <c:\lang\watcom\h\sys\stat.h>
#include "profile.h"
#include "..\locale\resource.h"
#include "avp_os2.h"
#include "start.h"
#include "disk.h"
#include "kernel.h"
#include "report.h"

int   hReportFile=-1;   // handle рапорт файла
ULONG reportLimSize=0; // огpаничение pазмеpа pапоpт файла
ULONG NewPointer=0;    //адрес нового положения
char szExtRep[0x200]="\r\n\r\nCommand line: ";

void reportInit(void)
 {
  struct stat statbuf;
  CHAR  Folderpath[CCHMAXPATH];
  struct dosdate_t scandate;
  struct dostime_t scantime;

  _dos_getdate(&scandate);                       // получить текущую дату
  _dos_gettime(&scantime);

  workStat.cek=scandate.day*86400+scantime.hour*3600+scantime.minute*60+scantime.second;

  if(prf_data.Report)
   {
    if(prf_data.ReportFileName[1]!=':')
     if(prf_data.ReportFileName[0]=='\\')
      sprintf(Folderpath,"%s%s",prgPath,prf_data.ReportFileName);
     else
      sprintf(Folderpath,"%s\\%s",prgPath,prf_data.ReportFileName);
    else
      sprintf(Folderpath,"%s",prf_data.ReportFileName);
    if(Folderpath[strlen(Folderpath)-1]=='\\') Folderpath[strlen(Folderpath)-1]=0;

    if((hReportFile=sopen(prf_data.ReportFileName,
      (prf_data.Append)?(O_WRONLY|O_CREAT|O_BINARY):
                        (O_WRONLY|O_CREAT|O_TRUNC|O_BINARY),SH_DENYNO,S_IWRITE))!=(-1))
     {
      if(fstat(hReportFile,&statbuf)) NewPointer=statbuf.st_size;
      if(prf_data.ReportFileLimit) //&& (statbuf.st_size/1024)>tPage3.m_Size)
       {
        reportLimSize=atol(prf_data.ReportFileSize)*1024;
        if(reportLimSize<NewPointer) chsize(hReportFile,reportLimSize);
       }
      if(prf_data.Append) lseek(hReportFile,0,SEEK_END);

//    DosSetFilePtr(hReportFile,(LONG)0,(LONG)FILE_END,&NewPointer);

     char s[CCHMAXPATH]; // Antiviral Toolkit Pro by Eugene Kaspersky
     reportPutS("\xfe");
     sprintf(s,LoadString(IDS_REPORT_BANNER_L,"   AVP for DOS32 start %02d-%02d-%02d %02d:%02d:%02d\r\n"),
           scandate.day,scandate.month,scandate.year-1900,scantime.hour,scantime.minute,scantime.second);
     reportPutS(s);
     if(prf_data.ExtReport==1)
      {
       sprintf(s,"         Version 3.0  build 132             \r\n");
       reportPutS(s);
       reportPutS("  ");
       sprintf(s,LoadString(IDS_LAST_UPDATE_L,"Last update: %02d.%02d.%04d, %d records."),SurBase.LastUpdateDay,SurBase.LastUpdateMonth,SurBase.LastUpdateYear,SurBase.RecCount);
       reportPutS(s);
       reportPutS(szExtRep);

       reportPutS("\r\n");
      }
    }
   else
    {
     prf_data.Report=0;
//   mb_ok_(219);
    }
   }
 }

// Записать строку в рапорт файл
void reportPutS(char *str)
 {
  ULONG Dlin;
  if((prf_data.Report)&&(hReportFile!=-1)) //while(*str) reportPutC(*str++);
   {
    if((reportLimSize>(NewPointer+strlen(str)))||(reportLimSize==0))
     {
      write(hReportFile,str,strlen(str));
      NewPointer+=strlen(str);
     }
    else
     {
      Dlin=reportLimSize-NewPointer;
      if(Dlin<strlen(str))
       {
        write(hReportFile,str,Dlin);
        NewPointer+=Dlin;
       }
      else
       {
        write(hReportFile,str,strlen(str));
        NewPointer+=strlen(str);
       }
     }
//    DosResetBuffer(hReportFile);
   }
 }

void reportForEach(char* szCurName)
 {
  CHAR TmpStr[CCHMAXPATH];
  struct dostime_t scantime;
  struct dosdate_t scandate;

  _dos_getdate(&scandate);                       // получить текущую дату
  _dos_gettime(&scantime);
  workStat.cekForEach=scandate.day*86400+scantime.hour*3600+scantime.minute*60+scantime.second-workStat.cekForEach;
  if(workStat.cekForEach==0) workStat.cekForEach=1;
  workStat.stForEach.second=(UCHAR)((workStat.cekForEach%3600)%60);
  workStat.stForEach.minute=(UCHAR)((workStat.cekForEach%3600)/60);
  workStat.stForEach.hour=(UCHAR)(workStat.cekForEach/3600);

  if((prf_data.Report)&&(hReportFile!=-1))
   {
    //  send_param_ptr=xxx;
    sprintf(TmpStr,"\r\nCurrent object: %s\r\n",szCurName);
    reportPutS(TmpStr);
    sprintf(TmpStr,"\r\n%s %6d    %s %6d\r\n",workStat.Sectors,workStat.sectForEach,workStat.knownVir,workStat.kvForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Files,workStat.filForEach,workStat.virBodies,workStat.vbForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Folder,workStat.foldForEach,workStat.Disinfect,workStat.dfForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Archives,workStat.archForEach,workStat.Deleted,workStat.deletedForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Packed,workStat.packForEach,workStat.Warning,workStat.warnForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"                                    %s %6d\r\n",workStat.Suspis,workStat.suspForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.countSize,(int)(workStat.CountSizeForEach/workStat.cekForEach),workStat.Corript,workStat.corForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s  %#02d:%#02d:%#02d %s %6d\r\n\r\n",workStat.scanTime,workStat.stForEach.hour,workStat.stForEach.minute,workStat.stForEach.second,workStat.IOError,workStat.errForEach);
    reportPutS(TmpStr);
   }
 }

// Завершить работу с рапорт файлом
void reportDone(void)
 {
  CHAR TmpStr[CCHMAXPATH];
  struct dostime_t scantime;
  struct dosdate_t scandate;

  _dos_getdate(&scandate);                       // получить текущую дату
  _dos_gettime(&scantime);
  workStat.cek=scandate.day*86400+scantime.hour*3600+scantime.minute*60+scantime.second-workStat.cek;
  if(workStat.cek==0) workStat.cek=1;
  workStat.st.second=(UCHAR)((workStat.cek%3600)%60);
  workStat.st.minute=(UCHAR)((workStat.cek%3600)/60);
  workStat.st.hour=(UCHAR)(workStat.cek/3600);

  if((prf_data.Report)&&(hReportFile!=-1))
   {
    //  send_param_ptr=xxx;
    sprintf(TmpStr,"\r\n%s\r\n",
      (flgScan==1)?
        LoadString(IDS_SCANPROC_INCOMPLETE,"Scan process incompleted.\n"):
        LoadString(IDS_SCANPROC_COMPLETE,"Scan process completed.\n"));
    reportPutS(TmpStr);
    reportPutS("\r\nResult for all objects:\r\n");
    sprintf(TmpStr,"\r\n%s %6d    %s %6d\r\n",workStat.Sectors,workStat.sect,workStat.knownVir,workStat.kv);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Files,workStat.fil,workStat.virBodies,workStat.vb);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Folder,workStat.fold,workStat.Disinfect,workStat.df);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Archives,workStat.arch,workStat.Deleted,workStat.deleted);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.Packed,workStat.pack,workStat.Warning,workStat.warn);
    reportPutS(TmpStr);
    sprintf(TmpStr,"                                    %s %6d\r\n",workStat.Suspis,workStat.susp);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6d\r\n",workStat.countSize,(int)(workStat.CountSize/workStat.cek),workStat.Corript,workStat.cor);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s  %#02d:%#02d:%#02d %s %6d\r\n\r\n",workStat.scanTime,workStat.st.hour,workStat.st.minute,workStat.st.second,workStat.IOError,workStat.err);
    reportPutS(TmpStr);
    //chsize(report_hnd,lseek(report_hnd,0,1));
    close(hReportFile);
   }
 }

