// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  08 April 2003,  20:42 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// Additional info
//  Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// File Name   -- os.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define OS_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "io.h"

#include <Prague/iface/i_buffer.h>

#include <windows.h>
#include <stuff/memmanag.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
// Short comments -- object system interface
// Extended comment -
//  Interface defines behavior of an object system. It responds for
//    - enumerate IO and Folder objects by creating Folder objects.
//    - create and delete IO and Folder objects
//  There are two possible ways to create or open IO object on OS. First is three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface OS. Inner data structure

typedef struct tag_PM_OsPrivate 
{
	tCHAR         m_ObjectName[cNAME_MAX_LEN]; // имя процесса
	OSVERSIONINFO m_OsVersionInfo;             // данные о операционной системе
	tDWORD        m_Reopen;                    // --
	tQWORD        m_Hash;                      // --
} PM_OsPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

	typedef struct tag_hi_OS 
	{
		const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
		const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
		PM_OsPrivate*      data; // pointer to the "OS" data structure
	} *hi_OS;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
  // OBJECT DESCRIPTION SECTION TRANSFERRED TO CORRESPONDING HEADER FILE
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call OS_ObjectInit( hi_OS _this );
tERROR pr_call OS_ObjectInitDone( hi_OS _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_OS_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       OS_ObjectInit,
	(tIntFnObjectInitDone)   OS_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// 	(tIntFnObjectClose)      OS_ObjectClose,
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpOS_PtrCreate) ( hi_OS _this, hObjPtr* result, hOBJECT name );              // -- opens ObjPtr object;
typedef   tERROR (pr_call *fnpOS_IOCreate)  ( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ); // -- opens IO object;
typedef   tERROR (pr_call *fnpOS_Copy)      ( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ); // -- copies object inside OS;
typedef   tERROR (pr_call *fnpOS_Rename)    ( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ); // -- rename/move object by name inside OS;
typedef   tERROR (pr_call *fnpOS_Delete)    ( hi_OS _this, hOBJECT name );              // -- phisically deletes an entity on OS;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iOSVtbl 
{
	fnpOS_PtrCreate  PtrCreate;
	fnpOS_IOCreate   IOCreate;
	fnpOS_Copy       Copy;
	fnpOS_Rename     Rename;
	fnpOS_Delete     Delete;
}; // "OS" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name );
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iOSVtbl e_OS_vtbl = 
{
	OS_PtrCreate,
	OS_IOCreate,
	OS_Copy,
	OS_Rename,
	OS_Delete
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// Interface "OS". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// Interface "OS". Property table
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(cNAME_MAX_LEN)) )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)('\0')) )
	mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "OS interface for memory scan", 29 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_REOPEN_DATA, PM_OsPrivate, m_Reopen, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOBJECT_HASH, PM_OsPrivate, m_Hash, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, PM_OsPrivate, m_ObjectName, cPROP_BUFFER_READ )
mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter OS::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_MEMSCAN,                             // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(PM_OsPrivate),                   // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"OS(IID_OS) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave OS::Register method, ret tERROR = 0x%08x", error );
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
tERROR pr_call OS_ObjectInit( hi_OS _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"OS::ObjectInit\" method" );

	// Place your code here
	error = errOK;
	
	lstrcpyn(_this->data->m_ObjectName, "Windows memory", cNAME_MAX_LEN);
	ZeroMemory(&_this->data->m_OsVersionInfo, sizeof(OSVERSIONINFO));
	_this->data->m_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if (GetVersionEx(&_this->data->m_OsVersionInfo) == FALSE)
		error = errUNEXPECTED;
	else
	{
		if (InitWinMemManagement(_this->data->m_OsVersionInfo.dwPlatformId, _MM__Manage_Alloc, _MM__Manage_Free) == FALSE)
			error = errOBJECT_CANNOT_BE_INITIALIZED;
	}

	PR_TRACE_A1( _this, "Leave \"OS::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call OS_ObjectInitDone( hi_OS _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"OS::ObjectInitDone\" method" );

	// Place your code here
	error = errOK;

	PR_TRACE_A1( _this, "Leave \"OS::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call OS_ObjectClose( hi_OS _this )
{

	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"OS::ObjectClose\" method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave \"OS::ObjectClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, PtrCreate )
// Extended method comment
//  It responds for opening ObjPtr object by:
//    -- hString object holded folder name
//    -- hIO object's folder
//    -- another hObjPtr object (cloning)
//
// Behaviour comment
//  Must realise the same result as if client uses following three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//  If parameter name is NULL must open *ROOT* folder
//
// Parameters are:
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name )
{
	tERROR  error = errNOT_IMPLEMENTED;
	hObjPtr ret_val = NULL;
	PR_TRACE_A0( _this, "Enter \"OS::PtrCreate\" method" );

	{
	// Place your code here
		tDWORD ObjType;
		if (name == NULL)
			ObjType = 0;
		else
		{
			ObjType = (tDWORD) -1;
			error = CALL_SYS_ObjectValid(_this, name);
			if (PR_SUCC(error))
			{
				if (CALL_SYS_PropertyGetDWord(name, pgPLUGIN_ID) == PID_MEMSCAN)
					ObjType = CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID);
			}
		}
		error = errNOT_SUPPORTED;
		switch (ObjType)
		{
		case 0:			// create default object
		{
				error = CALL_SYS_ObjectCreate(_this, (hOBJECT*) &ret_val, IID_OBJPTR, PID_MEMSCAN, 0);
				if (PR_SUCC(error) && (ret_val != NULL))
				{
					error = CALL_SYS_ObjectCreateDone(ret_val);
					if (PR_FAIL(error))
					{
						CALL_SYS_ObjectClose(ret_val);
						ret_val = NULL;
					}
				}
			}
			break;
		case IID_IO:		// create from IO
			break;
		case IID_OBJPTR:	// clone Enum
			break;
		default:			// error
			break;
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"OS::PtrCreate\" method, ret hObjPtr = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  It responds for opening IO object by:
//    -- hString held objects name
//    -- hIO object's name (cloning)
//
// Behaviour comment
//  Must realise the same result as if client uses following three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//  If parameter name is NULL must open *ROOT* folder
//
// Parameters are:
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode )
{
	tERROR error = errNOT_IMPLEMENTED;
	hIO    ret_val = NULL;
	PR_TRACE_A0( _this, "Enter \"OS::IOCreate\" method" );

	// Place your code here
	error = errPARAMETER_INVALID;
	if (PR_SUCC(CALL_SYS_ObjectCheck(_this, name, IID_BUFFER, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		hObjPtr hPtr = 0;
		tPTR ptrReopenData;
		hBUFFER hBuffer = (hBUFFER) name;
		tQWORD nReopenDataLen;
		error = errNOT_FOUND;

		if (PR_SUCC(CALL_Buffer_GetSize(hBuffer, &nReopenDataLen, 0)) &&
			PR_SUCC(CALL_Buffer_Lock(hBuffer, &ptrReopenData)))
		{
			if (PR_SUCC(OS_PtrCreate(_this, &hPtr, NULL)))
			{
				tERROR enumerr;
				hIO hIo;

				tPTR ptrReopen;
				tDWORD nReopenLen;

				while (PR_SUCC(enumerr = CALL_ObjPtr_Next(hPtr)))
				{
					ptrReopen = 0;
					nReopenLen = 0;

					if (PR_FAIL(CALL_SYS_PropertyGet(hPtr, &nReopenLen, pgOBJECT_REOPEN_DATA, NULL, 0)) && nReopenLen != 0)
						continue;
					
					if (PR_SUCC(CALL_SYS_ObjHeapAlloc(_this, &ptrReopen, nReopenLen)))
					{
						if (ptrReopen != NULL)
						{
							if (PR_FAIL(CALL_SYS_PropertyGet(hPtr, &nReopenLen, pgOBJECT_REOPEN_DATA, ptrReopen, nReopenLen)))
								CALL_SYS_ObjHeapFree(_this, ptrReopen);
							else
							{
								if ((nReopenDataLen == nReopenLen) && (memcmp(ptrReopen, ptrReopenData, nReopenLen) ==0))
								{
									hIo = 0;
									if (PR_SUCC(CALL_ObjPtr_IOCreate(hPtr, &hIo, NULL, fACCESS_RW, fOMODE_OPEN_IF_EXIST)))
									{
										ret_val = hIo;
										error = CALL_SYS_ParentSet((hOBJECT) hIo, NULL, (hOBJECT) _this);
									}
									break;
								}
							}
						}
					}
				}

				CALL_SYS_ObjectClose(hPtr);
			}

			CALL_Buffer_Unlock(hBuffer);
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"OS::IOCreate\" method, ret hIO = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"OS::Copy\" method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave \"OS::Copy\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Extended method comment
//  Renames/moves object by name inside single OS
//  Must move object through folders if nessesary
// Parameters are:
// "new_name"  : Target name
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"OS::Rename\" method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave \"OS::Rename\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//  phisically deletes an entity on OS
// Behaviour comment
//  if parameter "object" is really hIO or hObjPtr must close it and delete after close.
// Parameters are:
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"OS::Delete\" method" );
	// Place your code here
	error = errNOT_SUPPORTED;
	
	PR_TRACE_A1( _this, "Leave \"OS::Delete\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



