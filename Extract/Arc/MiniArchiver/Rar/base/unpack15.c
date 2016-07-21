#include <Prague/prague.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------

#define STARTL1  2
static unsigned int DecL1[]={0x8000,0xa000,0xc000,0xd000,0xe000,0xea00,
                             0xee00,0xf000,0xf200,0xf200,0xffff};
static unsigned int PosL1[]={0,0,0,2,3,5,7,11,16,20,24,32,32};

#define STARTL2  3
static unsigned int DecL2[]={0xa000,0xc000,0xd000,0xe000,0xea00,0xee00,
                             0xf000,0xf200,0xf240,0xffff};
static unsigned int PosL2[]={0,0,0,0,5,7,9,13,18,22,26,34,36};

#define STARTHF0  4
static unsigned int DecHf0[]={0x8000,0xc000,0xe000,0xf200,0xf200,0xf200,
                              0xf200,0xf200,0xffff};
static unsigned int PosHf0[]={0,0,0,0,0,8,16,24,33,33,33,33,33};


#define STARTHF1  5
static unsigned int DecHf1[]={0x2000,0xc000,0xe000,0xf000,0xf200,0xf200,
                              0xf7e0,0xffff};
static unsigned int PosHf1[]={0,0,0,0,0,0,4,44,60,76,80,80,127};


#define STARTHF2  5
static unsigned int DecHf2[]={0x1000,0x2400,0x8000,0xc000,0xfa00,0xffff,
                              0xffff,0xffff};
static unsigned int PosHf2[]={0,0,0,0,0,0,2,7,53,117,233,0,0};


#define STARTHF3  6
static unsigned int DecHf3[]={0x800,0x2400,0xee00,0xfe80,0xffff,0xffff,
                              0xffff};
static unsigned int PosHf3[]={0,0,0,0,0,0,0,2,16,218,251,0,0};


#define STARTHF4  8
static unsigned int DecHf4[]={0xff00,0xffff,0xffff,0xffff,0xffff,0xffff};
static unsigned int PosHf4[]={0,0,0,0,0,0,0,0,0,255,0,0,0};

#define fgetbits()  BitGet(input)
#define faddbits(x) BitAdd(input, x)

// ----------------------------------------------------------------------------

unsigned int DecodeNum(tRAR* data, int Num,unsigned int StartPos,
                       unsigned int *DecTab,unsigned int *PosTab);
void OldCopyString(tRAR* data, unsigned int Distance,unsigned int Length);
void CorrHuff(tRAR* data, unsigned int *CharSet,unsigned int *NumToPlace);
void InitHuff(tRAR* data);
void OldUnpInitData(tRAR* data, int Solid);
void GetFlagsBuf(tRAR* data);
void HuffDecode(tRAR* data);
void LongLZ(tRAR* data);
void ShortLZ(tRAR* data);
void OldUnpWriteBuf(tRAR* data);

// ----------------------------------------------------------------------------

void Unpack15(tRAR* data, tINPUT* input, rarbool Solid)
{
  if (data->Suspended)
    data->UnpPtr = data->WrPtr;
  else
  {
    UnpInitData(data, Solid);
    OldUnpInitData(data, Solid);
    if (!Solid)
    {
      InitHuff(data);
      data->UnpPtr=0;
    }
    else
      data->UnpPtr=data->WrPtr;
    --data->DestUnpSize;
  }
  if ((tINT)(data->DestUnpSize)>=0)
  {
    GetFlagsBuf(data);
    data->FlagsCnt=8;
  }

  while ( (tINT)(data->DestUnpSize)>=0 && !data->infile.eof )
  {
    data->UnpPtr &= MAXWINMASK;

//    if (InAddr>ReadTop-30 && !UnpReadBuf())
//      break;
    if (((data->WrPtr-data->UnpPtr) & MAXWINMASK) < 270 && 
          data->WrPtr != data->UnpPtr)
    {
      OldUnpWriteBuf(data);
      if (data->Suspended)
        return;
    }
    if (data->StMode)
    {
      HuffDecode(data);
      continue;
    }

    if ((tINT)(--data->FlagsCnt) < 0)
    {
      GetFlagsBuf(data);
      data->FlagsCnt=7;
    }

    if (data->FlagBuf & 0x80)
    {
      data->FlagBuf<<=1;
      if (data->Nlzb > data->Nhfb)
        LongLZ(data);
      else
        HuffDecode(data);
    }
    else
    {
      data->FlagBuf<<=1;
      if ((tINT)(--data->FlagsCnt) < 0)
      {
        GetFlagsBuf(data);
        data->FlagsCnt=7;
      }
      if (data->FlagBuf & 0x80)
      {
        data->FlagBuf<<=1;
        if (data->Nlzb > data->Nhfb)
          HuffDecode(data);
        else
          LongLZ(data);
      }
      else
      {
        data->FlagBuf<<=1;
        ShortLZ(data);
      }
    }
  }
  OldUnpWriteBuf(data);
}

