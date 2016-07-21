#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>
#include <prague.h>
#include <get_opt.h>
//#include <stdio.h>
#include <imagehlp.h>

#pragma hdrstop
#include "..\PrFormat.h"


int pr_call output( char* stream, int length, const char* format, va_list argptr );
int pr_call pr_vsprintf( char* stream, int length, const char* format, va_list argptr );

char title[] = "Prague module converter. Ver 1.0";
char msg_buff[0x4000];
int  count = 0;
BOOL console = FALSE;
BOOL check = FALSE;
BOOL brief = FALSE;
BOOL pause = FALSE;



int    ConvertFile( LPSTR szSrcFileName, LPSTR szDestFileName);
int    RealConvert( LOADED_IMAGE* image, HANDLE hDestFile, LPBYTE lpMetaData, DWORD dwMetaDataSize );
int    ConvertSectionTable( PrFileHeader* lpHeader, LOADED_IMAGE* image, HANDLE hFile, LPBYTE lpMetaData, DWORD dwMetaDataSize );
USHORT GetSectionId( LOADED_IMAGE* image, IMAGE_SECTION_HEADER* section );
UINT   CountImportsByDirID( LOADED_IMAGE* image, USHORT id, CHAR* table_name );
BOOL   HasAnyImports( LOADED_IMAGE* image );


// ---
USHORT g_skiped_sections[] = {
	IMAGE_DIRECTORY_ENTRY_RESOURCE,
	IMAGE_DIRECTORY_ENTRY_IMPORT,
	IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT,
};


// ---
typedef struct tag_Dir {
	USHORT id;
	CHAR*  name;
} tDir;

tDir g_dir_names[] = {
	{ IMAGE_DIRECTORY_ENTRY_EXPORT        ,  ".edata" }, // Export Directory
	{ IMAGE_DIRECTORY_ENTRY_IMPORT        ,  ".idata" }, // Import Directory
	{ IMAGE_DIRECTORY_ENTRY_RESOURCE      ,  ".rsrc"  }, // Resource Directory
	{ IMAGE_DIRECTORY_ENTRY_EXCEPTION     ,  ""       }, // Exception Directory
	{ IMAGE_DIRECTORY_ENTRY_SECURITY      ,  ""       }, // Security Directory
	{ IMAGE_DIRECTORY_ENTRY_BASERELOC     ,  ".reloc" }, // Base Relocation Table
	{ IMAGE_DIRECTORY_ENTRY_DEBUG         ,  ""       }, // Debug Directory
	{ IMAGE_DIRECTORY_ENTRY_ARCHITECTURE  ,  ""       }, // Architecture Specific Data
	{ IMAGE_DIRECTORY_ENTRY_GLOBALPTR     ,  ""       }, // RVA of GP
	{ IMAGE_DIRECTORY_ENTRY_TLS           ,  ""       }, // TLS Directory
	{ IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG   ,  ""       }, // Load Configuration Directory
	{ IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT  ,  ""       }, // Bound Import Directory in headers
	{ IMAGE_DIRECTORY_ENTRY_IAT           ,  ""       }, // Import Address Table
	{ IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT  ,  ""       }, // Delay Load Import Descriptors
	{ IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR,  ""       }, // COM Runtime descriptor
};

CHAR* GetDirName( USHORT id ) {
	UINT i;
	for( i=0; i<countof(g_dir_names); ) {
		if ( id == g_dir_names[i].id )
			return g_dir_names[i].name;
	}
	return "";
}


// ---
BOOL SkipSection( USHORT sec_id ) {
	UINT i;
	for( i=0; i<countof(g_skiped_sections); ++i ) {
		if ( sec_id == g_skiped_sections[i] )
			return TRUE;
	}
	return FALSE;
}



// ---
// petrovitch 16.03.01
void AddMsg( int significance, const char* format, ... ) {
  if ( !brief || significance ) {
    va_list arglist;
    va_start( arglist, format );
		if ( console )
			vprintf( format, arglist );
		else
			count += pr_vsprintf( msg_buff+count, sizeof(msg_buff)-count, format, arglist );
  }
}


