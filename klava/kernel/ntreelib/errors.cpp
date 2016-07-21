// errors.cpp
//
//

#include "tns_mgr.h"

#include <stdexcept>

void tns_raise_error (KLAV_ERR err, const char *info)
{
	if (info == 0)
		info = "";

	throw std::runtime_error (info);
}


