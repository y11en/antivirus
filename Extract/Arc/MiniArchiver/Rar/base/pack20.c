#include <Prague/prague.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------

#define  SIZELASTOC2       0x4000
#define  SIZELASTOCC   (4*0x8000)
#define  SIZEREADBUF        0x800

#define PutCode(C)             data->CodeHBuf[data->CodePtr++]=(tBYTE)(C)
#define CheckCodeBufWrite()    if (data->CodePtr>data->CodeSize) WriteCode(data)

unsigned int ReadData(tPACK20* data, int FirstBuf);
void WriteCode(tPACK20* data);
void PackData(tPACK20* data, int Solid);
void SearchMatch(tPACK20* data, int Depth);
void PackInit(tPACK20* data, int Solid,int Depth);
void ClearPackData(tPACK20* data);
void CompressGeneral(tPACK20* data);

static int SearchDepth[5]={5,15,35,250,1500};

// ----------------------------------------------------------------------------

void Pack20Proc(tPACK20* data, tBYTE *Addr,  unsigned int (* PackRead)
  (tPACK20* data, tBYTE *,unsigned int), void (* CodeBlock)
  (tPACK20* data), unsigned int Solid, unsigned int Method,
  tBYTE *CodeBufAddr,unsigned int CodeBufSize,long WindowSize)
{
  data->Window=Addr;
  data->LastOc2=data->Window+WindowSize;
  data->LastOcc=(tUINT*)(data->LastOc2+SIZELASTOC2);
  data->List=(tUINT*)((unsigned char *)data->LastOcc+SIZELASTOCC);
  data->CodeHBuf=CodeBufAddr;
  data->CodeSize=CodeBufSize-16;

  data->PackReadPtr=PackRead;
  data->PackWritePtr=CodeBlock;

  data->WinSize=WindowSize;
  data->WinMask=data->WinSize-1;
  data->WinMaxDist=data->WinSize-SIZEREADBUF-0x130;
  PutCode(CODE_STARTFILE);
  PackInit(data,Solid,SearchDepth[Method]);
  data->NextReadAddr=data->SavePtr;
  if (ReadData(data,!(Solid && data->SomeRead)))
    PackData(data,Solid);
  PutCode(CODE_ENDFILE);
}


unsigned int ReadData(tPACK20* data, int FirstBuf)
{
  int ReadSize;

  if (!FirstBuf && data->NextReadAddr==0)
    data->AllBuf=1;
  if (FirstBuf)
    ReadSize=0x10000L;
  else
  {
    ReadSize=SIZEREADBUF;
    if (data->NextReadAddr & 0x7ff)
      ReadSize-=data->NextReadAddr-(data->NextReadAddr & (0-SIZEREADBUF));
  }
  memset(&data->List[data->NextReadAddr],0,ReadSize*sizeof(*data->List));
  data->LastReadSize=(*data->PackReadPtr)(data,
    &data->Window[data->NextReadAddr],ReadSize);
  if (data->LastReadSize)
    data->SomeRead=1;
  data->NextReadAddr=(data->NextReadAddr+data->LastReadSize) & data->WinMask;
  return(data->LastReadSize);
}


void WriteCode(tPACK20* data)
{
  (*data->PackWritePtr)(data);
}


void PackData(tPACK20* data,int Solid)
{
  data->CurPtr=(Solid) ? data->SavePtr : 0;
  data->CmpLength=(data->LastReadSize<=0x101) ? data->LastReadSize : 0x101;

  while (1)
  {
    data->CurPtrChanged=0;

    CompressGeneral(data);

    data->CurPtr&=data->WinMask;
    if (((data->NextReadAddr-data->CurPtr)&data->WinMask)<0x129)
    {
      if (data->NextReadAddr==data->CurPtr)
      {
        if ((data->LastReadSize==0 || (data->CurPtrChanged && ReadData(data,0)==0)))
        {
          data->SavePtr=data->CurPtr;
          break;
        }
      }
      else
        if (data->LastReadSize!=0)
          ReadData(data,0);
      if (((data->NextReadAddr-data->CurPtr)&data->WinMask)<=0x101)
        data->CmpLength=(data->NextReadAddr-data->CurPtr)&data->WinMask;
      else
        data->CmpLength=0x101;
    }
  }
}

