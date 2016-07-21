/*
 *  i386regs.h,v 1.2 2002/10/17 09:55:03 tim 
 */

#ifndef I386REGS_H
#define I386REGS_H

#include "typedefs.h"


/*
 * Command prefix bitmask
 */
const DWORD PREFIX_ES 	= 0x0001;
const DWORD PREFIX_CS 	= 0x0002;
const DWORD PREFIX_DS 	= 0x0004;
const DWORD PREFIX_SS 	= 0x0008;
const DWORD PREFIX_FS 	= 0x0010;
const DWORD PREFIX_GS 	= 0x0020;
const DWORD PREFIX_OSIZE= 0x0040;
const DWORD PREFIX_ASIZE= 0x0080;
const DWORD PREFIX_LOCK	= 0x0100;
const DWORD PREFIX_REPE	= 0x0200;
const DWORD PREFIX_REPNE= 0x0400;

extern char * regnames_b[];
extern char * regnames_w[];
extern char * regnames_d[];
extern char * regnames_s[];

const int MAX_REGS = 8;
const int MAX_SEG_REGS = 6;


// byte-size registers
const int AL = 0;
const int CL = 1;
const int DL = 2;
const int BL = 3;
const int AH = 4;
const int CH = 5;
const int DH = 6;
const int BH = 7;

// word-size registers
const int AX = 0;
const int CX = 1;
const int DX = 2;
const int BX = 3;
const int SP = 4;
const int BP = 5;
const int SI = 6;
const int DI = 7;

// dword-size registers
const int EAX = 0;
const int ECX = 1;
const int EDX = 2;
const int EBX = 3;
const int ESP = 4;
const int EBP = 5;
const int ESI = 6;
const int EDI = 7;

// segment registers
const int ES = 0;
const int CS = 1;
const int SS = 2;
const int DS = 3;
const int FS = 4;
const int GS = 5;


/* CPU flags */
const DWORD	 OF = 1 << 11;
const DWORD	 DF = 1 << 10;
const DWORD	 IF = 1 << 9;
const DWORD	 TF = 1 << 8;
const DWORD	 SF = 1 << 7;
const DWORD	 ZF = 1 << 6;
const DWORD	 AF = 1 << 4;
const DWORD	 PF = 1 << 2;
const DWORD	 CF = 1 << 0;

const int NO_REG = -1;

/*
 * Command flag (for ApplyFlags)
 */
enum cmd_flag_t
{
	CMD_NONE,
	CMD_ADD,
	CMD_ADC,
	CMD_SUB,
	CMD_SBB,
	CMD_BITOPS,
	CMD_NEG,
    CMD_SHIFTOPS
};


/*
 * MOD flags
 */
enum mod_rmflags_t
{
	// Use pointer to register (like AX/EAX), not to register memory.
	MOD_REG,

	// Use pointer to register value (like [AX] or [BX+SI]).
	// reg1 specifies one 16/32bit register, reg2 - -1 or second 16bit register.
	MOD_PTR,

	// Add byte offset like [AX+18h] by reading next byte.
	// reg1 points to one 16/32bit register, reg2 - NULL or second 16bit register.
	// It is impossible here to have NULL in both registers.
	MOD_DISP8,

	// Add offset (16/32) like [AX+9283h] by reading next word/dword.
	// reg1 points to one 16/32bit register, reg2 - NULL or second 16bit register.
	// It is possible here to have NULL in both registers, so we use only offset.
	MOD_DISPS,

	// Next byte is SIB
    MOD_SIB,

	// Like MOD_DISP8, but includes SIB
    MOD_DISP8SIB,

	// Like MOD_DISPSIZE, but includes SIB
    MOD_DISP32SIB,
};


typedef struct
{
	int reg1;
	int reg2;
	int segreg;		// segment register
	int regrrr;
	mod_rmflags_t	type;

} mod_rm_t;



class I386CPU;
class CPU_Memory;


class CPU_Regs
{
	public:
		CPU_Regs();
		void			Init (I386CPU * pCpu);

		void			DumpRegs();
		const char	*	DumpFlags();
		const char	*	DumpSize (SDWORD data, int size = 0);

