////////////////////////////////////////////////////////////////////
//
//  CLINK.CPP
//  COFF Linker implementation.
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#ifdef __MWERKS__
	#include <string.h>
	#include <dupmem/dupmem.h>
	#include "avp_msg.h"
	#include <bases/retcode3.h>
	#include <byteorder.h>
#endif


#include <ScanAPI/RetFlags.h>


#if (defined(__WATCOMC__) && defined(__DOS__))
	#define NOAFX
	#include <os2.h>
	#include <string.h>
	#include "../typedef.h"
	#include "../retcode3.h"
	#include "../../common.fil/scanapi/avp_msg.h"
	#include <tchar.h>
	#define AFXAPI
	#include "../../common.fil/stuff/_carray.h"
#endif

#include <stdio.h>
#include <ScanAPI/Scanobj.h>
#include "Clink.h"
#include "Basework.h"
#include <_AVPIO.h>

#if defined (BASE_EMULATOR)
	#define SKIP_ENGINE_PARTS
	#include "emulator/emulator.h"
	#include "emulator/wrappers.h"
	#include <stdio.h>
	#undef SKIP_ENGINE_PARTS	
#else 
	#include "byteorder.h"
#endif /* defined BASE_EMULATOR */

//#define s_DEBUG


#if !defined (BASE_EMULATOR)

#if !defined (__unix__)
	#pragma intrinsic( memcmp )
#endif

class GName{
public:
	BOOL IsValid(){ return Name!=NULL;}
    int     Alloc;
    char* Name;
    DWORD Value;
    void* Link;
    GName(char* name, DWORD value, void* link, int alloc=0);
    ~GName();
};

GName::~GName(){
    if(Name) delete Name;
    if(Alloc && Value)
		DupFree((char*)Value);
}

GName::GName(char* name, DWORD value, void* link, int alloc)
{
	size_t NameSz = strlen(name)+1;
    Name=new char[NameSz];
    strcpy_s(Name,NameSz,name);
    Link=link;
    Alloc=alloc;
    if(alloc){
		Value=(DWORD)DupAlloc(alloc,0,0);
        if(value) *(DWORD*)Value=value;
    }
	else
	    Value=value;
}


void CGlobalNames::Remove(void* link)
{
	for(int b=0;b<16;b++)
	for(int i=NameArray[b].GetUpperBound();i>=0;i--){
		GName* gn=(GName*)NameArray[b][i];
		if(gn && gn->Link==link){
			NameArray[b].RemoveAt(i);
			delete (GName*)gn;
		}
	}
}

void CGlobalNames::FreeAll()
{
	for(int b=0;b<16;b++)
	{
		for(int i=0;i<NameArray[b].GetSize();i++)
		{
			GName* gn=(GName*)NameArray[b][i];
			if(gn)
				delete gn;
		}
		NameArray[b].RemoveAll();
	}
}

void CGlobalNames::Compact()
{
	for(int b=0;b<16;b++)
	for(int i=0;i<NameArray[b].GetSize();i++)
	{
		GName* gn=(GName*)NameArray[b][i];
		if(gn)
		{
			if(gn->Alloc)
			{
				if(gn->Name)
				{
					delete gn->Name;
					gn->Name=0;
				}
			}
			else
			{
				delete gn;
				NameArray[b][i]=0;
			}
		}
	}
}



DWORD CGlobalNames::GetName(char* name, BOOL remove)
{
	DWORD ret=0;
	int b=name[1]&0x0f;
	
	for(int i=0;i<NameArray[b].GetSize();i++){
		GName* gn=(GName*)NameArray[b][i];
		if(gn->Name && !strcmp(gn->Name,name)){
			ret=gn->Value;
			if(remove){
				NameArray[b].RemoveAt(i);
				delete gn;
			}
			break;
		}
	}
	return ret;
}

DWORD CGlobalNames::SetName(char* name, DWORD value, void* link, int alloc){
	
#ifdef s_DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"CGlobalNames::SetName");
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)name);
#endif
	
	if(GetName(name,TRUE))
		AvpCallback(AVP_CALLBACK_ERROR_SYMBOL_DUP,(DWORD)name);
	
	GName* gn=new GName(name,value,link,alloc);
	if(gn)
	{
		if(gn->IsValid()){
			int b=name[1]&0x0f;
			NameArray[b].Add(gn);
			return gn->Value;
		}
		delete gn;
		return 0;
	}
	AvpCallback(AVP_CALLBACK_ERROR_SYMBOL,(DWORD)name);
	return 0;
}
#endif /* !defined BASE_EMULATOR */


