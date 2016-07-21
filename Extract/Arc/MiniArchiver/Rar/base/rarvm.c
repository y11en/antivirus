#include <Prague/prague.h>

#include "const.h"
#include "proto.h"


#ifdef VM_DEBUG
RSTATIC byte * RATTRIB VM_OpCodeName(byte OpCode)
{
    byte *name;
    switch ( OpCode )
    {
        case VM_MOV     :   name = (byte*) "VM_MOV     "; break;
        case VM_CMP     :   name = (byte*) "VM_CMP     "; break;
        case VM_ADD     :   name = (byte*) "VM_ADD     "; break;
        case VM_SUB     :   name = (byte*) "VM_SUB     "; break;
        case VM_JZ      :   name = (byte*) "VM_JZ      "; break;
        case VM_JNZ     :   name = (byte*) "VM_JNZ     "; break;
        case VM_INC     :   name = (byte*) "VM_INC     "; break;
        case VM_DEC     :   name = (byte*) "VM_DEC     "; break;
        case VM_JMP     :   name = (byte*) "VM_JMP     "; break;
        case VM_XOR     :   name = (byte*) "VM_XOR     "; break;
        case VM_AND     :   name = (byte*) "VM_AND     "; break;
        case VM_OR      :   name = (byte*) "VM_OR      "; break;
        case VM_TEST    :   name = (byte*) "VM_TEST    "; break;
        case VM_JS      :   name = (byte*) "VM_JS      "; break;
        case VM_JNS     :   name = (byte*) "VM_JNS     "; break;
        case VM_JB      :   name = (byte*) "VM_JB      "; break;
        case VM_JBE     :   name = (byte*) "VM_JBE     "; break;
        case VM_JA      :   name = (byte*) "VM_JA      "; break;
        case VM_JAE     :   name = (byte*) "VM_JAE     "; break;
        case VM_PUSH    :   name = (byte*) "VM_PUSH    "; break;
        case VM_POP     :   name = (byte*) "VM_POP     "; break;
        case VM_CALL    :   name = (byte*) "VM_CALL    "; break;
        case VM_RET     :   name = (byte*) "VM_RET     "; break;
        case VM_NOT     :   name = (byte*) "VM_NOT     "; break;
        case VM_SHL     :   name = (byte*) "VM_SHL     "; break;
        case VM_SHR     :   name = (byte*) "VM_SHR     "; break;
        case VM_SAR     :   name = (byte*) "VM_SAR     "; break;
        case VM_NEG     :   name = (byte*) "VM_NEG     "; break;
        case VM_PUSHA   :   name = (byte*) "VM_PUSHA   "; break;
        case VM_POPA    :   name = (byte*) "VM_POPA    "; break;
        case VM_PUSHF   :   name = (byte*) "VM_PUSHF   "; break;
        case VM_POPF    :   name = (byte*) "VM_POPF    "; break;
        case VM_MOVZX   :   name = (byte*) "VM_MOVZX   "; break;
        case VM_MOVSX   :   name = (byte*) "VM_MOVSX   "; break;
        case VM_XCHG    :   name = (byte*) "VM_XCHG    "; break;
        case VM_MUL     :   name = (byte*) "VM_MUL     "; break;
        case VM_DIV     :   name = (byte*) "VM_DIV     "; break;
        case VM_ADC     :   name = (byte*) "VM_ADC     "; break;
        case VM_SBB     :   name = (byte*) "VM_SBB     "; break;
        case VM_PRINT   :   name = (byte*) "VM_PRINT   "; break;
        #ifdef VM_OPTIMIZE
        case VM_MOVB    :   name = (byte*) "VM_MOVB  * "; break;
        case VM_MOVD    :   name = (byte*) "VM_MOVD  * "; break;
        case VM_CMPB    :   name = (byte*) "VM_CMPB  * "; break;
        case VM_CMPD    :   name = (byte*) "VM_CMPD  * "; break;
        case VM_ADDB    :   name = (byte*) "VM_ADDB  * "; break;
        case VM_ADDD    :   name = (byte*) "VM_ADDD  * "; break;
        case VM_SUBB    :   name = (byte*) "VM_SUBB  * "; break;
        case VM_SUBD    :   name = (byte*) "VM_SUBD  * "; break;
        case VM_INCB    :   name = (byte*) "VM_INCB  * "; break;
        case VM_INCD    :   name = (byte*) "VM_INCD  * "; break;
        case VM_DECB    :   name = (byte*) "VM_DECB  * "; break;
        case VM_DECD    :   name = (byte*) "VM_DECD  * "; break;
        case VM_NEGB    :   name = (byte*) "VM_NEGB  * "; break;
        case VM_NEGD    :   name = (byte*) "VM_NEGD  * "; break;
        #endif
        case VM_STANDARD:   name = (byte*) "VM_STANDARD"; break;
        default:            name = (byte*) "UNKNOWN    "; break;
    }
    return name;
}

RSTATIC void RATTRIB VM_OpName(tOPERAND *CmdOp)
{
    switch ( CmdOp->Type )
    {
        case VM_OPREG   :
            Printf("R%01X",CmdOp->Data);
            break;

        case VM_OPINT   :
            Printf("0x%08X", CmdOp->Data);
            break;

        case VM_OPMEMBASE:
            Printf("[%08X]" ,CmdOp->Base);
            break;

        case VM_OPMEMREG:
            Printf("%08X[R%01X]",CmdOp->Base, CmdOp->Data);
            break;

        case VM_OPNONE:
            break;

        default         :
            Printf("UNKNOWN");
            ;
    }
}

RSTATIC void RATTRIB VM_CmdDump(tCOMMAND *cmd)
{
    Printf(" %02X %s", cmd->OpCode, VM_OpCodeName(cmd->OpCode));

    if ( cmd->OpCode != VM_STANDARD )
    {
        VM_OpName(&cmd->Op1);

        if ( cmd->Op2.Type != VM_OPNONE)
        {
            Printf(", ");
            VM_OpName(&cmd->Op2);
        }
    }
    else
    {
        #ifdef   VM_STANDARDFILTERS
        Printf(" %s",VM_FilterName(cmd->Op1.Data));
        #endif
    }

    Printf("" crlf);
}

