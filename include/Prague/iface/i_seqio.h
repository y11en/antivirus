// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:21 --------
// File Name   -- (null)i_seqio.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_seqio__7c4a8f14_ddd5_4d5e_b09c_24fd95b99885 )
#define __i_seqio__7c4a8f14_ddd5_4d5e_b09c_24fd95b99885
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end


// AVP Prague stamp begin( Interface comment,  )
// SeqIO interface implementation
// Short comments -- Sequential input/output interface
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_SEQIO  ((tIID)(64000))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )

// Seek constants
#define cSEEK_SET                      ((tDWORD)(0)) //  --
#define cSEEK_CUR                      ((tDWORD)(1)) //  --
#define cSEEK_END                      ((tDWORD)(2)) //  --
#define cSEEK_FORWARD                  ((tDWORD)(1)) //  --
#define cSEEK_BACK                     ((tDWORD)(3)) //  --

// Direction constants
#define cORDER_DIRECT                  ((tINT)(1)) //  --
#define cORDER_BACK                    ((tINT)(-1)) //  --
#define cORDER_REVERSE                 ((tINT)(0)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSEQ_IO;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iSeqIOVtbl;
typedef struct iSeqIOVtbl iSeqIOVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cSeqIO;
	typedef cSeqIO* hSEQ_IO;
