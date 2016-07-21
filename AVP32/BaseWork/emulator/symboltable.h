/*
 *  symboltable.h,v 1.2 2002/10/17 09:55:03 tim 
 */

#ifndef CMD_SYMBOLTABLE_H
#define CMD_SYMBOLTABLE_H

#include "typedefs.h"

enum cpu_cmd_t
{
	SYM_CPU_X86,
	SYM_CPU_NATIVE
};


typedef struct
{
	std::string 	name;
	DWORD			addr;
	cpu_cmd_t		cpu;

} symbol_t;



class SymbolTable
{
	public:
		SymbolTable();
		DWORD				AddSymbol (const char * name, unsigned long addr, cpu_cmd_t	cpu);
		const symbol_t	*	SymLookup (const char * name, cpu_cmd_t	cpu) const;
		unsigned long		SymOrWrapperLookup (const char * name, cpu_cmd_t cpu) const;
#if defined (ENABLE_DISASM)
		const symbol_t	*	SymLookup (DWORD addr, cpu_cmd_t cpu) const;
#endif

	private:
		std::vector<symbol_t>	m_Symbols;
};


#endif
