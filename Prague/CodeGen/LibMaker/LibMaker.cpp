// LibMaker.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "IntObjParse.h"

#include <Stuff/Parray.h>
#include <Stuff/Pathstr.h>
#include <Stuff/get_opt.c>

#include "ReadFile.h"

#include <property/property.h>

#include <datatreeutils/dtutils.h>
#include <serialize/kldtser.h>
#include <windows/dtdropper/dtdropper.h>

#include "../AVPPveID.h"

// warning level /W<level>
#define AVP3STR_FMT		" /DAVP3Cure=AVP3Cure_%s /DAVP3Decode=AVP3Decode_%s"

#define _STR_HEADER				"----------------------------------------------------------"
#define _STR_DEF_CL_OPTIONS		"%s \"%s\" /DPRAGUE /c /nologo /Ox /G3 /Gd /Fo\"%s\" /IO:/Prague/Include %s %s"
#define _STR_DEF_RC_OPTIONS		"rc %s"

// -----------------------------------------------------------------------------------------

#ifdef _DEBUG
long g_HeapUsageCounter = 0;
#endif

HANDLE g_hHeap = NULL;

void* _MM_Alloc(unsigned int nSize)
{
#ifdef _DEBUG
	InterlockedIncrement(&g_HeapUsageCounter);
#endif
	return HeapAlloc(g_hHeap, 0, nSize);
}
void _MM_Free(void* p)
{
#ifdef _DEBUG
	InterlockedDecrement(&g_HeapUsageCounter);
#endif
	HeapFree(g_hHeap, 0, p);
}

// -----------------------------------------------------------------------------------------
unsigned int CountCRC32__(unsigned int size, char* ptr, unsigned int dwOldCRC32)
{
	dwOldCRC32 = ~dwOldCRC32;
	while(size-- != 0)
	{
		unsigned long i;
		unsigned long c = *ptr++;
		for( i=0; i<8; i++)
		{
			if (((dwOldCRC32^c)&1)!=0)
				dwOldCRC32 = (dwOldCRC32>>1) ^ 0xEDB88320;
			else
				dwOldCRC32 = (dwOldCRC32>>1);
			c>>=1;
		}
	}
	return ~dwOldCRC32;
}

