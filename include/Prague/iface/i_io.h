// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  25 March 2005,  16:09 --------
// File Name   -- (null)i_io.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_io__37b13af8_ae7b_11d4_b757_00d0b7170e50 )
#define __i_io__37b13af8_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_IO  ((tIID)(10))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         IO_SIZE_TYPE; //  --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )

// ----------  Object size type constants  ----------
#define IO_SIZE_TYPE_EXPLICIT          ((IO_SIZE_TYPE)(0)) //  --
#define IO_SIZE_TYPE_APPROXIMATE       ((IO_SIZE_TYPE)(1)) //  --
#define IO_SIZE_TYPE_ESTIMATE          ((IO_SIZE_TYPE)(2)) //  --

// ----------            --  Access constants-flags  ----------
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
#define fACCESS_FILE_FLAG_NO_BUFFERING ((tDWORD)(0x00000200L)) // Bypass windows cache manager
#define mACCESS                        ((tDWORD)(0x000003ffL)) // mask to extract access mode flags

// ----------  Open mode constants  ----------
// These constants are valued as "open_mode" parameter of OS::IOCreate, ObjEnum::CreateCurrentObj methods and as IO::pgOBJECT_OPEN_MODE property

// ----------            --  Creation-disposition flagss  ----------
#define fOMODE_OPEN_IF_EXIST           ((tDWORD)(0x00000001L)) // Creates a new object. Fails if the specified file already exists.
#define fOMODE_CREATE_IF_NOT_EXIST     ((tDWORD)(0x00000002L)) // Create new object. Old object will be deleted
#define fOMODE_TRUNCATE                ((tDWORD)(0x00000004L)) // Opens the existing object. Once opened, the object must be truncated to zero length. Access mode must be at least fACCESS_WRITE
#define fOMODE_FOLDER_AS_IO            ((tDWORD)(0x00000008L)) // open as folder. If object with this name exists and represents a file method must return error
#define fOMODE_OPEN_ALWAYS             ((tDWORD)(fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST)) // open always behaviour
#define fOMODE_CREATE_ALWAYS           ((tDWORD)(fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST | fOMODE_TRUNCATE)) // create always behaviour
#define mOMODE_CREATION                ((tDWORD)(0x0000000fL)) // open mode mask for creation/disposition flags

// ----------            --  Shared mode flags  ----------
#define fOMODE_SHARE_DENY_READ         ((tDWORD)(0x00000010L)) // Subsequent open operations on the object will succeed only if read access is requested
#define fOMODE_SHARE_DENY_WRITE        ((tDWORD)(0x00000020L)) // Subsequent open operations on the object will succeed only if write access is requested
#define fOMODE_SHARE_DENY_RW           ((tDWORD)(0x00000030L)) // Subsequent open operations on the object will succeed only if read or write access is requested
#define fOMODE_SHARE_DENY_DELETE       ((tDWORD)(0x00000040L)) // Subsequent open operations on the object will succeed only if delete access is requested
#define mOMODE_SHARE                   ((tDWORD)(0x000000f0L)) // open mode share mask

// ----------            --  temporary flag  ----------
#define fOMODE_TEMPORARY               ((tDWORD)(0x00000100L)) // object is temporary and must be deleted on close without any messages

// ----------             --  prague specific flags  ----------
#define fOMODE_FILE_NAME_PREPARSING    ((tDWORD)(0x00001000L)) // file name will be preparsed by Windows

// ----------  availability flags  ----------
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



// --------------------------------------------------------------------------------
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

	#define pm_IO_PASSWORD_EXE_HEURISTIC   0x00002001


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
#define errIO_DELETE_ON_CLOSE_FAILED             PR_MAKE_DECL_ERR(IID_IO, 0x001) // 0x8000a001,-2147442687 (1) -- set if delete on close failed
#define errIO_FOLDER_NOT_FILE                    PR_MAKE_DECL_ERR(IID_IO, 0x002) // 0x8000a002,-2147442686 (2) -- object opened is a folder, not a file
#define errIO_BUFFERING                          PR_MAKE_DECL_ERR(IID_IO, 0x003) // 0x8000a003,-2147442685 (3) -- buffering is not available for the object
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hIO;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iIOVtbl;
typedef struct iIOVtbl iIOVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cIO;
	typedef cIO* hIO;