#if defined (BASE_EMULATOR)
int CLink::ExecuteEntry(CWorkArea * wa, int e, const char * part, const char * name)
#else
int CLink::ExecuteEntry(CWorkArea * wa, int e)
#endif /* defined BASE_EMULATOR */
{
	int ret = R_PREDETECT;
	AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);

#if defined (BASE_EMULATOR)
	char buf[256];
	sprintf (buf, "%s %s", part, name);
#endif

#if defined (__unix__)		
	if(wa->ScanObject->SO1.SType == OT_MEMORY_FILE){
#else
	if(wa->ScanObject->SType == OT_MEMORY_FILE){
#endif	
		ret=R_SKIP;
		*(DWORD*)(wa->Page_C)=0;
		goto ret;
	}
	
	switch(e)
	{
	case 0:
		ret=R_PREDETECT;
		if ( Entry ){
#if defined (BASE_EMULATOR)
			ret= gEmul.Execute (wa, (unsigned long) Entry, buf);
#else
			if(wa->ScanObject->MFlags & MF_LINK_FRAME){
				AvpCallback(AVP_CALLBACK_LINK_BEGIN,(LPARAM) this);
				ret= ((int (*)())Entry )();
				AvpCallback(AVP_CALLBACK_LINK_DONE,(LPARAM) this);
			}else{
				ret= ((int (*)())Entry )();
			}
			
#endif /* defined BASE_EMULATOR */
		}
		break;
		
	case 1:
		ret=R_PRECURE;
		if ( EntryC ){
#if defined (BASE_EMULATOR)
			ret= gEmul.Execute (wa, (unsigned long) EntryC, buf);
#else
			ret= ((int (*)())EntryC)();
#endif /* defined BASE_EMULATOR */
		}
		break;
	}

// Should be handled on OBJECT_DONE
//	if(wa->RFlags & RF_CORRUPT)
//		AvpCallback(AVP_CALLBACK_WA_CORRUPT,(DWORD)wa);

ret:	
	return ret;
}

CLink::~CLink()
{
    FreeAll();
}


void CLink::FreeAll()
{
    Entry=0;
    EntryC=0;
#ifdef LINK_RECORD_SAVE				
	if(Image) delete Image;
	Image=0;
	ImageSize=0;
#endif
}

CLink::CLink()
{
    Entry=0;
    EntryC=0;
#ifdef LINK_RECORD_SAVE				
	Image=0;
	ImageSize=0;
#endif
}

