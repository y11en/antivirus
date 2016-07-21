#include <Prague/prague.h>

#include "const.h"
#include "proto.h"


RSTATIC rarbool RATTRIB WindowMemFix(tRAR *data, void *newptr, void *oldptr)
{
    //oldptr;

    data->Window = newptr;
    return TRUE;
}

RSTATIC void RATTRIB UnpackCreate(tRAR *data, uint WinSize)
{
    // class constructor
    data->WindowSize = WinSize;
    data->WindowMask = WinSize-1;

    data->Window = RarAlloc(data,MAXWINSIZE,WindowMemFix);
    data->Suspended =   FALSE;
    data->UnpAllBuf =   FALSE;
    data->UnpSomeRead = FALSE;

    data->PPM.SubAlloc.SubAllocatorSize = 0;
}

RSTATIC void RATTRIB MakeDecodeTables(unsigned char *LenTab,struct sDECODE *Dec,int Size)
{
  int LenCount[16];
  int TmpPos[16];
  int I;
  long M;
  long N;

  memset(LenCount,0,sizeof(LenCount));
  memset(Dec->DecodeNum,0,Size*sizeof(*Dec->DecodeNum));

  for (I=0;I<Size;I++) LenCount[LenTab[I] & 0xF]++;

  LenCount[0]=0;
  for (TmpPos[0]=Dec->DecodePos[0]=Dec->DecodeLen[0]=0,N=0,I=1;I<16;I++)
  {
    N=2*(N+LenCount[I]);
    M=N<<(15-I);
    if (M>0xFFFF)
      M=0xFFFF;
    Dec->DecodeLen[I]=(unsigned int)M;
    TmpPos[I]=Dec->DecodePos[I]=Dec->DecodePos[I-1]+LenCount[I-1];
  }

  for (I=0;I<Size;I++)
    if (LenTab[I]!=0)
      Dec->DecodeNum[TmpPos[LenTab[I] & 0xF]++]=I;
  Dec->MaxNum=Size;
}

RSTATIC void RATTRIB UnpWriteData(tRAR *data, byte *Data,int Size)
{
    uint WriteSize;
    filesize LeftToWrite;

    if (data->WrittenFileSize >= data->DestUnpSize) return;
    WriteSize = Size;

    LeftToWrite = data->DestUnpSize - data->WrittenFileSize;

    if (WriteSize > LeftToWrite) WriteSize = (int)(LeftToWrite);

    data->wr_crc32 = CRC32(data->wr_crc32,Data,WriteSize);
    if ( !data->skip )
    {
        FileWrite(data, Data, WriteSize);
    }

    data->WrittenFileSize += Size;
}

RSTATIC void RATTRIB UnpWriteArea(tRAR *data, unsigned int StartPtr,unsigned int EndPtr)
{
    if ( EndPtr != StartPtr) data->UnpSomeRead = TRUE;

    if ( EndPtr < StartPtr)
    {
        UnpWriteData(data,&data->Window[StartPtr],(0-StartPtr) & MAXWINMASK);
        UnpWriteData(data,data->Window,EndPtr);
        data->UnpAllBuf = TRUE;
    }
    else
    {
        UnpWriteData(data,&data->Window[StartPtr],EndPtr-StartPtr);
    }
}

RSTATIC void RATTRIB UnpExecuteCode(tRAR *data, tSTACK_FILTER *filter)
{
    if ( filter->GlobalData.count > 0 )
    {
        filter->InitR[6] = (uint)(data->WrittenFileSize);
        VM_SetDwordValue((uint *)filter->GlobalData.ptr+0x24/4,(data->WrittenFileSize));
        VM_SetDwordValue((uint *)filter->GlobalData.ptr+0x28/4,0);
        //VM_SetDwordValue((uint *)filter->GlobalData.ptr+0x28/4,(data->WrittenFileSize>>32));
        VM_Execute(data,filter);
    }
}

