// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  15 February 2005,  17:49 --------
// File Name   -- (null)i_decodeio.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_decodeio__1bf6e0da_7953_4b99_95c0_ebff241b8fff )
#define __i_decodeio__1bf6e0da_7953_4b99_95c0_ebff241b8fff
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// DecodeIO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_DECODEIO  ((tIID)(54000))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         IO_SIZE_TYPE; //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// Object size type constants
#define IO_SIZE_TYPE_EXPLICIT          ((IO_SIZE_TYPE)(0)) //  --
#define IO_SIZE_TYPE_APPROXIMATE       ((IO_SIZE_TYPE)(1)) //  --
#define IO_SIZE_TYPE_ESTIMATE          ((IO_SIZE_TYPE)(2)) //  --

//           --  Access constants-flags
// These flags are valued as "access_mode" parameter of OS::IOCreate, ObjEnum::CreateCurrentObj methods and as IO::pgOBJECT_ACCESS_MODE property
// flags are acceptable for open_mode parameter of CreateObject methods of OS and ObjEnum interfaces and for IO pgACCESS_MODE property
#define fACCESS_READ                   ((tDWORD)(0x00000001L)) // read access to the object
#define fACCESS_WRITE                  ((tDWORD)(0x00000002L)) // write access to the object
#define fACCESS_RW                     ((tDWORD)(0x00000003L)) // read/write access to the object. The same as (cOS_OMODE_READ | cOS_OMODE_WRITE)
#define fACCESS_NO_EXTEND_RAND         ((tDWORD)(0x00000004L)) // object must not be able to extend randomly
#define fACCESS_NO_EXTEND              ((tDWORD)(0x00000008L)) // object must not be able to extend
#define fACCESS_NO_TRUNCATE            ((tDWORD)(0x00000010L)) // object cannot be truncated
#define fACCESS_NO_BUFFERING_READ      ((tDWORD)(0x00000020L)) // client do not want (if possible) the object to be read buffered
#define fACCESS_NO_BUFFERING_WRITE     ((tDWORD)(0x00000040L)) // client do not want (if possible) the object to be write buffered
#define fACCESS_NO_BUFFERING           ((tDWORD)(0x00000060L)) // client do not want (if possible) the object to be read buffered
#define fACCESS_NO_CHANGE_MODE         ((tDWORD)(0x00000080L)) // creater do not want the object to be reopened by set new pgACCESS_MODE property value
#define fACCESS_FORCE_READ             ((tDWORD)(0x00000100L)) // ignore sharing/lock during read operation
#define mACCESS                        ((tDWORD)(0x000003ffL)) // mask to extract access mode flags

// Open mode constants
// These constants are valued as "open_mode" parameter of OS::IOCreate, ObjEnum::CreateCurrentObj methods and as IO::pgOBJECT_OPEN_MODE property

//           --  Creation-disposition flagss
#define fOMODE_OPEN_IF_EXIST           ((tDWORD)(0x00000001L)) // Creates a new object. Fails if the specified file already exists.
#define fOMODE_CREATE_IF_NOT_EXIST     ((tDWORD)(0x00000002L)) // Create new object. Old object will be deleted
#define fOMODE_TRUNCATE                ((tDWORD)(0x00000004L)) // Opens the existing object. Once opened, the object must be truncated to zero length. Access mode must be at least fACCESS_WRITE
#define fOMODE_FOLDER_AS_IO            ((tDWORD)(0x00000008L)) // open as folder. If object with this name exists and represents a file method must return error
#define fOMODE_OPEN_ALWAYS             ((tDWORD)(fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST)) // open always behaviour
#define fOMODE_CREATE_ALWAYS           ((tDWORD)(fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST | fOMODE_TRUNCATE)) // create always behaviour
#define mOMODE_CREATION                ((tDWORD)(0x0000000fL)) // open mode mask for creation/disposition flags

//           --  Shared mode flags
#define fOMODE_SHARE_DENY_READ         ((tDWORD)(0x00000010L)) // Subsequent open operations on the object will succeed only if read access is requested
#define fOMODE_SHARE_DENY_WRITE        ((tDWORD)(0x00000020L)) // Subsequent open operations on the object will succeed only if write access is requested
#define fOMODE_SHARE_DENY_RW           ((tDWORD)(0x00000030L)) // Subsequent open operations on the object will succeed only if read or write access is requested
#define fOMODE_SHARE_DENY_DELETE       ((tDWORD)(0x00000040L)) // Subsequent open operations on the object will succeed only if delete access is requested
#define mOMODE_SHARE                   ((tDWORD)(0x000000f0L)) // open mode share mask

