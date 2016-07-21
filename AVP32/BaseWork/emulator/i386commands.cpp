/*
 *  i386commands.cpp,v 1.2 2002/10/17 09:55:02 tim 
 */

#include "typedefs.h"

void I386CPU::Cmd_0x80 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
    BYTE * reg = m_Regs.GetModRMRMB (mod_rm);
	WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
    
    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ADD eb,d8 */
        CPU_ADD (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("add %mrm, %lb"));
        break;

    case 0x08:  /* OR eb,d8 */
        CPU_OR (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("or %mrm, %lb"));
        break;

    case 0x10:  /* ADC eb,d8 */
        CPU_ADC (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("adc %mrm, %lb"));
        break;

    case 0x18:  /* SBB eb,b8 */
        CPU_SBB (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("sbb %mrm, %lb"));
        break;

    case 0x20:  /* AND eb,d8 */
        CPU_AND (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("and %mrm, %lb"));
        break;

    case 0x28:  /* SUB eb,d8 */
        CPU_SUB (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("sub %mrm, %lb"));
        break;

    case 0x30:  /* XOR eb,d8 */
        CPU_XOR (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("xor %mrm, %lb"));
        break;

    case 0x38:  /* CMP eb,d8 */
        CPU_CMP (&m_Regs.m_bFoo, reg);
        DISASSEMBLE (("cmp %mrm, %lb"));
        break;
    }
}



void I386CPU::Cmd_0x81_0x83 (BYTE cmd, bool fForce8bit)
{
    BYTE mod_rm = ReadIx86Byte();

    if ( m_Regs.GetOperandSize() == 16 )
    {
		WORD * dest = m_Regs.GetModRMRMW (mod_rm);

		if ( fForce8bit )
			WRITE_X86_WORD (&m_Regs.m_wFoo, (SWORD) (SBYTE) ReadIx86Byte());
		else
			WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());

		switch (mod_rm & 0x38)
		{
		case 0x00:  /* ADD ew,d16 */
            CPU_ADD (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("add %mrm, %lr"));
	        break;

		case 0x08:  /* OR ew,d16 */
            CPU_OR (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("or %mrm, %lr"));
	        break;

		case 0x10:  /* ADC ew,d16 */
            CPU_ADC (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("adc %mrm, %lr"));
	        break;

		case 0x18:  /* SBB ew,d16 */
            CPU_SBB (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("sbb %mrm, %lr"));
	        break;

		case 0x20:  /* AND ew,d16 */
            CPU_AND (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("and %mrm, %lr"));
	        break;

		case 0x28:  /* SUB ew,d16 */
            CPU_SUB (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("sub %mrm, %lr"));
	        break;

		case 0x30:  /* XOR ew,d16 */
            CPU_XOR (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("xor %mrm, %lr"));
        	break;

		case 0x38:  /* CMP ew,d16 */
            CPU_CMP (&m_Regs.m_wFoo, dest);
			DISASSEMBLE (("cmp %mrm, %lr"));
	        break;
		}
	}
	else
	{
		DWORD * dest = m_Regs.GetModRMRMD (mod_rm);

		if ( fForce8bit )
			WRITE_X86_DWORD (&m_Regs.m_dwFoo, (SDWORD) (SBYTE) ReadIx86Byte());
		else
			WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());

		switch (mod_rm & 0x38)
		{
		case 0x00:  /* ADD ew,d16 */
            CPU_ADD (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("add %mrm, %lr"));
	        break;

		case 0x08:  /* OR ew,d16 */
            CPU_OR (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("or %mrm, %lr"));
	        break;

		case 0x10:  /* ADC ew,d16 */
            CPU_ADC (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("adc %mrm, %lr"));
	        break;

		case 0x18:  /* SBB ew,d16 */
            CPU_SBB (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("sbb %mrm, %lr"));
	        break;

		case 0x20:  /* AND ew,d16 */
            CPU_AND (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("and %mrm, %lr"));
	        break;

		case 0x28:  /* SUB ew,d16 */
            CPU_SUB (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("sub %mrm, %lr"));
	        break;

		case 0x30:  /* XOR ew,d16 */
            CPU_XOR (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("xor %mrm, %lr"));
        	break;

		case 0x38:  /* CMP ew,d16 */
            CPU_CMP (&m_Regs.m_dwFoo, dest);
			DISASSEMBLE (("cmp %mrm, %lr"));
	        break;
		}
	}
}


