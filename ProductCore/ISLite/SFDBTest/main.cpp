#define _WIN32_WINNT 0x0400

#include <prague.h>
#include <windows.h>
#include <stdio.h>

#include <pr_loadr.h>
#include <iface/e_loader.h>
#include <iface/i_root.h>
#include <iface/i_plugin.h>
#include <iface/i_sfdb.h>
#include <iface/i_ifilesecurestatus.h>


#include "../hash_api/md5/hash_md5.h"
#include "../hash_api/crapi/hash_cr.h"
#include "../hash_api/hash.h"

#include "debug.h"
#include "cio.h"
#include "console.h"
#include "timing.h"
#include "recurser2.h"

#include "O:/Prague/ISLite/SFDBTestDll/sfdbtest.h"
#include "O:/Prague/ISLite/ichecker/ifilesec.h"
#include "O:/Prague/ISLite/sfdb/sfdb.h"

#define SFDB_COUNT 10

hSECUREFILEDB hSFDB[SFDB_COUNT];
hIFILESECURESTATUS hChecker = NULL;
hROOT         hRoot = 0;

#define COMMAND(n, cmd, code) if (stricmp(args[n], cmd) == 0) code; else

int do_command(char* cmd, char** args, int argc);

int Help()
{
	//	OUTPUT(("ls     - load signatures from text file"));
	//	OUTPUT(("gs/as  - add signature to tree"));
	//	OUTPUT(("is     - check signature existing"));
	//	OUTPUT(("ds     - delete signature from tree"));
	//	OUTPUT(("lt     - load tree from file"));
	//	OUTPUT(("st     - save tree to file"));
	//	OUTPUT(("ct     - compact tree"));
	//	OUTPUT(("dt     - dump tree"));
	//	OUTPUT(("del    - delete tree"));
		OUTPUT(("dir    - dir"));
		OUTPUT(("dirs   - dir with subfolders"));
		OUTPUT(("scan   - scan files and add new of them into SFDB"));
		OUTPUT(("scans  - --\"-- with subfolders"));
		OUTPUT(("timing - enable/disable timing for operations"));
		OUTPUT(("tdb    - calculate timing for database operations (new/get/update/delete)"));
		OUTPUT(("cls    - clear screen"));
		OUTPUT(("quit(q)- quit"));
	return 1;
}