BOOL CLink::Link(BYTE* body, CBaseWork* bw )
{
	DWORD i;
	COFF_Header* Header;
	COFF_Symbol* Symbol;
	COFF_SectionHeader* SHeader;
	char link_source_name[MAX_SYMBOL_NAME];
	int ready=0;
	DWORD*  sections=NULL;
	
	Header=(COFF_Header*)body;
	if ( ReadWordPtr(&Header->Machine) != 0x014c )     goto ret;
	
	Symbol=(COFF_Symbol*)(body + ReadDWordPtr(&Header->P_SymbolTable));
	SHeader=(COFF_SectionHeader*)(body+sizeof(COFF_Header) + ReadWordPtr(&Header->OptionHeaderSize));
	
	sections=new DWORD[ReadWordPtr(&Header->N_Sections)];

	for(i=0;i < ReadWordPtr(&Header->N_Sections); i++){
		char* sec=NULL;

		if(     memcmp(SHeader[i].Name,".drect",6)
			&&      memcmp(SHeader[i].Name,".debug",6))
		{
			AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);

			if (!memcmp(SHeader[i].Name,".rdata",6) ||
			   !!((SHeader[i].Flags) & (COFF_SF_COMMUNAL|COFF_SF_CODE)))
			{

#if defined (BASE_EMULATOR)
				sec = (char *) gEmul.AllocRegion (ReadDWordPtr(&SHeader[i].RawDataSize) + (ReadDWordPtr(&SHeader[i].RawDataSize)==0));
				if(sec==NULL)   goto ret;
#else
				DWORD len=SHeader[i].RawDataSize + (SHeader[i].RawDataSize==0);
				sec = (char*) bw->m_pPageMemoryManager->allocate(len);
				if(sec==NULL)
					goto ret;
#endif // defined BASE_EMULATOR 

			}
			else
			{
				BOOL alloc_by_new;
				sec=(char*)DupAlloc(ReadDWordPtr(&SHeader[i].RawDataSize) + (ReadDWordPtr(&SHeader[i].RawDataSize)==0),0,&alloc_by_new);
				if(sec==NULL)   
					goto ret;
				if(alloc_by_new)
					bw->Allocations.Add(sec);
			}

			if ( ReadDWordPtr(&SHeader[i].P_RawData) )
				memcpy(sec, body + ReadDWordPtr(&SHeader[i].P_RawData), ReadDWordPtr(&SHeader[i].RawDataSize));
			else
				memset(sec, '0', ReadDWordPtr(&SHeader[i].RawDataSize));
		}
		else{
			WriteWordPtr (&SHeader[i].N_Relocations, (WORD) 0);
		}

		sections[i]=(DWORD)sec;
	}
	
	// Fix SYMBOLS
	for(i=0; i < ReadDWordPtr(&Header->N_Symbols);i+=Symbol[i].Skip, i++){
		char buf[9];
		buf[8]=0;
		char* Name=buf;

		//NOVELL sertify
		if(i%20==0)
			AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);

		if(Symbol[i].Type == 0x67){
			strncpy_s(link_source_name,sizeof(link_source_name),Symbol[i+1].Name,sizeof(link_source_name)-1);
			*(DWORD*)(link_source_name + strlen(link_source_name) + 1)=(DWORD)this;
			AvpCallback(AVP_CALLBACK_LINK_NAME,(DWORD)link_source_name);
		}
		
		if(Symbol[i].Type & 0x60) goto skip;
		if ( ReadWordPtr(&Symbol[i].Section) > ReadWordPtr(&Header->N_Sections)) goto skip;
		
#ifdef __unix__
		if (ReadDWordPtr(&Symbol[i].Cs.NameWord) == 0 )
			Name=(char*)(Symbol+ReadDWordPtr(&Header->N_Symbols))+ReadDWordPtr(&Symbol[i].Cs.NameOffset);
#else
		if(Symbol[i].NameWord==0)
            Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
#endif
		else memcpy(buf,Symbol[i].Name,8);
		

		// PUBLIC symbols
		if ( ReadWordPtr(&Symbol[i].Section) )
		{
			WriteDWordPtr (&Symbol[i].Value, ReadDWordPtr(&Symbol[i].Value) + (DWORD)sections[ReadWordPtr(&Symbol[i].Section)-1]);

			if ( ReadDWordPtr(&Symbol[i].Value) && *Name=='_' )
			{
				if (!strcmp(Name,"_cure") )
					EntryC = ReadDWordPtr(&Symbol[i].Value);
				else if(!strcmp(Name,"_decode"))
					Entry = ReadDWordPtr(&Symbol[i].Value);
				else if(!strcmp(Name,"_jmp"))
					Entry = ReadDWordPtr(&Symbol[i].Value);
				else
#if defined (BASE_EMULATOR)
					gEmul.pTable.AddSymbol (Name, ReadDWordPtr(&Symbol[i].Value), SYM_CPU_X86);
#else
					bw->GlobalNames.SetName(Name,Symbol[i].Value,this);
#endif // defined BASE_EMULATOR 
			}
#if defined (BASE_EMULATOR)
			else
				gEmul.pTable.AddSymbol (Name, ReadDWordPtr(&Symbol[i].Value), SYM_CPU_X86);
#endif // defined BASE_EMULATOR 
		}
		// EXTERNAL symbols (UNDEF section)
		else
		{
			// External Function
			if( ReadWordPtr(&Symbol[i].Access) & 0x20 )
			{
#if defined (BASE_EMULATOR)
				unsigned long addr = gEmul.pTable.SymOrWrapperLookup(Name, SYM_CPU_X86);

				if ( !addr )
 				{
					AvpCallback(AVP_CALLBACK_ERROR_SYMBOL,(DWORD)Name);
					goto ret;
				}

				WriteDWordPtr (&Symbol[i].Value, addr);
#else
				Symbol[i].Value = bw->GlobalNames.GetName(Name);
#endif // defined BASE_EMULATOR 
			}
			//PUBLIC uninitialized data ?
			else if ( ReadDWordPtr(&Symbol[i].Value) )
			{
#if defined (BASE_EMULATOR)
				DWORD addr = (DWORD) gEmul.AllocRegion(ReadDWordPtr(&Symbol[i].Value));
				WriteDWordPtr (&Symbol[i].Value, gEmul.pTable.AddSymbol(Name, addr, SYM_CPU_X86));
#else
				Symbol[i].Value = bw->GlobalNames.SetName(Name,0,this,Symbol[i].Value);
#endif // defined BASE_EMULATOR 
			}
			//External data
			else
			{
#if defined (BASE_EMULATOR)
				unsigned long addr = gEmul.pTable.SymOrWrapperLookup(Name, SYM_CPU_X86);

				if ( !addr )
				{
					AvpCallback(AVP_CALLBACK_ERROR_SYMBOL,(DWORD)Name);
					goto ret;
				}
                WriteDWordPtr (&Symbol[i].Value, addr);
#else
				Symbol[i].Value = bw->GlobalNames.GetName(Name);
#endif // defined BASE_EMULATOR 
			}
			
#if !defined (BASE_EMULATOR)
			if(Symbol[i].Value==0)
			{
				AvpCallback(AVP_CALLBACK_ERROR_SYMBOL,(DWORD)Name);
				goto ret;
			}
#endif // !defined BASE_EMULATOR 
		}
