#ifndef __KERNEL_H
#define __KERNEL_H
#include "../TYPEDEF.H"
#include "../ScanAPI/Scanobj.h"

#pragma pack (1)

#define SECTORCHECKLIMIT 5
typedef struct _SectorCheck
 {
  char sDev[20];
 } SectorCheck;

extern SectorCheck SectChk[SECTORCHECKLIMIT];
extern ULONG ulSectChk;

void initWorcStat(void)
;
void incWorkStat(ULONG sw);
void iniWorkStatForEach(void)
;

extern char scan_goes; // продолжать ли процесс сканирования
                       // 1 - продолжать 0 - не продолжать

// INC CallBack
#define INCL_SECTORS       1 // number of scanned sector objects
#define INCL_FILES         2 // total number of scanned files
#define INCL_FOLDER        3 // number of scanned directories
//#define INCL_DISKS       4 // number of scanned disks
#define INCL_ARCHIVES      5 // number of archives
#define INCL_PACKED        6 // number of packed files
#define INCL_KNOWNVIR      7 // number of
#define INCL_VIRBODIES     8 // number of
#define INCL_DISINFECT     9 // number of disinfected files
#define INCL_DELETED       10// number of deleted files
#define INCL_WARNING       11// number of warnings
#define INCL_SUSPICION     12// number of suspicious files
#define INCL_CORRUPTED     13// number of
#define INCL_IOERRORS      14// number of input/output errors

extern _AVPScanObject ScanObject;

typedef struct _DisinfectDlg
 {
  //enum { IDD = IDD_DISINFECT };
  BOOL    m_All;
  BOOL    m_CopyFolder;
  CHAR    m_sObject[0x200];
  CHAR    m_sVirus[0x200];
  int     m_Action;
 } DisinfectDlg;

typedef struct _MessageDlg
 {
  CHAR    *Text;
  CHAR    *Title;
  ULONG   Param;
 } MessageDlg;

typedef struct _surBase
 {
  USHORT LastUpdateYear;
  BYTE   LastUpdateMonth;
  BYTE   LastUpdateDay;
  ULONG  LastUpdateTime;
  ULONG  VirCount;
  ULONG  RecCount;
  ULONG  RecCountArray[8];
 } _SurBase;

extern _SurBase SurBase;
extern BOOL SelfScan;
MRESULT MySendMsg(ULONG ulMsgId,MPARAM mp1,MPARAM mp2);
void StatusLine(char* Line);
BOOL load_avpdll(void);
BOOL load_avpbase(char* name);                     // Load a-v database
unsigned short UnloadBase(void);                   // Unload a-v database
BOOL ScanFile(char *fullFileName,unsigned long fileSize,unsigned long mFlags);
ULONG GetFlags(void);
UINT ScanAllPaths();
LONG avp_mess(DWORD mp1,DWORD mp2);

#endif