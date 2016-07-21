// AVP LIB.C source code
// this source contains descriptions (strings) for menus in AVP Editor
// _string means that this field contains a constant
// *string means that this field contains a pointer to data within Page
// Note!  Editor TAB size is 4

#include        <windows.h>
#include        "../bases/method.h"
#include        "signs.h"

// Jump records -----------------------------------------------------------

static uchar   *Jmp_Spc[]= { "Special", "", "", "" };
static uchar   *Jmp_Offs[]={ "Offset", "Offs", "", "" };
static uchar   *Jmp_Addr[]={ "Address", "Addr", "", "" };
static uchar   *Jmp_Data[]={ "Data", "Data", "", "" };
static uchar   *Jmp_FarA[]={ "Far_PA", "CS", "IP", "Add" };
static uchar   *Jmp_FarB[]={ "Far_PB", "CS", "IP", "Add" };
static uchar   *Jmp_FarAB[]={ "Far_PAB", "CS", "IP", "Add" };

// File records   '*' means offset in pages, '_' means constant

static uchar   *File_Special[]={ "Special", "", "", "", "", "" };
static uchar   *File_Fail[]={ "Fail", "", "", "", "", "" };
static uchar   *File_Delete[]={ "Delete", "", "", "", "", "" };

static uchar   *File_Move[]={ "Move", "*From", "_Len", "_To", "", "_Cut" };
static uchar   *File_Lehigh[]={ "Lehigh", "*From", "_Len", "_To", "", "_Cut" };
static uchar   *File_Jeru[]={ "Jeru", "_LenLo", "_LenHi", "", "", "_Cut" };
static uchar   *File_Start[]={ "Start", "_LenLo", "_LenHi", "", "_Cut2", "_Cut" };
static uchar   *File_Sys_SI[]={ "SYS SI", "*Str", "*Int", "", "", "_Cut" };

static uchar   *File_Com3Bytes[]={ "Com 3 Bytes", "*1st", "*2nd", "*3rd", "", "_Cut" };
static uchar   *File_Com4Bytes[]={ "Com 4 Bytes", "*1st", "*2nd", "*3rd", "*4th", "_Cut" };
static uchar   *File_ComJmp[]={ "Com Jump", "*Offs", "", "", "", "_Cut" };

static uchar   *File_E_CISS[]={ "Exe CISS", "*CS", "*IP", "*SS", "*SP", "_Cut" };
static uchar   *File_E_CIS[]={ "Exe CIS", "*CS", "*IP", "*SS", "", "_Cut" };
static uchar   *File_E_CI[]={ "Exe CI", "*CS", "*IP", "", "", "_Cut" };

static uchar   *File_E_CISS_10[]={ "Exe CISS 10", "*CS-10", "*IP", "*SS-10", "*SP", "_Cut" };
static uchar   *File_E_CIS_10[]={ "Exe CIS 10", "*CS-10", "*IP", "*SS-10", "", "_Cut" };
static uchar   *File_E_CI_10[]={ "Exe CI 10", "*CS-10", "*IP", "", "", "_Cut" };

static uchar   *File_E_CISS_Min[]={ "Exe CISS Min", "*-CS", "*IP", "*-SS", "*SP", "_Cut" };
static uchar   *File_E_CIS_Min[]={ "Exe CIS Min", "*-CS", "*IP", "*-SS", "", "_Cut" };
static uchar   *File_E_CI_Min[]={ "Exe CI Min", "*-CS", "*IP", "", "", "_Cut" };

static uchar   *File_E_CISS_Pls[]={ "Exe CISS Pls", "*+CS", "*IP", "*+SS", "*SP", "_Cut" };
static uchar   *File_E_CIS_Pls[]={ "Exe CIS Pls", "*+CS", "*IP", "*+SS", "", "_Cut" };
static uchar   *File_E_CI_Pls[]={ "Exe CI Pls", "*+CS", "*IP", "", "", "_Cut" };

static uchar   *File_E_CISS_Fix[]={ "Exe CISS Fix", "*CSx", "*IP", "*SSx", "*SP", "_Cut" };
static uchar   *File_E_CIS_Fix[]={ "Exe CIS Fix", "*CSx", "*IP", "*SSx", "", "_Cut" };
static uchar   *File_E_CI_Fix[]={ "Exe CI Fix", "*CSx", "*IP", "", "", "_Cut" };

static uchar   *File_ExeHdr_2Exe[]={ "ExeHdr 2Exe", "_Fix", "", "", "", "_Fill" };
static uchar   *File_ExeHdr_CS_IP[]={ "ExeHdr CS IP", "_CS", "_IP", "_Relocs", "_HdrLen", "_Fill" };
static uchar   *File_ExeHdr_CS_IP2[]={ "Reserved", "", "", "", "", "" };

static uchar   *File_OLE2[]=	{ "Macro",  "_Macros", "", "", "", "" };
static uchar   *File_Script[]=	{ "Script", "_Data1", "_Data2", "_Data3", "_Data4", "_Data5" };

static uchar   *File_PE_Fix_Struct[]=		{ "PE Just Fix",	"", "", "", "", "" };
static uchar   *File_PE_Cut_Section[]=		{ "PE RVA Sect-",	"*EIP", "", "", "", "_Cut" };
static uchar   *File_PE_Cut_Section_Sub[]=	{ "PE VA Sect-",	"*EIP", "", "", "", "_Cut" };
static uchar   *File_PE_Last_Section[]=		{ "PE RVA",		"*EIP", "", "", "", "_Cut" };
static uchar   *File_PE_Last_Section_Sub[]=	{ "PE VA",		"*EIP", "", "", "", "_Cut" };
static uchar   *File_PE_Offs_Cut_Section[]=	{ "PE Offs Sect-",	"*EIP", "", "", "", "_Cut" };
static uchar   *File_PE_Offs[]=			{ "PE Offs",		"*EIP", "", "", "", "_Cut" };

// Sector records 

static uchar   *Sect_Special[] ={ "Special",      "",          "",          "" };
static uchar   *Sect_Fail[]    ={ "Fail",         "",          "",          "" };
static uchar   *Sect_Overwrite[]={ "Overwrite",   "",          "",          "" };
static uchar   *Sect_Address[] ={ "Address",   "_Sector CX",    "_Head DH",   "" };
static uchar   *Sect_Absolute[]=   { "Absolute CX",  "*Sector [CX]",  "*Head [DH]", "_Plus" };
static uchar   *Sect_Absolute_CL[]={ "Absolute CL",  "*Sector [CL]",  "*Head [DH]", "_Plus" };
static uchar   *Sect_Logical[] ={ "Logical",   "*Sector [CX]",  "",          "_Plus" };
static uchar   *Sect_ActiveBoot[]={ "ActiveBoot", "",          "",          "" };
static uchar   *Sect_Constant[]=  { "Constant", "_360K/1.2M", "_720K/1.4M", "" };

// Memory records 

static uchar   *Mem_Info[]={ "Special", "Address", "Cut", "MCB", "Trace",
"Scan", "FullScan", "W95_Add", "W95_Ifs", "W32_Api"	};
// check  MEM_ defines!!!!

//  

int DLL_GetMethodCount(int type)
{
  switch(type)
  {
  case 0:          return 7;		// JMPs
  case 1:          return 37;		// FILEs
  case 2:          return 9;		// SECTORs
  case 3:          return 10;		// MEMs
  default:         return 0;
  }
}
char**  DLL_GetMethodText(int Object,int No){

// Object:      0 - Jmp, 1 - File, 2 - Sector, 3 - Mem
// No:          Method No

switch (Object) {

 case 0:        // JMP
 switch (No)    {

        case JMP_SPC:           return Jmp_Spc;        // Special
        case JMP_OFFSET:        return Jmp_Offs;       // Offset
        case JMP_ADDRESS:       return Jmp_Addr;       // Address
        case JMP_DATA:          return Jmp_Data;       // Data
        case JMP_FAR_A:			return Jmp_FarA;       // JmpFar, only Header
        case JMP_FAR_B:			return Jmp_FarB;       // JmpFar, Header and PageA
        case JMP_FAR_AB:        return Jmp_FarAB;      // JmpFar, Header and PageA
                }       return 0;

 case 1:        // FILE
 switch (No)    {
		case FILE_SPC			:		return	File_Special;
		case FILE_FAIL			:		return	File_Fail;
		case FILE_DELETE		:		return	File_Delete;

		case FILE_MOVE			:		return	File_Move;
		case FILE_LEHIGH		:		return	File_Lehigh;
		case FILE_JERU			:		return	File_Jeru;
		case FILE_START			:		return	File_Start;
		case FILE_SYS_SI		:		return	File_Sys_SI;

		case FILE_COM_3BYTES	:		return	File_Com3Bytes;
		case FILE_COM_4BYTES	:		return	File_Com4Bytes;
		case FILE_COM_JMP		:		return	File_ComJmp;

		case FILE_E_CISS		:		return	File_E_CISS;
		case FILE_E_CIS			:		return	File_E_CIS;
		case FILE_E_CI			:		return	File_E_CI;

		case FILE_E_CISS_10		:		return	File_E_CISS_10;
		case FILE_E_CIS_10		:		return	File_E_CIS_10;
		case FILE_E_CI_10		:		return	File_E_CI_10;

		case FILE_E_CISS_MIN	:		return	File_E_CISS_Min;
		case FILE_E_CIS_MIN		:		return	File_E_CIS_Min;
		case FILE_E_CI_MIN		:		return	File_E_CI_Min;

		case FILE_E_CISS_PLS	:		return	File_E_CISS_Pls;
		case FILE_E_CIS_PLS		:		return	File_E_CIS_Pls;
		case FILE_E_CI_PLS		:		return	File_E_CI_Pls;

		case FILE_E_CISS_FIX	:		return	File_E_CISS_Fix;
		case FILE_E_CIS_FIX		:		return	File_E_CIS_Fix;
		case FILE_E_CI_FIX		:		return	File_E_CI_Fix;

		case FILE_E_HDR_2EXE	:		return	File_ExeHdr_2Exe;
		case FILE_E_HDR_CS_IP	:		return	File_ExeHdr_CS_IP;
		case FILE_E_HDR_CS_IP2	:		return	File_ExeHdr_CS_IP2;

		case FILE_OLE2			:		return	File_OLE2;
		case FILE_SCRIPT		:		return	File_Script;

		case FILE_PE_FIX_STRUCT		:	return	File_PE_Fix_Struct;
		case FILE_PE_CUT_SCT		:	return	File_PE_Cut_Section;
		case FILE_PE_CUT_SCT_SUB	:	return	File_PE_Cut_Section_Sub;
		case FILE_PE_LST_SCT		:	return	File_PE_Last_Section;
		case FILE_PE_LST_SCT_SUB	:	return	File_PE_Last_Section_Sub;
		case FILE_PE_OFFS_SCTCUT	:	return	File_PE_Offs_Cut_Section;
		case FILE_PE_OFFS		:	return	File_PE_Offs;
                }       return 0;

 case 2:        // SECTOR
 switch (No)    {
        case SECT_SPC           :       return  Sect_Special;
        case SECT_FAIL          :       return  Sect_Fail;
        case SECT_OVERWRITE     :       return  Sect_Overwrite;
        case SECT_ADDRESS       :       return  Sect_Address;
        case SECT_ABSOLUTE      :		return  Sect_Absolute;
		case SECT_ABSOLUTE_CL   :		return	Sect_Absolute_CL;
        case SECT_LOGICAL       :       return  Sect_Logical;
        case SECT_ACTIVE_BOOT	:       return  Sect_ActiveBoot;
		case SECT_CONSTANT		:       return  Sect_Constant;
                }       return 0;

 case 3:                return  Mem_Info;      }

return 0;               }


BOOL WINAPI DllMain(HINSTANCE hDLLInst,
                  DWORD  fdwReason,
                  LPVOID lpvReserved)
{
/*
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:	break;
	case DLL_PROCESS_DETACH:	break;
	case DLL_THREAD_ATTACH:		break;
	case DLL_THREAD_DETACH:		break;
	}
*/
	return TRUE;
}
