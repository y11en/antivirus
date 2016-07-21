#include <Prague/prague.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------


#define putbits(data, n, x)                                     \
{                                                               \
  unsigned int PBWord,PBN,PBX;                                  \
  PBN=(unsigned int)n;                                          \
  PBX=(unsigned int)x;                                          \
  if ((tINT)PBN < data->bitcount)                               \
    data->subbitbuf |= PBX << (data->bitcount -= PBN);          \
  else                                                          \
  {                                                             \
    PBWord=data->subbitbuf | (PBX >> (PBN -= data->bitcount));  \
    data->OutBuf[data->OutPtr++]=(tBYTE)(PBWord>>8);            \
    if (data->OutPtr==sizeof(data->OutBuf)-16)                  \
      WriteBuf(data);                                           \
    data->OutBuf[data->OutPtr++]=(tBYTE)PBWord;                 \
    data->subbitbuf=(PBX << (data->bitcount = 16-PBN));         \
    if (data->OutPtr==sizeof(data->OutBuf)-16)                  \
      WriteBuf(data);                                           \
  }                                                             \
}

void fputbits(tPACK20* data, int n, unsigned int x)
{
  putbits(data,n,x);
}


void CodeBlock(tPACK20* data)
{
  unsigned int LitFreq[2*NC-1],LitCode[NC];
  unsigned int DistFreq[2*DC-1],DistCode[DC];
  unsigned int RepFreq[2*RC-1],RepCode[RC];
  struct sBIT_LENGTHS L;
  static unsigned int AndMask[17] = {0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535U};
  static unsigned char LCode1[32] = {0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11,12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15};
  static unsigned char LCode2[32] = {0,0,0,0,16,17,18,19,20,20,21,21,22,22,23,23,24,24,24,24,25,25,25,25,26,26,26,26,27,27,27,27};
  static unsigned char LBits[28]  = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5};
  static unsigned char DCode1[64] = {0,1,2,3,4,4,5,5,6,6,6,6,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11};
  static unsigned char DCode2[64] = {0,0,12,13,14,14,15,15,16,16,16,16,17,17,17,17,18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21};
  static unsigned char DCode3[64] = {0,0,22,23,24,24,25,25,26,26,26,26,27,27,27,27,28,28,28,28,28,28,28,28,29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31};
  static unsigned char DBits[DC]  = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16};
  static unsigned char SDCode[32] = {0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
  static unsigned char SDBits[8]  = {2,2,3,4,5,6,6,6};

  unsigned int Length,Distance,BlockSize,N;
  int TableAfterHead;
  tBYTE *BlockAddr;

  BlockAddr = data->CodeBuf;
  BlockSize = data->CodePtr;

  TableAfterHead=(BlockSize>0 && *BlockAddr==5);
  data->PutBlockSym=(!TableAfterHead); // (GetFileHeaderFlags() & LHD_SOLID));

  // PutFileHeader(3);

  memset(LitFreq,  0, NC*sizeof(LitFreq[0]));
  memset(DistFreq, 0, DC*sizeof(DistFreq[0]));
  memset(RepFreq,  0, RC*sizeof(RepFreq[0]));
  LitFreq[269]=1;

  while (BlockSize)
    switch(*BlockAddr)
    {
      case CODE_HUFFMAN:
        LitFreq[BlockAddr[1]]++;
        BlockSize-=2;
        BlockAddr+=2;
        break;
      case CODE_LZ:
        Length=BlockAddr[1];
        if (Length<32)
          LitFreq[270+LCode1[Length]]++;
        else
          LitFreq[270+LCode2[Length>>3]]++;
        Distance=BlockAddr[2]+(BlockAddr[3]<<8)+(BlockAddr[4]<<16);
        if (Distance<64)
          N=DCode1[Distance];
        else
          if (Distance<2048)
            N=DCode2[Distance>>5];
          else
            if (Distance<65536L)
              N=DCode3[Distance>>10];
            else
              if (Distance<98304L)
                N=32;
              else
                if (Distance<2*65536L)
                  N=33;
                else
                  N=32+(Distance>>16);
        DistFreq[N]++;
        *BlockAddr=(tBYTE)(N+20);
        BlockSize-=5;
        BlockAddr+=5;
        break;
      case CODE_LZ2:
        Distance=BlockAddr[1];
        if (Distance<32)
          N=SDCode[Distance];
        else
          N=(Distance>>6)+4;
        LitFreq[261+N]++;
        BlockSize-=2;
        BlockAddr+=2;
        break;
      case CODE_STARTFILE:
        BlockSize--;
        BlockAddr++;
        break;
      default:
        BlockSize--;
        BlockAddr++;
        break;
    }

  SetSecondFreq(data,LitFreq,NC);
  SetSecondFreq(data,DistFreq,DC);
  SetSecondFreq(data,RepFreq,RC);
  make_tree(data,NC,LitFreq,L.LitLength,LitCode);
  make_tree(data,DC,DistFreq,L.DistLength,DistCode);
  make_tree(data,RC,RepFreq,L.RepLength,RepCode);

  if (!TableAfterHead)
  {
    if (data->PutBlockSym)
      fputbits(data,data->BlockSymLength,data->BlockSymCode);
    WriteTables(data, (BitLengths*)(&L));
  }

  BlockAddr=data->CodeBuf;
  BlockSize=data->CodePtr;

  while (BlockSize)
    switch(*BlockAddr)
    {
      case CODE_HUFFMAN:
        putbits(data,L.LitLength[BlockAddr[1]],LitCode[BlockAddr[1]]);
        BlockSize-=2;
        BlockAddr+=2;
        break;
      case CODE_LZ2:
        Distance=BlockAddr[1];
        if (Distance<32)
          N=SDCode[Distance];
        else
          N=(Distance>>6)+4;
        putbits(data,L.LitLength[261+N],LitCode[261+N]);
        if ((N=SDBits[N])!=0)
          putbits(data,N,Distance & AndMask[N]);
        BlockSize-=2;
        BlockAddr+=2;
        break;
      case CODE_STARTFILE:
        FlushBuf(data);
//        PutFileHeader(0);
        if (TableAfterHead)
        {
          if (data->PutBlockSym)
            fputbits(data,data->BlockSymLength,data->BlockSymCode);
          WriteTables(data, (BitLengths*)&L);
          TableAfterHead=0;
        }
        BlockSize--;
        BlockAddr++;
        break;
      case CODE_ENDFILE:
        FlushBuf(data);
//        PutFileHeader(1);
        BlockSize--;
        BlockAddr++;
        break;
      default:
        Length=BlockAddr[1];
        if (Length<32)
          N=LCode1[Length];
        else
          N=LCode2[Length>>3];
        putbits(data,L.LitLength[270+N],LitCode[270+N]);
        if ((N=LBits[N])!=0)
          putbits(data,N,Length & AndMask[N]);
        Distance=BlockAddr[2]+(BlockAddr[3]<<8)+(BlockAddr[4]<<16);
        N=*BlockAddr-20;
        putbits(data,L.DistLength[N],DistCode[N]);
        N=DBits[N];
        putbits(data,N,Distance & AndMask[N]);
        BlockSize-=5;
        BlockAddr+=5;
        break;
    }

  data->LastBlockSize=data->CodePtr;
  data->CodePtr=0;
  data->BlockSymLength=L.LitLength[269];
  data->BlockSymCode=LitCode[269];
  memcpy(data->PackOldTable,&L,sizeof(L));
//  PutFileHeader(2);
}


