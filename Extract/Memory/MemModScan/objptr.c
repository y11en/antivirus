// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  13 March 2003,  16:17 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- objptr.c
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "objptr.h"
// AVP Prague stamp end

#include <stuff/MemManag.h>


#define pmc_MEMORY_PROCESS             ((tDWORD)(0x730f3626))
#define pm_MEMORY_PROCESS_REQUEST_DELETE ((tDWORD)(0x28cedca3))


// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end


#include "../../windows/hook/mklif/fssync/fssync.h"

typedef struct _tSimpleModuleInfo
{
	struct _tSimpleModuleInfo*	m_pNext;
	PWCHAR				m_pwchImagePath;
	LARGE_INTEGER		m_ImageBase;
	ULONG				m_ImageSize;
}SimpleModuleInfo, *PSimpleModuleInfo;

typedef struct _tENUM_DRV
{
	HMODULE					m_hFSSync;
	_tpfFSDrv_EnumModules	m_pfEnum;
	PSimpleModuleInfo		 m_pModules;
	PSimpleModuleInfo		 m_pCurrent;

}ENUM_DRV, *PENUM_DRV;


void FS_PROC cbEnumModulesCallback (
	PVOID dwContext,
	PWCHAR pwchImagePath,
	LARGE_INTEGER ImageBase,
	ULONG ImageSize,
	LARGE_INTEGER EntryPoint
	)
{
	PENUM_DRV pEnumDrv = (PENUM_DRV) dwContext;
	
	int len = 0;

	if (!pwchImagePath)
		return;


	len = lstrlenW(pwchImagePath);
	if (len)
	{
		PSimpleModuleInfo pItem = _MM__Manage_Alloc(sizeof(SimpleModuleInfo));

		if (pItem)
		{
			len = (len + 1) * sizeof(WCHAR);
			pItem->m_pwchImagePath = _MM__Manage_Alloc(len);
			if (!pItem->m_pwchImagePath)
				_MM__Manage_Free(pItem);
			else
			{
				pItem->m_pNext = NULL;
				memcpy(pItem->m_pwchImagePath, pwchImagePath, len);
				pItem->m_ImageBase = ImageBase;
				pItem->m_ImageSize = ImageSize;

				if (!pEnumDrv->m_pModules)
					pEnumDrv->m_pModules = pItem;
				else
				{
					// push back
					PSimpleModuleInfo pModulesLast = pEnumDrv->m_pModules;
					while (pModulesLast->m_pNext)
						pModulesLast = pModulesLast->m_pNext;
					pModulesLast->m_pNext = pItem;
				}
			}
		}
	}
}

char* GetNameOffset(char* pchImagePath)
{
	int len = lstrlen(pchImagePath);

	while (len)
	{
		if (pchImagePath[len] == L'\\')
			return pchImagePath + len + 1;

		len--;
	}

	return pchImagePath;
}


// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_ObjPtrPrivate 
{
	struct tag_ENUM_NT 
	{
		tDWORD m_Mod; // --
		tDWORD m_XZ;  // --
	} ENUM_NT;

	struct tag_ENUM_9x 
	{
		MODULEENTRY32 m_ME; // --
	} ENUM_9x;

	OSVERSIONINFO m_OsVersionInfo;              // данные об операционной системе
	tDWORD        m_PID;                        // идентификатор текщего процесса
	tCHAR         m_ModName[cNAME_MAX_LEN];     // имя текущего модуля
	tCHAR         m_ProcessName[cNAME_MAX_LEN]; // имя текущиего процесса
	tCHAR         m_ObjectPath[cNAME_MAX_LEN];  // имя файла, из которго был загружен модуль
	HANDLE        m_hProcess;                   // хэндл текущего процесса
	HANDLE		  m_hModHandle;                 // хэндл текущего модуля
	tDWORD        m_ModMemSize;                 // размер модуля в памяти

	ENUM_DRV		m_ENUM_DRV;
} ObjPtrPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	ObjPtrPrivate*     data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
tERROR pr_call ObjPtr_MsgReceive( hi_ObjPtr _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       ObjPtr_ObjectInit,
	(tIntFnObjectInitDone)   ObjPtr_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      ObjPtr_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       ObjPtr_MsgReceive,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )



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



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// Interface "ObjPtr". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)2)
// AVP Prague stamp end

