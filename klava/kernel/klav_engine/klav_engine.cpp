// scanner_engine.cpp
//
//

#include "scanner_engine_impl.h"
#include <klava/klav_utils.h>

#ifdef KL_PLATFORM_WINDOWS
# include <windows.h>
#endif

KLAV_Malloc_Alloc g_malloc;

extern "C"
KLAV_ERR KLAV_CALL KLAV_SCANNER_ENGINE_CREATE_FN (hKLAV_Scanner_Engine *phengine)
{
	KLAV_ERR err = KLAV_OK;
	*phengine = 0;

	KLAV_Alloc * allocator = & g_malloc;

	KLAV_Scanner_Engine_Impl *engine = KLAV_NEW (allocator) KLAV_Scanner_Engine_Impl (allocator);
	if (engine == 0)
		return KLAV_ENOMEM;

	*phengine = engine;
	return KLAV_OK;
}