#else
	typedef struct tag_hIO {
		const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hIO;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( IO_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpIO_SeekRead)  ( hIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
	typedef   tERROR (pr_call *fnpIO_SeekWrite) ( hIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
	typedef   tERROR (pr_call *fnpIO_GetSize)   ( hIO _this, tQWORD* result, IO_SIZE_TYPE type );         // -- returns size of the requested type of the object;
	typedef   tERROR (pr_call *fnpIO_SetSize)   ( hIO _this, tQWORD new_size );           // -- resizes the object;
	typedef   tERROR (pr_call *fnpIO_Flush)     ( hIO _this );                            // -- flush internal buffers;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iIOVtbl {
	fnpIO_SeekRead   SeekRead;
	fnpIO_SeekWrite  SeekWrite;
	fnpIO_GetSize    GetSize;
	fnpIO_SetSize    SetSize;
	fnpIO_Flush      Flush;
}; // "IO" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( IO_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_ORIGIN           mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
//! #define pgOBJECT_NAME             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_PATH             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000044 )
//! #define pgOBJECT_FULL_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
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
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_IO_SeekRead( _this, result, offset, buffer, size ) ((_this)->vtbl->SeekRead( (_this), result, offset, buffer, size ))
	#define CALL_IO_SeekWrite( _this, result, offset, buffer, size ) ((_this)->vtbl->SeekWrite( (_this), result, offset, buffer, size ))
	#define CALL_IO_GetSize( _this, result, type )                  ((_this)->vtbl->GetSize( (_this), result, type ))
	#define CALL_IO_SetSize( _this, new_size )                      ((_this)->vtbl->SetSize( (_this), new_size ))
	#define CALL_IO_Flush( _this )                                  ((_this)->vtbl->Flush( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_IO_SeekRead( _this, result, offset, buffer, size ) ((_this)->SeekRead( result, offset, buffer, size ))
	#define CALL_IO_SeekWrite( _this, result, offset, buffer, size ) ((_this)->SeekWrite( result, offset, buffer, size ))
	#define CALL_IO_GetSize( _this, result, type )                  ((_this)->GetSize( result, type ))
	#define CALL_IO_SetSize( _this, new_size )                      ((_this)->SetSize( new_size ))
	#define CALL_IO_Flush( _this )                                  ((_this)->Flush( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iIO {
		virtual tERROR pr_call SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- reads content of the object to the buffer
		virtual tERROR pr_call SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- writes buffer to the object
		virtual tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE type ) = 0; // -- returns size of the requested type of the object
		virtual tERROR pr_call SetSize( tQWORD new_size ) = 0; // -- resizes the object
		virtual tERROR pr_call Flush() = 0; // -- flush internal buffers
	};

	struct pr_abstract cIO : public iIO, public iObj {

		OBJ_IMPL( cIO );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hIO()   { return (hIO)this; }

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

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end


#if defined (__cplusplus) && !defined(_USE_VTBL) && !defined ( __RPC__ )
#  include "../pr_cpp.h"
	// ---
	struct cIOObj : public cAutoObj<cIO> {
		public:
			cIOObj(cObject* parent, tDWORD access_mode, tDWORD open_mode=fOMODE_OPEN_IF_EXIST, tPID pid=PID_NATIVE_FIO ) :
				m_last_err(errOK) {
				create( parent, NULL, access_mode, open_mode, pid );
			}
			cIOObj(cObject* parent, const cString* name, tDWORD access_mode, tDWORD open_mode=fOMODE_OPEN_IF_EXIST, tPID pid=PID_NATIVE_FIO ) :
				m_last_err(errOK) {
				create( parent, name, access_mode, open_mode, pid );
			}
			cIOObj(cObject* parent, const cStringObj& name, tDWORD access_mode, tDWORD open_mode=fOMODE_OPEN_IF_EXIST, tPID pid=PID_NATIVE_FIO ) :
				m_last_err(errOK) {
				cAutoString sname( name );
				create( parent, sname, access_mode, open_mode, pid );
			}
			cIOObj( cObject* parent, const tCHAR* name, tDWORD access_mode, tDWORD open_mode=fOMODE_OPEN_IF_EXIST, tPID pid=PID_NATIVE_FIO ) :
				m_last_err(errOK) {
				cAutoObj<cString> sname;
				sname.create((cObject *)g_root, IID_STRING);
				sname->ImportFromBuff( 0, (tPTR)name, 0, cCP_ANSI, cSTRING_Z );
				create( parent, sname, access_mode, open_mode, pid);
			}
			cIOObj( cObject* parent, const tWCHAR* name, tDWORD access_mode, tDWORD open_mode=fOMODE_OPEN_IF_EXIST, tPID pid=PID_NATIVE_FIO ) :
				m_last_err(errOK) {
				cAutoObj<cString> sname;
				sname.create((cObject *)g_root, IID_STRING);
				sname->ImportFromBuff( 0, (tPTR)name, 0, cCP_UNICODE, cSTRING_Z );
				create( parent, sname, access_mode, open_mode, pid);
			}
			cIOObj() : m_last_err(errOK) {}

			tERROR create( cObject* parent, const cString* name, tDWORD access_mode, tDWORD open_mode=fOMODE_OPEN_IF_EXIST, tPID pid=PID_NATIVE_FIO);
			tERROR last_error() const { return m_last_err; }
			static tERROR copy(cIO* pDst, cIO* pSrc, tDWORD nChunkSize = 0x1000);

			//template < int SIZE >
			//	static tERROR copy2( cIO* pDst, cIO* pSrc ) {
			//		return io_copy<SIZE>io_copy(pDst,pSrc);
			//	}
			
		private:
			tERROR m_last_err;
	};


	// ---
	inline tERROR cIOObj::create(cObject* owner, const cString* name, tDWORD access_mode, tDWORD open_mode, tPID pid)
	{
		if( !owner )
			m_last_err = errPARAMETER_INVALID;
		if ( PR_SUCC(m_last_err) )
			m_last_err = owner->sysCreateObject((hOBJECT*)&m_obj, IID_IO, pid);
		if( PR_SUCC(m_last_err) && name )
			m_last_err = const_cast<cString*>(name)->ExportToProp(NULL, cSTRING_WHOLE, (cObject*)m_obj, pgOBJECT_FULL_NAME);
		if( PR_SUCC(m_last_err) )
			m_last_err = m_obj->propSetDWord(pgOBJECT_OPEN_MODE, open_mode);
		if( PR_SUCC(m_last_err) )
			m_last_err = m_obj->propSetDWord(pgOBJECT_ACCESS_MODE, access_mode);
		if( PR_SUCC(m_last_err) )
			m_last_err = m_obj->sysCreateObjectDone();

		return m_last_err;
	}

	// ---
	inline tERROR cIOObj::copy( cIO* pDst, cIO* pSrc, tDWORD nChunkSize ) {
		tERROR nErr = errOK;
		
		tBYTE* pData = new tBYTE[nChunkSize];
		if( !pData )
			return errNOT_ENOUGH_MEMORY;
		
		tQWORD qwPos = 0;
		tDWORD dwBytes;
		tDWORD dwWritten;
		for(;;)
		{
			nErr = pSrc->SeekRead(&dwBytes, qwPos, pData, nChunkSize);
			if( nErr == errOUT_OF_OBJECT || !dwBytes )
			{
				nErr = errOK;
				break;
			}
			if( PR_FAIL(nErr) )
				break;
			nErr = pDst->SeekWrite(&dwWritten, qwPos, pData, dwBytes);
			if( PR_FAIL(nErr) )
				break;
			if( dwWritten != dwBytes )
			{
				nErr = errOBJECT_WRITE;
				break;
			}
			qwPos += dwBytes;
		}
		
		delete [] pData;
		return nErr;
	}

#endif // defined (__cplusplus )


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_io__37b13af8_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(IO_PROXY_DEFINITION)
#define IO_PROXY_DEFINITION

#define PROXY_IO_CHUNK_SIZE  0x2000

struct IOProxy_Data
{
	hIO			tmp_io;
	tQWORD		offset;
	unsigned	modify:1;
	unsigned	native:1;
	unsigned	read_all:1;
	unsigned	init_fail:1;
	unsigned	write_mode:1;
};

tERROR pr_call InitBuffering(hIO _this, IOProxy_Data *data, tBOOL bForWrite)
{
	if( data->init_fail )
		return errOBJECT_BAD_INTERNAL_STATE;

	if( data->tmp_io )
	{
		if( bForWrite && data->native && !data->write_mode )
		{
			data->write_mode = 1;
			return CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		}
		return errOK;
	}

	tCHAR path[0x200];
	tERROR error = CALL_SYS_PropertyGetStr(_this, NULL, pgOBJECT_NATIVE_PATH, path, sizeof(path), cCP_ANSI);
	if( PR_SUCC(error) )
		error = CALL_SYS_ObjectCreate(_this, &data->tmp_io, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY);
	if( PR_SUCC(error) ) {
		CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
		CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_READ);
		CALL_SYS_PropertySetStr(data->tmp_io, 0, pgOBJECT_NAME, path, 0, cCP_ANSI);
		error = CALL_SYS_ObjectCreateDone(data->tmp_io);	
	}
	if( PR_SUCC(error) ) {
		data->native = 1;
		data->read_all = 1;
	}
	else {
		if( data->tmp_io )
			CALL_SYS_ObjectClose(data->tmp_io);

		error = CALL_SYS_ObjectCreate(_this, &data->tmp_io, IID_IO, PID_TMPFILE, 0);
		if( PR_SUCC(error) ) {
			CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
			CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
			error = CALL_SYS_ObjectCreateDone(data->tmp_io);
		}
	}
	if( PR_FAIL(error) )
		data->init_fail = 1;

	return error;
}