RSTATIC void RATTRIB VM_PrgDump(tFILTER *filter)
{
    tCOMMAND *cmd;
    uint count;
    uint offset;

    cmd = filter->Cmd.ptr;
    count = filter->Cmd.count;
    offset = 0;

    Printf("  Program %02X operators" crlf, count);

    while ( count-- )
    {
        Printf("    %04X:", offset);
        VM_CmdDump(cmd);
        offset++;
        cmd++;
    }
}
#endif /* VM_DEBUG */

static byte VM_CmdFlags[]=
{
    /* VM_MOV   */ VMCF_OP2 | VMCF_byteMODE                                ,
    /* VM_CMP   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_ADD   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_SUB   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_JZ    */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_JNZ   */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_INC   */ VMCF_OP1 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_DEC   */ VMCF_OP1 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_JMP   */ VMCF_OP1 | VMCF_JUMP                                    ,
    /* VM_XOR   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_AND   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_OR    */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_TEST  */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_JS    */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_JNS   */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_JB    */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_JBE   */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_JA    */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_JAE   */ VMCF_OP1 | VMCF_JUMP | VMCF_USEFLAGS                    ,
    /* VM_PUSH  */ VMCF_OP1                                                ,
    /* VM_POP   */ VMCF_OP1                                                ,
    /* VM_CALL  */ VMCF_OP1 | VMCF_PROC                                    ,
    /* VM_RET   */ VMCF_OP0 | VMCF_PROC                                    ,
    /* VM_NOT   */ VMCF_OP1 | VMCF_byteMODE                                ,
    /* VM_SHL   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_SHR   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_SAR   */ VMCF_OP2 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_NEG   */ VMCF_OP1 | VMCF_byteMODE | VMCF_CHFLAGS                 ,
    /* VM_PUSHA */ VMCF_OP0                                                ,
    /* VM_POPA  */ VMCF_OP0                                                ,
    /* VM_PUSHF */ VMCF_OP0 | VMCF_USEFLAGS                                ,
    /* VM_POPF  */ VMCF_OP0 | VMCF_CHFLAGS                                 ,
    /* VM_MOVZX */ VMCF_OP2                                                ,
    /* VM_MOVSX */ VMCF_OP2                                                ,
    /* VM_XCHG  */ VMCF_OP2 | VMCF_byteMODE                                ,
    /* VM_MUL   */ VMCF_OP2 | VMCF_byteMODE                                ,
    /* VM_DIV   */ VMCF_OP2 | VMCF_byteMODE                                ,
    /* VM_ADC   */ VMCF_OP2 | VMCF_byteMODE | VMCF_USEFLAGS | VMCF_CHFLAGS ,
    /* VM_SBB   */ VMCF_OP2 | VMCF_byteMODE | VMCF_USEFLAGS | VMCF_CHFLAGS ,
    /* VM_PRINT */ VMCF_OP0
};

RSTATIC void RATTRIB VM_FreeFilter(tRAR *data, tFILTER *filter)
{
    ArrayDelete(data,&filter->Cmd);
    ArrayDelete(data,&filter->StaticData);
    MemFree(data,filter);
}

RSTATIC tFILTER * RATTRIB VM_AllocFilter(tRAR *data)
{
    tFILTER *filter;
    filter = MemRealloc(data,NULL,NULL, sizeof(tFILTER),"FILTER");

    if ( filter != NULL )
    {
        tFILTER **flt;

        ArrayInit(&filter->Cmd,sizeof(tCOMMAND));
        ArrayInit(&filter->StaticData,sizeof(byte));

        flt = ArrayAdd(data, &data->Filters, 1, "Filters");

        if ( flt != NULL )
        {
            *flt = filter;
        }
        else
        {
            //Printf("Cannot extend Filters array" crlf);
            MemFree(data, filter);
            filter = NULL;
        }
    }
    else
    {
        //Printf("Cannot allocate new filter" crlf);
    }
    return filter;
}

RSTATIC tSTACK_FILTER * RATTRIB VM_AllocStackFilter(tRAR *data)
{
    tSTACK_FILTER *StackFilter;

    StackFilter = MemRealloc(data,NULL,NULL, sizeof(tSTACK_FILTER),"STACK_FILTER");

    if ( StackFilter == NULL )
    {
        //Printf("Cannot allocate new stack filter" crlf);
    }
    else
    {
        uint EmptyCount;
        uint cnt;
        tSTACK_FILTER **flt;

        ArrayInit(&StackFilter->StaticData, sizeof(byte));
        ArrayInit(&StackFilter->GlobalData, sizeof(byte));

        EmptyCount = 0;

        flt = ArrayGet(data->PrgStack, 0, tSTACK_FILTER*);
        cnt = data->PrgStack.count;

        while ( cnt-- )
        {
            *(flt-EmptyCount) = *flt;

            if ( *flt == NULL )
            {
                EmptyCount++;
            }

            if ( EmptyCount )
            {
                *flt = NULL;
            }

            flt++;

        }

        if ( EmptyCount == 0)
        {
            flt = ArrayAdd(data, &data->PrgStack, 1, "PrgStack");

            if ( flt == NULL )
            {
                //Printf("Cannot allocate new stack filter ptr" crlf);
                return FALSE;
            }
            //Printf(" new StackFilter %02X" crlf, data->PrgStack.count-1);
        }
        else
        {
            flt = ArrayGet(data->PrgStack, data->PrgStack.count-EmptyCount, tSTACK_FILTER*);
            //Printf(" use StackFilter %02X" crlf, data->PrgStack.count-EmptyCount);
        }
        *flt = StackFilter;
    }
    return StackFilter;
}

RSTATIC void RATTRIB VM_InitFilters(tRAR *data)
{
  // fast version
  //Printf("Init filters" crlf);
  MemReset(data);
  ArrayInit(&data->Filters, sizeof(tFILTER*));
  ArrayInit(&data->PrgStack,sizeof(tFILTER*));
  ArrayInit(&data->OldFilterLengths, sizeof(uint));
  data->LastFilter = 0;
}

