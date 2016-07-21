/*
 *  i386cpu.cpp,v 1.2 2002/10/17 09:55:02 tim 
 */

#include "typedefs.h"

#if defined (CALCTIME)
inline unsigned long usecdiff (struct timeval *a, struct timeval *b)
{
	unsigned long s;
	s = b->tv_sec - a->tv_sec;
	s *= 1000000;
	s += b->tv_usec - a->tv_usec;
	return s;
}
#endif


I386CPU::I386CPU()
{
	m_dwNestLevel = 0;
}


void I386CPU::Init (SymbolTable * pTable)
{
    pSymbolTable = pTable;
	m_Regs.Init (this);
}


void I386CPU::fatal (int ex_number, const char *fmt, ... )
{
    va_list ap;
    static char buf[512];

    pDisasm.EnableDisasm(true);

	if ( fmt )
	{
	    va_start(ap, fmt);
	    strcpy(buf, "\nFATAL ERROR: ");

#if defined (WIN32)
		_vsnprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), fmt, ap);
#else
		vsnprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), fmt, ap);
#endif

	    buf[sizeof(buf) - 1] = '\0';
	    va_end(ap);

	    pDisasm.Log ("%s\n", buf);
	}

	pDisasm.Log ("VM Dump\n");

	pDisasm.Log ("%08X:  ", m_dwOldEIP);
	for ( int i = 0; i < 15; i++ )
			pDisasm.Log ("%02X ", *((BYTE *) m_dwOldEIP + i));

	m_Regs.DumpRegs();
	
    CPUexception exc (ex_number);
    throw (exc);
}



void I386CPU::Execute (BYTE * codeptr)
{
	m_Regs.eip = (DWORD) codeptr;
	m_Regs.m_dwOpcodeFlags = 0;
    m_dwCmdCount = 0;
    fExitEmul = false;
    BYTE cmd;
	DWORD prefix;
#if defined (CALCTIME)
	struct timeval timestart, timeend;
	gettimeofday (&timestart, 0);
#endif

	while ( !fExitEmul )
	{
		m_dwOldEIP = m_Regs.eip;
        m_wCmdLength = 0;

		do
		{
			prefix = 0;
    		cmd = ReadIx86Byte();

    		/* Handle prefixes */
    		switch (cmd)
    		{
			/* ES register prefix */
			case 0x26:	
				prefix = PREFIX_ES; 
				abort();
				break;

			/* CS register prefix */
			case 0x2E:
				prefix = PREFIX_CS; 
				abort();
				break;

			/* SS register prefix */
			case 0x36:
				prefix = PREFIX_SS; 
				abort();
				break;

			/* DS register prefix */
			case 0x3E:
				prefix = PREFIX_DS; 
				break;

			/* FS register prefix */
			case 0x64:
				prefix = PREFIX_FS; 
				abort();
				break;

			/* GS register prefix */
			case 0x65:
				prefix = PREFIX_GS;
				abort();
				break;

			/* Operand-size prefix */
			case 0x66:
				prefix = PREFIX_OSIZE;
				break;

			/* Address-size prefix */
			case 0x67:
				prefix = PREFIX_ASIZE;
				break;

			/* REPNE/REPNZ prefix */
			case 0xF2:
				prefix = PREFIX_REPNE;
				break;

			/* REP/REPE/REPZ prefix */
			case 0xF3:
				prefix = PREFIX_REPE;
				break;
       		}

			SET_BIT (m_Regs.m_dwOpcodeFlags, prefix, true);

		} while ( prefix );

		ExecuteCmd (cmd);
        m_Regs.m_dwOpcodeFlags = 0;
        m_dwCmdCount++;

#if defined (ENABLE_DISASM)
		pDisasm.Log ("%08X:  %s  (", m_dwOldEIP, pDisasm.GetDisasm());
		for ( ; m_wCmdLength > 0; m_wCmdLength--, m_dwOldEIP++ )
			if ( m_wCmdLength == 1 )
				pDisasm.Log ("%02X", *((BYTE *) m_dwOldEIP));
			else
				pDisasm.Log ("%02X ", *((BYTE *) m_dwOldEIP));

		pDisasm.Log (")\n\n");
		m_Regs.DumpRegs();

#endif /* ENABLE_DISASM */
	}

#if defined (CALCTIME)
	gettimeofday (&timeend, 0);

	float total = (float) usecdiff(&timestart, &timeend) / 1000000;
	if ( total > 1.00f )
		printf ("Total instruction statistic: %d instructions, %.2f usec, %.2f per second\n",
			m_dwCmdCount, total, (float) m_dwCmdCount / total);
#endif /* CALCTIME */
}


