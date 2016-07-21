#include "version.h"
#define N_PLAT_NLM
#ifdef NETWARE4
 #include <nwfile.h>
#else
 #include <fileengd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <ntypes.h>
#include <nwthread.h>
#include <nwconio.h>
#include <time.h>
#include <nwsnut.h>
#include <nwadv.h>
#include <fcntl.h>
#define cP ConsolePrintf

#include "viewer.mlh"
/* Globals */

typedef struct resource_list
{
   struct resource_list *next;
   int    screenHandle;
} ResourceList;

typedef void   (*PVF) ( void *);
int            gAlreadyLoaded = 0;
int            gMainThreadGroupID;
char           *gProgramName = "Text Viewer";
ResourceList   *gResList = (ResourceList *) NULL;
typedef struct LoadDefinitionStructure *LoadDefStructPtr;
typedef struct ScreenStruct *ScreenStructPtr;
int            maxStrLen=1024;

/* Prototypes */
int    ReadFromFile(int h,int offset1,int *offset2,char *buf,int size,int *rows);
int    SkipRowsBackward(char *buf,int size,int rows);
int    SkipRowsForward(char *buf,int size,int rows);
void   WriteFromBufToPortal(char *buf,int rows, PCB *mpPtr);
int    LogScreenHandle(int scrH);
void   CleanupResource(int which);
void   Cleanup(void);
extern LONG   _Prelude( );
LONG   MultipleLoadFilter(LoadDefStructPtr NLMHandle,
                        ScreenStructPtr    initErrorScreenID,
                        BYTE               *cmdLineP,
                        BYTE               *loadDirPath,
                        LONG               uninitDataLen,
                        LONG               NLMFileHandle,
                        LONG               cdecl (*readFunc)());

/*************************************************************************
 **  Main Routine
 */