RSTATIC void RATTRIB UnpWriteBuf(tRAR *data)
{
    uint WrittenBorder;
    uint WriteSize;
    uint I;

    WrittenBorder = data->WrPtr;
    WriteSize = (data->UnpPtr-WrittenBorder) & MAXWINMASK;

    for ( I=0 ; I < data->PrgStack.count ; I++ )
    {
        tSTACK_FILTER *flt;

        flt = *ArrayGet(data->PrgStack,I,tSTACK_FILTER*);

        if ( flt != NULL )
        {
            if ( flt->NextWindow )
            {
                flt->NextWindow = FALSE;
            }
            else
            {
                uint BlockStart;
                uint BlockLength;

                BlockStart  = flt->BlockStart ;
                BlockLength = flt->BlockLength;

                if ( ((BlockStart-WrittenBorder) & MAXWINMASK) < WriteSize )
                {
                    if ( WrittenBorder != BlockStart )
                    {
                        UnpWriteArea(data, WrittenBorder, BlockStart);
                        WrittenBorder = BlockStart;
                        WriteSize = (data->UnpPtr-WrittenBorder) & MAXWINMASK;
                    }

                    if ( BlockLength <= WriteSize )
                    {
                        uint BlockEnd;
                        byte *FilteredData;
                        uint  FilteredDataSize;

                        BlockEnd = (BlockStart + BlockLength) & MAXWINMASK;

                        if ( BlockStart < BlockEnd || BlockEnd == 0 )
                        {
                            VM_SetMemory(data, 0, data->Window+BlockStart, BlockLength);
                        }
                        else
                        {
                            uint FirstPartLength;
                            FirstPartLength = MAXWINSIZE-BlockStart;
                            VM_SetMemory(data, 0, data->Window+BlockStart, FirstPartLength);
                            VM_SetMemory(data, FirstPartLength, data->Window, BlockEnd);

                        }
                        //Printf("Executing StackFilter %02X, %02X commands" crlf, I, flt->CmdCount);
                        UnpExecuteCode(data, flt);

                        FilteredData     = flt->FilteredData    ;
                        FilteredDataSize = flt->FilteredDataSize;

                        MemFree(data,flt);
                        *ArrayGet(data->PrgStack,I,tSTACK_FILTER*) = NULL;

                        while ( (I+1) < data->PrgStack.count )
                        {
                            tSTACK_FILTER *NextFilter;

                            NextFilter = *ArrayGet(data->PrgStack,I+1,tSTACK_FILTER*);

                            if (NextFilter == NULL ||
                                NextFilter->BlockStart  != BlockStart ||
                                NextFilter->BlockLength != FilteredDataSize)
                                break;
                            VM_SetMemory(data,0,FilteredData,FilteredDataSize);

                            //Printf("Executing StackFilter %02X, %02X commands" crlf, I+1, NextFilter->CmdCount);

                            UnpExecuteCode(data,NextFilter);
                            FilteredData     = NextFilter->FilteredData;
                            FilteredDataSize = NextFilter->FilteredDataSize;
                            I++;
                            MemFree(data,NextFilter);
                            *ArrayGet(data->PrgStack,I,tSTACK_FILTER*) = NULL;
                        }
                        data->wr_crc32 = CRC32(data->wr_crc32,FilteredData,FilteredDataSize);
                        if ( !data->skip )
                        {
                            FileWrite(data, FilteredData,FilteredDataSize);
                        }
                        data->UnpSomeRead = TRUE;
                        data->WrittenFileSize += FilteredDataSize;
                        WrittenBorder = BlockEnd;
                        WriteSize = (data->UnpPtr - WrittenBorder) & MAXWINMASK;
                    }
                    else
                    {
                        uint J;
                        for ( J = I ; J < data->PrgStack.count; J++ )
                        {
                            tSTACK_FILTER *flt;

                            flt = *ArrayGet(data->PrgStack,J,tSTACK_FILTER*);

                            if (flt!=NULL && flt->NextWindow)
                                flt->NextWindow=FALSE;
                        }
                        data->WrPtr = WrittenBorder;
                        return;
                    }
                }
            }
        }
    }

    UnpWriteArea(data,WrittenBorder,data->UnpPtr);
    data->WrPtr = data->UnpPtr;
}