// ---
void results() {
  if ( console ) {
    //printf( title );
    //printf( "\n" );
		//#if !defined (_DEBUG)
		//	printf( msg_buff );
		//#endif
    if ( pause ) {
      printf( "\npress any key..." );
      getch();
    }
  }
  else 
    MessageBox( 0, msg_buff, title, MB_OK );
}



// ---
typedef struct {
	DWORD  flag;
	PSTR   name;
} DWORD_FLAG_DESCRIPTIONS;

DWORD_FLAG_DESCRIPTIONS SectionCharacteristics[] = {
	{ IMAGE_SCN_CNT_CODE, "CODE" },
	{ IMAGE_SCN_CNT_INITIALIZED_DATA, "INITIALIZED_DATA" },
	{ IMAGE_SCN_CNT_UNINITIALIZED_DATA, "UNINITIALIZED_DATA" },
	{ IMAGE_SCN_LNK_INFO, "LNK_INFO" },
	// { IMAGE_SCN_LNK_OVERLAY, "LNK_OVERLAY" },
	{ IMAGE_SCN_LNK_REMOVE, "LNK_REMOVE" },
	{ IMAGE_SCN_LNK_COMDAT, "LNK_COMDAT" },
	{ IMAGE_SCN_MEM_DISCARDABLE, "MEM_DISCARDABLE" },
	{ IMAGE_SCN_MEM_NOT_CACHED, "MEM_NOT_CACHED" },
	{ IMAGE_SCN_MEM_NOT_PAGED, "MEM_NOT_PAGED" },
	{ IMAGE_SCN_MEM_SHARED, "MEM_SHARED" },
	{ IMAGE_SCN_MEM_EXECUTE, "MEM_EXECUTE" },
	{ IMAGE_SCN_MEM_READ, "MEM_READ" },
	{ IMAGE_SCN_MEM_WRITE, "MEM_WRITE" },
};


// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (DWORD)(addValue))
#define NUMBER_SECTION_CHARACTERISTICS \
    (sizeof(SectionCharacteristics) / sizeof(DWORD_FLAG_DESCRIPTIONS))


char Usage[] =

  #ifdef _DEBUG
    "Debug version\n"
  #else
    "Release version\n"
  #endif
  "  Usage :\n"
  "    %s   [-?|h]   [-c|m]   [-p]   [-i]\"name\"   [-o\"name\"]\n"
  "      -m\t\t: output to message box\n"
  "      -b\t\t: brief\n"
  "      -p\t\t: pause\n"
  "      -i\"name\"\t: input file name (default option - key \"-i\" can be omitted)\n"
  "      -o\"name\"\t: output file name\n"
	"      -c\t\t: just check file for conversion possible\n"
  "    any file name can be used with quotation marks ( \" or \' )\n"
  "    or without ones if it doesn't contain spaces\n\n";

static char szOutput[MAX_PATH];
static char szTmpFile[MAX_PATH];
static char szTmpDir[MAX_PATH];
static char exe_name[MAX_PATH];
static char cmd_line[0x200];
static char file_name[MAX_PATH];

/*
szTmp = strrchr( szOutput, '.' );
if ( szTmp ) {
if ( !lstrcmpi(szTmp+1,"ppl") ) {
static char tmp[MAX_PATH];
lstrcpy( tmp, to_convert );
lstrcat( tmp, ".bak" );
DeleteFile( tmp );
MoveFile( to_convert, tmp );
to_convert = tmp;
}
*szTmp=0;
}
lstrcat(szOutput,".ppl");
*/