// -----------------------------------------------------------------------------------------
BOOL GetMemoryRules(UINT RuleResID, LPVOID* lpResource, DWORD* dwResSize)
{
	HMODULE hIns = GetModuleHandle(NULL);
	HRSRC resource = FindResource(hIns, MAKEINTRESOURCE(RuleResID), RT_RCDATA);
	if (resource != NULL)
	{
		*dwResSize = SizeofResource(hIns, resource);
		if (*dwResSize != 0)
		{
			HGLOBAL hglobal = NULL;
			hglobal = LoadResource(hIns, resource);
			if (hglobal != NULL)
			{
				*lpResource = LockResource(hglobal);
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

PTCHAR GenerateInclude(PTCHAR ptchSourceFolder, int level)
{
	CPathStr Include;
	TCHAR bufferUp[MAX_PATH];
	TCHAR bufferDummy[MAX_PATH];

	wsprintf(bufferUp, "/I\"%s..", ptchSourceFolder);
	wsprintf(bufferDummy, "/I\"%sdummy", ptchSourceFolder);

	for (int cou = 0; cou < level; cou++)
	{
		Include += bufferUp;
		Include += "\" ";
		Include += bufferDummy;
		Include += "\" ";

		lstrcat(bufferUp, "/..");
		lstrcat(bufferDummy, "/dummy");
	}

	return Include.Relinquish();
}

// -----------------------------------------------------------------------------------------

class _Context;
void DoOutput(bool bLogOnly, _Context *pContext, PTCHAR lpszFormat, ...);

// -----------------------------------------------------------------------------------------
#define	_CHECK_EXPORT	0x1
#define	_CHECK_IMPORT	0x2
#define	_CHECK_OTHER	0x4

struct _Engine_Check_Context
{
public:
	PTCHAR		m_ModuleName;
	PTCHAR		m_SourceFile;
	DWORD			m_dwFalgs;
	bool			m_bError;
	_Context*	m_pContext;
};

#define _MOD_OBJECTNAME_FLAG_NONE		0x000
#define _MOD_OBJECTNAME_FLAG_WRONG		0x001
struct _Mod_ObjectName
{
public:
	CPathStr	m_ModuleName;
	CPathStr	m_ObjectName;
	CPathStr	m_SourceFile;
	DWORD		m_dwFlags;
};

// -----------------------------------------------------------------------------------------

class _Context
{
public:
	bool			m_bRecurce;
	bool			m_bDeleteTempFiles;
	
	bool			m_bLog;
	CPathStr		m_LogFileName;
	HANDLE			m_hLogFile;

	int				m_nErrorCount;
	int				m_nMaxErrorCount;

	int				m_PluginID;

	CPathStr		m_OutputFolder;
	CPathStr		m_OutDllName;

	CPArray <TCHAR> m_Masks;
	PHashTree		m_pHashAllowedImports;

	CPArray <_Mod_ObjectName> m_ObjFiles;
	CPArray <TCHAR> m_TempFiles;

	CPArray <TCHAR> m_ListFile;

	CPathStr		m_CompilerPath;

	PHashTree		m_pHashImport;
	PHashTree		m_pHashExport;

	_Context()
	{
		m_bRecurce = false;
		m_bDeleteTempFiles = false;

		m_bLog = false;
		m_hLogFile = INVALID_HANDLE_VALUE;

		m_nErrorCount = 0;
		m_nMaxErrorCount = 100;

		m_CompilerPath = "cl.exe";
		m_LogFileName = CPathStr(MAX_PATH);

		m_PluginID = 0;
	}

	~_Context()
	{
		PTCHAR ptch;
		if (m_hLogFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hLogFile);
			m_hLogFile = INVALID_HANDLE_VALUE;
		}

		if (m_bDeleteTempFiles)
		{
			_Mod_ObjectName* pObjFile;
			for (unsigned int cou = 0; cou < m_ObjFiles.Count(); cou++)
			{
				pObjFile = m_ObjFiles[cou];
				ptch = (PTCHAR) pObjFile->m_ModuleName;
				if (DeleteFile(ptch) == FALSE) 
					DoOutput(false, NULL, "Can't delete file '%s'\n", ptch);
			}
			for (cou = 0; cou < m_TempFiles.Count(); cou++)
			{
				ptch = m_TempFiles[cou];
				if (DeleteFile(ptch) == FALSE) 
					DoOutput(false, NULL, "Can't delete temp file '%s'\n", ptch);
			}
		}

		// destructor
		_Hash_Done(m_pHashExport, 1);
		_Hash_Done(m_pHashImport, 1);
		_Hash_Done(m_pHashAllowedImports, 1);
	}

	bool PrepareLog(void)
	{
		m_bLog = false;
		m_hLogFile = CreateFile(m_LogFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hLogFile != INVALID_HANDLE_VALUE)
				m_bLog = true;
		return m_bLog;
	}
};

class _CmdLine
{
public:
	CPathStr m_szExe_name;
	CPathStr m_szCmd_line;
	_CmdLine()
	{
		CPathStr szTmp(MAX_PATH * 4);
		prepare_cmd_line(GetCommandLine(), 0, szTmp);
		m_szCmd_line = szTmp;
		szTmp = CPathStr(MAX_PATH * 4);
		GetModuleFileName(0, szTmp, MAX_PATH);
		m_szExe_name = szTmp;
	}
	~_CmdLine()
	{
	}

	BOOL Check()
	{
		return TRUE;
	}
};

// -----------------------------------------------------------------------------------------

void usage_help(const char* exe_module_name)
{
	char en[MAX_PATH];
	char* p;
	
	strcpy(en, exe_module_name);
	p = strrchr(en, '\\');
	if (p)
	{
		char* t;
		*p++ = 0;
		t = strrchr(p, '.');
		if (t)
			*t = 0;
	}
	else
		p = en;
	strlwr(p);
	
	printf("\nUsage: %s <source folders1>... <source folders n> [switches]\n", p);
	printf("/r - with subdirectories (default value - no recurse)\n");
	printf("/p<id> - plugin id\n");
	printf("/i<filename> - allowed imports\n");
	printf("/o<filepath> - output folder for .obj\n");
	printf("/n<filename> - output plugin file name\n");
	printf("/e<value> - max err count\n");
	printf("/@<FileName> - file list\n");
	printf("/l<FileName> - copy log to file\n");
	printf("/d delete temporary and .obj files after work (default value - don't delete)\n");
		
	printf("Any key value may be used with or without quotation marks (\" or \')\n\n");
}

void DoOutput(bool bLogOnly, _Context *pContext, PTCHAR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf;
	CPathStr szBuffer(4096);
	
	nBuf = _vsntprintf(szBuffer, 4096, lpszFormat, args);

	if (bLogOnly == false)
		printf(szBuffer);

	if (pContext != NULL && pContext->m_bLog == true)
	{
		DWORD dwWriten = 0;
		WriteFile(pContext->m_hLogFile, szBuffer, lstrlen(szBuffer), &dwWriten, NULL);
	}
	
	va_end(args);
}

int ShowError(_Context *pContext, int ErrCode, PTCHAR ptchErrStr)
{
	DoOutput(false, pContext, "Error %d (%#x) during execution '%s'", ErrCode, ErrCode, ptchErrStr);
	return ErrCode;
}

// -----------------------------------------------------------------------------------------
void _Go(_Context *pContext, PTCHAR ptchSourceFolder);

int _RefCompare(void* pFoundData, void* pInData, int DataLen)
{
	return lstrcmp((PTCHAR) pFoundData, (PTCHAR) pInData);
}

void _RefFreeItem(void* pData, int DataLen, void* pUserData)
{
	_Mod_ObjectName *pObjectName;
	
	pObjectName = (_Mod_ObjectName*) pUserData;
	if (pObjectName != NULL)
		delete pObjectName;
}


// -----------------------------------------------------------------------------------------
void MakeImpExp(_Context *pContext, PTCHAR ptchFileName, PHashTreeItem pItem);
void MakePluginHeader(_Context *pContext);
bool CompileFile(_Context *pContext, PTCHAR ptchFileName, PTCHAR ptchSourceFolder, PTCHAR ptchAddOptions, bool bForce, PTCHAR pAddName);
void MakePluginBodyAndComplie(_Context *pContext, LPVOID pPlugin_c, DWORD dwPlugin_c_size);
void MakeResource(_Context *pContext);

bool AnalyzeObj(_Context *pContext, DWORD dwEngineCheckFlags);

// -----------------------------------------------------------------------------------------

#include "_AVPIO.H"
int main(int argc, PTCHAR argv[])
{
	// -----------------------------------------------------------------------------------------
	g_hHeap = GetProcessHeap();
	if (g_hHeap == NULL)
		return ShowError(NULL, -1, "Can't initialize heap");

	_CmdLine CmdLine;

	if (CmdLine.Check() == FALSE)
		return -1;

	if (get_option(CmdLine.m_szCmd_line, '?', 0, 1) || get_option(CmdLine.m_szCmd_line, 'h', 0, 1))
	{
		usage_help(CmdLine.m_szExe_name);
		return 0;
	}

	::LoadIO();
	::DATA_Init_Library(_MM_Alloc, _MM_Free);
	::KLDT_Init_Library(_MM_Alloc, _MM_Free);
	::DTUT_Init_Library(_MM_Alloc, _MM_Free);
	::SWM_Init_Library(_MM_Alloc, _MM_Free);
	::DropDTToFileInitLibrary(_MM_Alloc, _MM_Free);

	_Hash_Init_Library(_MM_Alloc, _MM_Free, _RefCompare, _RefFreeItem);

	CPathStr szStrFolder;
	CPArray <TCHAR> SourceFolders;
	TCHAR tchValue[MAX_PATH];
	int nOpt;

	// -----------------------------------------------------------------------------------------
	_Context Context;
	DoOutput(false, NULL, "\nLib Maker (ver 0.8) ----------------------------------------------------------\n");
#ifdef _DEBUG
	DoOutput(false, NULL, "(Debug version)\n");
#endif
	// -----------------------------------------------------------------------------------------

	Context.m_pHashImport = _Hash_InitNew(8);
	Context.m_pHashExport = _Hash_InitNew(8);
	Context.m_pHashAllowedImports = _Hash_InitNew(8);

	LPVOID pPlugin_c = NULL;
	DWORD dwPlugin_c_size = 0;
	
	if (GetMemoryRules(1, &pPlugin_c, &dwPlugin_c_size) == FALSE)
	{
		return ShowError(NULL, GetLastError(), "--- Internal check error - can't find template for plugin.c");
	}

	if (!get_option(CmdLine.m_szCmd_line, 'l', tchValue, 1) || (!tchValue[0]))
		DoOutput(false, NULL, "--- No log\n");
	else
	{
		Context.m_LogFileName = tchValue;
		if (Context.PrepareLog() == false)
			ShowError(NULL, GetLastError(), "Can't open log file\n");
		else
			DoOutput(false, NULL, "--- Log file name '%s'\n", Context.m_LogFileName);
	}
	
	if (get_option(CmdLine.m_szCmd_line, 'r', 0, 1))
	{
		DoOutput(false, &Context, "--- With recurse\n");
		Context.m_bRecurce = true;
	}

	if (get_option(CmdLine.m_szCmd_line, 'd', 0, 1))
	{
		DoOutput(false, &Context, "--- Delete temporary and .obj files\n");
		Context.m_bDeleteTempFiles = true;
	}

	if (get_option(CmdLine.m_szCmd_line, 'e', tchValue, 1))
	{
		if (tchValue[0] != 0)
			Context.m_nMaxErrorCount = atoi(tchValue);
		DoOutput(false, &Context, "--- Max err count %d\n", Context.m_nMaxErrorCount);
	}

	if ((get_option(CmdLine.m_szCmd_line, 'i', tchValue, 1)) && (tchValue[0] != 0))
	{
		DoOutput(false, &Context, "--- Allowed imports file '%s'\n", tchValue);
		//Contex.m_Imports;
		HANDLE hFileImports = CreateFile(tchValue, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (hFileImports == INVALID_HANDLE_VALUE)
		{
			return ShowError(&Context, GetLastError(), "Can't open allowed imports file!");
		}
		else
		{
			char* line;
			int   len;
			
			_Mod_ObjectName* pAllowedMod;
			CPathStr AllowedImport;
			CReadFile file(hFileImports);
			
			while(file.readln(line, len))
			{
				if (line)
				{
					pAllowedMod = new _Mod_ObjectName;
					if (pAllowedMod == NULL)
					{
						DoOutput(false, &Context, "Error: not enough memory (allowed imports) for '%s'", line);
						Context.m_nErrorCount++;
					}
					else
					{
						pAllowedMod->m_ObjectName = CPathStr(line);
						_Hash_AddItem(Context.m_pHashAllowedImports, (PTCHAR) pAllowedMod->m_ObjectName, lstrlen(line) + 1, pAllowedMod);
					}
					delete []line;
				}
			}
			
			CloseHandle(hFileImports);
		}
		
	}

	if (get_option(CmdLine.m_szCmd_line, 'p', tchValue, 1))
	{
		if (tchValue[0] != 0)
		{
			Context.m_PluginID = atoi(tchValue);
			DoOutput(false, &Context, "--- Plugin id %d\n", Context.m_PluginID);
		}
	}
	if (Context.m_PluginID == 0)
		return ShowError(NULL, 1, "--- Error: you have to set plugin ID!\n");

	if (get_option(CmdLine.m_szCmd_line, '@', tchValue, 1))
	{
		DoOutput(false, &Context, "--- List files '%s'\n", tchValue);
		HANDLE hFileList = CreateFile(tchValue, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (hFileList == INVALID_HANDLE_VALUE)
		{
			return ShowError(&Context, GetLastError(), "Can't open list file for analyze");
		}
		else
		{
			char* line;
			int   len;
			CReadFile file(hFileList);

			while(file.readln(line, len))
			{
				if (line)
				{
					Context.m_ListFile.Add(CPathStr(line).Relinquish());
					delete []line;
				}
			}

			CloseHandle(hFileList);
		}
	}
	

	if (!get_option(CmdLine.m_szCmd_line, 'n', tchValue, 1) || (!tchValue[0]))
		return ShowError(NULL, 1, "--- Error: you have to set output plugin file!\n");
	else
	{
		DoOutput(false, &Context, "--- Output plugin file name '%s'\n", tchValue);
		Context.m_OutDllName = CPathStr(tchValue);
	}

	if (get_option(CmdLine.m_szCmd_line, 'o', tchValue, 1))
	{
		if (tchValue[0] != 0)
		{
			Context.m_OutputFolder = tchValue;
			DoOutput(false, &Context, "--- Output folder '%s'\n", tchValue);
		}
	}
	
	do
	{
		get_next_option(CmdLine.m_szCmd_line, &nOpt, tchValue, TRUE);
		if (tchValue[0] == 0)
			break;
		switch (nOpt)
		{
		case 0:
			DoOutput(false, &Context, "--- Source folder '%s'\n", tchValue);
			szStrFolder = CPathStr(tchValue);
			szStrFolder ^= "";
			SourceFolders.Add(szStrFolder.Relinquish());
			break;
		default:
			//DoOutput(false, "%c - %s\n", nOpt, tchValue);
			break;
		}
	} while (TRUE);

	if (lstrlen(Context.m_OutputFolder) == 0)
	{
		return ShowError(NULL, -2, "You have to set output folder!");
	}

	PTCHAR ptchSourceFolder;

	// check if not no working folders in parameters
	if (SourceFolders.Count() == 0)
	{
		CPathStr szCurrentFolder(MAX_PATH);
		GetCurrentDirectory(MAX_PATH, szCurrentFolder);
		szCurrentFolder ^= "";
		DoOutput(false, &Context, "--- Work with current folder '%s'\n", szCurrentFolder);
		SourceFolders.Add(szCurrentFolder.Relinquish());
	}

	if (Context.m_Masks.Count() == 0)
	{
		Context.m_Masks.Add(CPathStr("*.c").Relinquish());
		Context.m_Masks.Add(CPathStr("*.cpp").Relinquish());
	}
			
	for (unsigned int cou = 0; cou < SourceFolders.Count(); cou++)
	{
		ptchSourceFolder = SourceFolders[cou];
		_Go(&Context, ptchSourceFolder);
	}

	// -----------------------------------------------------------------------------------------
	CPathStr szReadyForUseFiles(MAX_PATH);
	szReadyForUseFiles = Context.m_OutputFolder;
	szReadyForUseFiles ^= "ReadyForUse.lst";

	DoOutput(false, NULL, "\nAnalyzing...\n");	
	AnalyzeObj(&Context, _CHECK_EXPORT);
	DoOutput(false, NULL, "Analyzing...\n");	
	AnalyzeObj(&Context, _CHECK_OTHER);
	DoOutput(false, NULL, "Analyzing...\n");	
	while (AnalyzeObj(&Context, _CHECK_IMPORT) == false)
		DoOutput(false, NULL, "Analyzing...\n");	
	DoOutput(false, NULL, "\n...analyzing complete\n");	

	MakeImpExp(&Context, "exports.h", Context.m_pHashExport->m_pFirst);
	MakeImpExp(&Context, "imports.h", Context.m_pHashImport->m_pFirst);
	MakePluginHeader(&Context);
	MakeResource(&Context);

	MakePluginBodyAndComplie(&Context, pPlugin_c, dwPlugin_c_size);

	// -----------------------------------------------------------------------------------------
	DoOutput(false, &Context, "\n"_STR_HEADER"\n --- Finished. Errors %d\n", Context.m_nErrorCount);

	// -----------------------------------------------------------------------------------------
	HANDLE hReadyForUse = CreateFile((PTCHAR) szReadyForUseFiles, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hReadyForUse != INVALID_HANDLE_VALUE)
	{
		_Mod_ObjectName* pReadyFile;
		PTCHAR ptch;
		DWORD dwWritten;

		PTCHAR ptchEndLine = "\r\n";
		
		for (unsigned int cou = 0; cou < Context.m_ObjFiles.Count(); cou++)
		{
			pReadyFile = Context.m_ObjFiles[cou];
			if (!(pReadyFile->m_dwFlags & _MOD_OBJECTNAME_FLAG_WRONG))
			{
				ptch = (PTCHAR) pReadyFile->m_SourceFile;
				WriteFile(hReadyForUse, ptch, lstrlen(ptch), &dwWritten, NULL);
				WriteFile(hReadyForUse, ptchEndLine, lstrlen(ptchEndLine), &dwWritten, NULL);
			}
		}
		CloseHandle(hReadyForUse);
	}
	
	return 0;
}

// -----------------------------------------------------------------------------------------

void callback(DWORD type, char* name, DWORD dwUserValue)
{
	_Engine_Check_Context* pEngineCheckContext = (_Engine_Check_Context*) dwUserValue;
	if (pEngineCheckContext == NULL)
	{
		DoOutput(false, NULL,  "Error - can't acquire EngineContext in AnylizeCallback!");
		return;
	}

	_Context *pContext = pEngineCheckContext->m_pContext;
	PHashTreeItem pItem;
	_Mod_ObjectName *pObjectName;

	if(lstrcmp(name, "_d_shp") == 0)
	{
		OutputDebugString("\n");
	}

	if (name[0] == '_')
		name++;
	switch(type)
	{
	case OBJPARSE_EXPORT:
		if (pEngineCheckContext->m_dwFalgs & _CHECK_EXPORT)
		{
			DoOutput(true, pContext, "EXPORT  %s in '%s'\n", name, (PTCHAR) pEngineCheckContext->m_SourceFile);
			pItem = _Hash_Find(pContext->m_pHashExport, name, lstrlen(name) + 1);
			if (pItem != NULL)
			{
				pObjectName = (_Mod_ObjectName*) pItem->m_pUserData;
				DoOutput(false, pContext, "Export '%s' already defined in '%s'\n", name, pObjectName->m_ModuleName);
				pEngineCheckContext->m_bError = true;
				pContext->m_nErrorCount++;
			}
			else
			{
				pObjectName = new _Mod_ObjectName;
				if (pObjectName == NULL)
				{
					DoOutput(false, pContext, "Error - ñan't allocate momory for export.\n");
					pContext->m_nErrorCount++;
					pEngineCheckContext->m_bError = true;
				}
				else
				{
					pObjectName->m_ModuleName = pEngineCheckContext->m_ModuleName;
					pObjectName->m_ObjectName = name;
					pObjectName->m_SourceFile = pEngineCheckContext->m_SourceFile;
					pObjectName->m_dwFlags = _MOD_OBJECTNAME_FLAG_NONE;
					_Hash_AddItem(pContext->m_pHashExport, (PTCHAR) pObjectName->m_ObjectName, lstrlen(name) + 1, pObjectName);
				}
			}
		}
		break;
	case OBJPARSE_IMPORT:
		if (pEngineCheckContext->m_dwFalgs & _CHECK_IMPORT)
		{
			DoOutput(true, pContext, "IMPORT  %s in '%s'\n", name, (PTCHAR) pEngineCheckContext->m_SourceFile);
			pItem = _Hash_Find(pContext->m_pHashExport, name, lstrlen(name) + 1);
			if (pItem == NULL)
			{
				pItem = _Hash_Find(pContext->m_pHashAllowedImports, name, lstrlen(name) + 1);
				if (pItem == NULL)
				{
					DoOutput(false, pContext, "Error: unresolved import %s in %s\n", name, (PTCHAR) pEngineCheckContext->m_SourceFile);
					pEngineCheckContext->m_bError = true;
				}
			}

			if (pItem != NULL)
			{
				pItem = _Hash_Find(pContext->m_pHashImport, name, lstrlen(name) + 1);
				if (pItem == NULL)
				{
					pObjectName = new _Mod_ObjectName;
					if (pObjectName == NULL)
					{
						DoOutput(false, pContext, "Error - ñan't allocate momory for export.\n");
						pContext->m_nErrorCount++;
						pEngineCheckContext->m_bError = true;
					}
					else
					{
						pObjectName->m_ModuleName = pEngineCheckContext->m_ModuleName;
						pObjectName->m_ObjectName = name;
						pObjectName->m_SourceFile = pEngineCheckContext->m_SourceFile;
						pObjectName->m_dwFlags = _MOD_OBJECTNAME_FLAG_NONE;
						_Hash_AddItem(pContext->m_pHashImport, (PTCHAR) pObjectName->m_ObjectName, lstrlen(name) + 1, pObjectName);
					}
				}
			}
		}
		break;
	case OBJPARSE_DATA:
		if (lstrcmp(name, _DEBUG_SEGMENT) == 0)
			break;
		if (pEngineCheckContext->m_dwFalgs & _CHECK_OTHER)
		{
			DoOutput(false, pContext, "Found DATA '%s' in '%s'\n", name, (PTCHAR) pEngineCheckContext->m_SourceFile);
			pContext->m_nErrorCount++;
			pEngineCheckContext->m_bError = true;
		}
		break;
	case OBJPARSE_BSS:
		if (pEngineCheckContext->m_dwFalgs & _CHECK_OTHER)
		{
			DoOutput(false, pContext, "Found BSS '%s' in '%s'\n", name, (PTCHAR) pEngineCheckContext->m_SourceFile);
			pContext->m_nErrorCount++;
			pEngineCheckContext->m_bError = true;
		}
		break;
	default:
		if (pEngineCheckContext->m_dwFalgs & _CHECK_OTHER)
		{
			DoOutput(false, pContext, "Found UNKNOWN '%s' in '%s'\n", name, (PTCHAR) pEngineCheckContext->m_SourceFile);
			pContext->m_nErrorCount++;
			pEngineCheckContext->m_bError = true;
		}
		break;
	}
}

_Mod_ObjectName* AddFileToAnalizeContext(_Context *pContext, CPathStr* pszWorkingFileOutput, PTCHAR pObjeName, CPathStr* pszWorkingFile)
{
	_Mod_ObjectName* pObjFile = NULL;
	pObjFile = new _Mod_ObjectName;
	if (pObjFile == NULL)
	{
		DoOutput(false, pContext, "Error allocating obj context");
		pContext->m_nErrorCount = pContext->m_nMaxErrorCount;
	}
	else
	{
		pObjFile->m_ModuleName = *pszWorkingFileOutput;
		pObjFile->m_ObjectName = pObjeName;
		pObjFile->m_SourceFile = *pszWorkingFile;
		pObjFile->m_dwFlags = _MOD_OBJECTNAME_FLAG_NONE;
		pContext->m_ObjFiles.Add(pObjFile);
	}

	return pObjFile;
}
	

bool EnginesCheck(_Context *pContext, PTCHAR ptchFileName, PTCHAR ptchSourceFile, DWORD dwCheckFlags)
{
	bool bRet = false;
	HANDLE hFile;

	_Engine_Check_Context EngineCheckContext;
	
	hFile = CreateFile(ptchFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DoOutput(false, pContext, "Can open file for analyze '%s'...waiting...", ptchFileName);
		Sleep(5000);
		hFile = CreateFile(ptchFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DoOutput(false, pContext, "failed. Error %d\n", GetLastError());
			pContext->m_nErrorCount = pContext->m_nMaxErrorCount;
		}
		else
			DoOutput(false, pContext, "\n");
	}
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD len = GetFileSize(hFile, 0);
		if(len != 0)
		{
			BYTE* body = (BYTE*) _MM_Alloc(len);
			if(body == NULL)
			{
				pContext->m_nErrorCount = pContext->m_nMaxErrorCount;
				DoOutput(false, pContext, "Can't allocate memory for ojb's body '%s'", ptchFileName);
			}
			else
			{
				if(ReadFile(hFile, body, len, &len,0))
				{
					EngineCheckContext.m_pContext = pContext;
					EngineCheckContext.m_ModuleName = ptchFileName;
					EngineCheckContext.m_SourceFile = ptchSourceFile;
					EngineCheckContext.m_dwFalgs = dwCheckFlags;
					EngineCheckContext.m_bError = false;
					
					ObjParse(body, len, callback, (DWORD) &EngineCheckContext);
					if (EngineCheckContext.m_bError == true)
						bRet = false;
					else
						bRet = true;
				}
				_MM_Free(body);
			}
		}
		
		CloseHandle(hFile);
	}

	return bRet;
}

bool CompileFile(_Context *pContext, PTCHAR ptchFileName, PTCHAR ptchSourceFolder, PTCHAR ptchAddOptions, bool bForce, PTCHAR pAddName)
{
	bool bRet = false;
	CPathStr szWorkingFile;
	CPathStr szWorkingFileOutput;

	CPathStr szGenName(MAX_PATH);

	static nInc = 0;

	szWorkingFile = ptchSourceFolder;
	szWorkingFile ^= ptchFileName;

	DoOutput(false, pContext, "--- File '%s'", szWorkingFile);

	szWorkingFileOutput = pContext->m_OutputFolder;
	szWorkingFileOutput ^= "Obj_";
	wsprintf(szGenName, "%s%s_%s.obj", (PTCHAR) szWorkingFileOutput, (PTCHAR) ptchFileName, pAddName);
	szWorkingFileOutput = szGenName;

	if (bForce == false)
	{
		HANDLE hRecompile = CreateFile((PTCHAR) szWorkingFileOutput, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (hRecompile != INVALID_HANDLE_VALUE)
		{
			AddFileToAnalizeContext(pContext, &szWorkingFileOutput, "compiled object file", &szWorkingFile);
			CloseHandle(hRecompile);
			DoOutput(false, pContext, "\n", szWorkingFileOutput);
			return true;
		}
	}
#ifdef _DEBUG
	DoOutput(false, pContext, " -> '%s'\n", szWorkingFileOutput);
#else
	DoOutput(false, pContext, "\n", szWorkingFileOutput);
#endif
	// -----------------------------------------------------------------------------------------
	HANDLE read_file = NULL;
	HANDLE write_file = NULL;
	
	SECURITY_ATTRIBUTES SecurityInformation;
	SecurityInformation.nLength              = sizeof(SECURITY_ATTRIBUTES);
	SecurityInformation.lpSecurityDescriptor = NULL;
	SecurityInformation.bInheritHandle       = TRUE;
	
	CreatePipe(&read_file, &write_file, &SecurityInformation, 0x10000);
	
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;
	StartupInfo.hStdInput = 0;
	StartupInfo.hStdOutput = write_file;
	StartupInfo.hStdError = write_file;

	CPathStr szCompileCommandLine(4096 * 4);

	PROCESS_INFORMATION ProcessInformation;
	ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

	PTCHAR pIncludeFolders = GenerateInclude(ptchSourceFolder, 4);

	wsprintf(szCompileCommandLine, _STR_DEF_CL_OPTIONS, 
		(PTCHAR) pContext->m_CompilerPath, (PTCHAR) szWorkingFile, (PTCHAR)szWorkingFileOutput,
		pIncludeFolders, ptchAddOptions);

	delete pIncludeFolders;

	if (!CreateProcess(0, szCompileCommandLine, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &StartupInfo, &ProcessInformation))
		DoOutput(false, pContext, "Can't compile file '%s'. Error %d", szWorkingFileOutput, GetLastError());
	else
	{
		BOOL bExit = FALSE;
		bRet = true; // successful compilation;
		
		do
		{

			bExit = (WAIT_TIMEOUT != WaitForSingleObject(ProcessInformation.hProcess, 100));

			CPArray<CId> Lines;

			CReadFile file(read_file);

			int   c;
			char* line;
			int   len;
			bool	bErrOut;
			c = Lines.Count();
			while(file.readln(line, len))
			{
				if (line)
				{
					bErrOut = false;
					CErrorLine err(line, ptchSourceFolder, ptchFileName);
					if (err.m_msg && *err.m_msg && lstrcmp(err.m_msg, ptchFileName))
					{
						/*bool not_found = true;
						if (err.m_line)
						{
							for(i=c-1; i>=0; i--)
							{
								CId& id = *Lines[i];
								if (err.m_line > id.m_line)
								{ // we found beginning of the method
									_asm int 3;
									not_found = false;
									if (!err.m_file)
										err.m_line -= id.m_line;
									//replace(err.m_msg, id.m_method_name, "HANDLER");
									//m_gen.add_error(id, err);
									if (bRet)
										bRet = false;
									break;
								}
							}
						}
						if (not_found)*/
						{
							CId id (0, 0);
							//m_gen.add_error(id, err);
							bErrOut = true;
							DoOutput(false, pContext, "'%s' (%d) : error %s : %s\n", (PTCHAR) err.m_file, err.m_line, err.m_str_id, err.m_msg);
							bRet = false;

							pContext->m_nErrorCount++;
						}
					}

					if (!bErrOut)
						DoOutput(false, NULL, "%s", line);
					if (line)
						delete []line;
				}
			}

		} while	(!bExit);
		

		_Mod_ObjectName* pObjFile = NULL;
		if (bRet == true)
			pObjFile = AddFileToAnalizeContext(pContext, &szWorkingFileOutput, "compiled object file", &szWorkingFile);
		
		if (pObjFile == NULL)
			DeleteFile((PTCHAR) szWorkingFileOutput);

		CloseHandle(ProcessInformation.hProcess);
	}

	if (read_file != NULL)
		CloseHandle(read_file);
	if (write_file != NULL)
		CloseHandle(write_file);

	return bRet;
}


// -----------------------------------------------------------------------------------------
bool CompileResFile(_Context *pContext, PTCHAR ptchFileName)
{
	bool bRet = false;
	CPathStr szWorkingFile;
	CPathStr szWorkingFileOutput;

	// -----------------------------------------------------------------------------------------
	HANDLE read_file = NULL;
	HANDLE write_file = NULL;
	
	SECURITY_ATTRIBUTES SecurityInformation;
	SecurityInformation.nLength              = sizeof(SECURITY_ATTRIBUTES);
	SecurityInformation.lpSecurityDescriptor = NULL;
	SecurityInformation.bInheritHandle       = TRUE;
	
	CreatePipe(&read_file, &write_file, &SecurityInformation, 0x10000);
	
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;
	StartupInfo.hStdInput = 0;
	StartupInfo.hStdOutput = write_file;
	StartupInfo.hStdError = write_file;

	CPathStr szCompileCommandLine(4096 * 4);

	PROCESS_INFORMATION ProcessInformation;
	ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

	wsprintf(szCompileCommandLine, _STR_DEF_RC_OPTIONS, ptchFileName);

	if (!CreateProcess(0, szCompileCommandLine, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &StartupInfo, &ProcessInformation))
		DoOutput(false, pContext, "Can't compile file '%s'. Error %d", szWorkingFileOutput, GetLastError());
	else
	{
		BOOL bExit = FALSE;
		do
		{
			bExit = (WAIT_TIMEOUT != WaitForSingleObject(ProcessInformation.hProcess, 100));

			CPArray<CId> Lines;

			CReadFile file(read_file);

			bRet = true; // successful compilation;
			
			char* line;
			int   len;
			Lines.Count();
			while(file.readln(line, len))
			{
				DoOutput(false, pContext, "%s\n", line);
				delete []line;
			}

		} while	(!bExit);

		CloseHandle(ProcessInformation.hProcess);
	}

	if (read_file != NULL)
		CloseHandle(read_file);
	if (write_file != NULL)
		CloseHandle(write_file);

	return bRet;
}

// -----------------------------------------------------------------------------------------

void _Go(_Context *pContext, PTCHAR ptchSourceFolder)
{
	int nFileProceeded = 0;
	
	DoOutput(false, pContext, _STR_HEADER"\n");

	WIN32_FIND_DATA df;
	TCHAR tchSF[MAX_PATH];
	TCHAR tchTmp[MAX_PATH];

	HANDLE hFile;

	if (pContext->m_ListFile.Count() > 0)
	{
		PTCHAR filename;
		CPathStr SourceName;
		CPathStr SourceFolder(MAX_PATH);
		CPathStr DefineID(MAX_PATH);

		CPathStr SourceNameForUse;
		
		pContext->m_bRecurce = false;

		PTCHAR op_file;
		PTCHAR ptchid;
		int co;

		for (unsigned int cou = 0; cou < pContext->m_ListFile.Count(); cou++)
		{
			ptchid = NULL;
			op_file = (PTCHAR) pContext->m_ListFile[cou];
			SourceNameForUse = op_file;
			for (co = lstrlen(op_file); co > 0; co--)
			{
				if (op_file[co] == ',')
				{
					ptchid = op_file + co;
					break;
				}
			}
			if (ptchid == NULL)
			{
				pContext->m_nErrorCount++;
				DoOutput(false, pContext, "Corrupted file name '%s' in list", op_file);
			}
			else
			{
				(op_file)[lstrlen(op_file) - lstrlen(ptchid)] = 0;
				ptchid++;
				while (ptchid[0] == ' ')
					ptchid++;

				if(lstrlen(ptchid) == 0)
				{
					pContext->m_nErrorCount++;
					DoOutput(false, pContext, "Corrupted file name '%s' in list", op_file);
				}
				else
				{
					wsprintf(DefineID, AVP3STR_FMT, ptchid, ptchid);

					GetFullPathName(op_file, MAX_PATH, (PTCHAR) SourceFolder, &filename);
					SourceName = filename;
					((PTCHAR) SourceFolder)[lstrlen((PTCHAR) SourceFolder) - lstrlen((PTCHAR) SourceName)] = 0;
					SourceFolder ^= "";
					CompileFile(pContext, (PTCHAR) SourceName, (PTCHAR) SourceFolder, DefineID, false, ptchid);
					nFileProceeded++;
				}
			}
			if (pContext->m_nErrorCount > pContext->m_nMaxErrorCount)
				break;
		}
	}
	else
	{
		TCHAR tchid[MAX_PATH];
		DoOutput(false, pContext, "--- '%s'...\n", ptchSourceFolder);
		for (unsigned int cou = 0; cou < pContext->m_Masks.Count(); cou++)
		{
			wsprintf(tchTmp, "%s%s", ptchSourceFolder, pContext->m_Masks[cou]);
			hFile = FindFirstFile(tchTmp, &df);
			
			if (hFile != INVALID_HANDLE_VALUE)
			{
				BOOL bExit = FALSE;
				while (bExit == FALSE)
				{
					if ((lstrcmp(df.cFileName, ".") != 0) && (lstrcmp(df.cFileName, "..") != 0))
					{
						if (!(df.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
						{
							wsprintf(tchid, "%d", nFileProceeded);
							CompileFile(pContext, df.cFileName, ptchSourceFolder, "", false, tchid);
							if (pContext->m_nErrorCount > pContext->m_nMaxErrorCount)
							{
								bExit = TRUE;
								continue;
							}
							nFileProceeded++;
						}
					}
					bExit = !FindNextFile(hFile, &df);
				}
				FindClose(hFile);
			}
		}
	}
	
	if (nFileProceeded != 0)
		DoOutput(false, pContext, "... Total files %d\n", nFileProceeded);
	else
		DoOutput(false, pContext, "... nothing to do\n");
	
	if (pContext->m_nErrorCount > pContext->m_nMaxErrorCount)
		return;
	
	wsprintf(tchTmp, "%s*.*", ptchSourceFolder);
	hFile = FindFirstFile(tchTmp, &df);
	
	if (pContext->m_bRecurce == true)
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			BOOL bExit = FALSE;
			while (bExit == FALSE)
			{
				if ((lstrcmp(df.cFileName, ".") != 0) && (lstrcmp(df.cFileName, "..") != 0))
				{
					if (df.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						wsprintf(tchSF, "%s%s\\", ptchSourceFolder, df.cFileName);
						_Go(pContext, tchSF);
					}
				}
				bExit = !FindNextFile(hFile, &df);
			}
			FindClose(hFile);
		}
	}
}

// -----------------------------------------------------------------------------------------
PTCHAR GetIdFromName(PTCHAR ptchName)
{
	PTCHAR ptchID = NULL;

	for (int cou = lstrlen(ptchName) - 1; cou > 0; cou--)
	{
		if (ptchName[cou] == '_')
		{
			ptchID = ptchName + cou + 1;
			break;
		}
	}

	return ptchID;
}

PTCHAR		gClasses[3] = {"ECLSID_AVP3LINK", "ECLSID_AVP3DECODE", "ECLSID_AVP3CURE"};
AVP_dword	gClass[3]	= {0x865ac586, 0x896dbd24, 0xcacaff9e};

#define _DEF_LINK		0
#define _DEF_DECODE		1
#define _DEF_CURE		2

AVP_dword GetClassID(PTCHAR ptchName)
{
	if RtlEqualMemory(ptchName, "AVP3Cure", sizeof("AVP3Cure") - 1)
		return _DEF_CURE;
	if RtlEqualMemory(ptchName, "AVP3Decode", sizeof("AVP3Decode") - 1)
		return _DEF_DECODE;
	
	return _DEF_LINK;
}

void MakeImpExp(_Context *pContext, PTCHAR ptchFileName, PHashTreeItem pItemFirst)
{
	CPathStr szPath;
	szPath = pContext->m_OutputFolder;
	szPath ^= ptchFileName;

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DoOutput(false, pContext, "Error: can't create '%s' file!", szPath);
		pContext->m_nErrorCount++;
		return;
	}

	DWORD dwWritten;
	TCHAR tchBuffer[0x200];

	// -----------------------------------------------------------------------------------------
#define _HEADER_ID_ "header_gen_h"

	wsprintf(tchBuffer, "#define IMPEX_CURRENT_HEADER  <%s>\r\n", ptchFileName);
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);

	lstrcpy(tchBuffer, "#include <iface/impexhlp.h>\r\n\r\n");
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);
	
	wsprintf(tchBuffer, "#if !defined(%s_%d) || defined(IMPEX_TABLE_CONTENT)\r\n", _HEADER_ID_, pContext->m_PluginID);
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);

	lstrcpy(tchBuffer, "#if (IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DECLARE)\r\n");
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);

	wsprintf(tchBuffer, "#define %s_%d\r\n", _HEADER_ID_, pContext->m_PluginID);
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);

	lstrcpy(tchBuffer, "#endif\r\n\r\n\tIMPEX_BEGIN\r\n\r\n");
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);
	
	// -----------------------------------------------------------------------------------------
	PHashTreeItem pItem = pItemFirst;
	_Mod_ObjectName *pObjectName;
	AVP_dword ClassID;

	TCHAR tchID[32];
	AVP_dword nID;
	while (pItem != NULL)
	{
		pObjectName = (_Mod_ObjectName*) pItem->m_pUserData;
		if (pObjectName != NULL)
		{
			ClassID = GetClassID((PTCHAR) pObjectName->m_ObjectName);
			if (ClassID == _DEF_LINK)
			{
				nID = CountCRC32__(lstrlen(pObjectName->m_ObjectName), pObjectName->m_ObjectName, 0);
				wsprintf(tchID, "%08x", nID);
			}
			else 
				lstrcpy(tchID, (PTCHAR) GetIdFromName((PTCHAR) pObjectName->m_ObjectName));
			wsprintf(tchBuffer, "\tIMPEX_NAME_ID(%s, %s, WORD, %s,  (struct tag_AVP3DATA* pAVP3Data))\r\n", (PTCHAR) gClasses[ClassID], tchID, (PTCHAR) pObjectName->m_ObjectName);
			WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);
		}
		
		pItem = pItem->m_pCommonNext;
	}
	
	lstrcpy(tchBuffer, "\r\n\tIMPEX_END\r\n\r\n#endif\r\n");
	WriteFile(hFile, tchBuffer, lstrlen(tchBuffer), &dwWritten, NULL);
	
	CloseHandle(hFile);
}


