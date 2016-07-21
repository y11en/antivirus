// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  13 March 2003,  15:32 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// File Name   -- objptr.c
// Additional info
//    Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "io.h"
#include "objptr.h"

#include "../MemModScan/os.h"
#include <mklif/fssync/fssync.h>
#include <stuff/memmanag.h>

#include <windows.h>

// AVP Prague stamp end

void FS_PROC cbEnumProcessesCallback (
	PVOID dwContext,
	ULONG pid,
	ULONG parent_pid,
	LARGE_INTEGER start_time,
	PWCHAR pwchImagePath,
	PWCHAR pwchCmdLine,
	PWCHAR pwchCurrDir
	);

// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end


typedef struct _tSimpleProcInfo
{
	struct _tSimpleProcInfo*	m_pNext;
	ULONG				m_PID;                           // идентификатор процесса
	ULONG				m_ParentPID;                     // идентификатор родительского процесса
	LARGE_INTEGER		m_StartTime;
	PWCHAR				m_pwchImagePath;
}SimpleProcInfo, *PSimpleProcInfo;

typedef struct _tENUM_DRV
{
	HMODULE					m_hFSSync;
	_tpfFSDrv_EnumProcesses	m_pfEnum;
	PSimpleProcInfo			 m_pProcesses;
	PSimpleProcInfo			 m_pCurrent;
}ENUM_DRV, *PENUM_DRV;

PWCHAR GetProcNameOffset(PWCHAR pwchImagePath)
{
	int len = lstrlenW(pwchImagePath);

	while (len)
	{
		if (pwchImagePath[len] == L'\\')
			return pwchImagePath + len + 1;

		len--;
	}

	return pwchImagePath;
}

char*
SkipUnicodePrefix (
	char* pchPath
	)
{
	int len = lstrlen(pchPath);
	if (len < 4)
		return pchPath;

	if (pchPath[0] == '\\' && pchPath[1] == '\\' && pchPath[2] == '?' && pchPath[3] == '\\')
		return pchPath + 4;

	if (pchPath[0] == '\\' && pchPath[1] == '?' && pchPath[2] == '?' && pchPath[3] == '\\')
		return pchPath + 4;

	return pchPath;
}

void
ReparsePath (
	char* pchPath
	)
{
	int len = lstrlen(pchPath);
	char* pchNewPath;
	if (!len)
		return;

	pchNewPath = _MM__Manage_Alloc( len + 1 + MAX_PATH );
	if (!pchNewPath)
		return;

	lstrcpy(pchNewPath, SkipUnicodePrefix(pchPath));

	if (len > sizeof("\\SystemRoot\\"))
	{
		if (!memcmp(pchNewPath, "\\SystemRoot\\", sizeof("\\SystemRoot\\") - 1))
		{
			memcpy(pchNewPath, "   %windir%", sizeof("   %windir%") - 1);
			
			ExpandEnvironmentStrings(pchNewPath, pchPath, len + MAX_PATH);
			lstrcpy(pchNewPath, pchPath + 3);
		}
	}

	lstrcpy(pchPath, pchNewPath);

	_MM__Manage_Free(pchNewPath);
}

// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_PM_ObjPtrPrivate 
{
	struct tag_ENUM_NT 
	{
		PCHAR         m_pInfoBuff;    // блок данных операционной системы по процессам
		PROCESS_INFO* m_pNextProcess; // блок данных по текущему процессу
	} ENUM_NT;

	struct tag_ENUM_9x 
	{
		HANDLE         m_hProcessSnap; // блок данных операционной системы по процессам
		PROCESSENTRY32 m_PE32;         // блок данных по текущему процессу
	} ENUM_9x;

	tDWORD        m_PID;                           // идентификатор процесса
	tDWORD        m_ParentPID;                     // идентификатор родительского процесса
	tDWORD        m_ThreadsNum;                    // число потоков в текущем процессе
	tCHAR         m_ObjectName[cNAME_MAX_LEN];     // имя процесса
	tCHAR         m_ObjectPath[cNAME_MAX_LEN];     // расположение объекта
	tCHAR         m_ObjectFullName[cNAME_MAX_LEN]; // полный путь к файлу из которого запущен текущий процесс
	tCHAR         m_Mask[cNAME_MAX_LEN];           // маска для перебора объектов (в данной реализации не поддерживается)
	OSVERSIONINFO m_OsVersionInfo;                 // описание операционной системы
	tDWORD        m_Size;                          // размер процесса в памяти

	ENUM_DRV	m_ENUM_DRV;
} PM_ObjPtrPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	PM_ObjPtrPrivate*  data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
  // OBJECT DESCRIPTION SECTION TRANSFERRED TO CORRESPONDING HEADER FILE
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       ObjPtr_ObjectInit,
	(tIntFnObjectInitDone)   NULL,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      ObjPtr_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
