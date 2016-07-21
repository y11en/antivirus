/*
 *  elfbases.cpp,v 1.2 2002/10/17 09:55:01 tim 
 */

#include "typedefs.h" 

//#define DEBUG_ELF_LOADER
//#define ELF_OBJ_DEBUG_SUPPORT
#define SIGNED_ELF_BASES


#if defined (SIGNED_ELF_BASES)
	#include "sign/sign.h"
	extern void** vndArray;
	extern int vndArrayCount;
#endif

#include "elfbasesunpack.cpp"


const char CURRENT_MACHINE = ELF_BASE_MACHINE_SPARC7;
const char CURRENT_BYTEORDER = ELF_BASE_BYTEORDER_BE;


ElfBaseLoader::ElfBaseLoader()
{
}


ElfBaseLoader::~ElfBaseLoader()
{
}


const char * ElfBaseLoader::GetLastError()
{
	return m_sErrorMsg;
}


bool ElfBaseLoader::LoadElfBases (SymbolTable * pTable)
{
	char buf[PATH_MAX], line[PATH_MAX], * p, current_dir[PATH_MAX];
	FILE * fp;

	getcwd(current_dir, sizeof(current_dir));
	sprintf (buf, "%s/elf.set", current_dir);

	if ( (fp = fopen(buf, "rb")) == 0 )
	{
		printf ("Error opening elf.set: %s\n", strerror(errno));
		return true;
	}

	while ( fgets(line, sizeof(line), fp) != 0 )
	{
		// Stop after first space character
		for (p = line; *p && *p > ' '; p++)
			;

		*p++ = 0;

		if ( line[0] == 0 || line[0] == ';' || line[0] == '#' )
			continue; // an empty line or a comment

#if defined (ELF_OBJ_DEBUG_SUPPORT)
		if ( !strncmp (line, "ELF_", 4) )
		{
			for ( char * s = p; *s; s++ )
				if ( *s == '\r' || *s == '\n' )
					*s = '\0';

			while (isspace(*p))
				p++;

			if ( *p == '\0' )
			{
				printf ("Loading ELF obj file: %s\n", line + 4);
				p = 0;
			}
			else
				printf ("Loading ELF obj file: %s (link %s)\n", line + 4, p);

			sprintf (buf, "%s/%s", current_dir, line + 4);
			ELFparser * elf = new ELFparser (line+4, p);

			if ( !elf->ParseFile(buf, pTable) )
			{
				sprintf (m_sErrorMsg, "Couldn't read ELF file %s: %s", 
					line + 4, elf->GetError());
			
				fclose (fp);
				return false;
			}

            m_ElfModules.push_back(elf);
			continue;
		}
#endif

		printf ("Loading base %s\n", line);
		sprintf (buf, "%s/%s", current_dir, line);

#if defined (SIGNED_ELF_BASES)
		switch (sign_check_file(buf, 1, vndArray, vndArrayCount, 0) )
		{
			case SIGN_OK:
				break;
			
			case SIGN_FILE_NOT_FOUND:
			case SIGN_CANT_READ:
				sprintf (m_sErrorMsg, "Can't open file %s.\n", buf);
				fclose (fp);
				return false;

			default:
				sprintf (m_sErrorMsg, "Base %s: integrity check failed.\n", buf);
				fclose (fp);
				return false;
		}
#endif

		if ( !LoadBase (buf, pTable) )
		{
			fclose (fp);
			return false;
		}
	}

	fclose (fp);
    return true;
}


bool ElfBaseLoader::LinkElfBases (SymbolTable * pTable)
{
	for ( unsigned int i = 0; i < m_ElfModules.size(); i++ )
	{
#if defined (DEBUG_ELF_LOADER)
		printf ("Linking ELF module %s    \n", m_ElfModules[i]->GetModuleName());
#endif

		if ( !m_ElfModules[i]->Link (pTable) )
		{
			sprintf (m_sErrorMsg, "Error linking module %s: %s\n", m_ElfModules[i]->GetModuleName(), m_ElfModules[i]->GetError());
			return false;
		}
	}

	for ( unsigned int j = 0; j < m_ElfModules.size(); j++ )
	{
		delete m_ElfModules[j];
	}

    m_ElfModules.clear();
    return true;
}