RSTATIC int RATTRIB DecodeNumber(tINPUT *input, struct sDECODE *Dec)
{
  unsigned int Bits;
  unsigned int N;
  unsigned int BitField;

  BitField = BitGet(input) & 0xfffe;

  if (BitField<Dec->DecodeLen[8])
    if (BitField<Dec->DecodeLen[4])
      if (BitField<Dec->DecodeLen[2])
        if (BitField<Dec->DecodeLen[1])
          Bits=1;
        else
          Bits=2;
      else
        if (BitField<Dec->DecodeLen[3])
          Bits=3;
        else
          Bits=4;
    else
      if (BitField<Dec->DecodeLen[6])
        if (BitField<Dec->DecodeLen[5])
          Bits=5;
        else
          Bits=6;
      else
        if (BitField<Dec->DecodeLen[7])
          Bits=7;
        else
          Bits=8;
  else
    if (BitField<Dec->DecodeLen[12])
      if (BitField<Dec->DecodeLen[10])
        if (BitField<Dec->DecodeLen[9])
          Bits=9;
        else
          Bits=10;
      else
        if (BitField<Dec->DecodeLen[11])
          Bits=11;
        else
          Bits=12;
    else
      if (BitField<Dec->DecodeLen[14])
        if (BitField<Dec->DecodeLen[13])
          Bits=13;
        else
          Bits=14;
      else
        Bits=15;

  BitAdd(input,Bits);
  N=Dec->DecodePos[Bits]+((BitField-Dec->DecodeLen[Bits-1])>>(16-Bits));
  if (N>=Dec->MaxNum)
    N=0;
  return(Dec->DecodeNum[N]);
}



RSTATIC void RATTRIB UnpInitData(tRAR *data, rarbool Solid)
{
    if (!Solid)
    {
        data->TablesRead = FALSE;
        memset(data->OldDist,0,sizeof(data->OldDist));
        data->OldDistPtr=0;
        data->LastDist = 0;
        data->LastLength = 0;
        memset(data->UnpOldTable,0,sizeof(data->UnpOldTable));
        data->UnpPtr = 0;
        data->WrPtr = 0;
        data->PPMEscChar=2;

        VM_InitFilters(data);
    }

    data->WrittenFileSize=0;
    UnpInitData20(data, Solid);
}