tERROR pr_call ReadBuffering(hIO _this, IOProxy_Data *data, tQWORD size)
{
	tCHAR	buffer[PROXY_IO_CHUNK_SIZE];
	tDWORD	read_size = sizeof(buffer);
	tQWORD	io_size = 0;
	tERROR	error = errOK;

	if( size < sizeof(buffer) )
		size = sizeof(buffer);

	while( PR_SUCC(error) && read_size == sizeof(buffer) && io_size < size ) {
		PR_INVOKE(IO, SeekRead, PR_ARG3(tid_DWORD|tid_POINTER, tid_QWORD, tid_BINARY|prf_OUT),
			PR_ARG4(&read_size, data->offset, buffer, sizeof(buffer)));
		if( error == errEOF )
			data->read_all = 1;

		if( PR_SUCC(error) )
			error = CALL_IO_SeekWrite( data->tmp_io, NULL, data->offset, buffer, read_size );

		data->offset += read_size;
		io_size += read_size;
	}

	return error;
}

tERROR pr_call FlushBuffering(hIO _this)
{
	PR_USE_DATA(IOProxy_Data)

	if( !data->modify || !data->tmp_io )
		return errOK;

	tERROR error = errOK;
	if( !data->native ) {
		tCHAR	buffer[PROXY_IO_CHUNK_SIZE];
		tDWORD	read_size = sizeof(buffer);
		tQWORD	offset = 0;

		while( PR_SUCC(error) && read_size == sizeof(buffer) ) {
			error = CALL_IO_SeekRead( data->tmp_io, &read_size, offset, buffer, read_size );

			if( PR_SUCC(error) )
				error = PR_INVOKE(IO, SeekWrite, PR_ARG3(tid_DWORD|tid_POINTER, tid_QWORD, tid_BINARY|prf_IN),
					PR_ARG4(NULL, offset, buffer, read_size));

			offset += read_size;
		}

		if( PR_SUCC(error) )
			error = PR_INVOKE(IO, SetSize, PR_ARG1(tid_QWORD), PR_ARG1(offset));
	}
	else {
		tBYTE ch;
		PR_INVOKE(IO, SeekRead, PR_ARG3(tid_DWORD|tid_POINTER, tid_QWORD, tid_BINARY|prf_OUT),
			PR_ARG4(NULL, (tQWORD)0, (tCHAR*)&ch, sizeof(ch)));

		PR_INVOKE(IO, SeekWrite, PR_ARG3(tid_DWORD|tid_POINTER, tid_QWORD, tid_BINARY|prf_IN),
			PR_ARG4(NULL, (tQWORD)0, (tCHAR*)&ch, sizeof(ch)));
	}

	data->modify = 0;
	return error;
}