//           --  temporary flag
#define fOMODE_TEMPORARY               ((tDWORD)(0x00000100L)) // object is temporary and must be deleted on close without any messages

// availability flags
// flags to tell them what you can do. Used as pgOBJECT_AVAILABILITY property value
#define fAVAIL_READ                    ((tDWORD)(0x00000001L)) // read operation available
#define fAVAIL_WRITE                   ((tDWORD)(0x00000002L)) // write operation available
#define fAVAIL_EXTEND                  ((tDWORD)(0x0000000cL)) // both sequential and random extend operation available
#define fAVAIL_EXTEND_SEQ              ((tDWORD)(0x00000004L)) // sequential extend operation available
#define fAVAIL_EXTEND_RAND             ((tDWORD)(0x00000008L)) // random extend operation available
#define fAVAIL_TRUNCATE                ((tDWORD)(0x00000010L)) // truncate operation available
#define fAVAIL_BUFFERING               ((tDWORD)(0x00000060L)) // both read and write buffering available
#define fAVAIL_BUFFERING_READ          ((tDWORD)(0x00000020L)) // read buffering available
#define fAVAIL_BUFFERING_WRITE         ((tDWORD)(0x00000040L)) // write buffering available
#define fAVAIL_CHANGE_MODE             ((tDWORD)(0x00000080L)) // change mode (reopen) operation available
#define fAVAIL_CREATE                  ((tDWORD)(0x00000100L)) // creation available
#define fAVAIL_SHARE                   ((tDWORD)(0x00000200L)) // sharing available
#define fAVAIL_DELETE_ON_CLOSE         ((tDWORD)(0x00000400L)) // delete on close operation available
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
//    параметр "send_point" в любом из этих сообщений может быть любого из трех типов: OS, ObjPtr или IO
#define pmc_IO 0x52e986b3 // класс сообщений связанных с деятельностью объектов OS, ObjPtr и IO

	#define pm_IO_DELETE_ON_CLOSE_SUCCEED 0x00000001 // (1) -- удаление при закрытии прошло успешно
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tDWORD, содержащую код ошибки errOK

	#define pm_IO_DELETE_ON_CLOSE_FAILED 0x00000002 // (2) -- удаление при закрытии завершилось с ошибкой
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tDWORD, содержащую код ошибки, "родной" для объектной системы (контейнера)

	#define pm_IO_PASSWORD_REQUEST 0x00000003 // (3) -- запрос пароля для доступа к физическому объекту
	// context comment
	//    объект типа hSTRING.
	//

	#define pm_IO_CLOSE_ERROR 0x00001000 // (4096) -- ошибка при закрытии объектов типов hIO и hOS
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tERROR, содержащую код ошибки

	#define pm_IO_CREATE_FAILED 0x00001001 // (4097) -- ошибка при открытии объектов типов hIO и hOS
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tERROR, содержащую код ошибки

	#define pm_IO_REOPEN_FAILED 0x00001002 // (4098) -- ошибка при переоткрытии объектов типов hIO
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tERROR, содержащую код ошибки

	#define pm_IO_SET_ATTRIBUTES_FAILED 0x00001003 // (4099) -- ошибка при установке аттрибутов на объект типа hIO
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tERROR, содержащую код ошибки

	#define pm_IO_RESORE_ATTRIBUTES_FAILED 0x00001004 // (4100) -- ошибка возвращении аттрибутов в исходные на объект типа hIO
	// context comment
	//    NULL
	// data content comment
	//    указатель на переменную типа tERROR, содержащую код ошибки
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
//#define errIO_DELETE_ON_CLOSE_FAILED             PR_MAKE_DECL_ERR(IID_DECODEIO, 0x001) // 0x8d2f0001,-1926299647 (1) -- set if delete on close failed
//#define errIO_FOLDER_NOT_FILE                    PR_MAKE_DECL_ERR(IID_DECODEIO, 0x002) // 0x8d2f0002,-1926299646 (2) -- object opened is a folder, not a file
//#define errIO_BUFFERING                          PR_MAKE_DECL_ERR(IID_DECODEIO, 0x003) // 0x8d2f0003,-1926299645 (3) -- buffering is not available for the object
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hDECODEIO;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iDecodeIOVtbl;
typedef struct iDecodeIOVtbl iDecodeIOVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cDecodeIO;
	typedef cDecodeIO* hDECODEIO;
