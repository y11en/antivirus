/*
 *  elf-format.h,v 1.2 2002/10/17 09:55:01 tim 
 */

#ifndef HDR_ELFFORMAT_H
#define HDR_ELFFORMAT_H

#define EI_NIDENT	16	// Size of e_ident
#define EI_TYPE		1	// e_type RELOCATABLE
#define EI_MACHINE	2	// e_machine SPARC
#define EI_CLASS32	1	// EI_CLASS 32
#define ELFDATA2LSB	1	// little endian
#define ELFDATA2MSB	2	// big endian

#define EI_MAG0		e_ident[0]
#define EI_MAG1		e_ident[1]
#define EI_MAG2		e_ident[2]
#define EI_MAG3		e_ident[3]
#define EI_CLASS	e_ident[4]
#define EI_DATA		e_ident[5]


#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

#pragma pack(1)
typedef struct
{
	BYTE	e_ident[EI_NIDENT];		// ELF "magic number"
	WORD	e_type;					// object file type
	WORD	e_machine;				// required architecture
	DWORD	e_version;				// object file version
	DWORD	e_entry;				// Entry point virtual address
	DWORD	e_phoff;				// Program header table file offset
	DWORD	e_shoff;				// Section header table file offset
	DWORD	e_flags;				// Processor-specific flags
	WORD	e_ehsize;				// ELF header size in bytes
	WORD	e_phentsize;			// Program header table entry size
	WORD	e_phnum;				// Program header table entry count
	WORD	e_shentsize;			// Section header table entry size
	WORD	e_shnum;				// Section header table entry count
	WORD	e_shstrndx;				// Section header string table index
} Elf32_Ehdr;


typedef struct
{
	DWORD	p_type;					// Program segment type
	DWORD	p_offset;				// Segment file offset
	DWORD	p_vaddr;				// Segment virtual address
	DWORD	p_paddr;				// Segment physical address (ignored on SystemV)
	DWORD	p_filesz;				// Segment size in file
	DWORD	p_memsz;				// Segment size in memory
	DWORD	p_flags;				// Segment flags
	DWORD	p_align;				// Segment alignment, file & memory
} Elf32_phdr;

/* section name */
#define SHN_UNDEF		0

/* for sh_type */
#define SHT_NULL		0
#define SHT_PROGBITS	1
#define SHT_SYMTAB		2
#define SHT_STRTAB		3
#define SHT_RELA		4
#define SHT_HASH		5
#define SHT_DYNAMIC		6
#define SHT_NOTE		7
#define SHT_NOBITS		8
#define SHT_REL			9
#define SHT_SHLIB		10
#define SHT_DYNSYM		11

/* for sh_flags */
#define SHF_WRITE		1
#define SHF_ALLOC		2
#define SHF_EXECINSTR	4


typedef struct
{
	DWORD	sh_name;				//	Section name, index in string tbl
	DWORD	sh_type;				//	Type of section
	DWORD	sh_flags;				//	Miscellaneous section attributes
	DWORD	sh_addr;				//	Section virtual addr at execution
	DWORD	sh_offset;				//	Section file offset
	DWORD	sh_size;				//	Size of section in bytes
	DWORD	sh_link;				//	Index of another section
	DWORD	sh_info;				//	Additional section information
	DWORD	sh_addralign;			//	Section alignment
	DWORD	sh_entsize;				//	Entry size if section holds table
} Elf32_Shdr;


#define ELF32_ST_BIND(i)		((i) >> 4)
#define ELF32_ST_TYPE(i)		((i) & 0xf)
#define ELF32_ST_INFO(b,t)		(((b) << 4) + ((t) & 0xf))


// st_shndx uses these values - section index
#define SHN_UNDEF		0			//  Undefined section reference
#define SHN_ABS			0xFFF1U		//  Associated symbol is absolute
#define SHN_COMMON		0xFFF2U		//  Associated symbol is in common


// values returned by ELF32_ST_TYPE
#define STT_NOTYPE	0				//	Symbol type is unspecified
#define STT_OBJECT	1				//	Symbol is a data object
#define STT_FUNC	2				//	Symbol is a code object
#define STT_SECTION	3				//	Symbol associated with a section
#define STT_FILE	4				//	Symbol gives a file name
#define	STT_NUM		5				//	Number of defined types

#define STB_LOCAL	0				//	Symbol not visible outside obj
#define STB_GLOBAL	1				//	Symbol visible outside obj
#define STB_WEAK	2				//	Like globals, lower precedence
#define	STB_NUM		3				//	Number of defined types.


typedef struct
{
	DWORD	st_name;				//	Symbol name index in string table
	DWORD	st_value;				//	Value of the symbol
	DWORD	st_size;				//	Associated symbol size
	BYTE	st_info;				//	Type and binding attributes
	BYTE	st_other;				//	No defined meaning, 0
	WORD	st_shndx;				//	Associated section index
} Elf32_Sym;


#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((BYTE)(i))
#define ELF32_R_INFO(s,t)	(((s) << 8) + ((BYTE)(t)))


typedef struct
{
	DWORD	r_offset;				//	Location at which to apply the action
	DWORD	r_info;					//	index and type of relocation
} Elf32_Rel;


// SUN SPARC specific definitions.
// SPARC relocs

#define R_SPARC_NONE		0		//	No reloc
#define R_SPARC_8			1		//	Direct 8 bit
#define R_SPARC_16			2		//	Direct 16 bit
#define R_SPARC_32			3		//	Direct 32 bit
#define R_SPARC_DISP8		4		//	PC relative 8 bit
#define R_SPARC_DISP16		5		//	PC relative 16 bit
#define R_SPARC_DISP32		6		//	PC relative 32 bit
#define R_SPARC_WDISP30		7		//	PC relative 30 bit shifted
#define R_SPARC_WDISP22		8		//	PC relative 22 bit shifted
#define R_SPARC_HI22		9		//	High 22 bit
#define R_SPARC_22			10		//	Direct 22 bit
#define R_SPARC_13			11		//	Direct 13 bit
#define R_SPARC_LO10		12		//	Truncated 10 bit
#define R_SPARC_GOT10		13		//	Truncated 10 bit GOT entry
#define R_SPARC_GOT13		14		//	13 bit GOT entry
#define R_SPARC_GOT22		15		//	22 bit GOT entry shifted
#define R_SPARC_PC10		16		//	PC relative 10 bit truncated
#define R_SPARC_PC22		17		//	PC relative 22 bit shifted
#define R_SPARC_WPLT30		18		//	30 bit PC relative PLT address
#define R_SPARC_COPY		19		//	Copy symbol at runtime
#define R_SPARC_GLOB_DAT	20		//	Create GOT entry
#define R_SPARC_JMP_SLOT	21		//	Create PLT entry
#define R_SPARC_RELATIVE	22		//	Adjust by program base
#define R_SPARC_UA32		23		//	Direct 32 bit unaligned
#define R_SPARC_NUM			24


typedef struct
{
	DWORD	r_offset;				//	Location at which to apply the action
	DWORD	r_info;					//	index and type of relocation
	DWORD	r_addend;				//	Constant addend used to compute value
} Elf32_Rela;

#pragma pack()

#endif // HDR_ELFFORMAT_H