//+ internal function
void ObjPtr_IOInit(hi_ObjPtr _this, hIO hIo)
{
	char chtmp[cNAME_MAX_LEN];

	CALL_SYS_PropertySet(hIo, 0, plIO_PID, &_this->data->m_PID, sizeof(tDWORD));

	if (_this->data->m_ENUM_DRV.m_pfEnum)
	{
		LARGE_INTEGER ImageBase = _this->data->m_ENUM_DRV.m_pCurrent->m_ImageBase;
		tDWORD ModBase = 0;

		CALL_SYS_PropertySet(hIo, 0, plIO_HMODULE, &ModBase, sizeof(tDWORD));
		CALL_SYS_PropertySet(hIo, 0, plSize, &_this->data->m_ENUM_DRV.m_pCurrent->m_ImageSize, sizeof(tDWORD));
		CALL_SYS_PropertySet(hIo, 0, plImageBase, &ImageBase, sizeof(_this->data->m_ENUM_DRV.m_pCurrent->m_ImageBase));

		wsprintf(chtmp, "%s", _this->data->m_ObjectPath);
	}
	else
	{
		CALL_SYS_PropertySet(hIo, 0, plIO_HMODULE, &_this->data->m_hModHandle, sizeof(tDWORD));
		CALL_SYS_PropertySet(hIo, 0, plSize, &_this->data->m_ModMemSize, sizeof(tDWORD));

		wsprintf(chtmp, "%s\\%s", _this->data->m_ProcessName, _this->data->m_ModName);
	}
	
	CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_FULL_NAME, chtmp, lstrlen(chtmp) + 1, cCP_ANSI);

	CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_NAME, _this->data->m_ModName, lstrlen(_this->data->m_ModName) + 1, cCP_ANSI);
	CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_PATH, _this->data->m_ObjectPath, lstrlen(_this->data->m_ObjectPath) + 1, cCP_ANSI);
	
}

void ReparseName(char* pchObjectPath, char* pPrefix, char* pexp)
{
	char chtmp[cNAME_MAX_LEN];
	int len = lstrlen(pPrefix) + 1;

	if (lstrlen(pchObjectPath) < len)
		return;

	lstrcpyn(chtmp, pchObjectPath, len);
	CharLower(chtmp);

	if (lstrcmp(chtmp, pPrefix) != 0)
		return;

	ExpandEnvironmentStrings(pexp, chtmp, sizeof(chtmp));
	lstrcat(chtmp, pchObjectPath + len - 1);
	lstrcpy(pchObjectPath, chtmp);
}
//- end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Enum interface for enumerating modules for given process", 57 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_MEMORY_PROCESS_MODULE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, ObjPtrPrivate, m_ModName, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PATH, ObjPtrPrivate, m_ObjectPath, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, ObjPtrPrivate, m_ModName, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY_PTR( pgMASK, "*", 2 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_SIZE, ObjPtrPrivate, m_ModMemSize, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plENUM_PID, ObjPtrPrivate, m_PID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, ObjPtr_PROP_GetObjectAttributes, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_GetObjectSizeQ, NULL )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, ObjPtr_PROP_CommonPropRead, ObjPtr_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_FN( pgOBJECT_HASH, ObjPtr_PROP_CommonPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PTR_STATE, ObjPtr_PROP_GetObjectState, NULL )
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
		PID_MEMMODSCAN,                         // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
		VID_SOBKO,                              // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(ObjPtrPrivate),                  // memory size
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

	//!!
	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_GetObjectState\" for property \"pgOBJECT_PTR_STATE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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

	if (buffer != NULL)
		*(tQWORD*) buffer = (tQWORD) _this->data->m_ModMemSize;

	*out_size = sizeof(tQWORD);

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

	//!!
	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method \"ObjPtr_PROP_GetObjectAttributes\" for property \"pgOBJECT_ATTRIBUTES\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_HASH
