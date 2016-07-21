#ifndef __PRCI_ERRLOCIDS_H__
#define __PRCI_ERRLOCIDS_H__

namespace KLPRCI
{
	enum
    {
		// general errors
        PRCIEL_TASK_IS_RUNNING = 1
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {PRCIEL_TASK_IS_RUNNING,   L"Task '%1' from taskstorage is already running"}
	};
};


#endif //__PRCI_ERRLOCIDS_H__
