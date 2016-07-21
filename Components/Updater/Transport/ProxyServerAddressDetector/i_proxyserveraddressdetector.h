// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  28 April 2007,  13:23 --------
// File Name   -- (null)i_proxyserveraddressdetector.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_proxyserveraddressdetector__2ef5bc0c_9d4d_455c_bd55_d39a744744ba )
#define __i_proxyserveraddressdetector__2ef5bc0c_9d4d_455c_bd55_d39a744744ba
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end
#include <Updater/transport.h>
// AVP Prague stamp begin( Interface comment,  )
// ProxyServerAddressDetector interface implementation
// AVP Prague stamp end
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_PROXYSERVERADDRESSDETECTOR  ((tIID)(24010))
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
    typedef tUINT hPROXYSERVERADDRESSDETECTOR;
#else
// AVP Prague stamp end
// AVP Prague stamp begin( Interface forward declaration,  )
struct iProxyServerAddressDetectorVtbl;
typedef struct iProxyServerAddressDetectorVtbl iProxyServerAddressDetectorVtbl;
// AVP Prague stamp end
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
    struct cProxyServerAddressDetector;
    typedef cProxyServerAddressDetector* hPROXYSERVERADDRESSDETECTOR;
#else
    typedef struct tag_hPROXYSERVERADDRESSDETECTOR 
    {
        const iProxyServerAddressDetectorVtbl* vtbl; // pointer to the "ProxyServerAddressDetector" virtual table
        const iSYSTEMVtbl*                    sys;  // pointer to the "SYSTEM" virtual table
    } *hPROXYSERVERADDRESSDETECTOR;
#endif // if defined( __cplusplus )
// AVP Prague stamp end
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( ProxyServerAddressDetector_PRIVATE_DEFINITION )
// AVP Prague stamp end
// AVP Prague stamp begin( Interface function forward declarations,  )


    typedef   tERROR (pr_call *fnpProxyServerAddressDetector_getProxyAddress)( hPROXYSERVERADDRESSDETECTOR _this, cProxySettings* proxyAddress, const cStringObj* destinationUrl ); // -- ;


// AVP Prague stamp end
// AVP Prague stamp begin( Interface declaration,  )
struct iProxyServerAddressDetectorVtbl 
{
    fnpProxyServerAddressDetector_getProxyAddress  getProxyAddress;
}; // "ProxyServerAddressDetector" external virtual table prototype
// AVP Prague stamp end
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( ProxyServerAddressDetector_PRIVATE_DEFINITION )
// AVP Prague stamp end
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
    #define CALL_ProxyServerAddressDetector_getProxyAddress( _this, proxyAddress, destinationUrl )                                ((_this)->vtbl->getProxyAddress( (_this), proxyAddress, destinationUrl ))
#else // if !defined( __cplusplus )
    #define CALL_ProxyServerAddressDetector_getProxyAddress( _this, proxyAddress, destinationUrl )                                ((_this)->getProxyAddress( proxyAddress, destinationUrl ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
    struct pr_abstract iProxyServerAddressDetector 
    {
        virtual tERROR pr_call getProxyAddress( cProxySettings* proxyAddress, const cStringObj* destinationUrl ) = 0;
    };

    struct pr_abstract cProxyServerAddressDetector : public iProxyServerAddressDetector, public iObj 
    {

        OBJ_IMPL( cProxyServerAddressDetector );

        operator hOBJECT() { return (hOBJECT)this; }
        operator hPROXYSERVERADDRESSDETECTOR()   { return (hPROXYSERVERADDRESSDETECTOR)this; }

    };

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end
// AVP Prague stamp begin( Header endif,  )
#endif // __i_proxyserveraddressdetector__2ef5bc0c_9d4d_455c_bd55_d39a744744ba
// AVP Prague stamp end
