// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  17 August 2007,  20:05 --------
// File Name   -- (null)i_transport.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_transport__7cf72612_026b_4d93_94d6_e718d1581240 )
#define __i_transport__7cf72612_026b_4d93_94d6_e718d1581240
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end
// AVP Prague stamp begin( Interface comment,  )
// Transport interface implementation
// AVP Prague stamp end
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TRANSPORT  ((tIID)(24004))
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
    typedef tUINT hTRANSPORT;
#else
// AVP Prague stamp end
// AVP Prague stamp begin( Interface forward declaration,  )
struct iTransportVtbl;
typedef struct iTransportVtbl iTransportVtbl;
// AVP Prague stamp end
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
    struct cTransport;
    typedef cTransport* hTRANSPORT;
#else
    typedef struct tag_hTRANSPORT 
    {
        const iTransportVtbl* vtbl; // pointer to the "Transport" virtual table
        const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
    } *hTRANSPORT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Transport_PRIVATE_DEFINITION )
// AVP Prague stamp end
// AVP Prague stamp begin( Interface function forward declarations,  )


    typedef   tERROR (pr_call *fnpTransport_httpGetFile)      ( hTRANSPORT _this, hSTRING url, hIO destinationIO ); // -- ;
    typedef   tERROR (pr_call *fnpTransport_httpCloseSession) ( hTRANSPORT _this );              // -- ;
    typedef   tERROR (pr_call *fnpTransport_httpPost)         ( hTRANSPORT _this, hSTRING url, hSTRING request, tVOID* postData, tDWORD postDataSize ); // -- ;


// AVP Prague stamp end
// AVP Prague stamp begin( Interface declaration,  )
struct iTransportVtbl 
{
    fnpTransport_httpGetFile       httpGetFile;
    fnpTransport_httpCloseSession  httpCloseSession;
    fnpTransport_httpPost          httpPost;
}; // "Transport" external virtual table prototype
// AVP Prague stamp end
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Transport_PRIVATE_DEFINITION )
// AVP Prague stamp end
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION                 mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT                 mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTRANSPORT_TM                      mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001000 )
#define pgPROXY_DETECTOR                    mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001001 )
#define pgTRANSPORT_USER_AGENT              mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001002 )
#define pgTRANSPORT_TASK                    mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001003 )
#define pgTRANSPORT_NETWORK_TIMEOUT_SECONDS mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgTRANSPORT_HTTP_HEADER             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001005 )
// AVP Prague stamp end
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
    #define CALL_Transport_httpGetFile( _this, url, destinationIO )                              ((_this)->vtbl->httpGetFile( (_this), url, destinationIO ))
    #define CALL_Transport_httpCloseSession( _this )                                             ((_this)->vtbl->httpCloseSession( (_this) ))
    #define CALL_Transport_httpPost( _this, url, request, postData, postDataSize )               ((_this)->vtbl->httpPost( (_this), url, request, postData, postDataSize ))
#else // if !defined( __cplusplus )
    #define CALL_Transport_httpGetFile( _this, url, destinationIO )                              ((_this)->httpGetFile( url, destinationIO ))
    #define CALL_Transport_httpCloseSession( _this )                                             ((_this)->httpCloseSession( ))
    #define CALL_Transport_httpPost( _this, url, request, postData, postDataSize )               ((_this)->httpPost( url, request, postData, postDataSize ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
    struct pr_abstract iTransport 
    {
        virtual tERROR pr_call httpGetFile( hSTRING url, hIO destinationIO ) = 0;
        virtual tERROR pr_call httpCloseSession() = 0;
        virtual tERROR pr_call httpPost( hSTRING url, hSTRING request, tVOID* postData, tDWORD postDataSize ) = 0;
    };

    struct pr_abstract cTransport : public iTransport, public iObj 
    {

        OBJ_IMPL( cTransport );

        operator hOBJECT() { return (hOBJECT)this; }
        operator hTRANSPORT()   { return (hTRANSPORT)this; }

        hOBJECT pr_call get_pgTRANSPORT_TM() { return (hOBJECT)getObj(pgTRANSPORT_TM); }
        tERROR pr_call set_pgTRANSPORT_TM( hOBJECT value ) { return setObj(pgTRANSPORT_TM,(hOBJECT)value); }

        hOBJECT pr_call get_pgPROXY_DETECTOR() { return (hOBJECT)getObj(pgPROXY_DETECTOR); }
        tERROR pr_call set_pgPROXY_DETECTOR( hOBJECT value ) { return setObj(pgPROXY_DETECTOR,(hOBJECT)value); }

        tERROR pr_call get_pgTRANSPORT_USER_AGENT( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgTRANSPORT_USER_AGENT,buff,size,cp); }
        tERROR pr_call set_pgTRANSPORT_USER_AGENT( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgTRANSPORT_USER_AGENT,buff,size,cp); }

        hOBJECT pr_call get_pgTRANSPORT_TASK() { return (hOBJECT)getObj(pgTRANSPORT_TASK); }
        tERROR pr_call set_pgTRANSPORT_TASK( hOBJECT value ) { return setObj(pgTRANSPORT_TASK,(hOBJECT)value); }

        tDWORD pr_call get_pgTRANSPORT_NETWORK_TIMEOUT_SECONDS() { return (tDWORD)getDWord(pgTRANSPORT_NETWORK_TIMEOUT_SECONDS); }
        tERROR pr_call set_pgTRANSPORT_NETWORK_TIMEOUT_SECONDS( tDWORD value ) { return setDWord(pgTRANSPORT_NETWORK_TIMEOUT_SECONDS,(tDWORD)value); }

        tERROR pr_call get_pgTRANSPORT_HTTP_HEADER( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgTRANSPORT_HTTP_HEADER,buff,size,cp); }
        tERROR pr_call set_pgTRANSPORT_HTTP_HEADER( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgTRANSPORT_HTTP_HEADER,buff,size,cp); }

    };

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end
// AVP Prague stamp begin( Header endif,  )
#endif // __i_transport__7cf72612_026b_4d93_94d6_e718d1581240
// AVP Prague stamp end