// ----------------------------------------------------------------------------

void OldUnpWriteBuf(tRAR* data)
{
  tUINT  Cnt;
  tBYTE* Ptr;

  if (data->UnpPtr!=data->WrPtr)
    data->UnpSomeRead=TRUE;
  if (data->UnpPtr<data->WrPtr)
  {
    Ptr = &data->Window[data->WrPtr];
    Cnt = (0-data->WrPtr) & MAXWINMASK;
    FileWrite(data, Ptr ,Cnt);
    data->wr_crc32 = CRC32(data->wr_crc32,Ptr,Cnt);
    Ptr = data->Window;
    Cnt = data->UnpPtr;
    FileWrite(data, Ptr, Cnt);
    data->wr_crc32 = CRC32(data->wr_crc32,Ptr,Cnt);
    data->UnpAllBuf = TRUE;
  }
  else
  {
    Ptr = &data->Window[data->WrPtr];
    Cnt = (data->UnpPtr-data->WrPtr);
    FileWrite(data, Ptr, Cnt);
    data->wr_crc32 = CRC32(data->wr_crc32,Ptr,Cnt);
  }

  data->WrPtr = data->UnpPtr;
}

// ----------------------------------------------------------------------------

void ShortLZ(tRAR* data)
{
  static unsigned int ShortLen1[]={1,3,4,4,5,6,7,8,8,4,4,5,6,6,4,0};
  static unsigned int ShortXor1[]={0,0xa0,0xd0,0xe0,0xf0,0xf8,0xfc,0xfe,
                                   0xff,0xc0,0x80,0x90,0x98,0x9c,0xb0};
  static unsigned int ShortLen2[]={2,3,3,3,4,4,5,6,6,4,4,5,6,6,4,0};
  static unsigned int ShortXor2[]={0,0x40,0x60,0xa0,0xd0,0xe0,0xf0,0xf8,
                                   0xfc,0xc0,0x80,0x90,0x98,0x9c,0xb0};


  unsigned int Length,SaveLength;
  unsigned int LastDistance;
  unsigned int Distance;
  int DistancePlace;
  tINPUT* input;
  unsigned int BitField;

  input = &data->infile;
  data->NumHuf=0;

  BitField = fgetbits();
  if (data->LCount==2)
  {
    faddbits(1);
    if (BitField >= 0x8000)
    {
      OldCopyString(data, (unsigned int)data->LastDist, data->LastLength);
      return;
    }
    BitField <<= 1;
    data->LCount=0;
  }

  BitField>>=8;

  ShortLen1[1] = ShortLen2[3] = data->Buf60 + 3;

  if (data->AvrLn1<37)
  {
    for (Length=0;;Length++)
      if (((BitField^ShortXor1[Length]) & (~(0xff>>ShortLen1[Length])))==0)
        break;
    faddbits(ShortLen1[Length]);
  }
  else
  {
    for (Length=0;;Length++)
      if (((BitField^ShortXor2[Length]) & (~(0xff>>ShortLen2[Length])))==0)
        break;
    faddbits(ShortLen2[Length]);
  }

  if (Length >= 9)
  {
    if (Length == 9)
    {
      data->LCount++;
      OldCopyString(data,(unsigned int)data->LastDist,data->LastLength);
      return;
    }
    if (Length == 14)
    {
      data->LCount=0;
      Length=DecodeNum(data,fgetbits(),STARTL2,DecL2,PosL2)+5;
      Distance=(fgetbits()>>1) | 0x8000;
      faddbits(15);
      data->LastLength=Length;
      data->LastDist=Distance;
      OldCopyString(data,Distance,Length);
      return;
    }

    data->LCount=0;
    SaveLength=Length;
    Distance=data->OldDist[(data->OldDistPtr-(Length-9)) & 3];
    Length=DecodeNum(data,fgetbits(),STARTL1,DecL1,PosL1)+2;
    if (Length==0x101 && SaveLength==10)
    {
      data->Buf60 ^= 1;
      return;
    }
    if (Distance > 256)
      Length++;
    if (Distance >= data->MaxDist3)
      Length++;

    data->OldDist[data->OldDistPtr++]=Distance;
    data->OldDistPtr = data->OldDistPtr & 3;
    data->LastLength=Length;
    data->LastDist=Distance;
    OldCopyString(data, Distance,Length);
    return;
  }

  data->LCount=0;
  data->AvrLn1 += Length;
  data->AvrLn1 -= data->AvrLn1 >> 4;

  DistancePlace=DecodeNum(data,fgetbits(),STARTHF2,DecHf2,PosHf2) & 0xff;
  Distance=data->ChSetA[DistancePlace];
  if (--DistancePlace != -1)
  {
    data->PlaceA[Distance]--;
    LastDistance=data->ChSetA[DistancePlace];
    data->PlaceA[LastDistance]++;
    data->ChSetA[DistancePlace+1]=LastDistance;
    data->ChSetA[DistancePlace]=Distance;
  }
  Length+=2;
  data->OldDist[data->OldDistPtr++] = ++Distance;
  data->OldDistPtr = data->OldDistPtr & 3;
  data->LastLength=Length;
  data->LastDist=Distance;
  OldCopyString(data,Distance,Length);
}

