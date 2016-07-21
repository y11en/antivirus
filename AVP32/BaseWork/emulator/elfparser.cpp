/*
 *  elfparser.cpp,v 1.2 2002/10/17 09:55:01 tim 
 */

#include "typedefs.h"

#if defined (hpux)
	#define RTLD_DEFAULT (void*) RTLD_GLOBAL
#endif

//#define DEBUG_ELF_LOADER

ELFparser::ELFparser(const char * name, const char * link_name)
{
	m_sBuf = 0;
    m_sModuleName = strdup(name);
    m_bLinkFound = false;

    if ( link_name )
    	m_sLinkName = strdup(link_name);
	else
		m_sLinkName = 0;

	strcpy (m_sErrorText, "file i/o error");
}


ELFparser::~ELFparser()
{
	if ( m_sBuf )
		delete m_sBuf;

	if ( m_sModuleName )
		free(m_sModuleName);

	if ( m_sLinkName )
		free (m_sLinkName);
}

// FIXME: ELFs with little-endian data
DWORD ELFparser::ElfDWord (DWORD data)
{
	return 0;
}

// FIXME: ELFs with little-endian data
WORD ELFparser::ElfWord  (WORD data)
{
	return 0;
}


void ELFparser::WriteUnalignedDWord (DWORD * ptr, DWORD data)
{
	((unsigned char *) ptr)[3] = (unsigned char) data;
	data >>= 8;
	((unsigned char *) ptr)[2] = (unsigned char) data;
	data >>= 8;
	((unsigned char *) ptr)[1] = (unsigned char) data;
	data >>= 8;
	((unsigned char *) ptr)[0] = (unsigned char) data;
}


DWORD ELFparser::ReadUnalignedDWord (DWORD * ptr)
{
	register DWORD value = 0;
	value |= ((unsigned char *) ptr)[0];
	value <<= 8;
	value |= ((unsigned char *) ptr)[1];
	value <<= 8;
	value |= ((unsigned char *) ptr)[2];
	value <<= 8;
	value |= ((unsigned char *) ptr)[3];
	return value;
}


#ifdef DEBUG_ELF_LOADER
void ELFparser::DumpSymbol (Elf32_Sym * sym, const char * symstring)
{
	const char * secname, * objname, * visibility;

	switch (sym->st_shndx)
	{
	case SHN_UNDEF:
		secname = "UNDEF";
		break;

	case SHN_ABS:
		secname = "ABS";
		break;

	case SHN_COMMON:
		secname = "COMMON";
		break;

	default:
		secname = m_sSectionStringTable + (m_Shdr_first + sym->st_shndx)->sh_name;
		break;
	}

	switch (ELF32_ST_TYPE(sym->st_info))
	{
	default:
		objname = "UNKNOWN";
		break;

	case STT_NOTYPE:
		objname = "NOTYPE";
		break;

	case STT_OBJECT:
		objname = "OBJECT";
		break;

	case STT_FUNC:
		objname = "FUNC";
		break;

	case STT_SECTION:
		objname = "SECTION";
		break;

	case STT_FILE:
		objname = "FILE";
		break;

	case STT_NUM:
		objname = "NUM";
		break;
	}

	switch (ELF32_ST_BIND(sym->st_info))
	{
	default:
		visibility = "UNKNOWN";
		break;

	case STB_LOCAL:
		visibility = "LOCAL";
		break;

	case STB_GLOBAL:
		visibility = "GLOBAL";
		break;

	case STB_WEAK:
		visibility = "WEAK";
		break;

	case STB_NUM:
		visibility = "NUM";
		break;
	}

	printf (" %08X  %08X  %-8s  %-7s   %-6s    %s",
		sym->st_value, sym->st_size, objname, visibility, secname,
		sym->st_name ? symstring + sym->st_name : "EMPTY");
}
#endif


const char * ELFparser::GetModuleName()
{
	return m_sModuleName ? m_sModuleName : "NONAME";
}


const char * ELFparser::GetError()
{
	return m_sErrorText;
}

