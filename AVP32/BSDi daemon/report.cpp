#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "profile.h"
#include "resource.h"
#include "avp_os2.h"
#include "start.h"
#include "kernel.h"

#include "report.h"

int   hReportFile=-1;  // handle рапорт файла
ULONG reportLimSize=0; // огpаничение pазмеpа pапоpт файла
ULONG NewPointer=0;    //адрес нового положения
char szExtRep[0x400]="\r\n\r\nCommand line: ";

void reportInit(ULONG flag)
 {
  time_t Timer;
  struct tm *Tm;

  Timer=time((time_t*)NULL);
  Tm=gmtime(&Timer);
  
  if(flag==0)
    workStat.cek=Tm->tm_mday*86400+Tm->tm_hour*3600+Tm->tm_min*60+Tm->tm_sec;
   
  if(prf_data.Report)
   {
    struct stat statbuf;
    CHAR  Folderpath[CCHMAXPATH];
    
    if(prf_data.ReportFileName[0]=='/')
      strcpy(Folderpath,prf_data.ReportFileName);
    else  
     if(prf_data.ReportFileName[0]=='.')
      sprintf(Folderpath,"%s/%s",prg_path,&prf_data.ReportFileName[2]);
     else
      sprintf(Folderpath,"%s/%s",prg_path,prf_data.ReportFileName);
    
    if(Folderpath[strlen(Folderpath)-1]=='/') Folderpath[strlen(Folderpath)-1]=0;

    if((hReportFile=open(Folderpath,
      (prf_data.Append)?(O_WRONLY|O_CREAT):
 //|O_BINARY
      //                (O_WRONLY|O_CREAT|O_TRUNC),SH_DENYNO,S_IWRITE))!=(-1))
                        (O_WRONLY|O_CREAT|O_TRUNC),S_IRUSR|S_IRGRP|S_IROTH|S_IWRITE))!=(-1))
     {
      if(flag==1)
       {lseek(hReportFile,0,SEEK_END); return;}
        
      if(fstat(hReportFile,&statbuf)) NewPointer=statbuf.st_size;
      if(prf_data.ReportFileLimit) //&& (statbuf.st_size/1024)>tPage3.m_Size)
       {
        reportLimSize=atol(prf_data.ReportFileSize)*1024;
        if(reportLimSize<NewPointer)
	 {
	  lseek(hReportFile,reportLimSize,SEEK_SET);
	  write(hReportFile,"",1);
	 } 
       }
      if(prf_data.Append) lseek(hReportFile,0,SEEK_END);

      char s[CCHMAXPATH]; // Antiviral Toolkit Pro by Eugene Kaspersky
      reportPutS("\xfe");
      if(flag==0)
       sprintf(s,LoadString(IDS_REPORT_BANNER_L,"   AVP for BSDi start %02d-%02d-%02d %02d:%02d:%02d\r\n"),
           Tm->tm_mday,Tm->tm_mon,Tm->tm_year+1900,Tm->tm_hour,Tm->tm_min,Tm->tm_sec);
      else   
       sprintf(s,"   AVP for BSDi reinit %02d-%02d-%02d %02d:%02d:%02d\r\n",
           Tm->tm_mday,Tm->tm_mon,Tm->tm_year+1900,Tm->tm_hour,Tm->tm_min,Tm->tm_sec);
      reportPutS(s);
      if(prf_data.ExtReport==1)
       {
        sprintf(s,"         Version 3.0  beta 1             \n\r");
        reportPutS(s);
        reportPutS("  ");
        sprintf(s,LoadString(IDS_LAST_UPDATE_L,"Last update: %02d.%02d.%04d, %d records."),SurBase.LastUpdateDay,SurBase.LastUpdateMonth,SurBase.LastUpdateYear,SurBase.RecCount);
        reportPutS(s);
        if(flag==0)
          reportPutS(szExtRep);
        reportPutS("\r\n");
       }
     }
    else
     {
 prf_data.Report=0;/* mb_ok_(219);*/
 }
   }
 }

