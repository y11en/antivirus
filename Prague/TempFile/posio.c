// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  22 April 2003,  16:36 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин работы с временными файлами
// Author      -- Andy Nikishin
// File Name   -- posio.c
// Additional info
//    Этот плагин создан для работы с временными файлами. При этом файлы первоначально создаются в памяти. Если размер файла превышает некое пороговое значение (в текущей реализации 16 килобайт), происходит создание файла на диске, и в последующем идет работа с файлом через бефер памяти.
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define PosIO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include "posio.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// PosIO interface implementation
// Short comments -- IO with Position
//    Полностью совместим с интерфейсом IO. Представляет упрощенный вариант интерфейса SeqIO. Имеет текущую позицию в IO и может читать/писать от этой позиции.
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface PosIO. Inner data structure

typedef struct tag_sLocals 
{
	hIO      real_io;                   // Handle to NativeIO if created
	tDWORD   m_dwfAccessMode;           // --
	tDWORD   m_dwfOpenMode;             // --
	tQWORD   FileSize;                  // --
	tCHAR    m_szFileName[cMAX_PATH];   // Имя временного файла, если таковой был создан
	tCHAR    m_szPath[cMAX_PATH];       // Путь к временному файлу, если таковой был создан
	tORIG_ID m_ObjectOrigin;            // --
	tBYTE    byFillChar;                // --
	tBOOL    bDeleteOnClose;            // --
	hOBJECT  hBasedOn;                  // --
	hSTRING  hStr;                      // --
	tCHAR    m_szObjectName[cMAX_PATH]; // Имя объекта (может не совпадать с именем файла)
	tCHAR    m_szObjectPath[cMAX_PATH]; // Путь к объекту (может не совпадать с путем к файлу)
	tERROR   WriteBackError;            // --
	tQWORD   m_qwCurPos;                // IO current position
	tQWORD   SwapSize;                  // --
} sLocals;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_PosIO 
{
	const iPosIOVtbl*  vtbl; // pointer to the "PosIO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	sLocals*           data; // pointer to the "PosIO" data structure
} *hi_PosIO;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call PosIO_ObjectInit( hi_PosIO _this );
tERROR pr_call PosIO_ObjectInitDone( hi_PosIO _this );
tERROR pr_call PosIO_ObjectPreClose( hi_PosIO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_PosIO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       PosIO_ObjectInit,
	(tIntFnObjectInitDone)   PosIO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   PosIO_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )



