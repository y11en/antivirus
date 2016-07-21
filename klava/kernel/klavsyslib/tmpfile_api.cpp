// tmpfile_api.cpp
//
//

#include <klava/klavsys.h>

////////////////////////////////////////////////////////////////

KLAV_ERR KLAVSYS_Create_Temp_File (
			KLAV_Properties * props,
			KLAV_IO_Object **ptempobj
		)
{
	return KLAVSYS_Get_Temp_Object_Manager ()->create_temp_object (0, props, ptempobj);
}