void reportPutS(char *str)
 {
  if((prf_data.Report)&&(hReportFile!=(-1))) //while(*str) reportPutC(*str++);
   {
    if((reportLimSize>(NewPointer+strlen(str)))||(reportLimSize==0))
     {
      write(hReportFile,str,strlen(str));
      NewPointer+=strlen(str);
     }
    else
     {
      ULONG Dlin;
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
  time_t Timer;
  struct tm *Tm;

  Timer =time((time_t*)NULL);
  Tm=gmtime(&Timer);
  workStat.cekForEach=Tm->tm_mday*86400+Tm->tm_hour*3600+Tm->tm_min*60+Tm->tm_sec-workStat.cekForEach;
  if(workStat.cekForEach==0) workStat.cekForEach=1;
  workStat.TmForEach.tm_sec=(UCHAR)((workStat.cekForEach%3600)%60);
  workStat.TmForEach.tm_min=(UCHAR)((workStat.cekForEach%3600)/60);
  workStat.TmForEach.tm_hour=(UCHAR)(workStat.cekForEach/3600);

  if((prf_data.Report)&&(hReportFile!=-1))
   {
    sprintf(TmpStr,"\r\nCurrent object: %s\r\n",szCurName);
    reportPutS(TmpStr);
    sprintf(TmpStr,"\r\n%s %6ld    %s %6ld\r\n",workStat.Sectors,workStat.sectForEach,workStat.knownVir,workStat.kvForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Files,workStat.filForEach,workStat.virBodies,workStat.vbForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Folder,workStat.foldForEach,workStat.Disinfect,workStat.dfForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Archives,workStat.archForEach,workStat.Deleted,workStat.deletedForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Packed,workStat.packForEach,workStat.Warning,workStat.warnForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"                                    %s %6ld\r\n",workStat.Suspis,workStat.suspForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6ld\r\n",workStat.countSize,(int)(workStat.CountSizeForEach/workStat.cekForEach),workStat.Corript,workStat.corForEach);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s  %02d:%02d:%02d %s %6ld\r\n\r\n",workStat.scanTime,workStat.TmForEach.tm_hour,workStat.TmForEach.tm_min,workStat.TmForEach.tm_sec,workStat.IOError,workStat.errForEach);
    reportPutS(TmpStr);
   }
 }

void reportDone(ULONG flag)
 {
  CHAR TmpStr[CCHMAXPATH];
  ULONG TmpSec;
  time_t Timer;
  struct tm *Tm;

  Timer =time((time_t*)NULL);
  Tm=gmtime(&Timer);

  if(flag==2) TmpSec=workStat.cek;
  
  workStat.cek=Tm->tm_mday*86400+Tm->tm_hour*3600+Tm->tm_min*60+Tm->tm_sec-workStat.cek;
  if(workStat.cek==0) workStat.cek=1;
  workStat.Tm.tm_sec=(UCHAR)((workStat.cek%3600)%60);
  workStat.Tm.tm_min=(UCHAR)((workStat.cek%3600)/60);
  workStat.Tm.tm_hour=(UCHAR)(workStat.cek/3600);

  if((prf_data.Report)&&(hReportFile!=-1))
   {
    //sprintf(TmpStr,"\r\n%s\r\n",
    //  (flgScan==1)?
    //    LoadString(IDS_SCANPROC_INCOMPLETE,"Scan process incompleted.\n"):
    //    LoadString(IDS_SCANPROC_COMPLETE,"Scan process completed.\n"));
    //reportPutS(TmpStr);
    reportPutS("\r\nResult for all objects:\r\n");
    sprintf(TmpStr,"\r\n%s %6ld    %s %6ld\r\n",workStat.Sectors,workStat.sect,workStat.knownVir,workStat.kv);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Files,workStat.fil,workStat.virBodies,workStat.vb);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Folder,workStat.fold,workStat.Disinfect,workStat.df);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Archives,workStat.arch,workStat.Deleted,workStat.deleted);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6ld    %s %6ld\r\n",workStat.Packed,workStat.pack,workStat.Warning,workStat.warn);
    reportPutS(TmpStr);
    sprintf(TmpStr,"                                    %s %6ld\r\n",workStat.Suspis,workStat.susp);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s %6d    %s %6ld\r\n",workStat.countSize,(int)(workStat.CountSize/workStat.cek),workStat.Corript,workStat.cor);
    reportPutS(TmpStr);
    sprintf(TmpStr,"%s  %02d:%02d:%02d %s %6ld\r\n",workStat.scanTime,workStat.Tm.tm_hour,workStat.Tm.tm_min,workStat.Tm.tm_sec,workStat.IOError,workStat.err);
    reportPutS(TmpStr);
    close(hReportFile);
   }

  if(flag==2) workStat.cek=TmpSec;
 }

void reportReinit(void)
 {
  reportDone(2)
;
  reportInit(2)
;
 }