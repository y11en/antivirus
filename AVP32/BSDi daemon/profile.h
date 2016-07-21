#ifndef __PROFILE_H
#define __PROFILE_H

#pragma pack (1)

// errors
#define PERR_MEM 0x01  // malloc() failed
#define PERR_OPN 0x02  // Load - open failed
#define PERR_DSK 0x03  // reading/writting/creating failed
#define PERR_COR 0x04  // Load - undet. value

extern char  DefProfile[0x50],LocFile[0x50],KeyFile[0x50],
      KeysPath[CCHMAXPATH],SetFile[0x50],BasePath[CCHMAXPATH];
extern char sInfectedFolder[CCHMAXPATH],sSuspiciousFolder[CCHMAXPATH];

typedef struct _profile_data
 {
  char *List,SubDirectories;
  char Memory,Sectors,Files,Packed,Archives,MailBases,MailPlain,UserMask[81];
  char ScanRemovable,ExcludeFiles,ExcludeMask[80],*ExcludeDir;
  char FileMask,InfectedAction,InfectedCopy,SuspiciousCopy;
  char InfectedFolder[CCHMAXPATH],SuspiciousFolder[CCHMAXPATH];
  char Warnings,CodeAnalyser,RedundantScan,ShowOK,ShowPack,Sound;
  char Symlinks,CrossFs;
  char Report,Append,ExtReport,ReportFileLimit,ReportFileName[CCHMAXPATH],ReportFileSize[8];
  char RepForEachDisk,LongStrings;
  char OtherMessages;
  char Color;
 } profile_data;

extern unsigned char prf_global_state; // Устанавливается при ошибке

extern profile_data prf_data;

// Установка опций по умолчанию
void set_default_options_();

// Сохранение PRF файла (return 0 - ok)
char save_profile_(char *name);

// Загрузка PRF файла (return 0 - ok)
char loadProfile(char *name,char *szRep);

ULONG load_ini(char *name);
ULONG SearchFile(char* sFileName,char* sDestName,char* sExt);
#endif