RSTATIC uint RATTRIB VM_ReadData(tINPUT *input)
{
    uint Data;
    uint add;

    Data = BitGet(input);

    switch ( Data & 0xC000 )
    {
        case 0x0000:
            //BitAdd(input, 6);
            add = 6;
            Data >>= 10;
            Data &= 0x000F;
            break;

        case 0x4000:
            if ( (Data & 0x3C00) == 0 )
            {
                Data = 0xFFFFFF00 | ((Data >> 2) & 0xFF);
                //BitAdd(input, 14);
                add = 14;
            }
            else
            {
                Data = (Data >> 6) & 0xFF;
                //BitAdd(input, 10);
                add = 10;
            }
            break;

        case 0x8000:
            BitAdd(input, 2);
            Data = BitGet(input);
            //BitAdd(input, 16);
            add = 16;
            break;

        default:
            BitAdd(input, 2);
            Data = BitGet(input) << 16;
            BitAdd(input, 16);
            Data |= BitGet(input);
            //BitAdd(input, 16);
            add = 16;

    }
    BitAdd(input, add);
    return Data;
}

#ifdef MACHINE_IS_BIG_ENDIAN
// always function use
RSTATIC void RATTRIB VM_SetDwordValue(void *ptr, uint value)
{
    ((byte *)ptr)[0]=(byte)(value);
    ((byte *)ptr)[1]=(byte)(value >> 8);
    ((byte *)ptr)[2]=(byte)(value >> 16);
    ((byte *)ptr)[3]=(byte)(value >> 24);
}

RSTATIC uint RATTRIB VM_GetDwordValue(void *ptr)
{
    byte *B = ptr;
    return((uint)B[0]|((uint)B[1]<<8)|((uint)B[2]<<16)|((uint)B[3]<<24));
}
#endif /* MACHINE_IS_BIG_ENDIAN */

#ifndef VM_VALUE_DEFINE
RSTATIC void RATTRIB VM_SetByteValue(void *ptr, uint value)
{
    *((byte*)(ptr)) = (byte)(value);
}

#ifndef MACHINE_IS_BIG_ENDIAN
RSTATIC void RATTRIB VM_SetDwordValue(void *ptr, uint value)
{
    *((uint*)(ptr)) = value;
}

RSTATIC uint RATTRIB VM_GetDwordValue(void *ptr)
{
    return *((uint*)(ptr));
}
#endif /* MACHINE_IS_BIG_ENDIAN */

RSTATIC uint RATTRIB VM_GetByteValue(void *ptr)
{
    return *((byte*)(ptr));
}

RSTATIC uint RATTRIB VM_GetValue(rarbool ByteMode, void *ptr)
{
    return ByteMode ? VM_GetByteValue(ptr) : VM_GetDwordValue(ptr);
}

RSTATIC void RATTRIB VM_SetValue(rarbool ByteMode, void *ptr, uint value)
{
    ByteMode ? VM_SetByteValue(ptr, value) : VM_SetDwordValue(ptr, value);
}
#endif /* VM_VALUE_DEFINE */

RSTATIC void RATTRIB VM_DecodeArg(tINPUT *input, tOPERAND *Op, rarbool ByteMode)
{
    uint Data;

    Data = BitGet(input);

    if (Data & 0x8000)
    {
        // register operand
        Op->Type = VM_OPREG;
        Op->Data = (Data>>12)&7;
        //Op->Addr = &data->VM.R[Op->Data];
        BitAdd(input, 4);
    }
    else
    {
        if ( (Data & 0xc000) == 0)
        {
            // immediate operand
            Op->Type = VM_OPINT;

            if ( ByteMode )
            {
                Op->Data = (Data>>6) & 0xff;
                BitAdd(input, 10);
            }
            else
            {
                BitAdd(input, 2);
                Op->Data = VM_ReadData(input);
            }

            //Op->Addr = &Op->Data;
        }
        else
        {

            if ((Data & 0x2000)==0)
            {
                // register only
                Op->Type = VM_OPMEMREG;
                Op->Data = (Data>>10) & 7;
                //Op->Addr = &data->VM.R[Op->Data];
                Op->Base = 0;
                BitAdd(input, 6);
            }
            else
            {
                if ((Data & 0x1000)==0)
                {
                    // register + base
                    Op->Type = VM_OPMEMREG;
                    Op->Data = (Data>>9)&7;
                    //Op->Addr = &data->VM.R[Op->Data];
                    BitAdd(input, 7);
                }
                else
                {
                    // base only
                    Op->Type = VM_OPMEMBASE;
                    Op->Data=0;
                    //Op->Addr=&Op->Data;
                    BitAdd(input, 4);
                }
                Op->Base = VM_ReadData(input);
            }
        }
    }
}

#ifdef VM_OPTIMIZE
RSTATIC void RATTRIB VM_Optimize(tFILTER *filter)
{
    uint count;
    tCOMMAND *Cmd;

    count  = filter->Cmd.count;
    Cmd    = filter->Cmd.ptr;

    while ( count-- )
    {
        switch(Cmd->OpCode)
        {
            case VM_MOV:
                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_MOVB:VM_MOVD);
                break;

            case VM_CMP:
                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_CMPB:VM_CMPD);
                break;

            default:
                if ( (VM_CmdFlags[Cmd->OpCode] & VMCF_CHFLAGS ) != 0 )
                {
                    rarbool FlagRequired;
                    tCOMMAND *cmd1;
                    uint cnt1;

                    FlagRequired = FALSE;
                    cmd1 = Cmd+1;
                    cnt1 = count;

                    while ( cnt1-- )
                    {
                        int Flags = VM_CmdFlags[cmd1->OpCode];

                        if (Flags & (VMCF_JUMP|VMCF_PROC|VMCF_USEFLAGS))
                        {
                            FlagRequired = TRUE;
                            break;
                        }

                        if (Flags & VMCF_CHFLAGS)
                            break;

                        cmd1++;
                    }

                    if ( !FlagRequired )
                    {
                        switch(Cmd->OpCode)
                        {
                            case VM_ADD:
                                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_ADDB:VM_ADDD);
                                break;

                            case VM_SUB:
                                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_SUBB:VM_SUBD);
                                break;

                            case VM_INC:
                                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_INCB:VM_INCD);
                                break;

                            case VM_DEC:
                                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_DECB:VM_DECD);
                                break;
                            case VM_NEG:
                                Cmd->OpCode = (byte)(Cmd->ByteMode ? VM_NEGB:VM_NEGD);
                                break;
                        }

                    }
                }
        }
        Cmd++;
    }
}
#endif /* VM_OPTIMIZE */

