// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  02 March 2005,  18:20 --------
// File Name   -- (null)i_as_trainwizard.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_as_trainwizard__698a509e_5edd_4903_8c5f_5f0094887190 )
#define __i_as_trainwizard__698a509e_5edd_4903_8c5f_5f0094887190
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// AS_TrainWizard interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AS_TRAINWIZARD  ((tIID)(40013))
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
	typedef tUINT hAS_TRAINWIZARD;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAS_TrainWizardVtbl;
typedef struct iAS_TrainWizardVtbl iAS_TrainWizardVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAS_TrainWizard;
	typedef cAS_TrainWizard* hAS_TRAINWIZARD;
#else
	typedef struct tag_hAS_TRAINWIZARD {
		const iAS_TrainWizardVtbl* vtbl; // pointer to the "AS_TrainWizard" virtual table
		const iSYSTEMVtbl*         sys;  // pointer to the "SYSTEM" virtual table
	} *hAS_TRAINWIZARD;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AS_TrainWizard_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAS_TrainWizard_Run) ( hAS_TRAINWIZARD _this, cSerializable* p_pSettings ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAS_TrainWizardVtbl {
	fnpAS_TrainWizard_Run  Run;
}; // "AS_TrainWizard" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AS_TrainWizard_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AS_TrainWizard_Run( _this, p_pSettings )                    ((_this)->vtbl->Run( (_this), p_pSettings ))
#else // if !defined( __cplusplus )
	#define CALL_AS_TrainWizard_Run( _this, p_pSettings )                    ((_this)->Run( p_pSettings ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAS_TrainWizard {
		virtual tERROR pr_call Run( cSerializable* p_pSettings ) = 0;
	};

	struct pr_abstract cAS_TrainWizard : public iAS_TrainWizard, public iObj {
		OBJ_IMPL( cAS_TrainWizard );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hAS_TRAINWIZARD()   { return (hAS_TRAINWIZARD)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_as_trainwizard__698a509e_5edd_4903_8c5f_5f0094887190
// AVP Prague stamp end



