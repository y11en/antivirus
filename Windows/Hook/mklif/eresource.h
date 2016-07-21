#ifndef __ERESOURCE__
#define __ERESOURCE__
//
//  Resource support
//

FORCEINLINE
VOID
AcquireResourceExclusive (
    IN PERESOURCE Resource
   )
{
	FltAcquireResourceExclusive(Resource);
}

FORCEINLINE
VOID
AcquireResourceShared (
    IN PERESOURCE Resource
   )
{
	FltAcquireResourceShared(Resource);
}

FORCEINLINE
VOID
ReleaseResource (
    IN PERESOURCE Resource
   )
{
	FltReleaseResource(Resource);
}


#endif ///__ERESOURCE__