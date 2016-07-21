#include <Prague/prague.h>

#include "const.h"
#include "proto.h"


RSTATIC void RATTRIB CopyString20(tRAR *data, unsigned int Length,unsigned int Distance)
{
    data->LastDist = data->OldDist[data->OldDistPtr++ & 3] = Distance;
    data->LastLength = Length;
    data->DestUnpSize20 -= Length;

    CopyString(data, Length, Distance);
}

RSTATIC rarbool RATTRIB ReadTables20(tRAR *data, tINPUT *input)
{
    byte BitLength[BC20];
    uint BitField;
    unsigned char Table[MC20*4];
    int TableSize;
    int N;
    int I;

    BitField = BitGet(input);
    data->UnpAudioBlock = (BitField & 0x8000) != 0;

    if (!(BitField & 0x4000)) memset(data->UnpOldTable20,0,sizeof(data->UnpOldTable20));

    BitAdd(input, 2);

    if (data->UnpAudioBlock)
    {
        data->UnpChannels = ((BitField>>12) & 3)+1;
        if (data->UnpCurChannel >= data->UnpChannels ) data->UnpCurChannel = 0;
        BitAdd(input, 2);
        TableSize = MC20*data->UnpChannels;
    }
    else
    {
        TableSize=NC20+DC20+RC20;
    }

    for (I=0;I<BC20;I++)
    {
        BitLength[I]=(byte)(BitGet(input) >> 12);
        BitAdd(input, 4);
    }

    MakeDecodeTables(BitLength,(struct sDECODE*)&data->BD,BC20);

    I=0;
    while (I<TableSize)
    {
        int Number;

        Number = DecodeNumber(input,(struct sDECODE*)&data->BD);

        if (Number<16)
        {
            Table[I]=(byte)((Number+data->UnpOldTable20[I]) & 0xf);
            I++;
        }
        else if (Number==16)
        {
            N=(BitGet(input) >> 14)+3;
            BitAdd(input, 2);
            if ( I ) while (N-- > 0 && I<TableSize)
            {
                Table[I]=Table[I-1];
                I++;
            }
        }
        else
        {
            if (Number==17)
            {
                N=(BitGet(input) >> 13)+3;
                BitAdd(input, 3);
            }
            else
            {
                N=(BitGet(input) >> 9)+11;
                BitAdd(input, 7);
            }
            while (N-- > 0 && I<TableSize) Table[I++]=0;
        }
    }

    // check eof
    if ( input->eof )
    {
        return FALSE;
    }

    if (data->UnpAudioBlock)
    {
        for (I=0;I<data->UnpChannels;I++)
        {
            MakeDecodeTables(&Table[I*MC20],(struct sDECODE*)&data->MD[I],MC20);
        }
    }
    else
    {
        MakeDecodeTables(&Table[0],(struct sDECODE*)&data->LD,NC20);
        MakeDecodeTables(&Table[NC20],(struct sDECODE*)&data->DD,DC20);
        MakeDecodeTables(&Table[NC20+DC20],(struct sDECODE*)&data->RD,RC20);
    }
    memcpy(data->UnpOldTable20,Table,sizeof(data->UnpOldTable20));
    return(TRUE);
}


RSTATIC void RATTRIB ReadLastTables(tRAR *data, tINPUT *input)
{
    if (data->UnpAudioBlock)
    {
      if (DecodeNumber(input,(struct sDECODE*)&data->MD[data->UnpCurChannel])==256)
        ReadTables20(data, input);
    }
    else
      if (DecodeNumber(input,(struct sDECODE*)&data->LD)==269)
        ReadTables20(data, input);
}

// --- used intrinsic ---
// RSTATIC int RATTRIB abs(int num)
// {
//   return (num < 0)? -num: num;
// }

RSTATIC byte RATTRIB DecodeAudio(tRAR *data, int Delta)
{
  tAUDIO_VARIABLES *V=&data->AudV[data->UnpCurChannel];
  unsigned int Ch;
  int PCh, D;

  V->ByteCount++;
  V->D4=V->D3;
  V->D3=V->D2;
  V->D2=V->LastDelta-V->D1;
  V->D1=V->LastDelta;
  PCh = 8*V->LastChar;
  PCh = PCh + V->K1*V->D1;
  PCh = PCh + V->K2*V->D2;
  PCh = PCh + V->K3*V->D3;
  PCh = PCh + V->K4*V->D4;
  PCh = PCh + V->K5*data->UnpChannelDelta;
  PCh = (PCh >> 3) & 0xFF;

  Ch=PCh-Delta;

  D=((signed char)Delta)<<3;

  V->Dif[0]+=abs(D);
  V->Dif[1]+=abs(D-V->D1);
  V->Dif[2]+=abs(D+V->D1);
  V->Dif[3]+=abs(D-V->D2);
  V->Dif[4]+=abs(D+V->D2);
  V->Dif[5]+=abs(D-V->D3);
  V->Dif[6]+=abs(D+V->D3);
  V->Dif[7]+=abs(D-V->D4);
  V->Dif[8]+=abs(D+V->D4);
  V->Dif[9]+=abs(D-data->UnpChannelDelta);
  V->Dif[10]+=abs(D+data->UnpChannelDelta);

  data->UnpChannelDelta=V->LastDelta=(signed char)(Ch-V->LastChar);
  V->LastChar=Ch;

  if ((V->ByteCount & 0x1F)==0)
  {
    unsigned int MinDif = V->Dif[0];
    unsigned int NumMinDif = 0;
    int I;

    V->Dif[0]=0;
    for (I=1;I<sizeof(V->Dif)/sizeof(V->Dif[0]);I++)
    {
      if (V->Dif[I]<MinDif)
      {
        MinDif=V->Dif[I];
        NumMinDif=I;
      }
      V->Dif[I]=0;
    }
    switch(NumMinDif)
    {
      case 1:
        if (V->K1>=-16)
          V->K1--;
        break;
      case 2:
        if (V->K1<16)
          V->K1++;
        break;
      case 3:
        if (V->K2>=-16)
          V->K2--;
        break;
      case 4:
        if (V->K2<16)
          V->K2++;
        break;
      case 5:
        if (V->K3>=-16)
          V->K3--;
        break;
      case 6:
        if (V->K3<16)
          V->K3++;
        break;
      case 7:
        if (V->K4>=-16)
          V->K4--;
        break;
      case 8:
        if (V->K4<16)
          V->K4++;
        break;
      case 9:
        if (V->K5>=-16)
          V->K5--;
        break;
      case 10:
        if (V->K5<16)
          V->K5++;
        break;
    }
  }
  return((byte)Ch);
}

