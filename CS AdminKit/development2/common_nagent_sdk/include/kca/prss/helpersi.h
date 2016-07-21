/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	helpersi.h
 * \author	Andrew Kazachkov
 * \date	21.10.2002 18:25:53
 * \brief	
 * 
 */

#ifndef _KLPRSS_HELPERSI_H_
#define _KLPRSS_HELPERSI_H_

#include "settingsstorage.h"
#include "prssconst.h"


bool KLPRSS_HasArrayItemPolicyFmt(KLPAR::Value* pVal);


KLCSKCA_DECL void KLPRSS_GetMandatoryAndDefault(
                            KLPAR::Params*  pInData,
                            KLPAR::Params** ppMandatory,
                            KLPAR::Params** ppDefault);

#endif // _KLPRSS_HELPERSI_H_