// ---
#ifdef _CONSOLE
  int main( void ) {
#elif _WINDOWS
  int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
#else
  #error One of _WINDOWS or _CONSOLE must be defined
#endif

  PCHAR szTmp;
  int   ret = 0;
	LPSTR dst;
	BOOL  the_same;
	DWORD dwAttrib;

  prepare_cmd_line( GetCommandLine(), exe_name, cmd_line );

  szTmp = strrchr( exe_name, '\\' );
  if ( szTmp )
    memmove( exe_name, szTmp+1, lstrlen(szTmp+1) );

  szTmp = strrchr( exe_name, '//' );
  if ( szTmp )
    memmove( exe_name, szTmp+1, lstrlen(szTmp+1) );
  
  szTmp = strrchr( exe_name, '.' );
  if ( szTmp )
    *szTmp = 0;

  if ( get_option(cmd_line,'m',0,1) )
		console = 0;
	else
		console = 1;

  check = get_option( cmd_line, 'c', 0, 1 );
  brief = get_option( cmd_line, 'b', 0, 1 );
  pause = get_option( cmd_line, 'p', 0, 1 );

  if ( get_option(cmd_line,'?',0,1) ) {
    goto do_usage_lbl;

    usage_lbl:
      ret = 1;
    do_usage_lbl:

      AddMsg( 1, Usage, exe_name );
      results();
      return ret;
  }

  if ( !(get_option(cmd_line,'i',file_name,1) || get_option(cmd_line,0,file_name,1)) )
    goto usage_lbl;

  if ( !(get_option(cmd_line,'o',szOutput,1) || get_option(cmd_line,0,szOutput,1)) )
    szOutput[0] = 0;
	
	if( !szOutput[0] )
		lstrcpy( szOutput, file_name );

	if ( stricmp(szOutput,file_name) ) {
		the_same = 0;
		dst = szOutput;
	}
	else {
		the_same = 1;
		GetTempPath( sizeof(szTmpDir), szTmpDir );
		GetTempFileName( szTmpDir, "ppl_", 0, szTmpFile );
		dst = szTmpFile;
	}

	if ( check )
		AddMsg( 1, "Checking if \"%s\" can be converted\n", file_name );
	else
		AddMsg( 1, "Converting of file \"%s\" into \"%s\"\n", file_name, szOutput );

	__try {
		ret = ConvertFile( file_name, dst );
	}
	__except( TRUE ) { // Should only get here if pNTHeader (above) is bogus
		AddMsg( 1, "invalid .EXE\n");
		ret = 1;
	}

	if ( ret != 0 ) 
		DeleteFile( dst );

	else if ( the_same && !check ) {
		// А кто будет RO отрабатывать ? :-)))))
		dwAttrib = GetFileAttributes(file_name );
		SetFileAttributes(file_name,  FILE_ATTRIBUTE_ARCHIVE);
		DeleteFile( file_name );
		MoveFile( dst, file_name );
		SetFileAttributes(file_name,  dwAttrib);
	}

	if ( ret == 6 ) { // already converted not a bug
		ret = 0;
	}

	results();
	return ret;
}



// ---
int ConvertFile( LPSTR szSrcFileName, LPSTR szDestFileName ) {

	int          result;
	LOADED_IMAGE image = {0};
	HANDLE       hDestFile = INVALID_HANDLE_VALUE;
	DWORD        dwMetaDataSize = 0;
	LPBYTE       lpMetaData = NULL;

	__try {
		HMODULE mod = LoadLibraryEx( szSrcFileName, 0, LOAD_LIBRARY_AS_DATAFILE );
		if ( mod ) {
			HGLOBAL res = 0;
			LPBYTE  mem = 0;
			HRSRC   src = FindResource( mod, MAKEINTRESOURCE(1), RT_RCDATA );
			if ( src )
				res = LoadResource( mod, src );
			if ( res )
				mem = (char*)LockResource( res );
			if ( mem ) {
				dwMetaDataSize = SizeofResource( mod, src );
				lpMetaData = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwMetaDataSize );
				if ( lpMetaData )
					memcpy( lpMetaData, mem, dwMetaDataSize );
			}
			FreeLibrary( mod );
		}

		if ( dwMetaDataSize == 0 )
			AddMsg(1,"Cannot find Meta data. This is may be not a Prague Plugin (%s)\n\n",szSrcFileName);

		if ( !MapAndLoad(szSrcFileName,0,&image,TRUE,TRUE) ) {
			AddMsg( 1,"Couldn't open file with MapAndLoad(%s)\n", szSrcFileName );
			return 2;
		}

		if ( !check ) {
			hDestFile = CreateFile( szDestFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
			if ( hDestFile == INVALID_HANDLE_VALUE ) {
				AddMsg(1,"Couldn't create file with CreateFile(%s)\n", szDestFileName);
				check = TRUE;
			}
		}

		result = RealConvert( &image, hDestFile, lpMetaData, dwMetaDataSize );

		if( 0 == result ) {
			if ( check )
				AddMsg( 1, "\nFile can be converted!\n" );
			else
				AddMsg( 1, "\nFile converted succesfully!\n" );
			return 0;
		}

		if ( 1 == result ) {
			AddMsg( 1, "\nFile cannot be converted for some reason.\n" );
			return result;
		}

		if ( 2 == result ) {
			AddMsg( 1, "\nFile is already in PRAGUE format!\n" );
			// Changed by Andy	
			// This seems to be an error but we'll assume that is not :)
			return 6;	// and it'll be checked in caller fn
		}
		CloseHandle( hDestFile );
		hDestFile = INVALID_HANDLE_VALUE;
		DeleteFile( szDestFileName );
		return 1;
	}

	__finally
	{
		if ( image.MappedAddress )
			UnMapAndLoad( &image );
		if ( hDestFile != INVALID_HANDLE_VALUE )
			CloseHandle( hDestFile );
		if ( lpMetaData )
			HeapFree( GetProcessHeap(),0, lpMetaData );
	}
	return 0;
}



