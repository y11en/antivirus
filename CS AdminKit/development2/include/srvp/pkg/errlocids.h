#ifndef __KLPKG_ERRLOCIDS_H__
#define __KLPKG_ERRLOCIDS_H__

namespace KLPKG
{
	enum
    {
        PKGEL_ERR_SHOULD_STOP_CATCHED_TASK = 1
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {PKGEL_ERR_SHOULD_STOP_CATCHED_TASK, L"ƒанный пакет используетс€ задачей \"%1\". ќстановите задачу и повторите попытку."}
	};
};

#endif //__KLPKG_ERRLOCIDS_H__
