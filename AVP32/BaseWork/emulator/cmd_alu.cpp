/*
 *  cmd_alu.cpp,v 1.2 2002/10/17 09:54:59 tim 
 */

#include "typedefs.h"


void I386CPU::Cmd_daa (BYTE cmd)
{
	DISASSEMBLE (("daa"));

	if ( IS_SET(m_Regs.eflags, AF) || (*m_Regs.regb[AL] & 0x0F) > 9 )
	{
		*m_Regs.regb[AL] += 0x06;
        SET_BIT (m_Regs.eflags, AF, 1);
    }
    else
		SET_BIT (m_Regs.eflags, AF, 0);

	if ( IS_SET(m_Regs.eflags, CF) || *m_Regs.regb[AL] > 0x9F )
	{
		*m_Regs.regb[AL] += 0x60;
        SET_BIT (m_Regs.eflags, CF, 1);
    }
    else
		SET_BIT (m_Regs.eflags, CF, 0);

	m_Regs.ApplyFlags (ZF|SF|PF, *m_Regs.regb[AL]);
}


void I386CPU::Cmd_aaa (BYTE cmd)
{
	DISASSEMBLE (("aaa"));

	if ( IS_SET(m_Regs.eflags, AF) || (*m_Regs.regb[AL] & 0x0F) > 9 )
	{
		*m_Regs.regb[AL] = (*m_Regs.regb[AL] + 6) & 0x0F;
		(*m_Regs.regb[AH])++;
        SET_BIT (m_Regs.eflags, AF|CF, 1);
    }
    else
		SET_BIT (m_Regs.eflags, AF|CF, 0);
}


void I386CPU::Cmd_aad (BYTE cmd)
{
	BYTE mul = ReadIx86Byte();
	DISASSEMBLE (("aad %imm8", mul));

	*m_Regs.regb[AL] = *m_Regs.regb[AH] * mul + *m_Regs.regb[AL];
	*m_Regs.regb[AH] = 0;

	SET_BIT (m_Regs.eflags, SF, 0);
	m_Regs.ApplyFlags (ZF|PF, *m_Regs.regb[AL]);
}


void I386CPU::Cmd_aam (BYTE cmd)
{
	BYTE mul = ReadIx86Byte();
	DISASSEMBLE (("aam %imm8", mul));

	if ( mul == 0 )
		fatal (0, "aam: parameter is 0");

	*m_Regs.regb[AH] = *m_Regs.regb[AL] / mul;
	*m_Regs.regb[AL] %= mul;

	SET_BIT (m_Regs.eflags, ZF, *m_Regs.regb[AL] == 0 && *m_Regs.regb[AH]);
	m_Regs.ApplyFlags (SF|PF, *m_Regs.regb[AL]);
}


void I386CPU::Cmd_aas (BYTE cmd)
{
	DISASSEMBLE (("aas"));

	if ( IS_SET(m_Regs.eflags, AF) || (*m_Regs.regb[AL] & 0x0F) > 9 )
	{
		*m_Regs.regb[AL] = (*m_Regs.regb[AL] - 6) & 0x0F;
		(*m_Regs.regb[AH])--;
        SET_BIT (m_Regs.eflags, AF|CF, 1);
    }
    else
		SET_BIT (m_Regs.eflags, AF|CF, 0);
}


void I386CPU::Cmd_das (BYTE cmd)
{
	DISASSEMBLE (("das"));

	if ( IS_SET(m_Regs.eflags, AF) || (*m_Regs.regb[AL] & 0x0F) > 9 )
	{
		*m_Regs.regb[AL] -= 0x06;
        SET_BIT (m_Regs.eflags, AF, 1);
    }
    else
		SET_BIT (m_Regs.eflags, AF, 0);

	if ( IS_SET(m_Regs.eflags, CF) || *m_Regs.regb[AL] > 0x9F )
	{
		*m_Regs.regb[AL] -= 0x60;
        SET_BIT (m_Regs.eflags, CF, 1);
    }
    else
		SET_BIT (m_Regs.eflags, CF, 0);

	m_Regs.ApplyFlags (ZF|SF|PF, *m_Regs.regb[AL]);
}


