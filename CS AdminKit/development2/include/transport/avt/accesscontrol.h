/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	avt/accesscontrol.h
 * \author	Mikhail Karmazine
 * \date	20.02.2003 12:32:18
 * \brief	
 * 
 */

#ifndef __KLAVT_ACCESSCONTROL_H__
#define __KLAVT_ACCESSCONTROL_H__

#if defined(__unix) || defined(_MBCS) || defined(N_PLAT_NLM)
# define KLAVT_OFF
#endif // _MBCS

//#define KLAVT_OFF

#include "std/base/klbase.h"
#include "transport/wat/authserver.h"
#include "transport/avt/acedecl.h"

namespace KLAVT
{
    class AccessControlList : public KLSTD::KLBase
    {
    public:
        virtual ~AccessControlList() {};

        virtual bool AccessCheck( KLWAT::AccessTokenHolder * pAccessTokenHolder, AVP_dword dwDesiredAccess ) = 0;
    };
}

KLCSTR_DECL void KLAVT_CreateACL( const KLAVT::ACE_DECLARATION* pAceList,
                                 int nAceCount,
                                 //const std::wstring & wstrDomain,
                                 KLAVT::AccessControlList** ppAccessControlList );

KLCSTR_DECL bool KLAVT_LookupActName(int nAccountId, std::wstring& wstrName);

#endif //__KLAVT_ACCESSCONTROL_H__

// Local Variables:
// mode: C++
// End:
