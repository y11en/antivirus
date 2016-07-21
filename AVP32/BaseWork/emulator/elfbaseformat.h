/*
 *  elfbaseformat.h,v 1.2 2002/10/17 09:55:01 tim 
 */

#ifndef HDR_ELFBASEFORMAT_H
#define HDR_ELFBASEFORMAT_H

/*
 * The main goals to create this format are:
 * - byte order portability. File signature and machine type must be accessible
 *   on any byte-order machine, rest of file must be in machine-specific byte 
 *   order
 * - structure member alignment. All DWORDs must be aligned at least at 4 bytes
 *   boundary, all WORDs - at least 2.
 */

#define ELF_BASE_ID_LENGTH	8
#define ELF_BASE_ID			"KAV_ELF~"

#define ELF_BASE_CURRENT_VERSION	3	/* version */	

#define ELF_BASE_MACHINE_NONE	0		/* error value */	
#define ELF_BASE_MACHINE_IX86	1		/* Intel x86 */	
#define ELF_BASE_MACHINE_SPARC7	2		/* Sparc v7	 */	

#define ELF_BASE_BYTEORDER_LE	1		/* Little-endian */
#define ELF_BASE_BYTEORDER_BE	2		/* Big-endian */


#pragma pack(1)

typedef struct
{
	char 	magic[ELF_BASE_ID_LENGTH];	/* Should be KAV_ELF\x00 */
	char	target_machine;				/* Machine which assembler is used in ELF bases */
	char	byteorder;					/* Byte order used in all the base structures */
	char	datatime[14];				/* Base creation date/time HHMMSSDDMMYYYY */
	DWORD   version;					/* ? */
	DWORD   creator_id;					/* ? */
	DWORD   flags;						/* ? */
	DWORD   number_of_sections;
	DWORD   first_section_offset;
} Elf_base_header_t;


#define ELF_BASE_SECTION_TYPE_ELF_OBJ	0
#define ELF_BASE_SECTION_TYPE_ELF_LINK	1
#define ELF_BASE_SECTION_NAME_SIZE		16

#define ELF_BASE_SECTION_PACKED			0x0001	/* Section is packed. First DWORD is uncompressed size */
#define ELF_BASE_SECTION_ENCRYPTED		0x0002	/* Section is encrypted by XORing with current offset */


typedef struct
{
	char	name[ELF_BASE_SECTION_NAME_SIZE];	/* 0-terminated section name */
	DWORD	type;
	DWORD	flags;
	DWORD	data_offset;
	DWORD	data_length;
	DWORD	section_length;
} Elf_base_section_t;

#pragma pack()

/* 
 * Symbols with this name are treated as links. They should use section type
 * ELF_BASE_SECTION_TYPE_ELF_LINK and they will be exported into symbol table
 * as a link name, not function name
 */
#define SYMBOL_DECODE_ENTRY_NAME		"decode"


/* 
 * Symbols starting with this prefix are full Intel-compatible wrappers.
 * Format: SYMBOL_WRAPPER_PREFIXargnum_name
 * i.e. wrapped strcmp(char*, int) should have "ELF_EXPORT_2_strcmp" name
 * Restrictions: function must have fixed argument number, arguments must be
 * 32bit (no QWORDs allowed)
 */

#define SYMBOL_WRAPPER_PREFIX			"ELF_EXPORT_"


#endif /* HDR_ELFBASEFORMAT_H */