#else
	struct tag_hSEQ_IO {
		const iSeqIOVtbl*  vtbl; // pointer to the "SeqIO" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	};
	#if !defined( SEQIO_DEFINED )
		#define SEQIO_DEFINED
		typedef struct tag_hSEQ_IO *hSEQ_IO;
	#endif // if defined( __cplusplus )
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( SeqIO_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )



	// ----------- Standard IO methods ----------
	typedef   tERROR (pr_call *fnpSeqIO_SeekRead)     ( hSEQ_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ); // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_SeekWrite)    ( hSEQ_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ); // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_GetSize)      ( hSEQ_IO _this, tQWORD* result, IO_SIZE_TYPE type );   // -- Return size of file;
	typedef   tERROR (pr_call *fnpSeqIO_SetSize)      ( hSEQ_IO _this, tQWORD new_size );     // -- Operation not possible while using region or stream reversing;
	typedef   tERROR (pr_call *fnpSeqIO_Flush)        ( hSEQ_IO _this );                      // -- flush internal buffers;

	// ----------- Additional Sequential IO methods - read ----------
	typedef   tERROR (pr_call *fnpSeqIO_Read)         ( hSEQ_IO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_ReadByte)     ( hSEQ_IO _this, tBYTE* result );                      // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_ReadWord)     ( hSEQ_IO _this, tWORD* result );                      // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_ReadDword)    ( hSEQ_IO _this, tDWORD* result );                      // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_ReadQword)    ( hSEQ_IO _this, tQWORD* result );                      // -- ;

	// ----------- Additional Sequential IO methods - write ----------
	typedef   tERROR (pr_call *fnpSeqIO_Write)        ( hSEQ_IO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_WriteByte)    ( hSEQ_IO _this, tBYTE value );         // -- Writes byte to stream;
	typedef   tERROR (pr_call *fnpSeqIO_WriteWord)    ( hSEQ_IO _this, tWORD value );         // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_WriteDword)   ( hSEQ_IO _this, tDWORD value );        // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_WriteQword)   ( hSEQ_IO _this, tQWORD value );        // -- ;

	// ----------- Additional Sequential IO methods - set file position ----------
	typedef   tERROR (pr_call *fnpSeqIO_Seek)         ( hSEQ_IO _this, tQWORD* result, tQWORD offset, tDWORD origin ); // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_SeekSet)      ( hSEQ_IO _this, tQWORD* result, tQWORD offset );       // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_SeekForward)  ( hSEQ_IO _this, tQWORD* result, tQWORD offset );       // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_SeekBack)     ( hSEQ_IO _this, tQWORD* result, tQWORD offset );       // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_SeekEnd)      ( hSEQ_IO _this, tQWORD* result, tQWORD offset );       // -- ;
	typedef   tERROR (pr_call *fnpSeqIO_SetSizeByPos) ( hSEQ_IO _this, tQWORD* result );                      // -- Set file size according current position;
	typedef   tERROR (pr_call *fnpSeqIO_IsNotEOF)     ( hSEQ_IO _this );                      // -- ;

	// ----------- Additional Sequential IO methods - set file region ----------
	typedef   tERROR (pr_call *fnpSeqIO_RegionSet)    ( hSEQ_IO _this, tQWORD start, tQWORD end ); // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iSeqIOVtbl {

// ----------- Standard IO methods ----------
	fnpSeqIO_SeekRead      SeekRead;
	fnpSeqIO_SeekWrite     SeekWrite;
	fnpSeqIO_GetSize       GetSize;
	fnpSeqIO_SetSize       SetSize;
	fnpSeqIO_Flush         Flush;

// ----------- Additional Sequential IO methods - read ----------
	fnpSeqIO_Read          Read;
	fnpSeqIO_ReadByte      ReadByte;
	fnpSeqIO_ReadWord      ReadWord;
	fnpSeqIO_ReadDword     ReadDword;
	fnpSeqIO_ReadQword     ReadQword;

// ----------- Additional Sequential IO methods - write ----------
	fnpSeqIO_Write         Write;
	fnpSeqIO_WriteByte     WriteByte;
	fnpSeqIO_WriteWord     WriteWord;
	fnpSeqIO_WriteDword    WriteDword;
	fnpSeqIO_WriteQword    WriteQword;

// ----------- Additional Sequential IO methods - set file position ----------
	fnpSeqIO_Seek          Seek;
	fnpSeqIO_SeekSet       SeekSet;
	fnpSeqIO_SeekForward   SeekForward;
	fnpSeqIO_SeekBack      SeekBack;
	fnpSeqIO_SeekEnd       SeekEnd;
	fnpSeqIO_SetSizeByPos  SetSizeByPos;
	fnpSeqIO_IsNotEOF      IsNotEOF;

// ----------- Additional Sequential IO methods - set file region ----------
	fnpSeqIO_RegionSet     RegionSet;
}; // "SeqIO" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( SeqIO_PRIVATE_DEFINITION )
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
#define pgBYTE_ORDER              mPROPERTY_MAKE_GLOBAL( pTYPE_SBYTE   , 0x00002001 )
#define pgBUFFER_ORDER            mPROPERTY_MAKE_GLOBAL( pTYPE_SBYTE   , 0x00002002 )
#define pgBUFFER_SIZE             mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00002003 )
//! #define pgINTERFACE_COMPATIBILITY mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000087 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_SeqIO_SeekRead( _this, result, offset, buffer, count )    ((_this)->vtbl->SeekRead( (_this), result, offset, buffer, count ))
	#define CALL_SeqIO_SeekWrite( _this, result, offset, buffer, count )   ((_this)->vtbl->SeekWrite( (_this), result, offset, buffer, count ))
	#define CALL_SeqIO_GetSize( _this, result, type )                      ((_this)->vtbl->GetSize( (_this), result, type ))
	#define CALL_SeqIO_SetSize( _this, new_size )                          ((_this)->vtbl->SetSize( (_this), new_size ))
	#define CALL_SeqIO_Flush( _this )                                      ((_this)->vtbl->Flush( (_this) ))
	#define CALL_SeqIO_Read( _this, result, buffer, count )                ((_this)->vtbl->Read( (_this), result, buffer, count ))
	#define CALL_SeqIO_ReadByte( _this, result )                           ((_this)->vtbl->ReadByte( (_this), result ))
	#define CALL_SeqIO_ReadWord( _this, result )                           ((_this)->vtbl->ReadWord( (_this), result ))
	#define CALL_SeqIO_ReadDword( _this, result )                          ((_this)->vtbl->ReadDword( (_this), result ))
	#define CALL_SeqIO_ReadQword( _this, result )                          ((_this)->vtbl->ReadQword( (_this), result ))
	#define CALL_SeqIO_Write( _this, result, buffer, count )               ((_this)->vtbl->Write( (_this), result, buffer, count ))
	#define CALL_SeqIO_WriteByte( _this, value )                           ((_this)->vtbl->WriteByte( (_this), value ))
	#define CALL_SeqIO_WriteWord( _this, value )                           ((_this)->vtbl->WriteWord( (_this), value ))
	#define CALL_SeqIO_WriteDword( _this, value )                          ((_this)->vtbl->WriteDword( (_this), value ))
	#define CALL_SeqIO_WriteQword( _this, value )                          ((_this)->vtbl->WriteQword( (_this), value ))
	#define CALL_SeqIO_Seek( _this, result, offset, origin )               ((_this)->vtbl->Seek( (_this), result, offset, origin ))
	#define CALL_SeqIO_SeekSet( _this, result, offset )                    ((_this)->vtbl->SeekSet( (_this), result, offset ))
	#define CALL_SeqIO_SeekForward( _this, result, offset )                ((_this)->vtbl->SeekForward( (_this), result, offset ))
	#define CALL_SeqIO_SeekBack( _this, result, offset )                   ((_this)->vtbl->SeekBack( (_this), result, offset ))
	#define CALL_SeqIO_SeekEnd( _this, result, offset )                    ((_this)->vtbl->SeekEnd( (_this), result, offset ))
	#define CALL_SeqIO_SetSizeByPos( _this, result )                       ((_this)->vtbl->SetSizeByPos( (_this), result ))
	#define CALL_SeqIO_IsNotEOF( _this )                                   ((_this)->vtbl->IsNotEOF( (_this) ))
	#define CALL_SeqIO_RegionSet( _this, start, end )                      ((_this)->vtbl->RegionSet( (_this), start, end ))