void I386CPU::Cmd_cbw_cwde (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
	{
		DISASSEMBLE (("cbw"));
		WORD * ax = m_Regs.GetReg16(AX);
		WRITE_X86_WORD (ax, READ_X86_WORD(ax) & 0x80 ? 
			READ_X86_WORD(ax) | 0xFF00 : READ_X86_WORD(ax) & 0x00FF);
	}
	else
	{
		DISASSEMBLE (("cwde"));
		DWORD * eax = m_Regs.GetReg32(EAX);
		WRITE_X86_DWORD (eax, READ_X86_DWORD(eax) & 0x00008000 ? 
			READ_X86_DWORD(eax) | 0xFFFF0000 : READ_X86_DWORD(eax) & 0x0000FFFF);
	}
}


void I386CPU::Cmd_cwd_cdq (BYTE cmd)
{
    /* Opcode 0x99 */
	if ( m_Regs.GetOperandSize() == 16 )
	{
		DISASSEMBLE (("cwd"));
		WORD * ax = m_Regs.GetReg16(AX);
		WORD * dx = m_Regs.GetReg16(DX);
		WRITE_X86_WORD (dx, READ_X86_WORD(ax) & 0x8000 ? 0xFFFF : 0x0000);
	}
	else
	{
		DISASSEMBLE (("cdq"));
		DWORD * eax = m_Regs.GetReg32(EAX);
		DWORD * edx = m_Regs.GetReg32(EDX);

		WRITE_X86_DWORD (edx, READ_X86_DWORD(eax) & 0x80000000 ? 0xFFFFFFFF : 0x00000000);
	}
}


void I386CPU::Cmd_imul (BYTE cmd, bool fForce8Bit, bool fIsImmxValue)
{
    BYTE mod_rm = ReadIx86Byte();
    SDWORD arg1, arg2 = 0;

    if ( fIsImmxValue )
    {
		if ( fForce8Bit )
			arg2 = (SDWORD) (SBYTE) ReadIx86Byte();
		else if ( m_Regs.GetOperandSize() == 16 )
			arg2 = (SDWORD) (SWORD) ReadIx86Word();
		else
			arg2 = (SDWORD) ReadIx86DWord();
	}

	if ( m_Regs.GetOperandSize() == 16 )
	{
		WORD * dest = m_Regs.GetModRMRegW (mod_rm);
		arg1 = (SDWORD) (SWORD) READ_X86_WORD(m_Regs.GetModRMRMW (mod_rm));

		if ( !fIsImmxValue )
			arg2 = (SDWORD) READ_X86_SIGNED_DATA (dest);

		SDWORD result = arg1 * arg2;
		WRITE_X86_WORD (dest, (WORD) result);

		SET_BIT (m_Regs.eflags, CF|OF, result & 0xFFFF0000);
	}
	else
	{
#if defined (__unix__)
		const QWORD bitmask = 0xFFFFFFFF00000000ll;
#else
		const QWORD bitmask = 0xFFFFFFFF00000000;
#endif
		DWORD * dest = m_Regs.GetModRMRegD (mod_rm);
		arg1 = (SDWORD) READ_X86_DWORD (m_Regs.GetModRMRMD (mod_rm));

		if ( !fIsImmxValue )
			arg2 = (SDWORD) READ_X86_DWORD (dest);

		SQWORD result = arg1 * arg2;
		WRITE_X86_DWORD (dest, (DWORD) result);

		SET_BIT (m_Regs.eflags, CF|OF, ((QWORD) result & bitmask) ? 1 : 0);
	}

	if ( fIsImmxValue )
		DISASSEMBLE (("imul %mreg, %mrm, %lr"));
	else
		DISASSEMBLE (("imul %mreg, %mrm"));
}


void I386CPU::Cmd_xchg_regs	(BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
	{
		WORD * src = m_Regs.GetReg16(AX);
		CPU_XCHG (src, m_Regs.GetReg16(cmd) );
        DISASSEMBLE (("xchg ax, %r16"));
	}
	else
	{
		DWORD * src = m_Regs.GetReg32(AX);
		CPU_XCHG (src, m_Regs.GetReg32(cmd) );
		DISASSEMBLE (("xchg eax, %r32"));
	}
}


