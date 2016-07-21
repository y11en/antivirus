/*
 *  i386disasm.h,v 1.2 2002/10/17 09:55:02 tim 
 */

#ifndef I386DISASM_H
#define I386DISASM_H

#include "typedefs.h"


const DWORD MAX_DISASM_STRING = 512;


#if defined (ENABLE_DISASM)
	#define DISASSEMBLE(A)			pDisasm.Disasm A
	#define DISASM_SET_PARAM(A)		pDisasm.SetParam A
#else	
	#define DISASSEMBLE(A)
	#define DISASM_SET_PARAM(A)
#endif


/*
 * Condition flags for CheckCondition()
 */
enum condition_t
{
	COND_ALWAYS,
	COND_A_NBE,
	COND_AE_NB,
	COND_B_NAE,
	COND_BE_NA,
	COND_C,
	COND_E_Z,
	COND_G_NLE,
	COND_GE_NL,
	COND_L_NGE,
	COND_LE_NG,
	COND_NC,
	COND_NE_NZ,
	COND_NO,
	COND_NP_PO,
   	COND_NS,
	COND_O,
	COND_P_PE,
	COND_S
};


enum disasm_t
{
	DISASM_GETREG8,
	DISASM_GETREG16,
	DISASM_GETREG32,
	DISASM_READIX86BYTE,
	DISASM_READIX86WORD,
	DISASM_READIX86DWORD,
	DISASM_GETSEGREG,
	DISASM_GETMODRMRMX,
	DISASM_GETMODRMREGX
};


class I386Disasm
{
	public:
		I386Disasm();

		/*
		 * Disasm() args:
		 * %imm8 %imm16 %imm32
		 * %cond - condition_t
		 * %rep - REP or nothing, depend on flags
		 * %rpe - REPE or REPNE or nothing, depend on flags
		 * 
		 * %r8,16,32 - GetRegxx
		 * %reg - GetRegxx
		 * %lb, %lw, %ld - ReadIx86Byte/word/dword, %lr - last read issue (any of), -lr - with sign
		 * %sreg - GetSegReg
		 * %mrm - GetModRMRMx
		 * %mreg - GetModRMRegx
		 * %imms - value read by ReadIx86Size, depends on GetAddressSize
		 */
		void	Disasm (const char * str, ...);
		void	SetParam (disasm_t param, const char * str, ...);
		void	SetParam (disasm_t param, DWORD value);
		void	EnableReadData (bool fEnable);
		const char * GetDisasm();

		void	Log (const char * str, ...);
		void	LogAlways (const char * str, ...);
		void	EnableDisasm (bool fEnable);

	private:
    	enum lastread_t
		{
			LAST_IS_BYTE,
			LAST_IS_WORD,
			LAST_IS_DWORD
		};

		void			Cleanup();
        const char * 	GetCondName (condition_t cond);
		void			sprintf_hex (char * buf, bool fSigned, lastread_t type, DWORD value);

		bool	fEnableReadData;
		bool	fEnableDisassembler;
		char 	m_sGetReg8[MAX_DISASM_STRING];
		char 	m_sGetReg16[MAX_DISASM_STRING];
		char 	m_sGetReg32[MAX_DISASM_STRING];
		char 	m_sGetRegLast[MAX_DISASM_STRING];
		char 	m_sGetSegReg[MAX_DISASM_STRING];
		char 	m_sGetModRmRmX[MAX_DISASM_STRING];
		char 	m_sGetModRmRegX[MAX_DISASM_STRING];
		char 	m_sDisasm[MAX_DISASM_STRING];
		BYTE	m_bReadByte;
		WORD	m_wReadWord;
		DWORD	m_dwReadDWord;
		DWORD	m_dwReadLast;
        lastread_t m_lastSym;
};


extern I386Disasm pDisasm;


#endif
