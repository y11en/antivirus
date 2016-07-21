// io_impl.cpp
//
// Base IO object implementation
//

#include <klava/klav_io.h>

KLAV_ERR KLAV_IO_Get_Current_Pos (KLAV_IO_Object *io, klav_filepos_t *ppos)
{
	return io->ioctl (KLAV_IOCTL_GET_CURRENT_POS, ppos, sizeof (klav_filepos_t));
}

KLAV_ERR KLAV_IO_Set_Current_Pos (KLAV_IO_Object *io, klav_filepos_t pos)
{
	return io->ioctl (KLAV_IOCTL_SET_CURRENT_POS, &pos, sizeof (klav_filepos_t));
}