//  -- OBJECT_REOPEN_DATA
tERROR pr_call ObjPtr_PROP_CommonPropRead( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method ObjPtr_PROP_CommonPropRead" );

	switch ( prop ) {

		case pgOBJECT_HASH:
			*out_size = 0;
			error = errNOT_SUPPORTED;
			break;

		case pgOBJECT_REOPEN_DATA:
			error = CALL_SYS_PropertyGetStr(_this, out_size, pgOBJECT_PATH, buffer, size, cCP_UNICODE);
			break;

		default:
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method ObjPtr_PROP_CommonPropRead, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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

	if (out_size )
	{
		*out_size = 0;
		PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	}
	else
	{
		error = errUNEXPECTED;
	}
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
	error = errUNEXPECTED;
	
	_this->data->ENUM_NT.m_XZ = 0;
	_this->data->m_hProcess = NULL;
	_this->data->m_hModHandle = NULL;
	_this->data->m_ModMemSize = 0;
	
	_this->data->ENUM_9x.m_ME.dwSize = 0;
	
	ZeroMemory(&_this->data->m_OsVersionInfo, sizeof(OSVERSIONINFO));
	_this->data->m_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if (GetVersionEx(&_this->data->m_OsVersionInfo) == FALSE)
		error = errUNEXPECTED;

	CALL_SYS_RegisterMsgHandler(_this, pmc_MEMORY_PROCESS, rmhLISTENER, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	{
		hOBJECT hParent = CALL_SYS_ParentGet(_this, IID_OS);
		if (hParent != 0)
		{
			if (PR_SUCC(CALL_SYS_ObjectCheck(_this, hParent, IID_OS, PID_MEMMODSCAN, SUBTYPE_ANY, cFALSE)))
				CALL_SYS_PropertySetDWord(_this, plENUM_PID, CALL_SYS_PropertyGetDWord(hParent, plPID));
		}
	}

	if (VER_PLATFORM_WIN32_NT == _this->data->m_OsVersionInfo.dwPlatformId)
	{
		_this->data->m_ENUM_DRV.m_hFSSync = LoadLibrary("FSSync.dll");

		if (_this->data->m_ENUM_DRV.m_hFSSync)
		{
			_this->data->m_ENUM_DRV.m_pfEnum = (_tpfFSDrv_EnumModules) GetProcAddress(
				_this->data->m_ENUM_DRV.m_hFSSync,
				"FSSync_PEnM"
				);

			if (_this->data->m_ENUM_DRV.m_pfEnum)
			{
				if (_this->data->m_ENUM_DRV.m_pfEnum(&_this->data->m_ENUM_DRV, _this->data->m_PID, cbEnumModulesCallback))
				{
					// test enum success
					ObjPtr_Reset(_this, TRUE);
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
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::ObjectInitDone\" method" );

	// Place your code here
	error = ObjPtr_Reset(_this, cTRUE);	
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
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
	error = errOK;
	
	if (_this->data->m_hProcess != NULL)
	{
		CloseHandle(_this->data->m_hProcess);
		_this->data->m_hProcess = NULL;
	}

	if (_this->data->m_ENUM_DRV.m_hFSSync)
	{
		FreeLibrary(_this->data->m_ENUM_DRV.m_hFSSync);
		_this->data->m_ENUM_DRV.m_hFSSync = NULL;
	}


	PR_TRACE_A1( _this, "Leave \"ObjPtr::ObjectClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//  Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
// "msg_cls_id"    : Message class identifier
// "msg_id"        : Message identifier
// "obj"           : Object as a reason of message
// "ctx"           : Context of the object processing
// "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
// "par_buf"       : Buffer of the parameters
// "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call ObjPtr_MsgReceive( hi_ObjPtr _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::MsgReceive\" method" );

	// Place your code here
	error = errOK;
	if (msg_id == pm_MEMORY_PROCESS_REQUEST_DELETE)
	{
		if ((par_buf != NULL) && (par_buf_len != NULL) && (*par_buf_len == sizeof(DWORD)))
		{
			if ((_this->data->m_PID == *(DWORD*) par_buf) && (_this->data->m_hProcess != NULL))
				ObjPtr_ObjectClose(_this);
		}
	}
	

	PR_TRACE_A1( _this, "Leave \"ObjPtr::MsgReceive\" method, ret tERROR = 0x%08x", error );
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

	error = errUNEXPECTED;

	if (_this->data->m_hProcess != NULL || _this->data->m_ENUM_DRV.m_pCurrent)
	{
		error = CALL_SYS_ObjectCreate(_this, (hOBJECT*) &ret_val, IID_IO, PID_MEMMODSCAN, 0);
		if (ret_val != NULL)
		{
			ObjPtr_IOInit(_this, ret_val);
			
			error = CALL_SYS_ObjectCreateDone(ret_val);
			if (PR_SUCC(error))
			{
				PR_TRACE(( g_root, prtNOTIFY, "mmsc\t create io: pid %d path '%s'", _this->data->m_PID, _this->data->m_ObjectPath ));
			}
			else
			{
				PR_TRACE(((hOBJECT) _this, prtIMPORTANT, "Create IO failed!"));
				CALL_SYS_ObjectClose(ret_val);
				ret_val = NULL;
			}
		}
	}
	
	if ( result )
		*result = ret_val;

	PR_TRACE_A2( _this, "Leave \"ObjPtr::IOCreate\" method, ret hIO = %S, error = 0x%08x", ret_val, error );
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
	error = errOK;
	if (_this->data->m_hProcess != NULL)
	{
		tDWORD dwSize = sizeof(_this->data->m_PID);
		CALL_SYS_SendMsg(_this, pmc_MEMORY_PROCESS, pm_MEMORY_PROCESS_REQUEST_DELETE, _this, &_this->data->m_PID, &dwSize);
		TerminateProcess(_this->data->m_hProcess, 0);
	}
	else
		error = errHANDLE_WRONG_STATUS;

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
	error = errUNEXPECTED;

	if (_this->data->m_ENUM_DRV.m_pfEnum)
	{
		PSimpleModuleInfo pTmp = NULL;

		while (_this->data->m_ENUM_DRV.m_pModules)
		{
			pTmp = _this->data->m_ENUM_DRV.m_pModules;

			_this->data->m_ENUM_DRV.m_pModules = _this->data->m_ENUM_DRV.m_pModules->m_pNext;

			if (pTmp->m_pwchImagePath)
				_MM__Manage_Free(pTmp->m_pwchImagePath);

			_MM__Manage_Free(pTmp);
		}

		_this->data->m_ENUM_DRV.m_pCurrent = NULL;

		return errOK;
	}

	ObjPtr_ObjectClose(_this);

	if (_this->data->m_PID != -1)
	{
		CHAR *pBuffer;
		
		switch (_this->data->m_OsVersionInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
		{
				if (GetNTProcessInfo(&pBuffer) == FALSE)
					error = errOBJECT_CANNOT_BE_INITIALIZED;
				else
				{
					BOOL bFound = FALSE;
					PROCESS_INFO* pNextProcess;

					pNextProcess = (PROCESS_INFO*) pBuffer;

					do
					{
						if (_this->data->m_PID == pNextProcess->PID)
							bFound = TRUE;
					}	while(bFound == FALSE && pNextProcess->Len && (pNextProcess = (PROCESS_INFO*)((CHAR*)pNextProcess + pNextProcess->Len)));

					if (bFound == TRUE)
					{
						//pNextProcess is nash
						PROC_INFO ProcInfoBuff;
						DWORD dwAccessMask = PROCESS_ALL_ACCESS;
						_this->data->m_hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);

						if(_this->data->m_hProcess == NULL)
						{
							HANDLE hTokenThread = NULL;
							if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &hTokenThread))
							{
								RevertToSelf();
								_this->data->m_hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);
								SetThreadToken(NULL, hTokenThread);
							}
						}

						if(_this->data->m_hProcess != NULL)
						{
							if(!pZwQueryInformationProcess(_this->data->m_hProcess, 0, (PVOID) &ProcInfoBuff, sizeof(ProcInfoBuff),NULL))
							{
								if(ReadProcessMemory(_this->data->m_hProcess, (PVOID) (ProcInfoBuff.UPEB + 0x0c), &_this->data->ENUM_NT.m_XZ, sizeof(_this->data->ENUM_NT.m_XZ), NULL))
								{
									if(ReadProcessMemory(_this->data->m_hProcess, (PVOID)(_this->data->ENUM_NT.m_XZ += 0x14), &_this->data->ENUM_NT.m_Mod, sizeof(_this->data->ENUM_NT.m_Mod), NULL))
									{
										wsprintf(_this->data->m_ProcessName, "%S", (const char*) pNextProcess->ProcessName);
										error = errOK;
									}
								}
							}
						}
						if (error != errOK)
						{
							if (_this->data->m_hProcess)
							{
								CloseHandle(_this->data->m_hProcess);
								_this->data->m_hProcess = NULL;
							}
							error = errOBJECT_CANNOT_BE_INITIALIZED;
						}
					}
					_MM__Manage_Free(pBuffer);
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
		{
				_this->data->m_hProcess = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if (_this->data->m_hProcess == (HANDLE) -1)
				{
					PR_TRACE(((hOBJECT) _this, prtIMPORTANT, "CreateToolhelp32Snapshot failed!"));
					_this->data->m_hProcess = NULL;
					error = errOBJECT_CANNOT_BE_INITIALIZED;
				}
				else
				{
					BOOL bFound = FALSE;
					BOOL bResult = TRUE;
					PROCESSENTRY32 PE32;

					PE32.dwSize = sizeof(PROCESSENTRY32);
					bResult = pProcess32First(_this->data->m_hProcess, &PE32);
					do
					{
						if (bResult == TRUE)
						{
							if (PE32.th32ProcessID != _this->data->m_PID)
								bResult = pProcess32Next(_this->data->m_hProcess, &PE32);
							else
							{
								tDWORD len;
								char *pch;
								
								bFound = TRUE;
								lstrcpyn(_this->data->m_ProcessName, PE32.szExeFile, cNAME_MAX_LEN);
								
								len = lstrlen(PE32.szExeFile) + 1;
								for (pch = PE32.szExeFile + len; len != 0 && *pch != '\\'; len--, pch--);
								pch++;
								len = lstrlen(pch) + 1;
								lstrcpyn(_this->data->m_ProcessName, pch, len);
								
								CloseHandle(_this->data->m_hProcess);

								_this->data->m_hProcess = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _this->data->m_PID);
								if (_this->data->m_hProcess == (HANDLE) -1)
								{
									PR_TRACE(((hOBJECT) _this, prtIMPORTANT, "CreateToolhelp32Snapshot failed!"));
									_this->data->m_hProcess = NULL;
									error = errOBJECT_CANNOT_BE_INITIALIZED;
								}
								else
									error = errOK;
							}
						}
					}	while((bFound == FALSE) && (bResult == TRUE));
					if (bFound == FALSE)
						CloseHandle(_this->data->m_hProcess);
				}	
			}
			break;
		default:
			error = errNOT_SUPPORTED;
			break;
		}
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
	PR_TRACE_A2( _this, "Leave \"ObjPtr::Clone\" method, ret hObjPtr = %u, error = 0x%08x", ret_val, error );
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
		if (!_this->data->m_ENUM_DRV.m_pModules)
		{
			_this->data->m_ENUM_DRV.m_pfEnum( &_this->data->m_ENUM_DRV, _this->data->m_PID, cbEnumModulesCallback);
			_this->data->m_ENUM_DRV.m_pCurrent = NULL;

			if (!_this->data->m_ENUM_DRV.m_pModules)
			{
				PR_TRACE(( g_root, prtIMPORTANT, "memmods\tno modules found. empty list" ));
			}

		}

		if (_this->data->m_ENUM_DRV.m_pModules)
		{
			if (_this->data->m_ENUM_DRV.m_pCurrent)
				_this->data->m_ENUM_DRV.m_pCurrent = _this->data->m_ENUM_DRV.m_pCurrent->m_pNext;
			else
				_this->data->m_ENUM_DRV.m_pCurrent = _this->data->m_ENUM_DRV.m_pModules;

			if (_this->data->m_ENUM_DRV.m_pCurrent)
			{
				_this->data->m_ModName[0] = 0;

				WideCharToMultiByte (
					CP_ACP,
					0,
					_this->data->m_ENUM_DRV.m_pCurrent->m_pwchImagePath,
					-1,
					_this->data->m_ObjectPath,
					sizeof(_this->data->m_ObjectPath),
					0,
					NULL
					);

					ReparseName(_this->data->m_ObjectPath, "\\systemroot\\", "%WinDir%\\");
					ReparseName(_this->data->m_ObjectPath, "\\??\\", "");

					lstrcpy(_this->data->m_ModName, GetNameOffset( _this->data->m_ObjectPath ));

					PR_TRACE(( g_root, prtNOT_IMPORTANT, "memmods\tnext module is %s", _this->data->m_ObjectPath ));
				
				error = errOK;
			}
		}

		return error;
	}
	
	// Place your code here
	if (_this->data->m_hProcess != NULL)
	{
		BOOL bError;
		switch (_this->data->m_OsVersionInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
		{
				ModuleInfo Mi;
				if (_this->data->ENUM_NT.m_Mod != _this->data->ENUM_NT.m_XZ)
				{
					bError = TRUE;
					if(ReadProcessMemory(_this->data->m_hProcess, (PVOID)(_this->data->ENUM_NT.m_Mod - 8), &Mi, sizeof(Mi), NULL))
					{
						// getting name
						tWCHAR  *Str;
						SIZE_T  len = Mi.ModNameLen;
						if (len)
						{
							_this->data->m_hModHandle = Mi.hModule;
							_this->data->m_ModMemSize = Mi.VirtualImageSize;
							Str = _MM__Manage_Alloc(len);
							if (Str != NULL)
							{
								if(ReadProcessMemory(_this->data->m_hProcess, Mi.pModName, Str, len, &len))
								{
									bError = FALSE;
									__try
									{
										WideCharToMultiByte(CP_ACP, 0, Str, -1, _this->data->m_ModName, (int)len, 0, NULL);							
									}
									__except(EXCEPTION_EXECUTE_HANDLER)
									{
										bError = TRUE;
									}
								}

								_MM__Manage_Free(Str);
							}
						}
						else
						{
							// unrecognized module, no name but have to check
							wsprintf(_this->data->m_ModName, "<unknown module> %x", Mi.hModule);
							bError = FALSE;
						}

						if (!lstrlen(_this->data->m_ModName	))
						{
							wsprintf (
								_this->data->m_ModName, "noname_module_offset_%x",
								_this->data->m_PID,
								_this->data->m_hModHandle
								);
						}

						
						if (bError == FALSE)
						{
							bError = TRUE;
							len = Mi.FullFileNameLen;
							if (len)
								Str = _MM__Manage_Alloc(len);
							else
								Str = NULL;

							_this->data->m_ObjectPath[0] = 0;

							if(Str != NULL)
							{
								if(ReadProcessMemory(_this->data->m_hProcess, Mi.pFullFileName, Str, len, &len))
								{
									bError = FALSE;
									__try
									{
										WideCharToMultiByte(CP_ACP, 0, Str, -1, _this->data->m_ObjectPath, (int)len, 0, NULL);
										
										ReparseName(_this->data->m_ObjectPath, "\\systemroot\\", "%WinDir%\\");
										ReparseName(_this->data->m_ObjectPath, "\\??\\", "");
									}
									__except(EXCEPTION_EXECUTE_HANDLER)
									{
										bError = TRUE;
									}
								}
								_MM__Manage_Free(Str);
							}

							if (!lstrlen(_this->data->m_ObjectPath))
							{
								wsprintf (
									_this->data->m_ObjectPath, "pid_0x%x\\noname_module_offset_%x",
									_this->data->m_PID,
									_this->data->m_hModHandle
									);
							}
						}
						
						_this->data->ENUM_NT.m_Mod = Mi.NextMod;
					}
					if (bError == FALSE)
						error = errOK;
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
		{
				if(_this->data->ENUM_9x.m_ME.dwSize == 0)
				{
					_this->data->ENUM_9x.m_ME.dwSize = sizeof(MODULEENTRY32);
					bError = !pModule32First(_this->data->m_hProcess, &_this->data->ENUM_9x.m_ME);
				}
				else
				{
					bError = !pModule32Next(_this->data->m_hProcess, &_this->data->ENUM_9x.m_ME);
				}
				if (bError == TRUE)
					error = errUNEXPECTED;
				else
				{
					_this->data->m_hModHandle = _this->data->ENUM_9x.m_ME.hModule;
					_this->data->m_ModMemSize = _this->data->ENUM_9x.m_ME.modBaseSize;
					lstrcpyn(_this->data->m_ModName, _this->data->ENUM_9x.m_ME.szModule, cNAME_MAX_LEN);
					lstrcpyn(_this->data->m_ObjectPath, _this->data->ENUM_9x.m_ME.szExePath, cNAME_MAX_LEN);
					error = errOK;
				}
			}
			break;
		default:
			error = errNOT_SUPPORTED;
			break;
		}
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
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"ObjPtr::ChangeTo\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