/*
int GetSignature(hSIGNATURESTORAGE hSigStorage, char* cmd, char** args, int argc)
{
	if (argc > 0)
	{
		int i;
		for (i=1; i<=argc; i++)
		{
			tERROR error = AddHexSignature(hSigStorage, (tBYTE*)args[i], -1, -1);
			if (PR_FAIL(error))
				ODS(("AddHexSignature returned error = 0x%08X", error));
		}
	}
	else
	{
		OUTPUT(("Enter signatures:"));
		if (GetMasks(hSigStorage) != errOK)
		{
			OUTPUT(("Error getting signatures"));
			return 1;
		}
	}
	return 0;
}

int IsSignature(hSIGNATURESTORAGE hSigStorage, char* cmd, char** args, int argc)
{
	if (argc > 0)
	{
		tSIGNATURE_ID SigID;
		tERROR error = IsHexSignature(hSigStorage, (tBYTE*)args[1], -1, &SigID);
		if (PR_FAIL(error))
		{
			if (error == errSIGNATURE_NOT_FOUND)
			{
				OUTPUT(("Signature not found"));
			}
			else
				OUTPUT(("IsSignature returned error = 0x%08X", error));
		}
		else
			OUTPUT(("Signature existing, ID=%08X", SigID));
	}
	else
	{
		OUTPUT(("Checks for signature existence"));
		OUTPUT(("usage: is <hex signature>"));
		return 1;
	}
	return 0;
}

int DeleteSignature(hSIGNATURESTORAGE hSigStorage, char* cmd, char** args, int argc)
{
	if (argc > 0)
	{
		tERROR error = DeleteHexSignature(hSigStorage, (tBYTE*)args[1], -1);
		if (PR_FAIL(error))
		{
			if (error == errSIGNATURE_NOT_FOUND)
			{
				OUTPUT(("Signature not found"));
			}
			else
				OUTPUT(("DeleteSignature returned error = 0x%08X", error));
		}
		else
			OUTPUT(("Signature deleted"));
	}
	else
	{
		OUTPUT(("Deletes existing signature"));
		OUTPUT(("usage: ds <hex signature>"));
		return 1;
	}
	return 0;
}

int LoadSignatures(hSIGNATURESTORAGE hSigStorage, char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Load signatures from file"));
		OUTPUT(("usage: ls <filename>"));
		return 1;
	}
	
	OUTPUT(("Loading signatures..."));
	if (LoadMasks(hSigStorage, args[1]) == -1)
	{
		OUTPUT(("Error loading signatures from '%s'", args[1]));
		return 1;
	}
	OUTPUT(("Signatures loaded."));
	return 0;
}

int LoadTree(hSIGNATURESTORAGE hSigStorage, char* cmd, char** args, int argc)
{
	do_command("del", NULL, 0);
	if (argc!=1)
	{
		OUTPUT(("Load tree from file"));
		OUTPUT(("usage: lt <filename>"));
		return 1;
	}
	
	OUTPUT(("Loading tree..."));
	tERROR err;
	hSEQ_IO hSeqIo;
	if (PR_FAIL(err = CreateSeqIO((hOBJECT)hRoot, &hSeqIo, args[1], fOMODE_OPEN_IF_EXIST, fACCESS_READ)))
	{
		ODS(("SaveTree: SeqIO create error 0x%08X", err));
		return 1;
	}
	err = CALL_SignatureStorage_Load(hSigStorage, hSeqIo);
	if (PR_FAIL(err))
	{
		ODS(("LoadTree: Tree load error 0x%08X", err));
		return 1;
	}
	CloseSeqIO(hSeqIo);
	OUTPUT(("Tree loaded."));
	return 0;
}

int SaveTree(hSIGNATURESTORAGE hSigStorage, char* cmd, char** args, int argc)
{
	hSEQ_IO hSeqIo;
	
	if (argc!=1)
	{
		OUTPUT(("Save tree from file"));
		OUTPUT(("usage: st <filename>"));
		return 1;
	}
	OUTPUT(("Saving tree..."));
	tERROR err;
	
	if (PR_FAIL(err = CreateSeqIO((hOBJECT)hRoot, &hSeqIo, args[1], fOMODE_CREATE_ALWAYS | fOMODE_SHARE_DENY_WRITE | fOMODE_SHARE_DENY_DELETE, fACCESS_WRITE)))
	{
		ODS(("SaveTree: SeqIO create error 0x%08X", err));
		return 1;
	}
	
	err = CALL_SignatureStorage_Save(hSigStorage, hSeqIo);
	CloseSeqIO(hSeqIo);
	OUTPUT(("Tree saved."));
	return 0;
}

int DumpTree(hSIGNATURESTORAGE hSigStorage)
{
	OUTPUT(("Tree dump:"));
//	hi_SignatureStorage hiSignatureStorage = (hi_SignatureStorage)hSigStorage;
//	TreeDump(((hi_SignatureStorage)hSigStorage)->data->pRoot);
	return 0;
}

int DelTree(hSIGNATURESTORAGE hSigStorage)
{
	OUTPUT(("Deleting tree..."));
	tERROR error = CALL_SignatureStorage_Clear(hSigStorage);
	if (PR_FAIL(error))
		ODS(("SignatureStorage::Clear returned error = 0x%08X", error));
	OUTPUT(("Tree deleted."));
	return 1;
}


int ScanTest(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Scan files with subfolders for signatures"));
		OUTPUT(("usage: scans <path>"));
		return 1;
	}
	ScanDir(args[1], ProcessFileTest, FALSE);
	return 0;
}

int ScanTestWithSubDir(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Scan files with subfolders for signatures"));
		OUTPUT(("usage: scans <path>"));
		return 1;
	}
	ScanDir(args[1], ProcessFileTest, TRUE);
	return 0;
}
*/
int Scan(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Scan files for signatures"));
		OUTPUT(("usage: scan <path>"));
		return 1;
	}
	ScanDir(args[1], ProcessFileScan, FALSE);
	return 0;
}

int ScanWithSubDir(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Scan files with subfolders for signatures"));
		OUTPUT(("usage: scans <path>"));
		return 1;
	}
	ScanDir(args[1], ProcessFileScan, TRUE);
	return 0;
}