// ---
tERROR pr_call ProxyIO_SeekRead( hIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	PR_USE_DATA(IOProxy_Data)

	tERROR error = InitBuffering(_this, data, cFALSE);

	if( PR_SUCC(error) && !data->read_all && data->offset < (offset + size) )
		error = ReadBuffering(_this, data, offset + size - data->offset);

	if( PR_SUCC(error) )
		error = CALL_IO_SeekRead(data->tmp_io, result, offset, buffer, size);

	return error;
}
// ---
tERROR pr_call ProxyIO_SeekWrite( hIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	PR_USE_DATA(IOProxy_Data)

	tERROR error = InitBuffering(_this, data, cTRUE);

	if( PR_SUCC(error) && !data->read_all && data->offset < (offset + size) )
		error = ReadBuffering(_this, data, offset + size - data->offset);

	if( PR_SUCC(error) )
		error = CALL_IO_SeekWrite(data->tmp_io, result, offset, buffer, size);

	if( PR_SUCC(error) )
		data->modify = 1;

	return error;
}
// ---
tERROR pr_call ProxyIO_GetSize( hIO _this, tQWORD* result, IO_SIZE_TYPE type )
{
	PR_USE_DATA(IOProxy_Data)

	if( data->read_all )
		return CALL_IO_GetSize(data->tmp_io, result, type);

	return PR_INVOKE(IO, GetSize, PR_ARG2(tid_QWORD|tid_POINTER, tid_DWORD), PR_ARG2(result, type));
}
// ---
tERROR pr_call ProxyIO_SetSize( hIO _this, tQWORD new_size )
{
	PR_USE_DATA(IOProxy_Data)

	tERROR error;
	if( data->read_all || new_size < data->offset ) {
		InitBuffering(_this, data, cTRUE);
		error = CALL_IO_SetSize(data->tmp_io, new_size);
		if( PR_SUCC(error) )
			data->read_all = 1;
	}
	else
		error = PR_INVOKE(IO, SetSize, PR_ARG1(tid_QWORD), PR_ARG1(new_size));

	if( PR_SUCC(error) )
		data->modify = 1;

	return error;
}
// ---
tERROR pr_call ProxyIO_Flush( hIO _this )
{
	tERROR error = FlushBuffering(_this);
	if( PR_FAIL(error) )
		return error;

	return PR_INVOKE(IO, Flush, PR_ARG0(), PR_ARG0());
}

iINTERNAL io_internal =
{
	NULL,// IntFnRecognize,
	NULL,// IntFnObjectNew,
	NULL,// IntFnObjectInit,
	NULL,// IntFnObjectInitDone,
	NULL,// IntFnObjectCheck,
	(tIntFnObjectPreClose)FlushBuffering,
	NULL,// IntFnObjectClose,
	NULL,// IntFnChildNew,
	NULL,// IntFnChildInitDone,
	NULL,// IntFnChildClose,
	NULL,// IntFnMsgReceive,
	NULL// IntFnIFaceTransfer
};

// ---
iIOVtbl g_IO = {
	ProxyIO_SeekRead,
	ProxyIO_SeekWrite,
	ProxyIO_GetSize,
	ProxyIO_SetSize,
	ProxyIO_Flush,
}; // "IO" external virtual table prototype

PR_PROXY_IMPLEMENT(IO, IID_IO, &io_internal, sizeof(IOProxy_Data), 0)

#endif // __PROXY_DECLARATION
