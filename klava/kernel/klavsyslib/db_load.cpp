// db_load.cpp
//

#include <klava/klavsys.h>
#include <klava/klav_utils.h>

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL KLAVSYS_Open_DB_File (
			const char * fname,
			hKLAV_DB_Loader *phdl
		)
{
	*phdl = 0;

	if (fname == 0 || fname [0] == 0)
		return KLAV_EINVAL;

	KLAV_Alloc *alloc = KLAV_Get_System_Allocator ();
	KLAV_Virtual_Memory *vmem = KLAVSYS_Get_Virtual_Memory ();
	
	KLAV_KFB_File_Loader * ldr = KLAV_NEW (alloc) KLAV_KFB_File_Loader ();
	if (ldr == 0)
		return KLAV_ENOMEM;

	KLAV_ERR err = ldr->open (alloc, vmem, fname);
	if (KLAV_FAILED (err))
	{
		KLAV_DELETE (ldr, alloc);
		return err;
	}

	*phdl = ldr;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAVSYS_Close_DB_File (
			hKLAV_DB_Loader hdl
		)
{
	if (hdl == 0)
		return KLAV_ENOTIMPL;

	KLAV_KFB_File_Loader *ldr = static_cast <KLAV_KFB_File_Loader *> (hdl);
	KLAV_ERR err = ldr->close ();

	KLAV_DELETE (ldr, KLAV_Get_System_Allocator ());

	return err;
}


