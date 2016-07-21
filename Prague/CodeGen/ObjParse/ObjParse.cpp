// ObjParse.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ObjParse.h"
#include <FileFormat/coff.h>

OBJPARSE_API int ObjParse(BYTE* body,DWORD body_len, OBJPARSE_CALLBACK callback_name, DWORD UserValue)
{
	int error=OBJPARSE_OK;
	COFF_Header* Header;
	COFF_Symbol* Symbol;
	COFF_SectionHeader* SHeader;
	DWORD i;
	DWORD* symbol_flags;
	
	Header=(COFF_Header*)body;
	if ( Header->Machine != 0x014c ) return OBJPARSE_BAD_COFF;	
	Symbol=(COFF_Symbol*)(body + Header->P_SymbolTable);
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

			if(Symbol[i].Type != 0x02) continue;

			if(Symbol[i].Section==0) {
				char buf[9]; buf[8]=0;
				char* Name=buf;
				if(Symbol[i].NameWord==0)
					Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
				else memcpy(buf,Symbol[i].Name,8);
				
				symbol_flags[i]=OBJPARSE_IMPORT;
			}
		}
	}
	
	for(i=0; i < Header->N_Symbols; i+=1+Symbol[i].Skip ){
		if(Symbol[i].Section > Header->N_Sections) continue;
		if(Symbol[i].Section){
			if( Symbol[i].Access & 0x20 ){ // EXPORT Function
				symbol_flags[i]=OBJPARSE_EXPORT;
			}
			else{
				if(SHeader[Symbol[i].Section-1].Flags & (IMAGE_SCN_LNK_INFO|IMAGE_SCN_LNK_OTHER|IMAGE_SCN_LNK_REMOVE))
					continue;

				if(SHeader[Symbol[i].Section-1].Flags & COFF_SF_NONINIT_DATA)
					symbol_flags[i]=OBJPARSE_BSS;
				if(SHeader[Symbol[i].Section-1].Flags & COFF_SF_INIT_DATA)
					symbol_flags[i]=OBJPARSE_DATA;
			}
		}
		
		if(symbol_flags[i])
		{
			char buf[9]; buf[8]=0;
			char* Name=buf;
			if(Symbol[i].NameWord==0)
				Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
			else memcpy(buf,Symbol[i].Name,8);
			callback_name(symbol_flags[i],Name,UserValue);
			
		}
	}		
	delete symbol_flags;
	return error;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

 