/*
int ScanWithSubDirAsAVP3(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Scan files with subfolders for signatures as AVP does"));
		OUTPUT(("usage: scansa <path>"));
		return 1;
	}
	ScanDir(args[1], ProcessFileScanAsAVP, TRUE);
	return 0;
}

int ConvertDatabase(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Load records from database to tree"));
		OUTPUT(("usage: db <path>"));
		return 1;
	}
	OUTPUT(("Loading database..."));
	
	if (!ConvertDB((hOBJECT)hRoot, args[1]))
	{
		OUTPUT(("Error loading database file '%s'", args[1]));
		return 1;
	}
	return 0;
}

int RunAVP4(char* cmd, char** args, int argc)
{
	if (argc!=1)
	{
		OUTPUT(("Tests AVP4 engine."));
		OUTPUT(("usage: a <database path>"));
		return 1;
	}
	OUTPUT(("Loading AVP4..."));
	
	hENGINE hAVP4;
	tERROR err;
	
	err = CALL_SYS_ObjectCreate(hRoot, &hAVP4, IID_ENGINE, PID_AVP4, SUBTYPE_ANY);
	if (PR_FAIL(err))
	{
		OUTPUT(("AVP4 failed on create with err=%08X.", err));
		return 1;
	}
	
	tCHAR szDBName[] = "db";
	err = CALL_SYS_PropertySetStr(hAVP4, NULL, plDB_LOAD_PATH, szDBName, strlen((char*)szDBName)+1, cCP_ANSI);
	if (PR_FAIL(err))
	{
		OUTPUT(("AVP4 failed on set property plDB_LOAD_PATH with err=%08X.", err));
		return 1;
	}
	
	err = CALL_SYS_ObjectCreateDone(hAVP4);
	if (PR_FAIL(err))
	{
		OUTPUT(("AVP4 failed on ObjectCreateDone with err=%08X.", err));
		return 1;
	}
	
	OUTPUT(("AVP4 loaded OK."));
	
	
	hIO hIo;
	err = CALL_SYS_ObjectCreate(hRoot, (hOBJECT*)&hIo, IID_IO, PID_WIN32_NFIO, 0);
	if (PR_SUCC(err))
		err = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
	if (PR_SUCC(err))
		err = CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_NAME, args[1], lstrlen(args[1]) + 1, cCP_ANSI);
	if (PR_SUCC(err))
		err = CALL_SYS_ObjectCreateDone(hIo);
	
	hOBJECT obj = (hOBJECT)hIo;
//	_bstr_t tmp = (_bstr_t )obj;
	if (PR_FAIL(err))
	{
		ODS(("IO create for file '%s', error 0x%08X", args[1], err));
	}
	else
	{
		OUTPUT(("Scanning IO with AVP4..."));
		err = CALL_Engine_Process(hAVP4, (hOBJECT)hIo);
		if (PR_FAIL(err))
		{
			ODS(("AVP4::Process failed with 0x%08X", err));
		}
		else
		{
			ODS(("AVP4::Process completed successfully with 0x%08X", err));
		}
	}
	
	
	err = CALL_SYS_ObjectClose(hAVP4);
	if (PR_FAIL(err))
	{
		OUTPUT(("AVP4 failed on close with err=%08X.", err));
		return 1;
	}
	
	OUTPUT(("AVP4 closed OK."));
	return 0;
}


int ProcessSQLCommandHelp()
{
	OUTPUT(("help        - sql commands help"));
	OUTPUT(("names       - dump records names"));
	OUTPUT(("recs        - dump records"));
	OUTPUT(("binrecs     - dump binary records"));
	OUTPUT(("detected    - dump records with detected names"));
	OUTPUT(("linkedrecs  - dump linked records"));
	OUTPUT(("src         - find appropriate sources for links"));
	OUTPUT(("src2db      - check signature existing"));
	return 0;
}

int ProcessSQLCommand(char* cmd, char** args, int argc)
{
	COMMAND(1, "", ProcessSQLCommandHelp());
	COMMAND(1, "?", ProcessSQLCommandHelp());
	COMMAND(1, "help", ProcessSQLCommandHelp());

	COMMAND(1, "src", FindLinksSources(args[2], FALSE, (hOBJECT)hRoot, args[3]));
	COMMAND(1, "src2db", FindLinksSources(args[2], TRUE, (hOBJECT)hRoot, args[3]));
	COMMAND(1, "linkedrecs", FindLinkDependedRecords());
	COMMAND(1, "names", DumpRecordsNames(args[2]));
	COMMAND(1, "recs", DumpRecords(args[2]));
	COMMAND(1, "binrecs", DumpBinaryRecords(args[2]));
	COMMAND(1, "detected", DumpRecordsNamesWithDetectedFiles(args[2]));
	return 0;
}
*/


