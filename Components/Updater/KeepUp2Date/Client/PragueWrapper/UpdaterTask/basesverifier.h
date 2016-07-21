// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  23 October 2007,  20:44 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- basesverifier.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __basesverifier_cpp__39d04561_855e_4d74_a745_146525995c5c )
#define __basesverifier_cpp__39d04561_855e_4d74_a745_146525995c5c
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Updater/plugin/p_updater2005.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CBaseVerifier : public cBasesVerifier 
{
private:
// Internal function declarations

// Property function declarations

public:
// External function declarations
	tERROR pr_call VerifyBases( hSTRING p_components, hSTRING p_blackListPath, tDATETIME* p_basesDate );

// Data declaration
// AVP Prague stamp end


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CBaseVerifier)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // basesverifier_cpp
// AVP Prague stamp end



