/*
 *  cmd_stack.cpp,v 1.2 2002/10/17 09:55:00 tim 
 */

#include "typedefs.h"


/* POP segreg */
void I386CPU::Cmd_pop_seg (int segreg)
{
    CPU_POP (m_Regs.GetSegReg(segreg));
	DISASSEMBLE (("pop %sreg"));
}


/* POP reg */
void I386CPU::Cmd_pop_reg (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
		CPU_POP (m_Regs.GetReg16(cmd));
	else
		CPU_POP (m_Regs.GetReg32(cmd));

	DISASSEMBLE (("pop %reg"));
}


void I386CPU::Cmd_push_seg (int segreg)
{
    CPU_PUSH (m_Regs.GetSegReg(segreg));
	DISASSEMBLE (("push %sreg"));
}


void I386CPU::Cmd_push_reg (BYTE reg)
{
	if ( m_Regs.GetOperandSize() == 16 )
		CPU_PUSH (m_Regs.GetReg16(reg));
	else
		CPU_PUSH (m_Regs.GetReg32(reg));

	DISASSEMBLE (("push %reg"));
}


void I386CPU::Cmd_push_imms	(BYTE cmd, bool fForce8bit)
{
	if ( m_Regs.GetOperandSize() == 16 )
		fatal (31, "Cmd_push_imms: 16bit operand");

	if ( fForce8bit )
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, (DWORD) ((SDWORD) ((SBYTE) ReadIx86Byte())));
	else if ( m_Regs.GetOperandSize() == 16 )
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, (DWORD) ((SDWORD) ((SWORD) ReadIx86Word())));
	else
		WRITE_X86_DWORD (&m_Regs.m_dwFoo, ReadIx86DWord());

	DISASSEMBLE (("push %lr"));
	CPU_PUSH (&m_Regs.m_dwFoo);
}


void I386CPU::Cmd_pop_mem (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

    // CHECK: address size?
    if ( m_Regs.GetOperandSize() == 16 )
    	CPU_POP (m_Regs.GetModRMRMW(mod_rm));
	else
    	CPU_POP (m_Regs.GetModRMRMD(mod_rm));

	DISASSEMBLE (("pop %mrm"));
}


void I386CPU::Cmd_pusha (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
	{
		DISASSEMBLE (("pusha"));

        // We have to keep old SP value to store it (according to Intel manual)
        WRITE_X86_WORD (&m_Regs.m_wFoo, READ_X86_WORD (m_Regs.GetReg16(SP)));

		CPU_PUSH (m_Regs.GetReg16(AX));
		CPU_PUSH (m_Regs.GetReg16(CX));
		CPU_PUSH (m_Regs.GetReg16(DX));
		CPU_PUSH (m_Regs.GetReg16(BX));
		CPU_PUSH (&m_Regs.m_wFoo);	// Store SP
		CPU_PUSH (m_Regs.GetReg16(BP));
		CPU_PUSH (m_Regs.GetReg16(SI));
		CPU_PUSH (m_Regs.GetReg16(DI));
	}
	else
	{
		DISASSEMBLE (("pushad"));

        WRITE_X86_DWORD (&m_Regs.m_dwFoo, READ_X86_DWORD (m_Regs.GetReg32(ESP)));

		CPU_PUSH (m_Regs.GetReg32(EAX));
		CPU_PUSH (m_Regs.GetReg32(ECX));
		CPU_PUSH (m_Regs.GetReg32(EDX));
		CPU_PUSH (m_Regs.GetReg32(EBX));
		CPU_PUSH (&m_Regs.m_dwFoo);	// old ESP
		CPU_PUSH (m_Regs.GetReg32(EBP));
		CPU_PUSH (m_Regs.GetReg32(ESI));
		CPU_PUSH (m_Regs.GetReg32(EDI));
	}
}


void I386CPU::Cmd_popa (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
	{
		DISASSEMBLE (("popa"));

		CPU_POP (m_Regs.GetReg16(DI));
		CPU_POP (m_Regs.GetReg16(SI));
		CPU_POP (m_Regs.GetReg16(BP));
		CPU_POP (&m_Regs.m_wFoo);	// dummy SP value
		CPU_POP (m_Regs.GetReg16(BX));
		CPU_POP (m_Regs.GetReg16(DX));
		CPU_POP (m_Regs.GetReg16(CX));
		CPU_POP (m_Regs.GetReg16(AX));
	}
	else
	{
		DISASSEMBLE (("popad"));
		CPU_POP (m_Regs.GetReg32(EDI));
		CPU_POP (m_Regs.GetReg32(ESI));
		CPU_POP (m_Regs.GetReg32(EBP));
		CPU_POP (&m_Regs.m_dwFoo);	// dummy ESP value
		CPU_POP (m_Regs.GetReg32(EBX));
		CPU_POP (m_Regs.GetReg32(EDX));
		CPU_POP (m_Regs.GetReg32(ECX));
		CPU_POP (m_Regs.GetReg32(EAX));
	}
}


void I386CPU::Cmd_pushf (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
	{
		DISASSEMBLE (("pushf"));
		WriteWordPtr (&m_Regs.m_wFoo, (WORD)m_Regs.eflags);
	    CPU_PUSH (&m_Regs.m_wFoo);
	}
	else
	{
		DISASSEMBLE (("pushfd"));
	    CPU_PUSH (&m_Regs.eflags);
	}
}


void I386CPU::Cmd_popf (BYTE cmd)
{
	if ( m_Regs.GetOperandSize() == 16 )
	{
		DISASSEMBLE (("popf"));
	    CPU_POP (&m_Regs.m_wFoo);
		m_Regs.eflags &= 0xFFFF0000;
		m_Regs.eflags |= ReadWordPtr(&m_Regs.m_wFoo);
	}
	else
	{
		DISASSEMBLE (("popfd"));
	    CPU_POP (&m_Regs.eflags);
	}
}


void I386CPU::Cmd_leave	()
{
	if ( m_Regs.GetOperandSize() == 16 )
		fatal (31, "Cmd_leave: 16bit operand");

	CPU_MOV (m_Regs.GetReg32(ESP), m_Regs.GetReg32(EBP));
    CPU_POP (m_Regs.GetReg32(EBP));
    DISASSEMBLE (("leave"));
}
