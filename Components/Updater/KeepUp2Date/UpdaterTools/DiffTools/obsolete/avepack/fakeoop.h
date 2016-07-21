#ifndef _FAKE_OOP_H_
#define _FAKE_OOP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE  unsigned char
#define WORD  unsigned short
#define DWORD unsigned long
#define BOOL  int
#define UINT  unsigned int

/* Извращение №1: имитация класса CMemFile при помощи С. */
typedef struct _SMemFile
{
  BYTE  *ptr;
  DWORD dwSize;
  DWORD dwPos;
  DWORD dwRsrvSize;
} SMemFile;

void  MemFileReset      (SMemFile *mf);
void  MemFileDelete     (SMemFile *mf);
DWORD MemFileSetSize    (SMemFile *mf, DWORD dwSize);
DWORD MemFileWrite      (SMemFile *mf, void *pdata, DWORD nCount);
DWORD MemFileRead       (SMemFile *mf, void *pdata, DWORD nCount);
DWORD MemFileGetLength  (SMemFile *mf);
void  MemFileSeekToBegin(SMemFile *mf);
void  MemFileSeekToEnd  (SMemFile *mf);
DWORD MemFileGetPosition(SMemFile *mf);
void *MemFileDetach     (SMemFile *mf);
void  MemFileAttach     (SMemFile *mf, void *pBuffer, DWORD nCount);
DWORD MemFileSeek       (SMemFile *mf, long lOffset, int nOrigin);
/*--------------------------end of MemFile-------------------------*/

/* Извращение №2: имитация класса CPtrArray при помощи С. */
typedef struct _SPtrArray
{
  void **pp;
  DWORD dwSize;
  DWORD dwRsrvSize;
} SPtrArray;

void  PtrArrayReset     (SPtrArray *pa);
int   PtrArrayAdd       (SPtrArray *pa, void *ptr);
void *PtrArrayGetAt     (SPtrArray *pa, int index);
void  PtrArraySetAt     (SPtrArray *pa, int index, void *ptr);
void  PtrArrayFreeAllPtr(SPtrArray *pa);
int   PtrArrayGetSize   (SPtrArray *pa);
int   PtrArrayRemoveAt  (SPtrArray *pa, int index);
int   PtrArrayFreeAt    (SPtrArray *pa, int index);

/*--------------------------end of PtrArray-------------------------*/

#ifdef __cplusplus
}
#endif

#endif /*_FAKE_OOP_H_*/