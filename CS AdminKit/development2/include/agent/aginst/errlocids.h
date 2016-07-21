#ifndef __KLAGINST_ERRLOCIDS_H__
#define __KLAGINST_ERRLOCIDS_H__

namespace KLAGINST
{
	enum
    {
		// general errors        
        AGINSTEL_RUN_TASK_LOGONUSER_ERROR = 1,
		AGINSTEL_START_COMPONENT_WRONG_REGISTRATION,
		AGINSTEL_START_COMPONENT_CANT_LOAD_BIN_MODULE
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {AGINSTEL_RUN_TASK_LOGONUSER_ERROR, L"Can't start task '%1': user's logon operation failed ( check user name or password )"},
		{AGINSTEL_START_COMPONENT_WRONG_REGISTRATION, L"Configuration of the application is wrong, please reinstall application."},
		{AGINSTEL_START_COMPONENT_CANT_LOAD_BIN_MODULE, L"Can't load binary module. Command line - '%1'. System error - '%2'"}
	};
};

#endif //__KLAGINST_ERRLOCIDS_H__
