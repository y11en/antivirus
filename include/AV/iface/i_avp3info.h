// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  07 October 2004,  12:45 --------
// File Name   -- (null)i_avp3info.cpp
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_avp3info__58fe3968_9440_4b3b_bf85_678b3dd050f4 )
#define __i_avp3info__58fe3968_9440_4b3b_bf85_678b3dd050f4
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/iface/i_string.h>
#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// AVP3Info interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AVP3INFO  ((tIID)(38011))
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
    typedef tUINT hAVP3INFO;
#else
// AVP Prague stamp end



#if defined ( __cplusplus )
	struct cSerializable;
#else
	typedef struct tag_cSerializable cSerializable;
#endif



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAVP3InfoVtbl;
typedef struct iAVP3InfoVtbl iAVP3InfoVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
    struct cAVP3Info;
    typedef cAVP3Info* hAVP3INFO;
#else
    typedef struct tag_hAVP3INFO 
    {
        const iAVP3InfoVtbl* vtbl; // pointer to the "AVP3Info" virtual table
        const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
    } *hAVP3INFO;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AVP3Info_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


    typedef   tERROR (pr_call *fnpAVP3Info_GetBaseFileInfo) ( hAVP3INFO _this, hOBJECT hObj, cSerializable* pInfo ); // -- ;
    typedef   tERROR (pr_call *fnpAVP3Info_GetBasesInfo)    ( hAVP3INFO _this, hSTRING hBasesPath, cSerializable* pInfo ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAVP3InfoVtbl 
{
    fnpAVP3Info_GetBaseFileInfo  GetBaseFileInfo;
    fnpAVP3Info_GetBasesInfo     GetBasesInfo;
}; // "AVP3Info" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AVP3Info_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
    #define CALL_AVP3Info_GetBaseFileInfo( _this, hObj, pInfo )                 ((_this)->vtbl->GetBaseFileInfo( (_this), hObj, pInfo ))
    #define CALL_AVP3Info_GetBasesInfo( _this, hBasesPath, pInfo )              ((_this)->vtbl->GetBasesInfo( (_this), hBasesPath, pInfo ))
#else // if !defined( __cplusplus )
    #define CALL_AVP3Info_GetBaseFileInfo( _this, hObj, pInfo )                 ((_this)->GetBaseFileInfo( hObj, pInfo ))
    #define CALL_AVP3Info_GetBasesInfo( _this, hBasesPath, pInfo )              ((_this)->GetBasesInfo( hBasesPath, pInfo ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
    struct pr_abstract iAVP3Info 
    {
        virtual tERROR pr_call GetBaseFileInfo( hOBJECT hObj, cSerializable* pInfo ) = 0;
        virtual tERROR pr_call GetBasesInfo( hSTRING hBasesPath, cSerializable* pInfo ) = 0;
    };

    struct pr_abstract cAVP3Info : public iAVP3Info, public iObj 
    {
        OBJ_IMPL( cAVP3Info );
        operator hOBJECT() { return (hOBJECT)this; }
        operator hAVP3INFO()   { return (hAVP3INFO)this; }

    };

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_avp3info__58fe3968_9440_4b3b_bf85_678b3dd050f4
// AVP Prague stamp end