		inline BYTE * GetModRMRegB (BYTE mod_rm)
		{
		    mod_rm_t * table = (GetAddressSize() == 16 ? m_ModRmTable16 : m_ModRmTable32);
			register BYTE * reg = regb[table[mod_rm].regrrr];

			DISASM_SET_PARAM ((DISASM_GETMODRMREGX, regnames_b[table[mod_rm].regrrr]));
			return reg;
		}

		inline WORD * GetModRMRegW (BYTE mod_rm)
		{
		    mod_rm_t * table = (GetAddressSize() == 16 ? m_ModRmTable16 : m_ModRmTable32);
			register WORD * reg = regw[table[mod_rm].regrrr];

			DISASM_SET_PARAM ((DISASM_GETMODRMREGX, regnames_w[table[mod_rm].regrrr]));
			return reg;
		}

		inline DWORD * GetModRMRegD (BYTE mod_rm)
		{
		    mod_rm_t * table = (GetAddressSize() == 16 ? m_ModRmTable16 : m_ModRmTable32);
			register DWORD * reg = regd[table[mod_rm].regrrr];

			DISASM_SET_PARAM ((DISASM_GETMODRMREGX, regnames_d[table[mod_rm].regrrr]));
			return reg;
		}

		inline BYTE * GetReg8 (BYTE cmd)
		{
			register unsigned regname = cmd & 0x07;
			register BYTE * reg = regb[regname];
#if defined (ENABLE_DISASM)
			if ( fDisasmToBuf )
				strcat (disasm_buf, regnames_b[regname]);
			else
				DISASM_SET_PARAM ((DISASM_GETREG8, regnames_b[regname]));
#endif
			return reg;
		}

		inline WORD * GetReg16 (BYTE cmd)
		{
			register unsigned regname = cmd & 0x07;
			register WORD * reg = regw[regname];
#if defined (ENABLE_DISASM)
			if ( fDisasmToBuf )
				strcat (disasm_buf, regnames_w[regname]);
			else
				DISASM_SET_PARAM ((DISASM_GETREG16, regnames_w[regname]));
#endif
			return reg;
		}

		inline DWORD * GetReg32 (BYTE cmd)
		{
			register unsigned regname = cmd & 0x07;
			register DWORD * reg = regd[regname];
#if defined (ENABLE_DISASM)
			if ( fDisasmToBuf )
				strcat (disasm_buf, regnames_d[regname]);
			else
				DISASM_SET_PARAM ((DISASM_GETREG32, regnames_d[regname]));
#endif
            return reg;
		}

		inline WORD * GetSegReg (BYTE cmd)
		{
			register unsigned regname = cmd & 0x07;
			register WORD * reg = &sreg[regname];
#if defined (ENABLE_DISASM)
			if ( fDisasmToBuf )
				strcat (disasm_buf, regnames_s[regname]);
			else
				DISASM_SET_PARAM ((DISASM_GETSEGREG, regnames_s[regname]));
#endif
			return reg;
		}

		inline int GetOperandSize()
		{
			return IS_SET (m_dwOpcodeFlags, PREFIX_OSIZE) ? 16 : 32;
		}

		inline int GetAddressSize ()
		{
			return IS_SET (m_dwOpcodeFlags, PREFIX_ASIZE) ? 16 : 32;
		}

		inline void * GetStackTop()
		{
			return (void *) LittleEndianToMachine (reg[ESP]);
		}

		inline void SetStackTop(void * stacktop)
		{
			reg[ESP] = MachineToLittleEndian ((DWORD) stacktop);
		}

		inline int IsFlag (DWORD flag)
		{
			return (IS_SET(eflags, flag) ? 1 : 0);
		}

		BYTE 		*	GetModRMRMB (BYTE mod_rm);
		WORD		*	GetModRMRMW (BYTE mod_rm);
		DWORD		*	GetModRMRMD (BYTE mod_rm);

		bool			CheckCondition (condition_t cond);