void CompressGeneral(tPACK20* data)
{
  unsigned long CurLastOc2;
  unsigned int CurWord;
  int I;
  do
  {
    data->CurPtr&=data->WinMask;
    data->CurPtrChanged=1;
    data->CurHashValue=((data->CurHashValue<<5)^data->Window[(data->CurPtr+2)&data->WinMask]) & 0x7fff;
    data->CurLastOcc=data->LastOcc[data->CurHashValue] & data->WinMask;
    data->LastOcc[data->CurHashValue]=data->CurPtr;
    if ((((data->Window[data->CurLastOcc]<<10)^(data->Window[(data->CurLastOcc+1)&data->WinMask]<<5)^
        data->Window[(data->CurLastOcc+2)&data->WinMask]^data->CurHashValue) & 0x7fff)!=0)
      data->CurLastOcc=0;
    data->List[data->CurPtr]=data->CurLastOcc;

    data->MaxLength=1;

    CurWord=data->Window[data->CurPtr]+(data->Window[(data->CurPtr+1)&data->WinMask]<<8);
    CurLastOc2=data->LastOc2[CurWord & 0x3fff] | (data->CurPtr & ~0xff);
    data->LastOc2[CurWord & 0x3fff]=(tBYTE)data->CurPtr;
    if (CurLastOc2>=data->CurPtr)
      CurLastOc2=(CurLastOc2-0x100)&data->WinMask;
    if (data->Window[CurLastOc2]==(tBYTE)CurWord &&
        data->Window[(CurLastOc2+1)&data->WinMask]==(tBYTE)(CurWord>>8) &&
        data->CmpLength > 1)
    {
      data->MaxLength=2;
      data->MaxDist=(data->CurPtr-CurLastOc2)&data->WinMask;
    }
    if (data->CurLastOcc)
      SearchMatch(data,data->PackLevel);
    if (data->MaxLength<2 || (!data->AllBuf && data->MaxDist>(tLONG)data->CurPtr))
    {
      PutCode(CODE_HUFFMAN);
      PutCode(data->Window[data->CurPtr]);
      CheckCodeBufWrite();
      data->CurPtr=(data->CurPtr+1)&data->WinMask;
    }
    else
    {
      data->CurPtr=(data->CurPtr+1)&data->WinMask;
      for (I=0;I<data->MaxLength-1;I++)
      {
        CurWord=data->Window[data->CurPtr]+(data->Window[(data->CurPtr+1)&data->WinMask]<<8);
        data->LastOc2[CurWord & 0x3fff]=(tBYTE)data->CurPtr;
        data->CurHashValue=((data->CurHashValue<<5)^data->Window[(data->CurPtr+2)&data->WinMask]) & 0x7fff;
        data->CurLastOcc=data->LastOcc[data->CurHashValue];
        data->LastOcc[data->CurHashValue]=data->CurPtr;
        if (((data->Window[(data->CurLastOcc+2)&data->WinMask]^data->CurHashValue) & 0x1f)!=0)
          data->CurLastOcc=0;
        data->List[data->CurPtr]=data->CurLastOcc;
        data->CurPtr=(data->CurPtr+1)&data->WinMask;
      }

      if (data->MaxLength==2)
      {
        PutCode(CODE_LZ2);
        PutCode(--data->MaxDist);
        CheckCodeBufWrite();
      }
      else
      {
        data->MaxLength-=3;
        if (data->MaxDist>=0x2000)
        {
          data->MaxLength--;
          if (data->MaxDist>=0x40000)
            data->MaxLength--;
        }
        PutCode(CODE_LZ);
        PutCode(data->MaxLength);
        PutCode(--data->MaxDist);
        PutCode(data->MaxDist>>8);
        PutCode(data->MaxDist>>16);
        CheckCodeBufWrite();
      }
    }
  } while (((data->NextReadAddr-data->CurPtr)&data->WinMask)>=0x129);
}


void SearchMatch(tPACK20* data, int Depth)
{
  unsigned long CurCmpPtr;
  unsigned long NewDistance;
  int CurLen;
  tBYTE CmpByte;

  CurCmpPtr=data->CurLastOcc;
  CmpByte=data->Window[(data->CurPtr+data->MaxLength)&data->WinMask];
  while (Depth--)
  {
    if (data->Window[(CurCmpPtr+data->MaxLength) & data->WinMask]!=CmpByte)
    {
      if ((CurCmpPtr=data->List[CurCmpPtr] & data->WinMask)==0)
        break;
      if (data->Window[(CurCmpPtr+data->MaxLength) & data->WinMask]!=CmpByte)
      {
        if ((CurCmpPtr=data->List[CurCmpPtr] & data->WinMask)==0)
          break;
        if (data->Window[(CurCmpPtr+data->MaxLength) & data->WinMask]!=CmpByte)
        {
          if ((CurCmpPtr=data->List[CurCmpPtr] & data->WinMask)==0)
            break;
          if (((data->Window[(CurCmpPtr+1)&data->WinMask]<<5)^data->Window[(CurCmpPtr+2)&data->WinMask]^
              data->CurHashValue)&0x3ff)
            break;
          continue;
        }
      }
    }

    for (CurLen=0;CurLen<data->CmpLength;)
      if (data->Window[(data->CurPtr+CurLen)&data->WinMask]==data->Window[(CurCmpPtr+CurLen)&data->WinMask])
        CurLen++;
      else
        break;
    if (CurLen>data->MaxLength)
    {
      NewDistance=(data->CurPtr-CurCmpPtr) & data->WinMask;
      if (NewDistance>data->WinMaxDist || NewDistance==0)
        return;
      if (NewDistance<0x8000 || data->MaxDist>=0x8000 || CurLen>data->MaxLength+1)
        if (CurLen>4 || (CurLen==4 && NewDistance<0x40000) ||
            (CurLen==3 && NewDistance<0x2000 ))
        {
          data->MaxDist=NewDistance;
          data->MaxLength=CurLen;
          if (data->MaxLength>=data->CmpLength)
            return;
          CmpByte=data->Window[(data->CurPtr+data->MaxLength)&data->WinMask];
        }
    }
    if ((CurCmpPtr=data->List[CurCmpPtr] & data->WinMask)==0)
      break;
  }
}


void PackInit(tPACK20* data,int Solid,int Depth)
{
  if (!Solid)
  {
    data->LastReadSize=0;
    data->AllBuf=0;
    data->CurHashValue=0;
    data->NextReadAddr=0;
    data->SavePtr=0;
    data->SomeRead=0;
    data->UnpAllBuf=0;
    memset(data->LastOcc,0,SIZELASTOCC);
    memset(data->LastOc2,0,SIZELASTOC2);
  }

  data->PackLevel=Depth;
  if ((data->PackLevel2=data->PackLevel/2)>200)
    data->PackLevel2=200;

  data->AllBuf|=data->UnpAllBuf;
}
