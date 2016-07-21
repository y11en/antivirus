/*
 *  emulator.cpp,v 1.2 2002/10/17 09:55:02 tim 
 */

#include "typedefs.h"

// Special definitions to get Workarea macroses working
typedef void	VOID;
#include "../workarea.h"

const DWORD STACK_SIZE = 192 * 1024;	// 192Kb
const int DUMP_LINK_INTERVAL = 180;	// seconds


/* A global object */
Emulator gEmul;


Emulator::Emulator()
{
	Cpu.Init (&pTable);
   	Cpu.m_dwStackTop = (DWORD) AllocRegion(STACK_SIZE) + STACK_SIZE;
    fIntelBO = false;

	pfnDoStdJump = 0;
	pfnIsProgram = 0;
	pfnGetFirstEntry = 0;
	pfnGetNextEntry = 0;
	pfnCureStdFile = 0;
	pfnCureStdSector = 0;
	pfnGetArchOffs = 0;

    wrappers_init(&pTable);

	/* VM self-test */
	if ( sizeof(BYTE) != 1 || sizeof(WORD) != 2 || sizeof(DWORD) != 4 || sizeof(QWORD) != 8 )
	{
		printf ("Emulator fatal error: bad type sizes\n");
		exit (1);
	}

	BYTE dw_data[] = "\xFF\xDD\xEE\xCC";
	BYTE w_data[] = "\xAA\x55";

	if ( ::ReadDWordPtr((DWORD*) dw_data) != 0xCCEEDDFF
	|| ::ReadWordPtr((WORD*) w_data) != 0x55AA )
	{
		printf ("Emulator fatal error: incorrect byte order\n");
		exit(1);
	}

    ::WriteDWordPtr((DWORD*) dw_data, 0x12345678);

#if defined (MACHINE_IS_BIG_ENDIAN)
	if ( ::ReadDWordPtr((DWORD*) dw_data) != 0x12345678
	|| *(DWORD*) dw_data != 0x78563412 )
	{
		printf ("Emulator fatal error: incorrect byte order/\n");
		exit(1);
	}
#endif

#if defined (ELF_BASES_SUPPORT)
    pElfLoader = 0;
#endif

#if defined (ACCOUNTING_LINK_EXECUTION_TIME)
	m_tLastDump = 0;
#endif
}


Emulator::~Emulator()
{
#if defined (ACCOUNTING_LINK_EXECUTION_TIME)
	DumpAccountingInfo();
#endif
}


void Emulator::InitStack()
{
	WRITE_X86_DWORD (Cpu.m_Regs.GetReg32(ESP), Cpu.m_dwStackTop);
}


void Emulator::PushArg (DWORD arg)
{
	DWORD * stacktop = (DWORD *) Cpu.m_Regs.GetStackTop() - 1;
    WRITE_X86_DWORD (stacktop, arg);
    Cpu.m_Regs.SetStackTop (stacktop);
}


DWORD Emulator::Execute (CWorkArea * wa, unsigned long addr, const char * fun)
{
#if defined (ENABLE_DISASM)
	pDisasm.LogAlways ("***** Start emulation: %s\n", fun);
#endif /* defined (ENABLE_DISASM) */

#if defined (ACCOUNTING_LINK_EXECUTION_TIME)
	if ( fun )
		StartAccounting (fun);
#endif /* defined (ACCOUNTING_LINK_EXECUTION_TIME) */

   	DWORD retval = Execute (wa, addr);

#if defined (ACCOUNTING_LINK_EXECUTION_TIME)
	if ( fun )
		StopAccounting (fun);

	time_t curr = time(0);

	if ( curr - m_tLastDump > DUMP_LINK_INTERVAL )
	{
		DumpAccountingInfo();
		m_tLastDump = curr;
	}
#endif /* defined (ACCOUNTING_LINK_EXECUTION_TIME) */

#if defined (ENABLE_DISASM)
	pDisasm.LogAlways ("\n***** End emulation: %s, return %08X\n\n", fun, retval);
#endif /* defined (ENABLE_DISASM) */

	return retval;
}


