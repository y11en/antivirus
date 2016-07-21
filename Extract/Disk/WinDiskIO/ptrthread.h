#ifndef __PTR_THREAD_H
#define __PTR_THREAD_H

#include <windows.h>

#include <prague.h>
#include <iface/i_buffer.h>

#define  IMPEX_EXPORT_LIB
#include "plugin/p_windiskio.h"


tERROR
exDiskExLogical_Next (
	hSTRING hStrLogical
	);

#endif __PTR_THREAD_H
