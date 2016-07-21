// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:22 --------
// File Name   -- (null)i_buffer.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_buffer__73203ea7_fb3c_43a0_8500_1b7cdbd28849 )
#define __i_buffer__73203ea7_fb3c_43a0_8500_1b7cdbd28849
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_tracer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Buffer interface implementation
// Short comments -- интерфейс доступа к региону памяти//eng:"piece of memory" interface
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_BUFFER  ((tIID)(1))
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
	typedef tUINT hBUFFER;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iBufferVtbl;
typedef struct iBufferVtbl iBufferVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cBuffer;
	typedef cBuffer* hBUFFER;
#else
	typedef struct tag_hBUFFER {
		const iBufferVtbl* vtbl; // pointer to the "Buffer" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hBUFFER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Buffer_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )



	// -----------  IO declared methods ----------
	typedef   tERROR (pr_call *fnpBuffer_SeekRead)  ( hBUFFER _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- //rus:вычитывает содержимое объекта в буфер памяти//eng:reads content of the object to the buffer;
	typedef   tERROR (pr_call *fnpBuffer_SeekWrite) ( hBUFFER _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- //rus:копирует буфер в регион памяти//eng:writes buffer to the object;
	typedef   tERROR (pr_call *fnpBuffer_GetSize)   ( hBUFFER _this, tQWORD* result, IO_SIZE_TYPE type );     // -- размер распределенного региона памяти//eng:returns size of the requested type of the object;
	typedef   tERROR (pr_call *fnpBuffer_SetSize)   ( hBUFFER _this, tQWORD new_size );       // -- //rus:перераспределяет регион с новым размером//eng:resizes the object;
	typedef   tERROR (pr_call *fnpBuffer_Flush)     ( hBUFFER _this );                        // -- в данном интерфейсе не должен ничего делать//eng:there is nothing to do here;

	// -----------  buffer declared methods ----------
	typedef   tERROR (pr_call *fnpBuffer_Lock)      ( hBUFFER _this, tPTR* result );                        // -- //rus:фиксирует регион памяти и возвращает указатель на него//eng:lock memory and get access to it;
	typedef   tERROR (pr_call *fnpBuffer_AddLock)   ( hBUFFER _this, tPTR* result, tDWORD size_to_add );    // -- //rus:добавляет указанный размер к региону и возвращает указатель на добавленную память//eng:add some space to the object and returns pointer to the added space;
	typedef   tERROR (pr_call *fnpBuffer_SizeLock)  ( hBUFFER _this, tPTR* result, tDWORD size );           // -- //rus:перераспределяет регион с новым размером, фиксирует его и вохвращает указатель на содержимое//eng:set size of the buffer, lock it and returns pointer to the content;
	typedef   tERROR (pr_call *fnpBuffer_Unlock)    ( hBUFFER _this );                        // -- //rus:разблокирует регион//eng:unlock memory;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iBufferVtbl {

// -----------  IO declared methods ----------
	fnpBuffer_SeekRead   SeekRead;
	fnpBuffer_SeekWrite  SeekWrite;
	fnpBuffer_GetSize    GetSize;
	fnpBuffer_SetSize    SetSize;
	fnpBuffer_Flush      Flush;

// -----------  buffer declared methods ----------
	fnpBuffer_Lock       Lock;
	fnpBuffer_AddLock    AddLock;
	fnpBuffer_SizeLock   SizeLock;
	fnpBuffer_Unlock     Unlock;
}; // "Buffer" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Buffer_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_PATH             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000044 )
//! #define pgOBJECT_FULL_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
//! #define pgOBJECT_OPEN_MODE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004a )
//! #define pgOBJECT_ACCESS_MODE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004e )
#define pgOBJECT_DELETE_ON_CLOSE  mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
//! #define pgOBJECT_AVAILABILITY     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
//! #define pgOBJECT_BASED_ON         mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000053 )
#define pgOBJECT_FILL_CHAR        mPROPERTY_MAKE_GLOBAL( pTYPE_BYTE    , 0x00001004 )
//! #define pgINTERFACE_COMPATIBILITY mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000087 )
//! #define pgOBJECT_ORIGIN           mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
#define pgLOCK_COUNT              mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Buffer_SeekRead( _this, result, offset, buffer, size )     ((_this)->vtbl->SeekRead( (_this), result, offset, buffer, size ))
	#define CALL_Buffer_SeekWrite( _this, result, offset, buffer, size )    ((_this)->vtbl->SeekWrite( (_this), result, offset, buffer, size ))
	#define CALL_Buffer_GetSize( _this, result, type )                      ((_this)->vtbl->GetSize( (_this), result, type ))
	#define CALL_Buffer_SetSize( _this, new_size )                          ((_this)->vtbl->SetSize( (_this), new_size ))
	#define CALL_Buffer_Flush( _this )                                      ((_this)->vtbl->Flush( (_this) ))
	#define CALL_Buffer_Lock( _this, result )                               ((_this)->vtbl->Lock( (_this), result ))
	#define CALL_Buffer_AddLock( _this, result, size_to_add )               ((_this)->vtbl->AddLock( (_this), result, size_to_add ))
	#define CALL_Buffer_SizeLock( _this, result, size )                     ((_this)->vtbl->SizeLock( (_this), result, size ))
	#define CALL_Buffer_Unlock( _this )                                     ((_this)->vtbl->Unlock( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_Buffer_SeekRead( _this, result, offset, buffer, size )     ((_this)->SeekRead( result, offset, buffer, size ))
	#define CALL_Buffer_SeekWrite( _this, result, offset, buffer, size )    ((_this)->SeekWrite( result, offset, buffer, size ))
	#define CALL_Buffer_GetSize( _this, result, type )                      ((_this)->GetSize( result, type ))
	#define CALL_Buffer_SetSize( _this, new_size )                          ((_this)->SetSize( new_size ))
	#define CALL_Buffer_Flush( _this )                                      ((_this)->Flush( ))
	#define CALL_Buffer_Lock( _this, result )                               ((_this)->Lock( result ))
	#define CALL_Buffer_AddLock( _this, result, size_to_add )               ((_this)->AddLock( result, size_to_add ))
	#define CALL_Buffer_SizeLock( _this, result, size )                     ((_this)->SizeLock( result, size ))
	#define CALL_Buffer_Unlock( _this )                                     ((_this)->Unlock( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iBuffer {
		virtual tERROR pr_call SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- //rus:вычитывает содержимое объекта в буфер памяти//eng:reads content of the object to the buffer
		virtual tERROR pr_call SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) = 0; // -- //rus:копирует буфер в регион памяти//eng:writes buffer to the object
		virtual tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE type ) = 0; // -- размер распределенного региона памяти//eng:returns size of the requested type of the object
		virtual tERROR pr_call SetSize( tQWORD new_size ) = 0; // -- //rus:перераспределяет регион с новым размером//eng:resizes the object
		virtual tERROR pr_call Flush() = 0; // -- в данном интерфейсе не должен ничего делать//eng:there is nothing to do here
		virtual tERROR pr_call Lock( tPTR* result ) = 0; // -- //rus:фиксирует регион памяти и возвращает указатель на него//eng:lock memory and get access to it
		virtual tERROR pr_call AddLock( tPTR* result, tDWORD size_to_add ) = 0; // -- //rus:добавляет указанный размер к региону и возвращает указатель на добавленную память//eng:add some space to the object and returns pointer to the added space
		virtual tERROR pr_call SizeLock( tPTR* result, tDWORD size ) = 0; // -- //rus:перераспределяет регион с новым размером, фиксирует его и вохвращает указатель на содержимое//eng:set size of the buffer, lock it and returns pointer to the content
		virtual tERROR pr_call Unlock() = 0; // -- //rus:разблокирует регион//eng:unlock memory
	};

	struct pr_abstract cBuffer : public iBuffer, public iObj {

		OBJ_IMPL( cBuffer );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hBUFFER()   { return (hBUFFER)this; }

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

		tIID pr_call get_pgINTERFACE_COMPATIBILITY() { return (tIID)getDWord(pgINTERFACE_COMPATIBILITY); }
		tERROR pr_call set_pgINTERFACE_COMPATIBILITY( tIID value ) { return setDWord(pgINTERFACE_COMPATIBILITY,(tDWORD)value); }

		tORIG_ID pr_call get_pgOBJECT_ORIGIN() { return (tORIG_ID)getDWord(pgOBJECT_ORIGIN); }
		tERROR pr_call set_pgOBJECT_ORIGIN( tORIG_ID value ) { return setDWord(pgOBJECT_ORIGIN,(tDWORD)value); }

		tDWORD pr_call get_pgLOCK_COUNT() { return (tDWORD)getDWord(pgLOCK_COUNT); }
		tERROR pr_call set_pgLOCK_COUNT( tDWORD value ) { return setDWord(pgLOCK_COUNT,(tDWORD)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_buffer__ae5346ba_8a1e_4aa9_b529_abdf874dace0
// AVP Prague stamp end

