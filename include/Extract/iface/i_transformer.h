// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  16 March 2005,  18:03 --------
// File Name   -- (null)i_transformer.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_transformer__247282f3_e224_4fd4_ae2d_8282e2428f79 )
#define __i_transformer__247282f3_e224_4fd4_ae2d_8282e2428f79
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end


#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TRANSFORMER  ((tIID)(49001))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD (pr_call * fTRANSFORMER_CALLBACK)(tPTR User, tBYTE* Buff, tDWORD Size); // Transformer callback
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
  typedef tUINT hTRANSFORMER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iTransformerVtbl;
typedef struct iTransformerVtbl iTransformerVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
  struct cTransformer;
  typedef cTransformer* hTRANSFORMER;
#else
  typedef struct tag_hTRANSFORMER 
  {
    const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
    const iSYSTEMVtbl*      sys;  // pointer to the "SYSTEM" virtual table
  } *hTRANSFORMER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Transformer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


  typedef   tERROR (pr_call *fnpTransformer_ProcessChunck)  ( hTRANSFORMER _this, tDWORD* result, tDWORD dwSize ); // -- Преобразовывает часть данных входного потока;
  typedef   tERROR (pr_call *fnpTransformer_Proccess)       ( hTRANSFORMER _this, tQWORD* result );              // -- Преобразовывает весь входной поток;
  typedef   tERROR (pr_call *fnpTransformer_ProcessQChunck) ( hTRANSFORMER _this, tQWORD* result, tQWORD qwSize ); // -- Same as ProcessChunck() but for tQWORD size;
  typedef   tERROR (pr_call *fnpTransformer_Decode)         ( hTRANSFORMER _this, tQWORD* result );              // -- Decode until stream eof or internal io limit;
  typedef   tERROR (pr_call *fnpTransformer_Reset)          ( hTRANSFORMER _this, tINT Method ); // -- Reset internal transformer state;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iTransformerVtbl 
{
  fnpTransformer_ProcessChunck   ProcessChunck;
  fnpTransformer_Proccess        Proccess;
  fnpTransformer_ProcessQChunck  ProcessQChunck;
  fnpTransformer_Decode          Decode;
  fnpTransformer_Reset           Reset;
}; // "Transformer" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Transformer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION     mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT     mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTRANSFORM_SIZE        mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00001002 )
#define pgINPUT_SEQ_IO          mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001003 )
#define pgOUTPUT_SEQ_IO         mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001004 )
#define pgINPUT_BUFF            mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001005 )
#define pgINPUT_BUFF_SIZE       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001006 )
#define pgOUTPUT_BUFF           mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001007 )
#define pgOUTPUT_BUFF_SIZE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001008 )
#define pgTRANSFORM_WINDOW      mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001009 )
#define pgTRANSFORM_WINDOW_SIZE mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000100a )
#define pgINPUT_CALLBACK        mPROPERTY_MAKE_GLOBAL( pTYPE_FUNC_PTR, 0x0000100b )
#define pgOUTPUT_CALLBACK       mPROPERTY_MAKE_GLOBAL( pTYPE_FUNC_PTR, 0x0000100c )
#define pgCALLBACK_USER         mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x0000100d )
#define pgTRANSFORM_INPUT_SIZE  mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x0000100e )
#define pgFORCE_WINDOW_CREATION mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100f )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
  #define CALL_Transformer_ProcessChunck( _this, result, dwSize )          ((_this)->vtbl->ProcessChunck( (_this), result, dwSize ))
  #define CALL_Transformer_Proccess( _this, result )                       ((_this)->vtbl->Proccess( (_this), result ))
  #define CALL_Transformer_ProcessQChunck( _this, result, qwSize )         ((_this)->vtbl->ProcessQChunck( (_this), result, qwSize ))
  #define CALL_Transformer_Decode( _this, result )                         ((_this)->vtbl->Decode( (_this), result ))
  #define CALL_Transformer_Reset( _this, Method )                          ((_this)->vtbl->Reset( (_this), Method ))