		template <class T> void ApplyFlags (DWORD chkflags, T newvalue, T oldvalue = 0, T argument = 0, cmd_flag_t cmd = CMD_NONE)
		{
			const T SIGNBIT = (1 << ((sizeof(T) * 8 - 1)));

			/* zero flag */
			if ( IS_SET(chkflags, ZF) )
				SET_BIT(eflags, ZF, newvalue == 0);

			/* Auxiliary flag */
			if ( IS_SET(chkflags, AF) )
				SET_BIT (eflags, AF, (((BYTE)newvalue ^ (BYTE)oldvalue ^ (BYTE)argument) & 0x10));

			/* Parity flag */
			if ( IS_SET(chkflags, PF) )
				SET_BIT(eflags, PF, m_bParityTable[(BYTE)newvalue]);

			/* Sign flag */
			if ( IS_SET(chkflags, SF) )
				SET_BIT(eflags, SF, (newvalue & SIGNBIT) ? true : false);

			/* Command-specific flags */
			switch (cmd)
			{
			case CMD_ADD:
		        /* Carry flag */
				if ( IS_SET(chkflags, CF) )
					SET_BIT(eflags, CF, newvalue < oldvalue);

				/* Overflow flag */
				if ( IS_SET(chkflags, OF) )
					SET_BIT(eflags, OF, ((newvalue ^ argument) & (newvalue ^ oldvalue) & SIGNBIT) ? true : false );

				break;

			case CMD_ADC:	
		        /* Carry flag */
				if ( IS_SET(chkflags, CF) )
					SET_BIT(eflags, CF, IS_SET(eflags, CF) && oldvalue != 0 ? 
						(newvalue <= oldvalue) : (newvalue < oldvalue) );

				/* Overflow flag */
				if ( IS_SET(chkflags, OF) )
					SET_BIT(eflags, OF, ((newvalue ^ argument) & (newvalue ^ oldvalue) & SIGNBIT) ? true : false );
		
				break;

			case CMD_SUB:
		        /* Carry flag */
				if ( IS_SET(chkflags, CF) )
					SET_BIT(eflags, CF, newvalue > oldvalue);

				/* Overflow flag */
				if ( IS_SET(chkflags, OF) )
					SET_BIT(eflags, OF, ((oldvalue ^ argument) & (oldvalue ^ newvalue) & SIGNBIT) ? true : false ); 
				break;

			case CMD_SBB:
		        /* Carry flag */
				if ( IS_SET(chkflags, CF) )
					SET_BIT(eflags, CF, IS_SET(eflags, CF) && oldvalue != 0 ? 
						(newvalue >= oldvalue) : (newvalue > oldvalue) );
		
				/* Overflow flag */
				if ( IS_SET(chkflags, OF) )
					SET_BIT(eflags, OF, ((oldvalue ^ argument) & (oldvalue ^ newvalue) & SIGNBIT) ? true : false ); 

				break;

			case CMD_NEG:		
		        /* Carry flag */
				if ( IS_SET(chkflags, CF) )
					SET_BIT(eflags, CF, newvalue ? true : false);

				break;

			case CMD_BITOPS:	
		        /* Carry flag */
				if ( IS_SET(chkflags, CF) )
					SET_BIT(eflags, CF, 0);

				/* Overflow flag */
				if ( IS_SET(chkflags, OF) )
					SET_BIT(eflags, OF, 0);

				SET_BIT (eflags, AF, 0);
				break;

			case CMD_SHIFTOPS:
				SET_BIT (eflags, AF, 1);
				break;

			default:	// to satisfy gcc
				break;
            }
		}

		void 	* 	GetModRmAddr(BYTE mod_rm);	// need to be public for LEA

		DWORD		eip;
		DWORD		eflags;

		BYTE		m_bFoo;			// buffer for something
		WORD		m_wFoo;			// buffer for something
		DWORD		m_dwFoo;		// buffer for something
		DWORD		m_dwOpcodeFlags;

		BYTE 	*	regb[MAX_REGS];		// AL, BL, CL, DL, AH, BH, CH, DH
			
	private:
        SDWORD		ParseSIB (BYTE mod_rm, BYTE sib);
        	
		WORD 	*	regw[MAX_REGS];		// AX, CX, DX, BX, SP, BP, SI, DI
		DWORD 	*	regd[MAX_REGS];		// EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

		DWORD 		reg[MAX_REGS];		// EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
		WORD 		sreg[MAX_SEG_REGS];	// ES, CS, SS, DS, FS, GS

		mod_rm_t	m_ModRmTable16[256];
        mod_rm_t	m_ModRmTable32[256];
        bool		m_bParityTable[256];

        // Disassembler stuff
#if defined (ENABLE_DISASM)        
        char		disasm_buf[512];
        bool		fDisasmToBuf;
#endif

		I386CPU	  *	pCpu;
		CPU_Memory* p_Memory;
};

#endif