void WriteTables(tPACK20* data, BitLengths *LengthTable)
{
  unsigned int BitFreq[2*BC-1],BitCode[BC];
  unsigned char BitLength[BC];
  unsigned char *Table;
  int Count,TableSize,I;

  fputbits(data,2,0);
  memset(data->PackOldTable,0,sizeof(data->PackOldTable));
  Table=(void *)LengthTable;
  TableSize=NC+DC+RC;
  memset(BitFreq,0,sizeof(BitFreq));
  for (I=0;I<TableSize;I++)
  {
    if (I<TableSize-2 && Table[I]==0 && Table[I+1]==0 && Table[I+2]==0)
    {
      for (Count=0,I+=3;I<TableSize && Table[I]==0 && Count<135;I++)
        Count++;
      if (Count<=7)
        BitFreq[17]++;
      else
        BitFreq[18]++;
      I--;
      continue;
    }
    if (I<TableSize-2 && I>0 && Table[I]==Table[I-1] && Table[I]==Table[I+1] && Table[I]==Table[I+2])
    {
      for (Count=0,I+=3;I<TableSize && Table[I]==Table[I-1] && Count<3;I++)
        Count++;
      BitFreq[16]++;
      I--;
      continue;
    }
    BitFreq[(Table[I]-data->PackOldTable[I]) & 0xF]++;
  }
  SetSecondFreq(data,BitFreq,BC);
  make_tree(data,BC,BitFreq,BitLength,BitCode);
  for (I=0;I<BC;I++)
    fputbits(data,4,BitLength[I]);
  for (I=0;I<TableSize;I++)
  {
    if (I<TableSize-2 && Table[I]==0 && Table[I+1]==0 && Table[I+2]==0)
    {
      for (Count=0,I+=3;I<TableSize && Table[I]==0 && Count<135;I++)
        Count++;
      if (Count<=7)
      {
        fputbits(data,BitLength[17],BitCode[17]);
        fputbits(data,3,Count);
      }
      else
      {
        fputbits(data,BitLength[18],BitCode[18]);
        fputbits(data,7,Count-8);
      }
      I--;
      continue;
    }
    if (I<TableSize-2 && I>0 && Table[I]==Table[I-1] && Table[I]==Table[I+1] && Table[I]==Table[I+2])
    {
      for (Count=0,I+=3;I<TableSize && Table[I]==Table[I-1] && Count<3;I++)
        Count++;
      fputbits(data,BitLength[16],BitCode[16]);
      fputbits(data,2,Count);
      I--;
      continue;
    }
    fputbits(data,BitLength[(Table[I]-data->PackOldTable[I])&0xF],BitCode[(Table[I]-data->PackOldTable[I])&0xF]);
  }
}

