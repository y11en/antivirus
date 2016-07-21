// general purpose AntiViral Daemon support for Linux
#define _UNIXAF_
//#define _INET_
//#define CONFIG_AVP_LOG      
//#define DEBUG 

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>        // select(),getppid()
#include <signal.h>        // SIGPIPE 
#include <sys/param.h>
#include <sys/stat.h>      // mkdir()  
#include <sys/types.h>     // socket() 
#include <sys/fcntl.h>     // O_RDONLY 
#include <sys/resource.h>  // socket() 
#include <sys/socket.h>    // socket() 
#include <netdb.h>         // gethostby...() 
#include <sys/un.h>        // struct sockaddr_un 
#include <ctype.h>
#include <sys/file.h>      // flock()
#include <sys/wait.h>      // wait()   
#ifdef _INET_
 #include <netinet/in.h>
 #include <arpa/nameser.h>
 #include <arpa/inet.h>
#endif
#include <string.h>        
//#include <sys/time.h>      // timeval 

// ... Cfg pathnames 
#define AVP_NODE_DIR      "/var/run"
#define AVP_NODE_DIR_MODE 0777
#define AVP_NODE_PID      AVP_NODE_DIR "/AvpPid"
#define AVP_NODE_LOG      AVP_NODE_DIR "/AvpLog"
#define AVP_NODE_CTL      AVP_NODE_DIR "/AvpCtl\0\0\0"
//#define AVP_NODE_FIFO     "/dev/AvpData"
char *NodePid=AVP_NODE_PID;
char *NodeCtl=AVP_NODE_CTL;

#include "typedef.h"
#include "start.h"

char *prgname;

extern int errno;

#ifdef CONFIG_AVP_LOG      // only if enabled 
// *===========================================================*
// *     ==>    the logging utility
// * This procedure is used to log info (mostly debugging ones)
// * the log file is quite large, actually, so be careful....
// *-----------------------------------------------------------
int Log(char *fmt,...)
 {
  va_list args;
  static FILE *f;
  static int pid;

  pid=getpid();
  if(!f)
   {
    f=fopen(AVP_NODE_LOG,"a");
    if(!f) f=fopen("/dev/null","w");  // not null, hopefully 
    setvbuf(f,NULL,_IONBF,0);
   }

  static time_t mytime;
  static char str_time[15];
  static char temp[80];
  time(&mytime);
  strftime(str_time,15,"%H:%M:%S",localtime(&mytime));
  sprintf(temp,"%s - %i: %s\n",str_time,pid,fmt);

  va_start(args,fmt);     // but the second argument is not used... 
  vfprintf(f,temp,args);
//  vfprintf(stdout,fmt,args);
  return 1;
 }
#endif

// ... Logging 
#ifdef CONFIG_AVP_LOG
 #define LOG(args) Log args
 void LogError(char *ErrStr);
 #define LOGERROR(args) LogError args
 extern int Log(char *fmt, ...);
#else
 #define LOG(args) 
 #define LOGERROR(args) 
#endif

#ifdef CONFIG_AVP_LOG      // only if enabled 
// Print AvpDaemon errors some place.
void LogError(char *ErrStr)
 {
  char buf[100];

  if(errno==0) (void)sprintf(buf,"AvpDaemon: %s",ErrStr);
  else
   if((unsigned)(errno > sys_nerr))
    (void)sprintf(buf,"AvpDaemon: %s: error %d",ErrStr,errno);
   else
    (void)sprintf(buf,"AvpDaemon: %s: %s",ErrStr,sys_errlist[errno]);
  errno=0;
  LOG(("Called loggerr, msg: %s",buf));
//  logmsg(LOG_SYSLOG|LOG_ERR,buf,LocalHostName,ADDDATE);
  return;
 } 
#endif

// *---------------------------------------------------------------*
int _PrintExit(char *f,int n,char *s,int err) // printf's and exit(1)
 {
  LOG(("%s(%i): Exiting: \"%s: %s\"",f,n,s,strerror(err) ));
  //PDEBUG((stderr,"PrintExit() invoked from %s(%i)\n",f,n));
  fprintf(stderr,"%s: %s: %s\n",prgname,s,strerror(err));
  exit(1);
 }
#define PrintExit(s) _PrintExit(__FILE__,__LINE__,(s),errno)

#include <sys/sem.h>

#ifdef NEED_UNION_SEMUN
// it makes no sense, but this isn't defined on solaris 
union semun
 {
  long val;
  struct semid_ds *buf;
  ushort *array;
 };
#endif

static int semId=-1;
static struct sembuf opOn;
static struct sembuf opOff;

// We get a random semaphore ... the lame sysv semaphore interface
// means we have to be sure to clean this up or else we'll leak semaphores.
//static void acceptMutexCleanup(void *foo)
// {
//  union semun ick;
//
//  if(semId<0) return;
//  // this is ignored anyhow 
//  ick.val=0;
//  if(semctl(semId,0,IPC_RMID,ick)<0) //kill sem
//    perror("semctl(IPC_RMID)");
// }

typedef struct pool pool;
static void acceptMutexInit(pool *p)
 {
#ifdef MUTEXSEM
  // pre-initialize these 
  opOn.sem_num =0;
  opOn.sem_op  =-1;
  opOn.sem_flg =SEM_UNDO; //0x1000 undo the operation on exit
  opOff.sem_num=0;
  opOff.sem_op =1;
  opOff.sem_flg=SEM_UNDO;

  // acquire the semaphore 
  semId=semget(IPC_PRIVATE,1,IPC_CREAT|0600);
  if(semId<0)
   {
    perror("semget");
    exit(ERR_LEVEL_SEM_INIT);
   }

  union semun ick;
  ick.val=1;
  if(semctl(semId,0,SETVAL,ick)<0) //set in icv
   {
    perror("semctl(SETVAL)");
    exit(ERR_LEVEL_SEM_INIT);
   }
  if(!getuid())
   {
    // restrict it to use only by the appropriate user_id ... not that this
    // stops CGIs from acquiring it and dinking around with it.
    struct semid_ds buf;
    buf.sem_perm.uid =getuid();//ap_user_id;
    buf.sem_perm.gid =getgid();//ap_group_id;
    buf.sem_perm.mode=0600;
    ick.buf=&buf;
    if(semctl(semId,0,IPC_SET,ick)<0)
     {
      perror("semctl(IPC_SET)");
      exit(ERR_LEVEL_SEM_INIT);
     }
   }
  //ap_register_cleanup(p, NULL, accept_mutex_cleanup, ap_null_cleanup);
#endif
 }

static void acceptMutexOn(void)  //wait and add
 {
#ifdef MUTEXSEM
  if(semop(semId,&opOn,1)<0)
   {
    perror("acceptMutexOn");
    //clean_child_exit(APEXIT_CHILDFATAL);
   }
#endif
 }

static void acceptMutexOff(void) //Add
 {
#ifdef MUTEXSEM
  if(semop(semId,&opOff,1)<0)
   {
    perror("acceptMutexOff");
    //clean_child_exit(APEXIT_CHILDFATAL);
   }
#endif
 }

// Reads the specified pidfile and returns the read pid.
// 0 is returned if either there's no pidfile, it's empty or no pid can be read.
int ReadPid(char *pidfile)
 {
  FILE *f;
  int pid;

  if(!(f=fopen(pidfile,"r")))
    return 0;
  fscanf(f,"%d", &pid);
  fclose(f);
  return pid;
 }

// Reads the pid using read_pid and looks up the pid in the process
// table (using /proc) to determine if the process already exists. If
// so 1 is returned, otherwise 0.
int CheckPid (char *pidfile)
 {
  int pid=ReadPid(pidfile);

  // Amazing ! _I_ am already holding the pid file... 
  if((!pid) || (pid==getpid ())) return 0;

  // The 'standard' method of doing this is to try and do a 'fake' kill
  // of the process.  If an ESRCH error is returned the process cannot
  // be found -- GW. But... errno is usually changed only on error.. 
  if(kill(pid, 0) && errno==ESRCH) return(0);

  return pid;
}

// Writes the pid to the specified file. If that fails 0 is
// returned, otherwise the pid.
int WritePid(char *pidfile)
 {
  FILE *f;
  int fd;
  int pid;

  if( ((fd=open(pidfile,O_RDWR|O_CREAT,0644))==-1) //fopen(AVP_NODE_PID,"w");
       || ((f=fdopen(fd,"r+"))==NULL) )
   {
    //if(getuid()) fprintf(stderr,"%s: you're not root, can you write to %s?\n",
    //  prgname,AVP_NODE_DIR);
    //PrintExit(AVP_NODE_PID);
    fprintf(stderr, "Can't open or create %s.\n", pidfile);
    return 0;
   }

  if(flock(fd,LOCK_EX|LOCK_NB)==-1)
   {
    fscanf(f,"%d",&pid);
    fclose(f);
    LOG(("Can't lock, lock is held by pid %d.\n", pid));
    return 0;
   }

  pid=getpid();
  if(!fprintf(f,"%d\n",pid))
   {
    LOG(("Can't write pid, %s.\n",strerror(errno)));
    close(fd);
    return 0;
   }
  fflush(f);

  if(flock(fd,LOCK_UN)==-1)
   {
    LOG(("Can't unlock pidfile %s, %s.\n",pidfile,strerror(errno)));
    close(fd);
    return 0;
   }
  close(fd);

  LOG(("Signed(pid %d)",getpid()));
  return pid;
 }

// Remove the the specified file. The result from unlink(2) is returned
int RemovePid(char *pidfile)
 {
  return unlink(pidfile);
 }
  
#include "avp_rc.h"
#include "avp_os2.h"
#include "kernel.h"
#include "resource.h"
#include "profile.h"
#include "report.h"
#include "dword.h"