RSTATIC rarbool RATTRIB VM_Prepare(struct sRAR *data, tINPUT *input, tFILTER *filter)
{
    uint DataFlag;
    tCOMMAND *CurCmd;

    //INPUT = input;
    //RarReadByte(input);     // skip crc sum
    BitGet(input);
    BitAdd(input,8);

    DataFlag = BitGet(input);
    BitAdd(input,1);

    if ( DataFlag & 0x8000 )
    {
        uint DataSize;
        byte *Data;

        DataSize = VM_ReadData(input)+1;

        Data = ArrayAdd(data, &filter->StaticData, DataSize, "Filter->StaticData");

        if ( Data == NULL )
        {
            return FALSE;
        }

        while ( DataSize-- && !input->eof )
        {
          // bugfix, 04.09.2003
          // *Data++ = RarReadByte(input);
          *Data++ = BitGet(input) >> 8; BitAdd(input, 8);           
        }
    }

    while ( !input->eof )
    {
        uint Data;
        uint OpNum;

        CurCmd = ArrayAdd(data, &filter->Cmd, 1, "Command");

        if ( CurCmd == NULL )
        {
            return FALSE;
        }

        Data = BitGet(input);
        //Printf("      Data = %04X" crlf, Data);

        // probably check here for eof and add new command also here

        if ( (Data & 0x8000) == 0 )
        {
            CurCmd->OpCode = (byte)(Data >> 12);
            BitAdd(input, 4);
        }
        else
        {
            CurCmd->OpCode = (byte)((Data >> 10) - 24);
            BitAdd(input, 6);
        }

        if (VM_CmdFlags[CurCmd->OpCode] & VMCF_byteMODE)
        {
            CurCmd->ByteMode = (rarbool)( (BitGet(input)>>15) != 0);
            BitAdd(input, 1);
        }
        else
        {
            CurCmd->ByteMode = 0;
        }

        CurCmd->Op1.Type = VM_OPNONE;
        CurCmd->Op2.Type = VM_OPNONE;

        OpNum = (VM_CmdFlags[CurCmd->OpCode] & VMCF_OPMASK);

        //CurCmd->Op1.Addr = &CurCmd->Op1.Data;
        //CurCmd->Op2.Addr = &CurCmd->Op2.Data;

        if ( OpNum > 0 )
        {
            VM_DecodeArg(input, &CurCmd->Op1, CurCmd->ByteMode);

            if (OpNum == 2)
            {
                VM_DecodeArg(input, &CurCmd->Op2, CurCmd->ByteMode);
            }
            else
            {
                if (CurCmd->Op1.Type==VM_OPINT && (VM_CmdFlags[CurCmd->OpCode]&(VMCF_JUMP|VMCF_PROC)))
                {
                    int Distance=CurCmd->Op1.Data;
                    if ( Distance >= 256 )
                    {
                        Distance-=256;
                    }
                    else
                    {
                        if (Distance >= 136)
                            Distance -= 264;
                        else
                            if (Distance>=16)
                                Distance-=8;
                            else
                                if (Distance>=8)
                                    Distance-=16;
                        Distance += filter->Cmd.count-1;  // warning!!!
                    }
                    CurCmd->Op1.Data=Distance;
                }
            }
        }
        //Printf("    %04X:", filter->Cmd.count-1);  VM_CmdDump(CurCmd);
    }
    #ifdef VM_STANDARDFILTERS
    {
        uint flttype;
        flttype = VM_StandardFilterCheck(input);
        if ( flttype != VMSF_NONE )
        {
            ArrayDelete(data, &filter->Cmd);
            CurCmd = ArrayAdd(data,&filter->Cmd,1, "StdFilter");
            if ( CurCmd == NULL )
            {
                return FALSE;
            }
            CurCmd->OpCode = VM_STANDARD;
            CurCmd->Op1.Data = flttype;
        }
    }
    #endif
    CurCmd = ArrayAdd(data,&filter->Cmd,1, "Command");

    if ( CurCmd == NULL )
    {
        return FALSE;
    }

    CurCmd->OpCode=VM_RET;

    CurCmd->Op1.Type = VM_OPNONE;
    CurCmd->Op2.Type = VM_OPNONE;
    
    //Printf("    %04X:", filter->Cmd.count-1);  VM_CmdDump(CurCmd);
    //Printf("    Program loaded: %02X commands" crlf crlf, filter->Cmd.count);
    //MemDump(data);
    //VM_PrgDump(filter);

    #ifdef VM_OPTIMIZE
        VM_Optimize(filter);
        #ifdef VM_DEBUG
            VM_PrgDump(filter);
        #endif
    #endif

    VM_PrepareOperands(&data->VM,filter);
    return TRUE;
}

RSTATIC rarbool RATTRIB VM_MemFix(struct sRAR *data, void *newptr, void *oldptr)
{
    //oldptr;
    data->VM.Mem = newptr;
    return TRUE;
}