#else // if !defined( __cplusplus )
  #define CALL_Transformer_ProcessChunck( _this, result, dwSize )          ((_this)->ProcessChunck( result, dwSize ))
  #define CALL_Transformer_Proccess( _this, result )                       ((_this)->Proccess( result ))
  #define CALL_Transformer_ProcessQChunck( _this, result, qwSize )         ((_this)->ProcessQChunck( result, qwSize ))
  #define CALL_Transformer_Decode( _this, result )                         ((_this)->Decode( result ))
  #define CALL_Transformer_Reset( _this, Method )                          ((_this)->Reset( Method ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
  struct pr_abstract iTransformer 
  {
    virtual tERROR pr_call ProcessChunck( tDWORD* result, tDWORD dwSize ) = 0; // -- Преобразовывает часть данных входного потока
    virtual tERROR pr_call Proccess( tQWORD* result ) = 0; // -- Преобразовывает весь входной поток
    virtual tERROR pr_call ProcessQChunck( tQWORD* result, tQWORD qwSize ) = 0; // -- Same as ProcessChunck() but for tQWORD size
    virtual tERROR pr_call Decode( tQWORD* result ) = 0; // -- Decode until stream eof or internal io limit
    virtual tERROR pr_call Reset( tINT Method ) = 0; // -- Reset internal transformer state
  };

  struct pr_abstract cTransformer : public iTransformer, public iObj {

		OBJ_IMPL( cTransformer );

    operator hOBJECT() { return (hOBJECT)this; }
    operator hTRANSFORMER()   { return (hTRANSFORMER)this; }

    tQWORD pr_call get_pgTRANSFORM_SIZE() { return (tQWORD)getQWord(pgTRANSFORM_SIZE); }
    tERROR pr_call set_pgTRANSFORM_SIZE( tQWORD value ) { return setQWord(pgTRANSFORM_SIZE,(tQWORD)value); }

    hOBJECT pr_call get_pgINPUT_SEQ_IO() { return (hOBJECT)getObj(pgINPUT_SEQ_IO); }
    tERROR pr_call set_pgINPUT_SEQ_IO( hOBJECT value ) { return setObj(pgINPUT_SEQ_IO,(hOBJECT)value); }

    hOBJECT pr_call get_pgOUTPUT_SEQ_IO() { return (hOBJECT)getObj(pgOUTPUT_SEQ_IO); }
    tERROR pr_call set_pgOUTPUT_SEQ_IO( hOBJECT value ) { return setObj(pgOUTPUT_SEQ_IO,(hOBJECT)value); }

    tPTR pr_call get_pgINPUT_BUFF() { return (tPTR)getPtr(pgINPUT_BUFF); }
    tERROR pr_call set_pgINPUT_BUFF( tPTR value ) { return setPtr(pgINPUT_BUFF,(tPTR)value); }

    tDWORD pr_call get_pgINPUT_BUFF_SIZE() { return (tDWORD)getDWord(pgINPUT_BUFF_SIZE); }
    tERROR pr_call set_pgINPUT_BUFF_SIZE( tDWORD value ) { return setDWord(pgINPUT_BUFF_SIZE,(tDWORD)value); }

    tPTR pr_call get_pgOUTPUT_BUFF() { return (tPTR)getPtr(pgOUTPUT_BUFF); }
    tERROR pr_call set_pgOUTPUT_BUFF( tPTR value ) { return setPtr(pgOUTPUT_BUFF,(tPTR)value); }

    tDWORD pr_call get_pgOUTPUT_BUFF_SIZE() { return (tDWORD)getDWord(pgOUTPUT_BUFF_SIZE); }
    tERROR pr_call set_pgOUTPUT_BUFF_SIZE( tDWORD value ) { return setDWord(pgOUTPUT_BUFF_SIZE,(tDWORD)value); }

    tPTR pr_call get_pgTRANSFORM_WINDOW() { return (tPTR)getPtr(pgTRANSFORM_WINDOW); }
    tERROR pr_call set_pgTRANSFORM_WINDOW( tPTR value ) { return setPtr(pgTRANSFORM_WINDOW,(tPTR)value); }

    tDWORD pr_call get_pgTRANSFORM_WINDOW_SIZE() { return (tDWORD)getDWord(pgTRANSFORM_WINDOW_SIZE); }
    tERROR pr_call set_pgTRANSFORM_WINDOW_SIZE( tDWORD value ) { return setDWord(pgTRANSFORM_WINDOW_SIZE,(tDWORD)value); }

    fTRANSFORMER_CALLBACK pr_call get_pgINPUT_CALLBACK() { fTRANSFORMER_CALLBACK value = {0}; get(pgINPUT_CALLBACK,&value,sizeof(value)); return value; }
    tERROR pr_call set_pgINPUT_CALLBACK( fTRANSFORMER_CALLBACK value ) { return set(pgINPUT_CALLBACK,&value,sizeof(value)); }

    fTRANSFORMER_CALLBACK pr_call get_pgOUTPUT_CALLBACK() { fTRANSFORMER_CALLBACK value = {0}; get(pgOUTPUT_CALLBACK,&value,sizeof(value)); return value; }
    tERROR pr_call set_pgOUTPUT_CALLBACK( fTRANSFORMER_CALLBACK value ) { return set(pgOUTPUT_CALLBACK,&value,sizeof(value)); }

    tPTR pr_call get_pgCALLBACK_USER() { return (tPTR)getPtr(pgCALLBACK_USER); }
    tERROR pr_call set_pgCALLBACK_USER( tPTR value ) { return setPtr(pgCALLBACK_USER,(tPTR)value); }

    tQWORD pr_call get_pgTRANSFORM_INPUT_SIZE() { return (tQWORD)getQWord(pgTRANSFORM_INPUT_SIZE); }
    tERROR pr_call set_pgTRANSFORM_INPUT_SIZE( tQWORD value ) { return setQWord(pgTRANSFORM_INPUT_SIZE,(tQWORD)value); }

    tBOOL pr_call get_pgFORCE_WINDOW_CREATION() { return (tBOOL)getBool(pgFORCE_WINDOW_CREATION); }
    tERROR pr_call set_pgFORCE_WINDOW_CREATION( tBOOL value ) { return setBool(pgFORCE_WINDOW_CREATION,(tBOOL)value); }

  };

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_transformer__247282f3_e224_4fd4_ae2d_8282e2428f79
// AVP Prague stamp end