#include "../ScanAPI/Avp_msg.h"
#include "../TYPEDEF.H"
#include "../../CommonFiles/Stuff/_CARRAY.H"
#include "../../CommonFiles/_AVPIO.H"
#include "../ScanAPI/BaseAPI.h"
#include "loadkeys.h"
#include "../../CommonFiles/AVPComID.h"
#include "../../CommonFiles/loadkeys/AVPKeyID.h"
#include "../../CommonFiles/property/Property.h"
#include "../../CommonFiles/Sign/SIGN.H"

#include "contain.h"

ULONG functionality=0;
CPtrArray KeyHandleArray;

BOOL blDaemon=FALSE;

extern int  ExitCode;
extern int  ApplyToAll;
extern int  LastAction;
extern int  MethodDelete;
extern BOOL SectorWarn;

int TestObject(char *szFileName);
int TestSectors(void);

#define AVP_NAME    "Antiviral Toolkit Pro for BSDi (Daemon)"
#define AVP_RELEASE "beta 1"
#define AVP_DATE    "August 1999"

#define	MAXLINE	    0x200  // maximum line length 
#define	MAXSVLINE     240  // maximum saved line length 
#define UNAMESZ	        8  // length of a login name 
#define MAXUNAMES      20  // maximum number of user names 
#define MAXFNAME      200  // max file pathname length 

#define F_CONSOLE	3  // console terminal 

#ifndef _PATH_CONSOLE
 #define _PATH_CONSOLE	"/dev/console"
#endif
char	ctty[]=_PATH_CONSOLE;

int OptQuit=0;
int OptKill=0;
int OptMult=0;
//int OptRepeater=0;

char LocalHostName[MAXHOSTNAMELEN+1]; // our hostname 
char *LocalDomain;	              // our local domain name 
int  funix=0; 
#ifdef _INET_
 int InetInuse = 0;	// non-zero if INET sockets are being used 
 int AcceptRemote = 0;	// receive messages that come via UDP 
 int finet;		// Internet datagram socket 
 int inetm=0;
 int LogPort;		// port number for INET connections 
#endif
static int restart=0;

char* LoadString(ULONG strId,char *defaultStr)
 {
  return defaultStr;
 }

VOID DisplayMessage(PCHAR szStr,BOOL flStop)
 {
  if(blDaemon==FALSE)
   {
    fprintf(stdout,"\n%s\n\n",szStr);
    if(flStop)
     if(cmdline.skip_dlg!=1)
      {
       fprintf(stdout,LoadString(IDS_PRESS_KEY_L,"Press any key\n"));
       getchar();
      }
   }      
 }

int InitPrfData(struct _cmdline *CmdLine,profile_data *PrfData,char *Opts,char *Path,char *TstObj);
// Take a raw input line, decode the message, and test the message
void TestLine(char *msg)
 {
  // test for special codes 
  int pri=0;//DEFUPRI;
  register char *p=msg;
  if(*p=='<')
   {
    pri=0;
    while(isdigit(*++p)) pri=10*pri+(*p-'0');
    if(*p=='>') ++p;
   }
  if(pri &~(LOG_FACMASK|LOG_PRIMASK)) pri=0;//DEFUPRI;

  //register char *q=line;
  //while((c=*p++ & 0x7f)!='\0' && q<&line[sizeof(line)-1])
  // if(c=='\n') *q++=' ';
  // else
  //  if(iscntrl(c))
  //   {
  //    *q++ ='^';
  //    *q++ =c^0x40;
  //   }
  //  else
  //   *q++=c;
  // *q='\0';
      
  char* nextTstFile;
  p=strrchr(p,':');
  if(*p==':')
   {
    p++;
    if(p[0]=='\xfe')
     {
      p++;
      char *Opts=NULL,*Paths=NULL;
      Opts=p;
      Paths=strchr(p,'\xfe');
      Paths[0]=0;
      Paths++;
      if(Opts||Paths)
       {
        char TmpTstObj[CCHMAXPATH];
        InitPrfData(&cmdline,&prf_data,Opts,Paths,TmpTstObj);
//  ReturnCode=TestSectors();
        char* nextTstFile;
        nextTstFile=strtok(TmpTstObj," ");
        while(nextTstFile)
         {
          LOG(("Test file: <%s>",nextTstFile));
          int rez;
          rez=TestObject(nextTstFile);
          LOG(("Test file rez=%d",rez));
          nextTstFile=strtok(0," ");
         }
       }  
     }
    else
     {
      nextTstFile=strtok(p," ");
      while(nextTstFile)
       {
        LOG(("Test file: <%s>",nextTstFile));
        int rez;
        rez=TestObject(nextTstFile);
        LOG(("Test file rez=%d",rez));
        nextTstFile=strtok(0," ");
       }
     }  
   }     
  return;
 }

// Parse the line to make sure that the msg is not a composite of more
// than one message.
void TestProcessChopped(const char *hostname,char *msg,int len,int fd)
 {
  LOG(("Message(%s) length: %d, File descriptor: %d.",msg,len,fd));
  pid_t pid;
  int status;

  switch (pid=fork())
   {
    case -1:
      LOG(("fork() for test file failed"));
      return;
    case 0:	// Child 
      // close all sorts of things, including the socket fd 
//    close(unixm);
      
      // Fork twice to disassociate from the child 
      switch (pid=fork())
       {
	case -1:
          LOG(("fork(2nd) for test file failed"));
          exit(1);
	case 0:	// Child(2nd)
          acceptMutexOn();  //wait and add

          ULONG mFlags;
          reportInit(1);

          scan_goes=1;
          ExitCode=0;
	  ApplyToAll=0;
          MethodDelete=0;
	  SectorWarn=0;
          mFlags=GetFlags();

          //VirCountArray.RemoveAll();
          //iniWorkStat();
          iniWorkStatForEach();

          reportPutS("\r\nQuery for the tests: ");
reportPutS(msg);
reportPutS("\r\n\r\n");
	  TestLine(msg);
          //if( (strlen(msg)+1)!=(unsigned)len)
          // {
          //  auto char *start=msg,*end;
          //  start=strchr(msg,'\0')+1;
          //  do
          //   {
          //    end=strchr(start+1,'\0');
          //    TestLine(start);
          //    start=end+1;
          //   } while(*start!='\0');
          // }
	   
          if(prf_data.RepForEachDisk==1) reportForEach(msg);
	  // -- End of Files Check ----
          //reportDone();
	  if(workStat.suspForEach) ExitCode=3;
	  if(workStat.warnForEach) ExitCode=2;
	  if(workStat.vbForEach)
	   if(workStat.vbForEach==workStat.dfForEach) ExitCode=5;
	     else ExitCode=4;
	  scan_goes=(char)0;
	  //if(flgScan==1) flgScan=0;

          char szExitCode[0x100];
	  sprintf(szExitCode,"%d",ExitCode); //itoa(ExitCode,(char*)szExitCode,10);
          write(fd,szExitCode,strlen(szExitCode));
//          close(fd);
          
          acceptMutexOff(); //Add
          exit(0);
	default:    // Father 
	  // After grandson has been forked off, there's nothing else to do. 
          exit(0);		    
       }
    default:
      // Wait until grandson has been forked off (without wait we'd leave a zombie) 
      waitpid(pid,&status,0);
      //waitret=waitpid(pid,&status,WNOHANG);
      //if(waitret==-1 && errno==EINTR) return -1;
      return;
    }
//  close(fd);
  return;
 }

// *---------------------------------------------------------------*
// * If "retain" is not 0, then a connection is open, and the fd is
// * returned (used by "-q"). Otherwise, a check is performed
// *---------------------------------------------------------------*
static int CheckUniqueness(char* sockName,int retain)
 {
  struct sockaddr_un CtlAddr;
  int CtlFd,pid,len;
  FILE *f;
  
  bzero((char *)&CtlAddr,sizeof(CtlAddr));
  CtlAddr.sun_family=AF_UNIX;
  strcpy(CtlAddr.sun_path,sockName);
  len=sizeof(CtlAddr.sun_family)+strlen(sockName)+1;
  if((CtlFd=socket(AF_UNIX,SOCK_STREAM,0))<0) PrintExit("CheckUniqueness error: socket() not created");
  //LOG(("Checking uniqueness. socket %s",CtlAddr.sun_path));
  if(connect(CtlFd,(struct sockaddr *)(&CtlAddr),len)>=0 ||OptKill)
   {
    //LOG(("it is."));
    if(retain) return CtlFd;
    // another AvpDaemon is runnin, get its pid 
    f=fopen(AVP_NODE_PID,"r");
    if(f && fscanf(f,"%i",&pid)==1)
     {
      if(OptKill) 
       {             //USR1
        if(kill(pid,SIGTERM)==-1) PrintExit("CheckUniqueness error in kill() previous AvpDaemon");
	exit(0);
       }
      else
       {
        //fprintf(stderr,"CheckUniqueness: AvpLinuxDaemon is already running as pid %i\n",pid);
        LOG(("CheckUniqueness: AvpLinuxDaemon is already running as pid %i",pid));
        return 1;
       }
     }
    else
     {
      //fprintf(stderr,"CheckUniqueness. Error in %s: fatal error (Socket server exists, bat %s not open or not read)\n",prgname,AVP_NODE_PID);
      LOG(("CheckUniqueness. Error in %s: fatal error (Socket server exists, bat %s not open or not read)",prgname,AVP_NODE_PID));
      exit(1);
     }
   }

  // not connected, so I'm alone 
  if(retain) { close(CtlFd); return -1; }

  if(OptKill)
   {
    //fprintf(stderr,"%s: Nobody to kill (Socket server not exists)\n",prgname);
    LOG(("%s: Nobody to kill (Socket server not exists)",prgname));
    unlink(AVP_NODE_PID); // just in case 
    exit(0);
   }
  return 0; // never
 }

char	**StripDomains = NULL;	// these domains may be stripped before writing logs 
char	**LocalHosts = NULL;	// these hosts are logged with their hostname 