// ---
void print_chars( DWORD flags ) {
	DWORD j;
	AddMsg( 0, "    characteristics: 0x%08x", flags );
	//AddMsg( 0, "    ");
	for ( j=0; j < NUMBER_SECTION_CHARACTERISTICS; j++ ) {
		if ( flags & SectionCharacteristics[j].flag )
			AddMsg( 0, "  %s", SectionCharacteristics[j].name );
	}
	AddMsg( 0, "\n");
}



// ---
int ConvertSectionTable( PrFileHeader* lpHeader, LOADED_IMAGE* image, HANDLE hFile, LPBYTE lpMetaData, DWORD dwMetaDataSize ) {
	UINT                  i, c;
	DWORD                 dwWrote;
	IMAGE_SECTION_HEADER* section = image->Sections;
	UINT                  section_count = image->NumberOfSections;
	SectionTable          SecTable;
	DWORD                 dwPointerToRawData = sizeof(PrFileHeader);
	DWORD                 dwVirtualSize = 0;
	PUCHAR                pBase = image->MappedAddress;

	AddMsg( 0, "Section Table\n" );

	for ( i=0 ; i < section_count; ++i,++section ) {
		if ( SkipSection(GetSectionId(image,section)) ) // if(memcmp(section->Name, ".rsrc", 5)==0)
			continue;
		dwPointerToRawData += sizeof(SectionTable);
	}

	if ( dwMetaDataSize > 0 )
		dwPointerToRawData += sizeof(SectionTable);

	//Verinfo section
	section = image->Sections;
	for ( i=0,c=0; i < section_count; ++i,++section ) {
		CHAR*  name;
		USHORT sec_id = GetSectionId( image, section );
		if ( SkipSection(sec_id) ) { // if ( memcmp(section->Name,".rsrc",5) == 0 )
			//AddMsg( 0, "\n\n");
			continue;
		}

		//memcpy( SecTable.chSectionName, section->Name, sizeof(SecTable.chSectionName) );
		SecTable.dwPhisicalSize = min(section->SizeOfRawData, section->Misc.VirtualSize);

		if ( (sec_id != IMAGE_DIRECTORY_ENTRY_BASERELOC) && !SecTable.dwPhisicalSize )
			continue;

		++c;
		name = section->Name;
		if ( !*name )
			name = GetDirName( sec_id );

		//AddMsg( 0, "  %02X %-8.8s  %s: %08X  VirtAddr:  %08X\n", i, section->Name, "VirtSize" , section->Misc.VirtualSize, section->VirtualAddress );
		AddMsg( 0, "  %02X %s\n", c, name );
		//AddMsg( 0, "    VirtSize:        0x%08x  VirtAddr:      0x%08x\n", section->Misc.VirtualSize, section->VirtualAddress );
		//AddMsg( 0, "    raw data offs:   0x%08x  raw data size: 0x%08x\n", section->PointerToRawData, section->SizeOfRawData );
		//AddMsg( 0, "    relocation offs: 0x%08x  relocations:   0x%08x\n", section->PointerToRelocations, section->NumberOfRelocations );
		//AddMsg( 0, "    line # offs:     0x%08x  line #'s:      0x%08x\n", section->PointerToLinenumbers, section->NumberOfLinenumbers );
		//print_chars( section->Characteristics );
		//AddMsg( 0, "    ----------------------------------------------\n" );

		SecTable.dwSectionOffset = dwPointerToRawData;
		dwPointerToRawData += SecTable.dwPhisicalSize;

		SecTable.dwSectionRVA = section->VirtualAddress;
		SecTable.dwVirtualSize = section->Misc.VirtualSize;
		SecTable.dwFlags = section->Characteristics;
		//if ( memcmp(section->Name,".reloc",6) != 0 ) {
		if ( sec_id == IMAGE_DIRECTORY_ENTRY_BASERELOC ) {
			if ( (section->VirtualAddress + SecTable.dwVirtualSize) > dwVirtualSize )
				dwVirtualSize = section->VirtualAddress + SecTable.dwVirtualSize;
		}
		else {
			lpHeader->dwReloSectionSize = SecTable.dwPhisicalSize;
			lpHeader->dwReloSectionOffset = SecTable.dwSectionOffset;
		}
		
		//AddMsg( 0, "    %-8.8s  %s: %08X  VirtAddr:  %08X\n", SecTable.chSectionName, "VirtSize", SecTable.dwVirtualSize , SecTable.dwSectionRVA);
		AddMsg( 0, "    VirtSize:        0x%08x  VirtAddr:      0x%08x\n", SecTable.dwVirtualSize , SecTable.dwSectionRVA );
		AddMsg( 0, "    raw data offs:   0x%08x  raw data size: 0x%08x\n", SecTable.dwSectionOffset, SecTable.dwPhisicalSize );
		print_chars( SecTable.dwFlags );
		if ( !check )
			WriteFile(hFile, &SecTable, sizeof(SecTable), &dwWrote, NULL);
	}

	//MetaData
	if ( dwMetaDataSize > 0 ) {
		ZeroMemory(&SecTable, sizeof(SecTable));
		// memcpy( SecTable.chSectionName, ".meta", 5 );

		lpHeader->dwMetaDataSectionSize = SecTable.dwPhisicalSize = dwMetaDataSize;
		lpHeader->dwMetaDataSectionOffset = SecTable.dwSectionOffset = dwPointerToRawData;
		lpHeader->wNumOfObjects++;

		dwPointerToRawData += SecTable.dwPhisicalSize;

		SecTable.dwFlags = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_DISCARDABLE|IMAGE_SCN_MEM_READ;
	
		AddMsg( 0, "  %02X .meta\n", ++c );
		AddMsg( 0, "    VirtSize:        0x%08X  VirtAddr:      0x%08x\n", SecTable.dwVirtualSize , SecTable.dwSectionRVA );
		AddMsg( 0, "    raw data offs:   0x%08x  raw data size: 0x%08x\n", SecTable.dwSectionOffset, SecTable.dwPhisicalSize );

		print_chars( SecTable.dwFlags );

		AddMsg( 0, "\n\n");

		if ( !check )
			WriteFile(hFile, &SecTable, sizeof(SecTable), &dwWrote, NULL);
	}

	section = image->Sections;
	for ( i=0; i < section_count; ++i,++section ) {
		DWORD size;
		if ( SkipSection(GetSectionId(image,section)) ) //if ( memcmp(section->Name,".rsrc",5) == 0 )
			continue;
		if ( !check ) {
			size = min( section->SizeOfRawData, section->Misc.VirtualSize );
			WriteFile(hFile, pBase+section->PointerToRawData, size, &dwWrote, NULL);
		}
		lpHeader->wNumOfObjects++;
	}

	//MetaData
	if ( !check && (dwMetaDataSize > 0) )
		WriteFile(hFile, lpMetaData , dwMetaDataSize, &dwWrote, NULL);

	dwVirtualSize += 0x1000;
	dwVirtualSize &= 0xfffff000;
	lpHeader->dwPluginVirtualSize = dwVirtualSize;
	return 0;
}



