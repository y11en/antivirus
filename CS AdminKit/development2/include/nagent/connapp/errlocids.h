#ifndef __KLCONN_ERRLOCIDS_H__
#define __KLCONN_ERRLOCIDS_H__

namespace KLCONN
{
	enum
    {
		// general errors
        CONNEL_FAILED_LOAD_CONNECTOR = 1,
        CONNEL_FAILED_INIT_CONNECTOR,
        CONNEL_PRODUCT_SIDE_SETTINGS_CHANGED,
        CONNEL_SERVER_SIDE_SETTINGS_CHANGED,
        CONNEL_REPLICATION_FAILED,
        CONNEL_REPLICATION_FAILED_FROM_PRODOUCT
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {CONNEL_FAILED_LOAD_CONNECTOR,              L"Failed loading integration module %1 for application %2. Error %3 ('%4') occured. "},
        {CONNEL_FAILED_INIT_CONNECTOR,              L"Failed initilize integration module for application %1. Module returned error %2 ('%3'). "},
        {CONNEL_PRODUCT_SIDE_SETTINGS_CHANGED,      L"Settings of application %1 changed (%2). Changes will be sent to the administration server. "},
        {CONNEL_SERVER_SIDE_SETTINGS_CHANGED,       L"Administration Server changed settings of application %1 (%2)."},
        {CONNEL_REPLICATION_FAILED,                 L"Error %1 (%2) occured while replicating settings for application %3. Operation code: %4."},
        {CONNEL_REPLICATION_FAILED_FROM_PRODOUCT,   L"Appication %1 returned error %2 (%3) while replicating settings. Operation code: %4."}
	};
};

#endif //__KLCONN_ERRLOCIDS_H__