#ifdef _INET_
// Return a printable representation of a host address.
const char *cvthname(struct sockaddr_in *f)
 {
  struct hostent *hp;
  register char *p;
  int count;

  if(f->sin_family != AF_INET)
   {
//    dprintf("Malformed from address.\n");
    return ("???");
   }
  hp=gethostbyaddr((char *) &f->sin_addr, sizeof(struct in_addr),f->sin_family);
  if(hp == 0)
   {
//    dprintf("Host name for your address (%s) unknown.\n",inet_ntoa(f->sin_addr));
    return (inet_ntoa(f->sin_addr));
   }
  // Convert to lower case, just like LocalDomain above
  for (p = (char *)hp->h_name; *p ; p++)
   if (isupper(*p)) *p = tolower(*p);

  // Notice that the string still contains the fqdn, but your
  // hostname and domain are separated by a '\0'.
  if ((p = index(hp->h_name, '.')))
   {
    if(strcmp(p + 1, LocalDomain) == 0)
     {
      *p = '\0';
      return (hp->h_name);
     }
    else
     {
      if(StripDomains)
       {
	count=0;
	while (StripDomains[count])
	 {
	  if(strcmp(p + 1, StripDomains[count]) == 0)
	   {
	    *p='\0';
	    return (hp->h_name);
	   }
	  count++;
	 }
       }
      if(LocalHosts)
       {
	count=0;
	while (LocalHosts[count])
	 {
	  if (!strcmp(hp->h_name, LocalHosts[count]))
	   {
	    *p = '\0';
	    return (hp->h_name);
	   }
	  count++;
	 }
       }
     }
   }
  return (hp->h_name);
 }
#endif

int SendTest(char *TstStr)
 {
  struct sockaddr_un AvpTestAddr;	// AF_UNIX address of local logger 
  static int  AvpFile=-1;		// fd for log 
  int rez=-1;

  bzero((char *)&AvpTestAddr,sizeof(AvpTestAddr));
  AvpTestAddr.sun_family=AF_UNIX;
  strcpy(AvpTestAddr.sun_path,NodeCtl);
  if((AvpFile=socket(AF_UNIX,SOCK_STREAM,0))<0)
   {
    LOG(("AvpTest cannot create socket."));
    return -1;
   }

  if(connect(AvpFile,(struct sockaddr *)(&AvpTestAddr),sizeof(AvpTestAddr.sun_family)+strlen("/var/run/AvpCtl")+1)<0)
   {
    LOG(("AvpTest cannot connected to AvpDaemon")); 
    rez=-2;
   }    
  else
   {
    char tbuf[0x200];
    time_t now;

    // build the message 
    (void)time(&now);
    (void)sprintf(tbuf,"<%d>%.15s:%s",0,ctime(&now)+4,TstStr);

    // output the message to the local logger 
    if(write(AvpFile,tbuf,strlen(tbuf)+1)>=0)
     {
      LOG(("Client write string {%s} to AvpDaemon",tbuf));
      int Rez; 
      if((Rez=read(AvpFile,tbuf,0x200))==-1)
       {
        LOG(("Client cannot reaf from AvpDaemon!"));
        rez=-3;
       }
      else 
       {
        tbuf[Rez]=0;
        Rez=atoi(tbuf);
        LOG(("Test rezult: %s(%d)",tbuf,Rez));
        rez=Rez;
       }
     }
   } 
  (void)close(AvpFile);

  return(rez);
 }
 
BOOL OnInitAboutDlg()
 {
  printf("Key name      Ser. number            Price pos.             Exp. date  Trial\r\n");
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
    p=strrchr(buf,'/');
    if(p) p++;
    else  p=buf;
    if(strlen(p)<=13) memcpy(szScr,p,strlen(p));
    else  { memcpy(szScr,p,10); memcpy(&szScr[10],"...",3); }

    AVP_dword keynumber[3];
    *buf=0;
    if(DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSERIALNUMBER,0),0,keynumber,sizeof(keynumber)) )
     sprintf(buf,"%04ld-%06ld-%08ld",keynumber[0],keynumber[1],keynumber[2]);
    else DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0),0,buf,0x200);
    if(strlen(buf)<=20) memcpy(&szScr[14],buf,strlen(buf));
    else  { memcpy(&szScr[14],buf,17); memcpy(&szScr[31],"...",3); }

    strcpy(buf,"AVP Key v1.0");
    DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOSNAME,0),0,buf,0x200);
    if(strlen(buf)<=20) memcpy(&szScr[37],buf,strlen(buf));
    else  { memcpy(&szScr[37],buf,17); memcpy(&szScr[54],"...",3); }

    //SYSTEMTIME stCurrent;
    //GetSystemTime(&stCurrent);
    time_t Timer;
    struct tm* stCurrent;
    Timer=time((time_t*)NULL);
    stCurrent=localtime(&Timer);
    SYSTEMTIME stExpir;
    memset(&stExpir,0,sizeof(SYSTEMTIME));
    strcpy(buf,"None");
    if(DATA_Get_Val( hKey, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, &stExpir, sizeof(AVP_date)) )
     {
      if(stCurrent->tm_year > stExpir.wYear) goto time_bad;
      if(stCurrent->tm_year < stExpir.wYear) goto time_ok;
      if(stCurrent->tm_mon > stExpir.wMonth) goto time_bad;
      if(stCurrent->tm_mon < stExpir.wMonth) goto time_ok;
      if(stCurrent->tm_mday > stExpir.wDay) goto time_bad;
      if(stCurrent->tm_mday < stExpir.wDay) goto time_ok;
time_bad:
      strcpy(buf,"Expired");
      goto ok;
time_ok:
      sprintf(buf,"%d:%d:%d",stExpir.wDay,stExpir.wMonth,stExpir.wYear);
ok:   ;
     }
    memcpy(&szScr[60],buf,strlen(buf));

    *buf=0;
    if(DATA_Find(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))
     memcpy(&szScr[71],"Yes",3);
    else
     memcpy(&szScr[71],"No",2);
    DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLICENCEINFO,0),0,buf+strlen(buf),0x200);
//_printf("LisInfo:%s ",buf);
    szScr[74]=0;
    fprintf(stdout,"%s\r\n",szScr);
   }
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
 }

void About(void)
 {
  printf("\n+-----------------------------------------------------+\n\r"
           "| AntiViral Toolkit Pro by Eugene Kaspersky for Linux |\n\r"
           "|                       (Daemon)                      |\n\r"
           "|           Copyright(C) Kaspersky Lab. 1998          |\n\r"
           "|                Version 3.0  build 130               |\n\r"
           "|                                                     |\n\r");
  printf("+-----------------------------------------------------+\n");

  printf("                   %s                  \n\r",LoadString(IDS_REGISTERINFO,"Registration info:"));
  if(functionality)
   {
    OnInitAboutDlg();
   }
  else
    puts("                   Evaluation copy                  ");
 }

static int usage(char *whofailed)
 {
  if(whofailed)
   {
    printf("Error in the %s specification. Try \"%s -h\".\n",whofailed,prgname);
    return 1;
   }

  printf(//"Usage: %s [options]\n",prgname);
"Usage: AvpDaemon [options] path[/name][...]\n"
"       Path - any Unix path\n" 
"       Name - wildcards * or ?  Default is executable files\n"
"Valid options are:\n" //(not all of them are implemented)
"    --  disinfect                       -*  check all files\n"
"    -E  delete infected files           -F=filename load alternate profile\n"
"    -LP no symlinks are followed        -XF=masks exclude files\n"
"    -LH symlinks on the command line    -XD=masks exclude directories\n"
"        are followed                    -@=filename check files by list\n"
"    -LL all symlinks are followed\n"
"    -P[-]  skip Master Boot Record test -S[-]  sound off\n"
"    -B[-]  skip DOS Boot Sector test    -R[-]  do not scan subdirectories\n"
"    -U[-]  disable unpack               -W[T][A][-][+][=filename]  save report\n"
"    -A[-]  disable extract                -WT or -WT+ truncate existing file\n"
"    -H[-]  disable heuristic analysis     -WA or -WA+ appends to existing file\n"

"    -V[-]  enable redundant scanning      -W+ or -WA+ extended report\n"
"    -K[-]  disable pack info            -C[-]  don't cross filesystem boundaries\n"
"    -O[-]  write OK messages            -Y[-]  skip all dialogs\n"
"    -MD[-] check mail databases         -Z[-]  disable aborting\n"
"    -MP[-] check plain mail             \n"
"For all options '-' inverts the default meaning.\n"
"Press enter to view next page...\n");
  while(!feof(stdin) && (fgetc(stdin)!='\n')) ;
  printf(
"Valid options for AvpDaemon are:\n"
"    -o{obj name;...} list of checking   -k kill a running AvpDaemon,\n"
"        objects                             to restart new AvpDaemon\n"
"    -q  quit after changing             -h or -? printthis help screen\n"
"        AvpDaemon behaviour                 and exit\n"
"    -v  print version and exit          -m[number] multiple daemon regim\n"        
"Press Enter to finish...\n");
  //while(kbhit()==0);
  while(!feof(stdin) && (fgetc(stdin)!='\n')) ;
//  "  -l charset      loads the inword() LUT (default '%s')\n"
//  "  -r number       sets the responsiveness (default %i)\n"
//  "  -R              enter repeater mode. X should read /dev/gpmdata\n"
//  "                  like it was a MouseSystem mouse device\n"
  return 1;
 }