// ----------------------------------------------------------------------------

void LongLZ(tRAR* data)
{
  unsigned int Length;
  unsigned int Distance;
  unsigned int DistancePlace,NewDistancePlace;
  unsigned int OldAvr2,OldAvr3;
  unsigned int BitField;
  tINPUT* input = &data->infile;

  data->NumHuf=0;
  data->Nlzb+=16;
  if (data->Nlzb > 0xff)
  {
    data->Nlzb=0x90;
    data->Nhfb >>= 1;
  }
  OldAvr2=data->AvrLn2;

  BitField=fgetbits();
  if (data->AvrLn2 >= 122)
    Length=DecodeNum(data,BitField,STARTL2,DecL2,PosL2);
  else
    if (data->AvrLn2 >= 64)
      Length=DecodeNum(data,BitField,STARTL1,DecL1,PosL1);
    else
      if (BitField < 0x100)
      {
        Length=BitField;
        faddbits(16);
      }
      else
      {
        for (Length=0;((BitField<<Length)&0x8000)==0;Length++)
          ;
        faddbits(Length+1);
      }

  data->AvrLn2 += Length;
  data->AvrLn2 -= data->AvrLn2 >> 5;

  BitField=fgetbits();
  if (data->AvrPlcB > 0x28ff)
    DistancePlace=DecodeNum(data,BitField,STARTHF2,DecHf2,PosHf2);
  else
    if (data->AvrPlcB > 0x6ff)
      DistancePlace=DecodeNum(data,BitField,STARTHF1,DecHf1,PosHf1);
    else
      DistancePlace=DecodeNum(data,BitField,STARTHF0,DecHf0,PosHf0);

  data->AvrPlcB += DistancePlace;
  data->AvrPlcB -= data->AvrPlcB >> 8;
  while (1)
  {
    Distance = data->ChSetB[DistancePlace & 0xff];
    NewDistancePlace = data->NToPlB[Distance++ & 0xff]++;
    if (!(Distance & 0xff))
      CorrHuff(data,data->ChSetB,data->NToPlB);
    else
      break;
  }

  data->ChSetB[DistancePlace]=data->ChSetB[NewDistancePlace];
  data->ChSetB[NewDistancePlace]=Distance;

  Distance=((Distance & 0xff00) | (fgetbits() >> 8)) >> 1;
  faddbits(7);

  OldAvr3=data->AvrLn3;
  if (Length!=1 && Length!=4){
    if (Length==0 && Distance <= data->MaxDist3)
    {
      data->AvrLn3++;
      data->AvrLn3 -= data->AvrLn3 >> 8;
    }
    else
      if (data->AvrLn3 > 0)
        data->AvrLn3--;
  }
  Length+=3;
  if (Distance >= data->MaxDist3)
    Length++;
  if (Distance <= 256)
    Length+=8;
  if (OldAvr3 > 0xb0 || (data->AvrPlc >= 0x2a00 && OldAvr2 < 0x40))
    data->MaxDist3=0x7f00;
  else
    data->MaxDist3=0x2001;
  data->OldDist[data->OldDistPtr++]=Distance;
  data->OldDistPtr = data->OldDistPtr & 3;
  data->LastLength=Length;
  data->LastDist=Distance;
  OldCopyString(data,Distance,Length);
}