//











// ----------- operations with the current object ----------
typedef   tERROR (pr_call *fnpObjPtr_IOCreate) ( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ); // -- creates object current in enumeration;
typedef   tERROR (pr_call *fnpObjPtr_Copy)     ( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite ); // -- copies the object pointed by the ptr;
typedef   tERROR (pr_call *fnpObjPtr_Rename)   ( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite ); // -- renames the object pointed by the ptr;
typedef   tERROR (pr_call *fnpObjPtr_Delete)   ( hi_ObjPtr _this );                         // -- deletes object pointed by ptr;

// ----------- navigation methods ----------
typedef   tERROR (pr_call *fnpObjPtr_Reset)    ( hi_ObjPtr _this, tBOOL to_root );          // -- Reinitializing of ObjEnum object;
typedef   tERROR (pr_call *fnpObjPtr_Clone)    ( hi_ObjPtr _this, hObjPtr* result );                         // -- Makes another ObjPtr object;
typedef   tERROR (pr_call *fnpObjPtr_Next)     ( hi_ObjPtr _this );                         // -- Returns next IO object;
typedef   tERROR (pr_call *fnpObjPtr_StepUp)   ( hi_ObjPtr _this );                         // -- Goes one level up;
typedef   tERROR (pr_call *fnpObjPtr_StepDown) ( hi_ObjPtr _this );                         // -- Goes one level up;
typedef   tERROR (pr_call *fnpObjPtr_ChangeTo) ( hi_ObjPtr _this, hOBJECT name );           // -- Changes folder for enumerating;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )












struct iObjPtrVtbl 
{

// ----------- operations with the current object ----------
	fnpObjPtr_IOCreate  IOCreate;
	fnpObjPtr_Copy      Copy;
	fnpObjPtr_Rename    Rename;
	fnpObjPtr_Delete    Delete;

// ----------- navigation methods ----------
	fnpObjPtr_Reset     Reset;
	fnpObjPtr_Clone     Clone;
	fnpObjPtr_Next      Next;
	fnpObjPtr_StepUp    StepUp;
	fnpObjPtr_StepDown  StepDown;
	fnpObjPtr_ChangeTo  ChangeTo;
}; // "ObjPtr" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
//
//












// operations with the current object
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this );

