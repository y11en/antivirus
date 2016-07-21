// pr_error.cpp
//
//

#include <klava/klav_prague.h>

KLAV_ERR KLAV_From_Pr_Error (tERROR pr_err)
{
	switch (pr_err)
	{
	case errOBJECT_NOT_FOUND:
		return KLAV_ENOTFOUND;
	default:
		break;
	}
	return (KLAV_ERR) pr_err;
}
