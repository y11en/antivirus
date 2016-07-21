// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  10 October 2003,  19:34 --------
// File Name   -- (null)i_posio_sp.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_posio_sp__b666c9da_7121_46a1_b07a_a7c26f83d9f3 )
#define __i_posio_sp__b666c9da_7121_46a1_b07a_a7c26f83d9f3
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_posio.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// PosIO_SP interface implementation
// Short comments -- PosIO with Stored Properties
//    Полностью совместим с интерфейсом POSIO. Представляет упрощенный вариант интерфейса SeqIO. Имеет текущую позицию в IO и может читать/писать от этой позиции. Кроме того данный интерфейс позволяет устанавливать дополнительные свойства, которые сохраняются вместе с данными объекта.
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_POSIO_SP  ((tIID)(58012))
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
	typedef tUINT hPOSIO_SP;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iPosIO_SPVtbl;
typedef struct iPosIO_SPVtbl iPosIO_SPVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cPosIO_SP;
	typedef cPosIO_SP* hPOSIO_SP;
#else
	typedef struct tag_hPOSIO_SP {
		const iPosIO_SPVtbl* vtbl; // pointer to the "PosIO_SP" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hPOSIO_SP;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( PosIO_SP_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )



	// ----------- Standard IO methods ----------
	typedef   tERROR (pr_call *fnpPosIO_SP_SeekRead)              ( hPOSIO_SP _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
	typedef   tERROR (pr_call *fnpPosIO_SP_SeekWrite)             ( hPOSIO_SP _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
	typedef   tERROR (pr_call *fnpPosIO_SP_GetSize)               ( hPOSIO_SP _this, tQWORD* result, IO_SIZE_TYPE type ); // -- returns size of the requested type of the object;
	typedef   tERROR (pr_call *fnpPosIO_SP_SetSize)               ( hPOSIO_SP _this, tQWORD new_size ); // -- resizes the object;
	typedef   tERROR (pr_call *fnpPosIO_SP_Flush)                 ( hPOSIO_SP _this );          // -- flush internal buffers;

	// ----------- Additional PosIO methods ----------
	typedef   tERROR (pr_call *fnpPosIO_SP_Seek)                  ( hPOSIO_SP _this, tQWORD* result, tLONGLONG offset, tDWORD origin ); // -- Переставляет текущую позицию в IO;
	typedef   tERROR (pr_call *fnpPosIO_SP_Read)                  ( hPOSIO_SP _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- Читает указанное кол-во байт с текущей позиции;
	typedef   tERROR (pr_call *fnpPosIO_SP_Write)                 ( hPOSIO_SP _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- Записывает указанное кол-во байт с текущей позиции;

	// -----------  Additional PosIO_PS methods ----------
	typedef   tERROR (pr_call *fnpPosIO_SP_GetStoredProperty)     ( hPOSIO_SP _this, tDWORD* result, const tSTRING sPropName, tPTR pBuffer, tDWORD dwSize ); // -- Получить сохраняемое свойство;
	typedef   tERROR (pr_call *fnpPosIO_SP_SetStoredProperty)     ( hPOSIO_SP _this, tDWORD* result, const tSTRING sPropName, const tPTR pBuffer, tDWORD dwSize ); // -- Установить сохраняемое свойство;
	typedef   tERROR (pr_call *fnpPosIO_SP_DeleteStoredProperty)  ( hPOSIO_SP _this, const tSTRING sPropName ); // -- Удалить сохраняемое свойство;
	typedef   tERROR (pr_call *fnpPosIO_SP_GetStoredPropertyName) ( hPOSIO_SP _this, tDWORD* result, tDWORD dwIndex, tSTRING pBuffer, tDWORD dwSize ); // -- Получает имя свойства;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iPosIO_SPVtbl {

// ----------- Standard IO methods ----------
	fnpPosIO_SP_SeekRead               SeekRead;
	fnpPosIO_SP_SeekWrite              SeekWrite;
	fnpPosIO_SP_GetSize                GetSize;
	fnpPosIO_SP_SetSize                SetSize;
	fnpPosIO_SP_Flush                  Flush;

// ----------- Additional PosIO methods ----------
	fnpPosIO_SP_Seek                   Seek;
	fnpPosIO_SP_Read                   Read;
	fnpPosIO_SP_Write                  Write;

// -----------  Additional PosIO_PS methods ----------
	fnpPosIO_SP_GetStoredProperty      GetStoredProperty;
	fnpPosIO_SP_SetStoredProperty      SetStoredProperty;
	fnpPosIO_SP_DeleteStoredProperty   DeleteStoredProperty;
	fnpPosIO_SP_GetStoredPropertyName  GetStoredPropertyName;
}; // "PosIO_SP" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( PosIO_SP_PRIVATE_DEFINITION )
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
	#define CALL_PosIO_SP_SeekRead( _this, result, offset, buffer, size )                       ((_this)->vtbl->SeekRead( (_this), result, offset, buffer, size ))
	#define CALL_PosIO_SP_SeekWrite( _this, result, offset, buffer, size )                      ((_this)->vtbl->SeekWrite( (_this), result, offset, buffer, size ))
	#define CALL_PosIO_SP_GetSize( _this, result, type )                                        ((_this)->vtbl->GetSize( (_this), result, type ))
	#define CALL_PosIO_SP_SetSize( _this, new_size )                                            ((_this)->vtbl->SetSize( (_this), new_size ))
	#define CALL_PosIO_SP_Flush( _this )                                                        ((_this)->vtbl->Flush( (_this) ))
	#define CALL_PosIO_SP_Seek( _this, result, offset, origin )                                 ((_this)->vtbl->Seek( (_this), result, offset, origin ))
	#define CALL_PosIO_SP_Read( _this, result, buffer, count )                                  ((_this)->vtbl->Read( (_this), result, buffer, count ))
	#define CALL_PosIO_SP_Write( _this, result, buffer, count )                                 ((_this)->vtbl->Write( (_this), result, buffer, count ))
	#define CALL_PosIO_SP_GetStoredProperty( _this, result, sPropName, pBuffer, dwSize )        ((_this)->vtbl->GetStoredProperty( (_this), result, sPropName, pBuffer, dwSize ))
	#define CALL_PosIO_SP_SetStoredProperty( _this, result, sPropName, pBuffer, dwSize )        ((_this)->vtbl->SetStoredProperty( (_this), result, sPropName, pBuffer, dwSize ))
	#define CALL_PosIO_SP_DeleteStoredProperty( _this, sPropName )                              ((_this)->vtbl->DeleteStoredProperty( (_this), sPropName ))
	#define CALL_PosIO_SP_GetStoredPropertyName( _this, result, dwIndex, pBuffer, dwSize )      ((_this)->vtbl->GetStoredPropertyName( (_this), result, dwIndex, pBuffer, dwSize ))
#else // if !defined( __cplusplus )
	#define CALL_PosIO_SP_SeekRead( _this, result, offset, buffer, size )                       ((_this)->SeekRead( result, offset, buffer, size ))
	#define CALL_PosIO_SP_SeekWrite( _this, result, offset, buffer, size )                      ((_this)->SeekWrite( result, offset, buffer, size ))
	#define CALL_PosIO_SP_GetSize( _this, result, type )                                        ((_this)->GetSize( result, type ))
	#define CALL_PosIO_SP_SetSize( _this, new_size )                                            ((_this)->SetSize( new_size ))
	#define CALL_PosIO_SP_Flush( _this )                                                        ((_this)->Flush( ))
	#define CALL_PosIO_SP_Seek( _this, result, offset, origin )                                 ((_this)->Seek( result, offset, origin ))
	#define CALL_PosIO_SP_Read( _this, result, buffer, count )                                  ((_this)->Read( result, buffer, count ))
	#define CALL_PosIO_SP_Write( _this, result, buffer, count )                                 ((_this)->Write( result, buffer, count ))
	#define CALL_PosIO_SP_GetStoredProperty( _this, result, sPropName, pBuffer, dwSize )        ((_this)->GetStoredProperty( result, sPropName, pBuffer, dwSize ))
	#define CALL_PosIO_SP_SetStoredProperty( _this, result, sPropName, pBuffer, dwSize )        ((_this)->SetStoredProperty( result, sPropName, pBuffer, dwSize ))
	#define CALL_PosIO_SP_DeleteStoredProperty( _this, sPropName )                              ((_this)->DeleteStoredProperty( sPropName ))
	#define CALL_PosIO_SP_GetStoredPropertyName( _this, result, dwIndex, pBuffer, dwSize )      ((_this)->GetStoredPropertyName( result, dwIndex, pBuffer, dwSize ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iPosIO_SP {
		virtual tERROR pr_call SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- reads content of the object to the buffer
		virtual tERROR pr_call SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- writes buffer to the object
		virtual tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE type ) = 0; // -- returns size of the requested type of the object
		virtual tERROR pr_call SetSize( tQWORD new_size ) = 0; // -- resizes the object
		virtual tERROR pr_call Flush() = 0; // -- flush internal buffers
		virtual tERROR pr_call Seek( tQWORD* result, tLONGLONG offset, tDWORD origin ) = 0; // -- Переставляет текущую позицию в IO
		virtual tERROR pr_call Read( tDWORD* result, tPTR buffer, tDWORD count ) = 0; // -- Читает указанное кол-во байт с текущей позиции
		virtual tERROR pr_call Write( tDWORD* result, tPTR buffer, tDWORD count ) = 0; // -- Записывает указанное кол-во байт с текущей позиции
		virtual tERROR pr_call GetStoredProperty( tDWORD* result, const tSTRING sPropName, tPTR pBuffer, tDWORD dwSize ) = 0; // -- Получить сохраняемое свойство
		virtual tERROR pr_call SetStoredProperty( tDWORD* result, const tSTRING sPropName, const tPTR pBuffer, tDWORD dwSize ) = 0; // -- Установить сохраняемое свойство
		virtual tERROR pr_call DeleteStoredProperty( const tSTRING sPropName ) = 0; // -- Удалить сохраняемое свойство
		virtual tERROR pr_call GetStoredPropertyName( tDWORD* result, tDWORD dwIndex, tSTRING pBuffer, tDWORD dwSize ) = 0; // -- Получает имя свойства
	};

	struct pr_abstract cPosIO_SP : public iPosIO_SP, public iObj {
		OBJ_IMPL( cPosIO_SP );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hPOSIO_SP()   { return (hPOSIO_SP)this; }

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

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_posio_sp__b666c9da_7121_46a1_b07a_a7c26f83d9f3
// AVP Prague stamp end



