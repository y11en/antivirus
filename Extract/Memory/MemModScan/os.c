// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  09 April 2003,  12:03 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- os.c
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define OS_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "os.h"
// AVP Prague stamp end

#include <stuff/MemManag.h>


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



#define pmc_MEMORY_PROCESS             ((tDWORD)(0x730f3626))
#define pm_MEMORY_PROCESS_REQUEST_DELETE ((tDWORD)(0x28cedca3))


// AVP Prague stamp begin( Data structure,  )
// Interface OS. Inner data structure

typedef struct tag_OsPrivate 
{
	OSVERSIONINFO m_OsVersionInfo;              // структура с описанием операционной системы
	tDWORD        m_PID;                        // идентификатор процесса
	tDWORD        m_ThreadsNum;                 // число потоков текущего процесса
	tCHAR         m_ProcessName[cNAME_MAX_LEN]; // имя процесса
} OsPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_OS 
{
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OsPrivate*         data; // pointer to the "OS" data structure
} *hi_OS;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD p_type );
tERROR pr_call OS_ObjectInit( hi_OS _this );
tERROR pr_call OS_ObjectInitDone( hi_OS _this );
tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_OS_vtbl = 
{
	(tIntFnRecognize)        OS_Recognize,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       OS_ObjectInit,
	(tIntFnObjectInitDone)   OS_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       OS_MsgReceive,
	(tIntFnIFaceTransfer)    NULL
};
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
tERROR pr_call OS_PROP_CommonPropRead( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_CommonPropWrite( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// Interface "OS". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "OS interface for module memory access", 38 )
	mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, OsPrivate, m_ProcessName, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(cNAME_MAX_LEN)) )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)('|')) )
	mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, OS_PROP_CommonPropRead, OS_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_BUF( plPID, OsPrivate, m_PID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"OS::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_MEMMODSCAN,                         // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(OsPrivate),                      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"OS(IID_OS) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"OS::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropRead )
// Interface "OS". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call OS_PROP_CommonPropRead( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method OS_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA" );

	{
		tDWORD dwSize = sizeof(DWORD) + lstrlen(_this->data->m_ProcessName) + 1;
		if (buffer != NULL)
		{
			if (dwSize < size)
				error = errBUFFER_TOO_SMALL;
			else
			{
				*(DWORD*)buffer = _this->data->m_PID;
				lstrcpy((char*)buffer + sizeof(DWORD), _this->data->m_ProcessName);

				PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tOSRequest reopen data: pid %d, len %d", _this->data->m_PID, dwSize));
			}
		}
		*out_size = dwSize;
	}

	PR_TRACE_A2( _this, "Leave *GET* method OS_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropWrite )
// Interface "OS". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call OS_PROP_CommonPropWrite( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method OS_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA" );

	*out_size = size;
	if (size < sizeof(DWORD) + sizeof(tCHAR))
		error = errPARAMETER_INVALID;
	else
	{
		hObjPtr hPtr = 0;
		tCHAR ObjectName[cNAME_MAX_LEN];
		DWORD Pid = *(DWORD*) buffer;

		tCHAR ObjectNameTmp[cNAME_MAX_LEN];
		lstrcpyn(ObjectName, buffer + sizeof(DWORD), size > cNAME_MAX_LEN ? cNAME_MAX_LEN : size);

		PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tOSIncome reopen data: pid %d, len %d", Pid, size));

		if (lstrlen(ObjectName) == 0)
			error = errUNEXPECTED;
		else
		{
			error = errNOT_FOUND;

			if (PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &hPtr, IID_OBJPTR, PID_MEMSCAN, 0)))
			{
				while (PR_SUCC(CALL_ObjPtr_Next(hPtr)))
				{
					tDWORD pidtmp = CALL_SYS_PropertyGetDWord(hPtr, plPID);
					PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tenum pid %d", pidtmp));
					if (Pid == pidtmp)
					{
						if (PR_FAIL(CALL_SYS_PropertyGetStr(hPtr, 0, pgOBJECT_NAME, ObjectNameTmp, sizeof(ObjectNameTmp), cCP_ANSI)))
						{
							PR_TRACE(( g_root, prtERROR, "MemModsc\tenum get name failed"));
							continue;
						}

						PR_TRACE(( g_root, prtERROR, "MemModsc\tenum name %s", ObjectName));
						if (lstrcmp(ObjectName, ObjectNameTmp) != 0)
							continue;

						CALL_SYS_PropertySet(_this, 0, plPID, &Pid, sizeof(tDWORD));
						lstrcpy(_this->data->m_ProcessName, ObjectName);
						error = errOK;

						PR_TRACE(( g_root, prtERROR, "MemModsc\tobject found"));
						break;
					}
				}
				CALL_SYS_ObjectClose(hPtr);
			}
		}
	}


	PR_TRACE_A2( _this, "Leave *SET* method OS_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, Recognize )
// Extended method comment
//  Static method. Kernel calls this method wihtout any created objects of this interface.
//  Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//  Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//  It must be quick (preliminary) analysis
// Parameters are:
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD p_type )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A2( 0, "Enter \"OS::Recognize\" method for object \"%p (iid=%u)\" ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

	// Place your code here
	error = errNOT_SUPPORTED;
	{
		if (CALL_SYS_PropertyGetDWord(_that, pgOBJECT_ORIGIN) == OID_MEMORY_PROCESS)
		{
			if ((CALL_SYS_PropertyGetDWord(_that, pgINTERFACE_ID) == IID_IO) || (CALL_SYS_PropertyGetDWord(_that, pgINTERFACE_ID) == IID_OBJPTR))
				error = errOK;
		}
	}


	PR_TRACE_A2( 0, "Leave \"OS::Recognize\" method for object \"%p (iid=%u)\" ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
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
	_this->data->m_PID = (tDWORD) -1;
	
	ZeroMemory(&_this->data->m_OsVersionInfo, sizeof(OSVERSIONINFO));
	_this->data->m_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	CALL_SYS_RegisterMsgHandler(_this, pmc_MEMORY_PROCESS, rmhLISTENER, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	
	if (GetVersionEx(&_this->data->m_OsVersionInfo) == FALSE)
		error = errUNEXPECTED;

	if (PR_SUCC(error))
	{
		hOBJECT hParent = CALL_SYS_ParentGet(_this, IID_ANY);
		if (hParent != NULL)
		{
			if (PR_SUCC(OS_Recognize(hParent, 0)))
			{
				tDWORD dw;
				tDWORD ProcessID;
				
				if (CALL_SYS_PropertyGetDWord(hParent, pgINTERFACE_ID) == IID_IO)
				{
					#define plProcessID__FromIO        mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x000000c0 )
					error = CALL_SYS_PropertyGet(hParent, &dw, plProcessID__FromIO, &ProcessID, sizeof(ProcessID));
				}
				else
				{
					if (CALL_SYS_PropertyGetDWord(hParent, pgINTERFACE_ID) == IID_OBJPTR)
					{
						#define plProcessID__FromPtr        mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
						error = CALL_SYS_PropertyGet(hParent, &dw, plProcessID__FromPtr, &ProcessID, sizeof(ProcessID));
					}
				}
				if (PR_SUCC(error))
					CALL_SYS_PropertySetDWord(_this, plPID, ProcessID);
				error = errOK;
			}
		}
	}

	if (PR_SUCC(errOK))
	{
		hObjPtr hPtr = 0;
		if (PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, (hOBJECT*) &hPtr, IID_OBJPTR, PID_MEMSCAN, 0)))
		{
			tCHAR ObjectName[cNAME_MAX_LEN];
			while (PR_SUCC(CALL_ObjPtr_Next(hPtr)))
			{
				if (_this->data->m_PID == CALL_SYS_PropertyGetDWord(hPtr, plPID))
				{
					if (PR_FAIL(CALL_SYS_PropertyGetStr(hPtr, 0, pgOBJECT_NAME, ObjectName, sizeof(ObjectName), cCP_ANSI)))
						continue;

					lstrcpy(_this->data->m_ProcessName, ObjectName);
					break;
				}
			}
			CALL_SYS_ObjectClose(hPtr);
		}
	}

	PR_TRACE(( g_root, prtIMPORTANT, "MemModsc\tIO init result %terr", error));
	
	
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
	
	if (_this->data->m_PID == (tDWORD) -1)
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	
	PR_TRACE_A1( _this, "Leave \"OS::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
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
tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"OS::MsgReceive\" method" );

	// Place your code here
	if (msg_id == pm_MEMORY_PROCESS_REQUEST_DELETE)
	{
		if ((par_buf != NULL) && (par_buf_len != NULL) && (*par_buf_len == sizeof(DWORD)))
		{
			if (_this->data->m_PID == *(DWORD*) par_buf)
				OS_Delete(_this, NULL);
		}
	}

	PR_TRACE_A1( _this, "Leave \"OS::MsgReceive\" method, ret tERROR = 0x%08x", error );
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

	// Place your code here
	error = errOK;
	{
		tDWORD ObjType;	
		if (name == NULL)
			ObjType = 0;
		else
		{
			ObjType = (tDWORD) -1;
			error = CALL_SYS_ObjectValid(_this, name);
			if (PR_SUCC(error))
			{
				if (CALL_SYS_PropertyGetDWord(name, pgPLUGIN_ID) == PID_MEMMODSCAN)
					ObjType = CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID);
			}
		}
		
		error = errNOT_SUPPORTED;

		switch (ObjType)
		{
		case 0:			// create default object
		{
				error = CALL_SYS_ObjectCreate(_this, (hOBJECT*) &ret_val, IID_OBJPTR, PID_MEMMODSCAN, 0);
				if (PR_SUCC(error) && (ret_val != NULL))
				{
					CALL_SYS_PropertySet(ret_val, 0, plENUM_PID, &_this->data->m_PID, sizeof(tDWORD));
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
	PR_TRACE_A2( _this, "Leave \"OS::PtrCreate\" method, ret hObjPtr = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  It responds for opening IO object by:
//    -- hString: open object by name
//    -- hIO: object's name (cloning)
//    -- hObjPtr: open object by ptr
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

	PR_TRACE(( g_root, prtERROR, "MemModsc\tIO create request"));

	if (!PR_SUCC(CALL_SYS_ObjectCheck(_this, name, IID_BUFFER, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		PR_TRACE(( g_root, prtERROR, "MemModsc\tIO create request failed - buffer invalid"));
	}
	else
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
					hIo = 0;
					if (PR_SUCC(CALL_ObjPtr_IOCreate(hPtr, &hIo, NULL, fACCESS_RW, fOMODE_OPEN_IF_EXIST)))
					{
						ptrReopen = 0;
						nReopenLen = 0;

						if (PR_FAIL(CALL_SYS_PropertyGet(hIo, &nReopenLen, pgOBJECT_REOPEN_DATA, NULL, 0)) && nReopenLen != 0)
							continue;
						
						if (PR_SUCC(CALL_SYS_ObjHeapAlloc(_this, &ptrReopen, nReopenLen)))
						{
							if (ptrReopen != NULL)
							{
								if (PR_FAIL(CALL_SYS_PropertyGet(hIo, &nReopenLen, pgOBJECT_REOPEN_DATA, ptrReopen, nReopenLen)))
									CALL_SYS_ObjHeapFree(_this, ptrReopen);
								else
								{
									if ((nReopenDataLen == nReopenLen) && (memcmp(ptrReopen, ptrReopenData, nReopenLen) ==0))
									{
										ret_val = hIo;
										error = CALL_SYS_ParentSet((hOBJECT) hIo, NULL, (hOBJECT) _this);
										break;
									}
								}
							}
						}
						CALL_SYS_ObjectClose(hIo);
					}
				}

				CALL_SYS_ObjectClose(hPtr);
			}

			CALL_Buffer_Unlock(hBuffer);
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"OS::IOCreate\" method, ret hIO = %S, error = 0x%08x", ret_val, error );
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
	error = errNOT_SUPPORTED;

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
	error = errNOT_SUPPORTED;

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
	_this->data->m_PID = -1;

	PR_TRACE_A1( _this, "Leave \"OS::Delete\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



