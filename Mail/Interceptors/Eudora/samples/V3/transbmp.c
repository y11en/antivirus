/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: transbmp.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <windowsx.h>

BOOL 
TransparentBitmap(	HBITMAP hBitmap, 
					int iXsize, 
					int iYsize, 
                    DWORD rgbTransparent, 
					HDC hDC, 
					int iXoffset,
					int iYoffset)
{
	HDC mDC, nDC;
	HBITMAP hMask;
	LPBYTE lpBits;

	//  Create a memory DC for bitmap work
	mDC = CreateCompatibleDC( hDC );

	//  If memory DC was created...
	if( mDC )                             
	{
		//  Select passed bitmap to memory DC
		SelectObject( mDC, hBitmap );      

		//  Allocate a buffer for monochrome bitmap
		lpBits = (LPBYTE) GlobalAllocPtr( GHND, iXsize * iYsize * 2 );

		if( lpBits )
		{   
			//  Create mono (mask) bitmap
			hMask = CreateBitmap( iXsize, iYsize, 1, 1, lpBits );
			if( hMask )
			{  
				//  If mono bitmap worked, create mem DC for it
				nDC = CreateCompatibleDC( hDC );

				if( nDC )
				{
					//  Apply mask bitmap to mem DC
					SelectObject( nDC, hMask );

					// Specify "see thru" color
					SetBkColor( mDC, rgbTransparent );   

					//  First, blit original bitmap into mono space
					//  to create the bitmap mask
					BitBlt( nDC, 
							0, 
							0, 
							iXsize, 
							iYsize, 
							mDC, 
							0, 
							0, 
							SRCCOPY );

					//  Now blit original onto screen and invert
					BitBlt( hDC, 
							iXoffset, 
							iYoffset,
							iXsize, 
							iYsize, 
							mDC, 
							0, 
							0, 
							SRCINVERT );

					//  AND screen bits with mask bits
					//  This will "mask" anything specified
					//  by the previous call to SetBkColor()
					BitBlt( hDC, 
							iXoffset, 
							iYoffset,
							iXsize, 
							iYsize, 
							nDC, 
							0, 
							0, 
							SRCAND );

					//  Blit original again inverting again
					//  to produce transparent image
					BitBlt( hDC, 
							iXoffset, 
							iYoffset, 
							iXsize, 
							iYsize, 
							mDC, 
							0, 
							0, 
							SRCINVERT );

					//  Delete second mem DC
					DeleteDC( nDC );                        
				}

				//  Delete mask bitmap
				DeleteObject( hMask );                       
			}

			//  Free space mask took up
			GlobalFreePtr( lpBits );                   
		}

		//  Kill off the memory DC, unlocking passed BMP
		DeleteDC( mDC );         
	}
	return( TRUE );
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
