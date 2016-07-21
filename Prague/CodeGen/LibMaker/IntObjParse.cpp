#include "stdafx.h"
#include "IntObjParse.h"

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
#pragma pack(push,pack_coff)
#define PACKSTRUCT
#else
#define PACKSTRUCT __attribute ((packed))
#endif
#pragma pack(1)

#define COFF_SF_COMMUNAL         0x00001000
#define COFF_SF_NOPAD            0x00000008
#define COFF_SF_REMOVE           0x00000800
#define COFF_SF_DISCARDABLE      0x02000000

#define COFF_SF_INFO             0x00000200
#define COFF_SF_INIT_DATA        0x00000040
#define COFF_SF_NONINIT_DATA     0x00000080
#define COFF_SF_CODE             0x00000020

#define COFF_SF_READ             0x40000000
#define COFF_SF_WRITE            0x80000000
#define COFF_SF_EXECUTE          0x20000000

#define COFF_SF_ALIGN_MASK       0x00F00000

typedef struct _COFF_Relocation
{
	DWORD   Offset;
	DWORD   SymbolIndex;
	WORD    Type;
} PACKSTRUCT COFF_Relocation;

typedef struct _COFF_Header
{
	WORD    Machine;
	WORD    N_Sections;
	DWORD   Timestamp;
	DWORD   P_SymbolTable;
	DWORD   N_Symbols;
	WORD    OptionHeaderSize;
	WORD    Characteristics;
} PACKSTRUCT COFF_Header;

typedef struct _COFF_SectionHeader
{
	union{
		char Name[8];
		struct{
			DWORD NameWord;
			DWORD NameOffset;
#ifdef __LINUX__
		}Cs;
#else
	};
#endif
};
DWORD   PhysicalAddress;
DWORD   VirtualAddress;
DWORD   RawDataSize;
DWORD   P_RawData;
DWORD   P_RelocationTable;
DWORD   P_LineNumbers;
WORD    N_Relocations;
WORD    N_LineNumbers;
DWORD   Flags;
} PACKSTRUCT COFF_SectionHeader;

int ObjParse(BYTE* body,DWORD body_len, OBJPARSE_CALLBACK callback_name, DWORD UserValue)
{
	int error=OBJPARSE_OK;
	COFF_Header* Header;
	IMAGE_SYMBOL* Symbol;
	COFF_SectionHeader* SHeader;
	DWORD i;
	DWORD* symbol_flags;
	
	Header=(COFF_Header*)body;
	if ( Header->Machine != 0x014c ) return OBJPARSE_BAD_COFF;	
	Symbol=(IMAGE_SYMBOL*)(body + Header->P_SymbolTable);
	SHeader=(COFF_SectionHeader*)(body+sizeof(COFF_Header) + Header->OptionHeaderSize);
	symbol_flags=new DWORD[Header->N_Symbols];
	if(symbol_flags==NULL) return OBJPARSE_NO_MEM;
	memset(symbol_flags,0,Header->N_Symbols*sizeof(DWORD));
	
	// process RELOCATIONS
	for(DWORD k = 0; k < Header->N_Sections; k++ )
	{
		COFF_Relocation* r=(COFF_Relocation*)(body + SHeader[k].P_RelocationTable);
		for(int j=0; j < SHeader[k].N_Relocations; j++)
		{
			i=r[j].SymbolIndex;

			if(Symbol[i].StorageClass != 0x02) continue;

			if(Symbol[i].SectionNumber==0) {
				char buf[9]; buf[8]=0;
				char* Name=buf;
				if(Symbol[i].N.Name.Short==0)
					Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].N.Name.Long;
				else memcpy(buf,Symbol[i].N.ShortName, 8);
				
				symbol_flags[i]=OBJPARSE_IMPORT;
			}
		}
	}
	
	for(i=0; i < Header->N_Symbols; i+=1+Symbol[i].NumberOfAuxSymbols ){
		if(Symbol[i].SectionNumber > Header->N_Sections) continue;
		if(Symbol[i].SectionNumber){
			if( Symbol[i].Type & 0x20 ){ // EXPORT Function
				symbol_flags[i]=OBJPARSE_EXPORT;
			}
			else{
				if(SHeader[Symbol[i].SectionNumber-1].Flags & (IMAGE_SCN_LNK_INFO|IMAGE_SCN_LNK_OTHER|IMAGE_SCN_LNK_REMOVE))
					continue;

				if(SHeader[Symbol[i].SectionNumber-1].Flags & COFF_SF_NONINIT_DATA)
					symbol_flags[i]=OBJPARSE_BSS;
				if(SHeader[Symbol[i].SectionNumber-1].Flags & COFF_SF_INIT_DATA)
					symbol_flags[i]=OBJPARSE_DATA;
			}
		}
		
		if(symbol_flags[i])
		{
			char buf[9]; buf[8]=0;
			char* Name=buf;
			if(Symbol[i].N.Name.Short==0)
				Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].N.Name.Long;
			else memcpy(buf,Symbol[i].N.ShortName,8);
			if ((Symbol[i].StorageClass == IMAGE_SYM_CLASS_STATIC) && (symbol_flags[i]) == OBJPARSE_EXPORT)
			{
				//if (lstrcmp(Name, _DEBUG_SEGMENT) == 0) continue;
				//OutputDebugString(Name);
				//OutputDebugString("\n");
			}
			else
			{
				callback_name(symbol_flags[i],Name,UserValue);
			}
		}
	}		
	delete symbol_flags;
	return error;
}