// ---
int RealConvert( LOADED_IMAGE* image, HANDLE hFile, LPBYTE lpMetaData, DWORD dwMetaDataSize ) {

	int                      result;
	PrFileHeader             PrHeader = {0};
	DWORD                    dwWrote;
	BOOL                     has_imports;
	IMAGE_OPTIONAL_HEADER32* opt;
	PrFileHeader*            now = (PrFileHeader*)image->MappedAddress;

	if ( now->dwSignature == cHEADER_SIGNATURE ) {
		if ( now->wCPUType != cINTEL_32_CPU ) {
			AddMsg( 1, "File is prague plugin with bad CPU Type 0x%x\n", now->wCPUType );
			return 2;
		}
		AddMsg( 1, "File is already Prague Plugin\n" );
		return 2;
	}

	// First, verify that the e_lfanew field gave us a reasonable pointer, then verify the PE signature.
	__try {
		if ( image->FileHeader->Signature != IMAGE_NT_SIGNATURE ) {
			AddMsg( 1, "Source file is not a Portable Executable (PE) EXE\n");
			return 1;
		}
	}
	__except( TRUE ) { // Should only get here if pNTHeader (above) is bogus
		AddMsg( 1, "invalid .EXE\n");
		return 1;
	}

	has_imports = HasAnyImports( image );
	if ( has_imports ) {
		if ( check )
			AddMsg( 1, "File has IMPORT table\n" );
		else
			AddMsg( 1, "Cannot convert file with IMPORT table\n" );
		AddMsg( 0, "\n" );
		check = TRUE;
		result = 1;
	}
	else if ( check ) {
		AddMsg( 1, "There is no any IMPORT table in the file\n" );
		AddMsg( 0, "\n" );
	}

  //Check Entry Point
	opt = &image->FileHeader->OptionalHeader;
	if ( opt->AddressOfEntryPoint == 0 ) {
		AddMsg( 1, "Cannot convert file with 0 entry point\n" );
		AddMsg( 0, "\n" );
		check = TRUE;
		result = 1;
	}

	//Check for DLL
	//if(pNTHeader->OptionalHeader.ImageBase != 0x10000000) {
	//	AddMsg( 1, "Cannot convert DLL with ImageBase != 0x10000000 into Prague Plugin\n\n" );
	//	return 1;
	//}

	PrHeader.dwSignature = cHEADER_SIGNATURE;
	switch ( image->FileHeader->FileHeader.Machine ) { // pNTHeader->FileHeader.Machine
		case IMAGE_FILE_MACHINE_I386 : // (0x14C) Intel 386.
		case 0x14D                   :
		case 0x14E                   :
			PrHeader.wCPUType = cINTEL_32_CPU;
			break;

		case IMAGE_FILE_MACHINE_R3000 : // (0x162) MIPS Mark I (R2000, R3000) 
		case 0x163                    : // (0x163) MIPS Mark II (R6000) 
		case IMAGE_FILE_MACHINE_R4000 : // (0x166) MIPS Mark III (R4000) 
			PrHeader.wCPUType = cMIPS_CPU;
			break;
		default:
			AddMsg( 1, "Unknown CPU type 0x%x\n", image->FileHeader->FileHeader.Machine ); // pNTHeader->FileHeader.Machine
			check = TRUE;
			result = 1;
			break;
	}
	
	PrHeader.dwEntryPoint = opt->AddressOfEntryPoint;
	PrHeader.dwImageBase = opt->ImageBase;
	PrHeader.dwOSVersion = 0x100;
	if ( !check )
		WriteFile( hFile, &PrHeader, sizeof(PrFileHeader), &dwWrote, NULL );

	ConvertSectionTable( &PrHeader, image, hFile, lpMetaData, dwMetaDataSize );

	AddMsg( 0, "Plugin Entry Point RVA       %#x\n",PrHeader.dwEntryPoint );
	AddMsg( 0, "Plugin Image Base            %#x\n",PrHeader.dwImageBase);
	AddMsg( 0, "Plugin CPU Type              %#x\n",PrHeader.wCPUType );
	AddMsg( 0, "Plugin's Number Of Object    %#x\n",PrHeader.wNumOfObjects );
	AddMsg( 0, "Plugin Virtual Size          %#x\n",PrHeader.dwPluginVirtualSize);
	AddMsg( 0, "Relocation Section Size      %#x\n",PrHeader.dwReloSectionSize);
	AddMsg( 0, "Relocation Section Offset    %#x\n",PrHeader.dwReloSectionOffset);
	AddMsg( 0, "MetaData Section Size        %#x\n",PrHeader.dwMetaDataSectionSize);
	AddMsg( 0, "MetaData Section Offset      %#x\n",PrHeader.dwMetaDataSectionOffset);

	if ( !check ) {
		SetFilePointer( hFile, 0, 0, FILE_BEGIN );
		WriteFile( hFile, &PrHeader, sizeof(PrFileHeader), &dwWrote, NULL );
	}

	return result;
}


