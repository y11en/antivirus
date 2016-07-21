/*
 *  cmd_alu.h,v 1.2 2002/10/17 09:54:59 tim 
 */

#ifndef CMD_ALU_H
#define CMD_ALU_H

// problems:
// argument - ReadIx86... - converted
// IMUL - OF flag

/*
 * Template stuff:
 * Count of bits in the type T: sizeof(T) * 8
 */


// ADD template
template <class T> inline void CPU_ADD (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue;

    newvalue = oldvalue + argument;
    WRITE_X86_DATA (dest, newvalue);

    m_Regs.ApplyFlags (ZF|AF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_ADD);
}


// ADC template
template <class T> inline void CPU_ADC (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue;

    newvalue = oldvalue + argument + m_Regs.IsFlag(CF);
    WRITE_X86_DATA (dest, newvalue);

    m_Regs.ApplyFlags (ZF|AF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_ADC);
}


// AND template
template <class T> inline void CPU_AND (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue & argument;
       
	WRITE_X86_DATA (dest, newvalue);
	m_Regs.ApplyFlags (ZF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_BITOPS);
}


// OR template
template <class T> inline void CPU_OR (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue | argument;
       
	WRITE_X86_DATA (dest, newvalue);
	m_Regs.ApplyFlags (ZF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_BITOPS);
}


// XOR template
template <class T> inline void CPU_XOR (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue ^ argument;
       
	WRITE_X86_DATA (dest, newvalue);
	m_Regs.ApplyFlags (ZF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_BITOPS);
}


// SBB template
template <class T> inline void CPU_SBB (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue - argument - m_Regs.IsFlag(CF);
       
	WRITE_X86_DATA (dest, newvalue);
	m_Regs.ApplyFlags (ZF|AF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_SBB);
}


// SUB template
template <class T> inline void CPU_SUB (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue - argument;

	WRITE_X86_DATA (dest, newvalue);
	m_Regs.ApplyFlags (ZF|AF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_SUB);
}
		

// CMP template (like SUB but not store the result)
template <class T> inline void CPU_CMP (T * src, T * dest)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue - argument;
	m_Regs.ApplyFlags (ZF|AF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_SUB);
}


// TEST template (like AND but not store the result)
template <class T> inline void CPU_TEST (T * dest, T * src)
{
	register T argument = READ_X86_DATA (src);
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue & argument;
	m_Regs.ApplyFlags (ZF|SF|PF|CF|OF, newvalue, oldvalue, argument, CMD_BITOPS);
}


// NOT template. Only "dest" argument is valid
template <class T> inline void CPU_NOT (T * src, T * dest)
{
    register T oldvalue = READ_X86_DATA (dest);
	WRITE_X86_DATA (dest, ~oldvalue);
}


// NEG template. Only "dest" argument is valid
template <class T> inline void CPU_NEG (T * src, T * dest)
{
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = 0 - oldvalue;

	WRITE_X86_DATA (dest, newvalue);
	m_Regs.ApplyFlags (ZF|SF|PF|AF|CF, newvalue, oldvalue, oldvalue, CMD_NEG);
}


// MUL template
template <class T> inline void CPU_MUL (T * src, T * destlow, T * desthigh)
{
    QWORD oldvalue = READ_X86_DATA (destlow);
	T argument = READ_X86_DATA (src);
    QWORD newvalue = (QWORD) argument * oldvalue;

    // newvalue now contain a value in native machine byte order
	WRITE_X86_DATA (destlow, (T) newvalue);

	// We should rotate QWORD (64bit) by sizeof(T) bytes
	T highreg = (T) (newvalue >> (sizeof(T) * 8));
	WRITE_X86_DATA (desthigh, highreg);
}


// IMUL template
template <class T> inline void CPU_IMUL (T * src, T * destlow, T * desthigh)
{
    SQWORD oldvalue = READ_X86_SIGNED_DATA (destlow);
	T argument = READ_X86_SIGNED_DATA (src);
    SQWORD newvalue = argument * oldvalue;

    // newvalue now contain a value in native machine byte order
	WRITE_X86_DATA (destlow, (T) newvalue);

	// We should rotate QWORD (64bit) by sizeof(T) bytes
	T highreg = (T) (newvalue >> (sizeof(T) * 8));
	WRITE_X86_DATA (desthigh, highreg);

	// sign and parity flags are inherit from one of arguments
//	m_Regs.ApplyFlags (ZF|SF|PF, (QWORD) newvalue, (QWORD) newvalue, (QWORD) newvalue);

//	SET_BIT(m_Regs.eflags, CF|OF, 
//		(newvalue >> (sizeof(T) * 8 - 1) != 0) && (newvalue >> (sizeof(T) * 8 - 1) != -1) );
}


// DIV template
template <class T> inline void CPU_DIV (T * src, T * destlow, T * desthigh)
{
//    const QWORD OVERFLOW_VALUE = (QWORD) 1 << (sizeof(T)*8);
    QWORD oldvalue = ((QWORD) READ_X86_DATA (desthigh)) << (sizeof(T) * 8);
	oldvalue |= READ_X86_DATA (destlow);
	T argument = READ_X86_DATA (src);

	if ( argument == 0 )
		fatal (0, "Divizion by zero");

	// we have qword in native machine format
    T res1 = (T) (oldvalue % argument);

//  It seems newer processors never generate any overflows
//	if ( oldvalue / (QWORD) argument > OVERFLOW_VALUE )
//		fatal (0, "Divide overflow");
    
    T res2 = (T) (oldvalue/ (QWORD)argument);

    WRITE_X86_DATA (destlow, res2);
	WRITE_X86_DATA (desthigh, res1);
}


// IDIV template
template <class T> inline void CPU_IDIV (T * src, T * destlow, T * desthigh)
{
    SQWORD oldvalue = (SQWORD) ((QWORD) READ_X86_DATA (desthigh)) << (sizeof(T) * 8);
	oldvalue |= READ_X86_DATA (destlow);
	SQWORD argument = READ_X86_SIGNED_DATA (src);

	if ( argument == 0 )
		fatal (0, "Divizion by zero");

	// we have qword in native machine format
    T res1 = (T) (oldvalue % argument);
    T res2 = (T) (oldvalue/argument);

    WRITE_X86_DATA (destlow, res2);
	WRITE_X86_DATA (desthigh, res1);
}


// INC template
template <class T> inline void CPU_INC (T * dest)
{
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue + 1;

    WRITE_X86_DATA (dest, newvalue);
    m_Regs.ApplyFlags (ZF|AF|SF|PF|OF, newvalue, oldvalue, (T) 1, CMD_ADD);
}


// DEC template
template <class T> inline void CPU_DEC (T * dest)
{
    register T oldvalue = READ_X86_DATA (dest);
    register T newvalue = oldvalue - 1;

    WRITE_X86_DATA (dest, newvalue);
    m_Regs.ApplyFlags (ZF|AF|SF|PF|OF, newvalue, oldvalue, (T) 1, CMD_SUB);
}


// XCHG template
template <class T> inline void CPU_XCHG (T * dest, T * src)
{
	register T srcval = READ_X86_DATA (src);
    register T destval = READ_X86_DATA (dest);

    WRITE_X86_DATA (dest, srcval);
    WRITE_X86_DATA (src, destval);
}

#endif
