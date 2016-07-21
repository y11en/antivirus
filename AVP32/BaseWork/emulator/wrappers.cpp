/*
 *  wrappers.cpp,v 1.2 2002/10/17 09:55:03 tim 
 */

#include "typedefs.h"


/*
 * Keeps wrappers list, (list of functions called by anti-viral bases).
 * List could be either static (no ELF bases), or dynamic (with additional bases).
 * This list is used during link calling (from BaseWork.cpp), and symbol 
 * table resolving.
 */
static std::vector<wrapper_t> vecWrappers;


/*
 * This area is created as emulation of kernel functions. For every function
 * we create a 3-byte sequence (HLT <functionid> <RETN>), and add the wrapper's
 * address to the symbol table. Next, HLT cmd parser will fetch next byte and 
 * call the appropriate engine function. After return, "RETN" will be called,
 * and the execution flow will continue.
 */
static char * wrapper_function_emulation_area = 0;


/*
const unsigned int STACK_SIZE = 65535;

extern "C" int ExecuteAddr (const char * name, unsigned int count, ... )
{
	I386CPU Cpu;
   	DWORD result;
	va_list marker;
	va_start (marker, count);
	char * stackbuf = new char [STACK_SIZE];

    Cpu.Init (&gEmul.pTable);
   	Cpu.m_dwStackTop = (DWORD) stackbuf + STACK_SIZE;
	WRITE_X86_DWORD (Cpu.m_Regs.GetReg32(ESP), Cpu.m_dwStackTop);

    DWORD * stacktop = (DWORD *) Cpu.m_Regs.GetStackTop();

    while ( count > 0 )
    {
    	stacktop--;
        count--;
		WRITE_X86_DWORD (stacktop, (DWORD) va_arg (marker, DWORD));
    }

    // Store return address
    WRITE_X86_DWORD (--stacktop, (DWORD) 0);
    Cpu.m_Regs.SetStackTop (stacktop);

	const symbol_t * sym = gEmul.pTable.SymLookup(name, SYM_CPU_X86);

	if ( !sym )
	{
		Cpu.fatal (31, "No %s() symbol found in the symbol table!", name);
		abort();
	}

	gEmul.ConvertWorkAreaToIntel();

    try
    {
		//printf ("Execute: %08X\n", sym->addr);
	    Cpu.Execute ((BYTE *) sym->addr);
        result = ReadDWordPtr(Cpu.m_Regs.GetReg32(EAX));
	}
	catch ( ... )
	{
		abort();
	}

    gEmul.ConvertWorkAreaToMachine();
    delete stackbuf;
	return result;
}
*/


// Engine functions
class CBaseWork;

extern "C" unsigned long CalcSum( unsigned char* mass,unsigned long len);
extern void Printf 		(const char * fmt, ...);
extern DWORD _DoJump	(CBaseWork* bw, BYTE* Page);
extern DWORD _Read 		(CBaseWork* bw, void* lpBuf, unsigned int nCount);
extern DWORD _Write		(CBaseWork* bw, const void* lpBuf, unsigned int nCount );
extern DWORD _Seek		(CBaseWork* bw, long lOff);
extern int _ChSize		(CBaseWork* bw, unsigned int len);
extern BOOL _Read13 	(CBaseWork * bw, BYTE Disk, WORD Sector, BYTE Head, WORD NumSectors, unsigned char * Buffer);
extern BOOL _Write13	(CBaseWork* bw, BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, unsigned char * Buffer);
extern BOOL _Read25		(CBaseWork* bw, BYTE Drive, unsigned int Sector, WORD NumSectors, unsigned char * Buffer);
extern BOOL _Write26	(CBaseWork* bw, BYTE Drive, unsigned int Sector, WORD NumSectors, unsigned char * Buffer);
extern DWORD _LSeek		(CBaseWork* bw, long offset, int from);
extern int _Copy_P_File	(CBaseWork* bw);
extern int _P_Read		(CBaseWork* bw, BYTE* buff, WORD size);
extern long _P_Seek 	(CBaseWork* bw, long offset, int from);
extern int _V_Read		(CBaseWork* bw, BYTE * buff,WORD size);
extern int _V_Write		(CBaseWork* bw, BYTE * buff,WORD nCount);
extern long _V_Seek		(CBaseWork* bw, long offset,int from);
extern int _S_Save		(CBaseWork* bw, char * buff, long size);
extern int _S_Load		(CBaseWork* bw, char * buff);
extern int _CheckExt	(CBaseWork* bw);
extern WORD _Rotate		(CBaseWork* bw, char flag);
extern BOOL _NextVolume	(CBaseWork* bw);
extern BYTE * _New		(long size);
extern void _Delete		(BYTE* ptr);
extern void * Get_Sector_Image (BYTE SubType);
extern DWORD Peek		(WORD seg, WORD off);
BOOL Poke				(WORD seg, WORD off, unsigned int value, BYTE size);
extern BOOL SavePages	(CBaseWork* bw);