/*
bool ELFparser::ParsePartOfFile (FILE * fp, size_t size, CPU_Memory * pMemory, SymbolTable * pTable)
{
    m_sBuf = new char [size];

	if ( (fread(m_sBuf, 1, size, fp)) != size )
	{
		sprintf (m_sErrorText, "%s: Could not read file: %s", m_sModuleName, strerror(errno) );
		return false;
	}

	return ParseElfBuffer(pMemory, pTable);
}
*/

bool ELFparser::ParseFile (const char * filename, SymbolTable * pTable)
{
	FILE * fp;

	if ( (fp = fopen(filename, "rb")) == 0 )
	{
		sprintf (m_sErrorText, "%s: Could not open %s: %s", m_sModuleName, filename, strerror(errno) );
		return false;
	}

	fseek (fp, 0, SEEK_END);
	DWORD size = ftell (fp);
	fseek (fp, 0, SEEK_SET);

    char * buf = new char [size];

	if ( (fread(buf, 1, size, fp)) != size )
	{
		sprintf (m_sErrorText, "%s: Could not read %s: %s", m_sModuleName, filename, strerror(errno) );
		fclose (fp);
		return false;
	}

	fclose (fp);
	return ParseBuffer(buf, pTable);
}


bool ELFparser::ParseBuffer (char * buf, SymbolTable * pTable)
{
    int i;

    m_sBuf = buf;

	// Parse header
    m_Ehdr = (Elf32_Ehdr *) m_sBuf;

    if ( m_Ehdr->EI_MAG0 != ELFMAG0
    || m_Ehdr->EI_MAG1 != ELFMAG1
    || m_Ehdr->EI_MAG2 != ELFMAG2
    || m_Ehdr->EI_MAG3 != ELFMAG3
    || m_Ehdr->EI_CLASS != EI_CLASS32
	|| m_Ehdr->e_type != EI_TYPE
	|| m_Ehdr->e_machine != EI_MACHINE
	|| m_Ehdr->e_ehsize && (m_Ehdr->e_ehsize != sizeof (Elf32_Ehdr))
	|| m_Ehdr->e_phentsize && (m_Ehdr->e_phentsize != sizeof (Elf32_phdr))
	|| m_Ehdr->e_shentsize && (m_Ehdr->e_shentsize!= sizeof (Elf32_Shdr)) )
	{
		sprintf (m_sErrorText, "%s: Bad ELF file: header error", m_sModuleName);
		return false;
	}

	if ( m_Ehdr->EI_DATA != ELFDATA2MSB )
	{
		sprintf (m_sErrorText, "%s: unsupported byte order", m_sModuleName);
		return false;
	}

	fDataIsBigEndian = true;

#ifdef DEBUG_ELF_LOADER
	printf ("Start parsing ELF file, big-endian mode\n");
#endif

	// Program headers
	if ( m_Ehdr->e_phnum )
	{
		sprintf (m_sErrorText, "%s: Program headers found, not an object file", m_sModuleName);
		return false;
	}

	// Section headers
	if ( !m_Ehdr->e_shnum )
	{
		sprintf (m_sErrorText, "%s: No section headers found", m_sModuleName);
		return false;
	}

	// Section headers
	if ( m_Ehdr->e_shstrndx == SHN_UNDEF )
	{
		sprintf (m_sErrorText, "%s: No string table found", m_sModuleName);
		return false;
	}

    m_Shdr_first = (Elf32_Shdr *) (m_sBuf + m_Ehdr->e_shoff);
	m_sSectionStringTable = m_sBuf + m_Shdr_first[m_Ehdr->e_shstrndx].sh_offset;

	for ( i = 1; i < m_Ehdr->e_shnum; i++ )
	{
		Elf32_Shdr * shdr = m_Shdr_first + i;

		if ( (shdr->sh_type != SHT_PROGBITS && shdr->sh_type != SHT_NOBITS)
		|| !(shdr->sh_flags & SHF_ALLOC) )
			continue;

		// section has data to read
		if ( shdr->sh_flags & SHF_WRITE )
			shdr->sh_addr = (DWORD) gEmul.AllocRegion (shdr->sh_size);
		else
			shdr->sh_addr = (DWORD) gEmul.AllocRegion (shdr->sh_size);

		// copy initialized data
#ifdef DEBUG_ELF_LOADER
		if ( shdr->sh_type != SHT_NOBITS )
		{
			printf("Section %s is loaded into memory at %08X\n", m_sSectionStringTable + shdr->sh_name, shdr->sh_addr);
			memcpy ((char*)shdr->sh_addr, m_sBuf + shdr->sh_offset, shdr->sh_size);
		}
		else
			printf("Section %s at %08X is cleaned\n", m_sSectionStringTable + shdr->sh_name, shdr->sh_addr);
#else
		if ( shdr->sh_type != SHT_NOBITS )
			memcpy ((char*)shdr->sh_addr, m_sBuf + shdr->sh_offset, shdr->sh_size);
#endif
	}

	// Export all global symbols
	for ( i = 1; i < m_Ehdr->e_shnum; i++ )
	{
		Elf32_Shdr * shdr = m_Shdr_first + i;

		// only symbol tables
		if ( shdr->sh_type != SHT_SYMTAB )
			continue;

		Elf32_Sym * symbol_table = (Elf32_Sym *) (m_sBuf + shdr->sh_offset);
		const char * sym_string_table = m_sBuf + m_Shdr_first[shdr->sh_link].sh_offset;

#ifdef DEBUG_ELF_LOADER
		printf("Parse symbol table section %s\n", m_sSectionStringTable + shdr->sh_name);
#endif

        int sym_entries = shdr->sh_size/shdr->sh_entsize;
        for ( int i = 1; i < sym_entries; i++ )
        {
        	Elf32_Sym * sym = symbol_table + i;
        	const char * orig_sym_name = sym_string_table + sym->st_name;
			std::string sym_name = (std::string) "_" + (std::string) (orig_sym_name);

			if ( sym->st_shndx == SHN_ABS || sym->st_shndx == SHN_COMMON )
			{
#ifdef DEBUG_ELF_LOADER
				printf ("Skip ABS/COMMON symbol %s\n", orig_sym_name);
#endif
				continue;
			}

			if ( sym->st_shndx != SHN_UNDEF )
			{
				// Symbol is defined in the current object
#ifdef DEBUG_ELF_LOADER
				DWORD old = sym->st_value;
                sym->st_value += (m_Shdr_first + sym->st_shndx)->sh_addr;
				printf ("[%d] Symbol %s value: %08X -> %08X, section %s\n", i, orig_sym_name, old, sym->st_value, m_sSectionStringTable + (m_Shdr_first + sym->st_shndx)->sh_name);
#else
				sym->st_value += (m_Shdr_first + sym->st_shndx)->sh_addr;
#endif					

				if ( ELF32_ST_BIND(sym->st_info) == STB_GLOBAL )
				{
					if ( !strcmp(SYMBOL_DECODE_ENTRY_NAME, orig_sym_name) )
					{
						if ( m_bLinkFound )
						{
							sprintf (m_sErrorText, "link defined twice!");
							return false;
						}

						if ( m_sLinkName )
						{
#ifdef DEBUG_ELF_LOADER
							printf ("Added ELF link: %s (%08X), section %s\n", m_sLinkName, sym->st_value, m_sSectionStringTable + (m_Shdr_first + sym->st_shndx)->sh_name);
#endif
							gEmul.AddElfFunction(m_sLinkName, (void*) sym->st_value);
                            m_bLinkFound = true;
						}
						else
						{
							sprintf (m_sErrorText, "link without a name!");
							return false;
						}
					}
					else if ( !strncmp(orig_sym_name, SYMBOL_WRAPPER_PREFIX, strlen(SYMBOL_WRAPPER_PREFIX)) )
					{
						char *p, tmpbuf[256];

						strcpy (tmpbuf, orig_sym_name + strlen(SYMBOL_WRAPPER_PREFIX));

						if ( (p = strchr (tmpbuf, '_')) == 0 )
						{
							sprintf (m_sErrorText, "%s: bad export format", orig_sym_name);
							return false;
						}

                        *p++ = '\0';
                        int numarg = atoi(tmpbuf);

#ifdef DEBUG_ELF_LOADER
						printf ("Added Intel->Sparc wrapper: %s (%d args)\n", p, numarg);
#endif
						wrapper_add (p, (void*) sym->st_value, numarg, WRAPPER_NATIVE);
					}
					else
					{
						pTable->AddSymbol (sym_name.c_str(), sym->st_value, SYM_CPU_NATIVE);
						wrapper_t * wr = wrapper_lookup (sym_name.c_str(), WRAPPER_NATIVE);

	                    if ( wr )
	                    {
							wr->addr = (void*) sym->st_value;
#ifdef DEBUG_ELF_LOADER
							printf ("Added global wrapped symbol %s (%08X), section %s\n", sym_name.c_str(), wr->addr, m_sSectionStringTable + (m_Shdr_first + sym->st_shndx)->sh_name);
#endif					
						}
#ifdef DEBUG_ELF_LOADER
						else
						{

							printf ("Added global symbol: %s (%08X), section %s\n", sym_name.c_str(), sym->st_value, m_sSectionStringTable + (m_Shdr_first + sym->st_shndx)->sh_name);
						}
#endif
					}
				}
			}
        }
	}

	return true;
}