#ifdef _DEBUG
#define TEST_RECORDS_COUNT 1000000
//#define TEST_RECORDS_COUNT 30000
#else
#define TEST_RECORDS_COUNT 1000000
#endif

extern void ByteToStr(DWORD cb, void* pv, LPSTR sz);

int TimeDB2(tDWORD dwMode, tDWORD n)
{
	LARGE_INTEGER t_start;
	LARGE_INTEGER t_end;
	LARGE_INTEGER t_total;

	tDWORD i;
	
	void* pMD5Ctx;
	void* pMD5Ctx2;
	char md5hash[16];
	DWORD dwHashSize;
	unsigned __int64 hash64;
	char sHashStr[0x100];
	tERROR error;
	tDWORD dwErrors = 0;
	tDWORD dwWarnings = 0;

	HASH_API* hash_api = (HASH_API*)&hash_api_md5;
	
	pMD5Ctx = malloc(hash_api->ContextSize);
	pMD5Ctx2 = malloc(hash_api->ContextSize);
	if (!hash_api->Init(CALG_MD5, pMD5Ctx, hash_api->ContextSize, 16, NULL))
	{
		ODS(("hash_api->Init failed!"));
		return 0;
	}
	
	OUTPUT((""));
	switch (dwMode)
	{
	case 0:
		OUTPUT(("Mode: GetRecord"));
		break;
	case 1:
		OUTPUT(("Mode: UpdateRecord"));
		break;
	case 2:
		OUTPUT(("Mode: DeleteRecord"));
		break;
	}

	t_total.QuadPart = 0;
	for (i=0;i<TEST_RECORDS_COUNT; i++)
	{
		if (!hash_api->Update(pMD5Ctx, &i, 1))
		{
			ODS(("hash_api->Update failed!"));
			break;
		}
		memcpy(pMD5Ctx2, pMD5Ctx, hash_api->ContextSize);
		dwHashSize = countof(md5hash);
		if (!hash_api->GetCurrentHash(pMD5Ctx2, &dwHashSize, md5hash))
		{
			ODS(("hash_api->Finalize failed!"));
		}
		else
		{
			//ByteToStr(dwHashSize, md5hash, sHashStr);
			//OUTPUT(("Hash: %s", sHashStr));
		}

		hash64 =  *(unsigned __int64*)&md5hash[0];
		hash64 ^= *(unsigned __int64*)&md5hash[8];

		switch (dwMode)
		{
		case 0:
			QueryPerformanceCounter(&t_start);
			error = CALL_SecureFileDatabase_GetRecord(hSFDB[n], &hash64, &md5hash);
			QueryPerformanceCounter(&t_end);
			break;
		case 1:
			QueryPerformanceCounter(&t_start);
			error = CALL_SecureFileDatabase_UpdateRecord(hSFDB[n], &hash64, &md5hash);
			QueryPerformanceCounter(&t_end);
			break;
		case 2:
			QueryPerformanceCounter(&t_start);
			error = CALL_SecureFileDatabase_DeleteRecord(hSFDB[n], &hash64);
			QueryPerformanceCounter(&t_end);
			break;
		}
		if (PR_SUCC(error) && error!=errOK)
		{
			dwWarnings++;
			//ODS(("Warning %08X", error));
		}
		if (PR_FAIL(error))
		{
			dwErrors++;
			//ODS(("Failed with %08X", error));
		}
		t_total.QuadPart += t_end.QuadPart - t_start.QuadPart;
	}
	if (dwWarnings)
		OUTPUT(("Warnings: %d", dwWarnings));
	if (dwErrors)
		OUTPUT(("Errors: %d", dwErrors));

	QueryPerformanceFrequency(&t_end);
	OUTPUT(("Time elapsed: %f", (float)t_total.QuadPart / (float)t_end.QuadPart));
		
	free(pMD5Ctx);
	free(pMD5Ctx2);
	return 0;
}

