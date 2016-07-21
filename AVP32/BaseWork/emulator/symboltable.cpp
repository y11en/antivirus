/*
 *  symboltable.cpp,v 1.2 2002/10/17 09:55:03 tim 
 */

#include "typedefs.h"

//#define DEBUG_SYMBOL_TABLE


SymbolTable::SymbolTable()
{
}


DWORD SymbolTable::AddSymbol (const char * name, unsigned long addr, cpu_cmd_t cpu)
{
	for ( unsigned i = 0; i < m_Symbols.size(); i++ )
	{
		if ( m_Symbols[i].name == (std::string) name )
		{
			if ( m_Symbols[i].cpu != cpu )
				//return 0;	// skip symbol
				continue;

#ifdef DEBUG_SYMBOL_TABLE
			if ( strcmp (name, ".strip.") )
				printf ("Symbol table: replaced symbol %s, addr %08X, CPU %d\n", name, addr, cpu);
#endif
			m_Symbols[i].addr = (DWORD) addr;
		    m_Symbols[i].name = name;
		    m_Symbols[i].cpu = cpu;
		    return m_Symbols[i].addr;
		}
	}

	symbol_t sym;
	sym.addr = (DWORD) addr;
    sym.name = name;
	sym.cpu = cpu;

	m_Symbols.push_back(sym);

#ifdef DEBUG_SYMBOL_TABLE
	if ( strcmp (name, ".strip.") )
		printf ("Symbol table: added symbol %s, addr %08X, CPU %d\n", name, addr, cpu);
#endif
	return sym.addr;
}


const symbol_t * SymbolTable::SymLookup (const char * name, cpu_cmd_t cpu) const
{
	for (unsigned i = 0; i < m_Symbols.size(); i++ )
	{
		if ( m_Symbols[i].cpu == cpu
		&& m_Symbols[i].name == (std::string) name )
			return &m_Symbols[i];
	}

	return 0;
}


unsigned long SymbolTable::SymOrWrapperLookup (const char * name, cpu_cmd_t cpu) const
{
	const symbol_t * entry;
	const wrapper_t * wrapper = 0;

	if ( cpu == SYM_CPU_X86 )
	{
		if ( (wrapper = wrapper_lookup (name, WRAPPER_NATIVE)) != 0 )
			return (unsigned long) wrapper->addr;

		if ( (entry = SymLookup(name, cpu)) != 0 )
			return (unsigned long) entry->addr;

		printf ("Can't find symbol or I2S wrapper: %s\n", name);
	}
	else
	{
		if ( (entry = SymLookup(name, cpu)) != 0 )
			return (unsigned long) entry->addr;

		if ( (wrapper = wrapper_lookup (name, WRAPPER_INTEL)) != 0 )
			return (unsigned long) wrapper->addr;

		printf ("Can't find symbol or S2I wrapper: %s\n", name);
	}

	return 0;
}


#if defined (ENABLE_DISASM)
const symbol_t * SymbolTable::SymLookup (DWORD addr, cpu_cmd_t cpu) const
{
	for (unsigned i = 0; i < m_Symbols.size(); i++ )
		if ( m_Symbols[i].addr == addr && m_Symbols[i].cpu == cpu )
			return &m_Symbols[i];

	return 0;
}
#endif
