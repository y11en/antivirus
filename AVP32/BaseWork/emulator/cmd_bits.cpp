#ifndef lint
static const char rcsid[] =
    " cmd_bits.cpp,v 1.2 2002/10/17 09:54:59 tim ";
#endif

#include "typedefs.h"


/*
 * bit manipilation commands
 */

/* BSF rs, r/ms */
void I386CPU::Cmd_bsf_rs_rms (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
        CPU_BSF (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
	else
        CPU_BSF (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));

	DISASSEMBLE (("bsf %mreg, %mrm"));
}


/* BSR rs, r/ms */
void I386CPU::Cmd_bsr_rs_rms (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
        CPU_BSR (m_Regs.GetModRMRegW (mod_rm), m_Regs.GetModRMRMW (mod_rm));
	else
        CPU_BSR (m_Regs.GetModRMRegD (mod_rm), m_Regs.GetModRMRMD (mod_rm));

	DISASSEMBLE (("bsr %mreg, %mrm"));
}



/* BTC rs, r/ms */
void I386CPU::Cmd_btc_rms_rs (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
        CPU_BTC (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm));
	else
        CPU_BTC (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm));

	DISASSEMBLE (("btc %mrm, %mreg"));
}


/* BT r/ms, rs */
void I386CPU::Cmd_bt_rms_rs (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
        CPU_BT (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm));
	else
        CPU_BT (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm));

	DISASSEMBLE (("bt %mrm, %mreg"));
}



/* BTR rs, r/ms */
void I386CPU::Cmd_btr_rms_rs (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
        CPU_BTR (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm));
	else
        CPU_BTR (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm));

	DISASSEMBLE (("btr %mrm, %mreg"));
}



/* BTS rs, r/ms */
void I386CPU::Cmd_bts_rms_rs (BYTE cmd)
{
    BYTE mod_rm = ReadIx86Byte();

	if ( m_Regs.GetOperandSize() == 16 )
        CPU_BTS (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm));
	else
        CPU_BTS (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm));

	DISASSEMBLE (("bts %mrm, %mreg"));
}


/* SETcc r/ms */
void I386CPU::Cmd_setcc_rms (BYTE cmd, condition_t cond)
{
    WRITE_X86_BYTE (m_Regs.GetModRMRMB (ReadIx86Byte()), m_Regs.CheckCondition (cond));
	DISASSEMBLE (("set%cond %mrm", cond));
}


void I386CPU::Cmd_shld_rms_rs (BYTE cmd)
{
    BYTE count, mod_rm = ReadIx86Byte();
    const char * disasm_string;
    
	if ( cmd == 0xA4 )
	{
		count = ReadIx86Byte();
		disasm_string = "shld %mrm, %mreg, %lb";
	}
	else
	{
		count = READ_X86_BYTE (m_Regs.GetReg8(CL));
		disasm_string = "shld %mrm, %mreg, cl";
	}
	
	if ( m_Regs.GetOperandSize() == 16 )
        CPU_SHLD (m_Regs.GetModRMRMW (mod_rm), m_Regs.GetModRMRegW (mod_rm), count);
	else
        CPU_SHLD (m_Regs.GetModRMRMD (mod_rm), m_Regs.GetModRMRegD (mod_rm), count);
        
	DISASSEMBLE ((disasm_string));
}


void I386CPU::Cmd_bswap_reg32 (BYTE cmd)
{
	DWORD * reg	= m_Regs.GetReg32(cmd);
	DWORD regval = READ_X86_DWORD (reg);

	DWORD byte0 = regval & 0xFF;
	regval >>= 8;
	DWORD byte1  = regval & 0xFF;
	regval >>= 8;
	DWORD byte2  = regval & 0xFF;
	regval >>= 8;
	DWORD byte3  = regval;

	WRITE_X86_DWORD (reg, (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3);
	DISASSEMBLE (("bswap %r32"));
}
