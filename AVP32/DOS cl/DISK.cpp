#include <os2.h>

#include "memalloc.h"
#include <string.h>
#include <ctype.h>
#include <share.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include "disk.h"
#include "avp_rc.h"

SelectSegm  SS;

DWORD CursorLine=0;
BOOL SetCursorOff(BOOL fl)
 {
  union REGPACK regs;
  RMI   rmi;

  if((CursorLine)==0)
   {// GET CURSOR POSITION AND SIZE
    memset(&rmi,0,sizeof(RMI));
    rmi.EAX = 0x0300;
    rmi.EBX = 0;    // BH = page number
                    // 0-3 in modes 2&3
                    // 0-7 in modes 0&1
                    // 0 in graphics modes
    memset(&regs,0,sizeof(REGPACK));
    regs.w.ax =0x0300;
    regs.h.bl =0x10;
    regs.h.bh =0;
    regs.w.cx =0;
    regs.w.es =FP_SEG(&rmi);
    regs.x.edi=FP_OFF(&rmi);
    intr(0x31,&regs);
//    if(((regs.w.flags&0x1)==0)&&((rmi.flags&0x1)==0))
//     {//Return: AX = 0000h (Phoenix BIOS)
      CursorLine=rmi.ECX;
//     }
   }
  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x0100;
  rmi.ECX = (fl==TRUE)? (CursorLine):(0x2000+CursorLine);
       // CH = cursor start and options (see #0012)
       // CL = bottom scan line containing cursor (bits 0-4)
  memset(&regs,0,sizeof(REGPACK));
  regs.w.ax =0x0300;
  regs.h.bl =0x10;
  regs.h.bh =0;
  regs.w.cx =0;
  regs.w.es =FP_SEG(&rmi);
  regs.x.edi=FP_OFF(&rmi);
  intr(0x31,&regs);

  return TRUE;
 }

long memsize(long size)
 {
  char *buff;
  long ret;
  int end_key=0;
  ret=10000;
  while(end_key==0)
   {
    buff=(char*)Malloc(ret);
    if(buff==NULL)
     { return (ret-10000);}
    Free(buff);
    if((ret>=size) || (ret==60000)) return ret;
    ret=ret+10000;
   };
  return 0;
 };

int copyfile(char *oldFile,char *newFile)
 {
  int  fin,fout;
  long len,len1;
  char *buff;
  int  err;

  if((fin =sopen(oldFile,O_BINARY|O_RDONLY,SH_DENYNO|SH_COMPAT,S_IWRITE))==-1 ) return 1;
  if((fout=sopen(newFile,O_BINARY|O_WRONLY|O_CREAT,SH_DENYNO|SH_COMPAT,S_IWRITE))==-1) return 2;

  len=lseek(fin,0,SEEK_END);
  lseek(fin,0,SEEK_SET);
  len1=memsize(len);
  if((buff=(char*)Malloc(len1))!=NULL)  //else return 3;
   {
    if(len1<len)
     {
      long n,i,k;
      n=len/len1;
      k=0;
      for(i=1;i<=n;i++)
       {
        if((err=read(fin,buff,len1))==-1)   goto ex;//return 4;
        if((err=write(fout,buff,len1))==-1) goto ex;//return 5;
        k=k+len1;
       }
      if(len>k)
       {
        if((err=read(fin,buff,len-k))==-1)   goto ex;//return 4;
        if((err=write(fout,buff,len-k))==-1) goto ex;//return 5;
       }
     }
    else
     {
      if((err=read(fin,buff,len))==-1)   goto ex;//return 4;
      if((err=write(fout,buff,len))==-1) goto ex;//return 5;
     }
   }
ex:
  Free(buff);
  close(fin);
  close(fout);
  return 0;
 };

#define DEVTYPE_35HD    DEVTYPE_UNKNOWN
#define DEVTYPE_OPTICAL 8
#define DEVTYPE_35ED    9
//#pragma pack(1)
typedef struct
 {
  BYTE bCmd;
  BYTE bDrive;
 } CMDPARM;
