/*
 *  i386regs.cpp,v 1.2 2002/10/17 09:55:03 tim 
 */

#include "typedefs.h"


/* DISASM */
char * regnames_b[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
char * regnames_w[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
char * regnames_d[] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
char * regnames_s[] = { "es", "cs", "ss", "ds", "fs", "gs" };


void CPU_Regs::Init (I386CPU * cpu)
{
	pCpu = cpu;
}


CPU_Regs::CPU_Regs ()
{
	int i, j, c;

	/* 
	 * Init CPU registers 
	 */
    eip = 0;
    eflags = 0x00000002;

    /* according to Intel manual */
    for ( i = 0; i < MAX_SEG_REGS; i++ )
    	sreg[i] = 0;

    for ( i = 0; i < MAX_REGS; i++ )
    	reg[i] = 0;

	/* 
	 * Init CPU register pointers.
	 */
    for ( i = 0; i < MAX_REGS; i++ )	
    {
    	regd[i] = &reg[i];

		// Registers are in Little-Endian byte order
    	regw[i] = (WORD *) &reg[i];

        /* only EAX, ECX, EDX, EBX have byte-size regs */
    	if ( i < 4 )
    	{
			regb[i] = (BYTE *) regw[i];			// low byte
			regb[i+4] = ((BYTE *) regw[i] + 1);	// high byte
		}
	}

	/* 
	 * Fill MOD/RM table
	 */
	for ( i = 0; i < 256; i++ )
	{
		BYTE mod = (i & 0xC0) >> 6;
		BYTE rrr = (i & 0x38) >> 3;
		BYTE rm = i & 0x07;

		m_ModRmTable16[i].regrrr = rrr;
		m_ModRmTable32[i].regrrr = rrr;

		/* initial values */
		m_ModRmTable16[i].type = MOD_REG;
		m_ModRmTable16[i].reg1 = NO_REG;
		m_ModRmTable16[i].reg2 = NO_REG;
		m_ModRmTable16[i].segreg = NO_REG;

		m_ModRmTable32[i].type = MOD_REG;
		m_ModRmTable32[i].reg1 = NO_REG;
		m_ModRmTable32[i].reg2 = NO_REG;
		m_ModRmTable32[i].segreg = NO_REG;


		switch (rm)
		{
		case 0:
			m_ModRmTable16[i].reg1 = BX;
			m_ModRmTable16[i].reg2 = SI;
            m_ModRmTable16[i].segreg = DS;

            m_ModRmTable32[i].reg1 = EAX;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 1:
			m_ModRmTable16[i].reg1 = BX;
			m_ModRmTable16[i].reg2 = DI;
            m_ModRmTable16[i].segreg = DS;

            m_ModRmTable32[i].reg1 = ECX;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 2:
			m_ModRmTable16[i].reg1 = BP;
			m_ModRmTable16[i].reg2 = SI;
            m_ModRmTable16[i].segreg = SS;

            m_ModRmTable32[i].reg1 = EDX;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 3:
			m_ModRmTable16[i].reg1 = BP;
			m_ModRmTable16[i].reg2 = DI;
            m_ModRmTable16[i].segreg = SS;

            m_ModRmTable32[i].reg1 = EBX;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 4:
			m_ModRmTable16[i].reg1 = SI;
			m_ModRmTable16[i].reg2 = NO_REG;
            m_ModRmTable16[i].segreg = DS;

            m_ModRmTable32[i].reg1 = NO_REG;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 5:
			m_ModRmTable16[i].reg1 = DI;
			m_ModRmTable16[i].reg2 = NO_REG;
            m_ModRmTable16[i].segreg = DS;

            m_ModRmTable32[i].reg1 = EBP;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 6:
			m_ModRmTable16[i].reg1 = BP;
			m_ModRmTable16[i].reg2 = NO_REG;
            m_ModRmTable16[i].segreg = DS;

            m_ModRmTable32[i].reg1 = ESI;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;

		case 7:
			m_ModRmTable16[i].reg1 = BX;
			m_ModRmTable16[i].reg2 = NO_REG;
            m_ModRmTable16[i].segreg = DS;

            m_ModRmTable32[i].reg1 = EDI;
            m_ModRmTable32[i].reg2 = NO_REG;
            m_ModRmTable32[i].segreg = DS;
            break;
		}

		switch (mod)
		{
		case 0:
			m_ModRmTable16[i].type = MOD_PTR;
            m_ModRmTable32[i].type = MOD_PTR;

			/* 32bit special case: SIB present */
			if ( rm == 4 )
				m_ModRmTable32[i].type = MOD_SIB;

			/* 32bit special case: disp32 */
			if ( rm == 5 )
			{
				m_ModRmTable32[i].reg1 = NO_REG;
				m_ModRmTable32[i].type = MOD_DISPS;
			}

			/* 16bit special case: disp16 */
			if ( rm == 6 )
			{
				m_ModRmTable16[i].reg1 = NO_REG;
				m_ModRmTable16[i].type = MOD_DISPS;
			}
		 
            break;

		case 1:
			m_ModRmTable16[i].type = MOD_DISP8;
            m_ModRmTable32[i].type = MOD_DISP8;

			/* 32bit special case: SIB present */
			if ( rm == 4 )
				m_ModRmTable32[i].type = MOD_DISP8SIB;
            break;

		case 2:
			m_ModRmTable16[i].type = MOD_DISPS;
            m_ModRmTable32[i].type = MOD_DISPS;

			/* 32bit special case: SIB present */
			if ( rm == 4 )
				m_ModRmTable32[i].type = MOD_DISP32SIB;
            break;


		case 3:
			/* Override register number */
			m_ModRmTable16[i].reg1 = rm;
            m_ModRmTable32[i].reg1 = rm;
			m_ModRmTable16[i].type = MOD_REG;
            m_ModRmTable32[i].type = MOD_REG;
            break;
		}

        /* Fill parity table */
        for ( j = i, c = 0; j > 0; j >>= 1)
            if (j & 0x01) 
            	c++;
      
        m_bParityTable[i] = !(c & 0x01);
	}

    pCpu = 0;
#if defined (ENABLE_DISASM)
    fDisasmToBuf = false;
#endif
}



/*
 * Return pointer to specified byte (r8) register or memory (r/m8)
 */
BYTE * CPU_Regs::GetModRMRMB (BYTE mod_rm)
{
	BYTE * ptr;
    mod_rm_t * table = (GetAddressSize() == 16 ? m_ModRmTable16 : m_ModRmTable32);

	if ( table[mod_rm].type == MOD_REG )
	{
		int reg = table[mod_rm].reg1;
		register BYTE * regptr = regb[reg];

		DISASM_SET_PARAM ((DISASM_GETMODRMRMX, regnames_b[reg]));
        return regptr;
	}

    ptr = (BYTE *) GetModRmAddr (mod_rm);
	return ptr;
}


/*
 * Return pointer to specified word (r16) register or memory (r/m16)
 */
WORD * CPU_Regs::GetModRMRMW (BYTE mod_rm)
{
	WORD * ptr;
    mod_rm_t * table = (GetAddressSize() == 16 ? m_ModRmTable16 : m_ModRmTable32);
	 
	if ( table[mod_rm].type == MOD_REG )
	{
		int reg = table[mod_rm].reg1;
		register WORD * regptr = regw[reg];

		DISASM_SET_PARAM ((DISASM_GETMODRMRMX, regnames_w[reg]));
        return regptr;
	}
    
    ptr = (WORD *) GetModRmAddr (mod_rm);
	return ptr;
}


/*
 * Return pointer to specified dword (r32) register or memory (r/m32)
 */
DWORD * CPU_Regs::GetModRMRMD (BYTE mod_rm)
{
	DWORD * ptr;
    mod_rm_t * table = (GetAddressSize() == 16 ? m_ModRmTable16 : m_ModRmTable32);

	if ( table[mod_rm].type == MOD_REG )
	{
		int reg = table[mod_rm].reg1;
		register DWORD * regptr = regd[reg];

		DISASM_SET_PARAM ((DISASM_GETMODRMRMX, regnames_d[reg]));
        return regptr;
	}
    
    ptr = (DWORD *) GetModRmAddr (mod_rm);
	return ptr;
}


void * CPU_Regs::GetModRmAddr(BYTE mod_rm)
{
    SDWORD offset = 0;
    SDWORD size;

#if defined (ENABLE_DISASM)
	fDisasmToBuf = true;
    disasm_buf[0] = '\0';
    pDisasm.EnableReadData(false);
#endif

	if ( GetAddressSize() == 16 )
	{
		switch (m_ModRmTable16[mod_rm].type)
		{
		case MOD_PTR:
			offset = READ_X86_SIGNED_DATA (GetReg16(m_ModRmTable16[mod_rm].reg1));

			if ( m_ModRmTable16[mod_rm].reg2 != NO_REG )
			{
#if defined (ENABLE_DISASM)
				strcat (disasm_buf, "+");
#endif
				offset += READ_X86_SIGNED_DATA (GetReg16(m_ModRmTable16[mod_rm].reg2));
			}

			break;

		case MOD_DISP8:
            size = (SDWORD) (SBYTE) pCpu->ReadIx86Byte();

			offset = READ_X86_SIGNED_DATA (GetReg16(m_ModRmTable16[mod_rm].reg1)) + size;

			if ( m_ModRmTable16[mod_rm].reg2 != NO_REG )
			{
#if defined (ENABLE_DISASM)
				strcat (disasm_buf, "+");
#endif
				offset += READ_X86_SIGNED_DATA (GetReg16(m_ModRmTable16[mod_rm].reg2));
			}
#if defined (ENABLE_DISASM)
            strcat (disasm_buf, DumpSize (size, 8));
#endif
			break;

		case MOD_DISPS:
			if ( m_ModRmTable16[mod_rm].reg1 != NO_REG )
			{
				offset = READ_X86_SIGNED_DATA (GetReg16(m_ModRmTable16[mod_rm].reg1));

				if ( m_ModRmTable16[mod_rm].reg2 != NO_REG )
				{
#if defined (ENABLE_DISASM)
					strcat (disasm_buf, "+");
#endif
					offset += READ_X86_SIGNED_DATA (GetReg16(m_ModRmTable16[mod_rm].reg2));
				}
			}

			size = (SDWORD) pCpu->ReadIx86Size();
			offset += size;
#if defined (ENABLE_DISASM)
            strcat (disasm_buf, DumpSize (size));
#endif
			break;

		default:
			break;
		}
	}
	else
	{
		switch (m_ModRmTable32[mod_rm].type)
		{
		case MOD_PTR:
			offset = READ_X86_SIGNED_DATA (GetReg32(m_ModRmTable32[mod_rm].reg1));
			break;

		case MOD_DISP8:
            size = (SDWORD) (SBYTE) pCpu->ReadIx86Byte();
			offset = READ_X86_SIGNED_DATA (GetReg32(m_ModRmTable32[mod_rm].reg1)) + size;
#if defined (ENABLE_DISASM)
            strcat (disasm_buf, DumpSize (size, 8));
#endif
			break;

		case MOD_DISPS:
			if ( m_ModRmTable32[mod_rm].reg1 != NO_REG )
				offset = READ_X86_SIGNED_DATA (GetReg32(m_ModRmTable32[mod_rm].reg1));

			size = (SDWORD) pCpu->ReadIx86Size();
			offset += size;
#if defined (ENABLE_DISASM)
            strcat (disasm_buf, DumpSize (size));
#endif
			break;

		case MOD_SIB:
			offset = (SDWORD) ParseSIB(mod_rm, pCpu->ReadIx86Byte());
	   		break;
		
		case MOD_DISP8SIB:
			offset = (SDWORD) ParseSIB(mod_rm, pCpu->ReadIx86Byte());
			size = (SDWORD) (SBYTE) pCpu->ReadIx86Byte();
			offset += size;
#if defined (ENABLE_DISASM)
            strcat (disasm_buf, DumpSize (size, 8));
#endif
	   		break;

		case MOD_DISP32SIB:
			offset = (SDWORD) ParseSIB(mod_rm, pCpu->ReadIx86Byte());
			size = (SDWORD) pCpu->ReadIx86Size();
			offset += size;
#if defined (ENABLE_DISASM)
            strcat (disasm_buf, DumpSize (size));
#endif
	   		break;

		default:
			break;
		}
	}

#if defined (ENABLE_DISASM)
    fDisasmToBuf = false;
	DISASM_SET_PARAM ((DISASM_GETMODRMRMX, "[%s]", disasm_buf));
    pDisasm.EnableReadData(true);
#endif
	return (void *) offset;
}


/*
 * Returns calculated reg value.
 * For example, if SIB is "[ECX*4+EBP] returns (ECX*4+EBP).
 */

SDWORD CPU_Regs::ParseSIB (BYTE mod_rm, BYTE sib)
{
#if defined (ENABLE_DISASM)
	char buf[32];
#endif
	BYTE bit_mod = (mod_rm & 0xC0) >> 6;
	BYTE bit_scale = (sib & 0xC0) >> 6;
	BYTE bit_index = (sib & 0x38) >> 3;
	BYTE bit_base = sib & 0x7;
    SDWORD base_data = 0;
    SDWORD index_data = 0;

	/* Do "none" scale section */
	if ( bit_index != 4 )
	{
		index_data = READ_X86_SIGNED_DATA (GetReg32(bit_index));

		if ( bit_scale )
		{
			index_data <<= bit_scale;
#if defined (ENABLE_DISASM)
			sprintf (buf, "*%d", 1 << bit_scale);
			strcat (disasm_buf, buf);
#endif
		}

#if defined (ENABLE_DISASM)
        strcat (disasm_buf, "+");
#endif

	}

	/* This is "special case" */
	if ( bit_base == 5 )
	{
		switch (bit_mod)
		{
		case 0:		// disp32[index]
			base_data = (SDWORD) pCpu->ReadIx86DWord();
#if defined (ENABLE_DISASM)
			sprintf (buf, "0x%08X", base_data);
			strcat (disasm_buf, buf);
#endif
            break;

		case 1:		// disp8[EBP][index]
			base_data = READ_X86_SIGNED_DATA (GetReg32(EBP));
			break;

		default:	// disp32[EBP][index]
			base_data = READ_X86_SIGNED_DATA (GetReg32(EBP));
			break;
		}
	}
	else
		base_data = READ_X86_SIGNED_DATA (GetReg32(bit_base));

	return index_data + base_data;
}



void CPU_Regs::DumpRegs ()
{
	pDisasm.Log ("Register dump:\n");
	pDisasm.Log ("EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X\n", 
			ReadDWordPtr(regd[EAX]), 
			ReadDWordPtr(regd[EBX]),
			ReadDWordPtr(regd[ECX]),
			ReadDWordPtr(regd[EDX]) );

	pDisasm.Log ("ESP: %08X  EBP: %08X  ESI: %08X  EDI: %08X, EFLAGS: %08X\n", 
			ReadDWordPtr(regd[ESP]),
			ReadDWordPtr(regd[EBP]),
			ReadDWordPtr(regd[ESI]),
			ReadDWordPtr(regd[EDI]),
			eflags );

	pDisasm.Log ("CS: %04X  DS: %04X  ES: %04X  SS: %04X EIP: %08X Flags: %s\n", 
			ReadWordPtr(&sreg[CS]),
			ReadWordPtr(&sreg[DS]),
			ReadWordPtr(&sreg[ES]),
			ReadWordPtr(&sreg[SS]),
			eip, 
			DumpFlags() );
}


const char * CPU_Regs::DumpFlags ()
{
	static char buf[18];

	strcpy (buf, ".................");

	if ( IS_SET(eflags, OF) )
		buf[16-11] = 'O';
	
	if ( IS_SET(eflags, DF) )
		buf[16-10] = 'D';

	if ( IS_SET(eflags, IF) )
		buf[16-9] = 'I';

	if ( IS_SET(eflags, TF) )
		buf[16-8] = 'T';

	if ( IS_SET(eflags, SF) )
		buf[16-7] = 'S';

	if ( IS_SET(eflags, ZF) )
		buf[16-6] = 'Z';

	if ( IS_SET(eflags, AF) )
		buf[16-4] = 'A';

	if ( IS_SET(eflags, PF) )
		buf[16-2] = 'P';

	if ( IS_SET(eflags, CF) )
		buf[16-0] = 'C';

	return buf;
}


bool CPU_Regs::CheckCondition (condition_t cond)
{
	int result = false;

	switch (cond)
	{
	case COND_ALWAYS:
        result = true;
		break;

	case COND_A_NBE:
        result = !( IS_SET(eflags, CF|ZF) );
		break;
		
	case COND_AE_NB:
        result = !IS_SET(eflags, CF);
		break;

	case COND_B_NAE:
        result = IS_SET(eflags, CF);
		break;

	case COND_BE_NA:
        result = IS_SET(eflags, CF|ZF);
		break;

	case COND_C:
        result = IS_SET(eflags, CF);
		break;

	case COND_E_Z:
        result = IS_SET(eflags, ZF);
		break;

	case COND_G_NLE:
        result = !IS_SET(eflags, ZF) && (IS_SET(eflags, SF|OF) == 0 || IS_SET(eflags, SF|OF) == (SF|OF));
		break;

	case COND_GE_NL:
        result = (IS_SET(eflags, SF|OF) == 0 || IS_SET(eflags, SF|OF) == (SF|OF));
		break;

	case COND_L_NGE:
        result = (IS_SET(eflags, SF|OF) != 0 && IS_SET(eflags, SF|OF) != (SF|OF));
		break;

	case COND_LE_NG:
        result = IS_SET(eflags, ZF) || (IS_SET(eflags, SF|OF) != 0 && IS_SET(eflags, SF|OF) != (SF|OF));
		break;

	case COND_NC:
        result = !IS_SET(eflags, CF);
		break;

	case COND_NE_NZ:
        result = !IS_SET(eflags, ZF);
		break;

	case COND_NO:
        result = !IS_SET(eflags, OF);
		break;

	case COND_NP_PO:
        result = !IS_SET(eflags, PF);
		break;

	case COND_NS:
        result = !IS_SET(eflags, SF);
		break;

	case COND_O:
        result = IS_SET(eflags, OF);
		break;

	case COND_P_PE:
        result = IS_SET(eflags, PF);
		break;

	case COND_S:
        result = IS_SET(eflags, SF);
		break;
	}

	return result ? true : false;
}


const char * CPU_Regs::DumpSize (SDWORD data, int size)
{
	static char buf[24];
	char templ[10];

	if ( data < 0 )
	{
		strcpy(templ, "-");
        data = 0 - data;
	}
	else if ( data > 0 )
		strcpy(templ, "+");
	else
		templ[0] = '\0';

	if ( size != 8 && size != 16 && size != 32 )
		size = GetAddressSize();

	sprintf (templ+strlen(templ), "0x%%0%dX", size/4);
	sprintf (buf, templ, data);
	return buf;
}