RSTATIC rarbool RATTRIB VM_FilterFix(struct sRAR *data)
{
    uint cnt;
    if ( !ArrayFix(data, &data->OldFilterLengths) )
    {
      //Printf("Cannot fix OldFilterLengths" crlf);
        ArrayDump(&data->OldFilterLengths,"OldFilterLengths");
        return FALSE;
    }

    cnt = data->Filters.count;

    if ( cnt )
    {
        tFILTER **flt;

        if ( !ArrayFix(data, &data->Filters) )
        {
          //Printf("Cannot fix Filters" crlf);
            return FALSE;
        }

        flt = data->Filters.ptr;

        while ( cnt-- )
        {
            tFILTER *filter;
            if ( ( *flt = MemPtrFix(data,*flt) ) == NULL )
            {
              //Printf("Cannot fix Filter ptr" crlf);
                return FALSE;
            }

            filter = *flt;

            if ( !ArrayFix(data, &filter->Cmd) )
            {
              //Printf("Cannont fix Filter->Cmd" crlf);
                return FALSE;
            }

            if ( !ArrayFix(data, &filter->StaticData) )
            {
              //Printf("Cannont fix Filter->StaticData" crlf);
                return FALSE;
            }

            VM_PrepareOperands(&data->VM, filter);

            flt++;
        }
    }
    return TRUE;
}

RSTATIC rarbool RATTRIB VM_AddCode(struct sRAR *data, uint FirstByte, tINPUT *input)
{
    uint FilterPos;
    rarbool NewFilter;
    tFILTER *Filter;
    tSTACK_FILTER *StackFilter;
    uint BlockStart;
    uint cnt;
    uint *GlobalData;
    uint *ptr;

    if ( data->VM.Mem == NULL )
    {
        if ( (data->VM.Mem = RarAlloc(data, VM_MEMSIZE+4, VM_MemFix)) == NULL )
        {
            return FALSE;
        }
    }

    if ( FirstByte & 0x80 )
    {
        FilterPos = VM_ReadData(input);

        if ( FilterPos == 0 )
        {
            VM_InitFilters(data);
        }
        else
        {
            FilterPos--;
        }
    }
    else
    {
        FilterPos = data->LastFilter;
    }

    if ( FilterPos > data->Filters.count || FilterPos > data->OldFilterLengths.count )
    {
        return FALSE;
    }

    data->LastFilter = FilterPos;

    NewFilter = (rarbool)(FilterPos == data->Filters.count);

    if ( NewFilter )
    {
        //Printf("    Allocating filter %02X," , FilterPos);
        Filter = VM_AllocFilter(data);

        if ( Filter == NULL )
        {
            return FALSE;
        }

        if ( ArrayAdd(data, &data->OldFilterLengths, 1, "OldFilterLengths") == NULL )
        {
            //Printf("Cannot expand OldFilterLengths" crlf);
            return FALSE;
        }

        Filter->ExecCount = 0;
    }
    else
    {
        //Printf("    Using filter %02X," , FilterPos);
        Filter = *ArrayGet(data->Filters, FilterPos, tFILTER* );
        Filter->ExecCount++;
    }


    StackFilter = VM_AllocStackFilter(data);

    if ( StackFilter == NULL )
    {
        return FALSE;
    }

    StackFilter->ExecCount = Filter->ExecCount;

    BlockStart = VM_ReadData(input);

    if ( FirstByte & 0x40 )
    {
        BlockStart += 258;
    }

    StackFilter->BlockStart = (BlockStart+data->UnpPtr) & MAXWINMASK;

    if (FirstByte & 0x20)
    {
        StackFilter->BlockLength = VM_ReadData(input);
    }
    else
    {
        StackFilter->BlockLength = *ArrayGet(data->OldFilterLengths,FilterPos,uint);
    }

    StackFilter->NextWindow = (data->WrPtr != data->UnpPtr && ( (data->WrPtr-data->UnpPtr) & MAXWINMASK) <= BlockStart );

    *ArrayGet(data->OldFilterLengths,FilterPos,uint) = StackFilter->BlockLength;

    memset(StackFilter->InitR,0,sizeof(StackFilter->InitR));

    StackFilter->InitR[3] = VM_GLOBALMEMADDR;
    StackFilter->InitR[4] = StackFilter->BlockLength;
    StackFilter->InitR[5] = StackFilter->ExecCount;

    if (FirstByte & 0x10)
    {
        uint InitMask;
        int I;

        InitMask = BitGet(input) >> 9;
        BitAdd(input,7);

        for ( I=0 ; I<7 ;I++)
        {
            if (InitMask & (1<<I)) StackFilter->InitR[I]= VM_ReadData(input);
        }
    }

    if ( NewFilter )
    {
        tINPUT inp;
        uint VmCodeSize;

        VmCodeSize = VM_ReadData(input);

        if ( VmCodeSize >= 0x10000 || VmCodeSize == 0)
        {
            return FALSE;
        }

        InputInitSrc(&inp,input,RarReadByteStream);
        InputSetLimit(&inp, VmCodeSize);
        inp.rd_crc = 0xFFFFFFFF;
        //Printf("Loading code, %02X bytes" crlf, VmCodeSize);
        if ( !VM_Prepare(data,&inp, Filter) )
        {
            return FALSE;
        }
    }

    StackFilter->Cmd      = Filter->Cmd.ptr;
    StackFilter->CmdCount = Filter->Cmd.count;

    cnt = Filter->StaticData.count;
    if (  cnt > 0 && cnt < VM_GLOBALMEMSIZE )
    {
        if ( ArrayAdd(data, &StackFilter->StaticData, cnt, "StackFilter->StaticData") == NULL)
        {
            return FALSE;
        }
        memcpy(StackFilter->StaticData.ptr, Filter->StaticData.ptr, cnt);
    }



    if ( FirstByte & 0x08 )
    {
        uint DataSize;
        byte *GlobalData;

        DataSize = VM_ReadData(input);

        if ( DataSize > 0x10000 )
        {
            return FALSE;
        }

        if ( ArrayAdd(data,&StackFilter->GlobalData,DataSize + VM_FIXEDGLOBALSIZE,"StackFilter->GlobalData") == NULL )
        {
            return FALSE;
        }

        GlobalData = ArrayGet(StackFilter->GlobalData, VM_FIXEDGLOBALSIZE, byte);

        while ( DataSize-- && !input->eof )
        {
          // bugfix, 04.09.2003
          // *GlobalData++ = RarReadByte(input);
          *GlobalData++ = BitGet(input) >> 8; BitAdd(input, 8);
        }
    }
    else
    {
        if ( ArrayAdd(data,&StackFilter->GlobalData,VM_FIXEDGLOBALSIZE,"StackFilter->GlobalData") == NULL )
        {
            return FALSE;
        }
    }

    GlobalData = (uint*)StackFilter->GlobalData.ptr;
    ptr = StackFilter->InitR;
    cnt = 7;

    while ( cnt-- )
    {
        VM_SetDwordValue(GlobalData++, *ptr++);
    }

    VM_SetDwordValue(GlobalData++, StackFilter->BlockLength);
    VM_SetDwordValue(GlobalData++, 0);
    VM_SetDwordValue(GlobalData++, StackFilter->ExecCount);
    memset(GlobalData,0,0x10);

    return TRUE;
}

