#if defined( _WINDOWS ) || defined( _Windows )
#  include <malloc.h>
#elif defined( __OS2__ )
#  include <malloc.h>
#elif defined( __TANDEM )
#  include <stdlib.h>
#elif defined( MSDOS ) || defined( __MSDOS__ ) || defined( __DOS__ )
#  include <malloc.h>
#elif defined (__unix__)
#  include <stdlib.h>
#elif defined( VAX )
#  include <string.h>
#elif defined( _WINVXD )
#elif defined( _WINSYS )
#else
#error Memory manager is not finished yet
#endif

#include "CT_SUPP.h"

CT_ULONG CTAPI AMemCoreleft()
{
#if defined( _WINDOWS ) || defined( _Windows ) || defined( __OS2__ ) || defined( _WIN32 ) || defined( __unix__ ) || defined( __TANDEM ) || defined( VAX ) || defined( _WINVXD ) || defined( _WINSYS )
    return (CT_ULONG)0xFFFFFFUL;
#elif defined( MSDOS ) || defined( __MSDOS__ )
#  ifdef __BORLANDC__
    return (CT_ULONG)coreleft();
#  else
    return (CT_ULONG)_memavl();
#  endif
#else
#error AMemCoreleft()
#endif
}

CT_PTR CTAPI AMemAlloc( CT_ULONG size )
{
#if defined( _WINSYS )
        return (CT_PTR)new (NonPagedPool)char[size];
#elif defined( _WINDOWS ) || defined( _Windows ) || defined( __OS2__ ) || defined( __DOS__ ) || defined( _WIN32 ) || defined( __unix__ ) || defined( __TANDEM ) || defined( VAX ) || defined( _WINVXD )
    return (CT_PTR)malloc( (size_t)size );
#else
#error AMemAlloc()
#endif
}

void CTAPI AMemFree( CT_PTR mptr )
{
#if defined( _WINSYS )
        delete mptr;
#elif defined( _WINDOWS ) || defined( _Windows ) || defined( __OS2__ ) || defined( __DOS__ ) || defined( _WIN32 ) || defined( __unix__ ) || defined( __TANDEM ) || defined( VAX ) || defined( _WINVXD )
    free( (void *)mptr );
#else
#error AMemFree()
#endif
}