void TimeSave(char* szFileName, tDWORD dwSerializeOptions, tDWORD n)
{
	LARGE_INTEGER t_start;
	LARGE_INTEGER t_end;
	LARGE_INTEGER t_total;
	tERROR error;
		
	OUTPUT(("\nSaving %s...", szFileName));
	t_total.QuadPart = 0;
	QueryPerformanceCounter(&t_start);
	if (PR_FAIL(error = CALL_SYS_PropertySetDWord(hSFDB[n], plSERIALIZE_OPTIONS, dwSerializeOptions)))
		OUTPUT(("CALL_SYS_PropertySetDWord(hSFDB, plSERIALIZE_OPTIONS) failed with %08X", error));
	error = CALL_SecureFileDatabase_Save(hSFDB[n], szFileName);
	QueryPerformanceCounter(&t_end);
	t_total.QuadPart += t_end.QuadPart - t_start.QuadPart;
				
	if (PR_FAIL(error))
		OUTPUT(("SecureFileDatabase_Save failed with %08X", error));

	QueryPerformanceFrequency(&t_end);
	OUTPUT(("Time elapsed: %f", (float)t_total.QuadPart / (float)t_end.QuadPart));
}

void TimeLoad(char* szFileName, tDWORD n)
{
	LARGE_INTEGER t_start;
	LARGE_INTEGER t_end;
	LARGE_INTEGER t_total;
	tERROR error;
	
	OUTPUT(("\nLoading %s...", szFileName));
	t_total.QuadPart = 0;
	QueryPerformanceCounter(&t_start);
	error = CALL_SecureFileDatabase_Load(hSFDB[n], szFileName);
	QueryPerformanceCounter(&t_end);
	t_total.QuadPart += t_end.QuadPart - t_start.QuadPart;
				
	if (PR_FAIL(error))
		OUTPUT(("SecureFileDatabase_Load failed with %08X", error));
	
	QueryPerformanceFrequency(&t_end);
	OUTPUT(("Time elapsed: %f", (float)t_total.QuadPart / (float)t_end.QuadPart));
}

int TimeDB()
{
	char szFileName[MAX_PATH];
	LARGE_INTEGER t_start;
	LARGE_INTEGER t_end;
	LARGE_INTEGER t_total;
	tDWORD i;

	GetTempPath(countof(szFileName), szFileName);
	strcat(szFileName, "saved.dat");

	OUTPUT(("Records count: %d", TEST_RECORDS_COUNT));

	TimeDB2(1, 0);
	TimeDB2(0, 0);
	TimeDB2(1, 0);
	TimeDB2(2, 0);
	TimeDB2(0, 0);
	TimeSave(szFileName, 0, 0);
	TimeLoad(szFileName, 0);
	TimeDB2(1, 0);
	strcat(szFileName, "2");
	TimeSave(szFileName, 0, 0);
	TimeLoad(szFileName, 0);
	strcat(szFileName, ".crypt");
	TimeSave(szFileName, cSFDB_SERIALIZE_CHECKSUM | cSFDB_SERIALIZE_CRYPT | cSFDB_SERIALIZE_COMPACT_ON_SAVE, 0);
	TimeLoad(szFileName, 0);
	TimeDB2(0, 0);

	QueryPerformanceCounter(&t_start);
	for (i=0;i<TEST_RECORDS_COUNT; i++)
		QueryPerformanceCounter(&t_end);
	t_total.QuadPart = t_end.QuadPart - t_start.QuadPart;
	QueryPerformanceFrequency(&t_end);
	OUTPUT(("\nQueryPerformanceCounter overhit: %f", (float)t_total.QuadPart / (float)t_end.QuadPart));
	
	return 0;
}

