#ifndef __KLTR_ERRLOCIDS_H__
#define __KLTR_ERRLOCIDS_H__

namespace KLTR
{
	enum
    {
		// general errors        
        TREL_TR_CONN_BROKEN = 1,
        TREL_TR_METHOD_NOT_FOUND,		
		TREL_TR_TIMEOUT,
		TREL_TR_TRANSPORT_ERROR
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {TREL_TR_CONN_BROKEN, L"Transport level error in process of communication with '%1': connection was broken"},
        {TREL_TR_METHOD_NOT_FOUND, L"Transport level error in process of communication with '%1': method not found"},		
		{TREL_TR_TIMEOUT, L"Transport level error in process of communication with '%1': timeout period is expired"},
		{TREL_TR_TRANSPORT_ERROR, L"Transport level error in process of communication with '%1': internal error code %2"}
	};
};

#endif //__KLTR_ERRLOCIDS_H__