// navigation methods
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root );
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result );
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iObjPtrVtbl e_ObjPtr_vtbl = 
{
	ObjPtr_IOCreate,
	ObjPtr_Copy,
	ObjPtr_Rename,
	ObjPtr_Delete,
	ObjPtr_Reset,
	ObjPtr_Clone,
	ObjPtr_Next,
	ObjPtr_StepUp,
	ObjPtr_StepDown,
	ObjPtr_ChangeTo
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call ObjPtr_PROP_GetObjectState( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetObjectSizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetObjectAttributes( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_CommonPropRead( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_CommonPropWrite( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



tSTRING g_ObjPtr_path = "Windows memory";

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// Interface "ObjPtr". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// Interface "ObjPtr". Property table
mPROPERTY_TABLE(ObjPtr_PropTable)
	mLOCAL_PROPERTY_FN( pgOBJECT_PTR_STATE, ObjPtr_PROP_GetObjectState, NULL )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_MEMORY_PROCESS)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_SIZE, PM_ObjPtrPrivate, m_Size, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, ObjPtr_PROP_GetObjectAttributes, NULL )
	mLOCAL_PROPERTY_BUF( plTHREADS_NUM, PM_ObjPtrPrivate, m_ThreadsNum, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Process ptr", 12 )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PATH, PM_ObjPtrPrivate, m_ObjectPath, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, PM_ObjPtrPrivate, m_ObjectName, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, PM_ObjPtrPrivate, m_ObjectFullName, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgMASK, PM_ObjPtrPrivate, m_Mask, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plPID, PM_ObjPtrPrivate, m_PID, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plPARENT_PID, PM_ObjPtrPrivate, m_ParentPID, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, ObjPtr_PROP_CommonPropRead, ObjPtr_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_FN( pgOBJECT_HASH, ObjPtr_PROP_CommonPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_GetObjectSizeQ, NULL )
mPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "ObjPtr". Register function
tERROR pr_call ObjPtr_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"ObjPtr::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OBJPTR,                             // interface identifier
		PID_MEMSCAN,                             // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
		VID_SOBKO,                              // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(PM_ObjPtrPrivate),               // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"ObjPtr::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectState )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PTR_STATE
tERROR pr_call ObjPtr_PROP_GetObjectState( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"ObjPtr_PROP_GetObjectState\" for property \"pgOBJECT_PTR_STATE\"" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_GetObjectState\" for property \"pgOBJECT_PTR_STATE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectSize )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_GetObjectSize( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"ObjPtr_PROP_GetObjectSize\" for property \"pgOBJECT_SIZE\"" );

	*out_size = sizeof(DWORD);
	if (buffer != NULL)
		*(DWORD*) buffer = _this->data->m_Size;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_GetObjectSize\" for property \"pgOBJECT_SIZE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectSizeQ )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_GetObjectSizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"ObjPtr_PROP_GetObjectSizeQ\" for property \"pgOBJECT_SIZE_Q\"" );

	*out_size = sizeof(tQWORD);
	*(tQWORD*)buffer = _this->data->m_Size;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_GetObjectSizeQ\" for property \"pgOBJECT_SIZE_Q\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectAttributes )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_GetObjectAttributes( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"ObjPtr_PROP_GetObjectAttributes\" for property \"pgOBJECT_ATTRIBUTES\"" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_GetObjectAttributes\" for property \"pgOBJECT_ATTRIBUTES\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
//	-- OBJECT_HASH
tERROR pr_call ObjPtr_PROP_CommonPropRead( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA" );

	switch (prop)
	{
	case pgOBJECT_REOPEN_DATA:
		{
			tDWORD dwSize = sizeof(DWORD) + lstrlen(_this->data->m_ObjectName) + 1;
			if (buffer != NULL)
			{
				if (dwSize < size)
					error = errBUFFER_TOO_SMALL;
				else
				{
					*(DWORD*)buffer = _this->data->m_PID;
					lstrcpy((char*)buffer + sizeof(DWORD), _this->data->m_ObjectName);
				}
			}
			*out_size = dwSize;
		}
		break;
	case pgOBJECT_HASH:
		error = errNOT_SUPPORTED;
		*out_size = 0;
		break;
	}


	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropWrite )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call ObjPtr_PROP_CommonPropWrite( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA" );

	*out_size = 0;
	error = errNOT_SUPPORTED;

	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::ObjectInit\" method" );

	// Place your code here
	error = errOK;

	_this->data->ENUM_NT.m_pInfoBuff = NULL;
	_this->data->ENUM_NT.m_pNextProcess = NULL;
	_this->data->m_ThreadsNum = 0;
	lstrcpy( _this->data->m_ObjectPath, g_ObjPtr_path );
	
	_this->data->ENUM_9x.m_hProcessSnap = (HANDLE) -1;
	_this->data->ENUM_9x.m_PE32.dwSize = 0;
	
	ZeroMemory(&_this->data->m_OsVersionInfo, sizeof(OSVERSIONINFO));
	_this->data->m_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	PR_TRACE(( g_root, prtIMPORTANT, "memscan\tinit"));
	
	if (GetVersionEx(&_this->data->m_OsVersionInfo) == FALSE)
		error = errUNEXPECTED;
	else
	{
		error = errOBJECT_CANNOT_BE_INITIALIZED;

		if (VER_PLATFORM_WIN32_NT == _this->data->m_OsVersionInfo.dwPlatformId)
		{
			_this->data->m_ENUM_DRV.m_hFSSync = LoadLibrary("FSSync.dll");

			if (_this->data->m_ENUM_DRV.m_hFSSync)
			{
				_this->data->m_ENUM_DRV.m_pfEnum = (_tpfFSDrv_EnumProcesses) GetProcAddress(
					_this->data->m_ENUM_DRV.m_hFSSync,
					"FSSync_PEn"
					);

				if (_this->data->m_ENUM_DRV.m_pfEnum)
				{
					if (_this->data->m_ENUM_DRV.m_pfEnum(&_this->data->m_ENUM_DRV, cbEnumProcessesCallback))
					{
						// test enum success
						PR_TRACE(( g_root, prtIMPORTANT, "memscan\ttest enum success" ));
					}
					else
					{
						// use old method
						_this->data->m_ENUM_DRV.m_pfEnum = NULL;
					}
				}

				if (_this->data->m_ENUM_DRV.m_pfEnum)
					error = errOK;
				else
				{
					FreeLibrary(_this->data->m_ENUM_DRV.m_hFSSync);
					_this->data->m_ENUM_DRV.m_hFSSync = NULL;
				}
			}
		}
		
		if (!PR_SUCC(error))
			error = ObjPtr_Reset(_this, TRUE);

		PR_TRACE(( g_root, prtIMPORTANT, "memscan\tinit %terr",  error ));
	}
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::ObjectClose\" method" );

	// Place your code here
	_this->data->m_PID = (tDWORD) -1;
	_this->data->m_ParentPID = (tDWORD) -1;
	_this->data->m_Size = 0;
	
	if (_this->data->ENUM_NT.m_pInfoBuff != NULL)
	{
		_MM__Manage_Free(_this->data->ENUM_NT.m_pInfoBuff);
		_this->data->ENUM_NT.m_pInfoBuff = NULL;
	}
	
	if (_this->data->ENUM_9x.m_hProcessSnap != (HANDLE) -1)
	{
		CloseHandle(_this->data->ENUM_9x.m_hProcessSnap);
		_this->data->ENUM_9x.m_hProcessSnap = (HANDLE) -1;
	}
	_this->data->ENUM_9x.m_PE32.dwSize = 0;

	if (_this->data->m_ENUM_DRV.m_hFSSync)
	{
		ObjPtr_Reset(_this, cFALSE);
		FreeLibrary(_this->data->m_ENUM_DRV.m_hFSSync);
		_this->data->m_ENUM_DRV.m_hFSSync = NULL;
	}
	

	PR_TRACE_A1( _this, "Leave \"ObjPtr::ObjectClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ObjectAttributes )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_ObjectAttributes( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"ObjPtr_PROP_ObjectAttributes\" for property \"pgOBJECT_ATTRIBUTES\"" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_ObjectAttributes\" for property \"pgOBJECT_ATTRIBUTES\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  Creates hIO or hObjEnum object current in enumeration.
// Result comment
//  Returns hIO or hObjEnum object. Depends on current object.
// Parameters are:
// "access_mode" : New object open mode
// "open_mode"   : Attributes of the new object
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode )
{
	tERROR error = errNOT_IMPLEMENTED;
	hIO    ret_val = NULL;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::IOCreate\" method" );

	// Place your code here
	PR_TRACE((g_root, prtIMPORTANT, "memsc: createio"));

	if (_this->data->m_PID != -1)
	{
		error = CALL_SYS_ObjectCreate(_this, (hOBJECT*) &ret_val, IID_IO, PID_MEMSCAN, 0);
		if (ret_val != NULL)
		{
			tDWORD NameSize;
			tDWORD PropSize;

			CALL_SYS_PropertySet(ret_val, &PropSize, plProcessID, &_this->data->m_PID, sizeof(tDWORD));
			if (PropSize == 0)
			{
				PR_TRACE(((hOBJECT) _this, prtIMPORTANT, "Can't set PID for IO! Error is %d", error));
			}
			
			CALL_SYS_PropertySetStr(ret_val, &NameSize, pgOBJECT_NAME, _this->data->m_ObjectName, lstrlen(_this->data->m_ObjectName), cCP_ANSI);

			error = CALL_SYS_ObjectCreateDone(ret_val);
			if (PR_FAIL(error))
			{
				PR_TRACE(((hOBJECT) _this, prtIMPORTANT, "MemScan: Create IO failed!"));
				CALL_SYS_ObjectClose(ret_val);
				ret_val = NULL;
			}
		}
	}
	
	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"ObjPtr::IOCreate\" method, ret hIO = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Copy\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"ObjPtr::Copy\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Rename\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"ObjPtr::Rename\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//  pointer must be advanced to the next object
// Parameters are:
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Delete\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"ObjPtr::Delete\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Extended method comment
//  First call of Next method after Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Reset\" method" );

	// Place your code here
	if (_this->data->m_ENUM_DRV.m_pfEnum)
	{
		PSimpleProcInfo pTmp = NULL;

		while (_this->data->m_ENUM_DRV.m_pProcesses)
		{
			pTmp = _this->data->m_ENUM_DRV.m_pProcesses;

			_this->data->m_ENUM_DRV.m_pProcesses = _this->data->m_ENUM_DRV.m_pProcesses->m_pNext;

			if (pTmp->m_pwchImagePath)
				_MM__Manage_Free(pTmp->m_pwchImagePath);

			_MM__Manage_Free(pTmp);
		}

		_this->data->m_ENUM_DRV.m_pCurrent = NULL;

		return errOK;
	}

	error = ObjPtr_ObjectClose(_this);
	switch (_this->data->m_OsVersionInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (GetNTProcessInfo(&_this->data->ENUM_NT.m_pInfoBuff) == TRUE)
		{
			_this->data->ENUM_NT.m_pNextProcess = NULL;
			error = errOK;
		}
		else
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		_this->data->ENUM_9x.m_hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(_this->data->ENUM_9x.m_hProcessSnap == (HANDLE) -1)
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		else
			error = errOK;
		break;
	default:
		error = errNOT_SUPPORTED;
		break;
	}

	PR_TRACE_A1( _this, "Leave \"ObjPtr::Reset\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// Extended method comment
//  Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameters are:
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result )
{
	tERROR  error = errNOT_IMPLEMENTED;
	hObjPtr ret_val = NULL;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Clone\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"ObjPtr::Clone\" method, ret hObjPtr = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Behaviour comment
//  First call of Next method after creating or Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this )
{
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Next\" method" );

	// Place your code here
	error = errEND_OF_THE_LIST;

	if (_this->data->m_ENUM_DRV.m_pfEnum)
	{
		if (!_this->data->m_ENUM_DRV.m_pProcesses)
		{
			_this->data->m_ENUM_DRV.m_pfEnum(&_this->data->m_ENUM_DRV, cbEnumProcessesCallback);
			_this->data->m_ENUM_DRV.m_pCurrent = NULL;

			if (!_this->data->m_ENUM_DRV.m_pProcesses)
			{
				PR_TRACE(( g_root, prtIMPORTANT, "memscan\tno processes found. empty list" ));
			}
		}

		if (_this->data->m_ENUM_DRV.m_pProcesses)
		{
			if (_this->data->m_ENUM_DRV.m_pCurrent)
				_this->data->m_ENUM_DRV.m_pCurrent = _this->data->m_ENUM_DRV.m_pCurrent->m_pNext;
			else
				_this->data->m_ENUM_DRV.m_pCurrent = _this->data->m_ENUM_DRV.m_pProcesses;
				
			if (_this->data->m_ENUM_DRV.m_pCurrent)
			{
				_this->data->m_PID = _this->data->m_ENUM_DRV.m_pCurrent->m_PID;

				_this->data->m_ParentPID = _this->data->m_ENUM_DRV.m_pCurrent->m_ParentPID;

				wsprintf(_this->data->m_ObjectName, "%S", GetProcNameOffset(_this->data->m_ENUM_DRV.m_pCurrent->m_pwchImagePath));

				if (lstrlenW(_this->data->m_ENUM_DRV.m_pCurrent->m_pwchImagePath) + 1 > sizeof(_this->data->m_ObjectPath))
					wsprintf(_this->data->m_ObjectPath, "<not resolved>" );
				else
				{
					int lentmp;
					wsprintf(_this->data->m_ObjectPath, "%S", _this->data->m_ENUM_DRV.m_pCurrent->m_pwchImagePath);
					lentmp = lstrlen(_this->data->m_ObjectPath);
					while (lentmp)
					{
						if (_this->data->m_ObjectPath[lentmp] == '\\')
						{
							_this->data->m_ObjectPath[lentmp] = 0;
							break;
						}
						_this->data->m_ObjectPath[lentmp] = 0;
						lentmp--;
					}

				}

				if (lstrlenW(_this->data->m_ENUM_DRV.m_pCurrent->m_pwchImagePath) + 1 > sizeof(_this->data->m_ObjectFullName))
					wsprintf(_this->data->m_ObjectFullName, "<not resolved>" );
				else
				{
					wsprintf(_this->data->m_ObjectFullName, "%S", _this->data->m_ENUM_DRV.m_pCurrent->m_pwchImagePath);
					ReparsePath(_this->data->m_ObjectFullName);
				}

				PR_TRACE((g_root, prtNOTIFY, "ObjPtr(IID_OBJPTR) enum 0x%x (%d) '%s'", 
					_this->data->m_PID,
					_this->data->m_PID,
					_this->data->m_ObjectFullName) );

				error = errOK;
			}
			else
			{
				PR_TRACE((g_root, prtNOTIFY, "ObjPtr(IID_OBJPTR) end of the list" ));
			}
		}

		return error;
	}
	
	_this->data->m_ThreadsNum = 0;
	_this->data->m_PID = (tDWORD) -1;
	_this->data->m_ParentPID = (tDWORD) -1;
	
	switch (_this->data->m_OsVersionInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
	{
			if (_this->data->ENUM_NT.m_pNextProcess == NULL)
				_this->data->ENUM_NT.m_pNextProcess = (PROCESS_INFO*) _this->data->ENUM_NT.m_pInfoBuff;
			
			if(_this->data->ENUM_NT.m_pInfoBuff != NULL)
			{
				if (_this->data->ENUM_NT.m_pNextProcess->Len != -1)
				{
					_this->data->m_PID = _this->data->ENUM_NT.m_pNextProcess->PID;
					_this->data->m_ParentPID = _this->data->ENUM_NT.m_pNextProcess->ParentPID;
					_this->data->m_ThreadsNum = _this->data->ENUM_NT.m_pNextProcess->ThreadsNum;
					_this->data->m_Size = _this->data->ENUM_NT.m_pNextProcess->WorkingSet;

					lstrcpy(_this->data->m_ObjectPath, "");
					lstrcpy(_this->data->m_ObjectFullName, "");
					
					if(_this->data->ENUM_NT.m_pNextProcess->PID == 0)
						lstrcpyn(_this->data->m_ObjectName, "System Idle", cNAME_MAX_LEN);
					else
						wsprintf(_this->data->m_ObjectName, "%S", (const char*) _this->data->ENUM_NT.m_pNextProcess->ProcessName);

					/*PR_TRACE(( g_root, prtIMPORTANT, "memscan\tenum 0x%x (%d)'%s'", 
						_this->data->m_PID,
						_this->data->m_PID,
						_this->data->m_ObjectName));*/
					
					if (_this->data->ENUM_NT.m_pNextProcess->Len == 0)
						_this->data->ENUM_NT.m_pNextProcess->Len = -1;
					else
					{
						_this->data->ENUM_NT.m_pNextProcess = (PROCESS_INFO*)(
						(CHAR*)_this->data->ENUM_NT.m_pNextProcess + _this->data->ENUM_NT.m_pNextProcess->Len
						);
					}

					if (_this->data->m_PID)
					{
						tERROR err = errUNEXPECTED;
						tDWORD XZ = 0;
						tDWORD Mod = 0;

						DWORD dwAccessMask = PROCESS_ALL_ACCESS;
						HANDLE hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);

						if(!hProcess)
						{
							HANDLE hTokenThread = NULL;
							if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &hTokenThread))
							{
								RevertToSelf();
								hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);
								SetThreadToken(NULL, hTokenThread);
							}
						}

						if(hProcess != NULL)
						{
							PROC_INFO ProcInfoBuff;
							if(!pZwQueryInformationProcess(hProcess, 0, (PVOID) &ProcInfoBuff, sizeof(ProcInfoBuff),NULL))
							{
								if(ReadProcessMemory(hProcess, (PVOID) (ProcInfoBuff.UPEB + 0x0c), &XZ, sizeof(XZ), NULL))
								{
									if(ReadProcessMemory(hProcess, (PVOID)(XZ += 0x14), &Mod, sizeof(Mod), NULL))
									{
										if (XZ != Mod)	//???
											err = errOK;
									}
								}
							}
						}

						if (PR_SUCC(err))
						{
							ModuleInfo Mi;
							if(ReadProcessMemory(hProcess, (PVOID)(Mod - 8), &Mi, sizeof(Mi), NULL))
							{
								tWCHAR  *Str;
								SIZE_T  len = Mi.FullFileNameLen;
								if (len)
								{
									Str = _MM__Manage_Alloc(len);
									if (Str != NULL)
									{
										if(ReadProcessMemory(hProcess, Mi.pFullFileName, Str, len, &len))
										{
											WideCharToMultiByte(CP_ACP, 0, Str, -1, _this->data->m_ObjectFullName, (int)len, 0, NULL);
											ReparsePath(_this->data->m_ObjectFullName);
											lstrcpy(_this->data->m_ObjectPath, _this->data->m_ObjectFullName);

											PR_TRACE(( g_root, prtIMPORTANT, "memscan\tnext process is 0x%x (%d) %s",
												_this->data->m_PID,
												_this->data->m_PID,
												_this->data->m_ObjectFullName ));
										}
										
										_MM__Manage_Free(Str);
									}
								}

							}
						}

						if (hProcess)
							CloseHandle(hProcess);

					}

					error = errOK;
				}
			}
		}
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
	{
			if (_this->data->ENUM_9x.m_hProcessSnap != (HANDLE) -1)
			{
				BOOL bResult;
				if(_this->data->ENUM_9x.m_PE32.dwSize == 0)
				{
					_this->data->ENUM_9x.m_PE32.dwSize = sizeof(PROCESSENTRY32);
					bResult = pProcess32First(_this->data->ENUM_9x.m_hProcessSnap, &_this->data->ENUM_9x.m_PE32);
				}
				else
					bResult = pProcess32Next(_this->data->ENUM_9x.m_hProcessSnap, &_this->data->ENUM_9x.m_PE32);
				
				if (bResult == TRUE)
				{
					tDWORD len;
					char *pch;
					_this->data->m_PID = _this->data->ENUM_9x.m_PE32.th32ProcessID;
					_this->data->m_ParentPID = _this->data->ENUM_9x.m_PE32.th32ParentProcessID;
					_this->data->m_ThreadsNum = _this->data->ENUM_9x.m_PE32.cntThreads;
					_this->data->m_Size = _this->data->ENUM_9x.m_PE32.dwSize;
					lstrcpyn(_this->data->m_ObjectName, _this->data->ENUM_9x.m_PE32.szExeFile, cNAME_MAX_LEN);
					
					len = (tDWORD)strlen(_this->data->ENUM_9x.m_PE32.szExeFile) + 1;
					for (pch = _this->data->ENUM_9x.m_PE32.szExeFile + len; len != 0 && *pch != '\\'; len--, pch--);
					pch++;
					len = (tDWORD)strlen(pch) + 1;
					lstrcpyn(_this->data->m_ObjectName, pch, len);
					lstrcpy(_this->data->m_ObjectFullName, _this->data->ENUM_9x.m_PE32.szExeFile);
					
					error = errOK;
				}
			}
		}
		break;
	default:
		error = errNOT_SUPPORTED;
		break;
	}
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::Next\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// Parameters are:
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::StepUp\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"ObjPtr::StepUp\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::StepDown\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"ObjPtr::StepDown\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// Parameters are:
// "name" : The same as "object" parameter of OS::EnumCreate method
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::ChangeTo\" method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave \"ObjPtr::ChangeTo\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


