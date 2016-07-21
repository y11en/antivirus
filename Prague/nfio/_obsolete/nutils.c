#ifdef __MWERKS__
	#include <sys\stat.h>
	#include <errno.h>
#endif	
	
#include <prague.h>
#include "nfio_io.h"
//#include "utils.h"


tERROR pr_call _convert_nfio_errno (int errcode) 
{
	switch (errcode)
	{
	case 0:				return errOK;
	case EROFS:
#ifndef __MWERKS__	
	case ETXTBSY:
#endif	
	case EACCES:		return errACCESS_DENIED;
	case ENAMETOOLONG:	return errBUFFER_TOO_SMALL;
	case ENOENT:
	case ENOTDIR:		return errNOT_FOUND;
	case EFAULT:		return errPARAMETER_INVALID;
	default:	return errUNEXPECTED;
	}
}

tUINT pr_call mb_len( tSTRING str, tUINT size ) {
	tUINT len;
	if ( !str || !*str )
		return 0;
	for( len=1; (len<size) && str[len]; len++ )
		;
	return len;
}

// ---
tBOOL pr_call is_relative_path( tCHAR* buffer, tDWORD size ) {
	if ( (size > MBS) && (*buffer == '\\' || (*buffer == '/')) )
		return cFALSE;
	else if ( (size >= 2*MBS) && (*(buffer+1) == ':') )
		return cFALSE;
	else
		return cTRUE;
}




// ---
tERROR pr_call is_folder( tCHAR* name, int * werr ) {
	int lwerr;
	struct stat stbuf;  

	if ( !werr )
		werr = &lwerr;

	if ( stat (name, &stbuf) == 0 )
	{
		*werr = 0;
		return S_ISDIR (stbuf.st_mode);
	}

    return _convert_nfio_errno (*werr = errno);
}

