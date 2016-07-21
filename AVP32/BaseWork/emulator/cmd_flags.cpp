/*
 *  cmd_flags.cpp,v 1.2 2002/10/17 09:55:00 tim 
 */

#include "typedefs.h"

/*
 * CPU flags manipilation commands
 */
void I386CPU::Cmd_IFops (BYTE cmd)
{
	DISASSEMBLE ((cmd == 0xFA ? "cli" : "sti"));
	SET_BIT (m_Regs.eflags, IF, cmd == 0xFA);	// CLI
}

void I386CPU::Cmd_DFops (BYTE cmd)
{
	DISASSEMBLE ((cmd == 0xFD ? "std" : "cld"));
	SET_BIT (m_Regs.eflags, DF, cmd == 0xFD);	// STD
}

void I386CPU::Cmd_CFops (BYTE cmd)
{
	if ( cmd == 0xF5 ) // CMC
	{
		DISASSEMBLE (("cmc"));
		SET_BIT (m_Regs.eflags, CF, !IS_SET(m_Regs.eflags, CF));
	}
	else
	{
    	DISASSEMBLE (("stc"));
		SET_BIT (m_Regs.eflags, CF, cmd == 0xF9); // STC
	}
}


void I386CPU::Cmd_lahf (BYTE cmd)
{
    WRITE_X86_BYTE (m_Regs.GetReg8(AH), (BYTE) m_Regs.eflags);
	DISASSEMBLE (("lahf"));
}


void I386CPU::Cmd_sahf (BYTE cmd)
{
    m_Regs.eflags = (m_Regs.eflags & 0xFFFFFF00) | (DWORD) READ_X86_BYTE (m_Regs.GetReg8(AH));
	DISASSEMBLE (("sahf"));
}


void I386CPU::Cmd_setalc (BYTE cmd)
{
	WRITE_X86_BYTE (m_Regs.GetReg8(AH), IS_SET(m_Regs.eflags, CF) ? 0xFF : 0);
	DISASSEMBLE (("setalc"));
}
