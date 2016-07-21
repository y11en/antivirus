// win_err.cpp
//
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_Translate_System_Error (uint32_t ec)
{
	// TODO:
	KLAV_ERR err = KLAV_EUNKNOWN;
	switch (ec)
	{
	case ERROR_SUCCESS:
		err = KLAV_OK;
		break;
	case ERROR_ACCESS_DENIED:
		err = KLAV_EACCESS;
		break;
	case ERROR_FILE_EXISTS:
	case ERROR_ALREADY_EXISTS:
		err = KLAV_EDUPLICATE;
		break;
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
		err = KLAV_ENOTFOUND;
		break;
	case ERROR_NOT_ENOUGH_MEMORY:
		err = KLAV_ENOMEM;
		break;
	case ERROR_READ_FAULT:
		err = KLAV_EREAD;
		break;
	case ERROR_WRITE_FAULT:
		err = KLAV_EWRITE;
		break;
	case ERROR_HANDLE_DISK_FULL:
	case ERROR_DISK_FULL:
		err = KLAV_ENOSPACE;
		break;
	default:
		err = KLAV_MAKE_SYSTEM_ERROR (ec);
		break;
	}
	return err;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_Get_System_Error ()
{
	uint32_t ec = GetLastError ();
	return KLAV_Translate_System_Error (ec);
}

#endif // _WIN32
