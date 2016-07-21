#ifndef __KLSTD_ERRLOCIDS_H__
#define __KLSTD_ERRLOCIDS_H__

namespace KLPRSS
{
	enum
    {
		// general errors
		PRSSEL_STORAGE_CORRUPT = 1
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
		{PRSSEL_STORAGE_CORRUPT,    L"Storage file %1 is corrupt"}
	};
};

#endif //__KLSTD_ERRLOCIDS_H__