skip:
		;
	}
	
	// Fix RELOCATIONS
	for( i = 0; i < ReadWordPtr (&Header->N_Sections); i++ )
	{
		AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);
		COFF_Relocation* r=(COFF_Relocation*)(body + ReadDWordPtr(&SHeader[i].P_RelocationTable));
		char* raw=(char*)sections[i];

		if ( !raw )
			continue;

		for(int j=0; j < ReadWordPtr (&SHeader[i].N_Relocations); j++)
		{
			COFF_Symbol* s = Symbol + ReadDWordPtr(&r[j].SymbolIndex);
			DWORD* p = (DWORD*) ( raw + ReadDWordPtr(&r[j].Offset) );

			//NOVELL sertify
			if(j%20==0)
				AvpCallback(AVP_CALLBACK_ROTATE,(LPARAM)NULL);

			switch ( ReadWordPtr(&r[j].Type) )
			{
			case 0x0006:    //DIR32
			case 0x0007:    //DIR32NB
				WriteDWordPtr (p, ReadDWordPtr(p) + ReadDWordPtr(&s->Value));
				break;

			case 0x0014:    //REL32
				WriteDWordPtr (p, ReadDWordPtr(p) + (long) (ReadDWordPtr(&s->Value)) - 4 - (long) p);
				break;

			default:
				{
					char* n=s->Name;
#ifdef __unix__
					if ( ReadDWordPtr(&s->Cs.NameWord) == 0)
						n=(char*)(Symbol+ReadDWordPtr(&Header->N_Symbols))+ReadDWordPtr(&s->Cs.NameOffset);
#else
					if ( ReadDWordPtr(&s->NameWord) == 0)
						n=(char*)(Symbol+Header->N_Symbols)+s->NameOffset;
#endif
					AvpCallback(AVP_CALLBACK_ERROR_FIXUPP,(DWORD)n);
				}
				break;
			}
		}
	}
	ready=1;
ret:
	if(!ready){
		FreeAll();
	}
	if(sections)
		delete sections;
	AvpCallback(AVP_CALLBACK_ROTATE_OFF,(LPARAM)NULL);
	return ready;
}


#ifdef LINK_RECORD_SAVE				

BOOL CLink::SaveImage(void* buf, DWORD size)
{
	if(size){
		Image=new char[size];
		if(Image){
			memcpy(Image,buf,size);
			ImageSize=size;
			return 1;
		}
	}
	return 0;
}

BOOL CLink::DeleteImage()
{
	if (Image){
		delete Image;
		Image=0;
		ImageSize=0;
	}
	return 1;
}
#endif
