// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  06 May 2005,  13:51 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- antispamfilter.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __antispamfilter_cpp__ba1d954d_0616_45de_974c_f2eb64b3536c )
#define __antispamfilter_cpp__ba1d954d_0616_45de_974c_f2eb64b3536c
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <Mail/plugin/p_antispamtask.h>
#include <Mail/structs/s_antispam.h>

#include "antispam_interface_imp2.h"
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAntispamFilter : public cAntispamFilter {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call Process( tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics );
	tERROR pr_call Train( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );
	tERROR pr_call SetSettings( const cSerializable* p_pSettings );
	tERROR pr_call GetSettings( cSerializable* p_pSettings );
	tERROR pr_call Init( const cSerializable* p_pSettings );

// Data declaration
	cAntispamSettings   m_settings;            // --
	cAntispamStatistics m_statistics;          // --
	tDWORD              m_state;               // --
	hREGISTRY           m_persistance_storage; // --
// AVP Prague stamp end



	CAntispamImplementer::CKLAntispamFilter* m_pFilterImp;
	CAntispamImplementer::CKLAntispamFilter* m_pFilterTrain;
	cCriticalSection* m_lock; //!< Критическая секция для корректного разведения настроек

private:
	struct LastObject_t
	{
		LastObject_t(): action(-1) {;};
		LastObject_t(cStringObj s, tDWORD act): sz(s), action(act)	{;};
		cStringObj sz;
		tDWORD     action;
	};
	typedef cVector<LastObject_t> LastObjectNames_t;
	LastObjectNames_t m_szLastObjectNames;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CAntispamFilter)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // antispamfilter_cpp
// AVP Prague stamp end