int	 Copy_P_File();
extern long LSeek (long offset,int from);
extern int	P_Read(char * buff,WORD size);
extern long P_Seek(long offset,int from);
extern int	V_Read(char * buff,WORD size);
extern int	V_Write(char * buff,WORD size);
extern long V_Seek(long offset,int from);
extern long S_Save(char * buff, long size);
extern long S_Load(char * buff);
extern WORD Rotate(char flag);


static wrapper_t wrapped_funcs[] = 
{
	{	"_Printf",		(void *) Printf,		-1,	WRAPPER_NATIVE	},
	{	"_Check_Sum",	(void *) CalcSum,		2,	WRAPPER_NATIVE	},
	{	"__DoJump",		(void *) _DoJump,		2,	WRAPPER_NATIVE	},
	{	"__Read",		(void *) _Read,			3,	WRAPPER_NATIVE	},
	{	"__Write",		(void *) _Write,		3,	WRAPPER_NATIVE	},
	{	"__Seek",		(void *) _Seek,			2,	WRAPPER_NATIVE	},
	{	"__Ch_Size",	(void *) _ChSize,		2,	WRAPPER_NATIVE	},
	{	"__Read_13",	(void *) _Read13,		6,	WRAPPER_NATIVE	},
	{	"__Write_13",	(void *) _Write13,		6,	WRAPPER_NATIVE	},
	{	"__Read_25",	(void *) _Read25,		5,	WRAPPER_NATIVE	},
	{	"__Write_26",	(void *) _Write26,		5,	WRAPPER_NATIVE	},
	{	"__LSeek",		(void *) _LSeek,		3,	WRAPPER_NATIVE	},
	{	"__Copy_P_File",(void *) _Copy_P_File,	1,	WRAPPER_NATIVE	},
	{	"__P_Read",		(void *) _P_Read,		3,	WRAPPER_NATIVE	},
	{	"__P_Seek",		(void *) _P_Seek,		3,	WRAPPER_NATIVE	},
	{	"__V_Read",		(void *) _V_Read,		3,	WRAPPER_NATIVE	},
	{	"__V_Write",	(void *) _V_Write,		3,	WRAPPER_NATIVE	},
	{	"__V_Seek",		(void *) _V_Seek,		3,	WRAPPER_NATIVE	},
	{	"__S_Save",		(void *) _S_Save,		3,	WRAPPER_NATIVE	},
	{	"__S_Load",		(void *) _S_Load,		2,	WRAPPER_NATIVE	},
	{	"__CheckExt",	(void *) _CheckExt,		1,	WRAPPER_NATIVE	},
	{	"__Rotate",		(void *) _Rotate,		2,	WRAPPER_NATIVE	},
	{	"__NextVolume",	(void *) _NextVolume,	1,	WRAPPER_NATIVE	},
	{	"_New",			(void *) _New,			1,	WRAPPER_NATIVE	},
	{	"_Delete",		(void *) _Delete,		1,	WRAPPER_NATIVE	},
	{	"_Peek",		(void *) Peek,			2,	WRAPPER_NATIVE	},
	{	"_Poke",		(void *) Poke,			4,	WRAPPER_NATIVE	},
	{	"_SavePages",	(void *) SavePages,		1,	WRAPPER_NATIVE	},
	{	"_Get_Sector_Image", (void *) Get_Sector_Image,	1,	WRAPPER_NATIVE	},

	{	"__mem_cpy",	(void *) memcpy,		0,	WRAPPER_INTEL	},
	{	"__mem_set",	(void *) memset,		0,	WRAPPER_INTEL	},
	{	"_New",			(void *) _New,			1,	WRAPPER_INTEL	},
	{	"_Delete",		(void *) _Delete,		1,	WRAPPER_INTEL	},

	{	0,				0,						0,	WRAPPER_INTEL	}
};


