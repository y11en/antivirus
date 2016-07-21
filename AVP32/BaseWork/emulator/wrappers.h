/*
 *  wrappers.h,v 1.2 2002/10/17 09:55:03 tim 
 */

#ifndef CMD_WRAPPERS_H
#define CMD_WRAPPERS_H

#include "symboltable.h"

enum wrapper_lookup_t
{
	WRAPPER_NATIVE,			// the function is written in NATIVE machine code and is called from INTEL code
	WRAPPER_INTEL			// the function is written in INTEL machine code and is called from NATIVE code
};


typedef struct
{
	char	*			name;	// symbol name
	void	*			addr;	// Function code address for Intel-Sparc wrappers,
								// wrapper function address for Sparc-Intel wrappers
	int					count;	// Param count for Intel-Sparc wrappers
    wrapper_lookup_t	type;	// CPU type
} wrapper_t;


void 		wrappers_init (SymbolTable * pTable);
void 		wrapper_add (const char * name, void * wrapaddr, int count, wrapper_lookup_t type);
wrapper_t * wrapper_lookup (const char * name, wrapper_lookup_t type);
wrapper_t * wrapper_lookup (DWORD addr, wrapper_lookup_t type);

#endif /* CMD_WRAPPERS_H */
