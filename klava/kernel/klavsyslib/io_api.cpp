// io_api.cpp
//
//

#include <klava/klavsys.h>
#include <klava/klav_props.h>
#include <klava/klav_utils.h>

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAVSYS_Create_File (
			const char * path,
			uint32_t access,
			uint32_t flags,
			KLAV_Properties *props,
			KLAV_IO_Object **pobject
		)
{
	return KLAVSYS_Get_IO_Library ()->create_file (0, path, access, flags, props, pobject);
}

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAVSYS_Delete_File (
			const char * path,
			uint32_t flags
		)
{
	return KLAVSYS_Get_IO_Library ()->delete_file (0, path, flags);
}

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAVSYS_Move_File (
			const char * src_path,
			const char * dst_path,
			uint32_t flags
		)
{
	return KLAVSYS_Get_IO_Library ()->move_file (0, src_path, dst_path, flags);
}