int TestMP_SFDB()
{
	char szFileName[MAX_PATH];
	LARGE_INTEGER t_start;
	LARGE_INTEGER t_end;
	LARGE_INTEGER t_total;
	tDWORD i;

	GetTempPath(countof(szFileName), szFileName);
	strcat(szFileName, "saved.dat");

	OUTPUT(("Records count: %d", TEST_RECORDS_COUNT));

	TimeDB2(1, 0);
	TimeDB2(0, 1);
	TimeDB2(1, 2);
	TimeDB2(2, 3);
	TimeDB2(0, 4);
	for (i=0; i<SFDB_COUNT; i++)
		TimeSave(szFileName, 0, i);
	TimeLoad(szFileName, 5);
	TimeLoad(szFileName, 6);
	TimeDB2(1, 7);
	strcat(szFileName, "2");
	TimeSave(szFileName, 0, 8);
	TimeLoad(szFileName, 9);
	strcat(szFileName, ".crypt");
	TimeSave(szFileName, cSFDB_SERIALIZE_CHECKSUM | cSFDB_SERIALIZE_CRYPT | cSFDB_SERIALIZE_COMPACT_ON_SAVE | cSFDB_AUTOLOAD | cSFDB_AUTOSAVE, 0);
	TimeLoad(szFileName, 1);
	TimeDB2(0, 2);

	QueryPerformanceCounter(&t_start);
	for (i=0;i<TEST_RECORDS_COUNT; i++)
		QueryPerformanceCounter(&t_end);
	t_total.QuadPart = t_end.QuadPart - t_start.QuadPart;
	QueryPerformanceFrequency(&t_end);
	OUTPUT(("\nQueryPerformanceCounter overhit: %f", (float)t_total.QuadPart / (float)t_end.QuadPart));
	
	return 0;
}


int do_command(char* cmd, char** args, int argc)
{
	static BOOL bTiming = TRUE;

	if (bTiming)
		StartTiming();

	if (stricmp(cmd, "timing") == 0)
	{
		if (argc==1 && stricmp(args[1], "on") == 0)
			bTiming = TRUE;
		else
		if (argc==1 && stricmp(args[1], "off") == 0)
			bTiming = FALSE;
		else
		{
			OUTPUT(("usage: timing [on|off]"));
			return 1;
		}
		OUTPUTN(("Timing is "));
		OUTPUT(((bTiming == TRUE ? "ON" : "OFF")));
		return 1;
	}
	

	COMMAND(0, "?", Help())
	COMMAND(0, "help", Help())
	COMMAND(0, "cls", _cls());
	COMMAND(0, "dir", ScanDir((argc ? args[1] : "*.*"), ProcessFileDir, FALSE))
	COMMAND(0, "dirs", ScanDir((argc ? args[1] : "*.*"), ProcessFileDir, TRUE))
//	COMMAND(0, "gs", GetSignature(hSigStorage, cmd, args, argc))
//	COMMAND(0, "is", IsSignature(hSigStorage, cmd, args, argc))
//	COMMAND(0, "ds", DeleteSignature(hSigStorage, cmd, args, argc))
//	COMMAND(0, "ls", LoadSignatures(hSigStorage, cmd, args, argc))
//	COMMAND(0, "lt", LoadTree(hSigStorage, cmd, args, argc))
//	COMMAND(0, "st", SaveTree(hSigStorage, cmd, args, argc))
//	COMMAND(0, "dt", DumpTree(hSigStorage))
//	COMMAND(0, "del", DelTree(hSigStorage))
//	COMMAND(0, "del", DelTree(hSigStorage))
//	COMMAND(0, "scant", ScanTest(cmd, args, argc))
//	COMMAND(0, "scans", ScanTestWithSubDir(cmd, args, argc))
	COMMAND(0, "s", Scan(cmd, args, argc))
	COMMAND(0, "scan", Scan(cmd, args, argc))
	COMMAND(0, "ss", ScanWithSubDir(cmd, args, argc))
	COMMAND(0, "scans", ScanWithSubDir(cmd, args, argc))
	COMMAND(0, "tdb", TimeDB())
	COMMAND(0, "tdb2", TestMP_SFDB())
//	COMMAND(0, "scansa", ScanWithSubDirAsAVP3(cmd, args, argc))
//	COMMAND(0, "db", ConvertDatabase(cmd, args, argc))
//	COMMAND(0, "a", RunAVP4(cmd, args, argc))
//	COMMAND(0, "sql", ProcessSQLCommand(cmd, args, argc))
	COMMAND(0, "quit", return 0)
	COMMAND(0, "q", return 0)
	if (*cmd != 0)
		OUTPUT(("Unknown command '%s'", cmd));

	if (bTiming)
	{
		StopTiming();
		DisplayTiming();
	}
	return 1;
}

