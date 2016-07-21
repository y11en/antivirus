/*
 *  emulator.h,v 1.2 2002/10/17 09:55:02 tim 
 */

#ifndef CMD_EMULATOR_H
#define CMD_EMULATOR_H

#include "typedefs.h"

extern const char * EMULATOR_BUILD_DATETIME;


const int MAX_STACK_ENTRY = 10;
typedef struct _CWorkArea CWorkArea;

typedef int (*ELF_DECODE_FUNC) (void);
#define DECLARE_NATIVE_LINK(name)	WORD DECODE_LINK_##name (void)


class ElfBaseLoader;


class Emulator
{
	public:
		Emulator();
		~Emulator();
		void 	InitStack();
		void 	PushArg(DWORD arg);
		DWORD 	Execute (CWorkArea * wa, unsigned long addr, const char * fun);
		DWORD 	ExecuteVoidAddr (CWorkArea * wa, unsigned long addr);
		void 	ConvertWorkAreaToIntel();
		void 	ConvertWorkAreaToMachine();
		const char * InitFunctionPointers ();
		bool	LoadElfBases();
		bool	LinkElfBases();
        ELF_DECODE_FUNC 	LookupElfFunction(const char * name);
        void	AddElfFunction (const char * name, void * addr);
		BYTE *	AllocRegion (unsigned long length);
        
		SymbolTable pTable;
		I386CPU 	Cpu;
		DWORD		pfnGetArchOffs;	// must be public
		DWORD		pfnDoStdJump;
		DWORD		pfnIsProgram;
		DWORD		pfnGetFirstEntry;
		DWORD		pfnGetNextEntry;
		DWORD		pfnCureStdFile;
		DWORD		pfnCureStdSector;

	private:
		DWORD 			Execute (CWorkArea * wa, unsigned long addr);
		DWORD			CalcSum (unsigned long start, unsigned long length);
        CWorkArea	*	m_WA;
		bool 		  	fIntelBO; // true if WorkArea is in Intel byte order

#if defined (ELF_BASES_SUPPORT)
		typedef struct
		{
			std::string name;
			void	*	addr;
		} decode_entry_t;

		std::vector<decode_entry_t>	m_DecodeEntries;
        ElfBaseLoader	* pElfLoader;
#endif

#if defined (ACCOUNTING_LINK_EXECUTION_TIME)
		void	StartAccounting (const char * name);
		void	StopAccounting (const char * name);
		void	DumpAccountingInfo();

		typedef struct
		{
			struct timeval	start_time;
			float 			total_time;
			DWORD 			executed_times;
		} accountung_exec_time_t;

		time_t	m_tLastDump;
		std::map<std::string, accountung_exec_time_t>	m_AccountExecTime;
		typedef std::map<std::string, accountung_exec_time_t>::iterator accexectime_iterator;
#endif
};

extern Emulator gEmul;

DWORD	i386_DoStdJump	(CWorkArea * wa, void * arg1, BYTE arg2, WORD arg3, WORD arg4, WORD arg5);
WORD	i386_IsProgram	(CWorkArea * wa, void * arg1, void * arg2);
BOOL	i386_GetFirstEntry (CWorkArea * wa);
BOOL	i386_GetNextEntry (CWorkArea * wa);
WORD	i386_CureStdFile (CWorkArea * wa, BYTE arg1, WORD arg2, WORD arg3, WORD arg4, WORD arg5, WORD arg6);
WORD	i386_CureStdSector (CWorkArea * wa, BYTE arg1, WORD arg2, WORD arg3, WORD arg4);
DWORD	i386_GetArchOffs (CWorkArea * wa);


#endif /* CMD_EMULATOR_H */