void FS_PROC cbEnumProcessesCallback (
	PVOID dwContext,
	ULONG pid,
	ULONG parent_pid,
	LARGE_INTEGER start_time,
	PWCHAR pwchImagePath,
	PWCHAR pwchCmdLine,
	PWCHAR pwchCurrDir
	)
{
	PENUM_DRV pEnumDrv = (PENUM_DRV) dwContext;
	int len = 0;

	if (!pwchImagePath)
		return;

	len = lstrlenW(pwchImagePath);
	if (len)
	{
		PSimpleProcInfo pItem = _MM__Manage_Alloc(sizeof(SimpleProcInfo));

		if (pItem)
		{
			len = (len + 1) * sizeof(WCHAR);
			pItem->m_pwchImagePath = _MM__Manage_Alloc(len);
			if (pItem->m_pwchImagePath)
			{
				memcpy(pItem->m_pwchImagePath, pwchImagePath, len);
				pItem->m_PID = pid;
				pItem->m_ParentPID = parent_pid;
				pItem->m_StartTime = start_time;

				PR_TRACE((g_root, prtNOT_IMPORTANT,"ObjPtr(IID_OBJPTR) drvenum 0x%x (%d) '%S'", 
					pItem->m_PID,
					pItem->m_PID,
					pItem->m_pwchImagePath) );
				
				pItem->m_pNext = pEnumDrv->m_pProcesses;

				pEnumDrv->m_pProcesses = pItem;
			}
			else
			{
				_MM__Manage_Free(pItem);
			}
		}
	}
}
