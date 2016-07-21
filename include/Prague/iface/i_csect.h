// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_csect.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_csect__55f14007_34f6_426e_b93e_febea613bee9 )
#define __i_csect__55f14007_34f6_426e_b93e_febea613bee9
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// CriticalSection interface implementation
// Short comments -- Critical section behaviour
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_CRITICAL_SECTION  ((tIID)(17))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tSHARE_LEVEL; // type of share level
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define SHARE_LEVEL_ERROR              ((tSHARE_LEVEL)(-1)) //  --
#define SHARE_LEVEL_READ               ((tSHARE_LEVEL)(0)) // count of threads can simulationly get access with this level
#define SHARE_LEVEL_WRITE              ((tSHARE_LEVEL)(1)) // only one thread at once can successfully get this level of access or extend SHARE_LEVEL_READ to this value
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hCRITICAL_SECTION;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iCriticalSectionVtbl;
typedef struct iCriticalSectionVtbl iCriticalSectionVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cCriticalSection;
	typedef cCriticalSection* hCRITICAL_SECTION;
#else
	typedef struct tag_hCRITICAL_SECTION {
		const iCriticalSectionVtbl* vtbl; // pointer to the "CriticalSection" virtual table
		const iSYSTEMVtbl*          sys;  // pointer to the "SYSTEM" virtual table
	} *hCRITICAL_SECTION;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( CriticalSection_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpCriticalSection_Enter)    ( hCRITICAL_SECTION _this, tSHARE_LEVEL level ); // -- locks synchronization object;
	typedef   tERROR (pr_call *fnpCriticalSection_Leave)    ( hCRITICAL_SECTION _this, tSHARE_LEVEL* prev_level ); // -- unlocks synchronization object;
	typedef   tERROR (pr_call *fnpCriticalSection_TryEnter) ( hCRITICAL_SECTION _this, tSHARE_LEVEL level ); // -- try enter critical section, but do not block a calling thread;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iCriticalSectionVtbl {
	fnpCriticalSection_Enter     Enter;
	fnpCriticalSection_Leave     Leave;
	fnpCriticalSection_TryEnter  TryEnter;
}; // "CriticalSection" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( CriticalSection_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION   mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgLAST_OS_ERROR       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgSHARE_LEVEL_SUPPORT mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001003 )
#define pgOWNING_THREAD       mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001004 )
#define pgOWNING_COUNT        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgCALLER_ADDR         mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001006 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_CriticalSection_Enter( _this, level )                          ((_this)->vtbl->Enter( (_this), level ))
	#define CALL_CriticalSection_Leave( _this, prev_level )                     ((_this)->vtbl->Leave( (_this), prev_level ))
	#define CALL_CriticalSection_TryEnter( _this, level )                       ((_this)->vtbl->TryEnter( (_this), level ))
#else // if !defined( __cplusplus )
	#define CALL_CriticalSection_Enter( _this, level )                          ((_this)->Enter( level ))
	#define CALL_CriticalSection_Leave( _this, prev_level )                     ((_this)->Leave( prev_level ))
	#define CALL_CriticalSection_TryEnter( _this, level )                       ((_this)->TryEnter( level ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iCriticalSection {
		virtual tERROR pr_call Enter( tSHARE_LEVEL level ) = 0; // -- locks synchronization object
		virtual tERROR pr_call Leave( tSHARE_LEVEL* prev_level ) = 0; // -- unlocks synchronization object
		virtual tERROR pr_call TryEnter( tSHARE_LEVEL level ) = 0; // -- try enter critical section, but do not block a calling thread
	};

	struct pr_abstract cCriticalSection : public iCriticalSection, public iObj {
		
		OBJ_IMPL( cCriticalSection );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hCRITICAL_SECTION()   { return (hCRITICAL_SECTION)this; }

		tDWORD pr_call get_pgLAST_OS_ERROR() { return (tDWORD)getDWord(pgLAST_OS_ERROR); }
		tERROR pr_call set_pgLAST_OS_ERROR( tDWORD value ) { return setDWord(pgLAST_OS_ERROR,(tDWORD)value); }

		tBOOL pr_call get_pgSHARE_LEVEL_SUPPORT() { return (tBOOL)getBool(pgSHARE_LEVEL_SUPPORT); }
		tERROR pr_call set_pgSHARE_LEVEL_SUPPORT( tBOOL value ) { return setBool(pgSHARE_LEVEL_SUPPORT,(tBOOL)value); }

		tDWORD pr_call get_pgOWNING_THREAD() { return (tDWORD)getDWord(pgOWNING_THREAD); }
		tDWORD pr_call get_pgOWNING_COUNT()  { return (tDWORD)getDWord(pgOWNING_COUNT); }
		tPTR   pr_call get_pgCALLER_ADDR()   { return (tPTR)getPtr(pgCALLER_ADDR); }
	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end


#if defined (__cplusplus)

class cAutoCS {
public:
	cAutoCS( const cCriticalSection* cs, bool exclusive ) : m_locked(0), m_cs(cs) { lock(exclusive); } // locks cs immediately
	cAutoCS( const cCriticalSection* cs, tUINT dummy0, tUINT dummy1 ) : m_locked(0), m_cs(cs) { ; } // doesn't lock cs immediately
	~cAutoCS()                      { while(m_locked && PR_SUCC(unlock()) ); }
	
public:
	tERROR lock( bool exclusive )   { tERROR e = !m_cs ? errOBJECT_BAD_INTERNAL_STATE : const_cast<cCriticalSection*>(m_cs)->Enter(exclusive ? SHARE_LEVEL_WRITE : SHARE_LEVEL_READ); m_locked += PR_SUCC(e); return e; }
	tERROR unlock()                 { if ( !m_locked ) return errNOT_OK; tERROR e = const_cast<cCriticalSection*>(m_cs)->Leave(0); m_locked -= PR_SUCC(e); return e; }
	
protected:
	tUINT                   m_locked;
	const cCriticalSection* m_cs;
};

#endif 

// AVP Prague stamp begin( Header endif,  )
#endif // __i_csect__55f14007_34f6_426e_b93e_febea613bee9
// AVP Prague stamp end

