/*!
 * (C) 2007 Kaspersky Lab
 *
 * \file	nap_const.h
 * \author	Eugene Rogozhnikov
 * \date	30.10.2007 14:59:25
 * \brief	
 *
 */

#ifndef __NAP_CONST_H__
#define __NAP_CONST_H__

#define NAPSHV_PRODUCT_NAME			L"NAP_SHV"
#define NAPSHV_PRODUCT_VERSION		L"1.0.0.0"

//! имена контейнеров, для правил
#define KLNAP_COMPLIANT_RULE   L"CompliantRule"

//! имена контейнеров, для условий
#define KLNAP_AV_INSTALLED_CONDITION		L"AVSoftwareInstalled"
#define KLNAP_AV_RUNNING_CONDITION		L"AVSoftwareRunning"
#define KLNAP_HAS_RTP_CONDITION			L"HasRTP"
#define KLNAP_RTP_STATE_CONDITION			L"RTPState"
#define KLNAP_LAST_FSCAN_CONDITION		L"LastFScanDate"
#define KLNAP_BASES_DATE_CONDITION		L"BasesDate"
#define KLNAP_LAST_CONNECTED_CONDITION	L"LastConnectedDate"

//! имена настроек условия
#define KLNAP_RULE_CONDITION_CHECK   L"check"	// BOOL
#define KLNAP_RULE_CONDITION_OPERATION   L"operation"	// INT - KLNAP::AttributeCompareOperation
#define KLNAP_RULE_CONDITION_VALUE   L"value"	// Зависит от условия

const wchar_t cszNapAgentServiceName[] = L"napagent";

//! флаг о том, должен ли в нагенте работать NAP SHA
#define KLNAP_NAGENT_ENABLE_NAP_FLAG   L"ENABLE_NAP_FLAG"

namespace KLNAP
{
	// The System Health ID used by both the SDK SHA (client) & SDK SHV (server).
	// В соответствии с постом http://forums.microsoft.com/TechNet/ShowPost.aspx?PostID=2353878&SiteID=17
	// у данного 4-байтного идентификатора первые три байта занимает KasperskyLabVendorId, а последний
	// байт - идентификатор самого компонента
	const UINT32 KL_SYSTEM_HEALTH_ID = 0x005C7401;

	// {19CE407F-9FEE-4873-9ABC-B010CEFEA501}
	static const GUID CLSID_KLNAP_SHV_UI = 
		{ 0x19CE407F, 0x9FEE, 0x4873, { 0x9A, 0xBC, 0xB0, 0x10, 0xCE, 0xFE, 0xA5, 0x01 } };

	//23668 - присвоен Kaspersky Lab смотреть на IANA (http://www.iana.org/assignments/enterprise-numbers)
	const UINT32 KasperskyLabVendorId = 23668;

	const wchar_t VENDOR_NAME[] = L"KasperskyLab";

	enum AttributeCompareOperation
	{
		ACO_EQUAL = 0,
		ACO_NOT_EQUAL,
		ACO_LESS,
		ACO_LESS_OR_EQUAL,
		ACO_GREATER,
		ACO_GREATER_OR_EQUAL,
		ACO_IN,
		ACO_NOT_IN
	};

	typedef enum
    {
        MaskRtpState_Stopped                = 0x00010000,
        MaskRtpState_Suspended              = 0x00020000,
        MaskRtpState_Starting               = 0x00040000,
        MaskRtpState_Running                = 0x00080000,
        MaskRtpState_Running_MaxProtection  = 0x00100000,
        MaskRtpState_Running_MaxSpeed       = 0x00200000,
        MaskRtpState_Running_Recomended     = 0x00400000,
        MaskRtpState_Running_Custom         = 0x00800000,
        MaskRtpState_Failure                = 0x01000000
    } RtpStateMask;

}; // namespace KLNAP

#endif //__NAP_CONST_H__