#else // if !defined( __cplusplus )
	#define CALL_SeqIO_SeekRead( _this, result, offset, buffer, count )    ((_this)->SeekRead( result, offset, buffer, count ))
	#define CALL_SeqIO_SeekWrite( _this, result, offset, buffer, count )   ((_this)->SeekWrite( result, offset, buffer, count ))
	#define CALL_SeqIO_GetSize( _this, result, type )                      ((_this)->GetSize( result, type ))
	#define CALL_SeqIO_SetSize( _this, new_size )                          ((_this)->SetSize( new_size ))
	#define CALL_SeqIO_Flush( _this )                                      ((_this)->Flush( ))
	#define CALL_SeqIO_Read( _this, result, buffer, count )                ((_this)->Read( result, buffer, count ))
	#define CALL_SeqIO_ReadByte( _this, result )                           ((_this)->ReadByte( result ))
	#define CALL_SeqIO_ReadWord( _this, result )                           ((_this)->ReadWord( result ))
	#define CALL_SeqIO_ReadDword( _this, result )                          ((_this)->ReadDword( result ))
	#define CALL_SeqIO_ReadQword( _this, result )                          ((_this)->ReadQword( result ))
	#define CALL_SeqIO_Write( _this, result, buffer, count )               ((_this)->Write( result, buffer, count ))
	#define CALL_SeqIO_WriteByte( _this, value )                           ((_this)->WriteByte( value ))
	#define CALL_SeqIO_WriteWord( _this, value )                           ((_this)->WriteWord( value ))
	#define CALL_SeqIO_WriteDword( _this, value )                          ((_this)->WriteDword( value ))
	#define CALL_SeqIO_WriteQword( _this, value )                          ((_this)->WriteQword( value ))
	#define CALL_SeqIO_Seek( _this, result, offset, origin )               ((_this)->Seek( result, offset, origin ))
	#define CALL_SeqIO_SeekSet( _this, result, offset )                    ((_this)->SeekSet( result, offset ))
	#define CALL_SeqIO_SeekForward( _this, result, offset )                ((_this)->SeekForward( result, offset ))
	#define CALL_SeqIO_SeekBack( _this, result, offset )                   ((_this)->SeekBack( result, offset ))
	#define CALL_SeqIO_SeekEnd( _this, result, offset )                    ((_this)->SeekEnd( result, offset ))
	#define CALL_SeqIO_SetSizeByPos( _this, result )                       ((_this)->SetSizeByPos( result ))
	#define CALL_SeqIO_IsNotEOF( _this )                                   ((_this)->IsNotEOF( ))
	#define CALL_SeqIO_RegionSet( _this, start, end )                      ((_this)->RegionSet( start, end ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iSeqIO {
		virtual tERROR pr_call SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ) = 0;
		virtual tERROR pr_call SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ) = 0;
		virtual tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE type ) = 0; // -- Return size of file
		virtual tERROR pr_call SetSize( tQWORD new_size ) = 0;
		virtual tERROR pr_call Flush() = 0; // -- flush internal buffers
		virtual tERROR pr_call Read( tDWORD* result, tPTR buffer, tDWORD count ) = 0;
		virtual tERROR pr_call ReadByte( tBYTE* result ) = 0;
		virtual tERROR pr_call ReadWord( tWORD* result ) = 0;
		virtual tERROR pr_call ReadDword( tDWORD* result ) = 0;
		virtual tERROR pr_call ReadQword( tQWORD* result ) = 0;
		virtual tERROR pr_call Write( tDWORD* result, tPTR buffer, tDWORD count ) = 0;
		virtual tERROR pr_call WriteByte( tBYTE value ) = 0; // -- Writes byte to stream
		virtual tERROR pr_call WriteWord( tWORD value ) = 0;
		virtual tERROR pr_call WriteDword( tDWORD value ) = 0;
		virtual tERROR pr_call WriteQword( tQWORD value ) = 0;
		virtual tERROR pr_call Seek( tQWORD* result, tQWORD offset, tDWORD origin ) = 0;
		virtual tERROR pr_call SeekSet( tQWORD* result, tQWORD offset ) = 0;
		virtual tERROR pr_call SeekForward( tQWORD* result, tQWORD offset ) = 0;
		virtual tERROR pr_call SeekBack( tQWORD* result, tQWORD offset ) = 0;
		virtual tERROR pr_call SeekEnd( tQWORD* result, tQWORD offset ) = 0;
		virtual tERROR pr_call SetSizeByPos( tQWORD* result ) = 0; // -- Set file size according current position
		virtual tERROR pr_call IsNotEOF() = 0;
		virtual tERROR pr_call RegionSet( tQWORD start, tQWORD end ) = 0;
	};

	struct pr_abstract cSeqIO : public iSeqIO, public iObj {

		OBJ_IMPL( cSeqIO );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSEQ_IO()   { return (hSEQ_IO)this; }

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

		tSBYTE pr_call get_pgBYTE_ORDER() { return (tSBYTE)getByte(pgBYTE_ORDER); }
		tERROR pr_call set_pgBYTE_ORDER( tSBYTE value ) { return setByte(pgBYTE_ORDER,(tBYTE)value); }

		tSBYTE pr_call get_pgBUFFER_ORDER() { return (tSBYTE)getByte(pgBUFFER_ORDER); }
		tERROR pr_call set_pgBUFFER_ORDER( tSBYTE value ) { return setByte(pgBUFFER_ORDER,(tBYTE)value); }

		tDWORD pr_call get_pgBUFFER_SIZE() { return (tDWORD)getDWord(pgBUFFER_SIZE); }
		tERROR pr_call set_pgBUFFER_SIZE( tDWORD value ) { return setDWord(pgBUFFER_SIZE,(tDWORD)value); }

		tIID pr_call get_pgINTERFACE_COMPATIBILITY() { return (tIID)getDWord(pgINTERFACE_COMPATIBILITY); }
		tERROR pr_call set_pgINTERFACE_COMPATIBILITY( tIID value ) { return setDWord(pgINTERFACE_COMPATIBILITY,(tDWORD)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_seqio__37b13f2e_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end

