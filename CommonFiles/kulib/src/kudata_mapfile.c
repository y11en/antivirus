
#include "kudata_mapfile.h"
#include "kudata_int.h"


#ifdef WIN32

# define WIN32_LEAN_AND_MEAN
# define VC_EXTRALEAN
# define NOUSER
# define NOSERVICE
# define NOIME
# define NOMCX
# include <windows.h>

int kudata_mapfile( kudata_t* data, const char* filename )
{
	HANDLE file;
	HANDLE map;

	kudata_init( data );

	file = CreateFile(
			filename, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL );
	if( file == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	map = CreateFileMapping( file, NULL, PAGE_READONLY, 0, 0, NULL );
	CloseHandle( file );
	if( map == NULL )
	{
		return 0;
	}

	data->header = (kudata_header_t*) MapViewOfFile( map, FILE_MAP_READ, 0, 0, 0 );
	CloseHandle( map );
	if( data->header == NULL )
	{
		return 0;
	}

	data->map = data->header;

	return 1;
}


void kudata_unmapfile( kudata_t* data )
{
	if( data->map != NULL )
	{
		UnmapViewOfFile( data->map );

		data->header = NULL;
		data->map = NULL;
	}
}


#else // WIN32




#endif