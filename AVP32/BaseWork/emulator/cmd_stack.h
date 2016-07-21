/*
 *  cmd_stack.h,v 1.2 2002/10/17 09:55:00 tim 
 */

#ifndef CMD_STACK_H
#define CMD_STACK_H


// PUSH template
template <class T> inline void CPU_PUSH (T * value)
{
	T * stacktop = (T *) m_Regs.GetStackTop() - 1;
    WRITE_X86_DATA(stacktop, READ_X86_DATA(value));
    m_Regs.SetStackTop(stacktop);
}


// POP template
template <class T> inline void CPU_POP (T * value)
{
	T * stacktop = (T *) m_Regs.GetStackTop();
    WRITE_X86_DATA (value, READ_X86_DATA(stacktop));
    m_Regs.SetStackTop (stacktop + 1);
}

#endif