// ----------------------------------------------------------------------------

void HuffDecode(tRAR* data)
{
  unsigned int CurByte,NewBytePlace;
  unsigned int Length;
  unsigned int Distance;
  int BytePlace;
  tINPUT* input;
  unsigned int BitField;

  input = &data->infile;
  BitField=fgetbits();

  if (data->AvrPlc > 0x75ff)
    BytePlace=DecodeNum(data,BitField,STARTHF4,DecHf4,PosHf4);
  else
    if (data->AvrPlc > 0x5dff)
      BytePlace=DecodeNum(data,BitField,STARTHF3,DecHf3,PosHf3);
    else
      if (data->AvrPlc > 0x35ff)
        BytePlace=DecodeNum(data,BitField,STARTHF2,DecHf2,PosHf2);
      else
        if (data->AvrPlc > 0x0dff)
          BytePlace=DecodeNum(data,BitField,STARTHF1,DecHf1,PosHf1);
        else
          BytePlace=DecodeNum(data,BitField,STARTHF0,DecHf0,PosHf0);
  BytePlace&=0xff;
  if (data->StMode)
  {
    if (BytePlace==0 && BitField > 0xfff)
      BytePlace=0x100;
    if (--BytePlace==-1)
    {
      BitField=fgetbits();
      faddbits(1);
      if (BitField & 0x8000)
      {
        data->NumHuf=data->StMode=0;
        return;
      }
      else
      {
        Length = (BitField & 0x4000) ? 4 : 3;
        faddbits(1);
        Distance=DecodeNum(data,fgetbits(),STARTHF2,DecHf2,PosHf2);
        Distance = (Distance << 5) | (fgetbits() >> 11);
        faddbits(5);
        OldCopyString(data,Distance,Length);
        return;
      }
    }
  }
  else
    if (data->NumHuf++ >= 16 && data->FlagsCnt==0)
      data->StMode=1;
  data->AvrPlc += BytePlace;
  data->AvrPlc -= data->AvrPlc >> 8;
  data->Nhfb+=16;
  if (data->Nhfb > 0xff)
  {
    data->Nhfb=0x90;
    data->Nlzb >>= 1;
  }

  data->Window[data->UnpPtr++]=(byte)(data->ChSet[BytePlace]>>8);
  --data->DestUnpSize;

  while (1)
  {
    CurByte=data->ChSet[BytePlace];
    NewBytePlace=data->NToPl[CurByte++ & 0xff]++;
    if ((CurByte & 0xff) > 0xa1)
      CorrHuff(data,data->ChSet,data->NToPl);
    else
      break;
  }

  data->ChSet[BytePlace]=data->ChSet[NewBytePlace];
  data->ChSet[NewBytePlace]=CurByte;
}