void stopProcess(void)
 {
  if(scan_goes!=0)
   {
    if(blDaemon==FALSE)
     {    
      char Chr;
      printf("\n%s (Yes/No)",LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
//    SetCursorOff(TRUE);
      Chr=(char)getchar();
      printf("\r\n");
      if((Chr=='y')||(Chr=='Y'))
       {
        prf_data.ShowOK=0;
        ExitCode=1;
        scan_goes=0;
        BAvpCancelProcessObject(1);
       }
     }
    else   
     {
      prf_data.ShowOK=0;
      ExitCode=1;
      scan_goes=0;
      BAvpCancelProcessObject(1);
     }
//    SetCursorOff(FALSE);
   }  
 }

void die(int sig)
 {
//  { }//close log

  if(sig)
   {
    stopProcess();
    char buf[100];
    (void)sprintf(buf,"Exiting on signal %d",sig);
    LOG(("Exiting on signal %d",sig));
    errno=0;
   }

  // Close the sockets. 
  close(funix);
#ifdef _INET_
  close(inetm);
#endif
  // Clean-up files. 
//  (void)unlink(Log);
//  (void)RemovePid(NodePid);
//  kill(getpid(),sig);
  exit(0);
 }

struct _cmdline cmdline;

// Возвращает количество дней прошедшее с 0-го года
unsigned long vldate_(unsigned short year,unsigned char month,unsigned char date)
 {
  return year*365+year/4+date+(month-1)*30+month/2-(year%4?2:1);
 }

BOOL flScanAll=FALSE;

char *start_path;//[CCHMAXPATH];   // Полный путь, откуда был запущен AVP
char prg_path[CCHMAXPATH];     // Полный путь, где находится AVP

char file_name[CCHMAXPATH],file_name_2[CCHMAXPATH];  // Буфера
char nameExe[CCHMAXPATH];
char scanner_name[20];

BOOL needs=0;

unsigned char f_help=0;                   // 1 - out help screen
ULONG fl_load=0;

unsigned char f_was_expir=0;      // =1 - вывести предупреждение, что лицензия
                                  // закончилась
unsigned char f_will_expir=0;     // =1 - вывести предупреждение, когда
                                  // закончиться лицензия
void setPrf(void)
 {

  if(!(functionality & FN_UNPACK))  prf_data.Packed=0;
  if(!(functionality & FN_EXTRACT)) prf_data.Archives=0;
  if(!(functionality & FN_MAIL))    prf_data.MailBases=0;
  if(!(functionality & FN_MAIL))    prf_data.MailPlain=0;

  switch (prf_data.InfectedAction)
   {
    case 0:
    case 3:
     break;
    case 1:
     if(!(functionality & FN_DISINFECT))
       prf_data.InfectedAction=0;
     break;
   }

  if(!(functionality & FN_CA)) prf_data.CodeAnalyser=0;
//   if((avp_key_flag==0)||(!(avp_key->Options&KEY_O_REMOTESCAN))) WinEnableWindow(hWndNB1RBTN2,FALSE);
 }

extern CDWordArray VirCountArray;
BYTE ctrlBreakState;               // реакция системы на "Ctrl"+"Break"

ULONG start2(void)
 {
  char dat[CCHMAXPATH];      // буфер для вывода даты
  unsigned ret=0;

  unsigned char o_report;

  LoadIO();
  //BAvpRegisterCallback();
  if(BAvpInit()==0) printf("Error allocate internal Buffers\n");  // Allocate internal Buffers

  // -- Загрузка баз && Memory check -----------------------
  char *curpath;

  curpath=getcwd(NULL,0);
  chdir(prg_path);

  if(strlen(BasePath)>0)
   {
    if(BasePath[strlen(BasePath)-1]!='/')
     sprintf(file_name,"%s/%s",BasePath,SetFile);
    else
     sprintf(file_name,"%s%s",BasePath,SetFile);
    if(access(file_name,0)==-1)
     {
      if(file_name[1]!=':')
       {
        if(BasePath[strlen(BasePath)-1]!='/')
         sprintf(file_name,"%s%s/%s",prg_path,BasePath,SetFile);
          else
         sprintf(file_name,"%s%s%s",prg_path,BasePath,SetFile);
        //if(access(file_name,0)==-1)
        // {
        // }
       }
     }
   }
  else
   {
    strcpy(file_name,SetFile);
    if(access(file_name,0)==-1)
     {
      strcpy(file_name,prg_path);
      strcat(file_name,SetFile);
     }
   }
//  if(access(file_name,0)==-1)
//   {
//    ULONG ulCurDisk;
//    ulCurDisk=_getdrive();
//    if((ulCurDisk==1)||(ulCurDisk==2))
//     {
//      printf("\nInsert disk with Antiviral bases and press <Enter>\n");
//      getchar();
//     }
//   }
  if(access(file_name,0)==-1)
   {
    printf("\nAntiviral bases not found. Press <Enter> to exit\n");
    getchar();
    ret=7;
   }
  if(load_avpbase(file_name)==FALSE)
    {DisplayMessage("Error loadin *.avc",1); ret=7;}

  chdir(curpath);

  if(ret!=0) return ret;
  fl_load|=FL_LOAD_BASE;

  // -- Проверка баз на устарение --------------------------
  unsigned long nmonth;
  int setfil;
//  USHORT date,time;
  if((setfil=open(file_name,O_RDONLY))!=(-1))
   {
//    _dos_getftime(setfil,&date,&time);
    close(setfil);
   }
//  struct dosdate_t currdate;
//  _dos_getdate(&currdate);                       // получить текущую дату
//  if(avp_key_flag) nmonth=avp_key->BaseDelay; else nmonth=3;
  if(functionality) nmonth=3;
//  if(vldate_(currdate.year,currdate.month,currdate.day)>
//    vldate_((USHORT)YEAR(date),(UCHAR)MONTH(date),(UCHAR)DAY(date))+nmonth*30)
//   DisplayMessage(LoadString(IDS_SET_OUTOFDATE,"The set of AVP antiviral bases is out of date.\nPlease take updates."),1);

  //if(prf_data.Memory) // Test of Memory
  // {
  //  out_string_(178);
  //  ScanObject.type=_MEMORY;
  //  ProcessObject();
  // }

  // -------------------------------------------------------
  if(strlen(prg_path)>0)
   prg_path[strlen(prg_path)-1]=0; // remove '\'

  // -- Проверка "AVP.EXE" на вирусы & CRC -----------------
  SelfScan=1;
  o_report=prf_data.Report;
  prf_data.Report=0;
  struct stat statbuf;
  sprintf(file_name,"%s\\%s",prg_path,nameExe);
  if(stat(file_name,&statbuf)!=(-1))
    ScanFile(file_name,statbuf.st_size,GetFlags());
  sprintf(file_name,"%s/AvpDaemon",start_path);

  if(stat(file_name,&statbuf)!=(-1))
    ScanFile(file_name,statbuf.st_size,GetFlags());
  SelfScan=0;
  prf_data.Report=o_report;

  //if((ScanObject.r_flags&RF_DETECT&&!(ScanObject.r_flags&RF_CURE))||!CheckCRC())
  // {f_damage=1; return 11;}

  //if(f_csdl) mb_ok_(8);

  //if(f_i_every_launch) info_every_launch_(); // out copy info

  // -- Вывод предупреждений касающихся лицензии -----------
//  if(avp_key!=NULL)
   {
    //send_param_ptr=(unsigned char *)dat;
    if(f_was_expir)
     {
//      sprintf(dat,LoadString(IDS_EXPIRIED_L,"Your AVP license was expiried\nat %d/%d/%d"),avp_key->ExpirMonth,avp_key->ExpirDay,avp_key->ExpirYear);
//      DisplayMessage(dat,1);
     }
//    else
     {
//      sprintf(dat,LoadString(IDS_EXPIR_ALARM_L,"Your AVP license will be expiried\nat %d/%d/%d"),avp_key->ExpirMonth,avp_key->ExpirDay,avp_key->ExpirYear);
//      if(f_will_expir) DisplayMessage(dat,1);
     }
   }

  // -- Проверка на запуск программы с сервера -------------
  //if(CheckForRemote(toupper(*prg_path)-'A')
  //   &&(!avp_key_flag||(avp_key_flag&&!(avp_key->Options&KEY_O_REMOTELAUNCH))))
  // {mb_ok_(254); return;}

  return 0;
 }

// This structure represents the files that will have log copies printed.
struct filed
 {
  short	 f_type;			// entry type, see below 
  short	 f_file;			// file descriptor 
  u_char f_pmask[LOG_NFACILITIES+1];	// priority mask 
  union
   {
    char     f_uname[MAXUNAMES][UNAMESZ+1];
    struct
     {
      char   f_hname[MAXHOSTNAMELEN+1];
#ifdef _UNIXAF_
      struct sockaddr_un f_addr;
#endif
#ifdef _INET_
      struct sockaddr_in f_addr;
#endif
     } f_forw;		// forwarding address 
    char	f_fname[MAXFNAME];
   } f_un;
  char	f_line[MAXSVLINE];	// last message logged 
//  time_t f_time;			// time this was last written 
  char	f_time[16];		// time of last occurrence 
  int	f_len;			// length of f_prevline 
  char	f_host[MAXHOSTNAMELEN+1];	// host from which recd. 
  int	f_pri;			// pri of f_prevline 
  int	f_flags;		// store some additional flags 
 };
struct	filed consfile;

// *--------------------------------------------------*
int InitPrfData(struct _cmdline *CmdLine,profile_data *PrfData,char *Opts,char *Path,char *TstObj)
 {
  TstObj[0]=0;
  unsigned short len;
  char *tmp_c=Opts;
  while(*tmp_c)
   {
    switch((*tmp_c))
 
     {
      case 'I': needs=1; break;                   
      case 'P': 
	if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Sectors=1; CmdLine->skip_mbr=0;}
        else CmdLine->skip_mbr=1;
	break;        
      case 'B':
	if((*(tmp_c+1))=='-') { tmp_c++; prf_data.Sectors=1; CmdLine->skip_boot=0;}
        else CmdLine->skip_boot=1;
	break;       
      case 'R':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->SubDirectories=1;}
        else PrfData->SubDirectories=0; 
        break; 
      case 'M':                                   //yes
        switch(toupper(*(tmp_c+1)))
         {
          case 'D':
	    tmp_c++;
    	    if((*(tmp_c+1))=='-') { tmp_c++; PrfData->MailBases=0;}
	    else PrfData->MailBases=1;
	    break;
          case 'P':
	    tmp_c++;
    	    if((*(tmp_c+1))=='-') { tmp_c++; PrfData->MailPlain=0;}
	    else PrfData->MailPlain=1;
	    break;
          default : PrfData->Memory=0;
         }
        break;
      case 'U':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->Packed=1;}
        else PrfData->Packed=0;
	break;         
      case 'A':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->Archives=1;}
        else PrfData->Archives=0;
	break;       
      case 'O':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->ShowOK=0;}
        else PrfData->ShowOK=1;
	break;         
      case 'K':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->ShowPack=1;}
        else PrfData->ShowPack=0;
	break;
      case 'S':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->Sound=1;}
        else PrfData->Sound=0;
	break;          
      case '*': PrfData->FileMask=2; break;       //yes
      case 'X':
        ++tmp_c;
        switch(toupper(*tmp_c))
         {
          case 'F':
             if(*++tmp_c=='=')
              {
               PrfData->ExcludeFiles|=0x01;
	       if(strchr(++tmp_c,'|')) len=(unsigned char)(strchr(tmp_c,'|')-tmp_c);
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
              PrfData->ExcludeFiles|=0x02;
              PLIST pList,pDel;
              while((pTail!=NULL)&&(pTail->Selected==2))
               {    
                pDel=pTail;
                pTail=pTail->pNext;
                free(pDel);
               }
              if(pTail!=NULL)
               {
                if(pTail->pNext!=NULL)
                 {
                  pList=pTail;
                  do
                   {
gtNext:             if(pList->pNext!=NULL)
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
                       goto gtNext;
                       free(pDel);
                      }
                    pList=pList->pNext;
                   } while(pList != NULL);
                 }
               }

              if(strchr(++tmp_c,'|')) len=(unsigned char)(strchr(tmp_c,'|')-tmp_c);
              else len=(unsigned short)strlen(tmp_c);
              char tmpExclDir[CCHMAXPATH],*z0;
              strncpy(tmpExclDir,tmp_c,len);
              tmpExclDir[len]=0;
	      tmp_c+=len;
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
//      case '?':
 f_help=1;
 break;
      case '-': 
//        if(toupper(*(tmp_c+1))=='H') { tmp_c++; f_help=1;
 }