DWORD Emulator::ExecuteVoidAddr (CWorkArea * wa, unsigned long addr)
{
#if defined (ENABLE_DISASM)
	pDisasm.Log ("***** Start emulation: %08X\n", addr);
	DWORD retval = Execute (wa, addr);
	pDisasm.Log ("\n***** End emulation: %08X, return %08X\n\n", addr, retval);
    return retval;
#else
	return Execute (wa, addr);
#endif
}


DWORD Emulator::Execute (CWorkArea * wa, unsigned long addr)
{
	DWORD result = 0;
	m_WA = wa;

	gEmul.PushArg ((DWORD) 0);	// push return address
    ConvertWorkAreaToIntel();

    try
    {
	    Cpu.Execute ((BYTE *) addr);
        result = ReadDWordPtr(Cpu.m_Regs.GetReg32(EAX));
	}
	catch ( ... )
	{
		abort();
	}

    ConvertWorkAreaToMachine();
	return result;
}


void Emulator::ConvertWorkAreaToIntel()
{
	if ( fIntelBO )
		abort();

	fIntelBO = true;

    m_WA->WAs.SType = MachineToLittleEndian (m_WA->WAs.SType);
	m_WA->AFlags = MachineToLittleEndian (m_WA->AFlags);
	m_WA->Disk_Max_CX = MachineToLittleEndian (m_WA->Disk_Max_CX);
	m_WA->RFlags = MachineToLittleEndian (m_WA->RFlags);
    m_WA->Length = MachineToLittleEndian (m_WA->Length);
    m_WA->AreaOwner = MachineToLittleEndian (m_WA->AreaOwner);
    m_WA->P_N = MachineToLittleEndian (m_WA->P_N);
	m_WA->Name = MachineToLittleEndian (m_WA->Name);
	m_WA->Ext = MachineToLittleEndian (m_WA->Ext);
}

void Emulator::ConvertWorkAreaToMachine()
{
	if ( !fIntelBO )
		abort();

	fIntelBO = false;

    m_WA->WAs.SType = LittleEndianToMachine (m_WA->WAs.SType);
	m_WA->AFlags = LittleEndianToMachine (m_WA->AFlags);
	m_WA->Disk_Max_CX = LittleEndianToMachine (m_WA->Disk_Max_CX);

	m_WA->RFlags = LittleEndianToMachine (m_WA->RFlags);
    m_WA->Length = LittleEndianToMachine (m_WA->Length);
    m_WA->AreaOwner = LittleEndianToMachine (m_WA->AreaOwner);
    m_WA->P_N = LittleEndianToMachine (m_WA->P_N);
	m_WA->Name = LittleEndianToMachine (m_WA->Name);
	m_WA->Ext = LittleEndianToMachine (m_WA->Ext);
}


DWORD i386_DoStdJump (CWorkArea * bw, void * arg1, BYTE arg2, WORD arg3, WORD arg4, WORD arg5)
{
	gEmul.InitStack();
	gEmul.PushArg ((DWORD) arg5);
	gEmul.PushArg ((DWORD) arg4);
	gEmul.PushArg ((DWORD) arg3);
	gEmul.PushArg ((DWORD) arg2);
	gEmul.PushArg ((DWORD) arg1);

	return gEmul.Execute(bw, gEmul.pfnDoStdJump, "_DoStdJump");
}

WORD i386_IsProgram (CWorkArea * bw, void * arg1, void * arg2)
{
	gEmul.InitStack();
	gEmul.PushArg ((DWORD) arg2);
	gEmul.PushArg ((DWORD) arg1);

	return gEmul.Execute(bw, gEmul.pfnIsProgram, "_IsProgram");
}

BOOL i386_GetFirstEntry(CWorkArea * bw)
{
	gEmul.InitStack();

	return gEmul.Execute(bw, gEmul.pfnGetFirstEntry, "_GetFirstEntry");
}

BOOL i386_GetNextEntry (CWorkArea * bw)
{
	gEmul.InitStack();

	return gEmul.Execute(bw, gEmul.pfnGetNextEntry, "_GetNextEntry");
}

