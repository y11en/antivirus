/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: transbmp.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_TransBMP_h	/* [ */
#define Included_TransBMP_h

#ifdef __cplusplus
extern "C" {
#endif

BOOL TransparentBitmap(HBITMAP hBitmap, 
					   int iXsize, 
					   int iYsize, 
                       DWORD rgbTransparent, 
					   HDC hDC, 
					   int iXoffset,
					   int iYoffset);

#ifdef __cplusplus
}
#endif

#endif /* ] Included_TransBMP_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