// ---
USHORT GetSectionId( LOADED_IMAGE* image, IMAGE_SECTION_HEADER* section ) {
	USHORT i;
	IMAGE_DATA_DIRECTORY* dir = image->FileHeader->OptionalHeader.DataDirectory;
	for( i=0; i<16; ++i,++dir ) {
		if ( dir->VirtualAddress == section->VirtualAddress )
			return i;
	}
	return -1;
}



// ---
UINT CountImportsByDirID( LOADED_IMAGE* image, USHORT id, CHAR* table_name ) {
	IMAGE_IMPORT_DESCRIPTOR* import;
	IMAGE_SECTION_HEADER*    last;
	UINT                     count;
	ULONG                    size;
	IMAGE_DATA_DIRECTORY*    dir = image->FileHeader->OptionalHeader.DataDirectory;

	static USHORT valid_sections[] = {
		IMAGE_DIRECTORY_ENTRY_IMPORT,
		IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT,
	};
	
	for( count=0; count<countof(valid_sections); ++count ) {
		if ( valid_sections[count] == id )
			break;
	}
	if ( count >= countof(valid_sections) )
		return 0;

	if ( dir[id].VirtualAddress == 0 )
		return 0;

	import = (IMAGE_IMPORT_DESCRIPTOR*)ImageDirectoryEntryToData( image->MappedAddress, FALSE, id, &size );
	last = image->LastRvaSection;
	if ( import->Characteristics )
		AddMsg( 0, table_name );
	for( count=0; import->Characteristics; ++import ) {
		UINT   fcount;
		DWORD* func;
		CHAR*  name = (CHAR*)ImageRvaToVa( image->FileHeader, image->MappedAddress, import->Name, &last );
		AddMsg( 0, "    %s:\n", name );
		func = (DWORD*)ImageRvaToVa( image->FileHeader, image->MappedAddress, import->FirstThunk, &last );
		for( fcount=0; func && *func; ++fcount,++func ) {
			CHAR* name;
			CHAR  bname[20];
			if ( IMAGE_ORDINAL_FLAG32 & *func ) {
				_itoa( (*func & (~IMAGE_ORDINAL_FLAG32)), bname, 10 );
				name = bname;
			}
			else {
				IMAGE_IMPORT_BY_NAME* entry = (IMAGE_IMPORT_BY_NAME*)ImageRvaToVa( image->FileHeader, image->MappedAddress, *func, &last );
				name = entry->Name;
			}
			AddMsg( 0, "      %s\n", name );
		}
		if ( fcount ) {
			count += fcount;
			AddMsg( 0, "      %d imports\n", fcount );
		}
	}
	if ( count )
		AddMsg( 0, "    %d imports total\n\n", count );
	return count;
}



// ---
BOOL HasAnyImports( LOADED_IMAGE* image ) {
	UINT imports0 = CountImportsByDirID( image, IMAGE_DIRECTORY_ENTRY_IMPORT,       "  IMPORT table:\n" );
	UINT imports1 = CountImportsByDirID( image, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, "  IMPORT table (delayed):\n");
	if ( imports0 + imports1 )
		return TRUE;
	return FALSE;
}