RSTATIC rarbool RATTRIB ReadTables(tRAR *data, tINPUT *input)
{
    byte BitLength[BC29];
    unsigned char Table[HUFF_TABLE_SIZE29];
    unsigned int BitField;
    int I;

    BitAlign(input);      // align on byte boundary

    BitField = BitGet(input);

    if (BitField & 0x8000)
    {
        data->UnpBlockType = BLOCK_PPM;
        #ifdef RAR_LARGE
            return(PPM_DecodeInit(data,input,&data->PPMEscChar));
        #else
            Printf("RAR 3.0 Stopped: Memory limit exceed" crlf);
            return FALSE;
        #endif
    }

    data->UnpBlockType=BLOCK_LZ;

    data->PrevLowDist=0;
    data->LowDistRepCount=0;

    if (!(BitField & 0x4000))
      memset(data->UnpOldTable,0,sizeof(data->UnpOldTable));

    BitAdd(input, 2);

    for (I=0;I<BC29;I++)
    {
        int Length;

        Length = (BitGet(input) >> 12);

        BitAdd(input, 4);

        if (Length==15)
        {
            int ZeroCount;

            ZeroCount =(byte)(BitGet(input) >> 12);
            BitAdd(input, 4);
            if (ZeroCount==0)
            {
                BitLength[I]=15;
            }
            else
            {
                ZeroCount+=2;
                while (ZeroCount-- > 0 && I<sizeof(BitLength)/sizeof(BitLength[0]))
                    BitLength[I++]=0;
                I--;
            }
        }
        else
        {
            BitLength[I]=(byte)Length;
        }
    }

    MakeDecodeTables(BitLength,(struct sDECODE *)&data->BD,BC29);

    for (I=0;I<HUFF_TABLE_SIZE29;)
    {
        int Number;

        Number = DecodeNumber(input,(struct sDECODE *)&data->BD);

        if (Number<16)
        {
            Table[I] = (byte)((Number+data->UnpOldTable[I]) & 0xf);
            I++;
        }
        else
        {
            if (Number<18)
            {
                int N;
                if (Number==16)
                {
                    N=(BitGet(input) >> 13)+3;
                    BitAdd(input, 3);
                }
                else
                {
                    N=(BitGet(input) >> 9)+11;
                    BitAdd(input, 7);
                }
                while (N-- > 0 && I<HUFF_TABLE_SIZE29)
                {
                    Table[I]=Table[I-1];
                    I++;
                }
            }
            else
            {
                int N;
                if (Number==18)
                {
                    N=(BitGet(input) >> 13)+3;
                    BitAdd(input, 3);
                }
                else
                {
                    N=(BitGet(input) >> 9)+11;
                    BitAdd(input, 7);
                }
                while (N-- > 0 && I<HUFF_TABLE_SIZE29) Table[I++]=0;
            }
        }
    }
    data->TablesRead = TRUE;
    MakeDecodeTables(&Table[0],(struct sDECODE *)&data->LD,NC29);
    MakeDecodeTables(&Table[NC29],(struct sDECODE *)&data->DD,DC29);
    MakeDecodeTables(&Table[NC29+DC29],(struct sDECODE *)&data->LDD,LDC29);
    MakeDecodeTables(&Table[NC29+DC29+LDC29],(struct sDECODE *)&data->RD,RC29);
    memcpy(data->UnpOldTable,Table,sizeof(data->UnpOldTable));
    return(TRUE);
}

RSTATIC rarbool RATTRIB ReadEndOfBlock(tRAR *data, tINPUT *input)
{
    uint BitField;
    rarbool NewTable;
    rarbool NewFile;

    BitField = BitGet(input);
    NewFile = FALSE;

    if (BitField & 0x8000)
    {
        NewTable = TRUE;
        BitAdd(input,1);
    }
    else
    {
        NewFile = TRUE;
        NewTable = (rarbool)((BitField & 0x4000) != 0);
        BitAdd(input,2);
    }
    data->TablesRead = (rarbool)(!NewTable);
    return (rarbool)(!(NewFile || (NewTable && !ReadTables(data, input))));
}

RSTATIC void RATTRIB CopyString(tRAR *data, uint len, uint dist)
{
  
  tBYTE* Window;
  tUINT  SrcPtr;
  tUINT  UnpPtr;
  tUINT  WinMask;
  tUINT  WinSize;

  if ( !dist || !len || len > data->WindowSize )
  {
    data->Error = cTRUE;
    return; // error
  }

  WinSize = data->WindowSize;
  WinMask = data->WindowMask;
  UnpPtr  = data->UnpPtr & WinMask;
  SrcPtr  = (UnpPtr - dist) & WinMask;
  Window  = data->Window;

  if ( (SrcPtr + len) < (WinSize - 260) && (UnpPtr + len) < (WinSize - 260) )
  {
    while ( len-- )
      Window[UnpPtr++] = Window[SrcPtr++];
  }
  else
  {
    while ( len-- )
      Window[UnpPtr++ & WinMask] = Window[SrcPtr++ & WinMask];
  }

  data->UnpPtr = (UnpPtr & WinMask);
}

RSTATIC void RATTRIB InsertOldDist(tRAR *data, unsigned int Distance)
{
    data->OldDist[3] = data->OldDist[2];
    data->OldDist[2] = data->OldDist[1];
    data->OldDist[1] = data->OldDist[0];
    data->OldDist[0] = Distance;
}