// -----------------------------------------------------------------------------------------
void MakePluginHeader(_Context *pContext)
{
	CPathStr szHeaderName("plugin.h");
		
	CPathStr szPath;
	szPath = pContext->m_OutputFolder;
	szPath ^= szHeaderName;
	
	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DoOutput(false, pContext, "Error: can't create %s file!", szPath);
		pContext->m_nErrorCount++;
		return;
	}

	DWORD dwWritten;
	TCHAR tchHeader[512];
	wsprintf(tchHeader, "#define PID_GENPLUGIN    ((tPID)(%#x))\r\n//#define VID_USER        ((tVID)(0))\r\n", pContext->m_PluginID);
	WriteFile(hFile, tchHeader, lstrlen(tchHeader), &dwWritten, NULL);

	CloseHandle(hFile);
}

PTCHAR MakeListObj(_Context *pContext)
{
	CPathStr szList("listobj.lst");
	
	CPathStr szPath;
	szPath = pContext->m_OutputFolder;
	szPath ^= szList;
	
	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DoOutput(false, pContext, "Error: can't create '%s' file for obj list!", szPath);
		pContext->m_nErrorCount++;
		return NULL;
	}
	
	DWORD dwWritten;

	_Mod_ObjectName* pObjFile;
	PTCHAR ptch;
	PTCHAR ptchret = "\r\n";
	for (unsigned int cou = 0; cou < pContext->m_ObjFiles.Count(); cou++)
	{
		pObjFile = pContext->m_ObjFiles[cou];

		if (!(pObjFile->m_dwFlags & _MOD_OBJECTNAME_FLAG_WRONG))
		{
			ptch = (PTCHAR) pObjFile->m_ModuleName;
			
			WriteFile(hFile, ptch, lstrlen(ptch), &dwWritten, NULL);
			WriteFile(hFile, ptchret, lstrlen(ptchret), &dwWritten, NULL);
		}
	}

	CPathStr ResourceName = pContext->m_OutputFolder;
	ResourceName ^= "Plugin.res";
	WriteFile(hFile, (PTCHAR) ResourceName, lstrlen((PTCHAR) ResourceName), &dwWritten, NULL);
	WriteFile(hFile, ptchret, lstrlen(ptchret), &dwWritten, NULL);
	
	CloseHandle(hFile);

	return szPath.Relinquish();
}

