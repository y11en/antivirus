// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:19 --------
// File Name   -- (null)i_posio.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_posio__7df96402_dc70_4c5e_895e_9ee51d4c4ec1 )
#define __i_posio__7df96402_dc70_4c5e_895e_9ee51d4c4ec1
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// PosIO interface implementation
// Short comments -- IO with Position
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_POSIO  ((tIID)(58008))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hPOSIO;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iPosIOVtbl;
typedef struct iPosIOVtbl iPosIOVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cPosIO;
	typedef cPosIO* hPOSIO;
#else
	typedef struct tag_hPOSIO {
		const iPosIOVtbl*  vtbl; // pointer to the "PosIO" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hPOSIO;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( PosIO_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )



	// ----------- Standard IO methods ----------
	typedef   tERROR (pr_call *fnpPosIO_SeekRead)  ( hPOSIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
	typedef   tERROR (pr_call *fnpPosIO_SeekWrite) ( hPOSIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
	typedef   tERROR (pr_call *fnpPosIO_GetSize)   ( hPOSIO _this, tQWORD* result, IO_SIZE_TYPE type );      // -- returns size of the requested type of the object;
	typedef   tERROR (pr_call *fnpPosIO_SetSize)   ( hPOSIO _this, tQWORD new_size );        // -- resizes the object;
	typedef   tERROR (pr_call *fnpPosIO_Flush)     ( hPOSIO _this );                         // -- flush internal buffers;

	// ----------- Additional PosIO methods ----------
	typedef   tERROR (pr_call *fnpPosIO_Seek)      ( hPOSIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin ); // -- Переставляет текущую позицию в IO;
	typedef   tERROR (pr_call *fnpPosIO_Read)      ( hPOSIO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- Читает указанное кол-во байт с текущей позиции;
	typedef   tERROR (pr_call *fnpPosIO_Write)     ( hPOSIO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- Записывает указанное кол-во байт с текущей позиции;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iPosIOVtbl {

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

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( PosIO_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_PATH             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000044 )
//! #define pgOBJECT_ORIGIN           mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
//! #define pgOBJECT_FULL_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
//! #define pgOBJECT_OPEN_MODE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004a )
#define pgOBJECT_ACCESS_MODE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004e )
#define pgOBJECT_DELETE_ON_CLOSE  mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
//! #define pgOBJECT_AVAILABILITY     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
//! #define pgOBJECT_BASED_ON         mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000053 )
#define pgOBJECT_FILL_CHAR        mPROPERTY_MAKE_GLOBAL( pTYPE_BYTE    , 0x00001004 )
//! #define pgINTERFACE_COMPATIBILITY mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000087 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_PosIO_SeekRead( _this, result, offset, buffer, size )    ((_this)->vtbl->SeekRead( (_this), result, offset, buffer, size ))
	#define CALL_PosIO_SeekWrite( _this, result, offset, buffer, size )   ((_this)->vtbl->SeekWrite( (_this), result, offset, buffer, size ))
	#define CALL_PosIO_GetSize( _this, result, type )                     ((_this)->vtbl->GetSize( (_this), result, type ))
	#define CALL_PosIO_SetSize( _this, new_size )                         ((_this)->vtbl->SetSize( (_this), new_size ))
	#define CALL_PosIO_Flush( _this )                                     ((_this)->vtbl->Flush( (_this) ))
	#define CALL_PosIO_Seek( _this, result, offset, origin )              ((_this)->vtbl->Seek( (_this), result, offset, origin ))
	#define CALL_PosIO_Read( _this, result, buffer, count )               ((_this)->vtbl->Read( (_this), result, buffer, count ))
	#define CALL_PosIO_Write( _this, result, buffer, count )              ((_this)->vtbl->Write( (_this), result, buffer, count ))
#else // if !defined( __cplusplus )
	#define CALL_PosIO_SeekRead( _this, result, offset, buffer, size )    ((_this)->SeekRead( result, offset, buffer, size ))
	#define CALL_PosIO_SeekWrite( _this, result, offset, buffer, size )   ((_this)->SeekWrite( result, offset, buffer, size ))
	#define CALL_PosIO_GetSize( _this, result, type )                     ((_this)->GetSize( result, type ))
	#define CALL_PosIO_SetSize( _this, new_size )                         ((_this)->SetSize( new_size ))
	#define CALL_PosIO_Flush( _this )                                     ((_this)->Flush( ))
	#define CALL_PosIO_Seek( _this, result, offset, origin )              ((_this)->Seek( result, offset, origin ))
	#define CALL_PosIO_Read( _this, result, buffer, count )               ((_this)->Read( result, buffer, count ))
	#define CALL_PosIO_Write( _this, result, buffer, count )              ((_this)->Write( result, buffer, count ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iPosIO {
		virtual tERROR pr_call SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- reads content of the object to the buffer
		virtual tERROR pr_call SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- writes buffer to the object
		virtual tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE type ) = 0; // -- returns size of the requested type of the object
		virtual tERROR pr_call SetSize( tQWORD new_size ) = 0; // -- resizes the object
		virtual tERROR pr_call Flush() = 0; // -- flush internal buffers
		virtual tERROR pr_call Seek( tQWORD* result, tLONGLONG offset, tDWORD origin ) = 0; // -- Переставляет текущую позицию в IO
		virtual tERROR pr_call Read( tDWORD* result, tPTR buffer, tDWORD count ) = 0; // -- Читает указанное кол-во байт с текущей позиции
		virtual tERROR pr_call Write( tDWORD* result, tPTR buffer, tDWORD count ) = 0; // -- Записывает указанное кол-во байт с текущей позиции
	};

	struct pr_abstract cPosIO : public iPosIO, public iObj {

		OBJ_IMPL( cPosIO );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hPOSIO()   { return (hPOSIO)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_PATH,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_PATH,buff,size,cp); }

		tORIG_ID pr_call get_pgOBJECT_ORIGIN() { return (tORIG_ID)getDWord(pgOBJECT_ORIGIN); }
		tERROR pr_call set_pgOBJECT_ORIGIN( tORIG_ID value ) { return setDWord(pgOBJECT_ORIGIN,(tDWORD)value); }

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

		tIID pr_call get_pgINTERFACE_COMPATIBILITY() { return (tIID)getDWord(pgINTERFACE_COMPATIBILITY); }
		tERROR pr_call set_pgINTERFACE_COMPATIBILITY( tIID value ) { return setDWord(pgINTERFACE_COMPATIBILITY,(tDWORD)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_posio__7df96402_dc70_4c5e_895e_9ee51d4c4ec1
// AVP Prague stamp end