void I386CPU::ExecuteCmd (BYTE cmd)
{
	DWORD dwRepCount = 1;
	bool fRep = false;

	/* Prepare to rep prefix */
	if ( IS_SET(m_Regs.m_dwOpcodeFlags, PREFIX_REPE)
	|| IS_SET(m_Regs.m_dwOpcodeFlags, PREFIX_REPNE) )
	{
		if ( m_Regs.GetAddressSize() == 16 )
			dwRepCount = (DWORD) ReadWordPtr(m_Regs.GetReg16(CX));
		else
			dwRepCount = (DWORD) ReadDWordPtr(m_Regs.GetReg32(ECX));

		fRep = true;
	}

	switch (cmd)
	{
    default:
    	fatal (6, "0x%02X: wrong cmd", cmd);

	/////////////////////////////////////////////////////////////////
	// ADD set

	/* ADD r8/m8, r8 */
	case 0x00:
		Cmd_add_rmx_rmx(cmd, true, 8);
		break;

	/* ADD rs/ms, rs */
	case 0x01:
		Cmd_add_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* ADD r8, r8/m8 */
	case 0x02:
		Cmd_add_rmx_rmx(cmd, false, 8);
		break;

	/* ADD rs/ms, rs */
	case 0x03:
		Cmd_add_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;

	/* ADD al, imm8 */
	case 0x04:
		Cmd_add_areg_imms(cmd, 8);
		break;

	/* ADD as, imms */
	case 0x05:
		Cmd_add_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* PUSH ES */
	case 0x06:
		Cmd_push_seg(ES);
		break;

	/* POP ES */
	case 0x07:
		Cmd_pop_seg(ES);
		break;


	/////////////////////////////////////////////////////////////////
	// OR set

	/* OR r8/m8, r8 */
	case 0x08:
		Cmd_or_rmx_rmx(cmd, true, 8);
		break;

	/* OR rs/ms, rs */
	case 0x09:
		Cmd_or_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* OR r8, r8/m8 */
	case 0x0A:
		Cmd_or_rmx_rmx(cmd, false, 8);
		break;

	/* OR rs/ms, rs */
	case 0x0B:
		Cmd_or_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;

	/* OR al, imm8 */
	case 0x0C:
		Cmd_or_areg_imms(cmd, 8);
		break;

	/* OR as, imms */
	case 0x0D:
		Cmd_or_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* unused */
	case 0x0E:
		break;

	/* Extended set */
	case 0x0F:
		Execute0FCmd(ReadIx86Byte());
		break;


	/////////////////////////////////////////////////////////////////
	// ADC set

	/* ADC r8/m8, r8 */
	case 0x10:
		Cmd_adc_rmx_rmx(cmd, true, 8);
		break;

	/* ADC rs/ms, rs */
	case 0x11:
		Cmd_adc_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* ADC r8, r8/m8 */
	case 0x12:
		Cmd_adc_rmx_rmx(cmd, false, 8);
		break;

	/* ADC rs/ms, rs */
	case 0x13:
		Cmd_adc_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;

	/* ADC al, imm8 */
	case 0x14:
		Cmd_adc_areg_imms(cmd, 8);
		break;

	/* ADC as, imms */
	case 0x15:
		Cmd_adc_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* PUSH SS */
	case 0x16:
		Cmd_push_seg(SS);
		break;

	/* POP SS */
	case 0x17:
		Cmd_pop_seg(SS);
		break;


	/////////////////////////////////////////////////////////////////
	// SBB set

	/* SBB r8/m8, r8 */
	case 0x18:
		Cmd_sbb_rmx_rmx(cmd, true, 8);
		break;

	/* SBB rs/ms, rs */
	case 0x19:
		Cmd_sbb_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* SBB r8, r8/m8 */
	case 0x1A:
		Cmd_sbb_rmx_rmx(cmd, false, 8);
		break;

	/* SBB rs/ms, rs */
	case 0x1B:
		Cmd_sbb_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;

	/* SBB al, imm8 */
	case 0x1C:
		Cmd_sbb_areg_imms(cmd, 8);
		break;

	/* SBB as, imms */
	case 0x1D:
		Cmd_sbb_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* PUSH DS */
	case 0x1E:
		Cmd_push_seg(DS);
		break;

	/* POP DS */
	case 0x1F:
		Cmd_pop_seg(DS);
		break;

	/////////////////////////////////////////////////////////////////
	// AND set

	/* AND r/m8, r8 */
	case 0x20:
		Cmd_and_rmx_rmx(cmd, true, 8);
		break;

	/* ADD r/ms, rs */
	case 0x21:
		Cmd_and_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* AND r8, r/m8 */
	case 0x22:
		Cmd_and_rmx_rmx(cmd, false, 8);
		break;

	/* AND rs, r/ms */
	case 0x23:
		Cmd_and_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;
	
	/* AND as, imms */
	case 0x24:
		Cmd_and_areg_imms(cmd, 8);
		break;

	/* AND as, imms */
	case 0x25:
		Cmd_and_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* seg ES: */
	case 0x26:	
		break;

	/* DAA */
	case 0x27:
   		Cmd_daa(cmd);
		break;


	/////////////////////////////////////////////////////////////////
	// SUB set

	/* SUB r/m8, r8 */
	case 0x28:
		Cmd_sub_rmx_rmx(cmd, true, 8);
		break;

	/* SUB r/ms, rs */
	case 0x29:
		Cmd_sub_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* SUB r8, r/m8 */
	case 0x2A:
		Cmd_sub_rmx_rmx(cmd, false, 8);
		break;

	/* SUB rs, r/ms */
	case 0x2B:
		Cmd_sub_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;
	
	/* SUB as, imms */
	case 0x2C:
		Cmd_sub_areg_imms(cmd, 8);
		break;

	/* SUB as, imms */
	case 0x2D:
		Cmd_sub_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* seg CS: */
	case 0x2E:
		break;

	/* DAS */
	case 0x2F:
   		Cmd_das(cmd);
		break;

	/////////////////////////////////////////////////////////////////
	// XOR set

	/* XOR r/m8, r8 */
	case 0x30:
		Cmd_xor_rmx_rmx(cmd, true, 8);
		break;

	/* XOR r/ms, rs */
	case 0x31:
		Cmd_xor_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* XOR r8, r/m8 */
	case 0x32:
		Cmd_xor_rmx_rmx(cmd, false, 8);
		break;

	/* XOR rs, r/ms */
	case 0x33:
		Cmd_xor_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;
	
	/* XOR as, imms */
	case 0x34:
		Cmd_xor_areg_imms(cmd, 8);
		break;

	/* XOR as, imms */
	case 0x35:
		Cmd_xor_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* seg SS: */
	case 0x36:
		break;

	/* AAA */
	case 0x37:
   		Cmd_aaa(cmd);
		break;

	/////////////////////////////////////////////////////////////////
	// CMP set

	/* CMP r/m8, r8 */
	case 0x38:
		Cmd_cmp_rmx_rmx(cmd, true, 8);
		break;

	/* CMP r/ms, rs */
	case 0x39:
		Cmd_cmp_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* CMP r8, r/m8 */
	case 0x3A:
		Cmd_cmp_rmx_rmx(cmd, false, 8);
		break;

	/* CMP rs, r/ms */
	case 0x3B:
		Cmd_cmp_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;
	
	/* CMP as, imms */
	case 0x3C:
		Cmd_cmp_areg_imms(cmd, 8);
		break;

	/* CMP as, imms */
	case 0x3D:
		Cmd_cmp_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* seg DS: */
	case 0x3E:
		break;

	/* AAS */
	case 0x3F:
   		Cmd_aas(cmd);
		break;

	/////////////////////////////////////////////////////////////////
	//  INC/DEC set

	/* INC rs */
	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
		Cmd_inc_rs(cmd);
		break;

	/* DEC rs */
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x4F:
		Cmd_dec_rs(cmd);
		break;

	/////////////////////////////////////////////////////////////////
	//  PUSH/POP set
	/* INC rs */
	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
		Cmd_push_reg(cmd);
		break;

	/* DEC rs */
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F:
		Cmd_pop_reg(cmd);
		break;

	/* PUSHA(D) */
	case 0x60:
		Cmd_pusha(cmd);
		break;

	/* POPA(D) */
	case 0x61:
		Cmd_popa(cmd);
		break;

	/* BOUND */
	case 0x62:
		Cmd_notimplemented(cmd, "BOUND");
		break;

	/* ARPL */
	case 0x63:
		Cmd_notimplemented(cmd, "ARPL");
		break;

	/* seg FS: */
	case 0x64:
		break;

	/* seg GS: */
	case 0x65:
		break;

	/* Operand-size prefix */
	case 0x66:
		break;

	/* Address-size prefix */
	case 0x67:
		break;

	/* PUSH imms */
	case 0x68:
		Cmd_push_imms(cmd, false);
		break;

	/* IMUL rs, r/ms, imms */
	case 0x69:
		Cmd_imul (cmd, false, true);
		break;

	/* PUSH imm8 */
	case 0x6A:
		Cmd_push_imms(cmd, true);
		break;

	/* IMUL rs, r/ms, imm8 */
	case 0x6B:
		Cmd_imul (cmd, true, true);
		break;

	/* INSB */
	case 0x6C:
		Cmd_notimplemented(cmd, "INSB");
		break;

	/* INSW/D */
	case 0x6D:
		Cmd_notimplemented(cmd, "INSW/D");
		break;

	/* OUTSB */
	case 0x6E:
		Cmd_notimplemented(cmd, "OUTSB");
		break;

	/* OUTSW/D */
	case 0x6F:
		Cmd_notimplemented(cmd, "OUTSW/D");
		break;

	/* Jcc set */
	case 0x70:
		Cmd_jmp_rels (cmd, true, COND_O);
		break;

	case 0x71:
		Cmd_jmp_rels (cmd, true, COND_NO);
		break;

	case 0x72:
		Cmd_jmp_rels (cmd, true, COND_B_NAE);
		break;

	case 0x73:
		Cmd_jmp_rels (cmd, true, COND_AE_NB);
		break;

	case 0x74:
		Cmd_jmp_rels (cmd, true, COND_E_Z);
		break;

	case 0x75:
		Cmd_jmp_rels (cmd, true, COND_NE_NZ);
		break;

	case 0x76:
		Cmd_jmp_rels (cmd, true, COND_BE_NA);
		break;

	case 0x77:
		Cmd_jmp_rels (cmd, true, COND_A_NBE);
		break;

	case 0x78:
		Cmd_jmp_rels (cmd, true, COND_S);
		break;

	case 0x79:
		Cmd_jmp_rels (cmd, true, COND_NS);
		break;

	case 0x7A:
		Cmd_jmp_rels (cmd, true, COND_P_PE);
		break;

	case 0x7B:
		Cmd_jmp_rels (cmd, true, COND_NP_PO);
		break;

	case 0x7C:
		Cmd_jmp_rels (cmd, true, COND_L_NGE);
		break;

	case 0x7D:
		Cmd_jmp_rels (cmd, true, COND_GE_NL);
		break;

	case 0x7E:
		Cmd_jmp_rels (cmd, true, COND_LE_NG);
		break;

	case 0x7F:
		Cmd_jmp_rels (cmd, true, COND_G_NLE);
		break;


	/////////////////////////////////////////////////////////////////
	// Instruction set

	/* ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r/m8,imm8 */
	case 0x80:
		Cmd_0x80(cmd);
		break;

	/* ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r/mS,immS */
	case 0x81:
		Cmd_0x81_0x83(cmd, false);
		break;

	case 0x82:
		Cmd_0x80(cmd);
		break;

	/* ADD/OR/ADC/SBB/AND/SUB/XOR/CMP r/mS,imm8 */
	case 0x83:
		Cmd_0x81_0x83(cmd, true);
		break;

	/* TEST r/m8, r8 */
	case 0x84:
		Cmd_test_rmx_rmx(cmd, true, 8);
		break;

	/* TEST r/ms, rs */
	case 0x85:
		Cmd_test_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* XCHG r/m8, r8 */
	case 0x86:
		Cmd_xchg_rms_rs(cmd, true);
		break;

	/* XCHG r/ms, rs */
	case 0x87:
		Cmd_xchg_rms_rs(cmd, false);
		break;

	/* MOV r8/m8, r8 */
	case 0x88:
		Cmd_mov_rmx_rmx(cmd, true, 8);
		break;

	/* MOV rs/ms, rs */
	case 0x89:
		Cmd_mov_rmx_rmx(cmd, true, m_Regs.GetOperandSize());
		break;

	/* MOV r8, r/m8 */
	case 0x8A:
		Cmd_mov_rmx_rmx(cmd, false, 8);
		break;

	/* MOV rs, r/ms */
	case 0x8B:
		Cmd_mov_rmx_rmx(cmd, false, m_Regs.GetOperandSize());
		break;

	/* MOV r/m16, sred */
	case 0x8C:
		Cmd_mov_rsreg_rsreg(cmd, false);
		break;

	/* LEA r/ms, ms */
	case 0x8D:
		Cmd_lea(cmd);
		break;

	/* MOV sred, r/m16 */
	case 0x8E:
		Cmd_mov_rsreg_rsreg(cmd, true);
		break;

	/* POP ms (SET, but for now only POP is here) */
	case 0x8F:
		Cmd_pop_mem(cmd);
		break;

	/* NOP */
	case 0x90:
   		DISASSEMBLE (("nop"));
		break;

	/* XCHG ax, regs */
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
		Cmd_xchg_regs(cmd);
		break;

    /* CWD/CDQ */
	case 0x98:
   		Cmd_cbw_cwde(cmd);
		break;

	/* CWD/CDQ */
	case 0x99:
   		Cmd_cwd_cdq(cmd);
		break;

	/* CALL Far */
	case 0x9A:
		Cmd_notimplemented(cmd, "CALL FAR");
		break;

	/* WAIT */
	case 0x9B:
		break;

	/* PUSHF */
	case 0x9C:
		Cmd_pushf(cmd);
		break;

	/* POPF */
	case 0x9D:
		Cmd_popf(cmd);
		break;

	/* SAHF */
	case 0x9E:
		Cmd_sahf(cmd);
		break;

	/* LAHF */
	case 0x9F:
		Cmd_lahf(cmd);
		break;

    /* MOV al, moffs8 */
	case 0xA0:
		Cmd_mov_aORoffs_aORoffs(cmd, true, 8);
		break;

	/* MOV aS, moffsS */
	case 0xA1:
		Cmd_mov_aORoffs_aORoffs(cmd, true, m_Regs.GetOperandSize());
		break;

    /* MOV moffs8, al */
	case 0xA2:
		Cmd_mov_aORoffs_aORoffs(cmd, false, 8);
		break;

	/* MOV moffsS, aS */
	case 0xA3:
		Cmd_mov_aORoffs_aORoffs(cmd, false, m_Regs.GetOperandSize());
		break;

	/* MOVSB */
	case 0xA4:
		Cmd_movss(cmd, 8, dwRepCount);
		break;

	/* MOVSW/MOVSD */
	case 0xA5:
		Cmd_movss(cmd, m_Regs.GetOperandSize(), dwRepCount);
		break;

	/* CMPSB */
	case 0xA6:
		Cmd_cmpss(cmd, 8, dwRepCount, fRep);
		break;

	/* CMPSW/CMPSD */
	case 0xA7:
		Cmd_cmpss(cmd, m_Regs.GetOperandSize(), dwRepCount, fRep);
		break;

	/* TEST al, imm8 */
	case 0xA8:
		Cmd_test_areg_imms(cmd, 8);
		break;

	/* TEST as, imms */
	case 0xA9:
		Cmd_test_areg_imms(cmd, m_Regs.GetOperandSize());
		break;

	/* STOSB */
	case 0xAA:
		Cmd_stoss(cmd, 8, dwRepCount);
		break;

	/* STOSW/STOSD */
	case 0xAB:
		Cmd_stoss(cmd, m_Regs.GetOperandSize(), dwRepCount);
		break;

	/* LODSB */
	case 0xAC:
		Cmd_lodss(cmd, 8, dwRepCount);
		break;

	/* LODSW/LODSD */
	case 0xAD:
		Cmd_lodss(cmd, m_Regs.GetOperandSize(), dwRepCount);
		break;

	/* SCASB */
	case 0xAE:
		Cmd_scass(cmd, 8, dwRepCount, fRep);
		break;

	/* SCASW/SCASD */
	case 0xAF:
		Cmd_scass(cmd, m_Regs.GetOperandSize(), dwRepCount, fRep);
		break;

	/* MOV reg8, imm8 */
	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:
		Cmd_mov_regs_imms(cmd, 8);
		break;

	/* MOV regs, imms */
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	case 0xBC:
	case 0xBD:
	case 0xBE:
	case 0xBF:
		Cmd_mov_regs_imms(cmd, m_Regs.GetOperandSize());
		break;

	case 0xC0:
    	Cmd_0xC0(cmd);
    	break;

	case 0xC1:
    	Cmd_0xC1(cmd);
    	break;

	/* RETN imm16 */
	case 0xC2:
		Cmd_retn (cmd, true);
		break;

	/* RETN */
	case 0xC3:
		Cmd_retn (cmd, false);
		break;

	/* LES */
	case 0xC4:
		Cmd_notimplemented(cmd, "LES");
		break;

	/* LDS */
	case 0xC5:
		Cmd_notimplemented(cmd, "LDS");
		break;

	/* MOV r/m8, imm8 */
	case 0xC6:
		Cmd_mov_rms_imms (cmd, 8);
		break;

	/* MOV r/ms, imms */
	case 0xC7:
		Cmd_mov_rms_imms (cmd, m_Regs.GetOperandSize());
		break;

	/* ENTER imm16 imm8 */
	case 0xC8:
		Cmd_notimplemented(cmd, "ENTER");
		break;

	/* LEAVE */
	case 0xC9:
		Cmd_leave();
		break;

	/* RET Far */
	case 0xCA:
		Cmd_notimplemented(cmd, "RET FAR");
		break;

	/* RET Far */
	case 0xCB:
		Cmd_notimplemented(cmd, "RET imm16 FAR");
		break;

	/* INT 3 */
	case 0xCC:
		Cmd_notimplemented(cmd, "INT 3");
		break;

	/* INT xx */
	case 0xCD:
		Cmd_notimplemented(cmd, "INT xx");
		break;

	/* INTO */
	case 0xCE:
		Cmd_notimplemented(cmd, "INTO");
		break;

	/* IRET */
	case 0xCF:
		Cmd_notimplemented(cmd, "IRET");
		break;

	case 0xD0:
    	Cmd_0xD0(cmd);
    	break;

	case 0xD1:
    	Cmd_0xD1(cmd);
    	break;

	case 0xD2:
    	Cmd_0xD2(cmd);
    	break;

	case 0xD3:
    	Cmd_0xD3(cmd);
    	break;

	/* AAM */
	case 0xD4:
   		Cmd_aam(cmd);
		break;

	/* AAD */
	case 0xD5:
   		Cmd_aad(cmd);
		break;

	/* SETALC */
	case 0xD6:
   		Cmd_setalc(cmd);
		break;

	/* XLAT */
	case 0xD7:
   		Cmd_xlat(cmd);
		break;

	/* FPU cmds */
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
		fatal (7, "FPU extension isn't supported");
		break;

	/* LOOPNE */
	case 0xE0:
		Cmd_loop (cmd, COND_NE_NZ);
		break;

	/* LOOPE */
	case 0xE1:
		Cmd_loop (cmd, COND_E_Z);
		break;

	/* LOOP */
	case 0xE2:
		Cmd_loop (cmd, COND_ALWAYS);
		break;

	/* JCXZ */
	case 0xE3:
		Cmd_jcxz (cmd);
		break;

	/* call near branch */
	case 0xE8:
		Cmd_call (cmd, true);
		break;

	/* jmp rels */
	case 0xE9:
		Cmd_jmp_rels (cmd, false, COND_ALWAYS);
		break;

	/* jmp rel8 */
	case 0xEB:
		Cmd_jmp_rels (cmd, true, COND_ALWAYS);
		break;

	/* HLT */
	case 0xF4:
		Cmd_wrapper(cmd);
		break;

	/* CMC */
	case 0xF5:
		Cmd_CFops(cmd);
		break;

	/* TEST/NOT/NEG/MUL/IMUL/DIV/IDIV r8, r/m8 */
	case 0xF6:
		Cmd_0xF6(cmd);
		break;

	/* TEST/NOT/NEG/MUL/IMUL/DIV/IDIV rs, r/ms */
	case 0xF7:
		Cmd_0xF7(cmd);
		break;

	/* CLC */
	case 0xF8:
		Cmd_CFops(cmd);
		break;

	/* STC */
	case 0xF9:
		Cmd_CFops(cmd);
		break;

	/* CLI */
	case 0xFA:
		Cmd_IFops(cmd);
		break;

	/* STI */
	case 0xFB:
		Cmd_IFops(cmd);
		break;

	/* CLD */
	case 0xFC:
		Cmd_DFops(cmd);
		break;

	/* STD */
	case 0xFD:
		Cmd_DFops(cmd);
		break;

	/* INC/DEC r/m8 */
	case 0xFE:
		Cmd_0xFE(cmd);
		break;

	/* INC/DEC/CALL/CALL FAR/JMP/JMP FAR/PUSH */
	case 0xFF:
		Cmd_0xFF(cmd);
		break;
	}

	if ( fRep )
	{
		if ( m_Regs.GetAddressSize() == 16 )
			WriteWordPtr(m_Regs.GetReg16(CX), (WORD) dwRepCount);
		else
			WriteDWordPtr(m_Regs.GetReg32(ECX), dwRepCount);
	}
}



