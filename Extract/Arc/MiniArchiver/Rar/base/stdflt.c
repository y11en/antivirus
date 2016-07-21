#include <Prague/prague.h>

#include "const.h"
#include "proto.h"


#ifdef VM_STANDARDFILTERS
RSTATIC uint RATTRIB FilterItanium_GetBits(byte *Data,int BitPos,int BitCount);
RSTATIC void RATTRIB FilterItanium_SetBits(byte *Data,unsigned int BitField,int BitPos, int BitCount);

#ifdef VM_DEBUG
RSTATIC byte * RATTRIB VM_FilterName(uint num)
{
    byte *name;
    switch ( num )
    {
        case VMSF_NONE   :  name = (byte*) "VMSF_NONE   "; break;
        case VMSF_E8     :  name = (byte*) "VMSF_E8     "; break;
        case VMSF_E8E9   :  name = (byte*) "VMSF_E8E9   "; break;
        case VMSF_ITANIUM:  name = (byte*) "VMSF_ITANIUM"; break;
        case VMSF_DELTA  :  name = (byte*) "VMSF_DELTA  "; break;
        case VMSF_RGB    :  name = (byte*) "VMSF_RGB    "; break;
        case VMSF_AUDIO  :  name = (byte*) "VMSF_AUDIO  "; break;
        case VMSF_UPCASE :  name = (byte*) "VMSF_UPCASE "; break;
        default          :  name = (byte*) "UNKNOWN     "; break;
    }
    return name;
}
#endif // VM_DEBUG

RSTATIC uint RATTRIB VM_StandardFilterCheck(tINPUT *input)
{
    int I;
    uint type;
    static struct StandardFilterSignature
    {
        int Length;
        uint CRC;
        byte Type;
    } StdList[]=
    {
    	{	53,		0xad576887, VMSF_E8     	},
		{	57,		0x3cd7e57e, VMSF_E8E9   	},
		{	120,	0x3769893f, VMSF_ITANIUM	},
		{	29,		0x0e06077d, VMSF_DELTA  	},
		{	149,	0x1c2c5dc8, VMSF_RGB    	},
		{	216,	0xbc85e701, VMSF_AUDIO  	},
		{	40,		0x46b9c560, VMSF_UPCASE		}
    };

    type = VMSF_NONE;

    for ( I=0;I<sizeof(StdList)/sizeof(StdList[0]);I++)
    {
        if (StdList[I].CRC == ~input->rd_crc /*&& StdList[I].Length==CodeSize*/)
        {
            type = StdList[I].Type;
            break;
        }
    }
    //Printf("Standard filter is %02X \"%s\"" crlf, type, VM_FilterName(type));
    return type;
}