void main(int argc, char *argv[])
{
   int      scrH, myThreadGroupID;
   int      ccode=0,i;
   char     buf[80],file[256];
   NUTInfo  *handlenut=NULL;
   LONG     allocTag=0;
   long     mainPortalNumber=-1;
   WORD     screenHeight, screenWidth;
   PCB             *mpPtr=NULL;
   LONG type=0;
   BYTE ch=0;
   int BufSize=1024*2;
   int RowNumber=0;
   int FileOffset1=0,FileOffset2=0,FileSize;
   int CurrPage=0,PageSize=22,x=0,y=0;
   int h=-1;                        // File handle
   int BytesRead=0;
   char *FileBuf=NULL;

   if(!gAlreadyLoaded)
   {
      gMainThreadGroupID = GetThreadGroupID();
      gAlreadyLoaded = 1;
      AtUnload(Cleanup);
   }
   else
   {
      myThreadGroupID = GetThreadGroupID();
      gAlreadyLoaded++;
   }

   if(gAlreadyLoaded==1)strcpy (buf,"Text Viewer");
   else sprintf(buf,"Text Viewer (%d)",gAlreadyLoaded-1);
   scrH = CreateScreen(buf, 0);
   if(!scrH)
   {
      ConsolePrintf("\nUnable to create screen...");
      goto NoScreenExit;
   }

   LogScreenHandle(scrH);
   SetCurrentScreen(scrH);
   if(argc < 2 && gAlreadyLoaded==1)
   	goto Exit;
   if(gAlreadyLoaded>1)  strcpy(file,(char *)argc);
   else	strcpy(file,argv[1]);

   if(gAlreadyLoaded==1)strcpy (buf,"Viewer Alloc Tag");
   else sprintf(buf,"Viewer Alloc Tag (%d)",gAlreadyLoaded-1);
   allocTag = AllocateResourceTag(GetNLMHandle(), buf, AllocSignature);
   if(!allocTag) goto Exit;

/* -----------------------  initialize NWSNUT  ---------------------------- */
   ccode = NWSInitializeNut(UTILITY, VERSION, SMALL_HEADER,
                         NUT_REVISION_LEVEL,
                         NULL,
                         0, scrH, allocTag, &handlenut);
  if(ccode) {
	         cP("NWSNut initialize error\n");
	         goto Exit;
            }
  DisplayScreen(scrH);    /*  activate the screen that we created */
  GetSizeOfScreen (&screenHeight, &screenWidth);

  i=3;
  while(i-->0)
  {
   FileBuf=malloc(BufSize);
   if(FileBuf==NULL) BufSize/=2;
   else break;
  }
  if(FileBuf==NULL) goto Exit;
//  h=FEsopen(file,O_RDONLY | O_TEXT, SH_COMPAT | SH_DENYNO,0,
//                 NO_RIGHTS_CHECK_ON_OPEN_BIT,0);

  h=sopen(file,O_RDONLY | O_TEXT, SH_COMPAT | SH_DENYNO,0);
  if(h<=0) goto Exit;
  FileSize=filelength(h);
  BytesRead=ReadFromFile(h,FileOffset1,&FileOffset2,FileBuf,BufSize-1,&RowNumber);
  if(CurrPage+PageSize-1>=RowNumber) CurrPage=RowNumber-1;
  else CurrPage=PageSize-1;
  if(CurrPage<0) CurrPage=0;
  y=CurrPage;
/* create main portal at the top of screen, no border, no header, size = 1 X screenWidth */
  mainPortalNumber = NWSCreatePortal(1L, 0L,
						screenHeight-1,screenWidth,
                        (RowNumber>PageSize)?RowNumber:PageSize, 256,
						SAVE,
                        NO_HEADER,
						VNORMAL, DOUBLE, 0, CURSOR_ON, VIRTUAL, handlenut);

  if(mainPortalNumber > MAXPORTALS) goto Exit;
  NWSGetPCB (&mpPtr, mainPortalNumber, handlenut);
  NWSSelectPortal (mainPortalNumber,handlenut);

//Display all data in virtual portal
  WriteFromBufToPortal(FileBuf,RowNumber,mpPtr);
  NWSPositionPortalCursor(y,x,mpPtr);
  NWSUpdatePortal(mpPtr);


  while(1)
  {
   if(NWSKeyStatus(handlenut))
   {
    NWSGetKey(&type,&ch,handlenut);
    if(type==K_ESCAPE && ch==0) break;
	if(ch!=0) continue;
	switch (type)
	{
	 case K_DOWN  : y++;
					if(y>=RowNumber)
					{
					 if(FileOffset2<FileSize)
					 {
					  FileOffset1=FileOffset2-SkipRowsBackward(FileBuf,FileOffset2-FileOffset1,PageSize);
					  BytesRead=ReadFromFile(h,FileOffset1,&FileOffset2,FileBuf,BufSize-1,&RowNumber);
					  WriteFromBufToPortal(FileBuf,RowNumber,mpPtr);
					  y=CurrPage=PageSize-1;
					  if(CurrPage>=RowNumber) y=CurrPage=RowNumber-1;
					  NWSPositionPortalCursor(0,x,mpPtr);
					  NWSUpdatePortal(mpPtr);
					 }
					 else {y=CurrPage=RowNumber-1;}
					}
		            break;
	 case K_PDOWN : if(CurrPage+PageSize-1>=RowNumber)
					{
					 if(FileOffset2<FileSize)
					 {
					  FileOffset1=FileOffset2-SkipRowsBackward(FileBuf,FileOffset2-FileOffset1,RowNumber-CurrPage);
					  BytesRead=ReadFromFile(h,FileOffset1,&FileOffset2,FileBuf,BufSize-1,&RowNumber);
					  WriteFromBufToPortal(FileBuf,RowNumber,mpPtr);
					  y=CurrPage=PageSize-1;
					  if(CurrPage>=RowNumber) y=CurrPage=RowNumber-1;
					  NWSPositionPortalCursor(0,x,mpPtr);
					  NWSUpdatePortal(mpPtr);
					 }
					 else {y=CurrPage=RowNumber-1;}
					}
					else y=CurrPage+=PageSize-1;
					break;
	 case K_UP    : y--;
					if(y<0)
					{
					 if(FileOffset1==0) {y=CurrPage=0;break;}
					 FileOffset2=FileOffset1+SkipRowsForward(FileBuf,BufSize-1,PageSize);
					 FileOffset1=FileOffset2-BufSize+1;
					 if(FileOffset1<0) FileOffset1=0;
					 BytesRead=ReadFromFile(h,FileOffset1,&FileOffset2,FileBuf,FileOffset2-FileOffset1-1,&RowNumber);
					 if(FileOffset1)
					 {
					// Find string begin and correct FileOffset1
					  char *ptr=FileBuf+strlen(FileBuf)+1;
					  FileOffset1+=ptr-FileBuf;
					  memmove(FileBuf,ptr,FileBuf+BufSize-ptr);
					  RowNumber--;
					 }
					 WriteFromBufToPortal(FileBuf,RowNumber,mpPtr);
					 NWSPositionPortalCursor(RowNumber-1,x,mpPtr);
					 NWSUpdatePortal(mpPtr);
				     y=CurrPage=(RowNumber-1)-(PageSize-1);
					}
					break;
	 case K_PUP   : if(CurrPage-(PageSize-1)<0)
					{
					 if(FileOffset1==0) {y=CurrPage=0;break;}
					 FileOffset2=FileOffset1+SkipRowsForward(FileBuf,BufSize-1,CurrPage+1);
					 FileOffset1=FileOffset2-BufSize+1;
					 if(FileOffset1<0) FileOffset1=0;
					 BytesRead=ReadFromFile(h,FileOffset1,&FileOffset2,FileBuf,FileOffset2-FileOffset1-1,&RowNumber);
					 if(FileOffset1)
					 {
					// Find string begin and correct FileOffset1
					  char *ptr=FileBuf+strlen(FileBuf)+1;
					  FileOffset1+=ptr-FileBuf;
					  memmove(FileBuf,ptr,FileBuf+BufSize-ptr);
					  RowNumber--;
					 }
					 WriteFromBufToPortal(FileBuf,RowNumber,mpPtr);
					 NWSPositionPortalCursor(RowNumber-1,x,mpPtr);
					 NWSUpdatePortal(mpPtr);
				     y=CurrPage=(RowNumber-1)-(PageSize-1);
					}
					else
					 y=CurrPage-=PageSize-1;
					break;
	 case K_RIGHT : x++;
					if(x>maxStrLen) x=maxStrLen-1;
					break;
	 case K_LEFT  : x--;
					if(x<0) x=0;
					break;
  	 case K_SLEFT : x=0;
					break;
  	 case K_SRIGHT: x=255;
					break;
	 case K_FRIGHT: x+=10;
					if(x>maxStrLen) x=maxStrLen-1;
					break;
	 case K_FLEFT : x-=10;
					if(x<0) x=0;
					break;

	 default      : break;
	}
	NWSPositionPortalCursor(y,x,mpPtr);
	NWSUpdatePortal (mpPtr);
   }
   delay(5);
  }

Exit :
   if(h>0) close(h);
   if(mainPortalNumber >= 0L) NWSDestroyPortal (mainPortalNumber, handlenut);
   if(handlenut) NWSRestoreNut(handlenut);
   CleanupResource(scrH);
   if(FileBuf) free(FileBuf);

NoScreenExit :
   ExitThread(EXIT_THREAD, 0);
}

