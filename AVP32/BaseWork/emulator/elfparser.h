/*
 *  elfparser.h,v 1.2 2002/10/17 09:55:02 tim 
 */

#ifndef HDR_ELFPARSER_H
#define HDR_ELFPARSER_H

#include "typedefs.h" 

class SymbolTable;

class ELFparser
{
	public:
		ELFparser(const char * name, const char * link_name = 0);
		~ELFparser();
		bool ParseFile (const char * filename, SymbolTable * pTable);
		bool ParseBuffer (char * buf, SymbolTable * pTable);
		bool Link  (SymbolTable * pTable);
        void DumpSymbol (Elf32_Sym * sym, const char * symstring);
        const char * GetModuleName();
        const char * GetError();

	private:
		bool			fDataIsBigEndian;
		DWORD			ElfDWord (DWORD data);
		WORD			ElfWord  (WORD data);
		void			WriteUnalignedDWord (DWORD * addr, DWORD data);
		DWORD			ReadUnalignedDWord (DWORD * addr);

		char 		*	m_sBuf;
		char		*	m_sModuleName;
		char		*	m_sLinkName;
		bool			m_bLinkFound;
        Elf32_Ehdr	*	m_Ehdr;
		Elf32_Shdr	*	m_Shdr_first;
		const char	*	m_sSectionStringTable;
		char			m_sErrorText[256];
};


#endif