RSTATIC void RATTRIB VM_StandardFilterExecute(tVM *vm, uint FilterType)
{
    switch(FilterType)
    {
        case VMSF_E8:
        case VMSF_E8E9:
        {
            int FileSize;
            byte *Data     ;
            uint DataSize  ;
            uint FileOffset;
            byte CmpByte2;
            uint CurPos;

            Data       = vm->Mem;
            DataSize   = vm->R[4];
            FileOffset = vm->R[6];

            if (DataSize>=VM_GLOBALMEMADDR) break;
            if (DataSize<4) break;

            FileSize = 0x1000000;
            CmpByte2 = (byte)((FilterType == VMSF_E8E9) ? 0xe9:0xe8);

            for (CurPos = 0; CurPos < DataSize-4 ;)
            {
              byte CurByte=*(Data++);
              CurPos++;
              if (CurByte==0xe8 || CurByte==CmpByte2)
              {
                long Offset=CurPos+FileOffset;
                long Addr = VM_GetDwordValue(Data);
                if (Addr<0)
                {
                  if (Addr+Offset>=0)
                    VM_SetDwordValue(Data,Addr+FileSize);
                }
                else
                  if (Addr<FileSize)
                    VM_SetDwordValue(Data,Addr-Offset);
                Data+=4;
                CurPos+=4;
              }
            }
          }
          break;

        case VMSF_ITANIUM:
          {
            byte *Data = vm->Mem;
            uint  DataSize = vm->R[4];
            uint  FileOffset = vm->R[6];
            uint  CurPos;

            if (DataSize>=VM_GLOBALMEMADDR) break;

            CurPos = 0;

            FileOffset >>= 4;

            while (CurPos<DataSize-21)
            {
              int Byte=(Data[0]&0x1f)-0x10;
              if (Byte>=0)
              {
                static byte Masks[16]={4,4,6,6,0,0,7,7,4,4,0,0,4,4,0,0};
                byte CmdMask=Masks[Byte];
                if (CmdMask!=0)
                {
                    int I;
                    for (I=0;I<=2;I++)
                    {
                        if (CmdMask & (1<<I))
                        {
                            int StartPos=I*41+5;
                            int OpType=FilterItanium_GetBits(Data,StartPos+37,4);
                            if (OpType==5)
                            {
                                int Offset=FilterItanium_GetBits(Data,StartPos+13,20);
                                FilterItanium_SetBits(Data,(Offset-FileOffset)&0xfffff,StartPos+13,20);
                            }
                        }
                    }
                }
              }
              Data+=16;
              CurPos+=16;
              FileOffset++;
            }
          }
          break;
        case VMSF_DELTA:
            {
                int CurChannel;
                int DataSize=vm->R[4];
                int Channels=vm->R[0];
                int SrcPos=0;
                int Border=DataSize*2;

                VM_SetDwordValue(&vm->Mem[VM_GLOBALMEMADDR+0x20],DataSize);

                if (DataSize>=VM_GLOBALMEMADDR/2) break;

                for (CurChannel=0;CurChannel<Channels;CurChannel++)
                {
                    int DestPos;
                    byte PrevByte=0;
                    for (DestPos=DataSize+CurChannel;DestPos<Border;DestPos+=Channels)
                        vm->Mem[DestPos]=(PrevByte = (byte)(PrevByte - vm->Mem[SrcPos++]));
                }
          }
          break;
        case VMSF_RGB:
          {
            int DataSize = vm->R[4];
            int Width = vm->R[0]-3;
            int PosR=vm->R[1];
            byte *SrcData=vm->Mem;
            byte *DestData=SrcData+DataSize;
            int Channels=3;
            int CurChannel;
            int I;
            int Border;

            VM_SetDwordValue(&vm->Mem[VM_GLOBALMEMADDR+0x20],DataSize);
            if (DataSize>=VM_GLOBALMEMADDR/2)
              break;
            for (CurChannel=0;CurChannel<Channels;CurChannel++)
            {
                unsigned int PrevByte=0;

              for (I=CurChannel;I<DataSize;I+=Channels)
              {
                unsigned int Predicted;
                int UpperPos=I-Width;
                if (UpperPos>=3)
                {
                  byte *UpperData=DestData+UpperPos;
                  unsigned int UpperByte=*UpperData;
                  unsigned int UpperLeftByte=*(UpperData-3);
                  int pa;
                  int pb;
                  int pc;

                  Predicted=PrevByte+UpperByte-UpperLeftByte;
                  pa=abs((int)(Predicted-PrevByte));
                  pb=abs((int)(Predicted-UpperByte));
                  pc=abs((int)(Predicted-UpperLeftByte));
                  if (pa<=pb && pa<=pc)
                    Predicted=PrevByte;
                  else
                    if (pb<=pc)
                      Predicted=UpperByte;
                    else
                      Predicted=UpperLeftByte;
                }
                else
                  Predicted=PrevByte;
                PrevByte=(byte)(Predicted-*(SrcData++));
                DestData[I]=(byte)(PrevByte);
              }
            }
            for (I=PosR,Border=DataSize-2;I<Border;I+=3)
            {
              byte G = DestData[I+1];
              DestData[I]   = (byte)(DestData[I]+G);
              DestData[I+2] = (byte)(DestData[I+2]+G);
            }
          }
          break;
        case VMSF_AUDIO:
          {
            int DataSize=vm->R[4];
            int Channels=vm->R[0];
            byte *SrcData=vm->Mem;
            byte *DestData=SrcData+DataSize;
            int CurChannel;
            int I;
            int ByteCount;

            VM_SetDwordValue(&vm->Mem[VM_GLOBALMEMADDR+0x20],DataSize);
            if (DataSize>=VM_GLOBALMEMADDR/2)
              break;
            for (CurChannel=0;CurChannel<Channels;CurChannel++)
            {
              unsigned int PrevByte=0,PrevDelta=0,Dif[7];
              int D1=0,D2=0,D3;
              int K1=0,K2=0,K3=0;
              memset(Dif,0,sizeof(Dif));

              for (I=CurChannel,ByteCount=0;I<DataSize;I+=Channels,ByteCount++)
              {
                uint Predicted;
                uint CurByte;
                int D;

                D3=D2;
                D2=PrevDelta-D1;
                D1=PrevDelta;

                Predicted=8*PrevByte+K1*D1+K2*D2+K3*D3;
                Predicted=(Predicted>>3) & 0xff;

                CurByte=*(SrcData++);

                Predicted-=CurByte;
                DestData[I]=(byte)Predicted;
                PrevDelta=(signed char)(Predicted-PrevByte);
                PrevByte=Predicted;

                D=((signed char)CurByte)<<3;

                Dif[0]+=abs(D);
                Dif[1]+=abs(D-D1);
                Dif[2]+=abs(D+D1);
                Dif[3]+=abs(D-D2);
                Dif[4]+=abs(D+D2);
                Dif[5]+=abs(D-D3);
                Dif[6]+=abs(D+D3);

                if ((ByteCount & 0x1f)==0)
                {
                  uint MinDif=Dif[0];
                  uint NumMinDif=0;
                  int J;

                  Dif[0]=0;
                  for (J=1;J<sizeof(Dif)/sizeof(Dif[0]);J++)
                  {
                    if (Dif[J]<MinDif)
                    {
                      MinDif=Dif[J];
                      NumMinDif=J;
                    }
                    Dif[J]=0;
                  }
                  switch(NumMinDif)
                  {
                    case 1: if (K1>=-16) K1--; break;
                    case 2: if (K1 < 16) K1++; break;
                    case 3: if (K2>=-16) K2--; break;
                    case 4: if (K2 < 16) K2++; break;
                    case 5: if (K3>=-16) K3--; break;
                    case 6: if (K3 < 16) K3++; break;
                  }
                }
              }
            }
          }
          break;

        case VMSF_UPCASE:
          {
            int DataSize=vm->R[4],SrcPos=0,DestPos=DataSize;
            if (DataSize>=VM_GLOBALMEMADDR/2)
              break;
            while (SrcPos<DataSize)
            {
              byte CurByte=vm->Mem[SrcPos++];
              if (CurByte==2 && (CurByte=vm->Mem[SrcPos++])!=2)
                CurByte-=32;
              vm->Mem[DestPos++]=CurByte;
            }
            VM_SetDwordValue(&vm->Mem[VM_GLOBALMEMADDR+0x1c],DestPos-DataSize);
            VM_SetDwordValue(&vm->Mem[VM_GLOBALMEMADDR+0x20],DataSize);
          }
          break;
      }
    }


RSTATIC uint RATTRIB FilterItanium_GetBits(byte *Data,int BitPos,int BitCount)
{
    int  InAddr = BitPos / 8;
    int  InBit  = BitPos & 7;
    uint BitField = (uint)Data[InAddr++];
    BitField |= (uint)Data[InAddr++] << 8;
    BitField |= (uint)Data[InAddr++] << 16;
    BitField |= (uint)Data[InAddr] << 24;
    BitField >>= InBit;
    return (BitField & (0xffffffff>>(32-BitCount)));
}

RSTATIC void RATTRIB FilterItanium_SetBits(byte *Data,unsigned int BitField,int BitPos, int BitCount)
{
    int I;
    int InAddr=BitPos/8;
    int InBit=BitPos&7;
    unsigned int AndMask=0xffffffff>>(32-BitCount);
    AndMask=~(AndMask<<InBit);

    BitField<<=InBit;

    for (I=0;I<4;I++)
    {
        Data[InAddr+I] &= AndMask;
        Data[InAddr+I] |= BitField;
        AndMask = (AndMask>>8)|0xff000000;
        BitField >>= 8;
    }
}
#endif
