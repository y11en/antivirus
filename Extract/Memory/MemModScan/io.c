// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  09 April 2003,  11:35 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- io.c
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "io.h"
// AVP Prague stamp end

#include <stuff/MemManag.h>

#define pmc_MEMORY_PROCESS             ((tDWORD)(0x730f3626))
#define pm_MEMORY_PROCESS_REQUEST_DELETE ((tDWORD)(0x28cedca3))

//+ internal function
extern void ObjPtr_IOInit(hObjPtr _this, hIO hIo);
//- end

extern hROOT g_root;

#define LCASE(c) (c>='A'? (c<='Z'? (c|0x20) : c) : c)

tBOOL MatchOkay(char* Pattern)
{
	while (*Pattern == '*')
		Pattern++;
	if(*Pattern)
		return cFALSE;
	return cTRUE;
}

tBOOL MatchWithPattern(char* String, char* Pattern, tBOOL CaseSensetivity)
{
	char ch;
	if(*Pattern=='*')
	{
		Pattern++;
		while(*String && *Pattern)
		{
			if (CaseSensetivity == cTRUE)
				ch = *String;
			else
				ch = LCASE(*String);
			if ((ch == '*') || (ch == *Pattern) || (*Pattern == '?'))
			{
				if(MatchWithPattern(String+1, Pattern+1, CaseSensetivity)) 
					return cTRUE;
			}
			String++;
		}
		
		return MatchOkay(Pattern);
	} 
	
	while(*String && *Pattern != L'*')
	{
		if (CaseSensetivity == cTRUE)
			ch = *String;
		else
			ch = LCASE(*String);
		
		if((*Pattern == ch) || (*Pattern == '?'))
		{
			Pattern++;
			String++;
		} else
			return cFALSE;
	}
	
	if(*String)
		return MatchWithPattern(String, Pattern, CaseSensetivity);
	
	return MatchOkay(Pattern);
}

tBOOL
IsSame (
	char* pchFileName,
	char* pchPattern
	)
{
	char pchFileNamePattern[(MAX_PATH + 1 ) * 2];

	if (lstrlen(pchFileName) > 4)
	{
		if (pchFileName[0] == L'\\'
			&& pchFileName[1] == L'?'
			&& pchFileName[2] == L'?'
			&& pchFileName[3] == L'\\')
		{
			pchFileName += 4;
		}

		if (pchFileName[0] == L'\\'
			&& pchFileName[1] == L'\\'
			&& pchFileName[2] == L'?'
			&& pchFileName[3] == L'\\')
		{
			pchFileName += 4;
		}
	}

	ExpandEnvironmentStrings( pchPattern, pchFileNamePattern, sizeof(pchFileNamePattern));

	if (!lstrcmpi(pchFileName, pchFileNamePattern ))
		return cTRUE;

	return cFALSE;
}

void ReparseName(char* pchObjectPath, char* pPrefix, char* pexp);

tBOOL IsDeleteAllowed (ULONG Pid, HANDLE hProcess)
{
	tBOOL bAllow = cTRUE;
	HMODULE hPsapi;
	if (hProcess)
	{
		hPsapi = LoadLibrary("psapi.dll");
		if (hPsapi)
		{
			char pchFileName[(MAX_PATH + 1 ) * 2];

			DWORD (__stdcall* _pfnGetModuleFileNameExA)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize );

			*(void**)&_pfnGetModuleFileNameExA = GetProcAddress(hPsapi, "GetModuleFileNameExA");
			if (_pfnGetModuleFileNameExA)
			{
				if (_pfnGetModuleFileNameExA(hProcess, NULL, pchFileName, sizeof(pchFileName)))
				{
					ReparseName(pchFileName, "\\systemroot\\", "%WinDir%\\");
					ReparseName(pchFileName, "\\??\\", "");

					if (IsSame(pchFileName, "%windir%\\system32\\csrss.exe"))
						bAllow = cFALSE;
					else if (IsSame(pchFileName, "%windir%\\system32\\winlogon.exe"))
						bAllow = cFALSE;
					else if (IsSame(pchFileName, "%windir%\\system32\\lsass.exe"))
						bAllow = cFALSE;
					else if (IsSame(pchFileName, "%windir%\\system32\\services.exe"))
						bAllow = cFALSE;
					else if (IsSame(pchFileName, "%windir%\\system32\\svchost.exe"))
						bAllow = cFALSE;

//					PR_TRACE(( g_root, prtERROR, "mmsc\tchecking deleting process by name '%s': %s", 
//						pchFileName, bAllow ? "possible" : "impossible"));
				}
			}

			FreeLibrary(hPsapi);
			return bAllow;
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, "cannot load psapi.dll"));
		}
	}

	PR_TRACE(( g_root, prtERROR, "mmsc\tchecking deleting process by pid 0x%x: %s", Pid , bAllow ? "possible" : "impossible"));

	return bAllow;
}

// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
// Extended comment -
//  Defines behavior of input/output of an object
//  Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end

#include <mklif/fssync/fssync.h>

typedef struct _tDRV_DATA
{
	HMODULE						m_hFSSync;
	_tpfFSDrv_ProcessReadMemory	m_pfProcessReadMemory;
}DRV_DATA, *PDRV_DATA;


// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_IOPrivate 
{
	tDWORD m_ObjectSize;                    // размер объекта в памяти
	tCHAR  m_ObjectName[cNAME_MAX_LEN];     // имя модуля
	tCHAR  m_ObjectPath[cNAME_MAX_LEN];     // имя файла из которого был загружен модуль
	tDWORD m_PID;                           // идентификатор процесса, загрузившего модуль
	HANDLE m_hModHandle;                    // хэндл модуля
	HANDLE m_hProcess;                      // хэндл процесса
	tDWORD m_ObjectOpenMode;                // режим открытия модуля в памяти
	tDWORD m_AccessMode;                    // режим доступа к модулю в памяти
	tBOOL  m_DeleteOnClose;                 // признак необходимости удаления модуля при завершении работы с ним
	tCHAR  m_ObjectFullName[cNAME_MAX_LEN]; // полный путь к модулю в памяти
	tDWORD m_NativeError;                   // --
	hOBJECT m_BasedOn;                       // --
	HANDLE  m_SyncG;
	tDWORD m_Avail;

	tQWORD m_ImageBase;

	DRV_DATA	m_DrvData;
} IOPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	IOPrivate*         data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectClose( hi_IO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       IO_ObjectInit,
	(tIntFnObjectInitDone)   IO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      IO_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpIO_SeekRead)  ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
