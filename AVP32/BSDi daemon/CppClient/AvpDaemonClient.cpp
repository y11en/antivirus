// Program to test daemon logging.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>        // struct sockaddr_un 
#include <sys/file.h>
#include <time.h>
#include <stdarg.h>
#include <paths.h>
#include <errno.h>

//#define DEBUG_MESSAGES

#ifdef DEBUG_MESSAGES
 #define dprintf(args) printf args
#else
 #define dprintf(args) 
#endif


// ... Cfg pathnames 
#define AVP_NODE_DIR      "/var/run"
#define AVP_NODE_DIR_MODE 0777
#define AVP_NODE_PID      AVP_NODE_DIR "/AvpPid"
#define AVP_NODE_LOG      AVP_NODE_DIR "/AvpLog"
#define AVP_NODE_CTL      AVP_NODE_DIR "/AvpCtl\0\0\0"
char *NodePid=AVP_NODE_PID;
char *NodeCtl=AVP_NODE_CTL;

static int  AvpFile=-1;		// fd for log 
static int  connected;		 // have done connect 

struct sockaddr_un AvpTestAddr;	// AF_UNIX address of local logger 
// start test
int AvpTestStart()
 {
  if(AvpFile==-1)
   {
    bzero((char *)&AvpTestAddr,sizeof(AvpTestAddr));
    AvpTestAddr.sun_family=AF_UNIX;
    strcpy(AvpTestAddr.sun_path,NodeCtl);
    if((AvpFile=socket(AF_UNIX,SOCK_STREAM,0))<0)
      printf("create socket error: socket() not created\n");
   }

  if(AvpFile!=-1 && !connected &&
    connect(AvpFile,(struct sockaddr *)(&AvpTestAddr),sizeof(AvpTestAddr.sun_family)+strlen(NodeCtl)+1)>=0)
   connected=1;
  if(connected==1)
   {
    dprintf(("AvpTestStart connected to AvpDaemon\n")); 
    return 0;
   }    
  printf("AvpTestStart cannot connected to AvpDaemon\n"); 
  return -1;
 }

// close the testing
void AvpTestClose()
 {
  (void)close(AvpFile);
  AvpFile=-1;
  connected=0;
  dprintf(("AvpTestClose. connected=%d\n",connected)); 
 }

int vAvpTest(int pri,const char *fmt,va_list ap)
 {
  if(AvpFile<0 || !connected) AvpTestStart();

  register int cnt;
  register char *p;
  int fd,saved_errno=0;
  char tbuf[2048], fmt_cpy[1024];
  time_t now;

  // build the message 
  (void)time(&now);
  (void)sprintf(tbuf,"<%d>%.15s:",pri,ctime(&now)+4);
  for(p=tbuf;*p;++p) ;

  // substitute error message for %m 
   {
    register char ch, *t1, *t2;

    for(t1= fmt_cpy;(ch=*fmt)!='\0' && t1<fmt_cpy+sizeof(fmt_cpy);++fmt)
     if(ch=='%'&& fmt[1]=='m')
      {
       ++fmt;
       for(t2=strerror(saved_errno);(*t1 = *t2++);++t1);
      }
     else
      *t1++ =ch;
    *t1='\0';
   }
  (void)vsprintf(p,fmt_cpy,ap);

  cnt=strlen(tbuf);

  // output the message to the local logger 
  dprintf(("write string (%s) to AvpDaemon\n",tbuf));
  if(write(AvpFile,tbuf,cnt+1)>=0)
   {
    int Rez; 
    if((Rez=read(AvpFile,tbuf,0x200))==-1)
     {
      printf("Cannot reaf from AvpDaemon!\n");
      return -1;
     }
    else 
     {
      tbuf[Rez]=0;
      Rez=atoi(tbuf);
      dprintf("Test rezult: %s(%d)\n",tbuf,Rez);
      switch (Rez)
       {
	case 7:	
          printf("File AvpLinux is corrupted

\n");
          break;
	case 5:	
          printf("All viruses deleted\n");
          break;
	case 4:	
          printf("Known viruses were detected
\n");
          break;
	case 3:	
          printf("Suspicious objects were found
\n");
          break;
	case 1:	
          printf("Virus scan was not complete
\n");
          break;
	case 0:	
          printf("No viruses were found\n");
          break;
	default:	
          printf("Error!(test result %d)\n",Rez);
          break;
       }
      return Rez;
     }
    return -1;
   }

  // output the message to the console; don't worry about
  // blocking, if console blocks everything will.
  if((fd=open(_PATH_CONSOLE,O_WRONLY|O_NOCTTY,0))<0) return -1;
  (void)strcat(tbuf,"\r\n");
  cnt+=2;
  p=index(tbuf,'>')+1;
  (void)write(fd,p,cnt-(p-tbuf));
  (void)close(fd);
  return -1;
 }

int AvpTest(int pri,const char *fmt, ...)
 {
  int rez=-1;
  va_list ap;

  va_start(ap, fmt);
  rez=vAvpTest(pri,fmt,ap);
  va_end(ap);
  return rez;
 }

int main(int argc,char *argv[])
 {
  int rez=-1;

  if(AvpTestStart()==0)
   if(argc>1)
    {
     if( (*argv[1]=='-') && (*(argv[1]+1)=='\0') )
      {
       auto char *nl,bufr[512];
       auto int tested=0;
       while (!feof(stdin))
	if(fgets(bufr, sizeof(bufr), stdin) !=(char *) 0 )
	 {
	  if( (nl=strrchr(bufr,'\n'))!=(char *)0) *nl='\0';
  	  rez=AvpTest(0,bufr);
	  tested+=strlen(bufr);
	  if(tested>1024)
	   {
	    sleep(1);
	    tested=0;
	   }
	 }
      }
     else
      while(argc-- >1) rez=AvpTest(0,argv++[1]);
    }
   else
    rez=AvpTest(0,"Not object to test.");

  AvpTestClose();

  return(rez);
 }
