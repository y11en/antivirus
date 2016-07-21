/*
 *  i386cpu.h,v 1.2 2002/10/17 09:55:02 tim 
 */

#ifndef I386CPU_H
#define I386CPU_H

#include "typedefs.h"

const int DUMP_SIGNED = 0x0001;
const unsigned int MAX_NESTED_CALLS = 5;

class SymbolTable;


class I386CPU
{
	public:
		I386CPU 	();
		void		Init	(SymbolTable * pTable);
		void		fatal 	(int exception, const char *fmt, ... );
		void		Execute (BYTE * codeptr);

		inline BYTE ReadIx86Byte ()
		{
			register BYTE tmp = *((BYTE *) m_Regs.eip);
		    m_Regs.eip++;
		    m_wCmdLength++;

		    DISASM_SET_PARAM ((DISASM_READIX86BYTE, (DWORD) tmp));
		    return tmp;
		}

		inline WORD ReadIx86Word ()
		{
			register WORD tmp = ::ReadWordPtr(((WORD*) m_Regs.eip));
		    m_Regs.eip += 2;
		    m_wCmdLength += 2;

		    DISASM_SET_PARAM ((DISASM_READIX86WORD, (DWORD) tmp));
		    return tmp;
		}

		inline DWORD ReadIx86DWord ()
		{
			register DWORD tmp = ::ReadDWordPtr(((DWORD*) m_Regs.eip));
		    m_Regs.eip += 4;
		    m_wCmdLength += 4;

		    DISASM_SET_PARAM ((DISASM_READIX86DWORD, tmp));
		    return tmp;
		}

		inline DWORD ReadIx86Size ()
		{
			if ( m_Regs.GetAddressSize() == 32 )
				return ReadIx86DWord();
	
			return ReadIx86Word();
		}

		// PEEK template (actually function)
		inline DWORD PEEK_STACK_VALUE (DWORD offset)
		{
			DWORD * stacktop = (DWORD *) m_Regs.GetStackTop();
		    return READ_X86_DATA (stacktop+offset+1);
		}


        /* 
         * Puts register addr in reg structure, depend on current size mode
         * and forcing byte-size registers.
         */
		DWORD			m_dwStackTop;		// ADDR of stack top, used in RETN and stack dumping
		DWORD			m_dwCmdCount;

        CPU_Memory *	m_Memory;
        CPU_Regs		m_Regs;

	private:
		WORD			m_wCmdLength;
        SymbolTable	*	pSymbolTable;
        bool			fExitEmul;
        DWORD			m_dwOldEIP;

        DWORD			m_dwNested_Eip[MAX_NESTED_CALLS];
        DWORD			m_dwNested_OldEip[MAX_NESTED_CALLS];
        DWORD			m_dwNested_Esp[MAX_NESTED_CALLS];
        DWORD			m_dwNested_StackTop[MAX_NESTED_CALLS];
        bool			m_dwNested_ExitEmul[MAX_NESTED_CALLS];
        DWORD			m_dwNestLevel;

		void			Cmd_notimplemented (BYTE cmd, const char * instr = 0);

		//////////////////////////////////////////////////////////////////////
		/// ALU commands
		//////////////////////////////////////////////////////////////////////

		// adc
		void	Cmd_adc_areg_imms (BYTE cmd, int size);
		void	Cmd_adc_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// add
		void	Cmd_add_areg_imms (BYTE cmd, int size);
		void	Cmd_add_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// and
		void	Cmd_and_areg_imms (BYTE cmd, int size);
		void	Cmd_and_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// or
		void	Cmd_or_areg_imms (BYTE cmd, int size);
		void	Cmd_or_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// xor
		void	Cmd_xor_areg_imms (BYTE cmd, int size);
		void	Cmd_xor_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// sbb
		void	Cmd_sbb_areg_imms (BYTE cmd, int size);
		void	Cmd_sbb_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// sub
		void	Cmd_sub_areg_imms (BYTE cmd, int size);
		void	Cmd_sub_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// cmp
		void	Cmd_cmp_areg_imms (BYTE cmd, int size);
		void	Cmd_cmp_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

		// test
		void	Cmd_test_areg_imms (BYTE cmd, int size);
		void	Cmd_test_rmx_rmx (BYTE cmd, bool fForceRMsRs, int size);

        /* inc/dec */
        void	Cmd_dec_rs		(BYTE cmd);
        void	Cmd_inc_rs		(BYTE cmd);

		/* data conversion */
        void	Cmd_aaa			(BYTE cmd);
        void	Cmd_aad			(BYTE cmd);
        void	Cmd_aam			(BYTE cmd);
        void	Cmd_aas			(BYTE cmd);
        void	Cmd_daa			(BYTE cmd);
        void	Cmd_das			(BYTE cmd);
        void	Cmd_cbw_cwde	(BYTE cmd);
        void	Cmd_cwd_cdq		(BYTE cmd);

        void	Cmd_imul 		(BYTE cmd, bool fForce8Bit, bool fIsImmxValue);



