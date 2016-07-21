// ENTRY.C ELF parsing routine header. Written by Valik Kolesnikov

typedef unsigned long	Elf32_Addr;
typedef unsigned short	Elf32_Half;
typedef unsigned long	Elf32_Off;
typedef long			Elf32_Sword;
typedef unsigned long	Elf32_Word;

#define ET_EXEC   2
#define ET_DYN    3

#define EM_386   3
#define EM_486   6   /* Perhaps disused */

#define EV_CURRENT	1

#define EI_NIDENT	16

typedef struct elf32_hdr{
  unsigned char	e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;  /* Entry point */
  Elf32_Off	e_phoff;
  Elf32_Off	e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct elf32_phdr{
  Elf32_Word	p_type;			// 2
  Elf32_Off	p_offset;           // 4
  Elf32_Addr	p_vaddr;        // 4
  Elf32_Addr	p_paddr;        // 4
  Elf32_Word	p_filesz;       // 2
  Elf32_Word	p_memsz;        // 2
  Elf32_Word	p_flags;        // 2
  Elf32_Word	p_align;        // 2 - total 22
} Elf32_Phdr;

#define	SIZEOF_ELF32_PHDR	22