/*
typedef struct
 {
  BIOSPARAMETERBLOCK bspblk;
  USHORT             cCyl;
  BYTE               bDevType;
  USHORT             DevAttr;
 } GETPARMDATA;
//#pragma pack ()
*/
BOOL ExistDisk(char device)
 {
  unsigned ulCurDisk,totaldrives;
  unsigned drive;

  _dos_getdrive(&ulCurDisk);
  drive=toupper(device)-'A'+1;
  _dos_setdrive(drive,&totaldrives);
  if(_getdrive()==drive)
   {
    _dos_setdrive(ulCurDisk,&totaldrives);
    return 0;
   }
  else
   {
    _dos_setdrive(ulCurDisk,&totaldrives);
    return 1;
   }
 }

// Возвращает тип устройства:
//  00h - no such drive
//  01h - floppy without change-line support
//  02h - floppy with change-line support
//  03h - hard disk
BYTE GetDriveType(BYTE num)
 {
  union REGPACK regs;
  memset(&regs,0,sizeof(REGPACK));
  regs.h.ah=0x15;
  regs.h.dl=num;
  intr(0x13,&regs);
  if((regs.w.flags&0x1)==0) return regs.h.ah;
  return 0;
 }

// Возвращает: 1 - disk is remote, 0 - disk is not remote
BYTE CheckForRemote(BYTE num)       //int 21
 {
  union REGPACK regs;
  memset(&regs,0,sizeof(REGPACK));
  regs.w.ax=0x4409;
  regs.h.bl=num;
  regs.h.bh=0;
  regs.h.bl++;
  intr(0x21,&regs);
  if((regs.w.flags&0x1)==0)
   {
    regs.w.dx=(unsigned short)(regs.w.dx & 0x1000);
    if(regs.h.dh!=0)
     if(regs.h.dh==0x10) return 1;
   }
  return 0;
 }

#define NO_DRIVE 0;
LONG DiskType(CHAR *dst)
 {
  ULONG type=NO_DRIVE;
  SHORT disk;
  unsigned ulCurDisk,totaldrives;
  //UCHAR j,len,*vol_ptr;
  //struct ffblk ffblk;

  //CountHDD=peekb(0x0040,0x0075);
//  type
//IDI_FLOPPY_12
//IDI_FLOPPY_144;
//IDI_LOCAL1;
//IDI_CD
//IDI_NET_ON
  ulCurDisk=_getdrive();
  if((dst[1]==':')&&(strlen(dst)<=3)&&((dst[2]==0)||(dst[2]=='\\')))
   {
    disk=(CHAR)(toupper(dst[0])-'A');
    if((disk==0)||(disk==1)) //a или б
     {
      if(GetDriveType((BYTE)disk))
       //if(CheckForRemote((BYTE)disk))
         type=IDI_FLOPPY_144;//Disk[i]|=DISK_REMOTE;
     }
    else
     {
      _dos_setdrive(disk+1,&totaldrives);
      if(_getdrive()==(disk+1))
       {
        type=IDI_LOCAL1;//DISK_HDD;
        if(CheckForRemote((BYTE)disk))
         type=IDI_NET_ON;//DISK_REMOTE;
       }
      //if(Disk[i]&DISK_HDD&&!findfirst("*.*",&ffblk,FA_LABEL))
      // {
      //  vol_ptr=VolumeLabel[i-2];
      //  len=strlen(ffblk.ff_name);
      //  for(j=0;j<=len;j++)
      //  if(*(ffblk.ff_name+j)!='.')
      //   {
      //    *vol_ptr=*(ffblk.ff_name+j);
      //    vol_ptr++;
      //   }
      // }
      //else *VolumeLabel[i-2]=0;
     }
   }
  else type=IDI_FOLD_CLOSE;

  _dos_setdrive(ulCurDisk,&totaldrives);
  return type;
 }