void I386CPU::Cmd_notimplemented (BYTE cmd, const char * instr)
{
	fatal (6, "Invalid (not done) opcode %02X %s", cmd, instr ? instr : "");
}


void I386CPU::Cmd_0xF6 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
    
    switch (mod_rm & 0x38)
    {
    case 0x00:  /* TEST eb,d8 */
    case 0x08:  /* ??? */
    {
    	BYTE * dest = m_Regs.GetModRMRMB (mod_rm);
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
        CPU_TEST (dest, &m_Regs.m_bFoo);
		DISASSEMBLE (("test %mrm, %lb"));
        break;
	}

    case 0x10:	/* NOT Eb */
        CPU_NOT ((BYTE *) 0, m_Regs.GetModRMRMB (mod_rm));
		DISASSEMBLE (("not %mrm"));
        break;

    case 0x18:  /* NEG Eb */
        CPU_NEG ((BYTE *) 0, m_Regs.GetModRMRMB (mod_rm));
		DISASSEMBLE (("neg %mrm"));
        break;

    case 0x20:  /* MUL AL, Eb */
        CPU_MUL (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetReg8(AL), m_Regs.GetReg8(AH));
		DISASSEMBLE (("mul %mrm"));
        break;

    case 0x28:  /* IMUL AL, Eb */
        CPU_IMUL (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetReg8(AL), m_Regs.GetReg8(AH));
		DISASSEMBLE (("imul %mrm"));
        break;

    case 0x30:  /* DIV AL, Eb */
        CPU_DIV (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetReg8(AL), m_Regs.GetReg8(AH));
		DISASSEMBLE (("div %mrm"));
        break;

    case 0x38:  /* IDIV AL, Eb */
        CPU_IDIV (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetReg8(AL), m_Regs.GetReg8(AH));
		DISASSEMBLE (("idiv %mrm"));
        break;
	}
}


void I386CPU::Cmd_0xF7 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
    
    if ( m_Regs.GetOperandSize() == 16 )
    {
	    switch (mod_rm & 0x38)
	    {
	    case 0x00:  /* TEST eb,d8 */
	    case 0x08:  /* ??? */
	    {
	    	WORD * dest = m_Regs.GetModRMRMW (mod_rm);
			WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
	        CPU_TEST (dest, &m_Regs.m_wFoo);
			DISASSEMBLE (("test %mrm, %lw"));
	        break;
		}

	    case 0x10:	/* NOT Eb */
	        CPU_NOT ((WORD *) 0, m_Regs.GetModRMRMW (mod_rm));
			DISASSEMBLE (("not %mrm"));
	        break;

	    case 0x18:  /* NEG Eb */
	        CPU_NEG ((WORD *) 0, m_Regs.GetModRMRMW (mod_rm));
			DISASSEMBLE (("neg %mrm"));
	        break;

	    case 0x20:  /* MUL AX, r/m16 */
	        CPU_MUL (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetReg16(AX), m_Regs.GetReg16(DX));
			DISASSEMBLE (("mul %mrm"));
			break;

	    case 0x28:  /* IMUL AX, r/m16 */
	        CPU_IMUL (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetReg16(AX), m_Regs.GetReg16(DX));
			DISASSEMBLE (("imul %mrm"));
	        break;

	    case 0x30:  /* DIV AL, Eb */
	        CPU_DIV (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetReg16(AX), m_Regs.GetReg16(DX));
			DISASSEMBLE (("div %mrm"));
	        break;

	    case 0x38:  /* IDIV AL, Eb */
	        CPU_IDIV (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetReg16(AX), m_Regs.GetReg16(DX));
			DISASSEMBLE (("idiv %mrm"));
    	    break;
    	}
	}
	else
    {
	    switch (mod_rm & 0x38)
	    {
	    case 0x00:  /* TEST eb,d8 */
	    case 0x08:  /* ??? */
	    {
	    	DWORD * dest = m_Regs.GetModRMRMD (mod_rm);
			WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
	        CPU_TEST (dest, &m_Regs.m_dwFoo);
			DISASSEMBLE (("test %mrm, %ld"));
	        break;
		}

	    case 0x10:	/* NOT Eb */
	        CPU_NOT ((DWORD *) 0, m_Regs.GetModRMRMD (mod_rm));
			DISASSEMBLE (("not %mrm"));
	        break;

	    case 0x18:  /* NEG Eb */
	        CPU_NEG ((DWORD *) 0, m_Regs.GetModRMRMD (mod_rm));
			DISASSEMBLE (("neg %mrm"));
	        break;

	    case 0x20:  /* MUL AL, Eb */
	        CPU_MUL (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetReg32(EAX), m_Regs.GetReg32(EDX));
			DISASSEMBLE (("mul %mrm"));
			break;

	    case 0x28:  /* IMUL AL, Eb */
	        CPU_IMUL (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetReg32(EAX), m_Regs.GetReg32(EDX));
			DISASSEMBLE (("imul %mrm"));
			break;

	    case 0x30:  /* DIV AL, Eb */
	        CPU_DIV (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetReg32(EAX), m_Regs.GetReg32(EDX));
			DISASSEMBLE (("div %mrm"));
	        break;

	    case 0x38:  /* IDIV AL, Eb */
	        CPU_IDIV (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetReg32(EAX), m_Regs.GetReg32(EDX));
			DISASSEMBLE (("idiv %mrm"));
    	    break;
    	}
	}
}


