/*
 *  cmd_calljump.cpp,v 1.2 2002/10/17 09:55:00 tim 
 */

#include "typedefs.h" 

/* JMP rel8/16/32 */
void I386CPU::Cmd_jmp_rels (BYTE cmd, bool fForce8bit, condition_t cond)
{
	SDWORD offset = 0;

	if ( fForce8bit )
		offset = (SDWORD) (SBYTE) ReadIx86Byte();	/* read signed byte */
	else if ( m_Regs.GetOperandSize() == 16 )
		offset = (SDWORD) (SWORD) ReadIx86Word();	/* read signed word */
	else
		offset = (SDWORD) ReadIx86DWord();			/* read signed dword */

	if ( m_Regs.CheckCondition (cond) )
		m_Regs.eip = (SDWORD) m_Regs.eip + offset;

	DISASSEMBLE (("j%cond %-lr", cond));
}


void I386CPU::Cmd_retn (BYTE cmd, bool fImm16Present)
{
	if ( m_Regs.GetOperandSize() == 16 )
		fatal (31, "16bit RETN is unsupported");

	// Check emulator exit condition
	if ( (DWORD) m_Regs.GetStackTop() == m_dwStackTop )
	{
		DISASSEMBLE (("retn (exit)"));
        fExitEmul = true;
        return;
	}

	// pop eip
	CPU_POP (&m_Regs.m_dwFoo);

	if ( m_Regs.m_dwFoo == 0 )
	{
		DISASSEMBLE (("retn (exit)"));
        fExitEmul = true;
        return;
	}

	if ( fImm16Present )
	{
		// Get imm16 words from stack top
		DWORD stacktop = (DWORD) m_Regs.GetStackTop();
        m_Regs.SetStackTop((void*)(stacktop + ReadIx86Word()));
		DISASSEMBLE (("retn %lw"));
	}
	else
	{
		DISASSEMBLE (("retn"));
	}

    m_Regs.eip = LittleEndianToMachine(m_Regs.m_dwFoo);
}



void I386CPU::Cmd_call (BYTE cmd, bool fimm32)
{
   	char buf[120];
   	DWORD addr = 0;

	if ( m_Regs.GetOperandSize() == 16 )
		fatal (31, "16bit CALL is unsupported");

	if ( fimm32 ) /* CALL near disp32 */
	{
		addr = (SDWORD) m_Regs.eip + (SDWORD) ReadIx86DWord();
        sprintf (buf, "call %%-ld");
	}
	else
	{
		addr = READ_X86_DWORD(m_Regs.GetModRMRMD(cmd)) ;
        sprintf (buf, "call %%mrm");
	}

	// push eip, converted to Intel byte order
	m_Regs.eip = MachineToLittleEndian(m_Regs.eip);
	CPU_PUSH (&m_Regs.eip);
	m_Regs.eip = addr;

#if defined (ENABLE_DISASM)
	const symbol_t * sym;

	if ( (sym = pSymbolTable->SymLookup(addr, SYM_CPU_X86)) != 0 )
		sprintf (buf, "call %%-ld (%s)", sym->name.c_str());
	
	DISASSEMBLE ((buf));
#endif
}


void I386CPU::Cmd_jcxz (BYTE cmd)
{
	DWORD value = 0;
	SDWORD offset = (SBYTE) ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
		value = READ_X86_WORD(m_Regs.GetReg16(CX));
	else
		value = READ_X86_DWORD(m_Regs.GetReg32(ECX));

	if ( value == 0 )
		m_Regs.eip = (SDWORD) m_Regs.eip + offset;

	DISASSEMBLE (("jcxz %-lr"));
}


void I386CPU::Cmd_loop (BYTE cmd, condition_t cond)
{
	SDWORD offset = (SDWORD) (SBYTE) ReadIx86Byte();	/* read signed byte */
    DWORD counter;

	if ( m_Regs.GetAddressSize() == 16 )
	{
		WORD * reg = m_Regs.GetReg16(CX);
		counter = ReadWordPtr(reg) - 1;
        WriteWordPtr(reg, (WORD) counter);
	}
	else
	{
		DWORD * reg = m_Regs.GetReg32(ECX);
		counter = READ_X86_DWORD (reg) - 1;
        WRITE_X86_DWORD (reg, counter);
	}

    if ( m_Regs.CheckCondition(cond) && counter != 0 )
		m_Regs.eip = (SDWORD) m_Regs.eip + offset;

	if ( cond != COND_ALWAYS )
		DISASSEMBLE (("loop%cond %-lb", cond));
	else
		DISASSEMBLE (("loop %-lb", cond));
}
