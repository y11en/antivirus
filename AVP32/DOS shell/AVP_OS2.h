#include <dos.h>
extern MRESULT GlobalRes;

#define STATLEN 30
typedef struct
 {
  char Sectors[STATLEN];
  ULONG sect;
  char Files[STATLEN];
  ULONG fil;
  char Folder[STATLEN];
  ULONG fold;
  char Archives[STATLEN];
  ULONG arch;
  char Packed[STATLEN];
  ULONG pack;
  char countSize[STATLEN];
  ULONG CountSize;
  char scanTime[STATLEN];
  dostime_t st;
  char knownVir[STATLEN];
  ULONG kv;
  char virBodies[STATLEN];
  ULONG vb;
  char Disinfect[STATLEN];
  ULONG df;
  char Deleted[STATLEN];
  ULONG deleted;
  char Warning[STATLEN];
  ULONG warn;
  char Suspis[STATLEN];
  ULONG susp;
  char Corript[STATLEN];
  ULONG cor;
  char IOError[STATLEN];
  ULONG err;
 } _workStat;
extern _workStat workStat;

extern BOOL  flgScan;
void  iniWorkStat(void);
void  setPrf(void);
char* LoadString(ULONG strId,char *defaultStr);
char* LoadMenuString(ULONG strId,char *defaultStr);

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