RSTATIC rarbool RATTRIB VM_ReadCode(tRAR *data, tINPUT *input)
{
    uint FirstByte;
    uint Length;
    rarbool rcode;

    rcode = FALSE;

    FirstByte = RarReadByte(input);

    Length = (FirstByte & 0x07) + 1;

    if ( Length == 7 )
    {
        Length = RarReadByte(input) +7;
    }
    else
    {
        if ( Length == 8 )
        {
            Length  = RarReadByte(input) << 8;
            Length += RarReadByte(input) ;
        }
    }

    InputSetLimit(input,Length);
    rcode = VM_AddCode(data, FirstByte, input);

    return rcode;
}

RSTATIC void RATTRIB VM_SetMemory   (struct sRAR *data, uint pos, byte *buf, uint count)
{
    if ( pos < VM_MEMSIZE && buf != (data->VM.Mem) )
    {
        MemMove(data->VM.Mem, buf, (count < (VM_MEMSIZE-pos)) ? count : (VM_MEMSIZE-pos));
    }
}


#define SET_IP(IP)                          \
    if ((IP)>=filter->CmdCount)             \
        return(TRUE);                       \
    if (--MaxOpCount<=0)                    \
    {                                       \
        return(FALSE);                      \
    }                                       \
    Cmd=filter->Cmd+(IP);

#ifdef VM_PREPARE_OPERAND
RSTATIC void RATTRIB VM_PrepareOperand (tVM *vm, tOPERAND *CmdOp)
{
    switch ( CmdOp->Type )
    {
        case VM_OPMEMREG:
        //case VM_OPMEMREGBASE:
        case VM_OPREG   :
            CmdOp->Addr = &vm->R[CmdOp->Data];
            break;

        case VM_OPINT   :
            CmdOp->Addr = &CmdOp->Data;
            break;

        /*
        case VM_OPMEMREG:
            // need to be fixed in future
            CmdOp->Addr = (uint*)(&vm->Mem[(vm->R[CmdOp->Data]) & VM_MEMMASK]);
            break;

        case VM_OPMEMREGBASE:
            // need to be fixed in future
            CmdOp->Addr = (uint*)(&vm->Mem[(vm->R[CmdOp->Data]+CmdOp->Base) & VM_MEMMASK]);
            break;
        */
        case VM_OPMEMBASE:
            CmdOp->Addr = (uint*)(&vm->Mem[CmdOp->Base & VM_MEMMASK]);
            break;

    }
}

RSTATIC void RATTRIB VM_PrepareOperands(tVM *vm, tFILTER *filter)
{
    tCOMMAND *Cmd;
    uint count;

    Cmd = filter->Cmd.ptr;
    count = filter->Cmd.count;

    while ( count-- )
    {
        VM_PrepareOperand(vm, &Cmd->Op1);
        VM_PrepareOperand(vm, &Cmd->Op2);
        Cmd++;
    }
}

RSTATIC __inline uint *VM_GetOperand(tVM *vm, tOPERAND *CmdOp)
{
    uint *ptr = CmdOp->Addr;

    if ( CmdOp->Type == VM_OPMEMREG )
    {
        ptr = (uint*)(&vm->Mem[(*ptr+CmdOp->Base) & VM_MEMMASK]);
    }
    return ptr;
}

#else /* VM_PREPARE_OPERAND */
RSTATIC uint * RATTRIB VM_GetOperand(tVM *vm, tOPERAND *CmdOp)
{
    uint *ptr = &CmdOp->Data;

    switch ( CmdOp->Type )
    {
        case VM_OPREG   :
            ptr = &vm->R[CmdOp->Data];
            break;

        case VM_OPINT   :
            //ptr = &CmdOp->Data; already calculated
            break;

        case VM_OPMEMREG:
            ptr = (uint*)(&vm->Mem[(vm->R[CmdOp->Data]) & VM_MEMMASK]);
            break;

        case VM_OPMEMREGBASE:
            ptr = (uint*)(&vm->Mem[(vm->R[CmdOp->Data]+CmdOp->Base) & VM_MEMMASK]);
            break;

        case VM_OPMEMBASE:
            ptr = (uint*)(&vm->Mem[CmdOp->Base & VM_MEMMASK]);
            break;

    }
    return ptr;
}

#endif /* VM_PREPARE_OPERAND */