void MakePluginBodyAndComplie(_Context *pContext, LPVOID pPlugin_c, DWORD dwPlugin_c_size)
{
	CPathStr szPluginName("plugin.c");

	CPathStr szPath;
	szPath = pContext->m_OutputFolder;
	szPath ^= szPluginName;

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DoOutput(false, pContext, "Error: can't create plugin.c!", szPath);
		pContext->m_nErrorCount++;
		return;
	}
	
	DWORD dwWritten;
	WriteFile(hFile, pPlugin_c, dwPlugin_c_size, &dwWritten, NULL);

	CloseHandle(hFile);
	
	if (CompileFile(pContext, szPluginName, pContext->m_OutputFolder, "", true, ""))
	{
		PTCHAR ptchLink = MakeListObj(pContext);
		if (ptchLink != NULL)
		{
			TCHAR tchLink[MAX_PATH];
			wsprintf(tchLink, "link @\"%s\" /DLL /OUT:\"%s\" /ENTRY:DllMain", (PTCHAR) ptchLink, (PTCHAR) pContext->m_OutDllName);
			//
			HANDLE read_file = NULL;
			HANDLE write_file = NULL;
			
			SECURITY_ATTRIBUTES SecurityInformation;
			SecurityInformation.nLength              = sizeof(SECURITY_ATTRIBUTES);
			SecurityInformation.lpSecurityDescriptor = NULL;
			SecurityInformation.bInheritHandle       = TRUE;
			
			CreatePipe(&read_file, &write_file, &SecurityInformation, 0);
			
			STARTUPINFO StartupInfo;
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof(STARTUPINFO);
			StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			StartupInfo.hStdOutput = write_file;
			StartupInfo.hStdError = write_file;
			
			CPathStr szCompileCommandLine(4096 * 4);
			
			PROCESS_INFORMATION ProcessInformation;
			ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
		
			if (CreateProcess(0, tchLink, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &StartupInfo, &ProcessInformation))
			{
				BOOL bExit = FALSE;
				do
				{
					bExit = (WAIT_TIMEOUT != WaitForSingleObject(ProcessInformation.hProcess, 100));
					
					CReadFile file(read_file);
					char* line;
					int   len;
					while(file.readln(line, len))
						DoOutput(false, pContext, "'%s'\n", line);
				} while	(!bExit);
			}

			pContext->m_TempFiles.Add(ptchLink);
		}
	}
}