bool ElfBaseLoader::LoadBase (const char * filename, SymbolTable * pTable)
{
	FILE * fp;
	Elf_base_header_t basehdr;
    int base_day, base_month, base_year, base_hour, base_min, base_sec;
    char linkname[256];
    unsigned char linkname_length;

	if ( (fp = fopen(filename, "rb")) == 0 )
	{
		sprintf (m_sErrorMsg, "Couldn't open file %s: %s", filename, strerror(errno));
		return false;
	}
		
	if ( fread (&basehdr, 1, sizeof(basehdr), fp) != sizeof(basehdr) )
	{
		sprintf (m_sErrorMsg, "Couldn't read base header: %s", strerror(errno));
		fclose (fp);
		return false;
	}

	/* Check the header magic */
	if ( memcmp(ELF_BASE_ID, basehdr.magic, ELF_BASE_ID_LENGTH)  )
	{
		sprintf (m_sErrorMsg, "Corrupted base header");
		fclose (fp);
		return false;
	}

	if ( basehdr.version > ELF_BASE_CURRENT_VERSION )
	{
		sprintf (m_sErrorMsg, "Incorrect base version, please update your executable.");
		fclose (fp);
		return false;
	}

	if ( basehdr.version < ELF_BASE_CURRENT_VERSION )
	{
		sprintf (m_sErrorMsg, "Unsupported base version, please update your bases.");
		fclose (fp);
		return false;
	}

	/* Check target machine and byte order */
	if ( basehdr.target_machine != ELF_BASE_MACHINE_SPARC7
	||  basehdr.byteorder != ELF_BASE_BYTEORDER_BE
	|| sscanf(basehdr.datatime, "%02d%02d%02d%02d%02d%04d", 
		&base_hour, &base_min, &base_sec, &base_day, &base_month, &base_year ) != 6 )
	{
		sprintf (m_sErrorMsg, "Wrong base");
		fclose (fp);
		return false;
	}

#if defined (DEBUG_ELF_LOADER)
	printf ("Base created: %02d:%02d:%02d %02d-%02d-%04d\n", 
		base_hour, base_min, base_sec, base_day, base_month, base_year);
#endif

	if ( fseek (fp, basehdr.first_section_offset, SEEK_SET) )
	{
		sprintf (m_sErrorMsg, "Couldn't seek to sections");
		fclose (fp);
		return false;
	}

#if defined (DEBUG_ELF_LOADER)
	printf ("Sections: %d\n", basehdr.number_of_sections);
#endif

	for ( DWORD i = 0; i < basehdr.number_of_sections; i++ )
	{
		Elf_base_section_t secthdr;
		long position;

		if ( fread (&secthdr, 1, sizeof(secthdr), fp) != sizeof(secthdr) )
		{
			sprintf (m_sErrorMsg, "Couldn't read section: %s", strerror(errno));
			fclose (fp);
			return false;
		}

		if ( secthdr.type != ELF_BASE_SECTION_TYPE_ELF_OBJ 
		&&   secthdr.type != ELF_BASE_SECTION_TYPE_ELF_LINK )
		{
#if defined (DEBUG_ELF_LOADER)
			printf ("Unsupported section type: %d\n", secthdr.type);
#endif
			continue;
		}

		position = ftell(fp) + secthdr.section_length - sizeof(Elf_base_section_t);
        linkname_length = 0;

		// Seek to section beginning
		if ( fseek (fp, secthdr.data_offset, SEEK_SET) )
		{        
			sprintf (m_sErrorMsg, "Couldn't seek to section %s: %s", secthdr.name, strerror(errno));
			fclose (fp);
			return false;
		}

		if ( secthdr.type == ELF_BASE_SECTION_TYPE_ELF_LINK
		&& ( fread (&linkname_length, 1, 1, fp) != 1 
			|| fread (linkname, 1, linkname_length, fp) != linkname_length ) )
		{
			sprintf (m_sErrorMsg, "Couldn't read the name of section %s: %s", secthdr.name, strerror(errno));
			fclose (fp);
			return false;
		}

		linkname[linkname_length] = '\0';

#if defined (DEBUG_ELF_LOADER)
		printf ("Loading section %s (%s) (offset %08X, length %08X)\n", 
			secthdr.name,
			linkname_length ? linkname : "", 
			secthdr.data_offset, 
			secthdr.data_length);
#endif

        char * basebuf = new char [secthdr.data_length];	// will be freed by ELFparser
		if ( fread (basebuf, 1, secthdr.data_length, fp) != secthdr.data_length )
		{
			sprintf (m_sErrorMsg, "Couldn't read section %s: %s", secthdr.name, strerror(errno));
			fclose (fp);
			return false;
		}

		if ( secthdr.flags & ELF_BASE_SECTION_ENCRYPTED )
		{
			for ( DWORD jj = 0; jj < secthdr.data_length; jj++ )
				basebuf[jj] ^= (char) jj;
		}

		if ( secthdr.flags & ELF_BASE_SECTION_PACKED )
		{
			DWORD uncompressed_size = *((DWORD*) basebuf);
            char * uncbuf = new char [uncompressed_size + 1];	// will be freed by ELFparser

			if ( unpack_unsqu((BYTE*) basebuf + 4, (BYTE*) uncbuf) != uncompressed_size )
			{
				sprintf (m_sErrorMsg, "Couldn't unpack section %s: integrity error", secthdr.name);
				fclose (fp);
				return false;
			}

			delete basebuf;
            basebuf = uncbuf;
		}

        ELFparser * elf = new ELFparser (secthdr.name, linkname_length ? linkname : 0);

		if ( fseek (fp, position, SEEK_SET)
		|| !elf->ParseBuffer (basebuf, pTable) )
		{
			sprintf (m_sErrorMsg, "Couldn't read section %s: %s", secthdr.name, elf->GetError());
			fclose (fp);
			return false;
		}

        m_ElfModules.push_back(elf);
	}

#if defined (DEBUG_ELF_LOADER)
	printf ("Base is loaded\n");
#endif

	fclose (fp);
	return true;
}