void I386CPU::Cmd_0x0F_0xBA (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

    if ( m_Regs.GetOperandSize() == 16 )
    {
    	/* Store immediate byte value in temp value - need for memory guard */
        WORD * dest = m_Regs.GetModRMRMW (mod_rm);
		WRITE_X86_WORD (&m_Regs.m_wFoo, (WORD) ReadIx86Byte());

	    switch (mod_rm & 0x38)
	    {
	    default:
	    	fatal (6, "0x0F 0x0A: wrong cmd");

		case 0x20:	/* BT  r/m16, imm8 */
	        CPU_BT (dest, &m_Regs.m_wFoo);
			DISASSEMBLE (("bt %mrm, %lb"));
	        break;

		case 0x28:	/* BTS r/m16, imm8 */
	        CPU_BTS (dest, &m_Regs.m_wFoo);
			DISASSEMBLE (("bts %mrm, %lb"));
	        break;

		case 0x30:	/* BTR r/m16, imm8 */
	        CPU_BTR (dest, &m_Regs.m_wFoo);
			DISASSEMBLE (("btr %mrm, %lb"));
	        break;

		case 0x38:	/* BTC r/m16, imm8 */
	        CPU_BTC (dest, &m_Regs.m_wFoo);
			DISASSEMBLE (("btc %mrm, %lb"));
	        break;
		}
	}
	else
	{
    	/* Store immediate byte value in temp value - need for memory guard */
        DWORD * dest = m_Regs.GetModRMRMD (mod_rm);
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, (DWORD) ReadIx86Byte());

	    switch (mod_rm & 0x38)
	    {
	    default:
	    	fatal (6, "0x0F 0x0A: wrong cmd");

		case 0x20:	/* BT  r/m16, imm8 */
	        CPU_BT (dest, &m_Regs.m_dwFoo);
			DISASSEMBLE (("bt %mrm, %lb"));
	        break;

		case 0x28:	/* BTS r/m16, imm8 */
	        CPU_BTS (dest, &m_Regs.m_dwFoo);
			DISASSEMBLE (("bts %mrm, %lb"));
	        break;

		case 0x30:	/* BTR r/m16, imm8 */
	        CPU_BTR (dest, &m_Regs.m_dwFoo);
			DISASSEMBLE (("btr %mrm, %lb"));
	        break;

		case 0x38:	/* BTC r/m16, imm8 */
	        CPU_BTC (dest, &m_Regs.m_dwFoo);
			DISASSEMBLE (("btc %mrm, %lb"));
	        break;
		}
	}
}


/* INC/DEC r/m8 */
void I386CPU::Cmd_0xFE (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

    switch (mod_rm & 0x38)
    {
    default:
    	fatal (6, "0xFE: wrong cmd 0x%02X", mod_rm & 0x38);

    case 0x00:  /* INC r/m8 */
        CPU_INC (m_Regs.GetModRMRMB (mod_rm));
		DISASSEMBLE (("inc %mrm"));
        break;

    case 0x08:	/* DEC r/m8 */
        CPU_DEC (m_Regs.GetModRMRMB (mod_rm));
		DISASSEMBLE (("dec %mrm"));
        break;
	}
}


