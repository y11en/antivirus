#include <dos.h>
extern MRESULT GlobalRes;

#define STATLEN 30
typedef struct
 {
  ULONG cek;
  ULONG cekForEach;
  char  Sectors[STATLEN];
  ULONG sect;
  ULONG sectForEach;
  char  Files[STATLEN];
  ULONG fil;
  ULONG filForEach;
  char  Folder[STATLEN];
  ULONG fold;
  ULONG foldForEach;
  char  Archives[STATLEN];
  ULONG arch;
  ULONG archForEach;
  char  Packed[STATLEN];
  ULONG pack;
  ULONG packForEach;
  char  countSize[STATLEN];
  ULONG CountSize;
  ULONG CountSizeForEach;
  char  scanTime[STATLEN];
  dostime_t st;
  dostime_t stForEach;
  char  knownVir[STATLEN];
  ULONG kv;
  ULONG kvForEach;
  char  virBodies[STATLEN];
  ULONG vb;
  ULONG vbForEach;
  char  Disinfect[STATLEN];
  ULONG df;
  ULONG dfForEach;
  char  Deleted[STATLEN];
  ULONG deleted;
  ULONG deletedForEach;
  char  Warning[STATLEN];
  ULONG warn;
  ULONG warnForEach;
  char  Suspis[STATLEN];
  ULONG susp;
  ULONG suspForEach;
  char  Corript[STATLEN];
  ULONG cor;
  ULONG corForEach;
  char  IOError[STATLEN];
  ULONG err;
  ULONG errForEach;
 } _workStat;
extern _workStat workStat;

extern BOOL  flgScan;
void  iniWorkStat(void);
char* LoadString(ULONG strId,char *defaultStr);

extern UCHAR attr;

int _sprintf(char * buf, const char *fmt, ...);
int _printf(char const *fmt, ...);
int _puts(char *str);
UCHAR GetCharAttr(void);
#ifdef __cplusplus
   extern "C" {
#endif
ULONG StrPos(void);
void SetCharAttr(char attr,char cumb);
#ifdef __cplusplus
   }
#endif
BOOL PutStrAttr(char* strWrt,UCHAR attr);