bool ELFparser::Link (SymbolTable * pTable)
{
#ifdef DEBUG_ELF_LOADER
	const char * globalsym = "", *reloctxt;
#endif

	// Apply the relocations
	for ( int i = 1; i < m_Ehdr->e_shnum; i++ )
	{
		Elf32_Shdr * shdr = m_Shdr_first + i;

		// only relocations RELA
		if ( shdr->sh_type != SHT_RELA )
			continue;

#ifdef DEBUG_ELF_LOADER
		printf("\nParse relocation section %s (symbol table %d)\n", m_sSectionStringTable + shdr->sh_name, shdr->sh_link);
#endif

		// Set up relocation symbol table
		Elf32_Sym * symbol_table = (Elf32_Sym *) (m_sBuf + (m_Shdr_first + shdr->sh_link)->sh_offset);
		const char * sym_string_table = m_sBuf + m_Shdr_first[(m_Shdr_first + shdr->sh_link)->sh_link].sh_offset;

		// Set up first relocation
        Elf32_Rela * rel_first = (Elf32_Rela *) (m_sBuf + shdr->sh_offset);
        int rel_entries = shdr->sh_size/shdr->sh_entsize;

        for ( int i = 0; i < rel_entries; i++ )
        {
        	Elf32_Rela * rel = rel_first + i;

            int reloc_sym = ELF32_R_SYM(rel->r_info);
            int reloc_type = ELF32_R_TYPE(rel->r_info);
            DWORD * reloc_addr = 0;

            // Is section loaded?
			if ( m_Shdr_first[shdr->sh_info].sh_addr == 0 )
                reloc_addr = (DWORD *) m_sBuf + m_Shdr_first[shdr->sh_info].sh_offset + rel->r_offset;
			else
				reloc_addr = (DWORD *) (m_Shdr_first[shdr->sh_info].sh_addr + rel->r_offset);

			if ( !reloc_sym )
			{
				sprintf (m_sErrorText, "%s: Empty symbol relocation", m_sModuleName);
				return false;
			}

			Elf32_Sym * sym = symbol_table + reloc_sym;
			std::string sym_name = (std::string) "_" + std::string(sym_string_table + sym->st_name);
			DWORD sym_addr = sym->st_value;

			// Resolve global symbols
            if ( sym->st_shndx == SHN_UNDEF )
			{
				// Symbol must be defined outside the current object
				if ( sym_name == "_" || ELF32_ST_BIND(sym->st_info) != STB_GLOBAL )
				{
					sprintf (m_sErrorText, "%s: Undefinite non-GLOBAL symbol found: '%s'", m_sModuleName, sym_name.c_str());
					return false;
				}

				const symbol_t * entry;

				// First we try to find a wrapper
				const wrapper_t * wrapper = wrapper_lookup (sym_name.c_str(), WRAPPER_INTEL);

				if ( wrapper && wrapper->addr )
				{
					// Symbol is a wrapper
					sym_addr = (DWORD) wrapper->addr;
#ifdef DEBUG_ELF_LOADER
					globalsym = "SPARC wrapper";
#endif
				}
				else if ( (wrapper = wrapper_lookup (sym_name.c_str(), WRAPPER_NATIVE)) != 0 )
				{
					sym_addr = (DWORD) wrapper->addr;
#ifdef DEBUG_ELF_LOADER
					globalsym = "SPARC wrapper";
#endif
				}
				else if ( (entry = pTable->SymLookup(sym_name.c_str(), SYM_CPU_NATIVE)) != 0 )
				{
					// Symbol is defined in SPARC code
					sym_addr = entry->addr;
#ifdef DEBUG_ELF_LOADER
					globalsym = "SPARC symbols";
#endif
				}
				else if ( (entry = pTable->SymLookup(sym_name.c_str(), SYM_CPU_X86)) != 0 )
				{
#ifdef DEBUG_ELF_LOADER
					globalsym = "INTEL symbols";
#endif
					// Symbol is defined in Intel machine code
					// If the symbol is not a function, link it
					if ( reloc_type == STT_FUNC )
					{
						sprintf (m_sErrorText, "%s: Can't find the SPARC->INTEL wrapper for symbol %s.", m_sModuleName, sym_name.c_str());
						return false;
					}

					sym_addr = entry->addr;
				}
				else
				{
					// Symbol isn't found in symbol table. Try to resolve with local symbols
					if ( sym_name[0] == '_'
					&& dlsym (RTLD_DEFAULT, sym_name.c_str() + 1) ) // skip _
					{
#ifdef DEBUG_ELF_LOADER
						globalsym = "internal symbols";
#endif
						sym_addr = (DWORD) dlsym (RTLD_DEFAULT, sym_name.c_str() + 1);

						if ( !sym_addr )
						{
							sprintf (m_sErrorText, "%s: Wrapped symbol %s could not be resolved", m_sModuleName, sym_name.c_str());
							return false;
						}
					}
					else
					{
						sprintf (m_sErrorText, "%s: Unresolved external symbol: %s", m_sModuleName, sym_name.c_str());
						return false;
					}
				}
			}

			DWORD old_value = *reloc_addr;

			switch (reloc_type)
			{
			case R_SPARC_32:
				*reloc_addr = sym_addr + rel->r_addend;
#ifdef DEBUG_ELF_LOADER
                reloctxt = "32";
#endif
				break;

			case R_SPARC_WDISP30:
				*reloc_addr = (old_value & 0xC0000000) | ((rel->r_addend + sym_addr - (unsigned int) reloc_addr) >> 2);
#ifdef DEBUG_ELF_LOADER
                reloctxt = "WDISP30";
#endif
				break;

			case R_SPARC_HI22:
				if ( !sym_addr )
					sym_addr = (DWORD) m_sBuf + rel->r_addend;
				else
					sym_addr += rel->r_addend;
				*reloc_addr = (old_value & 0xFFC00000) | (sym_addr >> 10);

#ifdef DEBUG_ELF_LOADER
                reloctxt = "HI22";
#endif
				break;

			case R_SPARC_LO10:
				if ( !sym_addr )
					sym_addr = (DWORD) m_sBuf + rel->r_addend;
				else
					sym_addr += rel->r_addend;
				*reloc_addr = (old_value & ~0x3FF) | (sym_addr & 0x3FF);

#ifdef DEBUG_ELF_LOADER
                reloctxt = "LO10";
#endif
				break;

			case R_SPARC_UA32:
				WriteUnalignedDWord (reloc_addr, sym_addr + rel->r_addend);
#ifdef DEBUG_ELF_LOADER
                reloctxt = "UA32";
#endif
				break;

			default:
				sprintf (m_sErrorText, "%s: Unsupported relocation type: %d", m_sModuleName, reloc_type);
				return false;
			}

#ifdef DEBUG_ELF_LOADER
			printf ("%-7s [%d]%s value %08X, addend %08X, sect_addr %08X, %08X: %08X -> %08X %s\n",
					reloctxt, 
                    reloc_sym,
					sym_name.c_str() + 1, 
					sym->st_value, 
					rel->r_addend, 
					(DWORD) m_Shdr_first[shdr->sh_info].sh_addr,
                    reloc_addr,
					old_value, 
					*reloc_addr,
					globalsym);
			globalsym = "";
#endif
        }
	}

	return true;
}