WORD i386_CureStdFile (CWorkArea * bw, BYTE arg1, WORD arg2, WORD arg3, WORD arg4, WORD arg5, WORD arg6)
{
	gEmul.InitStack();
	gEmul.PushArg ((DWORD) arg6);
	gEmul.PushArg ((DWORD) arg5);
	gEmul.PushArg ((DWORD) arg4);
	gEmul.PushArg ((DWORD) arg3);
	gEmul.PushArg ((DWORD) arg2);
	gEmul.PushArg ((DWORD) arg1);

    return gEmul.Execute(bw, gEmul.pfnCureStdFile, "_CureStdFile");
}

WORD i386_CureStdSector (CWorkArea * bw, BYTE arg1, WORD arg2, WORD arg3, WORD arg4)
{
	gEmul.InitStack();
	gEmul.PushArg ((DWORD) arg4);
	gEmul.PushArg ((DWORD) arg3);
	gEmul.PushArg ((DWORD) arg2);
	gEmul.PushArg ((DWORD) arg1);

	return gEmul.Execute(bw, gEmul.pfnCureStdSector, "_CureStdSector");
}

DWORD i386_GetArchOffs(CWorkArea * bw)
{
	gEmul.InitStack();

	return gEmul.Execute(bw, gEmul.pfnGetArchOffs, "_GetArchOffs");
}


const char * Emulator::InitFunctionPointers ()
{
	const symbol_t	* sym;

	if ( (sym = pTable.SymLookup("_DoStdJump", SYM_CPU_X86)) == 0 )
		return "_DoStdJump";
	else
		pfnDoStdJump = sym->addr;

	if ( (sym = pTable.SymLookup("_IsProgram", SYM_CPU_X86)) == 0 )
		return "_IsProgram";
	else
		pfnIsProgram = sym->addr;

	if ( (sym = pTable.SymLookup("_GetFirstEntry", SYM_CPU_X86)) == 0 )
		return "_GetFirstEntry";
	else
		gEmul.pfnGetFirstEntry = sym->addr;

	if ( (sym = pTable.SymLookup("_GetNextEntry", SYM_CPU_X86)) == 0 )
		return "_GetNextEntry";
	else
		gEmul.pfnGetNextEntry = sym->addr;

	if ( (sym = pTable.SymLookup("_CureStdFile", SYM_CPU_X86)) == 0 )
		return "_CureStdFile";
	else
		gEmul.pfnCureStdFile = sym->addr;

	if ( (sym = pTable.SymLookup("_CureStdSector", SYM_CPU_X86)) == 0 )
		return "_CureStdSector";
	else
		gEmul.pfnCureStdSector = sym->addr;

	if ( (sym = pTable.SymLookup("_GetArchOffs", SYM_CPU_X86)) != 0 )
		gEmul.pfnGetArchOffs = sym->addr;

	return 0;
}


BYTE * Emulator::AllocRegion (unsigned long length)
{
	if ( (length & 3) != 0 )
		length += 4 - (length & 3);	// size is rounded to DWORD boundary

	BYTE * ptr = new BYTE [length];
	memset (ptr, 0, length);

    return ptr;
}


#if !defined (ELF_BASES_SUPPORT)
bool Emulator::LoadElfBases()
{
	return true;
}

bool Emulator::LinkElfBases()
{
	return true;
}

ELF_DECODE_FUNC Emulator::LookupElfFunction (const char * path)
{
	return 0;
}

void Emulator::AddElfFunction (const char * name, void * addr)
{
}

#else /* defined (ELF_BASES_SUPPORT) */


bool Emulator::LoadElfBases()
{
    pElfLoader = new ElfBaseLoader;

    if ( !pElfLoader->LoadElfBases(&pTable) )
    {
    	printf ("%s\n", pElfLoader->GetLastError());
        printf ("Please update the antivirus package.\n");
    	delete pElfLoader;
        pElfLoader = 0;
    	return true;
    }

    return true;
}


bool Emulator::LinkElfBases()
{
	if ( pElfLoader && !pElfLoader->LinkElfBases(&pTable) )
	{
    	printf ("%s\n", pElfLoader->GetLastError());
        printf ("Please update the antivirus bases.\n");

        // Remove all the links from symbols
		m_DecodeEntries.clear();
    	return true;
    }

	return true;
}