/* INC/DEC/CALL/CALL FAR/JMP/JMP FAR/PUSH */
void I386CPU::Cmd_0xFF (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

    switch (mod_rm & 0x38)
    {
    default:
    	fatal (6, "0xFF: unsupported cmd 0x%02X", (mod_rm & 0x38) >> 3 );

	case 0x00:	/* INC  r/ms */
		if ( m_Regs.GetOperandSize() == 16 )
			CPU_INC (m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_INC (m_Regs.GetModRMRMD (mod_rm));

		DISASSEMBLE (("inc %mrm"));
		break;

	case 0x08:	/* DEC  r/ms */
		if ( m_Regs.GetOperandSize() == 16 )
			CPU_DEC (m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_DEC (m_Regs.GetModRMRMD (mod_rm));

		DISASSEMBLE (("dec %mrm"));
		break;

    case 0x10:  /* CALL r/m32 */
    	Cmd_call (mod_rm, false);
    	break;

    /* 0x18: CALL m32:32 */

    case 0x20:	/* JMP r/m32 */
		m_Regs.eip = READ_X86_DWORD(m_Regs.GetModRMRMD(mod_rm));
		DISASSEMBLE (("jmp %mrm"));
		break;

    /* 0x28: JMP m32:32 */

    case 0x30:  /* PUSH ms */
		if ( m_Regs.GetOperandSize() == 16 )
			CPU_PUSH (m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_PUSH (m_Regs.GetModRMRMD (mod_rm));

		DISASSEMBLE (("push %mrm"));
        break;
	}
}



void I386CPU::Cmd_0xC0 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
    BYTE * dest = m_Regs.GetModRMRMB(mod_rm);
	BYTE count = ReadIx86Byte();

    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ROL r/m8, imm8 */
        CPU_ROL (dest, count);
		DISASSEMBLE (("rol %mrm, %lb"));
        break;

    case 0x08:  /* ROR r/m8, imm8 */
        CPU_ROR (dest, count);
		DISASSEMBLE (("ror %mrm, %lb"));
        break;

    case 0x10:  /* RCL r/m8, imm8 */
        CPU_RCL (dest, count);
		DISASSEMBLE (("rcl %mrm, %lb"));
        break;

    case 0x18:  /* RCR r/m8, imm8 */
        CPU_RCR (dest, count);
		DISASSEMBLE (("rcr %mrm, %lb"));
        break;

    case 0x20:  /* SAL r/m8, imm8 */
    case 0x30:  /* SHL r/m8, imm8 */
        CPU_SAL (dest, count);
		DISASSEMBLE (("sal %mrm, %lb"));
        break;

    case 0x28:  /* SHR r/m8, imm8 */
        CPU_SHR (dest, count);
		DISASSEMBLE (("shr %mrm, %lb"));
        break;

    case 0x38:  /* SAR r/m8, imm8 */
        CPU_SAR (dest, count);
		DISASSEMBLE (("sar %mrm, %lb"));
        break;
    }
}



void I386CPU::Cmd_0xC1 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ROL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_ROL (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_ROL (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("rol %mrm, %lb"));
        break;

    case 0x08:  /* ROR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_ROR (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_ROR (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("ror %mrm, %lb"));
        break;

    case 0x10:  /* RCL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_RCL (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_RCL (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("rcl %mrm, %lb"));
        break;

    case 0x18:  /* RCR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_RCR (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_RCR (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("rcr %mrm, %lb"));
        break;

    case 0x20:  /* SAL r/ms, imms */
    case 0x30:  /* SHL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_SAL (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_SAL (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("sal %mrm, %lb"));
        break;

    case 0x28:  /* SHR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_SHR (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_SHR (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("shr %mrm, %lb"));
        break;

    case 0x38:  /* SAR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
		{
			WORD * dest = m_Regs.GetModRMRMW(mod_rm);
        	CPU_SAR (dest, ReadIx86Byte());
		}
		else
		{
			DWORD * dest = m_Regs.GetModRMRMD(mod_rm);
        	CPU_SAR (dest, ReadIx86Byte());
		}

		DISASSEMBLE (("sar %mrm, %lb"));
        break;
    }
}



void I386CPU::Cmd_0xD0 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
	const BYTE count = 1;

    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ROL r/m8, imm8 */
        CPU_ROL (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("rol %mrm, 1"));
        break;

    case 0x08:  /* ROR r/m8, imm8 */
        CPU_ROR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("ror %mrm, 1"));
        break;

    case 0x10:  /* RCL r/m8, imm8 */
        CPU_RCL (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("rcl %mrm, 1"));
        break;

    case 0x18:  /* RCR r/m8, imm8 */
        CPU_RCR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("rcr %mrm, 1"));
        break;

    case 0x20:  /* SAL r/m8, imm8 */
    case 0x30:  /* SHL r/m8, imm8 */
        CPU_SAL (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("sal %mrm, 1"));
        break;

    case 0x28:  /* SHR r/m8, imm8 */
        CPU_SHR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("shr %mrm, 1"));
        break;

    case 0x38:  /* SAR r/m8, imm8 */
        CPU_SAR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("sar %mrm, 1"));
        break;
    }
}