//	else 
	 if(functionality) PrfData->InfectedAction=2; 
        break;
      case 'E': PrfData->InfectedAction=3; break; //yes
      case 'H':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->CodeAnalyser=1;}
        else PrfData->CodeAnalyser=0;
	break;   
      case 'V':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->RedundantScan=0;}
        else PrfData->RedundantScan=1;
	break;  
      case 'F':                                   //yes
        char defprofile[CCHMAXPATH];
        if(*++tmp_c=='=')
         {
          if(strchr(++tmp_c,'|')) len=(unsigned char)(strchr(tmp_c,'|')-tmp_c);
           else len=(unsigned short)strlen(tmp_c);
          strncpy(defprofile,tmp_c,len);
          defprofile[len]=0;
	  tmp_c+=len;
          loadProfile(defprofile,NULL);
         }
        break;
      case 'W':                                     //yes
        PrfData->Report=1;
	BOOL blReinit;
	blReinit=FALSE;
        while((*tmp_c)&&(*tmp_c!='|'))
         switch(toupper(*++tmp_c))
          {
           case '-': PrfData->Report=0; blReinit=TRUE; break;
 //ExtReport=0
           case 'T': PrfData->Append=0; blReinit=TRUE; break;
           case 'A': PrfData->Append=1; blReinit=TRUE; break;
           case '+': PrfData->ExtReport=1; blReinit=TRUE; break;
           case '=':
             if(strchr(++tmp_c,'|')) len=(unsigned char)(strchr(tmp_c,'|')-tmp_c);
             else len=(unsigned char)strlen(tmp_c);
             strncpy(PrfData->ReportFileName,tmp_c,len);
             PrfData->ReportFileName[len]=0;
	     tmp_c+=len;
	     blReinit=TRUE;
             break;
          }
	if(blReinit==TRUE) reportReinit();
        break;
      case '@':                                     //yes
        needs=1;
        //if(*++tmp_c=='!') {CmdLine->KillList=1; tmp_c++;}
        if(*tmp_c=='=')
         {
          if(strchr(++tmp_c,'|')) len=(unsigned char)(strchr(tmp_c,'|')-tmp_c);
          else len=(unsigned short)strlen(tmp_c);
          char ListFile[0x200];
	  strncpy(ListFile,tmp_c,len);
          ListFile[len]=0;
	  tmp_c+=len;
      
          int hListFile;
          if((hListFile=open(ListFile,O_RDONLY))!=(-1))
 //|O_BINARY
           {  
            struct stat statbuf;
            int Len;      //новая длина существующего файла
            fstat(hListFile,&statbuf);
            free(CmdLine->ListFile);
            if((CmdLine->ListFile=(char*)malloc(statbuf.st_size+1))!=0)
             {
              if((Len=read(hListFile,CmdLine->ListFile,statbuf.st_size))!=(-1))
               {
                if(Len<statbuf.st_size) CmdLine->ListFile[Len]=0;
                else CmdLine->ListFile[statbuf.st_size]=0;
               }
             }
            close(hListFile);
           }
         }
        break;
      case 'Y':
	if((*(tmp_c+1))=='-') { tmp_c++; CmdLine->skip_dlg=0;}
        else CmdLine->skip_dlg=1;
	break;
      case 'Z':
	if((*(tmp_c+1))=='-') { tmp_c++; CmdLine->abort_disable=0;}
        else CmdLine->abort_disable=1;
	break;
      case 'L':     
        switch(toupper(*(tmp_c+1)))
         {
          case 'P': tmp_c++; PrfData->Symlinks=0; break;
 //no symlinks are followed
          case 'L': tmp_c++; PrfData->Symlinks=2; break;
 //all symlinks are followed
          case 'H':   //symlinks on the command line are followed
          default : tmp_c++; PrfData->Symlinks=1; break;

         }
        break;
      case 'C':
	if((*(tmp_c+1))=='-') { tmp_c++; PrfData->CrossFs=1;}
        else PrfData->CrossFs=0;
	break;   
      case 'o':
        if(*++tmp_c=='{')
         {
          if(strchr(++tmp_c,'}')) len=(unsigned char)(strchr(tmp_c,'}')-tmp_c);
	   {
            strncpy(TstObj,tmp_c,len);
            TstObj[len]=0;
	    tmp_c+=len;
	   }
         }
        break;
//     case 'L':
//        if((*(tmp_c+1))=='-') { tmp_c++; PrfData->ScanRemovable=0;}
//        else PrfData->ScanRemovable=1;
//        break;

//    case 'N': CmdLine->remote=1; break;
//    case 'T':
//      if(*++tmp_c!='=') while(*tmp_c&&*tmp_c!='/') tmp_c++;
//      else
//       {
//        if(strchr(++tmp_c,'/')) len=(unsigned char)(strchr(tmp_c,'/')-tmp_c);
//        else len=(unsigned short)strlen(tmp_c);
//        strncpy(temp_path,tmp_c,len);
//        temp_path[len]=0;
//        tmp_c+=len;
//       }
//      break;
     } // switch(toupper(*++tmp_c))
    while(*tmp_c&&*tmp_c!='|') tmp_c++;
    if(*tmp_c) tmp_c++;
 
   } // while(*tmp_c)

  // -------------------------------------------------------
  if(*Path)
   {
    SelectAllRecords(0);
    char *_ttop=strtok(Path,";");
    while(_ttop)
     {
      //clear last / if strlen>1
      if(_ttop[1]&&_ttop[strlen(_ttop)-1]=='/') _ttop[strlen(_ttop)-1]=0;
      // if * or *:
      if(*_ttop=='*'&&(!_ttop[1]||_ttop[1]==':'&&!(_ttop[2])))
       {
        flScanAll=TRUE;
        SelectDiskType(2,1);
        if(PrfData->ScanRemovable==1)
         {
          SelectDiskType(1,1);
          SelectDiskType(3,1);
         }
       }
//      else
//       if(_ttop[1]==':'&&!_ttop[2]&&
//          toupper(*_ttop)>='A'&&toupper(*_ttop)<='Z')//&&Disk[toupper(*_ttop)-'A'])
//         AddRecord((char*)_ttop,1);
       else
        {
         if((!strcmp(_ttop,"."))||(!strcmp(_ttop,"./"))) AddRecord(start_path,1);
         else
          {
//           PrfData->Path=1;
           if(access(_ttop,0)!=-1)
 AddRecord(_ttop,1);
           else
            {
             LOG(("None directory <%s>",_ttop));
//              char *szTmpMsk=GNAM(_ttop);
//              strcpy(PrfData->UserMask,szTmpMsk);
//(strrchr(_ttop,'/')?(strrchr(_ttop,'/')+1):
:_ttop);


//
//              if((strlen(szTmpMsk)>0)&&(szTmpMsk[0]=='{'))
//               {
//                strncpy(PrfData->UserMask,&szTmpMsk[1],strlen(szTmpMsk)-1);
//                PrfData->UserMask[strlen(szTmpMsk)-1]=0;
//               }
//              else
//               {
//                strncpy(PrfData->UserMask,szTmpMsk,strlen(szTmpMsk));
//                PrfData->UserMask[strlen(szTmpMsk)]=0;
//               }
//              if(PrfData->UserMask[strlen(PrfData->UserMask)-1]=='}')
//                PrfData->UserMask[strlen(PrfData->UserMask)-1]=0;
//              if(szTmpMsk!=_ttop)
//               {
//                strncat((char*)szT,_ttop,GNAM(_ttop)-_ttop);
//                //if((strlen((char*)szT)<=3)&&(szT[1]==':')&&
//                //  (tolower(szT[0])>='a') && (tolower(szT[0])<='z') )
//                // {if(szT[2]!='\\') strcat((char*)szT,"\\");}
//                //else
//                 if(szT[strlen((char*)szT)-1]=='/') szT[strlen((char*)szT)-1]=0;
//                AddRecord((char*)szT,1);
//            }
//           else
//            {
//             char *CurrDir;
//             CurrDir=getcwd(NULL,0);
//             AddRecord(CurrDir,1);
//             Free(CurrDir);
//            }
//           PrfData->FileMask=3;
           }
         }
       }
      _ttop=strtok(NULL,";");
     }
    needs=1;
   }
  return 0;
 }
 
