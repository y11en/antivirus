/*
 *  elfbases.h,v 1.2 2002/10/17 09:55:01 tim 
 */

#ifndef HDR_ELFBASES_H
#define HDR_ELFBASES_H

#include "typedefs.h" 


class ElfBaseLoader
{
	public:
		ElfBaseLoader();
		~ElfBaseLoader();
		bool LoadElfBases (SymbolTable * pTable);
		bool LinkElfBases  (SymbolTable * pTable);
		const char * GetLastError();

	private:
		bool LoadBase (const char * filename, SymbolTable * pTable);

		// ELF modules are kept here after loading up to linking
		std::vector<ELFparser*>		m_ElfModules;
		char						m_sErrorMsg[256];
};


#endif