void I386CPU::Execute0FCmd (BYTE cmd)
{
	switch (cmd)
	{
    default:
    	fatal (6, "0x0F 0x%02X: wrong cmd", cmd);

	case 0x80:
		Cmd_jmp_rels (cmd, false, COND_O);
		break;

	case 0x81:
		Cmd_jmp_rels (cmd, false, COND_NO);
		break;

	case 0x82:
		Cmd_jmp_rels (cmd, false, COND_B_NAE);
		break;

	case 0x83:
		Cmd_jmp_rels (cmd, false, COND_AE_NB);
		break;

	case 0x84:
		Cmd_jmp_rels (cmd, false, COND_E_Z);
		break;

	case 0x85:
		Cmd_jmp_rels (cmd, false, COND_NE_NZ);
		break;

	case 0x86:
		Cmd_jmp_rels (cmd, false, COND_BE_NA);
		break;

	case 0x87:
		Cmd_jmp_rels (cmd, false, COND_A_NBE);
		break;

	case 0x88:
		Cmd_jmp_rels (cmd, false, COND_S);
		break;

	case 0x89:
		Cmd_jmp_rels (cmd, false, COND_NS);
		break;

	case 0x8A:
		Cmd_jmp_rels (cmd, false, COND_P_PE);
		break;

	case 0x8B:
		Cmd_jmp_rels (cmd, false, COND_NP_PO);
		break;

	case 0x8C:
		Cmd_jmp_rels (cmd, false, COND_L_NGE);
		break;

	case 0x8D:
		Cmd_jmp_rels (cmd, false, COND_GE_NL);
		break;

	case 0x8E:
		Cmd_jmp_rels (cmd, false, COND_LE_NG);
		break;

	case 0x8F:
		Cmd_jmp_rels (cmd, false, COND_G_NLE);
		break;

	/////////////////////////////////////////////////////////////////
	//  SETcc
	case 0x90:
		Cmd_setcc_rms (cmd, COND_O);
		break;

	case 0x91:
		Cmd_setcc_rms (cmd, COND_NO);
		break;

	case 0x92:
		Cmd_setcc_rms (cmd, COND_B_NAE);
		break;

	case 0x93:
		Cmd_setcc_rms (cmd, COND_AE_NB);
		break;

	case 0x94:
		Cmd_setcc_rms (cmd, COND_E_Z);
		break;

	case 0x95:
		Cmd_setcc_rms (cmd, COND_NE_NZ);
		break;

	case 0x96:
		Cmd_setcc_rms (cmd, COND_BE_NA);
		break;

	case 0x97:
		Cmd_setcc_rms (cmd, COND_A_NBE);
		break;

	case 0x98:
		Cmd_setcc_rms (cmd, COND_S);
		break;

	case 0x99:
		Cmd_setcc_rms (cmd, COND_NS);
		break;

	case 0x9A:
		Cmd_setcc_rms (cmd, COND_P_PE);
		break;

	case 0x9B:
		Cmd_setcc_rms (cmd, COND_NP_PO);
		break;

	case 0x9C:
		Cmd_setcc_rms (cmd, COND_L_NGE);
		break;

	case 0x9D:
		Cmd_setcc_rms (cmd, COND_GE_NL);
		break;

	case 0x9E:
		Cmd_setcc_rms (cmd, COND_LE_NG);
		break;

	case 0x9F:
		Cmd_setcc_rms (cmd, COND_G_NLE);
		break;

	/* PUSH FS */
	case 0xA0:
		Cmd_push_seg(FS);
		break;

	/* POP FS */
	case 0xA1:
		Cmd_pop_seg(FS);
		break;

	case 0xA3:
		Cmd_bt_rms_rs(cmd);
		break;

	case 0xA4:
		Cmd_shld_rms_rs(cmd);
		break;

	case 0xA5:
		Cmd_shld_rms_rs(cmd);
		break;
		
	/* PUSH GS */
	case 0xA8:
		Cmd_push_seg(GS);
		break;

	/* POP GS */
	case 0xA9:
		Cmd_pop_seg(GS);
		break;

	case 0xAB:
		Cmd_bts_rms_rs(cmd);
		break;

	case 0xAF:
		Cmd_imul(cmd, false, false);
		break;

	case 0xB3:
		Cmd_btr_rms_rs(cmd);
		break;

	case 0xB6:
		Cmd_movzx_rs_rms(cmd, 8);
		break;

	case 0xB7:
		Cmd_movzx_rs_rms(cmd, 16);
		break;

	case 0xBA:
		Cmd_0x0F_0xBA(cmd);
		break;

	case 0xBB:
		Cmd_btc_rms_rs(cmd);
		break;

	case 0xBC:
		Cmd_bsf_rs_rms(cmd);
		break;

	case 0xBD:
		Cmd_bsr_rs_rms(cmd);
		break;

	case 0xBE:
		Cmd_movsx_rs_rms(cmd, 8);
		break;

	case 0xBF:
		Cmd_movsx_rs_rms(cmd, 16);
		break;

	/* BSWAP r32 */
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
		Cmd_bswap_reg32(cmd);
		break;
	}
}
