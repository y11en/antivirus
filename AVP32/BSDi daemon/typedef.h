#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#define far
#define FAR far

#define CCHMAXPATH 0x200

typedef void far *PVOID;
typedef char   CHAR;
typedef unsigned char  UCHAR;
typedef CHAR far *PCHAR;
typedef unsigned char  BYTE;
typedef unsigned short USHORT;
typedef long           LONG;
//typedef int           LONG;
typedef unsigned long  ULONG;
//typedef unsigned int  ULONG;
typedef ULONG APIRET;
typedef struct _QWORD
 {
  ULONG ulLo;
  ULONG ulHi;
 } QWORD;
 
#define MRESULT       ULONG
#define MPARAM        ULONG
#define VOID void

#define BOOL unsigned char

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif