#include "debug.h"
#include "osspec.h"

#ifdef _DEBUG
	#pragma message ("----------------------- Debug detected - DbgBreakPoint, HOOKKdPrint are enabled")
#else
	#pragma message ("----------------------- Release detected - DbgBreakPoint, HOOKKdPrint are disabled")
#endif

#ifdef __DBG__
/*#ifdef _HOOK_NT_
#pragma data_seg(".userd")
#endif*/
	// located in ".user" section
	DEBUG_INFO_LEVEL DebLev = DL_IMPORTANT;
	ULONG DebCat = -1;
/*#ifdef _HOOK_NT_
#pragma data_seg()
#endif*/
#endif // __DBG__


#ifdef _HOOK_NT_
#pragma data_seg(".userd")
#pragma const_seg(".userd")
#endif

// located in ".user" section
CHAR *DebugLevelName[] = {
	"FATAL_ERROR",
	"ERROR",
	"WARNING",
	"IMPORTANT",
	"NOTIFY",
	"INFO",
	"SPAM"
};

// located in ".user" section
CHAR NullStr[] = "\0";
WCHAR NullStrU[] = L"\0";

CHAR UnknownStr[] = "-?-";
WCHAR UnknownStrW[] = L"-?-";

#ifdef __DBG__

CHAR *DebCatName[] = {
	NullStr,
		"SYS",
		"FILE",
		"DISK",
		"REG",
		"R3",
		"FWALL",
		"I21",
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		NullStr,
		"IOCTL",
		"CLIENT",
		"EVENT",
		"FILTER",
		"INVTH",
		"LOG",
		"DOG",	
		"NAMEC",	
		"TSP",
		"OBJC",
		"Low level disk IO",
		"Filtering Params",
};

//+ ----------------------------------------------------------------------------------------

BOOLEAN
__DbPrint(ULONG Category, DEBUG_INFO_LEVEL Level)
{
	int CatIdx;
	PCHAR DCN;
	PCHAR DLN;

	if(DebLev < Level)
		return FALSE;

	if(Level > DL_WARNING && !(Category & DebCat))
		return FALSE;

	__asm bsf eax,Category;
	__asm mov CatIdx, eax;
	if(CatIdx < sizeof(DebCatName) / sizeof(CHAR*))
		DCN = DebCatName[CatIdx];
	else
		DCN = "";
	if(Level < sizeof(DebugLevelName) / sizeof(CHAR*))
		DLN = DebugLevelName[Level];
	else
		DLN = "-???-";
	
	DbgPrint("(%08x) %s-%s: ", PsGetCurrentThreadId(), DLN, DCN);
	return TRUE;
}

CHAR *FileDevTypeName[]={
	"Undef",
		"BEEP",
		"CD_ROM",
		"CD_ROM_FILE_SYSTEM",
		"CONTROLLER",
		"DATALINK",
		"DFS",
		"DISK",
		"DISK_FILE_SYSTEM",
		"FILE_SYSTEM",
		"INPORT_PORT",
		"KEYBOARD",
		"MAILSLOT",
		"MIDI_IN",
		"MIDI_OUT",
		"MOUSE",
		"MULTI_UNC_PROVIDER",
		"NAMED_PIPE",
		"NETWORK",
		"NETWORK_BROWSER",
		"NETWORK_FILE_SYSTEM",
		"NULL",
		"PARALLEL_PORT",
		"PHYSICAL_NETCARD",
		"PRINTER",
		"SCANNER",
		"SERIAL_MOUSE_PORT",
		"SERIAL_PORT",
		"SCREEN",
		"SOUND",
		"STREAMS",
		"TAPE",
		"TAPE_FILE_SYSTEM",
		"TRANSPORT",
		"UNKNOWN",
		"VIDEO",
		"VIRTUAL_DISK",
		"WAVE_IN",
		"WAVE_OUT",
		"8042_PORT",
		"NETWORK_REDIRECTOR",
		"BATTERY",
		"BUS_EXTENDER",
		"MODEM",
		"VDM",
		"MASS_STORAGE",
		"SMB",
		"KS",
		"CHANGER",
		"SMARTCARD",
		"ACPI",
		"DVD",
		"FULLSCREEN_VIDEO",
		"DFS_FILE_SYSTEM",
		"DFS_VOLUME",
		"SERENUM",
		"TERMSRV",
		"KSEC"
};

PCHAR DbgGetDevTypeName(ULONG type)
{
	if (type > sizeof(FileDevTypeName) / sizeof(CHAR*))
		return NullStr;
	
	return FileDevTypeName[type];
}
#endif //__DBG__
