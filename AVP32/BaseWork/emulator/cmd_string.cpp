/*
 *  cmd_string.cpp,v 1.2 2002/10/17 09:55:00 tim 
 */

#include "typedefs.h"


/* repe/repne */
void I386CPU::Cmd_scass (BYTE cmd, int size, DWORD& dwRepCount, bool fRep)
{
	DWORD *edi = m_Regs.GetReg32(EDI);
	DWORD edivalue = READ_X86_DWORD(edi);
	DWORD condition = 0;
	bool fDecrement = IS_SET(m_Regs.eflags, DF) ? true : false;

    if ( IS_SET(m_Regs.m_dwOpcodeFlags, PREFIX_REPE) )
    	condition = ZF;
	else
		condition = 0;

	// if-else blocks allow to declare variables (and switch-case doesn't)
	if ( size == 8 )
	{
		DISASSEMBLE (("%rpe scasb", m_Regs.m_dwOpcodeFlags));
       	BYTE * reg = m_Regs.GetReg8(AL);

       	while ( dwRepCount != 0 )
       	{
			CPU_CMP (reg, (BYTE*) edivalue);

			if ( fDecrement )
       			edivalue--;
       		else
       			edivalue++;

			dwRepCount--;

			if ( fRep && (m_Regs.eflags & ZF) != condition )
        		break;
		}
	}
	else if ( size == 16 )
	{
		DISASSEMBLE (("%rpe scasw", m_Regs.m_dwOpcodeFlags));
       	WORD * reg = m_Regs.GetReg16(AX);

       	while ( dwRepCount != 0 )
       	{
			CPU_CMP (reg, (WORD *) edivalue);

			if ( fDecrement )
       			edivalue -= 2;
       		else
       			edivalue += 2;

			dwRepCount--;

			if ( fRep && (m_Regs.eflags & ZF) != condition )
        		break;
		}
	}
	else
	{
		DISASSEMBLE (("%rpe scasd", m_Regs.m_dwOpcodeFlags));
       	DWORD * reg = m_Regs.GetReg32(EAX);

       	while ( dwRepCount != 0 )
       	{
			CPU_CMP (reg, (DWORD *) edivalue);

			if ( fDecrement )
       			edivalue -= 4;
       		else
       			edivalue += 4;

			dwRepCount--;

			if ( fRep && (m_Regs.eflags & ZF) != condition )
        		break;
		}
	}

	WRITE_X86_DWORD (edi, edivalue);
}



void I386CPU::Cmd_cmpss (BYTE cmd, int size, DWORD& dwRepCount, bool fRep)
{
	DWORD *edi = m_Regs.GetReg32(EDI);
	DWORD *esi = m_Regs.GetReg32(ESI);
	DWORD ediptr = READ_X86_DWORD(edi);
	DWORD esiptr = READ_X86_DWORD(esi);
	DWORD condition = 0;
	bool fDecrement = IS_SET(m_Regs.eflags, DF) ? true : false;

    if ( IS_SET(m_Regs.m_dwOpcodeFlags, PREFIX_REPE) )
    	condition = ZF;
	else
		condition = 0;

	switch (size)
	{
	case 8:
		DISASSEMBLE (("%rpe cmpsb", m_Regs.m_dwOpcodeFlags));

       	while ( dwRepCount != 0 )
       	{
			CPU_CMP ((BYTE *) ediptr, (BYTE *) esiptr);

			if ( fDecrement )
			{
				ediptr--;
                esiptr--;
			}
       		else
       		{
				ediptr++;
                esiptr++;
			}

			dwRepCount--;

			if ( fRep && (m_Regs.eflags & ZF) != condition )
        		break;
		}
        break;

	case 16:
		DISASSEMBLE (("%rpe cmpsw", m_Regs.m_dwOpcodeFlags));
       	while ( dwRepCount != 0 )
       	{
			CPU_CMP ((WORD *) ediptr, (WORD *) esiptr);

			if ( fDecrement )
			{
				ediptr -= 2;
                esiptr -= 2;
			}
       		else
       		{
				ediptr += 2;
                esiptr += 2;
			}

			dwRepCount--;

			if ( fRep && (m_Regs.eflags & ZF) != condition )
        		break;
		}
        break;

	default:
		DISASSEMBLE (("%rpe cmpsd", m_Regs.m_dwOpcodeFlags));
       	while ( dwRepCount != 0 )
       	{
			CPU_CMP ((DWORD *) ediptr, (DWORD *) esiptr);

			if ( fDecrement )
			{
				ediptr -= 4;
                esiptr -= 4;
			}
       		else
       		{
				ediptr += 4;
                esiptr += 4;
			}

			dwRepCount--;

			if ( fRep && (m_Regs.eflags & ZF) != condition )
        		break;
		}
        break;
	}

	WRITE_X86_DWORD (edi, ediptr);
	WRITE_X86_DWORD (esi, esiptr);
}



