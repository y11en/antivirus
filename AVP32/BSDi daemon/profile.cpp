#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include "typedef.h"

#include "avp_os2.h"
#include "avp_rc.h"
#include "profile.h"
#include "start.h"
#include "resource.h"
#include "../TYPEDEF.H"
#include "../../CommonFiles/Stuff/_CARRAY.H"

#include "loadkeys.h"
extern ULONG functionality;

#include "contain.h"

// Уничтожить все пpобелы в стpоке
void remove_space_(unsigned char *str)
{
 unsigned char *from=str,*to=str;

 while(*str)
 {
  if(*str!=' ')
  {
   *to=*from;
   if(from!=to) *from=' ';
   to++;
  }
  from++; str++;
 }
 *to=0;
}

profile_data prf_data; // данные из "avp.prf"

unsigned char prf_global_state; // Устанавливается при ошибке

char GetStr(char *par,char *in,char **out)
{
 if(strncasecmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') (*in)++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!strlen(in)) return 1;
 if(*out) free(*out);
// if((*out=strdup(in))==NULL)
 if((*out=(char *)malloc(strlen(in)+1))==NULL) return -1;
 if(strcpy(*out,in)==NULL) { /*prg_error=1; exit_();*/ return -1; }
 if(!*out) prf_global_state=PERR_MEM;
 return 1;
}

char xget_str_(char *par, char *in, char *out, int lim)
{
 if(strncasecmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') (*in)++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!strlen(in)) return 1;
 if(strlen(in)>(size_t)lim) return 0;
 strcpy(out,in);
 return 1;
}

char xxget_str(char *par, char *in, char *out,int lim)
 {
  if(strncasecmp(in,par,strlen(par))) return 0;
  in+=strlen(par);
  while(*in==' ') (*in)++;
  if(*in++!='=') return 0;
  while(*in==' ') in++;
  if(!strlen(in)) return 1;
  if(strlen(in)>(size_t)lim) return 0;
  while(*in) if(*in>='0'&&*in<='9') { *out++=*in; in++; }
  *out=0;
  return 1;
 }

char get_atoi_(char *par, char *in, char *out, char max)
{
 if(strncasecmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') (*in)++;
 if(*in++!='=') return 0;
 if(*(in+1)||*in<'0'||*in>max+'0') return 1;
 *out=(char)(*in-'0');
 return 1;
}

//char get_atoI_(char *par, char *in, LONG *out)
//{
// if(strncasecmp(in,par,strlen(par))) return 0;
// in+=strlen(par);
// while(*in==' ') *in++;
// if(*in++!='=') return 0;
  // if(*(in+1)||*in<'0'||*in>max+'0') return 1;
// *out=atoi(in);
// return 1;
//}

char get_bool_(char *par, char *in, char *out)
{
 if(strncasecmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') (*in)++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!strcasecmp(in,"Yes"))
 *out=1;
 else
  if(!strcasecmp(in,"No")) *out=0;
  else
 prf_global_state=PERR_COR;
 return 1;
}

//void bool_val_(char *z, char val)
//{
// if(val) strcpy(z,"Yes"); else strcpy(z,"No");
//}

void set_default_options_()
 {
  prf_data.List=NULL;
  prf_data.SubDirectories=1;
  prf_data.Memory=1;
//  prf_data.ScanAllSector=0;
  prf_data.Sectors=0;
  prf_data.ScanRemovable=1;
  prf_data.Files=1;
  prf_data.Packed=1;
  prf_data.Archives=1;
  prf_data.MailBases=0;
  prf_data.MailPlain=0;
  prf_data.FileMask=0;
  *(prf_data.UserMask)=0;
  prf_data.ExcludeFiles=0;
  prf_data.ExcludeDir=NULL;
  strcpy(prf_data.ExcludeMask,"*.txt *.cmd");
  prf_data.InfectedAction=0;
  prf_data.InfectedCopy=0;
  strcpy(prf_data.InfectedFolder,"infected");
  prf_data.SuspiciousCopy=0;
  strcpy(prf_data.SuspiciousFolder,"suspic");
  prf_data.Warnings=1;
  prf_data.CodeAnalyser=1;
  prf_data.RedundantScan=0;
  prf_data.ShowOK=0;
  prf_data.ShowPack=1;
  prf_data.Sound=1;
  prf_data.Symlinks=0;
    // -LH symbolic links on the command line are followed (Symbolic
    //     links encountered in the tree traversal are not followed)
.
    // -LL all symbolic links are followed.
    // -LP no symbolic links are followed.
  prf_data.CrossFs=1;
 // If true, don't cross filesystem boundaries.
  prf_data.Report=0;
  strcpy(prf_data.ReportFileName,"report.txt");
  prf_data.ExtReport=1;
  prf_data.RepForEachDisk=0;
  prf_data.LongStrings=0;
  prf_data.Append=0;
  *(prf_data.ReportFileSize)=0;
  prf_data.ReportFileLimit=0;
  prf_data.OtherMessages=1;
  prf_data.Color=0;
 }

ULONG SearchFile(char* sFileName,char* sDestName,char* sExt)
 {
  //нет точки или последняя точка раньше '\'(т.е. в директории)
  if(strlen(sExt)>0)
   if(!strchr(sFileName,'.') || (strrchr(sFileName,'.')<strrchr(sFileName,'/')) )
     strcat(sFileName,sExt);
//printf("1 %s\n",sFileName);
  if(access(sFileName,0)==-1)
   {
    strcpy(sDestName,prg_path);
    strcat(sDestName,sFileName);
//printf("2 %s\n",sDestName);
    if(access(sDestName,0)==-1)
     {
      strcpy(sDestName,start_path);
      strcat(sDestName,sFileName);
//printf("3 %s\n",sDestName);
      if(access(sDestName,0)==-1) strcpy(sDestName,sFileName);
//printf("4 %s\n",sDestName);
     }
   }
  else
   {
    strcpy(sDestName,sFileName);
//printf("5 %s\n",sDestName);
   }    
//printf("6 %s\n",sDestName);
//getchar();
  return 0;
 }

char  DefProfile[0x50],LocFile[0x50],KeyFile[0x50],
KeysPath[CCHMAXPATH],
      SetFile[0x50],BasePath[CCHMAXPATH];

ULONG load_ini(char *name)
 {
  int   fil;
  char  dname[CCHMAXPATH];
  char  *buf,*next;
  unsigned len;
  char  section=0xff;
  char  sectstr[]={"AVP32]\0Configuration]\0"};
  char  secofs[] ={0,7};

  strcpy(DefProfile,"defUnix.prf");
  strcpy(LocFile,"avp_loc.dll");
  strcpy(KeyFile,"AvpUnix.key");
  strcpy(SetFile,"avp.set");
  strcpy(BasePath,".");

  SearchFile(name,dname,".ini");

  if((fil=open(dname,O_RDONLY))==-1) //|O_CREAT
   {
    return PERR_OPN;
 //|O_TEXT
   }    
  len=lseek(fil,0,SEEK_END);
  lseek(fil,0,SEEK_SET);
  buf=(char *)malloc(len);
  if(!buf) return PERR_MEM;
  if(read(fil,(void *)buf,len)==-1)
   {
    close(fil);
    free(buf);
    return PERR_DSK;
   }
  if(close(fil)==-1)
   {
    free(buf);
    return PERR_DSK;
   }

  // ok, now parse file in mem
  next=strtok(buf,"\r\n");
  while(next)
   {
    while(*next==' ') next++;
    if(*next=='[')
     {
      next++;
      int fl;
      section=0xff;
      for(fl=0;fl<2;fl++)
       if(!strcasecmp(next,sectstr+secofs[fl]))
        {section =(char)fl; break;}
     }
    else
     switch(section)
     {
      case 0:
        if(xget_str_("DefaultProfile",next,DefProfile,CCHMAXPATH-1)) break;
        if(xget_str_("LocFile",next,LocFile,CCHMAXPATH-1)) break;
           //AVP_LOC.DLL
              break;
      case 1:
        if(xget_str_("KeyFile",next,KeyFile,CCHMAXPATH-1))  break;
        if(xget_str_("KeysPath",next,KeysPath,CCHMAXPATH-1))break;
           //Avp.key
        if(xget_str_("SetFile",next,SetFile,CCHMAXPATH-1))  break;
           //AVP.SET
        if(xget_str_("BasePath",next,BasePath,CCHMAXPATH-1))break;
           //base
              break;
     } // switch (section)
    next=strtok(0,"\r\n");
    //else next = 0;
   }
  free(buf);
  return 0;
 }

char sInfectedFolder[CCHMAXPATH],sSuspiciousFolder[CCHMAXPATH];
// Загрузка prf-файла и проверка его на ошибки
char loadProfile(char *name,char *szRep)
 {
  int   fl;
  char  *z0,section = 0xff,*buf,*next;
  unsigned z;
  char  sectstr[]={"Location]\0Objects]\0Actions]\0Options]\0Customize\0Monitor]\0"};
  char  secofs[] ={0,10,19,28,37,47};
  char  dname[CCHMAXPATH];
//  char* pList=NULL;
//  free(prf_data.List);
//  prf_data.List=NULL;
  PLIST pList;

  if(pTail!=NULL)
   {
    do
     {
      pList=pTail;
      pTail=pTail->pNext;
      free(pList);
     } while(pTail != NULL);
   }

//  CHAR  typePath[]=" :\\";
//  unsigned ulCurDisk,totaldrives;
//  BYTE cFloppyDrives,disk;
  // Check the number of floppy drives
//  cFloppyDrives=GetNumberFloppyDisks();

//  ulCurDisk=_getdrive();
//  for(disk=0;disk<27;disk++)
//   {
//    typePath[0]=(char)('a'+disk);
//    if(TestPresentDisk(disk,typePath)) AddRecord(typePath,0);
//   }
//  _dos_setdrive(ulCurDisk,&totaldrives);

  SearchFile(name,dname,".prf");
  prf_global_state=0;
  // load bastard

  //unsigned bytes;
  //unsigned char *cbuf;

  if((fl=open(dname,O_RDONLY))==-1) //|O_CREAT
    return PERR_OPN;
 //|O_TEXT
  if(szRep!=NULL)
   {
    time_t Timer;
    struct tm *Tm;
    
    Timer =time((time_t*)NULL);
    Tm=gmtime(&Timer);

    sprintf(&szRep[strlen(szRep)]," (from %02d-%02d-%02d %02d:%02d:%02d)",
      Tm->tm_year,Tm->tm_mon,Tm->tm_mday,Tm->tm_hour,Tm->tm_min,Tm->tm_sec);
   }

  z=lseek(fl,0,SEEK_END);
  lseek(fl,0,SEEK_SET);
  buf=(char *)malloc(z);
  if(!buf) return PERR_MEM;
   if(read(fl,(void *)buf,z)==-1)
   {
    close(fl);
    free(buf);
    return PERR_DSK;
   }
  if(close(fl)==-1)
   {
    free(buf);
    return PERR_DSK;
   }
  // ok, now parse file in mem
  next=strtok(buf,"\r\n");
  while(next)
   {
//    if(*next==0x0a) next++;
    while(*next==' ') next++;
    if(*next=='[')
     {
      next++;
      int fl;
      section=0xff;
      for(fl=0;fl<6;fl++)
       if(!strcasecmp(next,sectstr+secofs[fl]))
        {section =(char)fl; break;}
     }
    else
     switch(section)
     {
      case 0: //if(GetStr("List",next,&pList)) break;
              if(GetStr("List",next,&(prf_data.List))) break;
              get_bool_("SubDirectories",next,&(prf_data.SubDirectories));
              break;
      case 1: if(get_bool_("Memory",next,&(prf_data.Memory))) break;
              if(get_bool_("Sectors",next,&(prf_data.Sectors))) break;
//              if(get_bool_("ScanAllSector",next,&(prf_data.ScanAllSector))) break;
              if(get_bool_("Files",next,&(prf_data.Files))) break;
              if(get_bool_("Packed",next,&(prf_data.Packed))) break;
              if(get_bool_("Archives",next,&(prf_data.Archives))) break;
              if(get_bool_("MailBases",next,&(prf_data.MailBases))) break;
              if(get_bool_("MailPlain",next,&(prf_data.MailPlain))) break;
              if(get_atoi_("FileMask",next,&(prf_data.FileMask),3)) break;
              if(xget_str_("UserMask",next,prf_data.UserMask,80)) break;
              if(get_atoi_("ExcludeFiles",next,&(prf_data.ExcludeFiles),3)) break;
              //if(get_bool_("ExcludeFiles",next,&(prf_data.ExcludeFiles))) break;
              if(xget_str_("ExcludeMask",next,prf_data.ExcludeMask,80)) break;
              if(GetStr("ExcludeDir",next,&(prf_data.ExcludeDir))) break;
              break;
      case 2: if(xget_str_("InfectedFolder",next,prf_data.InfectedFolder,CCHMAXPATH-1)) break;
              if(xget_str_("SuspiciousFolder",next,prf_data.SuspiciousFolder,CCHMAXPATH-1)) break;
              if(get_atoi_("InfectedAction",next,&(prf_data.InfectedAction),3)) break;
              if(get_bool_("InfectedCopy",next,&(prf_data.InfectedCopy))) break;
              get_bool_("SuspiciousCopy",next,&(prf_data.SuspiciousCopy));
              break;
      case 3: if(xxget_str("ReportFileSize",next,prf_data.ReportFileSize,7)) break;
              if(get_bool_("Warnings",next,&(prf_data.Warnings))) break;
              if(get_bool_("CodeAnalyser",next,&(prf_data.CodeAnalyser))) break;
              if(get_bool_("RedundantScan",next,&(prf_data.RedundantScan))) break;
              if(get_bool_("ShowOK",next,&(prf_data.ShowOK))) break;
              if(get_bool_("ShowPack",next,&(prf_data.ShowPack))) break;
              if(get_bool_("Sound",next,&(prf_data.Sound))) break;
              if(get_atoi_("Symlinks",next,&(prf_data.Symlinks),2)) break;
              if(get_bool_("CrossFs",next,&(prf_data.CrossFs))) break;
              if(get_bool_("Report",next,&(prf_data.Report))) break;
              if(xget_str_("ReportFileName",next,prf_data.ReportFileName,CCHMAXPATH-1)) break;
              if(get_bool_("ExtReport",next,&(prf_data.ExtReport))) break;
              if(get_bool_("RepForEachDisk",next,&(prf_data.RepForEachDisk))) break;
              if(get_bool_("LongStrings",next,&(prf_data.LongStrings))) break;
              if(get_bool_("Append",next,&(prf_data.Append))) break;
              if(get_bool_("ReportFileLimit",next,&(prf_data.ReportFileLimit))) break;
              break;
      case 4:
//              if(get_bool_("Sound",next,&(prf_data.Sound))) break;
              if(get_bool_("OtherMessages",next,&(prf_data.OtherMessages))) break;
      case 5:
              if(get_bool_("Color",next,&(prf_data.Color))) break;
              break;
     } // switch (section)
    if(prf_global_state != PERR_MEM) next = strtok(0,"\r\n"); else next = 0;
   }
  free(buf);

//  if(pList!=NULL)
  if(prf_data.List!=NULL)
   {
//    prf_data.List=(char*)malloc(0x200);
//    z0=strtok(pList,"; ");
    z0=strtok(prf_data.List,"; ");
    while(z0)
     {
//      if((strlen(z0)>3)&&(z0[strlen(z0)-1]=='/')) z0[strlen(z0)-1]=0;
//   if(z0[2]==0&&z0[1]==':')
//    {
//     if(toupper(*z0)>='A'&&toupper(*z0)<='Z'&&_exist(z0))
//      {
//      }
//    }
      if((z0[0]=='.')||(z0[0]=='*'))
       {
        if((strlen(z0)>4)&&(z0[strlen(z0)-1]=='/')) z0[strlen(z0)-1]=0;
//        if((strlen(prf_data.List)+strlen(&z0[1])+1)<0x200)
//	 {
//          strcat(prf_data.List,&z0[1]);
//          strcat(prf_data.List," ");
//	 } 
        AddRecord(&z0[1],1);
       }
      z0=strtok(0,";");
     }
    free(prf_data.List);
    prf_data.List=0;
   }

  if(prf_data.ExcludeDir!=NULL)
   {
    z0=strtok(prf_data.ExcludeDir,"; ,*");
    while(z0)
     {
      if((strlen(z0)>3)&&(z0[strlen(z0)-1]=='/')) z0[strlen(z0)-1]=0;
      AddRecord(z0,2);
      z0=strtok(0,"; ,*");
     }
    free(prf_data.ExcludeDir);
    prf_data.ExcludeDir=0;
   }

  remove_space_((unsigned char*)prf_data.UserMask);
  //o_location[0]=IsSelectAllLocHDD();
  //o_location[1]=IsSelectAllNetWork();
//  if(!avp_key_flag||avp_key_flag&&!(avp_key->Options&KEY_O_DISINFECT))
  if(!(functionality & FN_DISINFECT))
   {
    if(prf_data.InfectedAction==1||prf_data.InfectedAction==2)
     prf_data.InfectedAction=0;
   }

  if(strlen(prf_data.InfectedFolder)>0)
   {
    if(prf_data.InfectedFolder[0]=='/')
      strcpy(sInfectedFolder,prf_data.InfectedFolder);
    else
     {  
      if(strlen(prg_path)>0)
       if(prg_path[strlen(prg_path)-1]!='/') strcat(prg_path,"/"); 
      if(prf_data.InfectedFolder[0]=='.')
       sprintf(sInfectedFolder,"%s/%s",prg_path,&prf_data.InfectedFolder[2]);
      else
       sprintf(sInfectedFolder,"%s/%s",prg_path,prf_data.InfectedFolder);
     }  
   }
  else
   sInfectedFolder[0]=0;
  if(strlen(prf_data.SuspiciousFolder)>0)
   {
    if(prf_data.SuspiciousFolder[0]=='/')
      strcpy(sSuspiciousFolder,prf_data.SuspiciousFolder);
    else
     {  
      if(strlen(prg_path)>0)
       if(prg_path[strlen(prg_path)-1]!='/') strcat(prg_path,"/"); 
      if(prf_data.SuspiciousFolder[0]=='.')
       sprintf(sSuspiciousFolder,"%s/%s",prg_path,&prf_data.SuspiciousFolder[2]);
      else
       sprintf(sSuspiciousFolder,"%s/%s",prg_path,prf_data.SuspiciousFolder);
     }  
   }
  else
   sSuspiciousFolder[0]=0;

  return prf_global_state;
 }
/*
#include <paths.h>
#include <sys/socket.h>
#include <netdb.h>

#include <errno.h>
#define MAXLINE         1024            // maximum line length

extern int     Debug;                  // debug flag
extern int     UniquePriority;     // Only log specified priority?
extern char    LocalHostName[MAXHOSTNAMELEN+1];        // our hostname
extern int     LogPort;                // port number for INET connections
extern char    *ConfFile;// = _PATH_LOGCONF;
extern char    ctty[];
extern struct  filed *Files;
extern int     Initialized; //=0;      // set when we have initialized ourselves

#define dprintf         if (Debug) printf

void deadq_enter(pid_t pid)
;
void fprintlog(struct filed *f,int flags,char *msg)
;
#define ADDDATE         0x004   // add a date to the message
void logmsg(int pri,char *msg,char *from,int flags)
;

// ... Logging 
#ifdef CONFIG_AVP_LOG
 void LogError(char *ErrStr);
 #define LOGERROR(args) LogError args
 extern int Log(char *fmt, ...);
 #define LOG(args) Log args
#else
 #define LOG(args) 
 #define LOGERROR(args) 
#endif

//  Decode a symbolic name to a numeric value
int decode(const char *name,CODE *codetab)
 {
  CODE *c;
  char *p, buf[40];

  if (isdigit(*name)) return (atoi(name));

  for (p = buf; *name && p < &buf[sizeof(buf) - 1]; p++, name++)
   {
    if (isupper(*name)) *p = tolower(*name);
    else                *p = *name;
   }
  *p = '\0';
  for (c = codetab; c->c_name; c++)
   if (!strcmp(buf, c->c_name)) return (c->c_val);

  return (-1);
 }

// Crack a configuration file line
void cfline(char *line,struct filed *f,char *prog)
 {
  struct hostent *hp;
  int i, pri;
  char *bp, *p, *q;
  char buf[MAXLINE], ebuf[100];

  dprintf("cfline(\"%s\", f, \"%s\")\n", line, prog);

  errno = 0;      // keep strerror() stuff out of logerror messages

  // clear out file entry
  memset(f, 0, sizeof(*f));
  for (i = 0; i <= LOG_NFACILITIES; i++)
          f->f_pmask[i] = INTERNAL_NOPRI;

  // save program name if any
  if(prog && *prog=='*') prog = NULL;
  if(prog)
   {
    f->f_program=(char *)calloc(1, strlen(prog)+1);
    if(f->f_program)
     {
      strcpy(f->f_program, prog);
     }
   }

  // scan through the list of selectors
  for(p = line; *p && *p != '\t' && *p != ' ';)
   {
    int pri_done;
    int pri_cmp;

    // find the end of this facility name list
    for(q = p; *q && *q != '\t' && *q != ' ' && *q++ != '.'; )
      continue;

    // get the priority comparison
    pri_cmp = 0;
    pri_done = 0;
    while(!pri_done)
     {
      switch (*q)
       {
        case '<':
          pri_cmp |= PRI_LT;
          q++;
          break;
        case '=':
          pri_cmp |= PRI_EQ;
          q++;
          break;
        case '>':
          pri_cmp |= PRI_GT;
          q++;
          break;
        default:
          pri_done++;
          break;
       }
     }
    if(!pri_cmp)
      pri_cmp = (UniquePriority) ? (PRI_EQ) : (PRI_EQ | PRI_GT);

    // collect priority name
    for (bp = buf; *q && !strchr("\t,; ", *q); )
       *bp++ = *q++;
    *bp = '\0';

    // skip cruft
    while (strchr(",;", *q)) q++;

    // decode priority name
    if (*buf == '*') pri = LOG_PRIMASK + 1;
    else
     {
      pri = decode(buf, prioritynames);
      if (pri < 0)
       {
        (void)snprintf(ebuf, sizeof ebuf,"unknown priority name \"%s\"",buf);
        LOGERROR((ebuf));
        return;
       }
     }

    // scan facilities
    while (*p && !strchr("\t.; ", *p))
     {
      for (bp = buf; *p && !strchr("\t,;. ", *p); )
              *bp++ = *p++;
      *bp = '\0';

      if(*buf == '*')
       for (i = 0; i < LOG_NFACILITIES; i++)
        {
         f->f_pmask[i] = pri;
         f->f_pcmp[i] = pri_cmp;
        }
      else
       {
        i = decode(buf, facilitynames);
        if (i < 0)
         {
          (void)snprintf(ebuf,sizeof ebuf,"unknown facility name \"%s\"",buf);
          LOGERROR((ebuf));
          return;
         }
        f->f_pmask[i >> 3] = pri;
        f->f_pcmp[i >> 3] = pri_cmp;
       }
      while (*p == ',' || *p == ' ') p++;
     }

    p = q;
   }

  // skip to action part
  while (*p == '\t' || *p == ' ') p++;

  switch (*p)
   {
    case '@':
      (void)strncpy(f->f_un.f_forw.f_hname, ++p,
              sizeof(f->f_un.f_forw.f_hname)-1);
      f->f_un.f_forw.f_hname[sizeof(f->f_un.f_forw.f_hname)-1] = '\0';
      hp = gethostbyname(f->f_un.f_forw.f_hname);
      if (hp == NULL)
       {
        extern int h_errno;

        (hstrerror(h_errno));
//	LOGERROR();
	break;
       }
      memset(&f->f_un.f_forw.f_addr, 0,
               sizeof(f->f_un.f_forw.f_addr));
      f->f_un.f_forw.f_addr.sin_family = AF_INET;
      f->f_un.f_forw.f_addr.sin_port=LogPort;
      memmove(&f->f_un.f_forw.f_addr.sin_addr, hp->h_addr, hp->h_length);
      f->f_type = F_FORW;
      break;
    case '/':
      if ((f->f_file = open(p, O_WRONLY|O_APPEND, 0)) < 0)
       {
        f->f_type = F_UNUSED;
        LOGERROR((p));
        break;
       }
      if (isatty(f->f_file))
       {
        if (strcmp(p, ctty) == 0)
         f->f_type = F_CONSOLE;
        else
         f->f_type = F_TTY;
        (void)strcpy(f->f_un.f_fname, p + sizeof _PATH_DEV - 1);
       }
      else
       {
        (void)strcpy(f->f_un.f_fname, p);
        f->f_type = F_FILE;
       }
      break;
    case '|':
      f->f_un.f_pipe.f_pid = 0;
      (void)strcpy(f->f_un.f_pipe.f_pname, p + 1);
      f->f_type = F_PIPE;
      break;
    case '*':
      f->f_type = F_WALL;
      break;
    default:
      for (i = 0; i < MAXUNAMES && *p; i++)
       {
        for (q = p; *q && *q != ','; ) q++;
        (void)strncpy(f->f_un.f_uname[i], p, UT_NAMESIZE);
        if ((q - p) > UT_NAMESIZE)
          f->f_un.f_uname[i][UT_NAMESIZE] = '\0';
        else
          f->f_un.f_uname[i][q - p] = '\0';
        while (*q == ',' || *q == ' ') q++;
        p = q;
       }
      f->f_type = F_USERS;
      break;
   }
 }

//  INIT -- Initialize syslogd from configuration table
void init(int signo)
 {
  int i;
  FILE *cf;
  struct filed *f, *next, **nextp;
  char *p;
  char cline[LINE_MAX];
  char prog[NAME_MAX+1];

  dprintf("init\n");

  //  Close all open log files.
  Initialized = 0;
  for(f = Files; f != NULL; f = next)
   {
    // flush any pending output
    if (f->f_prevcount)
 fprintlog(f, 0, (char *)NULL);

    switch (f->f_type)
     {
      case F_FILE:
      case F_FORW:
      case F_CONSOLE:
      case F_TTY:
        (void)close(f->f_file);
        break;
      case F_PIPE:
        (void)close(f->f_file);
        if (f->f_un.f_pipe.f_pid > 0)
 deadq_enter(f->f_un.f_pipe.f_pid);
        f->f_un.f_pipe.f_pid = 0;
        break;
     }
    next = f->f_next;
    if(f->f_program) free(f->f_program);
    free((char *)f);
   }
  Files = NULL;
  nextp = &Files;

  // open the configuration file
  if((cf=fopen(ConfFile, "r")) == NULL)
   {
    dprintf("cannot open %s\n", ConfFile);
    *nextp = (struct filed *)calloc(1, sizeof(*f));
    cfline("*.ERR\t/dev/console", *nextp, "*");
    (*nextp)->f_next = (struct filed *)calloc(1, sizeof(*f));
    cfline("*.PANIC\t*", (*nextp)->f_next, "*");
    Initialized = 1;
    return;
   }

  //  Foreach line in the conf table, open that file.
  f = NULL;
  strcpy(prog, "*");
  while (fgets(cline, sizeof(cline), cf) != NULL)
   {
    // check for end-of-section, comments, strip off trailing
    // spaces and newline character. #!prog is treated specially:
    // following lines apply only to that program.
    for (p = cline; isspace(*p); ++p) continue;
    if (*p == 0) continue;
    if(*p == '#')
     {
      p++;
      if(*p!='!') continue;
     }
    if(*p=='!')
     {
      p++;
      while(isspace(*p)) p++;
      if((!*p) || (*p == '*'))
       {
        strcpy(prog, "*");
        continue;
       }
      for(i = 0; i < NAME_MAX; i++)
       {
        if(!isalnum(p[i])) break;
        prog[i] = p[i];
       }
      prog[i] = 0;
      continue;
     }
    for (p = strchr(cline, '\0'); isspace(*--p);) continue;
    *++p = '\0';
    f = (struct filed *)calloc(1, sizeof(*f));
    *nextp = f;
    nextp = &f->f_next;
    cfline(cline, f, prog);
   }

  // close the configuration file
  (void)fclose(cf);

  Initialized = 1;

  if(Debug)
   {
    for (f = Files; f; f = f->f_next)
     {
      for (i = 0; i <= LOG_NFACILITIES; i++)
       if (f->f_pmask[i] == INTERNAL_NOPRI)
         printf("X ");
       else
         printf("%d ", f->f_pmask[i]);
      printf("%s: ", TypeNames[f->f_type]);
      switch (f->f_type)
       {
        case F_FILE:
          printf("%s", f->f_un.f_fname);
          break;
        case F_CONSOLE:
        case F_TTY:
          printf("%s%s", _PATH_DEV, f->f_un.f_fname);
          break;
        case F_FORW:
          printf("%s", f->f_un.f_forw.f_hname);
          break;
        case F_PIPE:
          printf("%s", f->f_un.f_pipe.f_pname);
          break;
        case F_USERS:
          for (i = 0; i < MAXUNAMES && *f->f_un.f_uname[i]; i++)
            printf("%s, ", f->f_un.f_uname[i]);
          break;
       }
      if(f->f_program)
       {
        printf(" (%s)", f->f_program);
       }
      printf("\n");
     }
   }

  logmsg(LOG_SYSLOG|LOG_INFO, "syslogd: restart", LocalHostName, ADDDATE);
  dprintf("syslogd: restarted\n");
 }


*/