#else
	typedef struct tag_hDECODEIO 
	{
		const iDecodeIOVtbl* vtbl; // pointer to the "DecodeIO" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hDECODEIO;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( DecodeIO_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpDecodeIO_SeekRead)  ( hDECODEIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
	typedef   tERROR (pr_call *fnpDecodeIO_SeekWrite) ( hDECODEIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
	typedef   tERROR (pr_call *fnpDecodeIO_GetSize)   ( hDECODEIO _this, tQWORD* result, IO_SIZE_TYPE type );   // -- returns size of the requested type of the object;
	typedef   tERROR (pr_call *fnpDecodeIO_SetSize)   ( hDECODEIO _this, tQWORD new_size );     // -- resizes the object;
	typedef   tERROR (pr_call *fnpDecodeIO_Flush)     ( hDECODEIO _this );                      // -- flush internal buffers;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iDecodeIOVtbl 
{
	fnpDecodeIO_SeekRead   SeekRead;
	fnpDecodeIO_SeekWrite  SeekWrite;
	fnpDecodeIO_GetSize    GetSize;
	fnpDecodeIO_SetSize    SetSize;
	fnpDecodeIO_Flush      Flush;
}; // "DecodeIO" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( DecodeIO_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_ORIGIN           mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
//! #define pgOBJECT_NAME             mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000040 )
//! #define pgOBJECT_PATH             mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000044 )
//! #define pgOBJECT_FULL_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00000041 )
//! #define pgOBJECT_OPEN_MODE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004a )
#define pgOBJECT_ACCESS_MODE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004e )
#define pgOBJECT_DELETE_ON_CLOSE  mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
//! #define pgOBJECT_AVAILABILITY     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
//! #define pgOBJECT_BASED_ON         mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000053 )
#define pgOBJECT_FILL_CHAR        mPROPERTY_MAKE_GLOBAL( pTYPE_BYTE    , 0x00001004 )
//! #define pgOBJECT_HASH             mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00000055 )
//! #define pgOBJECT_REOPEN_DATA      mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00000056 )
//! #define pgOBJECT_ATTRIBUTES       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000054 )
#define pgOBJECT_CREATION_TIME    mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001005 )
#define pgOBJECT_LAST_WRITE_TIME  mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001006 )
#define pgOBJECT_LAST_ACCESS_TIME mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001007 )
//! #define pgPLUGIN_CODEPAGE         mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000c5 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_DecodeIO_SeekRead( _this, result, offset, buffer, size )       ((_this)->vtbl->SeekRead( (_this), result, offset, buffer, size ))
	#define CALL_DecodeIO_SeekWrite( _this, result, offset, buffer, size )      ((_this)->vtbl->SeekWrite( (_this), result, offset, buffer, size ))
	#define CALL_DecodeIO_GetSize( _this, result, type )                        ((_this)->vtbl->GetSize( (_this), result, type ))
	#define CALL_DecodeIO_SetSize( _this, new_size )                            ((_this)->vtbl->SetSize( (_this), new_size ))
	#define CALL_DecodeIO_Flush( _this )                                        ((_this)->vtbl->Flush( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_DecodeIO_SeekRead( _this, result, offset, buffer, size )       ((_this)->SeekRead( result, offset, buffer, size ))
	#define CALL_DecodeIO_SeekWrite( _this, result, offset, buffer, size )      ((_this)->SeekWrite( result, offset, buffer, size ))
	#define CALL_DecodeIO_GetSize( _this, result, type )                        ((_this)->GetSize( result, type ))
	#define CALL_DecodeIO_SetSize( _this, new_size )                            ((_this)->SetSize( new_size ))
	#define CALL_DecodeIO_Flush( _this )                                        ((_this)->Flush( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iDecodeIO 
	{
		virtual tERROR pr_call SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- reads content of the object to the buffer
		virtual tERROR pr_call SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- writes buffer to the object
		virtual tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE type ) = 0; // -- returns size of the requested type of the object
		virtual tERROR pr_call SetSize( tQWORD new_size ) = 0; // -- resizes the object
		virtual tERROR pr_call Flush() = 0; // -- flush internal buffers
	};

	struct pr_abstract cDecodeIO : public iDecodeIO, public iObj {

		OBJ_IMPL( cDecodeIO );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hDECODEIO()   { return (hDECODEIO)this; }

		tORIG_ID pr_call get_pgOBJECT_ORIGIN() { return (tORIG_ID)getDWord(pgOBJECT_ORIGIN); }
		tERROR pr_call set_pgOBJECT_ORIGIN( tORIG_ID value ) { return setDWord(pgOBJECT_ORIGIN,(tDWORD)value); }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_PATH,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_PATH,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_FULL_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_FULL_NAME,buff,size,cp); }

		tDWORD pr_call get_pgOBJECT_OPEN_MODE() { return (tDWORD)getDWord(pgOBJECT_OPEN_MODE); }
		tERROR pr_call set_pgOBJECT_OPEN_MODE( tDWORD value ) { return setDWord(pgOBJECT_OPEN_MODE,(tDWORD)value); }

		tDWORD pr_call get_pgOBJECT_ACCESS_MODE() { return (tDWORD)getDWord(pgOBJECT_ACCESS_MODE); }
		tERROR pr_call set_pgOBJECT_ACCESS_MODE( tDWORD value ) { return setDWord(pgOBJECT_ACCESS_MODE,(tDWORD)value); }

		tBOOL pr_call get_pgOBJECT_DELETE_ON_CLOSE() { return (tBOOL)getBool(pgOBJECT_DELETE_ON_CLOSE); }
		tERROR pr_call set_pgOBJECT_DELETE_ON_CLOSE( tBOOL value ) { return setBool(pgOBJECT_DELETE_ON_CLOSE,(tBOOL)value); }

		tDWORD pr_call get_pgOBJECT_AVAILABILITY() { return (tDWORD)getDWord(pgOBJECT_AVAILABILITY); }
		tERROR pr_call set_pgOBJECT_AVAILABILITY( tDWORD value ) { return setDWord(pgOBJECT_AVAILABILITY,(tDWORD)value); }

		hOBJECT pr_call get_pgOBJECT_BASED_ON() { return (hOBJECT)getObj(pgOBJECT_BASED_ON); }
		tERROR pr_call set_pgOBJECT_BASED_ON( hOBJECT value ) { return setObj(pgOBJECT_BASED_ON,(hOBJECT)value); }

		tBYTE pr_call get_pgOBJECT_FILL_CHAR() { return (tBYTE)getByte(pgOBJECT_FILL_CHAR); }
		tERROR pr_call set_pgOBJECT_FILL_CHAR( tBYTE value ) { return setByte(pgOBJECT_FILL_CHAR,(tBYTE)value); }

		tQWORD pr_call get_pgOBJECT_HASH() { return (tQWORD)getQWord(pgOBJECT_HASH); }
		tERROR pr_call set_pgOBJECT_HASH( tQWORD value ) { return setQWord(pgOBJECT_HASH,(tQWORD)value); }

		tERROR pr_call get_pgOBJECT_REOPEN_DATA( tPTR value, tDWORD size ) { return get(pgOBJECT_REOPEN_DATA,value,size); }
		tERROR pr_call set_pgOBJECT_REOPEN_DATA( tPTR value, tDWORD size ) { return set(pgOBJECT_REOPEN_DATA,value,size); }

		tDWORD pr_call get_pgOBJECT_ATTRIBUTES() { return (tDWORD)getDWord(pgOBJECT_ATTRIBUTES); }
		tERROR pr_call set_pgOBJECT_ATTRIBUTES( tDWORD value ) { return setDWord(pgOBJECT_ATTRIBUTES,(tDWORD)value); }

		tERROR pr_call get_pgOBJECT_CREATION_TIME( tDATETIME* value ) { return get(pgOBJECT_CREATION_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgOBJECT_CREATION_TIME( tDATETIME* value ) { return set(pgOBJECT_CREATION_TIME,value,sizeof(tDATETIME)); }

		tERROR pr_call get_pgOBJECT_LAST_WRITE_TIME( tDATETIME* value ) { return get(pgOBJECT_LAST_WRITE_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgOBJECT_LAST_WRITE_TIME( tDATETIME* value ) { return set(pgOBJECT_LAST_WRITE_TIME,value,sizeof(tDATETIME)); }

		tERROR pr_call get_pgOBJECT_LAST_ACCESS_TIME( tDATETIME* value ) { return get(pgOBJECT_LAST_ACCESS_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgOBJECT_LAST_ACCESS_TIME( tDATETIME* value ) { return set(pgOBJECT_LAST_ACCESS_TIME,value,sizeof(tDATETIME)); }

		tCODEPAGE pr_call get_pgPLUGIN_CODEPAGE() { return (tCODEPAGE)getDWord(pgPLUGIN_CODEPAGE); }
		tERROR pr_call set_pgPLUGIN_CODEPAGE( tCODEPAGE value ) { return setDWord(pgPLUGIN_CODEPAGE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_decodeio__1bf6e0da_7953_4b99_95c0_ebff241b8fff
// AVP Prague stamp end



