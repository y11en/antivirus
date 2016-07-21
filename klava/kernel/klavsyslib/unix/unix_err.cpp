// unic_err.cpp
//
//

#if defined (__unix__)
#include <errno.h>
#include <klavsys_unix.h>

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_Translate_System_Error (uint32_t ec)
{
	// TODO:
	KLAV_ERR err = KLAV_EUNKNOWN;
	switch ( ec )
	{
	case 0:
		err = KLAV_OK;
		break;
	case ENOMEM:
		err = KLAV_ENOMEM;
		break;
	case EACCES:
	case EPERM:
	case EROFS:
	case ETXTBSY:
	case EISDIR:
		err = KLAV_EACCESS;
		break;
	case EBADF:
	case EINVAL:
		err = KLAV_EINVAL;
		break;
	case ENOENT:
	case ENOTDIR:
		err = KLAV_ENOTFOUND;
		break;
	case EEXIST:
		err = KLAV_EDUPLICATE;
		break;
	case ENOSPC:
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
	return KLAV_Translate_System_Error (errno);
}

#endif // __unix__
