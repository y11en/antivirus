/*!
 * (C) 2007 Kaspersky Lab
 *
 * \file	NapSettings.h
 * \author	Eugene Rogozhnikov
 * \date	02.11.2007 16:37:37
 * \brief	
 *
 */

#ifndef __NAPSETTINGS_H__
#define __NAPSETTINGS_H__

#include <nap\nap_sdk\naptypes.h>

namespace KLNAP
{
	#define COMPLIANCE_RESULT_E_NOTPATCHED				 _HRESULT_TYPEDEF_(0x80270050L)
	#define COMPLIANCE_RESULT_E_COMPLIANT				 _HRESULT_TYPEDEF_(0x00000000L)

	/////////////////////////////////////////////////////////////////////////
	// Идентификаторы атрибутов
	typedef enum {
		AVP_AVINSTALLED_CODE = 0,
		AVP_AVRUNNING_CODE,
		AVP_HAS_RTP_CODE,
		AVP_RTP_STATE_CODE,
		AVP_LAST_FSCAN_CODE,
		AVP_BASES_DATE_CODE,
		AVP_LAST_CONNECTED_CODE
	} AVP_TYPE;

	const int AVP_HEADER_LENGTH = 4;
	typedef struct tagNAP_AVP
	{
		unsigned short uCode;
		unsigned short uLength;
		unsigned char uValue[maxSoHAttributeSize];
	} NAP_AVP;

	const UINT16 KL_NAP_AVP_HEADER_SIZE = sizeof(unsigned short);// Размер uCode
}; // namespace KLNAP

#endif //__NAPSETTINGS_H__