RSTATIC void RATTRIB Unpack20(tRAR *data, tINPUT *input, rarbool Solid)
{
  static const unsigned char LDecode[]={0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224};
  static const unsigned char LBits[]=  {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5};
  static const int DDecode[]={0,1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,768,1024,1536,2048,3072,4096,6144,8192,12288,16384,24576,32768U,49152U,65536,98304,131072,196608,262144,327680,393216,458752,524288,589824,655360,720896,786432,851968,917504,983040};
  static const unsigned char DBits[]=  {0,0,0,0,1,1,2, 2, 3, 3, 4, 4, 5, 5,  6,  6,  7,  7,  8,  8,   9,   9,  10,  10,  11,  11,  12,   12,   13,   13,    14,    14,   15,   15,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,    16};
  static const unsigned char SDDecode[]={0,4,8,16,32,64,128,192};
  static const unsigned char SDBits[]=  {2,2,3, 4, 5, 6,  6,  6};
  unsigned int Bits;

  data->DestUnpSize20 = data->DestUnpSize;
  UnpInitData(data, Solid);
  if (!Solid)
  {
    if (!ReadTables20(data, input))
     return;
  }

  //UnpInitData(data,Solid);
  data->DestUnpSize20--;
  while (data->DestUnpSize20 < data->DestUnpSize && !input->eof)
  {
    int Number;

    data->UnpPtr &= MAXWINMASK;

    if (((data->WrPtr-data->UnpPtr) & MAXWINMASK)<270 && data->WrPtr!=data->UnpPtr)
    {
      UnpWriteBuf(data);
      if (data->Suspended) return;
    }
    if (data->UnpAudioBlock)
    {
      int  AudioNumber;
      unsigned char da;
      AudioNumber = DecodeNumber(input,(struct sDECODE*)&data->MD[data->UnpCurChannel]);

      if (AudioNumber==256)
      {
        if (!ReadTables20(data,input))
          break;
        continue;
      }

      da = DecodeAudio(data,AudioNumber);
      data->Window[data->UnpPtr++] = da;
      if (++data->UnpCurChannel==data->UnpChannels)
        data->UnpCurChannel=0;
      --data->DestUnpSize20;
      continue;
    }

    Number=DecodeNumber(input,(struct sDECODE*)&data->LD);
    if (Number<256)
    {
      data->Window[data->UnpPtr++]=(byte)Number;
      --data->DestUnpSize20;
      continue;
    }
    if (Number>269)
    {
      int Length;
      int DistNumber;
      uint Distance;

      Length = LDecode[Number-=270]+3;
      if ((Bits=LBits[Number])>0)
      {
        Length+=BitGet(input)>>(16-Bits);
        BitAdd(input, Bits);
      }

      DistNumber=DecodeNumber(input,(struct sDECODE*)&data->DD);
      Distance=DDecode[DistNumber]+1;

      if ((Bits=DBits[DistNumber])>0)
      {
        Distance+=BitGet(input)>>(16-Bits);
        BitAdd(input, Bits);
      }

      if (Distance>=0x2000)
      {
        Length++;
        if (Distance>=0x40000L)
          Length++;
      }

      CopyString20(data, Length,Distance);
      continue;
    }
    if (Number==269)
    {
      if (!ReadTables20(data,input))
        break;
      continue;
    }
    if (Number==256)
    {
      CopyString20(data,data->LastLength,data->LastDist);
      continue;
    }
    if (Number<261)
    {
      unsigned int Distance = data->OldDist[(data->OldDistPtr-(Number-256)) & 3];
      int LengthNumber=DecodeNumber(input,(struct sDECODE*)&data->RD);
      int Length=LDecode[LengthNumber]+2;
      if ((Bits=LBits[LengthNumber])>0)
      {
        Length+=BitGet(input)>>(16-Bits);
        BitAdd(input, Bits);
      }
      if (Distance>=0x101)
      {
        Length++;
        if (Distance>=0x2000)
        {
          Length++;
          if (Distance>=0x40000)
            Length++;
        }
      }
      CopyString20(data,Length,Distance);
      continue;
    }
    if (Number<270)
    {
      unsigned int Distance=SDDecode[Number-=261]+1;
      if ((Bits=SDBits[Number])>0)
      {
        Distance+=BitGet(input)>>(16-Bits);
        BitAdd(input, Bits);
      }
      CopyString20(data,2,Distance);
      continue;
   }
  }
  ReadLastTables(data,input);
  UnpWriteBuf(data);
}


RSTATIC void RATTRIB UnpInitData20(tRAR *data, int Solid)
{
  if (!Solid)
  {
    data->UnpChannelDelta=data->UnpCurChannel=0;
    data->UnpChannels=1;
    memset(data->AudV,0,sizeof(data->AudV));
    memset(data->UnpOldTable20,0,sizeof(data->UnpOldTable20));
  }
}