/*
 * __cdecl is the default calling convention for C and C++ programs.
 * Because the stack is cleaned up by the caller, it can do vararg functions.
 * The following list shows the implementation of this calling convention.
 * - Arguments are passing from right to left (first argument is on the stack top)
 * - Calling function pops the arguments from the stack (actually peeks)
 * - All arguments <= DWORD are converted to DWORD, > - to DOUBLE
 * - Underscore character (_) is prefixed to names 
 * - No case translation performed 
 */

#include "emulator.h"


void wrapper_add (const char * name, void * wrapaddr, int count, wrapper_lookup_t type)
{
	wrapper_t wr;

	wr.name = strdup(name);
	wr.addr = wrapaddr;
	wr.count = count;
	wr.type = type;
	vecWrappers.push_back(wr);
}


void wrappers_init(SymbolTable * pTable)
{
	const unsigned WRAPPER_AREA_SIZE = sizeof(wrapped_funcs)/sizeof(wrapped_funcs[0]);
	wrapper_function_emulation_area = new char[WRAPPER_AREA_SIZE * 3];
	memset (wrapper_function_emulation_area, 0xCC, WRAPPER_AREA_SIZE * 3);
	unsigned int count = 0;
		
	for ( wrapper_t * ptr = wrapped_funcs; ptr->name; ptr++, count++ )
	{
		if ( ptr->type == WRAPPER_NATIVE )
		{
			unsigned int offset = count * 3;
			wrapper_function_emulation_area[offset] = 0xF4;
			wrapper_function_emulation_area[offset+1] = count;
			wrapper_function_emulation_area[offset+2] = 0xC3;
			
			pTable->AddSymbol (ptr->name, (DWORD) (wrapper_function_emulation_area + offset), SYM_CPU_X86);
		}
		else
		{
			wrapper_add (ptr->name, ptr->addr, ptr->count, ptr->type);
		}
	}
}


wrapper_t * wrapper_lookup (const char * name, wrapper_lookup_t type)
{
	for ( unsigned int i = 0; i < vecWrappers.size(); i++ )
	{
		if ( name[0] == vecWrappers[i].name[0]
		&& !strcmp(name, vecWrappers[i].name) 
		&& vecWrappers[i].type == type )
			return &vecWrappers[i];
	}
	return 0;
}


wrapper_t * wrapper_lookup (DWORD addr, wrapper_lookup_t type)
{
	for ( unsigned int i = 0; i < vecWrappers.size(); i++ )
		if ( addr == (DWORD) vecWrappers[i].addr && vecWrappers[i].type == type )
			return &vecWrappers[i];

	return 0;
}


/*
 * Assembler-to-Engine function wrappers.
 * These functions are called by Intel CPU machine code, we have to transfer
 * it to engine.
 */