// ----------------------------------------------------------------------------

void GetFlagsBuf(tRAR* data)
{
  unsigned int Flags,NewFlagsPlace;
  unsigned int FlagsPlace;
  tINPUT* input = &data->infile;

  FlagsPlace = DecodeNum(data, fgetbits(),STARTHF2,DecHf2,PosHf2);

  while (1)
  {
    Flags=data->ChSetC[FlagsPlace];
    data->FlagBuf=Flags>>8;
    NewFlagsPlace=data->NToPlC[Flags++ & 0xff]++;
    if ((Flags & 0xff) != 0)
      break;
    CorrHuff(data,data->ChSetC,data->NToPlC);
  }

  data->ChSetC[FlagsPlace]=data->ChSetC[NewFlagsPlace];
  data->ChSetC[NewFlagsPlace]=Flags;
}

// ----------------------------------------------------------------------------

void OldUnpInitData(tRAR* data, int Solid)
{
  if (!Solid)
  {
    data->AvrPlcB=data->AvrLn1=data->AvrLn2=data->AvrLn3=data->NumHuf=
      data->Buf60=0;
    data->AvrPlc=0x3500;
    data->MaxDist3=0x2001;
    data->Nhfb=data->Nlzb=0x80;
  }
  data->FlagsCnt=0;
  data->FlagBuf=0;
  data->StMode=0;
  data->LCount=0;
  data->ReadTop=0;
}

// ----------------------------------------------------------------------------

void InitHuff(tRAR* data)
{
  unsigned int I;

  for (I=0; I<256;I++)
  {
    data->Place[I]=data->PlaceA[I]=data->PlaceB[I]=I;
    data->PlaceC[I]=(~I+1) & 0xff;
    data->ChSet[I]=data->ChSetB[I]=I<<8;
    data->ChSetA[I]=I;
    data->ChSetC[I]=((~I+1) & 0xff)<<8;
  }
  memset(data->NToPl,0,sizeof(data->NToPl));
  memset(data->NToPlB,0,sizeof(data->NToPlB));
  memset(data->NToPlC,0,sizeof(data->NToPlC));
  CorrHuff(data,data->ChSetB,data->NToPlB);
}

// ----------------------------------------------------------------------------

void CorrHuff(tRAR* data, unsigned int *CharSet,unsigned int *NumToPlace)
{
  int I, J;
  for (I=7;I>=0;I--)
    for (J=0;J<32;J++,CharSet++)
      *CharSet=(*CharSet & ~0xff) | I;
  memset(NumToPlace,0,sizeof(data->NToPl));
  for ( I=6; I >= 0; I--)
    NumToPlace[I] = (unsigned int)((7-I)*32);
}

// ----------------------------------------------------------------------------
 
void OldCopyString(tRAR* data, unsigned int Distance,unsigned int Length)
{
  data->DestUnpSize-=Length;
  while (Length--)
  {
    data->Window[data->UnpPtr]=data->Window[(data->UnpPtr-Distance) & MAXWINMASK];
    data->UnpPtr=(data->UnpPtr+1) & MAXWINMASK;
  }
}

// ----------------------------------------------------------------------------

unsigned int DecodeNum(tRAR* data, int Num,unsigned int StartPos,
                       unsigned int *DecTab,unsigned int *PosTab)
{
  int I;
  tINPUT* input;

  input = &data->infile;
  for (Num &= 0xfff0, I=0; DecTab[I] <= (unsigned int)Num; I++)
    StartPos++;
  faddbits(StartPos);
  return(((Num-(I ? DecTab[I-1]:0))>>(16-StartPos))+PosTab[StartPos]);
}

// ----------------------------------------------------------------------------
