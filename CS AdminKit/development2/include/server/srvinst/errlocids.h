#ifndef __KLHSTGRP_ERRLOCIDS_H__
#define __KLHSTGRP_ERRLOCIDS_H__

namespace KLHSTGRP
{
	enum
    {
		// general errors        
        HSTGRPEL_ERR_NO_HOST_ADDRESS = 1,
        HSTGRPEL_ERR_HOST_NOT_RESPONDING,
        HSTGRPEL_ERR_NO_NAGENT_PACKAGE,
        HSTGRPEL_ERR_SLAVE_NOT_CONNECTED,
        HSTGRPEL_ERR_FAILED_TO_SYNC_WITH_HOST,
        HSTGRPEL_ERR_FAILED_TO_REPLICATE,
        HSTGRPEL_ERR_FAILED_TO_SYNCHRONIZE,
        HSTGRPEL_ERR_FAILED_TO_MERGE_POLICY,
        HSTGRPEL_ERR_STOP_DUE_TO_ERROR,
        HSTGRPEL_ERR_FAILED_TO_ACCESS_DB,
        HSTGRPEL_ERR_INVALID_IP_RESOLUTION
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {HSTGRPEL_ERR_NO_HOST_ADDRESS, L"Address of host '%1' is unavailable."}, 
        {HSTGRPEL_ERR_HOST_NOT_RESPONDING, L"Host '%1' is not responding."},
        {HSTGRPEL_ERR_NO_NAGENT_PACKAGE, L"Package for Network agent doesn't exist. At first create it, please."},
        {HSTGRPEL_ERR_SLAVE_NOT_CONNECTED, L"Slave server '%1' isn't connected."},
        {HSTGRPEL_ERR_FAILED_TO_SYNC_WITH_HOST, L"Failed to synchronize with host %1."},
        {HSTGRPEL_ERR_FAILED_TO_REPLICATE, L"Failed to replicate data on host %1."},
        {HSTGRPEL_ERR_FAILED_TO_SYNCHRONIZE, L"Failed to to synchronize data with host %1."},
        {HSTGRPEL_ERR_FAILED_TO_MERGE_POLICY, L"Failed to merge policy with settings for host %1 (%2). Policy: '%3', settings: '%4'."},
        {HSTGRPEL_ERR_STOP_DUE_TO_ERROR, L"Kaspersky Administration Server stopped due to error."},
        {HSTGRPEL_ERR_FAILED_TO_ACCESS_DB, L"Failed to connect to the database in %1 seconds."},
        {HSTGRPEL_ERR_INVALID_IP_RESOLUTION, L"Invalid ip-address resolution: ip address %1 was resolved into '%2' and into '%3'."}
	};
};

#endif //__KLHSTGRP_ERRLOCIDS_H__