void DeleteFromHashBySourceFileName(PHashTree pTree, PTCHAR pSourceFileName)
{
	PHashTreeItem pItem = _Hash_GetFirst(pTree);
	PHashTreeItem pItemNext;
	_Mod_ObjectName* pObjectName;
	while (pItem != NULL)
	{
		pItemNext = pItem->m_pCommonNext;
		pObjectName = (_Mod_ObjectName*) pItem->m_pUserData;
		if (pObjectName != NULL)
		{
			if (lstrcmp((PTCHAR) pObjectName->m_SourceFile, pSourceFileName) == 0)
				_Hash_DelItem(pTree, pItem, 1);
				
		}
		pItem = pItemNext;
	}
}

bool AnalyzeObj(_Context *pContext, DWORD dwEngineCheckFlags)
{
	bool bRet = true;
	PTCHAR ptch;
	_Mod_ObjectName* pObjFile;

	if (pContext->m_nErrorCount > pContext->m_nMaxErrorCount)
		return true;

	for (unsigned int cou = 0; cou < pContext->m_ObjFiles.Count(); cou++)
	{
		pObjFile = pContext->m_ObjFiles[cou];
		ptch = (PTCHAR) pObjFile->m_ModuleName;

		if (!(pObjFile->m_dwFlags & _MOD_OBJECTNAME_FLAG_WRONG))
		{
			if (EnginesCheck(pContext, ptch, (PTCHAR) pObjFile->m_SourceFile, dwEngineCheckFlags) == false)
			{
				pObjFile->m_dwFlags |= _MOD_OBJECTNAME_FLAG_WRONG;
				bRet = false;
				if (!(dwEngineCheckFlags & _CHECK_EXPORT))
				{
					DeleteFromHashBySourceFileName(pContext->m_pHashExport, (PTCHAR) pObjFile->m_SourceFile);
					DeleteFromHashBySourceFileName(pContext->m_pHashImport, (PTCHAR) pObjFile->m_SourceFile);
				}
			}
		}
	}

	return bRet;
}

