#ifndef __KLNAG_ERRLOCIDS_H__
#define __KLNAG_ERRLOCIDS_H__

namespace KLNAG
{
	enum
    {
		// general errors
        NAGEL_WFW_FAILED_ACCESS_FM = 1,
		NAGEL_WFW_FAILED_ADD_PORT,
        NAGEL_WFW_FAILED_REMOVE_PORT,
        NAGEL_WFW_FAILED_GENERAL,
        NAGNF_FULLSCAN_SENDING,
        NAGNF_FULLSCAN_SENT,
        NAGNF_PRES_ERROR,
        NAGNF_TSK_TSK_CANCELED_BY_USER,
        NAGNF_TSK_CANCELED_BY_NAGENT_STOP
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {NAGEL_WFW_FAILED_ACCESS_FM,        L"Error %1 ('%2') occured while opening firewall manager"},
        {NAGEL_WFW_FAILED_ADD_PORT,         L"Error %1 ('%2') occured while adding port %3 to firewall exceptions list"},
        {NAGEL_WFW_FAILED_REMOVE_PORT,      L"Error %1 ('%2') occured while removing port %3 from firewall exceptions list"},
        {NAGEL_WFW_FAILED_GENERAL,          L"Error %1 ('%2') occured while processing firewall exceptions"},
        {NAGNF_FULLSCAN_SENDING,            L"Sending to server time of last full scan"},
        {NAGNF_FULLSCAN_SENT,               L"Time of last full scan has been successively sent to the Administration Server"},
        {NAGNF_PRES_ERROR,                  L"Error %1 ('%2') occured while opening events storage"},
        {NAGNF_TSK_TSK_CANCELED_BY_USER,    L"Task execution was canceled by user"}, //@todo add this into localization
        {NAGNF_TSK_CANCELED_BY_NAGENT_STOP, L"Task execution was canceled due to the Network agent stopping"} //@todo add this into localization        
	};
};

#endif //__KLNAG_ERRLOCIDS_H__