RSTATIC void RATTRIB InsertLastMatch(tRAR *data, unsigned int Length,unsigned int Distance)
{
    data->LastDist = Distance;
    data->LastLength = Length;
}

static const unsigned char LDecode[]={0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224};
static const unsigned char LBits[]=  {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5};
static const int DDecode[DC29] =
    {
        0,1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,
        768,1024,1536,2048,3072,4096,6144,8192,12288,16384,24576,
        32768,49152,65536,98304,131072,196608,262144,327680,393216,
        458752,524288,589824,655360,720896,786432,851968,917504,
        983040,1048576,1310720,1572864,1835008,2097152,2359296,
        2621440,2883584,3145728,3407872,3670016,3932160
    };
static const byte DBits[DC29] =
    {
        0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,
        12,12,13,13,14,14,15,15,16,16,16,16,16,16,16,16,16,16,16,
        16,16,16,18,18,18,18,18,18,18,18,18,18,18,18
    };
static const unsigned char SDDecode[]={0,4,8,16,32,64,128,192};
static const unsigned char SDBits[]=  {2,2,3, 4, 5, 6,  6,  6};

RSTATIC void RATTRIB Unpack29(tRAR *data, tINPUT *input,rarbool Solid)
{
    int  run;
    uint Bits;
    uint Mask;

    data->Error = cFALSE;
    data->FileExtracted = cTRUE;
    UnpInitData(data, Solid);

    if ((!Solid || !data->TablesRead) && !ReadTables(data,input))
    {
        return ;
    }

    run  = 1;
    Mask = data->WindowMask;
    while ( run && !input->eof )
    {
        data->UnpPtr &= data->WindowMask;
        if ( data->Error )
          break;

        if (((data->WrPtr-data->UnpPtr) & Mask)<260 && data->WrPtr != data->UnpPtr)
        {
            UnpWriteBuf(data);

            if ( data->WrittenFileSize > data->DestUnpSize) return;

            if ( data->Suspended )
            {
                data->FileExtracted = FALSE;
                return;
            }
        }



        if ( data->UnpBlockType == BLOCK_PPM )
        {
            #ifndef RAR_LARGE
            return ;
            #else
            int Ch;

            //Printf("Switching to PPM" crlf);

            if ( (Ch = PPM_DecodeChar(data, input)) == -1 )
            {
                break;
            }

            if ( Ch == data->PPMEscChar )
            {
                int NextCh;

                NextCh = PPM_DecodeChar(data, input);

                switch ( NextCh )
                {
                    case -1:
                    case  2:
                        run = 0;
                        break;

                    case  0:
                        run = ReadTables(data, input);
                        break;

                    case  3:
                        {
                            tINPUT inp;

                            //Printf("Loading VM code" crlf);

                            InputInitSrc(&inp,input,RarReadBytePPM);

                            run  = VM_ReadCode(data, &inp);
                        }
                        break;

                    case  4:
                        {
                            uint Distance;
                            uint Length = 0;
                            int Ch;
                            int i;

                            Distance = 0;

                            for ( i=0;i<4;i++ )
                            {
                                Ch = PPM_DecodeChar(data, input);
                                if ( Ch == -1 )
                                {
                                    run = 0;
                                    break;
                                }
                                else
                                {
                                    if ( i == 3 )
                                    {
                                        Length = (byte)(Ch);
                                    }
                                    else
                                    {
                                        Distance = (Distance << 8) | (byte)(Ch);
                                    }
                                }
                            }

                            if ( run )
                            {
                                CopyString(data,Length+32,Distance+2);
                            }
                        }
                        break;

                    case  5:
                        {
                            int Length;
                            Length = PPM_DecodeChar(data, input);
                            if ( Length == -1 )
                            {
                                run = 0;
                            }
                            else
                            {
                                CopyString(data,Length+4,1);
                            }
                        }
                        break;

                    default:
                        data->Window[data->UnpPtr++] = (byte)Ch;
                        break;
                }

            }
            else
            {
                data->Window[data->UnpPtr++] = (byte)Ch;
            }
            #endif
        }
        else
        {
            int Number;
          
            Number = DecodeNumber(input,(struct sDECODE *)(&data->LD));

            if ( Number < 256)
            {
                data->Window[data->UnpPtr++] = (byte)Number;
            }
            else if ( Number >= 271 )
            {
                int Length;
                int DistNumber;
                uint Distance;

                Number -= 271;
                Length = LDecode[Number] + 3;
                Bits = LBits[Number];
                if ( Bits > 0 )
                {
                    Length += BitGet(input) >> (16-Bits);
                    BitAdd(input,Bits);
                }

                DistNumber = DecodeNumber(input,(struct sDECODE *)(&data->DD));
                Distance = DDecode[DistNumber]+1;

                Bits = DBits[DistNumber];

                if ( Bits > 0 )
                {
                    if (DistNumber>9)
                    {
                        if (Bits>4)
                        {
                            Distance+=((BitGet(input)>>(20-Bits))<<4);
                            BitAdd(input, Bits-4);
                        }

                        if ( data->LowDistRepCount > 0)
                        {
                            data->LowDistRepCount--;
                            Distance += data->PrevLowDist;
                        }
                        else
                        {
                            int LowDist;
                            LowDist = DecodeNumber(input,(struct sDECODE *)&data->LDD);

                            if ( LowDist == 16)
                            {
                                data->LowDistRepCount=LOW_DIST_REP_COUNT-1;
                                Distance += data->PrevLowDist;
                            }
                            else
                            {
                                Distance+=LowDist;
                                data->PrevLowDist=LowDist;
                            }
                        }
                    }
                    else
                    {
                        Distance+=BitGet(input)>>(16-Bits);
                        BitAdd(input, Bits);
                    }
                }

                if (Distance>=0x2000)
                {
                    Length++;
                    if ( Distance >= 0x40000 ) Length++;
                }

                InsertOldDist(data, Distance);
                InsertLastMatch(data, Length, Distance);
                CopyString(data, Length, Distance);
            }
            else if ( Number == 256 )
            {
                if ( !ReadEndOfBlock(data, input) )
                {
                    break;
                }
            }
            else if ( Number == 257 )
            {
                tINPUT inp;

                //Printf("Loading VM code" crlf);

                InputInitSrc(&inp,input,RarReadByteStream);

                if ( !VM_ReadCode(data, &inp) )
                {
                    break;
                }
                continue;
            }
            else if ( Number == 258 )
            {
                CopyString(data,data->LastLength,data->LastDist);
            }
            else if ( Number < 263 )
            {
                int DistNum;
                uint Distance;
                int I;
                int LengthNumber;
                int Length;

                DistNum = Number-259;
                Distance = data->OldDist[DistNum];

                for (I=DistNum;I>0;I--) data->OldDist[I] = data->OldDist[I-1];
                data->OldDist[0]=Distance;

                LengthNumber = DecodeNumber(input,(struct sDECODE *)&data->RD);
                Length = LDecode[LengthNumber]+2;
                if ((Bits=LBits[LengthNumber])>0)
                {
                    Length+=BitGet(input)>>(16-Bits);
                    BitAdd(input,Bits);
                }
                InsertLastMatch(data,Length,Distance);
                CopyString(data,Length,Distance);
            }
            else if ( Number < 272 )
            {
                unsigned int Distance;

                Number -= 263;
                Distance = SDDecode[Number]+1;

                if ((Bits=SDBits[Number])>0)
                {
                    Distance += BitGet(input)>>(16-Bits);
                    BitAdd(input,Bits);
                }

                InsertOldDist(data,Distance);
                InsertLastMatch(data,2,Distance);
                CopyString(data,2,Distance);
            }

        }

    }
    UnpWriteBuf(data);
}

