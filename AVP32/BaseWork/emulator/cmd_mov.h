/*
 *  cmd_mov.h,v 1.2 2002/10/17 09:55:00 tim 
 */

#ifndef CMD_MOV_H
#define CMD_MOV_H


// MOV template
template <class T> inline void CPU_MOV (T * dest, T * src)
{
	WRITE_X86_DATA (dest, READ_X86_DATA (src));
}


#endif
