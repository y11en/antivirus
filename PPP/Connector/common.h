#pragma once

//////////////////////////////////////////////////////////////////////////
// events

#define EVENT_KAV6_LOCID  0x34869808 //is a crc32 for "EVENT_KAV6_LOCID"

#define MAKE_EVENT_ID(event, severity) (((event) << 16) | ((severity) << 0))
#define EVENT_EVENTID(eventid)  (enNotifyIds     (((eventid) >> 16) & 0xffff))
#define EVENT_SEVERITY(eventid) (enNotifySeverity(((eventid) >>  0) & 0xffff))

inline int PragueSeveriry2CscSeveriry(enNotifySeverity eSeverity)
{
	switch(eSeverity)
	{
		case eNotifySeverityCritical:  return KLEVP_EVENT_SEVERITY_CRITICAL;
		case eNotifySeverityError:     return KLEVP_EVENT_SEVERITY_ERROR;
		case eNotifySeverityImportant: return KLEVP_EVENT_SEVERITY_WARNING;
	}
	return KLEVP_EVENT_SEVERITY_INFO;
}

inline wstring GetEventType(enNotifyIds eEventID, enNotifySeverity eSeverity)
{
	wstring strEventID;
	switch(MAKE_EVENT_ID(eEventID, eSeverity))
	{
	case MAKE_EVENT_ID(eNotifyThreats, eNotifySeverityCritical):
		strEventID = KLEVP::c_szwGnrlEvent_VirusFound;
		break;
	
	case MAKE_EVENT_ID(eNotifySuspicious, eNotifySeverityCritical):
		strEventID = KLEVP::c_szwGnrlEvent_SuspiciousFound;
		break;

	case MAKE_EVENT_ID(eNotifyThreatNotCured, eNotifySeverityCritical):
		strEventID = KLEVP::c_szwGnrlEvent_ObjectNotCured;
		break;
	
	case MAKE_EVENT_ID(eNotifyPswdArchive, eNotifySeverityNotImportant):
		strEventID = KLEVP::c_szwGnrlEvent_PasswProtectedFound;
		break;

	case MAKE_EVENT_ID(eNotifyThreats, eNotifySeverityNotImportant):
		strEventID = KLEVP::c_szwGnrlEvent_ObjectCured;
		break;

	case MAKE_EVENT_ID(eNotifyThreatDeleted, eNotifySeverityNotImportant):
		strEventID = KLEVP::c_szwGnrlEvent_ObjectDeleted;
		break;

	case MAKE_EVENT_ID(eNotifyThreatQuarantined, eNotifySeverityNotImportant):
		strEventID = KLEVP::c_szwGnrlEvent_Quarantined;
		break;

	case MAKE_EVENT_ID(eNotifyAttackBlocked, eNotifySeverityCritical):
		strEventID = KLEVP::c_szwGnrlEvent_AttackDetected;
		break;

//		strEventID = KLEVP::c_szwGnrlEvent_MacroBlocked;
//		strEventID = KLEVP::c_szwGnrlEvent_MacroAllowed;

	default:
		strEventID.resize(16);
		strEventID.resize(swprintf((wchar_t *)strEventID.c_str(), L"%08x", MAKE_EVENT_ID(eEventID, eSeverity)));
		break;
	}
	return strEventID;
}

const wchar_t **GetEventFreeParams(const wstring &strEventType)
{
	static const wchar_t *g_FreeEventParams_Virus[]  = { KLEVP::c_er_par9, KLEVP::c_er_par4, KLEVP::c_er_par3, KLEVP::c_er_par1, NULL };
	static const wchar_t *g_FreeEventParams_Attack[] = { KLEVP::c_er_par9, KLEVP::c_er_par8, KLEVP::c_er_par7, KLEVP::c_er_par6, KLEVP::c_er_par5, NULL };
	static const wchar_t *g_FreeEventParams_Other[]  = { KLEVP::c_er_par9, KLEVP::c_er_par8, KLEVP::c_er_par7, KLEVP::c_er_par6, KLEVP::c_er_par5, KLEVP::c_er_par4, KLEVP::c_er_par3, KLEVP::c_er_par2, KLEVP::c_er_par1, NULL };

	if(
		strEventType == KLEVP::c_szwGnrlEvent_VirusFound ||
		strEventType == KLEVP::c_szwGnrlEvent_SuspiciousFound ||
		strEventType == KLEVP::c_szwGnrlEvent_ObjectNotCured ||
		strEventType == KLEVP::c_szwGnrlEvent_PasswProtectedFound ||
		strEventType == KLEVP::c_szwGnrlEvent_ObjectCured ||
		strEventType == KLEVP::c_szwGnrlEvent_ObjectDeleted ||
		strEventType == KLEVP::c_szwGnrlEvent_Quarantined
		)
		return g_FreeEventParams_Virus;

	if( strEventType == KLEVP::c_szwGnrlEvent_AttackDetected )
		return g_FreeEventParams_Attack;

	return g_FreeEventParams_Other;
}

//////////////////////////////////////////////////////////////////////////