RSTATIC rarbool RATTRIB VM_ExecuteCode (struct sRAR *data, tSTACK_FILTER *filter)
{

    tCOMMAND *Cmd;
    int MaxOpCount;
    tVM *vm;

    //Printf("Executing StackFilter %08X, %d commands" crlf, filter, filter->CmdCount);

    MaxOpCount = 25000000;
    vm = &data->VM;
    Cmd = filter->Cmd;

    for ( ;; )
    {
        uint *Op1;
        uint *Op2;

        Op1 = VM_GetOperand(vm,&Cmd->Op1);
        Op2 = VM_GetOperand(vm,&Cmd->Op2);

        switch( Cmd->OpCode )
        {
            case VM_MOV:
                VM_SetValue(Cmd->ByteMode,Op1,VM_GetValue(Cmd->ByteMode,Op2));
                break;

        #ifdef VM_OPTIMIZE

            case VM_MOVB:
                VM_SetByteValue(Op1,VM_GetByteValue(Op2));
                break;

            case VM_MOVD:
                VM_SetDwordValue(Op1,VM_GetDwordValue(Op2));
                break;

        #endif

            case VM_CMP:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint Result=Value1-VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:(Result>Value1)|(Result&VM_FS);
                }
                break;

        #ifdef VM_OPTIMIZE
            case VM_CMPB:
                {
                    uint Value1=VM_GetByteValue(Op1);
                    uint Result=Value1-VM_GetByteValue(Op2);
                    vm->Flags=Result==0 ? VM_FZ:(Result>Value1)|(Result&VM_FS);
                }
                break;
            case VM_CMPD:
                {
                    uint Value1=VM_GetDwordValue(Op1);
                    uint Result=Value1-VM_GetDwordValue(Op2);
                    vm->Flags=Result==0 ? VM_FZ:(Result>Value1)|(Result&VM_FS);
                }
                break;
        #endif

            case VM_ADD:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint Result=Value1+VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:(Result<Value1)|(Result&VM_FS);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

        #ifdef VM_OPTIMIZE
            case VM_ADDB:
                VM_SetByteValue(Op1,VM_GetByteValue(Op1)+VM_GetByteValue(Op2));
                break;

            case VM_ADDD:
                VM_SetDwordValue(Op1,VM_GetDwordValue(Op1)+VM_GetDwordValue(Op2));
                break;
        #endif

            case VM_SUB:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint Result=Value1-VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:(Result>Value1)|(Result&VM_FS);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

        #ifdef VM_OPTIMIZE
            case VM_SUBB:
                VM_SetByteValue(Op1,VM_GetByteValue(Op1)-VM_GetByteValue(Op2));
                break;

            case VM_SUBD:
                VM_SetDwordValue(Op1,VM_GetDwordValue(Op1)-VM_GetDwordValue(Op2));
                break;
        #endif
            case VM_JZ:
                if ((vm->Flags & VM_FZ)!=0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_JNZ:
                if ((vm->Flags & VM_FZ)==0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_INC:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)+1;
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                    vm->Flags=Result==0 ? VM_FZ:Result&VM_FS;
                }
                break;

        #ifdef VM_OPTIMIZE
            case VM_INCB:
                VM_SetByteValue(Op1,VM_GetByteValue(Op1)+1);
                break;

            case VM_INCD:
                VM_SetDwordValue(Op1,VM_GetDwordValue(Op1)+1);
                break;
        #endif
            case VM_DEC:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)-1;
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                    vm->Flags=Result==0 ? VM_FZ:Result&VM_FS;
                }
                break;

        #ifdef VM_OPTIMIZE
            case VM_DECB:
                VM_SetByteValue(Op1,VM_GetByteValue(Op1)-1);
                break;

            case VM_DECD:
                VM_SetDwordValue(Op1,VM_GetDwordValue(Op1)-1);
                break;
        #endif
            case VM_JMP:
                SET_IP(VM_GetDwordValue(Op1));
                continue;

            case VM_XOR:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)^VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:Result&VM_FS;
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_AND:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)&VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:Result&VM_FS;
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_OR:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)|VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:Result&VM_FS;
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_TEST:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)&VM_GetValue(Cmd->ByteMode,Op2);
                    vm->Flags=Result==0 ? VM_FZ:Result&VM_FS;
                }
                break;

            case VM_JS:
                if ((vm->Flags & VM_FS)!=0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_JNS:
                if ((vm->Flags & VM_FS)==0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_JB:
                if ((vm->Flags & VM_FC)!=0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_JBE:
                if ((vm->Flags & (VM_FC|VM_FZ))!=0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_JA:
                if ((vm->Flags & (VM_FC|VM_FZ))==0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_JAE:
                if ((vm->Flags & VM_FC)==0)
                {
                    SET_IP(VM_GetDwordValue(Op1));
                    continue;
                }
                break;

            case VM_PUSH:
                vm->R[7]-=4;
                VM_SetDwordValue((uint *)&vm->Mem[vm->R[7]&VM_MEMMASK],VM_GetDwordValue(Op1));
                break;

            case VM_POP:
                VM_SetDwordValue(Op1,VM_GetDwordValue((uint *)&vm->Mem[vm->R[7] & VM_MEMMASK]));
                vm->R[7]+=4;
                break;

            case VM_CALL:
                vm->R[7]-=4;
                VM_SetDwordValue((uint *)&vm->Mem[vm->R[7]&VM_MEMMASK],(uint)(Cmd-filter->Cmd+1));
                SET_IP(VM_GetDwordValue(Op1));
                continue;

            case VM_RET:
                if (vm->R[7]>=VM_MEMSIZE) return(TRUE);
                SET_IP(VM_GetDwordValue((uint *)&vm->Mem[vm->R[7] & VM_MEMMASK]));
                vm->R[7]+=4;
                continue;

            case VM_NOT:
                VM_SetValue(Cmd->ByteMode,Op1,~VM_GetValue(Cmd->ByteMode,Op1));
                break;

            case VM_SHL:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint Value2=VM_GetValue(Cmd->ByteMode,Op2);
                    uint Result=Value1<<Value2;
                    vm->Flags=(Result==0 ? VM_FZ:(Result&VM_FS))|((Value1<<(Value2-1))&0x80000000 ? VM_FC:0);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_SHR:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint Value2=VM_GetValue(Cmd->ByteMode,Op2);
                    uint Result=Value1>>Value2;
                    vm->Flags=(Result==0 ? VM_FZ:(Result&VM_FS))|((Value1>>(Value2-1))&VM_FC);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_SAR:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint Value2=VM_GetValue(Cmd->ByteMode,Op2);
                    uint Result=((int)Value1)>>Value2;
                    vm->Flags=(Result==0 ? VM_FZ:(Result&VM_FS))|((Value1>>(Value2-1))&VM_FC);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_NEG:
                {
                    uint Result;
                    if ( Cmd->ByteMode )
                    {
                        Result = -(char)(VM_GetByteValue(Op1));
                    }
                    else
                    {
                        Result = -(int)(VM_GetDwordValue(Op1));
                    }

                    vm->Flags=Result==0 ? VM_FZ:VM_FC|(Result&VM_FS);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

        #ifdef VM_OPTIMIZE

            case VM_NEGB:
                VM_SetByteValue(Op1,-(char)(VM_GetByteValue(Op1)));
                break;

            case VM_NEGD:
                VM_SetDwordValue(Op1,-(int)(VM_GetDwordValue(Op1)));
                break;
        #endif
            case VM_PUSHA:
                {
                    int I;
                    int SP;
                    const int RegCount=sizeof(vm->R)/sizeof(vm->R[0]);
                    for (I=0,SP=vm->R[7]-4;I<RegCount;I++,SP-=4)
                        VM_SetDwordValue((uint *)&vm->Mem[SP & VM_MEMMASK],vm->R[I]);
                    vm->R[7]-=RegCount*4;
                }
                break;

            case VM_POPA:
                {
                    int I;
                    int SP;
                    const int RegCount=sizeof(vm->R)/sizeof(vm->R[0]);
                    for (I=0,SP=vm->R[7];I<RegCount;I++,SP+=4)
                    vm->R[7-I]=VM_GetDwordValue((uint *)&vm->Mem[SP & VM_MEMMASK]);
                }
                break;

            case VM_PUSHF:
                vm->R[7]-=4;
                VM_SetDwordValue((uint *)&vm->Mem[vm->R[7]&VM_MEMMASK],vm->Flags);
                break;

            case VM_POPF:
                vm->Flags=VM_GetDwordValue((uint *)&vm->Mem[vm->R[7] & VM_MEMMASK]);
                vm->R[7]+=4;
                break;

            case VM_MOVZX:
                VM_SetDwordValue(Op1,VM_GetByteValue(Op2));
                break;

            case VM_MOVSX:
                VM_SetDwordValue(Op1,(signed char)VM_GetByteValue(Op2));
                break;

            case VM_XCHG:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    VM_SetValue(Cmd->ByteMode,Op1,VM_GetValue(Cmd->ByteMode,Op2));
                    VM_SetValue(Cmd->ByteMode,Op2,Value1);
                }
                break;

            case VM_MUL:
                {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)*VM_GetValue(Cmd->ByteMode,Op2);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_DIV:
                {
                    uint Divider=VM_GetValue(Cmd->ByteMode,Op2);
                    if (Divider!=0)
                    {
                    uint Result=VM_GetValue(Cmd->ByteMode,Op1)/Divider;
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                    }
                }
                break;

            case VM_ADC:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint FC=(vm->Flags&VM_FC);
                    uint Result=Value1+VM_GetValue(Cmd->ByteMode,Op2)+FC;
                    vm->Flags=Result==0 ? VM_FZ:(Result<Value1 || (Result==Value1 && FC))|(Result&VM_FS);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

            case VM_SBB:
                {
                    uint Value1=VM_GetValue(Cmd->ByteMode,Op1);
                    uint FC=(vm->Flags&VM_FC);
                    uint Result=Value1-VM_GetValue(Cmd->ByteMode,Op2)-FC;
                    vm->Flags=Result==0 ? VM_FZ:(Result>Value1 || (Result==Value1 && FC))|(Result&VM_FS);
                    VM_SetValue(Cmd->ByteMode,Op1,Result);
                }
                break;

        #ifdef VM_STANDARDFILTERS
            case VM_STANDARD:
                VM_StandardFilterExecute(vm, Cmd->Op1.Data);
                break;
        #endif
            case VM_PRINT:
                break;
        }
        Cmd++;
        --MaxOpCount;
    }
}

RSTATIC rarbool RATTRIB VM_Execute(struct sRAR *data, tSTACK_FILTER *filter)
{
    uint GlobalSize;
    uint StaticSize;
    uint NewBlockPos;
    uint NewBlockSize;
    uint DataSize;

    memcpy(data->VM.R, filter->InitR, sizeof(filter->InitR));

    GlobalSize = Min(filter->GlobalData.count,VM_GLOBALMEMSIZE);

    if ( GlobalSize ) memcpy(data->VM.Mem+VM_GLOBALMEMADDR,filter->GlobalData.ptr,GlobalSize);
    StaticSize = Min(filter->StaticData.count,VM_GLOBALMEMSIZE-GlobalSize);

    if (StaticSize) memcpy(data->VM.Mem+VM_GLOBALMEMADDR+GlobalSize,filter->StaticData.ptr,StaticSize);

    data->VM.R[7] = VM_MEMSIZE;     // stack register
    data->VM.Flags = 0;

    if ( !VM_ExecuteCode(data, filter)) filter->Cmd[0].OpCode = VM_RET;

    NewBlockPos =  VM_GetDwordValue(&data->VM.Mem[VM_GLOBALMEMADDR+0x20])&VM_MEMMASK;
    NewBlockSize = VM_GetDwordValue(&data->VM.Mem[VM_GLOBALMEMADDR+0x1c])&VM_MEMMASK;

    if ( (NewBlockPos+NewBlockSize) >= VM_MEMSIZE ) NewBlockPos = NewBlockSize = 0;

    filter->FilteredData = data->VM.Mem+NewBlockPos;
    filter->FilteredDataSize = NewBlockSize;

    ArrayDelete(data,&filter->GlobalData);
    DataSize = Min(VM_GetDwordValue((uint*)&data->VM.Mem[VM_GLOBALMEMADDR+0x30]),VM_GLOBALMEMSIZE);

    if ( DataSize != 0)
    {
      // bugfix, 04.09.2003
      // ArrayAdd(data,&filter->GlobalData, DataSize+VM_FIXEDGLOBALSIZE,"Filter->GlobalData");
      // memcpy(filter->GlobalData.ptr,&data->VM.Mem[VM_GLOBALMEMADDR],DataSize+VM_FIXEDGLOBALSIZE);
      if ( ArrayAdd(data,&filter->GlobalData, DataSize+VM_FIXEDGLOBALSIZE,"Filter->GlobalData") != NULL )
      {
        memcpy(filter->GlobalData.ptr,&data->VM.Mem[VM_GLOBALMEMADDR],DataSize+VM_FIXEDGLOBALSIZE);
      }
      else
      {
        return FALSE;
      }
    }
    return TRUE;
}