void I386CPU::Cmd_xchg_rms_rs (BYTE cmd, bool fForce8bit)
{
    BYTE mod_rm = ReadIx86Byte();

    if ( fForce8bit )
    	CPU_XCHG (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB (mod_rm));
	else if ( m_Regs.GetOperandSize() == 16 )
    	CPU_XCHG (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm));
	else
    	CPU_XCHG (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm));

    DISASSEMBLE (("xchg %mreg, %mrm"));
}





//////////////////////////////////////////////////////////////////////////
/* adc al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_adc_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit adc
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_ADC (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("adc al, %lb"));
		break;

	case 16:	// 16bit adc
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_ADC (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("adc ax, %lw"));
		break;

	default:	// 32bit adc
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_ADC (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("adc eax, %ld"));
		break;
	}
}



/* adc r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_adc_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit adc
		if ( fForceRMsRs )
			CPU_ADC (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_ADC (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit adc
		if ( fForceRMsRs )
            CPU_ADC (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_ADC (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit adc
		if ( fForceRMsRs )
            CPU_ADC (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_ADC (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("adc %mrm, %mreg"));
	else
		DISASSEMBLE (("adc %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* add al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_add_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit add
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_ADD (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("add al, %lb"));
		break;

	case 16:	// 16bit add
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_ADD (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("add ax, %lw"));
		break;

	default:	// 32bit add
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_ADD (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("add eax, %ld"));
		break;
	}
}



/* add r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_add_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit add
		if ( fForceRMsRs )
			CPU_ADD (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_ADD (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit add
		if ( fForceRMsRs )
            CPU_ADD (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_ADD (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit add
		if ( fForceRMsRs )
            CPU_ADD (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_ADD (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("add %mrm, %mreg"));
	else
		DISASSEMBLE (("add %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* and al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_and_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit and
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_AND (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("and al, %lb"));
		break;

	case 16:	// 16bit and
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_AND (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("and ax, %lw"));
		break;

	default:	// 32bit and
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_AND (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("and eax, %ld"));
		break;
	}
}



/* and r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_and_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit and
		if ( fForceRMsRs )
			CPU_AND (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_AND (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit and
		if ( fForceRMsRs )
            CPU_AND (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_AND (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit and
		if ( fForceRMsRs )
            CPU_AND (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_AND (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("and %mrm, %mreg"));
	else
		DISASSEMBLE (("and %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* or al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_or_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit or
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_OR (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("or al, %lb"));
		break;

	case 16:	// 16bit or
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_OR (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("or ax, %lw"));
		break;

	default:	// 32bit or
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_OR (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("or eax, %ld"));
		break;
	}
}



/* or r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_or_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit or
		if ( fForceRMsRs )
			CPU_OR (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_OR (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit or
		if ( fForceRMsRs )
            CPU_OR (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_OR (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit or
		if ( fForceRMsRs )
            CPU_OR (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_OR (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("or %mrm, %mreg"));
	else
		DISASSEMBLE (("or %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* xor al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_xor_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit xor
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_XOR (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("xor al, %lb"));
		break;

	case 16:	// 16bit xor
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_XOR (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("xor ax, %lw"));
		break;

	default:	// 32bit xor
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_XOR (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("xor eax, %ld"));
	}
}



/* xor r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_xor_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit xor
		if ( fForceRMsRs )
			CPU_XOR (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_XOR (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit xor
		if ( fForceRMsRs )
            CPU_XOR (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_XOR (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit xor
		if ( fForceRMsRs )
            CPU_XOR (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_XOR (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("xor %mrm, %mreg"));
	else
		DISASSEMBLE (("xor %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* sbb al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_sbb_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit sbb
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_SBB (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("sbb al, %lb"));
		break;

	case 16:	// 16bit sbb
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_SBB (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("sbb ax, %lw"));
		break;

	default:	// 32bit sbb
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_SBB (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("sbb eax, %ld"));
		break;
	}
}



/* sbb r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_sbb_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit sbb
		if ( fForceRMsRs )
			CPU_SBB (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_SBB (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit sbb
		if ( fForceRMsRs )
            CPU_SBB (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_SBB (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit sbb
		if ( fForceRMsRs )
            CPU_SBB (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_SBB (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("sbb %mrm, %mreg"));
	else
		DISASSEMBLE (("sbb %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* sub al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_sub_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit sub
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_SUB (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("sub al, %lb"));
		break;

	case 16:	// 16bit sub
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_SUB (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("sub ax, %lw"));
		break;

	default:	// 32bit sub
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_SUB (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("sub eax, %ld"));
		break;
	}
}



/* sub r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_sub_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit sub
		if ( fForceRMsRs )
			CPU_SUB (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_SUB (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit sub
		if ( fForceRMsRs )
            CPU_SUB (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_SUB (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit sub
		if ( fForceRMsRs )
            CPU_SUB (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_SUB (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("sub %mrm, %mreg"));
	else
		DISASSEMBLE (("sub %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* cmp al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_cmp_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit cmp
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_CMP (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("cmp al, %lb"));
		break;

	case 16:	// 16bit cmp
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_CMP (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("cmp ax, %lw"));
		break;

	default:	// 32bit cmp
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_CMP (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("cmp eax, %ld"));
		break;
	}
}



/* cmp r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_cmp_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit cmp
		if ( fForceRMsRs )
			CPU_CMP (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_CMP (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit cmp
		if ( fForceRMsRs )
            CPU_CMP (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_CMP (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit cmp
		if ( fForceRMsRs )
            CPU_CMP (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_CMP (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("cmp %mrm, %mreg"));
	else
		DISASSEMBLE (("cmp %mreg, %mrm"));
}



//////////////////////////////////////////////////////////////////////////
/* test al/ax/eax, imm8/16/32 */
void I386CPU::Cmd_test_areg_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:		// 8bit test
		WRITE_X86_BYTE (&m_Regs.m_bFoo, ReadIx86Byte());
		CPU_TEST (&m_Regs.m_bFoo, m_Regs.GetReg8 (AL));

		DISASSEMBLE (("test al, %lb"));
		break;

	case 16:	// 16bit test
		WRITE_X86_WORD (&m_Regs.m_wFoo, ReadIx86Word());
		CPU_TEST (&m_Regs.m_wFoo, m_Regs.GetReg16 (AX));

		DISASSEMBLE (("test ax, %lw"));
		break;

	default:	// 32bit test
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());
		CPU_TEST (&m_Regs.m_dwFoo, m_Regs.GetReg32 (EAX));

		DISASSEMBLE (("test eax, %ld"));
		break;
	}
}