typedef   tERROR (pr_call *fnpIO_SeekWrite) ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
typedef   tERROR (pr_call *fnpIO_GetSize)   ( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );         // -- returns size of the requested type of the object;
typedef   tERROR (pr_call *fnpIO_SetSize)   ( hi_IO _this, tQWORD new_size );           // -- resizes the object;
typedef   tERROR (pr_call *fnpIO_Flush)     ( hi_IO _this );                            // -- flush internal buffers;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iIOVtbl 
{
	fnpIO_SeekRead   SeekRead;
	fnpIO_SeekWrite  SeekWrite;
	fnpIO_GetSize    GetSize;
	fnpIO_SetSize    SetSize;
	fnpIO_Flush      Flush;
}; // "IO" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iIOVtbl e_IO_vtbl = 
{
	IO_SeekRead,
	IO_SeekWrite,
	IO_GetSize,
	IO_SetSize,
	IO_Flush
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call IO_PROP_CommonPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_CommonPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// Interface "IO". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "IO interface for access to module's memory", 43 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_MEMORY_PROCESS_MODULE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_AVAILABILITY, IOPrivate, m_Avail, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, IOPrivate, m_BasedOn, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PATH, IOPrivate, m_ObjectPath, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, IOPrivate, m_ObjectName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, IOPrivate, m_ObjectFullName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, IOPrivate, m_DeleteOnClose, cPROP_BUFFER_READ_WRITE)
	mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, IOPrivate, m_AccessMode, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, IOPrivate, m_ObjectOpenMode, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plIO_HMODULE, IOPrivate, m_hModHandle, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( plIO_PID, IOPrivate, m_PID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( pgNATIVE_ERR, IOPrivate, m_NativeError, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_BUF( plSize, IOPrivate, m_ObjectSize, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( plImageBase, IOPrivate, m_ImageBase, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) 
{
	tERROR error;
	

	PR_TRACE_A0( root, "Enter \"IO::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_MEMMODSCAN,                         // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(IOPrivate),                      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"IO::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_CommonPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method IO_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA" );

	if (size <= sizeof(DWORD))
	{
		error = errBUFFER_TOO_SMALL;
		if (out_size)
		{
			tCHAR  buffertmp[sizeof(_this->data->m_ObjectPath)];
			tDWORD out_size_tmp = 0;
			error = CALL_SYS_PropertyGetStr(_this, &out_size_tmp, pgOBJECT_PATH, buffertmp, sizeof(buffertmp), cCP_ANSI);
			if (PR_SUCC(error))
				*out_size = out_size_tmp + sizeof(DWORD);

			PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tIO requset - need buffer size %d", *out_size));
		}
	}
	else
	{
		*(DWORD*)buffer = _this->data->m_PID;
		error = CALL_SYS_PropertyGetStr(_this, out_size, pgOBJECT_PATH, buffer + sizeof(DWORD), size - sizeof(DWORD), cCP_ANSI);
		*out_size += sizeof(DWORD);

		PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tIORequest reopen data: pid %d, len %d", _this->data->m_PID, *out_size));
	}

	PR_TRACE_A2( _this, "Leave *GET* method IO_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_CommonPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA" );

	*out_size = size;
	if (size <= sizeof(DWORD))
		error = errBUFFER_TOO_SMALL;
	else
	{
		hObjPtr hPtr = 0;
		tCHAR ObjectName[cNAME_MAX_LEN];
		tCHAR ObjectNameTmp[cNAME_MAX_LEN];
		
		DWORD Pid = *(DWORD*) buffer;
		lstrcpyn(ObjectName, buffer + sizeof(DWORD), size > cNAME_MAX_LEN ? cNAME_MAX_LEN : size - sizeof(DWORD));

		PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tIoIncome reopen data: pid %d, len %d", Pid, size));

		if (lstrlen(ObjectName) == 0)
			error = errUNEXPECTED;
		else
		{
			error = errNOT_FOUND;

			if (PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &hPtr, IID_OBJPTR, PID_MEMMODSCAN, 0)))
			{
				while (PR_SUCC(CALL_ObjPtr_Next(hPtr)))
				{
					if (Pid != CALL_SYS_PropertyGetDWord(hPtr, plENUM_PID))
						continue;

					if (PR_FAIL(CALL_SYS_PropertyGetStr(hPtr, 0, pgOBJECT_PATH, ObjectNameTmp, sizeof(ObjectNameTmp), cCP_ANSI)))
						continue;

					if (lstrcmp(ObjectName, ObjectNameTmp) != 0)
						continue;


					ObjPtr_IOInit(hPtr, (hIO)_this);
					error = errOK;
					break;
				}
				CALL_SYS_ObjectClose(hPtr);
			}
		}
	}

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call IO_ObjectInit( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::ObjectInit\" method" );

	// Place your code here
	error = errOK;
	_this->data->m_PID = -1;
	_this->data->m_hProcess = NULL;
	_this->data->m_hModHandle = (HANDLE) -1;
	_this->data->m_NativeError = ERROR_SUCCESS;
	_this->data->m_AccessMode = fACCESS_RW;
	_this->data->m_SyncG = CreateSemaphore(0, 1, 1, "MEM_BHLA");

	error = errNOT_SUPPORTED;
	{
		hOBJECT hParent = CALL_SYS_ParentGet(_this, IID_OS);
		if (hParent != 0)
		{
			if (PR_SUCC(CALL_SYS_ObjectCheck(_this, hParent, IID_OS, PID_MEMMODSCAN, SUBTYPE_ANY, cFALSE)))
			{
				_this->data->m_BasedOn = hParent;
				error = errOK;
			}
		}
	}

	{
		OSVERSIONINFO OsVersionInfo;

		ZeroMemory(&OsVersionInfo, sizeof(OSVERSIONINFO));
		OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx(&OsVersionInfo))
		{
			if (VER_PLATFORM_WIN32_NT == OsVersionInfo.dwPlatformId)
			{
				_this->data->m_DrvData.m_hFSSync = LoadLibrary("FSSync.dll");

				if (_this->data->m_DrvData.m_hFSSync)
				{
					_this->data->m_DrvData.m_pfProcessReadMemory = (_tpfFSDrv_ProcessReadMemory) GetProcAddress(
						_this->data->m_DrvData.m_hFSSync,
						"FSSync_PRM"
						);

					if (!_this->data->m_DrvData.m_pfProcessReadMemory)
					{
						FreeLibrary(_this->data->m_DrvData.m_hFSSync);
						_this->data->m_DrvData.m_hFSSync = NULL;
					}
				}
			}
		}
	}

	PR_TRACE_A1( _this, "Leave \"IO::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IO_ObjectInitDone( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::ObjectInitDone\" method" );

	// Place your code here
	error = errOK;

	if ((_this->data->m_PID == -1) || (_this->data->m_hModHandle == (HANDLE) -1))
	{
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	else
	{
		if (!_this->data->m_hModHandle)
		{
			if (_this->data->m_DrvData.m_pfProcessReadMemory)
			{
				DWORD dwAccessMask = PROCESS_TERMINATE;
				_this->data->m_hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);

				PR_TRACE(( _this, prtERROR, "MemModsc\tOpenProcess %d winerr 0x%x", _this->data->m_PID, GetLastError()));

				_this->data->m_Avail = fAVAIL_READ | fAVAIL_WRITE;

				if ( _this->data->m_hProcess)
					_this->data->m_Avail |= fAVAIL_DELETE_ON_CLOSE;

				error = errOK;
			}
		}
		else
		{
			DWORD dwAccessMask = PROCESS_ALL_ACCESS;//PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_TERMINATE;
			_this->data->m_hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);
			if(_this->data->m_hProcess == NULL)
			{
				HANDLE hTokenThread = NULL;
				if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &hTokenThread))
				{
					RevertToSelf();
					_this->data->m_hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);
					if(_this->data->m_hProcess == NULL)
					{
						error = errOBJECT_CANNOT_BE_INITIALIZED;
						PR_TRACE(( _this, prtERROR, "MemModsc\tOpenProcess after impesronate failed winerr 0x%x", GetLastError()));
					}

					SetThreadToken(NULL, hTokenThread);
				}
				else
				{
					PR_TRACE(( _this, prtERROR, "MemModsc\tOpenThreadToken failed winerr 0x%x", GetLastError()));
				}
			}

			if ( _this->data->m_hProcess)
			{
				_this->data->m_Avail = fAVAIL_READ | fAVAIL_WRITE;
			
				if (IsDeleteAllowed(_this->data->m_PID, _this->data->m_hProcess))
					 _this->data->m_Avail |= fAVAIL_DELETE_ON_CLOSE;
			}
		}
	}
	
	PR_TRACE_A1( _this, "Leave \"IO::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call IO_ObjectClose( hi_IO _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"IO::ObjectClose\" method" );

	// Place your code here
	_this->data->m_NativeError = ERROR_SUCCESS;

	if (_this->data->m_DeleteOnClose)
	{
		if (_this->data->m_DrvData.m_pfProcessReadMemory)
		{
			// skip check
		}
		else
		{
			OSVERSIONINFO OsVersionInfo;
			ZeroMemory(&OsVersionInfo, sizeof(OSVERSIONINFO));
			OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			
			if (GetVersionEx(&OsVersionInfo))
			{
				if (VER_PLATFORM_WIN32_NT == OsVersionInfo.dwPlatformId)
				{
					if (!IsDeleteAllowed(_this->data->m_PID, _this->data->m_hProcess))
					{
						_this->data->m_DeleteOnClose = cFALSE;
					}
				}
			}
		}
	}

	if (_this->data->m_DeleteOnClose == TRUE)
	{
		tDWORD dwSize = sizeof(_this->data->m_PID);
		CALL_SYS_SendMsg(_this, pmc_MEMORY_PROCESS, pm_MEMORY_PROCESS_REQUEST_DELETE, _this, &_this->data->m_PID, &dwSize);
		if (TerminateProcess(_this->data->m_hProcess, 0) == 0)
		{
			tDWORD del_err;
			tDWORD size = sizeof(del_err);

			del_err = errNOT_OK;
			error = errIO_DELETE_ON_CLOSE_FAILED;
			_this->data->m_NativeError = GetLastError();

			CALL_SYS_SendMsg(_this, pmc_IO, pm_IO_DELETE_ON_CLOSE_FAILED, 0, &del_err, &size);
		}
		else
			CALL_SYS_SendMsg(_this, pmc_IO, pm_IO_DELETE_ON_CLOSE_SUCCEED, 0, 0, 0);
	}
	
	if (_this->data->m_hProcess != NULL)
		CloseHandle(_this->data->m_hProcess);

	if(_this->data->m_SyncG)
		CloseHandle(_this->data->m_SyncG);

	if (_this->data->m_DrvData.m_hFSSync)
	{
		FreeLibrary(_this->data->m_DrvData.m_hFSSync);
		_this->data->m_DrvData.m_hFSSync = NULL;
	}


	PR_TRACE_A1( _this, "Leave \"IO::ObjectClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



int MemModException(DWORD exception_code)
{
	OutputDebugString( "MemModScan : Exception Memory Read\n" );
	return EXCEPTION_EXECUTE_HANDLER;
}

// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//  reads content of the object to the buffer. Returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	SIZE_T ret_val = 0;
	PR_TRACE_A0( _this, "Enter \"IO::SeekRead\" method" );

	// Place your code here
	if(_this->data->m_SyncG)
		WaitForSingleObject(_this->data->m_SyncG, INFINITE);

	_this->data->m_NativeError = ERROR_SUCCESS;
	__try
	{
		error = errOK;
		if (offset < _this->data->m_ObjectSize)
		{
			BOOL bRead;
			if (offset + size > _this->data->m_ObjectSize)
				size = _this->data->m_ObjectSize - (DWORD) offset;
			ret_val = 0;

			if (_this->data->m_DrvData.m_pfProcessReadMemory)
			{
				LARGE_INTEGER new_offset;
				new_offset.QuadPart = _this->data->m_ImageBase + offset;
				bRead = _this->data->m_DrvData.m_pfProcessReadMemory ( 
				_this->data->m_PID,
				new_offset,
				buffer,
				size,
				&ret_val
				);
			}
			else
				bRead = ReadProcessMemory(_this->data->m_hProcess, (PVOID)(offset + (DWORD)_this->data->m_hModHandle), buffer, size, &ret_val);

			if (!bRead)
			{
				if (ret_val == 0)
				{
					_this->data->m_NativeError = GetLastError();
					PR_TRACE(( _this, prtERROR, "can't read module memory (size = %d). last error %#x", size, _this->data->m_NativeError));
					error = errOBJECT_READ;
				}
			}
		}
		else
			error = errEOF;
		
	}
	__except(MemModException(_exception_code()))
	{
		error = errOBJECT_READ;
	}

	if(_this->data->m_SyncG)
		ReleaseSemaphore(_this->data->m_SyncG, 1, 0);

	if ( result )
		*result = (tDWORD)ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::SeekRead\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//  writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	SIZE_T ret_val = 0;
	PR_TRACE_A0( _this, "Enter \"IO::SeekWrite\" method" );

	// Place your code here
	_this->data->m_NativeError = ERROR_SUCCESS;
	error = errOK;
	{
		tDWORD OldProtection;
		tDWORD _OldProtection;
		
		if (offset + size < _this->data->m_ObjectSize)
		{
			VirtualProtectEx(_this->data->m_hProcess, (PVOID)(offset + (DWORD)_this->data->m_hModHandle), size, PAGE_EXECUTE_READWRITE, &OldProtection);
			if(!WriteProcessMemory(_this->data->m_hProcess, (PVOID)(offset + (DWORD)_this->data->m_hModHandle), buffer, size, &ret_val))
				error = errOBJECT_WRITE;
			
			VirtualProtectEx(_this->data->m_hProcess, (PVOID)(offset + (DWORD)_this->data->m_hModHandle), size, OldProtection, &_OldProtection);
		}
		else
		{
			error = errEOF;
			_this->data->m_NativeError = GetLastError();
		}
	}
	
	if ( result )
		*result = (tDWORD)ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::SeekWrite\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//  object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
// "type" : Size type is one of the following:
//            -- explicit
//            -- approximate
//            -- estimate
//
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter \"IO::GetSize\" method" );

	// Place your code here
	if (result != NULL)
	{
		error = errOK;
		ret_val = _this->data->m_ObjectSize;
	}
	else
		error = errPARAMETER_INVALID;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::GetSize\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::SetSize\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"IO::SetSize\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//  Flush internal buffers.
// Behaviour comment
//  Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call IO_Flush( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::Flush\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"IO::Flush\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



