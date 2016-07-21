#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fakeoop.h"

/* Извращение №1: имитация класса CMemFile при помощи С. */
#define MEM_FILE_GRANULARITY 0x1000

void MemFileReset(SMemFile *mf)
{
  mf->ptr    = 0;
  mf->dwSize = 0;
  mf->dwPos  = 0;
  mf->dwRsrvSize = 0;
}

void MemFileAttach(SMemFile *mf, void *pBuffer, DWORD nCount)
{
  mf->ptr = pBuffer;
  mf->dwSize = nCount;
  mf->dwRsrvSize = nCount;
  mf->dwPos = 0;
}

void * MemFileDetach(SMemFile *mf)
{
  void *pRet = (void *)mf->ptr;
  MemFileReset(mf);
  return pRet;
}

void MemFileDelete(SMemFile *mf)
{
  if (mf->ptr) free(mf->ptr);
  MemFileReset(mf);
}

DWORD MemFileSetSize(SMemFile *mf, DWORD dwSize)
{
  DWORD dwNewAllocSize = 0;
  if (dwSize == 0)
  {
    MemFileDelete(mf);
    return 0;
  }

  if (dwSize == mf->dwSize)
    return dwSize;

  if (dwSize > mf->dwRsrvSize || ((dwSize < mf->dwRsrvSize) && (mf->dwRsrvSize-dwSize>=MEM_FILE_GRANULARITY)))
  { // reallocation needed
    BYTE *pNewPtr;
    dwNewAllocSize = (((dwSize-1)/MEM_FILE_GRANULARITY)+1)*MEM_FILE_GRANULARITY;
    pNewPtr = (BYTE *)realloc( mf->ptr, dwNewAllocSize );
    if (pNewPtr==0)
      return mf->dwSize;

    mf->ptr = pNewPtr;
    mf->dwRsrvSize = dwNewAllocSize;
  }
  
  mf->dwSize = dwSize;

  if (mf->dwPos > mf->dwSize)
    mf->dwPos = mf->dwSize;
  return dwSize;
}

DWORD MemFileWrite(SMemFile *mf, void *pdata, DWORD nCount)
{
  if (nCount==0)
    return 0;

  if (mf->dwPos + nCount > mf->dwSize)
  {
    DWORD dwNewSize = mf->dwPos + nCount;
    DWORD dwActSize = MemFileSetSize(mf, dwNewSize);
    if (dwActSize != dwNewSize)
      return 0;
  }

  memcpy(mf->ptr + mf->dwPos, pdata, nCount);
  mf->dwPos += nCount;
  return nCount;
}

DWORD MemFileRead(SMemFile *mf, void *pdata, DWORD nCount)
{
  DWORD dwBytesLeft = mf->dwSize - mf->dwPos;
  if (dwBytesLeft == 0)
    return 0;
  if (nCount == 0)
    return 0;
  if (dwBytesLeft<nCount)
    nCount = dwBytesLeft;

  memcpy(pdata, mf->ptr + mf->dwPos, nCount);
  mf->dwPos += nCount;
  return nCount;
}

DWORD MemFileGetLength(SMemFile *mf)
{
  return mf->dwSize;
}

void MemFileSeekToBegin(SMemFile *mf)
{
  mf->dwPos = 0;
}

void MemFileSeekToEnd(SMemFile *mf)
{
  mf->dwPos = mf->dwSize;
}

DWORD MemFileGetPosition(SMemFile *mf)
{
  return mf->dwPos;
}

DWORD MemFileSeek(SMemFile *mf, long lOffset, int nOrigin)
{
  switch(nOrigin)
  {
  case SEEK_SET:
    if (lOffset<0) lOffset = 0;
    mf->dwPos = lOffset;
    break;
  case SEEK_CUR:
    if ((long)(mf->dwPos)+lOffset <= 0)
    {
      mf->dwPos = 0;
      return 0;
    }
    mf->dwPos += lOffset;
    break;
  case SEEK_END:
    if ((long)(mf->dwSize)+lOffset <= 0)
    {
      mf->dwPos = 0;
      return 0;
    }
    mf->dwPos = mf->dwSize + lOffset;
    break;
  }
  if (mf->dwPos > mf->dwSize) mf->dwPos = mf->dwSize;
  return mf->dwPos;
}
/*--------------------------end of MemFile-------------------------*/

/* Извращение №2: имитация класса CPtrArray при помощи С. */
#define PTR_ARR_GRANULARITY 10

void PtrArrayReset(SPtrArray *pa)
{
  pa->pp = 0;
  pa->dwSize = 0;
  pa->dwRsrvSize = 0;
}

int PtrArrayAdd(SPtrArray *pa, void *ptr)
{
  if (pa->dwSize == pa->dwRsrvSize)
  {
    void *ppNew = realloc(pa->pp, (pa->dwRsrvSize + PTR_ARR_GRANULARITY)*sizeof(void*));
    if (ppNew==NULL)
      return -1;

    pa->pp = ppNew;
    pa->dwRsrvSize += PTR_ARR_GRANULARITY;
  }
  pa->pp[pa->dwSize] = ptr;
  pa->dwSize++;
  return ((int)pa->dwSize - 1);
}

void * PtrArrayGetAt(SPtrArray *pa, int index)
{
  return pa->pp[index];
}

void PtrArraySetAt(SPtrArray *pa, int index, void *ptr)
{
  pa->pp[index] = ptr;
}

void PtrArrayFreeAllPtr(SPtrArray *pa)
{
  int i;
  for(i=0; i<(int)pa->dwSize; i++) if (pa->pp[i]) { free(pa->pp[i]); pa->pp[i]=0; }
  free(pa->pp);
  PtrArrayReset(pa);
}

int PtrArrayGetSize(SPtrArray *pa)
{
  return (int)pa->dwSize;
}

int PtrArrayRemoveAt(SPtrArray *pa, int index)
{
  int i;
  if (index<0 || index>=(int)pa->dwSize)
    return -1;

  for(i=index; i<(int)pa->dwSize-1; i++)
    pa->pp[i] = pa->pp[i+1];
  pa->dwSize--;

  if (pa->dwSize==0)
  {
    free(pa->pp);
    PtrArrayReset(pa);
    return 0;
  }

  if (pa->dwRsrvSize-pa->dwSize >= PTR_ARR_GRANULARITY)
  {
    DWORD dwNewSize=(((pa->dwSize-1)/PTR_ARR_GRANULARITY)+1)*PTR_ARR_GRANULARITY;
    void *ppNew = realloc(pa->pp, dwNewSize);
    if (ppNew==NULL)
      return -1;

    pa->pp = ppNew;
    pa->dwRsrvSize = dwNewSize;
  }
  return 0;
}

int PtrArrayFreeAt(SPtrArray *pa, int index)
{
  if (index<0 || index>=(int)pa->dwSize)
    return -1;
  if (pa->pp[index]==0)
    return -2;

  free(pa->pp[index]);
  return PtrArrayRemoveAt(pa,index);
}
/*--------------------------end of PtrArray-------------------------*/
