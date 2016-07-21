/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_auth.h
 * \author	Andrew Kazachkov
 * \date	19.03.2003 12:25:39
 * \brief	
 * 
 */

#ifndef __KLPRCI_AUTH_H__
#define __KLPRCI_AUTH_H__

#include "transport/avt/accesscontrol.h"
#include "transport/klsecurity/exports.h"
#include "transport/wat/authserver.h"
#include "transport/avtl/acllib.h"
#include "kca/prci/componentinstance.h"

namespace KLPRCI
{
    typedef AVTL::AutoAclWat  AutoAcl;
};

KLCSKCA_DECL void KLPRCI_CreateSecContext(
					KLWAT::ClientContext*	pContext,
					KLPRCI::SecContext**	ppSecContext);

KLCSKCA_DECL bool KLPRCI_AccessCheckAcl(
					KLPRCI::SecContext*				pContext,
					const KLAVT::ACE_DECLARATION*	pAcl,
					long							nAcl,
					AVP_dword						dwAccess,
					bool							bThrowException=true);

KLCSKCA_DECL bool KLPRCI_AccessCheckAclObj(
					KLPRCI::SecContext*				pContext,
					KLAVT::AccessControlList*		pAclObj,
					AVP_dword						dwAccess,
					bool							bThrowException=true);

KLCSKCA_DECL bool KLPRCI_AccessCheckEx(
					KLPRCI::SecContext*				pContext,
					const KLAVT::ACE_DECLARATION*	pAcl,
					long							nAcl,
					AVP_dword						dwAccess,
					const std::wstring*				pwstrBasicAuthLogin,
					bool							bThrowException=true);

/*!
  \brief	Returns textual represenatation of current user's SID
            If current thread imperonated uses imersonated SID

  \return	std::wstring - SID in textual form
*/
KLCSKCA_DECL std::wstring KLPRCI_GetCurrentSID();

//! OBSOLETE
KLSTD_INLINEONLY bool KLPRCI_AccessCheck(
					KLPRCI::SecContext*				pContext,
					const KLAVT::ACE_DECLARATION*	pAcl,
					long							nAcl,
					AVP_dword						dwAccess,
					bool							bThrowException=true)
{
    return KLPRCI_AccessCheckAcl(
					pContext,
					pAcl,
					nAcl,
					dwAccess,
					bThrowException);
}

//! OBSOLETE
KLSTD_INLINEONLY bool KLPRCI_AccessCheck(
					KLPRCI::SecContext*				pContext,
					KLAVT::AccessControlList*		pAclObj,
					AVP_dword						dwAccess,
					bool							bThrowException=true)
{
    return KLPRCI_AccessCheckAclObj(
					pContext,
					pAclObj,
					dwAccess,
					bThrowException);
};


#endif //__KLPRCI_AUTH_H__

// Local Variables:
// mode: C++
// End:
