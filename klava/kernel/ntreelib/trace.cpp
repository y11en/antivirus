// trace.cpp
//
//

#include "tns_impl.h"
#include <stdio.h>

void Tree_Manager::tns_trace (const char *fmt, ...)
{
#ifdef TNS_ENABLE_TRACE
	if ((m_node_flags & TNS_F_TRACE) != 0)
	{
		va_list ap;
		va_start (ap, fmt);
		vprintf (fmt, ap);
		va_end (ap);
	}
#endif // TNS_ENABLE_TRACE
}