		//////////////////////////////////////////////////////////////////////
		/// Data moving commands
		//////////////////////////////////////////////////////////////////////

        /* mov */
        void	Cmd_mov_regs_imms	(BYTE cmd, int size);
		void	Cmd_mov_rmx_rmx		(BYTE cmd, bool fForceRMsRs, int size);
		void	Cmd_mov_rsreg_rsreg (BYTE cmd, bool fForceSreg_rm16);
        void	Cmd_lea				(BYTE cmd);
		void	Cmd_mov_aORoffs_aORoffs (BYTE cmd, bool fForceAregOffs, int size);
		void	Cmd_mov_rms_imms 	(BYTE cmd, int size);
		void	Cmd_movsx_rs_rms	(BYTE cmd, int size);
		void	Cmd_movzx_rs_rms	(BYTE cmd, int size);

		void	Cmd_xchg_rms_rs (BYTE cmd, bool fForce8bit);
		void	Cmd_xchg_regs	(BYTE cmd);

		/* stream repeatable operations */
		void	Cmd_scass		(BYTE cmd, int size, DWORD& dwRepCount, bool fRep);
		void	Cmd_cmpss		(BYTE cmd, int size, DWORD& dwRepCount, bool fRep);
		void	Cmd_movss		(BYTE cmd, int size, DWORD& dwRepCount);
		void	Cmd_stoss		(BYTE cmd, int size, DWORD& dwRepCount);
		void	Cmd_lodss		(BYTE cmd, int size, DWORD& dwRepCount);
		void	Cmd_xlat		(BYTE cmd);

        /* bit operations */
        void	Cmd_bt_rms_rs	(BYTE cmd);
        void	Cmd_btr_rms_rs	(BYTE cmd);
        void	Cmd_bts_rms_rs	(BYTE cmd);
        void	Cmd_btc_rms_rs	(BYTE cmd);
        void	Cmd_bsf_rs_rms	(BYTE cmd);
        void	Cmd_bsr_rs_rms	(BYTE cmd);
		void	Cmd_setcc_rms	(BYTE cmd, condition_t cond);
		void    Cmd_shld_rms_rs (BYTE cmd);
		void    Cmd_bswap_reg32 (BYTE cmd);

		/* CPU flags operations */
		void	Cmd_IFops 		(BYTE cmd);
		void	Cmd_DFops		(BYTE cmd);
		void	Cmd_CFops		(BYTE cmd);
		void	Cmd_lahf		(BYTE cmd);
		void	Cmd_sahf		(BYTE cmd);
        void	Cmd_setalc		(BYTE cmd);
	

		/* stack operations */
		void 	Cmd_pop_seg 	(int segreg);
		void	Cmd_pop_reg		(BYTE cmd);
		void	Cmd_pop_mem		(BYTE cmd);
		void 	Cmd_push_seg 	(int segreg);
		void	Cmd_push_reg	(BYTE cmd);
		void	Cmd_push_imms	(BYTE cmd, bool fForce8bit);
		void	Cmd_pusha		(BYTE cmd);
		void	Cmd_popa		(BYTE cmd);
		void	Cmd_pushf		(BYTE cmd);
		void	Cmd_popf		(BYTE cmd);
		void	Cmd_leave		();


		/* calls/jumps */
		void	Cmd_jmp_rels	(BYTE cmd, bool fForce8bit, condition_t cond);
		void	Cmd_jcxz		(BYTE cmd);
		void	Cmd_retn		(BYTE cmd, bool fImm16Present);
		void	Cmd_loop		(BYTE cmd, condition_t cond);
		void	Cmd_call		(BYTE cmd, bool fimm32);

        /* misc */
        void	Cmd_wrapper		(BYTE cmd);
        void	Cmd_0x80 		(BYTE cmd);
        void	Cmd_0x81_0x83	(BYTE cmd, bool fForce8bit);
        void	Cmd_0xF6 		(BYTE cmd);
        void	Cmd_0xF7 		(BYTE cmd);
        void	Cmd_0x0F_0xBA	(BYTE cmd);
        void	Cmd_0xFE 		(BYTE cmd);
        void	Cmd_0xFF 		(BYTE cmd);
        void	Cmd_0xC0 		(BYTE cmd);
        void	Cmd_0xC1 		(BYTE cmd);
        void	Cmd_0xD0 		(BYTE cmd);
        void	Cmd_0xD1 		(BYTE cmd);
        void	Cmd_0xD2 		(BYTE cmd);
        void	Cmd_0xD3 		(BYTE cmd);

        void	ExecuteCmd 		(BYTE cmd);
        void	Execute0FCmd 	(BYTE cmd);

        // Wrappers
        void    WrapperIntelToSparc (wrapper_t * ptr);

		BYTE *	m_bCodeStart;

// This is CORRECT place for those includes!!!
#include "cmd_alu.h"
#include "cmd_bits.h"
#include "cmd_mov.h"
#include "cmd_stack.h"

};



#endif