void MakeResource(_Context *pContext)
{
/*
VE_PID_ROOT

  VE_PID_PL_EXPORTS
  VE_PID_PL_IMPORTS

  VE_PID_PL_IMPEX_METHOD_CLASS				// ClassID
  VE_PID_PL_IMPEX_METHOD_FUNC				// nID
  VE_PID_PL_IMPEX_METHOD_NAME				// pObjectName->m_ObjectName
  VE_PID_PL_IMPEX_METHOD_MODULE_NAME		// pObjectName->m_SourceFile
*/

	_Mod_ObjectName *pObjectName;
	PHashTreeItem pItem;

	AVP_dword ClassID;
	AVP_dword nID;

	HDATA hRootData = DATA_Add(0, NULL, AVP_PID_NOTHING, 0, 0);
	if (hRootData == 0)
		DoOutput(false, pContext, "--- Internal check error - can't create resource tree");
	else
	{
		// -----------------------------------------------------------------------------------------
		int idx = 1;
		HDATA hTmp;
		HDATA hExports = DATA_Add(hRootData, NULL, VE_PID_PL_EXPORTS, 0, 0);
		HDATA hImports = DATA_Add(hRootData, NULL, VE_PID_PL_IMPORTS, 0, 0);
		if (hExports != 0 && hImports != 0)
		{
			pItem = pContext->m_pHashExport->m_pFirst;
			while (pItem != NULL)
			{
				pObjectName = (_Mod_ObjectName*) pItem->m_pUserData;
				
				ClassID = GetClassID((PTCHAR) pObjectName->m_ObjectName);
				nID = CountCRC32__(lstrlen((PTCHAR) pObjectName->m_ObjectName), (PTCHAR) pObjectName->m_ObjectName, 0);

				hTmp = DATA_Add(hExports, NULL, MAKE_AVP_PID(idx++, AVP_APID_GLOBAL, avpt_dword, 0), 0, 0);
				if (hTmp != 0)
				{
					DATA_Add_Prop(hTmp, NULL, VE_PID_PL_IMPEX_METHOD_CLASS, gClass[ClassID], sizeof(AVP_dword));
					DATA_Add_Prop(hTmp, NULL, VE_PID_PL_IMPEX_METHOD_FUNC, nID, sizeof(nID));
					DATA_Add_Prop(hTmp, NULL, VE_PID_PL_IMPEX_METHOD_NAME, (AVP_dword) (PTCHAR) pObjectName->m_ObjectName, lstrlen((PTCHAR) pObjectName->m_ObjectName) + 1);
				}
				pItem = pItem->m_pCommonNext;
			}

			pItem = pContext->m_pHashImport->m_pFirst;
			while (pItem != NULL)
			{
				pObjectName = (_Mod_ObjectName*) pItem->m_pUserData;
				
				ClassID = GetClassID((PTCHAR) pObjectName->m_ObjectName);
				nID = CountCRC32__(lstrlen((PTCHAR) pObjectName->m_ObjectName), (PTCHAR) pObjectName->m_ObjectName, 0);
				
				hTmp = DATA_Add(hImports, NULL, MAKE_AVP_PID(idx++, AVP_APID_GLOBAL, avpt_dword, 0), 0, 0);
				if (hTmp != 0)
				{
					DATA_Add_Prop(hTmp, NULL, VE_PID_PL_IMPEX_METHOD_CLASS, ClassID, sizeof(ClassID));
					DATA_Add_Prop(hTmp, NULL, VE_PID_PL_IMPEX_METHOD_FUNC, nID, sizeof(nID));
					DATA_Add_Prop(hTmp, NULL, VE_PID_PL_IMPEX_METHOD_NAME, (AVP_dword) (PTCHAR) pObjectName->m_ObjectName, lstrlen((PTCHAR) pObjectName->m_ObjectName) + 1);
				}
				pItem = pItem->m_pCommonNext;
			}
				
		}
		
		// -----------------------------------------------------------------------------------------
		CPathStr ResourceName = pContext->m_OutputFolder;
		ResourceName ^= "Plugin.tr";
		KLDT_SerializeUsingSWM((PTCHAR) ResourceName, hRootData, "null");

		ResourceName = pContext->m_OutputFolder;
		ResourceName ^= "Plugin.rc";
		
		HANDLE hRecource = CreateFile((PTCHAR) ResourceName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hRecource == INVALID_HANDLE_VALUE)
		{
			DoOutput(false, pContext, "Can't create recource file '%s'. Last error %d", (PTCHAR) ResourceName, GetLastError());
			pContext->m_nErrorCount++;
		}
		else
		{
			DWORD dwWritten;
			CPathStr resources(MAX_PATH * 2);
			pContext->m_OutputFolder ^= "";
			wsprintf((PTCHAR)resources, "1 RCDATA  DISCARDABLE \"%sPlugin.tr\"", (PTCHAR) pContext->m_OutputFolder);
			WriteFile(hRecource, resources, lstrlen((PTCHAR) resources), &dwWritten, NULL);

			CloseHandle(hRecource);

			CompileResFile(pContext, (PTCHAR) ResourceName);
		}

		DATA_Remove(hRootData, NULL);
	}
}
