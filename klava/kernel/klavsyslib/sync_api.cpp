// sync_api.cpp
//
//

#include <klava/klavsys.h>

////////////////////////////////////////////////////////////////

hKLAV_Mutex KLAV_CALL KLAVSYS_Create_Mutex (klav_bool_t recursive)
{
	return KLAVSYS_Get_Sync_Factory ()->create_mutex (recursive);
}

hKLAV_RWLock KLAV_CALL KLAVSYS_Create_RWLock ()
{
	return KLAVSYS_Get_Sync_Factory ()->create_rwlock ();
}

