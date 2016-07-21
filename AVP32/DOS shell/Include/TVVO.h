/*
  	Define __DOS16__ if it is not defined
*/
#if defined(__MSDOS__) && !defined(__DOS32__) && !defined(__DOS16__)
#define __DOS16__
#endif
/*
	Check target platform
*/
#if !defined(__MSDOS__) && !defined(__OS2__) && !defined(__NT__)
#error "Unknown target platform!"
#endif
/*
	Define TV_CDECL
*/
#ifdef __NT__
#define TV_CDECL
#else
#define TV_CDECL	cdecl
#endif
/*
	Issue some pragmas for Borland
*/
#if defined(__MSDOS__) && defined(__BORLANDC__)
#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif
#endif // __MSDOS__ && __BORLANDC__