void I386CPU::Cmd_movss (BYTE cmd, int size, DWORD& dwRepCount)
{
	DWORD *edi = m_Regs.GetReg32(EDI);
	DWORD *esi = m_Regs.GetReg32(ESI);
	DWORD ediptr = READ_X86_DWORD(edi);
	DWORD esiptr = READ_X86_DWORD(esi);
	bool fDecrement = IS_SET(m_Regs.eflags, DF) ? true : false;

	switch (size)
	{
	case 8:
    	DISASSEMBLE (("%rep movsb", m_Regs.m_dwOpcodeFlags));
		for ( ; dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV ((BYTE *) ediptr, (BYTE *) esiptr);

			if ( fDecrement )
			{
				ediptr--;
                esiptr--;
			}
       		else
       		{
				ediptr++;
                esiptr++;
			}
		}
        break;

	case 16:
		DISASSEMBLE (("%rep movsw", m_Regs.m_dwOpcodeFlags));
		for ( ; dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV ((WORD *) ediptr, (WORD *) esiptr);

			if ( fDecrement )
			{
				ediptr -= 2;
                esiptr -= 2;
			}
       		else
       		{
				ediptr += 2;
                esiptr += 2;
			}
		}
        break;

	case 32:
		DISASSEMBLE (("%rep movsd", m_Regs.m_dwOpcodeFlags));
		for ( ; dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV ((DWORD *) ediptr, (DWORD *) esiptr);

			if ( fDecrement )
			{
				ediptr -= 4;
                esiptr -= 4;
			}
       		else
       		{
				ediptr += 4;
                esiptr += 4;
			}
		}
        break;
	}

	WRITE_X86_DWORD (edi, ediptr);
	WRITE_X86_DWORD (esi, esiptr);
}



void I386CPU::Cmd_stoss (BYTE cmd, int size, DWORD& dwRepCount)
{
	DWORD *edi = m_Regs.GetReg32(EDI);
	DWORD ptr = READ_X86_DWORD(edi);
	bool fDecrement = IS_SET(m_Regs.eflags, DF) ? true : false;

	if ( size == 8 )
	{
		DISASSEMBLE (("%rep stosb", m_Regs.m_dwOpcodeFlags));
		for ( BYTE * value = m_Regs.GetReg8(AL); dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV ((BYTE *) ptr, value);

			if ( fDecrement )
                ptr -= 1;
       		else
				ptr += 1;
		}
	}
	else if ( size == 16 )
	{
		DISASSEMBLE (("%rep stosw", m_Regs.m_dwOpcodeFlags));
		for ( WORD * value = m_Regs.GetReg16(AX); dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV ((WORD *) ptr, value);

			if ( fDecrement )
                ptr -= 2;
       		else
				ptr += 2;
		}
	}
	else
	{
		DISASSEMBLE (("%rep stosd", m_Regs.m_dwOpcodeFlags));
		for ( DWORD * value = m_Regs.GetReg32(EAX); dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV ((DWORD *) ptr, value);

			if ( fDecrement )
                ptr -= 4;
       		else
				ptr += 4;
		}
	}

	WRITE_X86_DWORD (edi, ptr);
}



void I386CPU::Cmd_lodss (BYTE cmd, int size, DWORD& dwRepCount)
{
	DWORD *edi = m_Regs.GetReg32(ESI);
	DWORD ptr = READ_X86_DWORD(edi);
	bool fDecrement = IS_SET(m_Regs.eflags, DF) ? true : false;

	if ( size == 8 )
	{
		DISASSEMBLE (("%rep lodsb", m_Regs.m_dwOpcodeFlags));
		for ( BYTE * value = m_Regs.GetReg8(AL); dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV (value, (BYTE *) ptr);

			if ( fDecrement )
                ptr -= 1;
       		else
				ptr += 1;
		}
	}
	else if ( size == 16 )
	{
		DISASSEMBLE (("%rep lodsw", m_Regs.m_dwOpcodeFlags));
		for ( WORD * value = m_Regs.GetReg16(AX); dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV (value, (WORD *) ptr);

			if ( fDecrement )
                ptr -= 2;
       		else
				ptr += 2;
		}
	}
	else
	{
		DISASSEMBLE (("%rep lodsd", m_Regs.m_dwOpcodeFlags));
		for ( DWORD * value = m_Regs.GetReg32(EAX); dwRepCount != 0; dwRepCount-- )
		{
			CPU_MOV (value, (DWORD *) ptr);

			if ( fDecrement )
                ptr -= 4;
       		else
				ptr += 4;
		}
	}

	WRITE_X86_DWORD (edi, ptr);
}



void I386CPU::Cmd_xlat (BYTE cmd)
{
    DWORD dest = READ_X86_DWORD (m_Regs.GetReg32(EBX)) + (DWORD) READ_X86_BYTE(m_Regs.GetReg8(AL));
	WRITE_X86_BYTE (m_Regs.GetReg8(AL), READ_X86_BYTE((BYTE *) dest) );
	DISASSEMBLE (("xlat"));
}