void WriteBuf(tPACK20* data)
{
  unsigned int I;
  if (data->OutPtr & 0xF)
    for (I=data->OutPtr;I<(data->OutPtr & ~0xF)+0x10;I++)
      data->OutBuf[I]=0;
  Pack20Write(data,data->OutBuf,data->OutPtr);
  data->OutPtr=0;
}


void FlushBuf(tPACK20* data)
{
  if (data->bitcount<16)
  {
    data->subbitbuf&=~(0xFFFF>>(16-data->bitcount));
    data->OutBuf[data->OutPtr++]=(tBYTE)(data->subbitbuf>>8);
    if (data->bitcount<8)
      data->OutBuf[data->OutPtr++]=(tBYTE)data->subbitbuf;
    data->bitcount=16;
    data->subbitbuf=0;
  }
  WriteBuf(data);
}


void FlushCodeBuffer(tPACK20* data)
{
  if (data->CodePtr!=0)
    CodeBlock(data);
}

void PackInitData(tPACK20* data, int Solid)
{
  if (!Solid)
  {
    data->CodePtr=0;
    data->subbitbuf=0;
    data->OutPtr=0;
    data->bitcount=16;
    data->LastBlockSize=0;
    memset(data->PackOldTable,0,sizeof(data->PackOldTable));
  }
}


void SetSecondFreq(tPACK20* data, unsigned int *TabFreq,int TabSize)
{
  int I,NonZero;
  for (NonZero=I=0;I<TabSize && NonZero<2;I++)
    if (TabFreq[I])
      NonZero++;
  if (NonZero==1) {
    if (TabFreq[0]==0)
      TabFreq[0]=1;
    else
      TabFreq[1]=1;
  }
}