void I386CPU::WrapperIntelToSparc (wrapper_t * ptr)
{
	DWORD retvalue, count = 0;

	if ( ptr->count == -1 )
	{
		const char * str = (char *) PEEK_STACK_VALUE(0);
        count++;

		for ( const char * p = str; *p; p++ )
		{
			if ( *p != '%' )
				continue;

			if ( p[1] == '%' )
				p++;
			else
				count++;
		}
	}
	else
		count = ptr->count;

	// Call the function
	gEmul.ConvertWorkAreaToMachine();

	switch (count)
	{
	case 0:
		retvalue = ((DWORD (*)()) ptr->addr) ();
		break;

	case 1:
		retvalue = ((DWORD (*)(DWORD)) ptr->addr) (PEEK_STACK_VALUE(0));
		break;

	case 2:
		retvalue = ((DWORD (*)(DWORD, DWORD)) ptr->addr) (PEEK_STACK_VALUE(0), PEEK_STACK_VALUE(1));
		break;

	case 3:
		retvalue = ((DWORD (*)(DWORD, DWORD, DWORD)) ptr->addr)
			(PEEK_STACK_VALUE(0), PEEK_STACK_VALUE(1), PEEK_STACK_VALUE(2));
		break;

	case 4:
		retvalue = ((DWORD (*)(DWORD, DWORD, DWORD, DWORD)) ptr->addr)
			(PEEK_STACK_VALUE(0), PEEK_STACK_VALUE(1), PEEK_STACK_VALUE(2),
			PEEK_STACK_VALUE(3));
		break;

	case 5:
		retvalue = ((DWORD (*)(DWORD, DWORD, DWORD, DWORD, DWORD)) ptr->addr)
			(PEEK_STACK_VALUE(0), PEEK_STACK_VALUE(1), PEEK_STACK_VALUE(2),
			PEEK_STACK_VALUE(3), PEEK_STACK_VALUE(4));
		break;

	case 6:
		retvalue = ((DWORD (*)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)) ptr->addr)
			(PEEK_STACK_VALUE(0), PEEK_STACK_VALUE(1), PEEK_STACK_VALUE(2),
			PEEK_STACK_VALUE(3), PEEK_STACK_VALUE(4), PEEK_STACK_VALUE(5));
		break;

	case 7:
		retvalue = ((DWORD (*)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)) ptr->addr)
			(PEEK_STACK_VALUE(0), PEEK_STACK_VALUE(1), PEEK_STACK_VALUE(2),
			PEEK_STACK_VALUE(3), PEEK_STACK_VALUE(4), PEEK_STACK_VALUE(5),
			PEEK_STACK_VALUE(6));
		break;

	default:
		abort();
	}

	gEmul.ConvertWorkAreaToIntel();
	WriteDWordPtr (m_Regs.GetReg32(EAX), retvalue);
}


/*
 * Checks whether the destination address is an address of the internal 
 * function, and calls this function if it is. Returns true if CALL/JUMP 
 * is handled, otherwise returns false.
 */
void I386CPU::Cmd_wrapper (BYTE cmd)
{
	char buf[120];
	const unsigned WRAPPER_AREA_SIZE = sizeof(wrapped_funcs)/sizeof(wrapped_funcs[0]);
	BYTE idx = ReadIx86Byte();

	if ( idx >= WRAPPER_AREA_SIZE )
		abort();
	
	wrapper_t * ptr = wrapped_funcs + idx;
	
    if ( ptr->type != WRAPPER_NATIVE )
    	abort();

   	if ( m_dwNestLevel >= MAX_NESTED_CALLS )
   	{
   		pDisasm.Log ("%08X:  %s: nested call buffer overflow\n", m_dwOldEIP, ptr->name);
   		abort();
   	}

   	// hey, it is an external wrapped func
#if defined (ENABLE_DISASM)
	pDisasm.Log ("%08X:  NATIVE CALL: %s [%d]\n", m_dwOldEIP, ptr->name, m_dwNestLevel);
#endif

	// Store ESP and EIP
	m_dwNested_Eip[m_dwNestLevel] = m_Regs.eip;
	m_dwNested_OldEip[m_dwNestLevel] = m_dwOldEIP;
	m_dwNested_Esp[m_dwNestLevel] = (DWORD) m_Regs.GetStackTop();
	m_dwNested_StackTop[m_dwNestLevel] = m_dwStackTop;
	m_dwNested_ExitEmul[m_dwNestLevel] = fExitEmul;
	m_dwStackTop = m_dwNested_Esp[m_dwNestLevel];
	m_dwNestLevel++;

	WrapperIntelToSparc (ptr);

	m_dwNestLevel--;
	m_Regs.eip = m_dwNested_Eip[m_dwNestLevel];
	m_dwOldEIP = m_dwNested_OldEip[m_dwNestLevel];
	m_dwStackTop = m_dwNested_StackTop[m_dwNestLevel];
	fExitEmul = m_dwNested_ExitEmul[m_dwNestLevel];
	m_Regs.SetStackTop((void*)m_dwNested_Esp[m_dwNestLevel]);

	sprintf (buf, "NATIVE CALL RETURNS: %s [%d]", ptr->name, m_dwNestLevel);
	DISASSEMBLE ((buf));
}
