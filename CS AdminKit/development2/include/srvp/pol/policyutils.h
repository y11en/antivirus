/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	policyutils.h
 * \author	Mikhail Karmazine
 * \date	10.04.2003 16:19:49
 * \brief	файл для работы с объектами POlicy, пердающимися через SOAP
 * 
 */


#ifndef __KLPOL_POLICYUTILS_H__
#define __KLPOL_POLICYUTILS_H__

#include "soaph.h"
#include "policies.h"

void PolicyFromSoap( const klpol_policy_common_data & policySource, 
                     KLPOL::PolicyCommonData & policyDest );

void PolicyForSoap( struct soap* soap,
                    const KLPOL::PolicyCommonData & policySource,
                    klpol_policy_common_data & policyDest );

void PolicyFromSoap( const klpol_policy_common_data & policySource, 
                    KLPAR::ParamsPtr& policyDest );

KLPAR::ParamsValuePtr PolicyFromSoap( 
                   const klpol_policy_common_data & policySource);


void PolicyForSoap( struct soap* soap,
                    KLPAR::Params* policySource,
                    klpol_policy_common_data & policyDest );



#endif //__KLPOL_POLICYUTILS_H__