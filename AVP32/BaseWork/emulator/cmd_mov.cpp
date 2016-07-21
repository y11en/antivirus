#ifndef lint
static const char rcsid[] =
    " cmd_mov.cpp,v 1.2 2002/10/17 09:55:00 tim ";
#endif

#include "typedefs.h"


/*
 *  MOV commands
 */

void I386CPU::Cmd_mov_regs_imms (BYTE cmd, int size)
{
	switch (size)
	{
	case 8:
		WRITE_X86_BYTE (m_Regs.GetReg8(cmd), ReadIx86Byte());
		DISASSEMBLE (("mov %r8, %lb"));
		break;

	case 16:
	    WRITE_X86_WORD (m_Regs.GetReg16(cmd), ReadIx86Word());
		DISASSEMBLE (("mov %r16, %lw"));
		break;

	default:	// 32
	    WRITE_X86_DWORD (m_Regs.GetReg32(cmd), ReadIx86DWord());
		DISASSEMBLE (("mov %r32, %ld"));
		break;
	}
}


void I386CPU::Cmd_mov_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size)
{
    BYTE mod_rm = ReadIx86Byte();

	switch (size)
	{
	case 8:
		if ( fForceRMsRs )
			CPU_MOV (m_Regs.GetModRMRMB (mod_rm), m_Regs.GetModRMRegB (mod_rm));
		else
			CPU_MOV (m_Regs.GetModRMRegB (mod_rm), m_Regs.GetModRMRMB (mod_rm));
		break;

	case 16:
		if ( fForceRMsRs )
			CPU_MOV (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm));
		else
			CPU_MOV (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
		break;

	default:
		if ( fForceRMsRs )
			CPU_MOV (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm));
		else
			CPU_MOV (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));
		break;
	}

	if ( fForceRMsRs )
		DISASSEMBLE (("mov %mrm, %mreg"));
	else
		DISASSEMBLE (("mov %mreg, %mrm"));
}



void I386CPU::Cmd_mov_rsreg_rsreg (BYTE cmd, bool fForceSreg_rm16)
{
    BYTE mod_rm = ReadIx86Byte();
    WORD * sreg = m_Regs.GetSegReg((mod_rm & 0x38) >> 3);

    if ( fForceSreg_rm16 )
    {
    	CPU_MOV (sreg, m_Regs.GetModRMRMW (mod_rm));
        DISASSEMBLE (("mov %sreg, %mrm"));
	}
	else
	{
    	CPU_MOV (m_Regs.GetModRMRMW (mod_rm), sreg);
		DISASSEMBLE (("mov %mrm, %sreg"));
	}
}


void I386CPU::Cmd_lea (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();
	DWORD address = (DWORD) m_Regs.GetModRmAddr(mod_rm);

	if ( m_Regs.GetOperandSize() == 16 )
	{
        WRITE_X86_DATA (m_Regs.GetModRMRegW(mod_rm), (WORD) address);
		DISASSEMBLE (("lea %mreg, %mrm"));
	}
	else
	{
		WRITE_X86_DATA (m_Regs.GetModRMRegD(mod_rm), address);
		DISASSEMBLE (("lea %mreg, %mrm"));
	}
}


void I386CPU::Cmd_mov_aORoffs_aORoffs (BYTE cmd, bool fForceAregOffs, int size)
{
	DWORD offset = ReadIx86Size();

	switch (size)
	{
	case 8:
		if ( fForceAregOffs )
			CPU_MOV (m_Regs.GetReg8(AL),(BYTE *) offset);
		else
			CPU_MOV ((BYTE *) offset, m_Regs.GetReg8(AL));
		break;

	case 16:
		if ( fForceAregOffs )
			CPU_MOV (m_Regs.GetReg16(AX), (WORD *) offset);
		else
			CPU_MOV ((WORD *) offset, m_Regs.GetReg16(AX));
		break;
	
	default:
		if ( fForceAregOffs )
			CPU_MOV (m_Regs.GetReg32(EAX), (DWORD *) offset);
		else
			CPU_MOV ((DWORD *) offset, m_Regs.GetReg32(EAX));
		break;
	}

	if ( fForceAregOffs )
		DISASSEMBLE (("mov %reg, [%lr]"));
	else
		DISASSEMBLE (("mov [%lr], %reg"));
}



void I386CPU::Cmd_mov_rms_imms (BYTE cmd, int size)
{
    BYTE mod_rm = ReadIx86Byte();

    // Fucked VC calculates second argument first :(
	if ( size == 8 )
	{
		BYTE * ptr = m_Regs.GetModRMRMB(mod_rm);
		WRITE_X86_DATA (ptr, ReadIx86Byte());
	}
	else if ( size == 16 )
	{
		WORD * ptr = m_Regs.GetModRMRMW(mod_rm);
		WRITE_X86_DATA (ptr, ReadIx86Word());
	}
	else
	{
		DWORD * ptr = m_Regs.GetModRMRMD(mod_rm);
		WRITE_X86_DATA (ptr, ReadIx86DWord());
	}

	DISASSEMBLE (("mov %mrm, %lr"));
}


void I386CPU::Cmd_movsx_rs_rms (BYTE cmd, int size)
{
	BYTE mod_rm = ReadIx86Byte();

    if ( size == 8 )
    {
    	BYTE src = READ_X86_BYTE (m_Regs.GetModRMRMB(mod_rm));

    	if ( m_Regs.GetOperandSize() == 16 )
    		WRITE_X86_WORD(m_Regs.GetModRMRegW(mod_rm), src & 0x80 ? src | 0xFF00 : src & 0x00FF);
		else
			WRITE_X86_DWORD(m_Regs.GetModRMRegD(mod_rm), src & 0x80 ? src | 0xFFFFFF00 : src & 0x000000FF);
	}
	else
	{
		WORD src = READ_X86_WORD (m_Regs.GetModRMRMW(mod_rm));

    	if ( m_Regs.GetOperandSize() == 16 )
    		WRITE_X86_WORD(m_Regs.GetModRMRegW(mod_rm), src);
		else
			WRITE_X86_DWORD(m_Regs.GetModRMRegD(mod_rm), src & 0x8000 ? src | 0xFFFF0000 : src & 0x0000FFFF);
	}

	DISASSEMBLE (("movsx %mreg, %mrm"));
}


void I386CPU::Cmd_movzx_rs_rms (BYTE cmd, int size)
{
	BYTE mod_rm = ReadIx86Byte();

    if ( size == 8 )
    {
    	BYTE src = READ_X86_BYTE (m_Regs.GetModRMRMB(mod_rm));

    	if ( m_Regs.GetOperandSize() == 16 )
    		WRITE_X86_WORD(m_Regs.GetModRMRegW(mod_rm), src & 0x00FF);
		else
			WRITE_X86_DWORD(m_Regs.GetModRMRegD(mod_rm), src & 0x000000FF);
	}
	else
	{
		WORD src = READ_X86_WORD (m_Regs.GetModRMRMW(mod_rm));

    	if ( m_Regs.GetOperandSize() == 16 )
    		WRITE_X86_WORD (m_Regs.GetModRMRegW(mod_rm), src);
		else
			WRITE_X86_DWORD (m_Regs.GetModRMRegD(mod_rm), src & 0x0000FFFF);
	}

	DISASSEMBLE (("movzx %mreg, %mrm"));
}