/* test r/m8/16/32, r/m8/16/32 */
void I386CPU::Cmd_test_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    register BYTE mod_rm = ReadIx86Byte();

    switch (size)
    {
    case 8:  // 8bit test
		if ( fForceRMsRs )
			CPU_TEST (m_Regs.GetModRMRegB(mod_rm), m_Regs.GetModRMRMB (mod_rm));
		else
			CPU_TEST (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB(mod_rm));
		break;

	case 16: // 16bit test
		if ( fForceRMsRs )
            CPU_TEST (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		else
			CPU_TEST (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW(mod_rm));
		break;

	default: // 32bit test
		if ( fForceRMsRs )
            CPU_TEST (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		else
			CPU_TEST (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD(mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("test %mrm, %mreg"));
	else
		DISASSEMBLE (("test %mreg, %mrm"));
}


/* inc rs */
void I386CPU::Cmd_inc_rs (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
		CPU_INC (m_Regs.GetReg16(cmd));
	else
		CPU_INC (m_Regs.GetReg32(cmd));

	DISASSEMBLE (("inc %reg"));
}


/* dec rs */
void I386CPU::Cmd_dec_rs (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
		CPU_DEC (m_Regs.GetReg16(cmd));
	else
		CPU_DEC (m_Regs.GetReg32(cmd));

	DISASSEMBLE (("dec %reg"));
}
