/*
 *  cmd_bits.h,v 1.2 2002/10/17 09:55:00 tim 
 */

#ifndef CMD_BITS_H
#define CMD_BITS_H


// BSF template
template <class T> inline void CPU_BSF (T * dest, T * src)
{
	register T argument = READ_X86_DATA (src);
	const int BITS = sizeof(T) * 8 - 1;
    T mask = 1;

	for ( BYTE i = 0; i < BITS; i++, mask = mask << 1 )
	{
		if ( IS_SET (argument, mask) )
		{
			WRITE_X86_DATA (dest, (T) i);
            m_Regs.ApplyFlags (ZF, 1, 1);	// Nice trick to not set ZF manually
            return;
		}
	}

    // Nice trick to not set ZF manually
	m_Regs.ApplyFlags (ZF, 0, 0);
}


template <class T> inline void CPU_BSR (T * dest, T * src)
{
	register T argument = READ_X86_DATA (src);
	const int BITS = sizeof(T) * 8 - 1;
    T mask = (1 << BITS);

	for ( BYTE i = BITS; i > 0; i--, mask >>= 1 )
	{
		if ( IS_SET (argument, mask) )
		{
			WRITE_X86_DATA (dest, (T) i);
            m_Regs.ApplyFlags (ZF, 1, 1);	// Nice trick to not set ZF manually
            return;
		}
	}

    // Nice trick to not set ZF manually
	m_Regs.ApplyFlags (ZF, 0, 0);
}



template <class T> inline void CPU_BT (T * datasrc, T * bitnum)
{
	register T src = READ_X86_DATA (datasrc);
	register T bit = 1 << READ_X86_DATA (bitnum);

    SET_BIT (m_Regs.eflags, CF, IS_SET(src, bit));
}


template <class T> inline void CPU_BTC (T * datasrc, T * bitnum)
{
	register T src = READ_X86_DATA (datasrc);
	register T bit = 1 << READ_X86_DATA (bitnum);

	// Toggle bit
	src ^= bit;

    WRITE_X86_DATA (datasrc, src);
    SET_BIT (m_Regs.eflags, CF, !IS_SET(src,  bit)); // previous bit value
}


template <class T> inline void CPU_BTR (T * datasrc, T * bitnum)
{
	register T src = READ_X86_DATA (datasrc);
	register T bit = 1 << READ_X86_DATA (bitnum);

	SET_BIT (m_Regs.eflags, CF, IS_SET(src, bit));
	SET_BIT (src, bit, (T) 0);
    WRITE_X86_DATA (datasrc, src);
}


template <class T> inline void CPU_BTS (T * datasrc, T * bitnum)
{
	register T src = READ_X86_DATA (datasrc);
	register T bit = 1 << READ_X86_DATA (bitnum);

	SET_BIT (m_Regs.eflags, CF, IS_SET(src, bit));
	SET_BIT (src, bit, (T) 1);
    WRITE_X86_DATA (datasrc, src);
}


template <class T> inline void CPU_ROL (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;
	T flagCF = 0;

	for ( ; bitnum > 0; bitnum-- )
	{
		flagCF = (value & SIGNBIT) != 0 ? 1 : 0;
		value = (value << 1) | flagCF;
	}

	WRITE_X86_DATA (datasrc, value);
	SET_BIT (m_Regs.eflags, CF, flagCF);
	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
}



template <class T> inline void CPU_ROR (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;
	T flagCF = 0;

	for ( ; bitnum > 0; bitnum-- )
	{
		flagCF = (value & 1) != 0 ? SIGNBIT : 0;
		value = (value >> 1) | flagCF;
	}

	WRITE_X86_DATA (datasrc, value);
	SET_BIT (m_Regs.eflags, CF, flagCF);
	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
}


template <class T> inline void CPU_RCL (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;
	T flagCF = 0, oldCF = 0;

    flagCF = IS_SET(m_Regs.eflags, CF) ? 1 : 0;

	for ( ; bitnum > 0; bitnum-- )
	{
		oldCF = flagCF;
		flagCF = (value & SIGNBIT) != 0 ? 1 : 0;
		value = (value << 1) | oldCF;
	}

	WRITE_X86_DATA (datasrc, value);
	SET_BIT (m_Regs.eflags, CF, flagCF);
	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
}



template <class T> inline void CPU_RCR (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;
	T flagCF = 0, oldCF = 0;

    flagCF = IS_SET(m_Regs.eflags, CF) ? 1 : 0;

	for ( ; bitnum > 0; bitnum-- )
	{
		oldCF = flagCF ? SIGNBIT : 0;
		flagCF = (value & 1) != 0;
		value = (value >> 1) | oldCF;
	}

	WRITE_X86_DATA (datasrc, value);
	SET_BIT (m_Regs.eflags, CF, flagCF);
	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
}


template <class T> inline void CPU_SAL (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;

	SET_BIT (m_Regs.eflags, CF, ((value << (bitnum-1)) & SIGNBIT) );
	value <<= bitnum;

	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
	WRITE_X86_DATA (datasrc, value);
    m_Regs.ApplyFlags (ZF|SF|PF|AF, value, (T) 1, (T) 1, CMD_SHIFTOPS);
}


template <class T> inline void CPU_SHR (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;

	SET_BIT (m_Regs.eflags, CF, ((value >> (bitnum - 1)) & 1) );
	value >>= bitnum;

	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
	WRITE_X86_DATA (datasrc, value);
    m_Regs.ApplyFlags (ZF|SF|PF|AF, value, (T) 1, (T) 1, CMD_SHIFTOPS);
}


template <class T> inline void CPU_SAR (T * datasrc, BYTE bitnum)
{
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));
	T value = READ_X86_DATA (datasrc);
    T signbit = value & SIGNBIT;
	T flagCF = 0;

	for ( ; bitnum > 0; bitnum-- )
	{
		flagCF = value & 1;
		value = (value >> 1) | signbit;
	}

	WRITE_X86_DATA (datasrc, value);
	SET_BIT (m_Regs.eflags, CF, flagCF);
	SET_BIT (m_Regs.eflags, OF, signbit != (value & SIGNBIT) );
    m_Regs.ApplyFlags (ZF|SF|PF|AF, value, (T) 1, (T) 1, CMD_SHIFTOPS);
}


template <class T> inline void CPU_SHLD (T * dest, T * bitsrc, BYTE count)
{
	const T BITSCOUNT = sizeof(T) * 8;
	const T SIGNBIT = (1 << (sizeof(T) * 8 - 1));	
	T op1 = READ_X86_DATA (dest);
	T op2 = READ_X86_DATA (bitsrc);
	count &= 0x1F;
	
	T result = (op1 << count) | (op2 >> (BITSCOUNT - count));
	WRITE_X86_DATA (dest, result);

	SET_BIT (m_Regs.eflags, CF, op1 >> (BITSCOUNT - count) & 0x01);
	SET_BIT (m_Regs.eflags, OF, (count == 1 && ((op1 ^ result) & SIGNBIT) > 0));
	
    m_Regs.ApplyFlags (ZF|SF|PF, result, (T) 1, (T) 1, CMD_SHIFTOPS);
}


#endif
