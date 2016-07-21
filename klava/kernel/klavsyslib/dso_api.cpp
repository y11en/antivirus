// dso_api.cpp
//
//

#include <klava/klavsys.h>

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
KLAV_ERR KLAVSYS_DSO_Load (
			const char * path,
			uint32_t flags,
			void ** phdl
		)
{
	return KLAVSYS_Get_DSO_Loader ()->dl_open (path, phdl);
}

KLAV_EXTERN_C
void * KLAVSYS_DSO_Symbol (
			void * hdl,
			const char * name
		)
{
	return KLAVSYS_Get_DSO_Loader ()->dl_sym (hdl, name);
}

KLAV_EXTERN_C
void KLAVSYS_DSO_Unload (
			void * hdl
		)
{
	KLAVSYS_Get_DSO_Loader ()->dl_close (hdl);
}