// -----------------------------------------------------------------------------------------
PR_MAKE_TRACE_FUNC;

// ---
void Trace( tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel) {
	static char b[0x1000];
	static unsigned long ulNestLevel = 0;
	unsigned long i;
	WORD wOldAttr;
	WORD wOldBGAttr;
	char dbginfo[0x100];
	
	dbginfo[0] = 0;
	
	int l = lstrlen( str );
	if ( str[l] != '\n' ) {
		memcpy( b, str, l );
		*(WORD*)(b + l) = MAKEWORD( '\n', 0 );
		str = b;
	}
	if (str[0]=='-' && ulNestLevel > 0)
		ulNestLevel--;
	
	// get debug info
#ifdef _DEBUG
	if (PrDbgGetInfoEx) {
		DWORD dwSkipLevels = 0;
		CHAR* pSkipModules = "prkernel,prloader";
		DWORD dwAddress;
		if (PR_FAIL(PrDbgGetInfoEx(0, pSkipModules, dwSkipLevels, DBGINFO_CALLER_ADDR, &dwAddress, sizeof(DWORD), NULL)))
			pSkipModules = SKIP_THIS_MODULE;
		dwSkipLevels--;
		do {
			dwSkipLevels++;
			if (PR_FAIL(PrDbgGetInfoEx(0, pSkipModules, dwSkipLevels, DBGINFO_SYMBOL_NAME, dbginfo, sizeof(dbginfo), NULL)))
				break;
		} while (strstr(dbginfo, "trace")!=NULL || strstr(dbginfo, "Trace")!=NULL);
				
		if (PR_FAIL(PrDbgGetCallerInfo(pSkipModules, dwSkipLevels, dbginfo, sizeof(dbginfo))))
			dbginfo[0] = 0;
		else
			strcat(dbginfo, " : ");
	}

	OutputDebugString(dbginfo);
#endif

	for (i=0; i<ulNestLevel; i++)
		OutputDebugString("  ");
	OutputDebugString(str);
	

	// FOREGROUND_BLUE, FOREGROUND_GREEN, FOREGROUND_RED, FOREGROUND_INTENSITY, BACKGROUND_BLUE, BACKGROUND_GREEN, BACKGROUND_RED, and BACKGROUND_INTENSITY
	wOldAttr = GetConsoleTextAttr();
	wOldBGAttr = wOldAttr & 0xF0;
	SetConsoleTextAttr(wOldAttr | FOREGROUND_INTENSITY);
	printf("%s", dbginfo);
	switch (dwLevel)
	{
	case prtERROR:
		SetConsoleTextAttr(wOldBGAttr | FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case prtNOTIFY:
	case prtNOT_IMPORTANT:
		SetConsoleTextAttr(wOldBGAttr | FOREGROUND_GREEN);
		break;
	case prtIMPORTANT:
		SetConsoleTextAttr(wOldBGAttr | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case prtALWAYS_REPORTED_MSG:
		SetConsoleTextAttr(wOldAttr);
		break;
	default:
		SetConsoleTextAttr(wOldAttr | FOREGROUND_INTENSITY);
		break;
	}
	for (i=0; i<ulNestLevel; i++)
		printf("   ");
	printf("%s", str);
	SetConsoleTextAttr(wOldAttr);

	if (str[0]=='+')
		ulNestLevel++;
}

tERROR pr_call RootCheck(hROOT hRoot, tVOID* param)
{
    tDWORD tr_ptr = (tDWORD)Trace;
	tDWORD dwMaxLevel;
	tDWORD dwMinLevel;
	tERROR err;
	
	CALL_SYS_PropertySet( hRoot, 0, pgOUTPUT_FUNC, &tr_ptr, sizeof(tr_ptr) );
	err = CALL_SYS_TraceLevelSet(hRoot, tlsALL_OBJECTS, prtERROR, prtMIN_TRACE_LEVEL);
	err = CALL_SYS_TraceLevelGet(hRoot, tlsTHIS_OBJECT, &dwMaxLevel, &dwMinLevel);
	
	return errOK;
}

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(imp_loader)
#include "IFace/e_loader.h"
IMPORT_TABLE_END

// -----------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	char buffer[5*MAX_PATH];
	char* ptr;

#define BACKGROUND_WHITE (BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN)
	unsigned short 	wOldAttr = GetConsoleTextAttr();
	//SetConsoleTextAttr(BACKGROUND_WHITE | BACKGROUND_INTENSITY);
	_cls();
	GetModuleFileName( 0, (LPSTR)buffer, sizeof(buffer) );

/*
	{
		HMODULE hSFDBTestDll = LoadLibrary("O:\\Prague\\ISLite\\SFDBTestDll\\Debug\\SFDBTestDll.dll");
		if (hSFDBTestDll)
		{
			tInit fInit;
			tGetStatus fGetStatus;
			tDone fDone;
			DWORD dwData1, dwData2;

			fInit = (tInit)GetProcAddress(hSFDBTestDll, "Init");
			fGetStatus = (tGetStatus)GetProcAddress(hSFDBTestDll, "GetStatus");
			fDone = (tDone)GetProcAddress(hSFDBTestDll, "Done");

			if (!(fInit("C:\\sfdb.dat") & 0x8000000))
			{
				fGetStatus("C:\\WINDOWS\\explorer.exe", &dwData1, &dwData2);
				fDone();
			}


			FreeLibrary(hSFDBTestDll);
		}
	}
*/
	// loader module name
	ptr = strrchr( (const char *)buffer, '\\' );
	if ( ptr ) 
		++ptr;
	else 
		ptr = buffer;
	
	lstrcpy( (LPSTR)ptr, PR_LOADER_MODULE_NAME );

	HINSTANCE loader = LoadLibrary( buffer );
	if ( loader ) {
		tERROR            error;
		PragueLoadFunc    plf = (PragueLoadFunc)   GetProcAddress( loader, PR_LOADER_LOAD_FUNC );
		PragueUnloadFunc  puf = (PragueUnloadFunc) GetProcAddress( loader, PR_LOADER_UNLOAD_FUNC );
		
		if ( plf ) {
			
			error = plf( &hRoot, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH/*|PR_LOADER_FLAG_USE_TRY_CATCH*/, RootCheck, NULL ); 
			
			if ( hRoot ) {
				tDWORD i;

				if (PR_FAIL(error = CALL_Root_ResolveImportTable(hRoot, NULL, imp_loader, PID_APPLICATION)))
					printf("Cannot import loader exports\n");
				
				for (i=0; i<SFDB_COUNT; i++)
					hSFDB[i] = NULL;
				
				for (i=0; i<SFDB_COUNT; i++)
				{
					if (PR_FAIL(error = CALL_SYS_ObjectCreateQuick(hRoot,&hSFDB[i],IID_SECUREFILEDB,PID_ANY,SUBTYPE_ANY)))
						break;
					else
					if (PR_FAIL(error = CALL_SecureFileDatabase_Load(hSFDB[i], "c:\\sfdb.dat")))
						break;
				}

				if (PR_FAIL(error))
				{
					printf("Cannot init database\n");
					for (i=0; i<SFDB_COUNT; i++)
						if (hSFDB[i] != NULL)
							CALL_SYS_ObjectClose(hSFDB[i]);
				}
				

				error = CALL_SYS_ObjectCreate(hRoot,&hChecker,IID_IFILESECURESTATUS, PID_ANY, SUBTYPE_ANY);
				if ( PR_SUCC(error) )
					error = CALL_SYS_PropertySetStr( hChecker, NULL, plDB_PATHNAME, "c:\\sfdb.dat", 0, cCP_ANSI );
				if ( PR_SUCC(error) )
					error = CALL_SYS_ObjectCreateDone( hChecker );
				
				if (PR_FAIL(error))
				{
					printf("Cannot init iChecker %08X\n", error);
					if (hChecker != NULL)
						CALL_SYS_ObjectClose(hChecker);
				}

				console(do_command);
				
				if ( puf )
					puf( hRoot );
				else
					CALL_SYS_ObjectClose( hRoot );
			}
			else
				printf("Cannot initialize hRoot object. Error code is - 0x%08x\n", error );
		}
		else
			printf("Loader doesn't export init function\n" );
	}
  else
	  printf("Cannot start loader module (\"%s\")\n", buffer );
  SetConsoleTextAttr(wOldAttr);
  return 0;
}