void I386CPU::Cmd_0xD1 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
	const BYTE count = 1;

    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ROL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_ROL (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_ROL (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("rol %mrm, 1"));
        break;

    case 0x08:  /* ROR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_ROR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_ROR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("ror %mrm, 1"));
        break;

    case 0x10:  /* RCL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_RCL (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_RCL (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("rcl %mrm, 1"));
        break;

    case 0x18:  /* RCR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_RCR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_RCR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("rcr %mrm, 1"));
        break;

    case 0x20:  /* SAL r/ms, imms */
    case 0x30:  /* SHL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_SAL (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_SAL (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("sal %mrm, 1"));
        break;

    case 0x28:  /* SHR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_SHR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_SHR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("shr %mrm, 1"));
        break;

    case 0x38:  /* SAR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_SAR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_SAR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("sar %mrm, 1"));
        break;
    }
}



void I386CPU::Cmd_0xD2 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
	BYTE count = READ_X86_BYTE(m_Regs.GetReg8(CL));

    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ROL r/m8, imm8 */
        CPU_ROL (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("rol %mrm, cl"));
        break;

    case 0x08:  /* ROR r/m8, imm8 */
        CPU_ROR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("ror %mrm, cl"));
        break;

    case 0x10:  /* RCL r/m8, imm8 */
        CPU_RCL (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("rcl %mrm, cl"));
        break;

    case 0x18:  /* RCR r/m8, imm8 */
        CPU_RCR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("rcr %mrm, cl"));
        break;

    case 0x20:  /* SAL r/m8, imm8 */
    case 0x30:  /* SHL r/m8, imm8 */
        CPU_SAL (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("sal %mrm, cl"));
        break;

    case 0x28:  /* SHR r/m8, imm8 */
        CPU_SHR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("shr %mrm, cl"));
        break;

    case 0x38:  /* SAR r/m8, imm8 */
        CPU_SAR (m_Regs.GetModRMRMB(mod_rm), count);
		DISASSEMBLE (("sar %mrm, cl"));
        break;
    }
}



void I386CPU::Cmd_0xD3 (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
	BYTE count = READ_X86_BYTE(m_Regs.GetReg8(CL));

    switch (mod_rm & 0x38)
    {
    case 0x00:  /* ROL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_ROL (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_ROL (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("rol %mrm, cl"));
        break;

    case 0x08:  /* ROR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_ROR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_ROR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("ror %mrm, cl"));
        break;

    case 0x10:  /* RCL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_RCL (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_RCL (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("rcl %mrm, cl"));
        break;

    case 0x18:  /* RCR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_RCR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_RCR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("rcr %mrm, cl"));
        break;

    case 0x20:  /* SAL r/ms, imms */
    case 0x30:  /* SHL r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_SAL (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_SAL (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("sal %mrm, cl"));
        break;

    case 0x28:  /* SHR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_SHR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_SHR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("shr %mrm, cl"));
        break;

    case 0x38:  /* SAR r/ms, imms */
		if ( m_Regs.GetOperandSize() == 16 )
        	CPU_SAR (m_Regs.GetModRMRMW(mod_rm), count);
		else
        	CPU_SAR (m_Regs.GetModRMRMD(mod_rm), count);

		DISASSEMBLE (("sar %mrm, cl"));
        break;
    }
}
