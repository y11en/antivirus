// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  07 October 2004,  12:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- avp3info.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __avp3info_cpp__5c158cc0_beda_4cb5_a16a_5370b7eed5b7 )
#define __avp3info_cpp__5c158cc0_beda_4cb5_a16a_5370b7eed5b7
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <AV/plugin/p_avp3info.h>
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAVP3Info : public cAVP3Info
{
private:
// Internal function declarations

// Property function declarations

public:
// External function declarations
	tERROR pr_call GetBaseFileInfo( hOBJECT p_hObj, cSerializable* p_pInfo );
	tERROR pr_call GetBasesInfo( hSTRING p_hBasesPath, cSerializable* p_pInfo );

// Data declaration
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
    mDECLARE_INITIALIZATION(CAVP3Info)
};
// AVP Prague stamp end

// returns errOBJECT_NOT_FOUND if unknown format
tERROR GetBaseFileInfo_KDB (hOBJECT hParent, const void *data, tUINT size, struct cAVP3BasesInfo* pInfo);

// AVP Prague stamp begin( Header endif,  )
#endif // avp3info_cpp
// AVP Prague stamp end