int main(int argc, char **argv)
 {
  prgname=argv[0];
  setuid(0); // real user id (just in case... )

  // ... parse command line 
  //int  opt;
  //while((opt=getopt(argc,argv,"qkdmvh"))!=-1)
  // {
  //  switch (opt)
  for(unsigned char z=1;z<argc;z++)
   {
    if(*argv[z]=='-')
     switch (argv[z][1])
     {
      case 'd': ; break; //opt_...=optarg
      case 'q': OptQuit++; break;
      case 'k': OptKill++; break;
      case 'm': 
        if(argv[z][2]=='[')
         {
          int len;
          if(strchr(&argv[z][3],']')) 
           {
            len=(unsigned char)(strchr(&argv[z][3],']')-&argv[z][3]);
//	    int oldlen=strlen(NodeCtl);
//	    strncat(&NodeCtl[oldlen],&argv[z][3],len);
//            NodeCtl[oldlen+len]=0;
	    //OptMult=atoi(&NodeCtl[oldlen]);
//printf("NodeCtl<%s><%s> %d %d\n",NodeCtl,&argv[z][3],oldlen,len);	    
           }
	 }  
	break;
//      case 'l':
//	if (LocalHosts)
//	 {
//	  printf ("Only one -l argument allowed,the first one is taken.\n");
//  	  break;
//	 }
//	LocalHosts = crunch_list(optarg);
//	break;
//      case 'p':		/* path */
//	LogName = optarg;
//	break;
//      case 'r':		/* accept remote messages */
//	AcceptRemote = 1;
//	break;
//      case 's':
//	if (StripDomains)
//	 {
//	  printf ("Only one -s argument allowed, the first one is taken.\n");
// 	  break;
//	 }
//	StripDomains = crunch_list(optarg);
//	break;
      case 'h': 
      case '?': exit(usage(NULL));
      case 'v': printf(AVP_NAME " " AVP_RELEASE ", " AVP_DATE "\n"); exit(0);
      //default: usage("commandline");

      //case 'i': opt_time=atoi(optarg); break;
      //case 'l': opt_lut=optarg; break;
      //case 'L': opt_lut=optarg; opt_quit++; break;
      //case 'o':
      //  if(!strcmp(optarg,"dtr"))       opt_toggle=TIOCM_DTR;
      //    else if (!strcmp(optarg,"rts"))  opt_toggle=TIOCM_RTS;
      //      else exit(usage("toggle lines"));
      //      break;
      //case 'p': opt_ptrdrag=0; break;
      //case 'R': opt_repeater++; break;
     }
   }

  char CmdOpts[0x50];
  char CmdPath[0x200];
  CmdOpts[0]=0;
  CmdPath[0]=0;
  for(unsigned char z=1;z<argc;z++)
   {
    if((strlen(szExtRep)+strlen(argv[z])+2)<0x200) 
     {strcat(szExtRep,argv[z]); strcat(szExtRep," ");
 }
    if(*argv[z]=='-')
     {
      if((strlen(CmdOpts)+strlen(argv[z]))<0x50)
       { strcat(CmdOpts,&argv[z][1]);
 strcat(CmdOpts,"|");}
     }
    else
     {
      if((strlen(CmdPath)+strlen(argv[z])+2)<0x200) 
       { strcat(CmdPath,argv[z]); strcat(CmdPath,";");}
     }
   }
  if(argc<=1) strcat(szExtRep,"None");
  strcat(szExtRep,"\r\n");

  if(CheckUniqueness(NodeCtl,0)==1) //exists
   {
    if((strlen(CmdOpts)+strlen(CmdPath))==0) exit(0);
     // now we can put the options 
    int ret=0;
    char *buf;
    buf=(char *)malloc(strlen(CmdOpts)+strlen(CmdPath)+3);
    if(buf)
     {
      strcpy(buf,"\xfe");
      strcat(buf,CmdOpts);
      strcat(buf,"\xfe");
      strcat(buf,CmdPath);
      ret=SendTest(buf);
      free(buf);
     } 
    LOG(("Client requiest rezult: %d ",ret));
    exit(ret);
   }    

  //if(OptRepeater)
  // {
  //  if(mkfifo(AVP_NODE_FIFO,0666) && errno!=EEXIST) PrintExit(AVP_NODE_FIFO);
  //  if((fifofd=open(AVP_NODE_FIFO,O_RDWR|O_NONBLOCK))<0) PrintExit(AVP_NODE_FIFO);
  // }

  // *----------- start ------------------*

  // -- получить имя сканера -------------------------
  unsigned char  *pt;
  pt=(unsigned char*)strrchr(*argv,'/');
  if(pt==NULL) pt=(unsigned char*)(*argv-1);
  strcpy(file_name,(char*)(pt+1));
  // *(strchr(file_name,'.'))=0;
  //strupr(file_name);
  strcpy(scanner_name,file_name);

  // -- получить полный путь откуда был запущен AVP --
  if((start_path=getcwd(NULL,0))==NULL)
   {
    //fprintf(stderr,"getcwd() failed\n");
    printf("getcwd() failed\n");
    exit(0);
   }

  if(!*start_path || start_path[strlen(start_path)-1]!='/') strcat(start_path,"/");

  // -- получить полный путь к AVP -------------------
  strcpy(prg_path,*argv); // program name
  for(int I=(unsigned short)(strlen(prg_path)-1);I!=0;I--)
   {
    switch(prg_path[I])
     { 
      case '\\':
      case '/':
      case ':': prg_path[I+1]=0; goto avp_path_done;
     } 
   }
  prg_path[0]=0;
avp_path_done:
//printf("s: %s \np: %s |%s\n",start_path,prg_path,*argv);
  if(load_ini("AvpUnix.ini")==0)
 fl_load|=FL_LOAD_INI;

//   if(prg_path[strlen(prgPath)-1]=='\\')
//    sprintf(file_name,"%sAvpDaemon",prg_path);
//   else
//    sprintf(file_name,"%s\\AvpDaemon",prg_path);
//   if(access(file_name,0)==-1)
//     sprintf(file_name,"%s\\AvpDaemon",start_path);
//   if(sign_check_file(file_name,1,0,0,0)) { }
//   else fl_load|=FL_UNSIGN_EXE;

  // -- Загрузка "avp.key" ---------------------------
  SearchFile(KeyFile,file_name,".key");
//  avp_key_flag=load_key_file_(file_name);

  time_t Timer;
  struct tm* Tm;
  Timer=time((time_t*)NULL);
//  Tm=gmtime(&Timer);
  Tm=localtime(&Timer);

  functionality=0;
  if(strlen(file_name)>0)
    functionality|=LoadKeys(file_name,&KeyHandleArray,AVP_APID_LINUX);
  if(strlen(KeysPath)==0) strcpy(KeysPath,prg_path);
  if(KeysPath[strlen(KeysPath)-1]!='/') strcat(KeysPath,"/*.key");
  else strcat(KeysPath,"*.key");

  functionality|=LoadKeys(KeysPath,&KeyHandleArray,AVP_APID_LINUX);

  functionality|=LoadKeys(start_path,&KeyHandleArray,AVP_APID_LINUX);
  if(!functionality)
   {
    //tPage2.m_Packed   &= !!(functionality & FN_UNPACK);
    //tPage2.m_Archives &= !!(functionality & FN_EXTRACT);
    //tPage2.m_MailBases&= !!(functionality & FN_MAIL);
    //tPage2.m_MailPlain&= !!(functionality & FN_MAIL);
    //tPage3.m_CA       &= !!(functionality & FN_CA);
    //if(!(functionality & FN_DISINFECT) && tPage6.m_Action<3)  tPage6.m_Action = 0;
//    puts("\nAVP Key is not loaded or your AVP license was expiried\n");
   }
  else  //avpKeyFlag=1;
   {
    fl_load|=FL_LOAD_KEY;
//    if(avp_key->Flags&KEY_F_INFOEVERYLAUNCH) f_i_every_launch=1;
//    if(avp_key->ExpirMonth&&avp_key->ExpirDay)
     {
//      if(vldate_(Tm->tm_year,Tm->tm_mon,Tm->tm_mday)>
//        vldate_(avp_key->ExpirYear,(unsigned char)avp_key->ExpirMonth,(unsigned char)avp_key->ExpirDay))
       {
//        f_was_expir=1;
//        avp_key_flag=0;
       }
//      else
//       if(vldate_(Tm->tm_year,Tm->tm_mon,Tm->tm_mday)+avp_key->AlarmDays>
//         vldate_(avp_key->ExpirYear,(unsigned char)avp_key->ExpirMonth,(unsigned char)avp_key->ExpirDay))
//      f_will_expir=1;
     }
   }

  // -- Загрузка "default.prf" -----------------------------
  strcat(szExtRep,"Profile ");
  strcat(szExtRep,DefProfile);
  set_default_options_(); // Установка опций по умолчанию
  if(loadProfile(DefProfile,szExtRep)==0)
   fl_load|=FL_LOAD_PRF;
  else
   strcat(szExtRep," (profile is not loaded)");
  strcat(szExtRep,"\r\n");

  cmdline.skip_mbr      =0;
  cmdline.skip_boot     =0;
  cmdline.skip_dlg      =0;
  cmdline.remote        =0;
  cmdline.abort_disable =0;
  cmdline.ListFile      =0;
  cmdline.start_minumize=0;

  char tmpTstObj[CCHMAXPATH];
  InitPrfData(&cmdline,&prf_data,CmdOpts,CmdPath,tmpTstObj);

  if(f_help==1)
   {
    usage(NULL);
    exit(0);
   }    

  pt=(unsigned char*)strrchr(*argv,'/');
  if(pt==NULL) pt=(unsigned char*)strrchr(*argv,':');
  if(pt==NULL) pt=(unsigned char*)(*argv-1);
  strcpy(nameExe,(char*)(pt+1));

  About();
  if((fl_load & FL_LOAD_INI)==0) if(prf_data.OtherMessages)DisplayMessage("Ini-file is not loaded.",0);
  if((fl_load & FL_LOAD_KEY)==0) if(prf_data.OtherMessages)DisplayMessage("AVP Key is not loaded or your AVP license was expiried.",0);
  if((fl_load & FL_LOAD_PRF)==0) if(prf_data.OtherMessages)DisplayMessage("Profile is not loaded.",0);
//  if((fl_load & FL_UNSIGN_EXE)==0) if(prf_data.OtherMessages)DisplayMessage("AvpDos32.exe has been damaged or infected by unknown virus!",0);

//  interruptHandler(SIGINT,hndlr24);
  if(start2())
   {
//    interruptDef(SIGINT);
    return -1;
    //exit(0);
   }    

  acceptMutexInit((pool*)0);
  //acceptMutexCleanup((void *)0);

  initWorcStat();
  VirCountArray.SetSize(0,0x200);
  reportInit(0);

  if(prf_data.FileMask==3)
   {
    BAvpAddMask(NULL);
    CHAR  s[CCHMAXPATH];
    strcpy(s,prf_data.UserMask);
    char *span;
    span=strtok(s,";, ");
    while(span)
     {
      if(strlen(span)<13) BAvpAddMask(span);
 //strlwr
      span=strtok(NULL,";, ");
     }
   }
  //ScanObject.mode=prf_data.FileMask;

//  if(!(*cmdline.ListFile||(BOOL)IfSelectRecords(1) ) ) //!clStart)
  if(!((BOOL)IfSelectRecords(1)) ) //!clStart)
   {
    if(prf_data.OtherMessages)
     avp_mess(AVP_CALLBACK_MB_OK,(ULONG)LoadString(IDS_NOTHING_LOCATION_L,"Nothing to scan.\x0d\x0aYou should select at least one directory to scan."));
//    f_help=1;
   }
  else
   if((prf_data.Memory==0)&&(prf_data.Sectors==0)&&(prf_data.Files==0))
    {
     if(prf_data.OtherMessages)
      avp_mess(AVP_CALLBACK_MB_OK,(ULONG)LoadString(IDS_NOTHING_FILES_L,"Nothing to scan.\x0d\x0aYou should select Files and/or Sectors in *.prf file."));
//     f_help=1;
    }
   else
  //    if(prf_data.InfectedAction==3)
  //     {
  //      if(avp_mess(MPFROMLONG(AVP_CALLBACK_MB_OKCANCEL),LoadString(IDS_SHURE_DELETEALL,"Вы уверены в том, что Вы хотите УДАЛИТЬ ВСЕ зараженные объекты?"))==DID_OK)
  //       goto scanstart;
  //      else
  //       WinSendMsg(hWndNBook,BKM_TURNTOPAGE,(MPARAM)realPageInfo[2].ulPageID,NULL);
  //     }
  //    else
    {
     acceptMutexOn();  //wait and add
     int rez;
     scan_goes=1;
     ExitCode=TestSectors();
     ApplyToAll=0;
     MethodDelete=0;
     SectorWarn=0;

     char *szScan;
     szScan=strtok(tmpTstObj,"; ,");
     while(szScan)
      {
       rez=TestObject(szScan);
       szScan=strtok(0,"; ,");
      }
     if(workStat.suspForEach) ExitCode=3;
     if(workStat.warnForEach) ExitCode=2;
     if(workStat.vbForEach)
      if(workStat.vbForEach==workStat.dfForEach) ExitCode=5;
        else ExitCode=4;
     scan_goes=(char)0;
   
     acceptMutexOff(); //Add

     printf("\r                                                      \r");
     if(scan_goes==1)
       printf(LoadString(IDS_SCANPROC_INCOMPLETE,"Scan process incompleted.\n"));
     else
       printf(LoadString(IDS_SCANPROC_COMPLETE,"Scan process completed.\n"));

     if(workStat.cek==0) workStat.cek=1;
     printf("\n%s %6ld    %s %6ld\n",workStat.Sectors,workStat.sect,workStat.knownVir,workStat.kv);
     printf("%s %6ld    %s %6ld\n",workStat.Files,workStat.fil,workStat.virBodies,workStat.vb);
     printf("%s %6ld    %s %6ld\n",workStat.Folder,workStat.fold,workStat.Disinfect,workStat.df);
     printf("%s %6ld    %s %6ld\n",workStat.Archives,workStat.arch,workStat.Deleted,workStat.deleted);
     printf("%s %6ld    %s %6ld\n",workStat.Packed,workStat.pack,workStat.Warning,workStat.warn);
     printf("                                    %s %6ld\n",workStat.Suspis,workStat.susp);
     printf("%s %6d    %s %6ld\n",workStat.countSize,(int)(workStat.CountSize/workStat.cek),workStat.Corript,workStat.cor);
     printf("%s  %02d:%02d:%02d %s %6ld\n\n",workStat.scanTime,workStat.Tm.tm_hour,workStat.Tm.tm_min,workStat.Tm.tm_sec,workStat.IOError,workStat.err);
    } 

  if(OptQuit)
   { 
//  ScanList.RemoveAll();
    exit(ExitCode);
   }

  blDaemon=TRUE;

#ifndef DEBUG // avoid forking under gdb 
  int quitpid = 0;

  quitpid=getpid();
  // go to background and become a session leader
  switch(fork())
   {
    case -1: PrintExit("fork(). Unable to fork new process"); // error 
    case  0: break;          // child 
    default:
      // We try to wait the end of initialization 
//      sleep(10); //exit error;
      exit(ExitCode);        // parent 
   }
  // The setpgrp() stuff was snarfed from http_main.c 
  //#ifndef NO_SETSID
  //if((pgrp=setsid())==-1)
  // { fpprintf(stderr,"setsid failed\n"); exit(1); }
  //#elif defined(NEXT) || defined(NEWSOS)
  //pid_t pgrp;
  //if(setpgrp(0,getpid())==-1 || (pgrp=getpgrp(0)) == -1)
  // { fprintf(stderr, "setpgrp or getpgrp failed\n"); exit(1); }
  //#else
  //  if((pgrp=setpgrp(getpid(),0))==-1)
  //   { fprintf(stderr,"setpgrp failed\n"); exit(1); }
  //#endif

  // close out the standard file descriptors 
  if(freopen("/dev/null","r",stdin)==NULL)
    fprintf(stderr,"unable to replace stdin with /dev/null: %s\n",strerror(errno));
    // continue anyhow -- note we can't close out descriptor 0 because we
    // have nothing to replace it with, and if we didn't have a descriptor
    // 0 the next file would be created with that value ... leading to havoc.

  if(freopen("/dev/null","w",stdout)==NULL)
    fprintf(stderr,"unable to replace stdout with /dev/null: %s\n",strerror(errno));
    // stderr is a tricky one, we really want it to be the error_log,
    // but we haven't opened that yet.  So leave it alone for now and it'll
    // be reopened moments later.

  if(!freopen("/dev/console","w",stderr)) // the currently current console 
    {printf("AvpDaemon: freopen(stderr) failed\n"); exit(1);}

  if(setsid()<0) PrintExit("setsid()");
#endif

//  if(!opt_kernel) // FIXME -- this is to find kmouse.o 
//   {
//    if(chdir(GPM_NODE_DIR) && mkdir(GPM_NODE_DIR,GPM_NODE_DIR_MODE))
//      PrintExit(GPM_NODE_DIR);
//    if(chdir(GPM_NODE_DIR))
//      PrintExit(GPM_NODE_DIR); // well, I must create my directory first 
//   }

  register int i;
#ifndef DEBUG 
 #ifndef CONFIG_AVP_LOG      
  int num_fds;
  num_fds=getdtablesize();
//  for(i=0;i<num_fds;i++) (void)close(i);
 #endif
  //  untty();
#else
 #ifndef SYSV
  setlinebuf(stdout);
 #endif
#endif

  // now sign 
  if(!WritePid(AVP_NODE_PID)) //PidFile
   {
    LOG(("Can't write pid."));
    exit(1);
   }

  register char *p;

 #if !defined(__GLIBC__)
  int lens;
 #else  // __GLIBC__ 
  size_t lens;
 #endif // __GLIBC__

  fd_set unixm, readfds;
  struct hostent *hent;

  //consfile.f_type=F_CONSOLE;
  //(void)strcpy(consfile.f_un.f_fname,ctty);
  (void)gethostname(LocalHostName,sizeof(LocalHostName));
//reportPutS("\r\nchild gethostname Host");
reportPutS(LocalHostName);
reportPutS("\r\n");

  if( (p=index(LocalHostName,'.')) )
   {
    *p++ ='\0';
    LocalDomain=p;
   }
  else
   {
    LocalDomain="";

    // It's not clearly defined whether gethostname() should return the simple
    // hostname or the fqdn. A good piece of software should be aware of both
    // and we want to distribute good software.  Joey
		 
    hent=gethostbyname(LocalHostName);
    if(hent!=NULL)
     {
      strcpy(LocalHostName,(char*)hent->h_name);
//reportPutS("\r\nchild index gethostbyname!!");
reportPutS(LocalHostName);
reportPutS("\r\n");

      if( (p=index(LocalHostName,'.')) )
       {
        *p++ = '\0';
        LocalDomain=p;
       }
     }  
   }
  // Convert to lower case to recognize the correct domain laterly
  for(p=(char *)LocalDomain;*p;p++)
   if(isupper(*p)) *p=tolower(*p);
  LOG(("LocalHostName: %s LocalDomain: %s",LocalHostName,LocalDomain));

  // ... catch interesting signals 
  if(getppid()!=1)
   {//SIGTTOU SIGTTIN SIGTSTR
    (void)signal(SIGALRM,SIG_IGN);// domark);
    (void)signal(SIGCHLD,SIG_IGN);// reapchild);
    (void)signal(SIGINT, die);// SIG_IGN);
    (void)signal(SIGQUIT,die);// SIG_IGN);
    (void)signal(SIGTERM,die);                            //AvpKilled);
    (void)signal(SIGUSR1,SIG_IGN);// Debug ? debug_switch : SIG_IGN); //AvpKilled);
   } // usr1 is used by a new Avp killing the old     
  //(void)alarm(TIMERINTVL);
  //(void)unlink(LogName);

  chdir("/");

#ifdef _UNIXAF_
  int fd;
  struct sockaddr_un sunx, fromunix;
  char line[MAXLINE+1];

  bzero((char *)&sunx,sizeof(sunx));
  sunx.sun_family=AF_UNIX;
  (void)strncpy(sunx.sun_path,NodeCtl,sizeof(sunx.sun_path));
  unlink(NodeCtl);
  funix=socket(AF_UNIX,SOCK_STREAM,0);
  if(funix<0)
   {
    (void)sprintf(line,"cannot create %s",sunx.sun_path);
    LOGERROR((line));
   }
  if(bind(funix,(struct sockaddr *)&sunx,
    sizeof(sunx.sun_family)+strlen(sunx.sun_path)+1)<0 ||
    chmod(NodeCtl,0777)<0 || listen(funix,5)<0)
   {
    (void)sprintf(line,"cannot bind to %s,%d %d",sunx.sun_path,sizeof(sunx.sun_family),strlen(sunx.sun_path));
    LOGERROR((line));
 #ifndef SYSV
    die(0);
 #endif
   }
#endif

#ifdef _INET_
  struct sockaddr_in sin, frominet;
  char *from;
  char line[MAXLINE +1];

  finet=socket(AF_INET,SOCK_STREAM,0);//DGRAM,0);
  if(finet==-1)
   {
    LOGERROR(("AvpDaemon: Unknown protocol, suspending inet service."));
    exit(-1);
   }
  
  auto int on = 1;
  struct servent *sp;

  sp=getservbyname("AvpDaemon","tcp");
  if(sp==NULL)
   {
    errno=0;
    LOGERROR(("network logging disabled (AvpDaemon/tcp service unknown)."));
    LOGERROR(("see AvpDaemon man for details of whether and how to enable it."));
  
    int nport;
    nport=LogPort;
    nport=htons((u_short)nport);
    bzero((char *)&sin,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=LogPort;
    sin.sin_addr.s_addr=INADDR_ANY;
    if(bind(finet,(struct sockaddr *)&sin,sizeof(sin)) < 0)
     {
      LOGERROR(("bind, suspending inet"));
      exit(-1);
     }
    else
     {
      if(listen(finet,5)==-1)
       {
        LOG(("Error listening on AvpDaemon TCP port."));
        exit(-1);
       }
       
      inetm=finet;
      InetInuse=1;
      LOG(("listening on AvpDaemon TCP port."));
     }
   }
  else
   {
    sin.sin_family=AF_INET;
    sin.sin_port =LogPort=sp->s_port;
    sin.sin_addr.s_addr=0;
//    if(setsockopt(finet,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on))<0)
//     {
//      LOGERROR(("setsockopt, suspending inet"));
//     }
//    else
//     {
      if(bind(finet, (struct sockaddr *)&sin,sizeof(sin)) < 0)
       {
        LOGERROR(("bind, suspending inet"));
       }
      else
       {
        inetm=finet;
        InetInuse=1;
        LOG(("listening on AvpDaemon TCP port."));
       }
//     }
   }
#endif

#ifdef DEBUG 
  LOG(("Debugging disabled, SIGUSR1 to turn on debugging."));
  debugging_on = 0;
#endif

  if(quitpid) kill(quitpid,SIGINT);

  struct rlimit flim;
  getrlimit(RLIMIT_NOFILE,&flim);
//  for(int fd=0;fd<flim.rlim_max;fd++) close(fd);

  openlog("AvpLog",LOG_PID|LOG_CONS,LOG_DAEMON);
  syslog(LOG_INFO,"AvpDaemon started.");
  closelog();
  
  // Main loop begins here. 
  FD_ZERO(&unixm);
  FD_ZERO(&readfds);
  for(;;)
   {
    int nfds;
    errno = 0;
#ifdef _UNIXAF_
    //Add the Unix Domain Socket to the list of read descriptors.
    FD_SET(funix, &readfds);
    for(nfds=0;nfds<FD_SETSIZE;++nfds)
     if(FD_ISSET(nfds,&unixm)) FD_SET(nfds,&readfds);
#endif
#ifdef _INET_
    //Add the Internet Domain Socket to the list of read descriptors.
    if(InetInuse&&AcceptRemote) FD_SET(inetm,&readfds);
#endif

#ifdef DEBUG 
    LOG(("Calling select, active file descriptors: "));
    for(nfds=0;nfds<FD_SETSIZE;++nfds)
     if(FD_ISSET(nfds,&readfds) ) LOG(("%d "),nfds);
    LOG(("\n"));
#endif
    nfds=select(FD_SETSIZE,(fd_set*)&readfds,(fd_set*)NULL,
	(fd_set *)NULL,(struct timeval *)NULL);
    if(restart)
     {
      LOG(("\nReceived SIGHUP, reloading AvpDaemon."));
      restart=0;
      continue;
     }
    if(nfds==0)
     {
      LOG(("No select activity."));
      continue;
     }
    if(nfds<0)
     {
      if(errno!=EINTR) LOGERROR(("select"));
      LOG(("Select interrupted."));
      continue;
     }

#ifdef DEBUG 
    LOG(("\nSuccessful select, descriptor count = %d, Activity on: ",nfds));
    for(nfds=0;nfds<FD_SETSIZE;++nfds)
      if(FD_ISSET(nfds,&readfds) ) LOG(("%d ",nfds));
    LOG(("\n"));
#endif

#ifdef _UNIXAF_
 #ifdef DEBUG 
    LOG(("Checking UNIX connections, active: "));
    for(nfds=0;nfds<FD_SETSIZE;++nfds)
      if(FD_ISSET(nfds,&unixm) ) LOG(("%d ",nfds));
    LOG(("\n"));
 #endif
    for(fd=0;fd<=FD_SETSIZE;++fd)
     if(FD_ISSET(fd,&readfds) && FD_ISSET(fd,&unixm) )
      {
       LOG(("Message from UNIX socket #%d.",fd));
       memset(line,'\0',sizeof(line));
       i=read(fd,line,MAXLINE);
       if(i>0)
         TestProcessChopped(LocalHostName,line,i,fd);
       else
        if(i<0)
         {
          if(errno!=EINTR) LOGERROR(("recvfrom unix"));
	 }
	else
	 {
	  LOG(("Unix socket (%d) closed.",fd));
	  //if(parts[fd]!=(char *)0)
	  // {
	  //  LOGERROR(("Printing partial message"));
	  //  line[0] = '\0';
	  //  TestProcessChopped(LocalHostName,line,strlen(parts[fd]) + 1,fd);
	  // }
	  close(fd);
	  FD_CLR(fd,&unixm);
	  FD_CLR(fd,&readfds);
	 }
       }
    // Accept a new unix connection 
    if(FD_ISSET(funix,&readfds))
     {
      lens=sizeof(fromunix);
      if((fd=accept(funix,(struct sockaddr *)&fromunix,&lens))>=0)
       {
        LOG(("New UNIX connect assigned to fd: %d.",fd));
        FD_SET(fd,&unixm);
        FD_SET(fd,&readfds);
       }
      else
       {
        LOG(("Error accepting UNIX connection: %d = %s.",errno,strerror(errno)));
       }
     }
#endif

#ifdef _INET_
    if(InetInuse && AcceptRemote && FD_ISSET(inetm, &readfds))
     {
      lens=sizeof(frominet);
      memset(line, '\0', sizeof(line));
      i=recvfrom(finet,line,MAXLINE-2,0,(struct sockaddr *)&frominet,&lens);
      LOG(("Message from inetd socket: #%d, host: %s",
	inetm, inet_ntoa(frominet.sin_addr)));
      if(i>0)
       {
        line[i]=line[i+1]='\0';
        from=(char *)cvthname(&frominet);
	// Here we could check if the host is permitted to send us AvpDaemon
	// messages. We just have to catch the result of cvthname, look for a dot
	// and if that doesn't exist, replace the first '\0' with '.' and we have
	// the fqdn in lowercase letters so we could match them against whatever.
	TestProcessChopped(from,line,i + 2,  finet);
       }
      else
       if(i<0 && errno!=EINTR)
        {
	 LOG(("INET socket error: %d = %s.",errno,strerror(errno)));
	 LOGERROR(("recvfrom inet"));
	 sleep(10);
	}
     }
#endif
   }
  reportDone(0);
 }

#ifdef NEED_WAITPID
// From ikluft@amdahl.com
// this is not ideal but it works for SVR3 variants
// Modified by dwd@bell-labs.com to call wait3 instead of wait because
//   apache started to use the WNOHANG option.
//int waitpid(pid_t pid, int *statusp, int options)
// {
//  int tmp_pid;
//  if(kill(pid, 0) == -1)
//   {
//    errno = ECHILD;
//    return -1;
//   }
//  while(((tmp_pid=wait3(statusp,options,0))!=pid) &&
//	(tmp_pid!=-1) && (tmp_pid!=0) && (pid!=-1)) ;
//  return tmp_pid;
// }
#endif
