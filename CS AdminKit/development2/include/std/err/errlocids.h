#ifndef __KLSTD_ERRLOCIDS_H__
#define __KLSTD_ERRLOCIDS_H__

namespace KLSTD
{
	enum
    {
		// general errors
		STDEL_FILE_OPEN_ERROR = 1,
        STDEL_FILE_IO_ERROR,
        STDEL_FILESYSTEM_ERROR,
        STDEL_FILE_MOVE_ERROR,
        STDEL_FILE_COPY_ERROR,
        STDEL_FILE_DELETE_ERROR,
        STDEL_DIR_DELETE_ERROR,
        STDEL_ERROR_INFO,
        STDEL_APP_PENDING, //!todo: localize me
		STDEL_MISSING_ARG, //!todo: localize me
        STDEL_UNKNOWN_ARG, //!todo: localize me
        STDEL_INVALID_ARG  //!todo: localize me
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
		{STDEL_FILE_OPEN_ERROR,     L"Error %1 ('%2') occured while opening file '%3'"},
        {STDEL_FILE_IO_ERROR,       L"I/o error %1 ('%2') occured while working with file '%3'"},
        {STDEL_FILESYSTEM_ERROR,    L"Error %1 ('%2') accessing filesystem object '%3'"},
        {STDEL_FILE_MOVE_ERROR,     L"Error %1 ('%2') occured while moving file '%3' to '%4'"},
        {STDEL_FILE_COPY_ERROR,     L"Error %1 ('%2') occured while copying file '%3' to '%4'"},
        {STDEL_FILE_DELETE_ERROR,   L"Error %1 ('%2') occured while deleting file '%3'"},
        {STDEL_DIR_DELETE_ERROR,    L"Error %1 ('%2') occured while deleting directory '%3'"},
        {STDEL_ERROR_INFO,          L"Error information: %1/%2 (%3), %4, %5"},
        {STDEL_APP_PENDING,         L"Operation cannot be performed as the application is initializing or deinitializing just now. Please, try later."},
        {STDEL_MISSING_ARG,         L"Invalid command line syntax: missing necessary parameter %1"},
        {STDEL_UNKNOWN_ARG,         L"Invalid command line syntax: unknown parameter %1"},
        {STDEL_INVALID_ARG,         L"Invalid command line syntax: invalid parameter '%1'"}
	};
};

#endif //__KLSTD_ERRLOCIDS_H__