// ----------- Standard IO methods ----------
typedef   tERROR (pr_call *fnpPosIO_SeekRead)  ( hi_PosIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
typedef   tERROR (pr_call *fnpPosIO_SeekWrite) ( hi_PosIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
typedef   tERROR (pr_call *fnpPosIO_GetSize)   ( hi_PosIO _this, tQWORD* result, IO_SIZE_TYPE type );      // -- returns size of the requested type of the object;
typedef   tERROR (pr_call *fnpPosIO_SetSize)   ( hi_PosIO _this, tQWORD new_size );        // -- resizes the object;
typedef   tERROR (pr_call *fnpPosIO_Flush)     ( hi_PosIO _this );                         // -- flush internal buffers;

// ----------- Additional PosIO methods ----------
typedef   tERROR (pr_call *fnpPosIO_Seek)      ( hi_PosIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin ); // -- Переставляет текущую позицию в IO;
typedef   tERROR (pr_call *fnpPosIO_Read)      ( hi_PosIO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- Читает указанное кол-во байт с текущей позиции;
typedef   tERROR (pr_call *fnpPosIO_Write)     ( hi_PosIO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- Записывает указанное кол-во байт с текущей позиции;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )


struct iPosIOVtbl 
{

// ----------- Standard IO methods ----------
	fnpPosIO_SeekRead   SeekRead;
	fnpPosIO_SeekWrite  SeekWrite;
	fnpPosIO_GetSize    GetSize;
	fnpPosIO_SetSize    SetSize;
	fnpPosIO_Flush      Flush;

// ----------- Additional PosIO methods ----------
	fnpPosIO_Seek       Seek;
	fnpPosIO_Read       Read;
	fnpPosIO_Write      Write;
}; // "PosIO" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )



// Standard IO methods
tERROR pr_call PosIO_SeekRead( hi_PosIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call PosIO_SeekWrite( hi_PosIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call PosIO_GetSize( hi_PosIO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call PosIO_SetSize( hi_PosIO _this, tQWORD new_size );
tERROR pr_call PosIO_Flush( hi_PosIO _this );

// Additional PosIO methods
tERROR pr_call PosIO_Seek( hi_PosIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin );
tERROR pr_call PosIO_Read( hi_PosIO _this, tDWORD* result, tPTR buffer, tDWORD count );
tERROR pr_call PosIO_Write( hi_PosIO _this, tDWORD* result, tPTR buffer, tDWORD count );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iPosIOVtbl e_PosIO_vtbl = 
{
	PosIO_SeekRead,
	PosIO_SeekWrite,
	PosIO_GetSize,
	PosIO_SetSize,
	PosIO_Flush,
	PosIO_Seek,
	PosIO_Read,
	PosIO_Write
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call PosIO_PROP_CommonGet( hi_PosIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call PosIO_PROP_CommonSet( hi_PosIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "PosIO". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define PosIO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(PosIO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, PosIO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Temporary File PosIO", 21 )

// IO properties
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, sLocals, m_szObjectName, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PATH, sLocals, m_szObjectPath, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, sLocals, m_ObjectOrigin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, PosIO_PROP_CommonGet, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_OPEN_MODE, PosIO_PROP_CommonGet, PosIO_PROP_CommonSet )
	mLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, PosIO_PROP_CommonGet, PosIO_PROP_CommonSet )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, sLocals, bDeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, sLocals, hBasedOn, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FILL_CHAR, sLocals, byFillChar, cPROP_BUFFER_READ_WRITE )
//!
// PosIO interface is compatible with IO one
	mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(IID_IO)) )
mPROPERTY_TABLE_END(PosIO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "PosIO". Register function
tERROR pr_call PosIO_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter PosIO::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_POSIO,                              // interface identifier
		TEMPIO_ID,                              // plugin identifier
		0x00000000,                             // subtype identifier
		PosIO_VERSION,                          // interface version
		VID_PETROVITCH,                         // interface developer
		&i_PosIO_vtbl,                          // internal(kernel called) function table
		(sizeof(i_PosIO_vtbl)/sizeof(tPTR)),    // internal function table size
		&e_PosIO_vtbl,                          // external function table
		(sizeof(e_PosIO_vtbl)/sizeof(tPTR)),    // external function table size
		PosIO_PropTable,                        // property table
		mPROPERTY_TABLE_SIZE(PosIO_PropTable),  // property table size
		sizeof(sLocals),                        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"PosIO(IID_POSIO) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave PosIO::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonGet )
// Interface "PosIO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
tERROR pr_call PosIO_PROP_CommonGet( hi_PosIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method PosIO_PROP_CommonGet" );

	switch ( prop ) {

		case pgOBJECT_FULL_NAME:
			*out_size = 0;
			break;

		case pgOBJECT_OPEN_MODE:
			*out_size = 0;
			break;

		case pgOBJECT_ACCESS_MODE:
			*out_size = 0;
			break;

		default:
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method PosIO_PROP_CommonGet, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonSet )
// Interface "PosIO". Method "Set" for property(s):
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
tERROR pr_call PosIO_PROP_CommonSet( hi_PosIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* multyproperty method PosIO_PROP_CommonSet" );

	switch ( prop ) {

		case pgOBJECT_OPEN_MODE:
			*out_size = 0;
			break;

		case pgOBJECT_ACCESS_MODE:
			*out_size = 0;
			break;

		default:
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method PosIO_PROP_CommonSet, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call PosIO_ObjectInit( hi_PosIO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter PosIO::ObjectInit method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave PosIO::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call PosIO_ObjectInitDone( hi_PosIO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter PosIO::ObjectInitDone method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave PosIO::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call PosIO_ObjectPreClose( hi_PosIO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter PosIO::ObjectPreClose method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave PosIO::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR pr_call PosIO_SeekRead( hi_PosIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter PosIO::SeekRead method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave PosIO::SeekRead method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call PosIO_SeekWrite( hi_PosIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter PosIO::SeekWrite method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave PosIO::SeekWrite method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR pr_call PosIO_GetSize( hi_PosIO _this, tQWORD* result, IO_SIZE_TYPE type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter PosIO::GetSize method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave PosIO::GetSize method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call PosIO_SetSize( hi_PosIO _this, tQWORD new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter PosIO::SetSize method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave PosIO::SetSize method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call PosIO_Flush( hi_PosIO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter PosIO::Flush method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave PosIO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Seek )
// Extended method comment
//    Текущая позиция сдвигается в зависимости от параметра origin, задающего точку отсчета,  и на величину параметра offset.
// Behaviour comment
//    Возвращает текущую позицию после сдвига.
//
//    Для выяснения текущей позиции в IO можно воспользоваться вызовом Seek(io, &qwCurPos, 0, cSEEK_CUR)
// Parameters are:
//   "origin" : Принимает значения:
//                cSEEK_SET - отсчет от начала файла
//                cSEEK_CUR - отсчет от текущей позиции
//                cSEEK_END - отсчет от конца файла
//
tERROR pr_call PosIO_Seek( hi_PosIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter PosIO::Seek method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave PosIO::Seek method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Read )
// Behaviour comment
//    Возвращает кол-во реально прочитанных байт. Текущая позиция смещается на кол-во реально прочитанных байт.
// Parameters are:
tERROR pr_call PosIO_Read( hi_PosIO _this, tDWORD* result, tPTR buffer, tDWORD count )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter PosIO::Read method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave PosIO::Read method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Write )
// Behaviour comment
//    Возвращает кол-во реально записанных байт. Текущая позиция смещается на кол-во реально записанных байт
// Parameters are:
tERROR pr_call PosIO_Write( hi_PosIO _this, tDWORD* result, tPTR buffer, tDWORD count )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter PosIO::Write method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave PosIO::Write method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



