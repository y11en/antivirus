#ifndef __DISK_H
#define __DISK_H

typedef unsigned long       DWORD;
typedef struct _SELECT_SEGM
 {
  USHORT  segment;
  union
   {
    char far* str;
    struct
     {
      ULONG  offset;
      USHORT selector;
     };
   };
 } SelectSegm;

extern SelectSegm  SS;
#define SIZEALLOC 0x8000 //0xffff

typedef struct rminfo
 {
    long EDI;
    long ESI;
    long EBP;
    long reservedBySystem;
    long EBX;
    long EDX;
    long ECX;
    long EAX;
    short flags;
    short ES,DS,FS,GS,IP,CS,SP,SS;
 } RMI;

#ifdef __cplusplus
   extern "C" {
#endif
BOOL SimulateRMInt(UCHAR Int,RMI far* rmi);
BOOL pmAllocMem(USHORT Size,USHORT *selector,USHORT *segment);
BOOL pmResizeMem(USHORT NewSize,USHORT selector);
#ifdef __cplusplus
   }
#endif
BOOL pmFreeMem(short selector);
#pragma aux pmFreeMem=  \
  "mov   ax,0101h"      \
  "int   031h"          \
  "jc    FM_Err  "      \
  "mov   eax,1   "      \
  "jmp   short FM_Done" \
"FM_Err:         "      \
  "mov   eax,0   "      \
"FM_Done:        "      \
  parm   [edx]          ;
//  modify [eax]         ;

BOOL SetCursorOff(BOOL fl);
void GetCurrentDir(CHAR* szPath);

#define OPEN_AVP  0x0008  /* ---- ---- ---- 1--- */
int copyfile(char *oldFile,char *newFile);

BOOL ExistDisk(char device);
// Возвращает тип устройства:
//  00h - no such drive
//  01h - floppy without change-line support
//  02h - floppy with change-line support
//  03h - hard disk
LONG DiskType(CHAR *dst);

// Возвращает: 1 - disk is remote, 0 - disk is not remote
//unsigned char CheckForRemote(unsigned char num);

//Число дисковых накопителей
//биты 6-7 в байте статуса - количество НГМД(если бит 1 = 1)
UCHAR GetNumberFloppyDisks(void);
#pragma aux GetNumberFloppyDisks= \
  "int  11h        "      \
  "test al,0       "      \
  "jne  No_Drives  "      \
  "and  al,1100000b"      \
  "mov  cl,5       "      \
  "shr  al,cl      "      \
  "inc  al         "      \
  "jmp  GNF_Done   "      \
"No_Drives:        "      \
  "mov  ax,0       "      \
"GNF_Done:         "      \
  modify [cl]             ;
//  value  [ax];

#define DISK_FDD_WITHOUT   1              // floppy without change-line support
#define DISK_FDD_WITH      2              // floppy with change-line support
#define DISK_HDD           8              // hard disk
#define DISK_REMOTE       16              // remote disk
#define DISK_SELECTED    128              // disk selected by user

//extern unsigned char CountHDD,            // количество физических HDD в системе
//                     CountDisk,           // количество реальных логических дисков
//                     CountNetWork,        // количество сетевых логических дисков
//                     Disk[26],            // Disk... (хранит информацию о дисках)
//                     VolumeLabel[26][12]; // метки томов не floppy дисков
                                          // VolumeLabel[2]  - label c:
                                          // VolumeLabel[3]  - label d:
                                          // ...
                                          // VolumeLabel[25] - label z:

// Получить информацию о всех существующих в системе логических дисках:
//  - количество физических HDD в системе
//  - количество всех дисков
//  - количество сетевых логических дисков
//  - что из себя представляет каждый диск в отдельности
//  - метки дисков
//void  InitDisks(void);

// Получить текущий логический диск (00h='A:',01h='B:',02h='С:',etc)
// Возвращает: номер текущего логического диска
//unsigned char GetCurrentDisk();

// Установить текущий логический диск (00h='A:',01h='B:',02h='С:',etc)
// Возвращает: номер потенциально возможного логического диска
//unsigned char SetCurrentDisk(unsigned char num);

// Узнать реальный номер диска по номеру в списке
//unsigned char GetRealDiskNum(unsigned char num);

// Возвpщает: 1 - выделены все локальные HDD, иначе 0
//unsigned char IsSelectAllLocHDD();

// Возвpащает: 1 - выделены все сетевые диски, иначе 0
//unsigned char IsSelectAllNetWork();

// Возвращает: 1 - выделен хотя бы один диск, иначе 0
//unsigned char IsDiskSelect();

#endif