LONG StartUp(LoadDefStructPtr   NLMHandle,
                        ScreenStructPtr    initErrorScreenID,
                        BYTE               *cmdLineP,
                        BYTE               *loadDirPath,
                        LONG               uninitDataLen,
                        LONG               NLMFileHandle,
                        LONG               cdecl (*readFunc)())
{
   int   myThreadGroupID;
   char buf[1024];
   if(!gAlreadyLoaded)            /*  first time through  */
      return _Prelude(NLMHandle, initErrorScreenID, cmdLineP, loadDirPath,
                      uninitDataLen, NLMFileHandle, readFunc);

   /*  subsequent times through...  */
   myThreadGroupID = SetThreadGroupID(gMainThreadGroupID);
   strcpy(buf,cmdLineP);
   BeginThreadGroup((PVF) main, NULL, NULL, buf	);
   SetThreadGroupID(myThreadGroupID);

   return 0L;
}

int LogScreenHandle(int scrH)
{
   ResourceList   *r = gResList;

   if(!r)
   {
      r = gResList = (ResourceList *) malloc(sizeof(ResourceList));
      if (!r)
         return -1;
   }
   else
   {
      while (r->next)      /*  (find end of list...)  */
         r = r->next;

      r->next = (ResourceList *) malloc(sizeof(ResourceList));

      r = r->next;
      if (!r)
         return -1;
   }

   r->next         = (ResourceList *) NULL;
   r->screenHandle = scrH;

   return 0;
}

void CleanupResource(int which)
{
   register ResourceList   *next,
                           *prev = (ResourceList *) NULL,
                           *r = gResList;
   while(r)
   {
      next = r->next;

      if (r->screenHandle == which)
      {
         DestroyScreen(r->screenHandle);

         if (prev)         /*  patch holes left...  */
            prev->next = next;
         else
            gResList = next;

         free(r);
         break;
      }

      prev = r;
      r    = next;
   }
}

void Cleanup( void )
{
   register ResourceList   *next,
                           *r = gResList;

   while(r)
   {
      next = r->next;
      DestroyScreen(r->screenHandle);
      free(r);
      r = next;
   }
}


// size = FileBufSize-1;
int ReadFromFile(int h,int offset1,int *offset2,char *buf,int size,int *rows)
{
 int BytesRead=0;
 char *ptr,*p;

 lseek(h,offset1,0);
 BytesRead=read(h,buf,size);
 if(BytesRead<=0) return 0;
 ptr=buf+size-1;
 if(eof(h)!=1)
 {
  // Find end of string - \xd\xa
  while(*ptr!=0xa) {if(ptr==buf) break;ptr--;}
  *offset2=offset1+(ptr-buf)+1;
 } // eof()
 else {ptr=buf+BytesRead-1;*(buf+BytesRead)=0x0;*offset2=offset1+BytesRead;}
 p=buf;
 *rows=0;
 while(p<=ptr)
 {
  if(*p==0xd)
  {
   (*rows)++;
	*p=' ';
  }
  else
   if(*p==0xa) *p=0x0;
  p++;
 }


 return  BytesRead;
}

void WriteFromBufToPortal(char *buf,int rows, PCB *mpPtr)
{
  char *ptr=buf;
  int i,len;

  NWSClearPortal(mpPtr);
  for(i=0;i<rows;i++)
  {
   len=strlen(ptr);
   NWSDisplayTextInPortal (i,0,ptr,0,mpPtr);
   ptr+=len+1;
  }
}

int SkipRowsBackward(char *buf,int size,int rows)
{
 char *ptr=buf+size;
 int offset=0;
 int i=rows;

 while(ptr>=buf)
 {
  if(*ptr==0x0) i--;
  if(i<=0) break;
  else ptr--;
 }

 offset=buf+size-ptr-1;
 return offset;
}

int SkipRowsForward(char *buf,int size,int rows)
{
 char *ptr=buf;
 int offset=0;
 int i=rows;

 while(ptr<buf+size)
 {
  if(*ptr==0x0) i--;
  if(i<=0) break;
  else ptr++;
 }

 offset=ptr-buf+1;
 return offset;
}
