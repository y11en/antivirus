// proc_resolver.cpp
//
//

#include <klava/klavsys.h>
#include <klava/klav_utils.h>

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Proc_Resolver KLAV_CALL KLAVSYS_Create_Proc_Resolver (
			hKLAV_Proc_Resolver chain
		)
{
	KLAV_Alloc * alloc = KLAV_Get_System_Allocator ();
	KLAV_DSO_Loader * dso_loader = KLAVSYS_Get_DSO_Loader ();

	return KLAV_Create_DSO_Proc_Resolver (alloc, dso_loader, chain);
}

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Destroy_Proc_Resolver (
			hKLAV_Proc_Resolver obj
		)
{
	return KLAV_Destroy_DSO_Proc_Resolver (obj);
}