//#define DEBUG_ELF

ELF_DECODE_FUNC Emulator::LookupElfFunction (const char * name)
{
	for ( unsigned int i = 0; i < m_DecodeEntries.size(); i++ )
		if ( m_DecodeEntries[i].name == (std::string) name )
		{
#if defined (DEBUG_ELF)
			printf ("LookupElfFunction: %s returns %08X\n", name, m_DecodeEntries[i].addr);
#endif
			return (ELF_DECODE_FUNC) m_DecodeEntries[i].addr;
		}

#if defined (DEBUG_ELF)
	printf ("LookupElfFunction: %s returns 0\n", name);
#endif
	return 0;
}


void Emulator::AddElfFunction (const char * name, void * addr)
{
	decode_entry_t entry;

	if ( LookupElfFunction(name) )
	{
		printf ("ERROR: ELF link %s is duplicated!\n", name);
		return;
	}

    entry.name = name;
    entry.addr = addr;

	m_DecodeEntries.push_back (entry);

#if defined (DEBUG_ELF)
	printf ("AddElfFunction: %s %08X\n", name, addr);
#endif
}

#endif /* defined (ELF_BASES_SUPPORT) */



#if defined (ACCOUNTING_LINK_EXECUTION_TIME)
static inline unsigned long usecdiff (struct timeval *a, struct timeval *b)
{
	unsigned long s;
	s = b->tv_sec - a->tv_sec;
	s *= 1000000;
	s += b->tv_usec - a->tv_usec;
	return s;
}


void Emulator::StartAccounting (const char * name)
{
	gettimeofday (&(m_AccountExecTime[name].start_time), NULL);
    m_AccountExecTime[name].executed_times++;
}


void Emulator::StopAccounting (const char * name)
{
	struct timeval endtime;
	gettimeofday (&endtime, NULL);

	m_AccountExecTime[name].total_time += (float) usecdiff(&(m_AccountExecTime[name].start_time), &endtime) / 1000000.0;
}


static const char * STATISTIC_FILENAME = "/tmp/KasperskyAntivirusExecStats";

void Emulator::DumpAccountingInfo ()
{
	char buf[512], *p;
	int executed, haveread = 0;
	float total;
	time_t tm = time(0);
	FILE * fp;
	
	if ( (fp = fopen (STATISTIC_FILENAME, "r")) != 0 )
	{
		// Read the old file content
		while ( fgets(buf, sizeof(buf), fp) != NULL )
		{
			if ( buf[0] != '{' || (p = strchr(buf, '}')) == 0 )
				continue;

			*p++ = '\0';	// remove last }

			if ( sscanf(p, "%d %f ", &executed, &total) != 2 )
				continue;

			haveread++;
			m_AccountExecTime[buf+1].total_time += total;
			m_AccountExecTime[buf+1].executed_times += executed;
		}

		fclose (fp);
	}

	fp = fopen (STATISTIC_FILENAME, "w");

	if ( !fp )
		return;

	fprintf (fp, "%d links have been executed (%d read)\n", m_AccountExecTime.size(), haveread);
	fprintf (fp, "Last update: %s\n", ctime(&tm));

	for (accexectime_iterator it = m_AccountExecTime.begin(); it != m_AccountExecTime.end(); it++)
	{
		DWORD average_time = 0, total_time = (DWORD) (*it).second.total_time;

		if ( (*it).second.executed_times > 0 )
			average_time = total_time / (*it).second.executed_times;

		fprintf (fp, "{%s} %d %f %d:%02d:%02d %d:%02d:%02d\n",
				(*it).first.c_str(),
                (*it).second.executed_times,
                (*it).second.total_time,
				total_time / 3600,
				(total_time / 60) % 60,
				total_time % 60,
				average_time / 3600,
				(average_time / 60) % 60,
				average_time % 60);

		(*it).second.total_time = 0;
		(*it).second.executed_times = 0;
	}

	fclose (fp);
    m_AccountExecTime.clear();
}

#endif /* defined (ACCOUNTING_LINK_EXECUTION_TIME) */
