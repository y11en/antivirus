////////////////////////////////////////////////////////////////////
//
//  DIBConv.cpp
//  DDB to DIB and DIB to DDB conversion functions implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "DIBConv.h"

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.
#define WIDTHBYTES(bits)           ((unsigned)((bits+31)&(~31))/8)  /* ULONG aligned ! */

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

#define PALVERSION   0x300

#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)


#define DibWidthBytesN(lpbi, n) (UINT)WIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)

#define DibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
																		? ((DWORD)(UINT)DibWidthBytes(lpbi) * (DWORD)(UINT)(lpbi)->biHeight) \
                                    : (lpbi)->biSizeImage)

#define FixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)               \
																	(lpbi)->biSizeImage = DibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
																	(lpbi)->biClrUsed = DibNumColors(lpbi);   /*\
                                if ((lpbi)->biCompression == BI_BITFIELDS && (lpbi)->biClrUsed == 0) \
                                    (lpbi)->biClrUsed = 3;                    */


// ---
static void InitBitmapInfoHeader ( LPBITMAPINFOHEADER lpBmInfoHdr, DWORD dwWidth, DWORD dwHeight, WORD nBPP )	{
	memset (lpBmInfoHdr, 0, sizeof (BITMAPINFOHEADER));

	lpBmInfoHdr->biSize			= sizeof (BITMAPINFOHEADER);
	lpBmInfoHdr->biWidth		= dwWidth;
	lpBmInfoHdr->biHeight		= dwHeight;
	lpBmInfoHdr->biPlanes		= 1;
  lpBmInfoHdr->biCompression  = BI_RGB;

	if ( nBPP <= 1 )
		nBPP = 1;
	else 
		if (nBPP <= 4) {
			nBPP = 4;
			lpBmInfoHdr->biClrUsed = 16;
		}
		else 
			if (nBPP <= 8) {
				nBPP = 8;
				lpBmInfoHdr->biClrUsed = 256;
			}
			else
				nBPP = 24;

	lpBmInfoHdr->biBitCount  = nBPP;
	lpBmInfoHdr->biSizeImage = WIDTHBYTES(dwWidth * nBPP) * dwHeight;
}

// ---
static WORD GetNumColors( LPSTR lpbi ) {

	// if handle to DIB is invalid, return 0 
	if ( lpbi == NULL )	
		return 0;

	WORD wBitCount;  // DIB bit count

	//  If this is a Windows-style DIB, the number of colors in the
	//  color table can be less than the number of bits per pixel
	//  allows for (i.e. lpbi->biClrUsed can be set to some value).
	//  If this is the case, return the appropriate value.

	if ( IS_WIN30_DIB(lpbi) )	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if ( dwClrUsed != 0 )	
			return (WORD)dwClrUsed;
	}
	//  Calculate the number of colors in the color table based on
	//  the number of bits per pixel for the DIB.

	if ( IS_WIN30_DIB(lpbi) )
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	// return number of colors based on bits per pixel
	switch ( wBitCount ) {
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
		default:
			return 0;
	}
}
	
// ---
static WORD GetPaletteSize( LPSTR lpBI ) 	{
	// if handle to DIB is invalid, return 0 
	if (lpBI == NULL)	
  	return 0;

 // calculate the size required by the palette 
  BOOL bWin30;

  bWin30 = (IS_WIN30_DIB (lpBI));
  
  if (bWin30)
		return (WORD)(GetNumColors(lpBI) * sizeof(RGBQUAD));
  else
	  return (WORD)(GetNumColors(lpBI) * sizeof(RGBTRIPLE));
}

// ---	
static LPSTR FindDIBBits( LPSTR lpDIBHeader ) {
	// if handle to DIB is invalid, return NULL
  if (lpDIBHeader == NULL)		
  	return NULL;

	return (lpDIBHeader + *(LPDWORD)lpDIBHeader + ::GetPaletteSize(lpDIBHeader));
}
	


// DDBToDIB				- Creates a DIB from a DDB
// hBitmap				- Device dependent bitmap
// hPalette				- Logical palette
HANDLE DDBToDIB( HBITMAP hBitmap, HPALETTE hPalette, DWORD *pdwDIBSize ) {
	BITMAP							rcBitmap;
	BITMAPINFOHEADER		bmInfoHdr;
	LPBITMAPINFOHEADER	lpbmInfoHdr;
	LPSTR								lpBits;
	HDC									hMemDC;
	HPALETTE 						hOldPal = NULL;
	HANDLE              hDIB;
	DWORD               dwLen;

	// Do some setup -- make sure the rcBitmap passed in is valid,
	//  get info on the bitmap (like its height, width, etc.),
	//  then setup a BITMAPINFOHEADER.

	if ( !hBitmap )
		return NULL;

	if ( !::GetObject (hBitmap, sizeof (rcBitmap), (LPSTR) &rcBitmap) )
		return NULL;

	::InitBitmapInfoHeader( &bmInfoHdr, rcBitmap.bmWidth, rcBitmap.bmHeight,
												  WORD(rcBitmap.bmPlanes * rcBitmap.bmBitsPixel) );

	// Now allocate memory for the DIB.  Then, set the BITMAPINFOHEADER
	//  into this memory, and find out where the bitmap bits go.
	dwLen = sizeof (BITMAPINFOHEADER) +	::GetPaletteSize((LPSTR) &bmInfoHdr) + bmInfoHdr.biSizeImage;

	hDIB = ::GlobalAlloc( GPTR, dwLen );

	if ( !hDIB )
		return NULL;

	lpbmInfoHdr = (LPBITMAPINFOHEADER)hDIB;

	*lpbmInfoHdr = bmInfoHdr;
	lpBits       = ::FindDIBBits ((LPSTR) lpbmInfoHdr);


	// Now, we need a DC to hold our bitmap.  If the app passed us
	//  a palette, it should be selected into the DC.

	hMemDC = ::GetDC (NULL);

	if ( hPalette != NULL ) {
		hOldPal = ::SelectPalette( hMemDC, hPalette, FALSE );
		::RealizePalette( hMemDC );
	}

	// We're finally ready to get the DIB.  Call the driver and let
	//  it party on our bitmap.  It will fill in the color table,
	//  and bitmap bits of our global memory block.

	if (!::GetDIBits (hMemDC,
						hBitmap,
						0,
						rcBitmap.bmHeight,
						lpBits,
						(LPBITMAPINFO) lpbmInfoHdr,
						DIB_RGB_COLORS))	{
		::GlobalFree( hDIB );
		hDIB = NULL;
	}

	// Finally, clean up and return.

	if ( hOldPal != NULL )
		::SelectPalette( hMemDC, hOldPal, FALSE );

	::ReleaseDC (NULL, hMemDC );

	if ( pdwDIBSize )
		*pdwDIBSize = dwLen;

	return hDIB;
}


// ---
static LPBITMAPINFOHEADER GetBitmapInfo( HANDLE hDIB ) {
	LPBITMAPINFOHEADER  pdibInfo = NULL;

	pdibInfo = (LPBITMAPINFOHEADER)hDIB;

  FixBitmapInfo( pdibInfo );

	return pdibInfo;
}
	

// --- Effect : This function creates a palette from a DIB by allocating memory for the
//				logical palette, reading and storing the colors from the DIB's color table
//				into the logical palette, creating a palette from this logical palette,
//				and then returning the palette's handle. This allows the DIB to be
//				displayed using the best possible colors (important for DIBs with 256 or
//				more colors).
// ---
static BOOL InitPalette( LPSTR lpbi, HPALETTE *hPalette )	{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = TRUE; // Initialized with TRUE because when NumColors returns ZERO
							 // ie. when reading a TRUE COLOR DIB, NumColors returns ZERO
							 // and then InitPalette  succeedes because there is no need
							 // for a palette.

	*hPalette = NULL;

	// if handle to DIB is invalid, return FALSE
	if ( lpbi == NULL )	
		return FALSE;

  // get pointer to BITMAPINFO (Win 3.0)
  lpbmi = (LPBITMAPINFO)lpbi;

  // get pointer to BITMAPCOREINFO (old 1.x)
  lpbmc = (LPBITMAPCOREINFO)lpbi;
	
  // get the number of colors in the DIB
  wNumColors = ::GetNumColors( lpbi );

  if ( wNumColors != 0 ) {
		// allocate memory block for logical palette 
		hLogPal = ::GlobalAlloc(GPTR, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);

		// if not enough memory, clean up and return NULL 
		if (hLogPal == 0)	
			return FALSE;

		lpPal = (LPLOGPALETTE)hLogPal;

		// set version and number of palette entries
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		// is this a Win 3.0 DIB?
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)	{
			if (bWinStyleDIB)	{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else	{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		// create the palette and get handle to it
		*hPalette = ::CreatePalette( lpPal );

		::GlobalFree((HGLOBAL) hLogPal);
	}

	return bResult;
}
	

// DIBToDDB	- Creates a DDB from a DIB
// hDIB   	- Device independent bitmap
HBITMAP DIBToDDB( HANDLE hDIB ) {
	HBITMAP             hBitmap;
	LPBITMAPINFOHEADER	lpbi;
	HPALETTE						hPalette = NULL;
	HPALETTE						hOldPal = NULL;
	HDC 								hDC;	
	LPSTR								lpDIBHdr, lpDIBBits;

	if ( hDIB == NULL )
		return NULL;

  lpbi = ::GetBitmapInfo( hDIB );

  if ( !::InitPalette((LPSTR)lpbi, &hPalette) )
			return FALSE;

	hDC = ::GetDC( NULL );

	if ( hPalette ) {
		// Select and realize the palette
		hOldPal = ::SelectPalette( hDC, hPalette, FALSE );
		::RealizePalette( hDC );
	}

	lpDIBHdr  = (LPSTR)lpbi;
	lpDIBBits = FindDIBBits(lpDIBHdr);

	hBitmap = ::CreateDIBitmap (hDC,
	 	                       (LPBITMAPINFOHEADER)lpDIBHdr,
	     	                   CBM_INIT,
	        	               lpDIBBits,
	            	           (LPBITMAPINFO) lpbi,
	                	       DIB_RGB_COLORS);
	if ( hPalette ) {
		::SelectPalette( hDC, hOldPal, FALSE );
		::DeleteObject( hPalette );
	}

	::ReleaseDC (NULL, hDC );

	return hBitmap;
}

/*
// DIBToDDB	- Creates a DDB from a DIB
// hDIB   	- Device independent bitmap
HBITMAP DIBToDDB( HANDLE hDIB ) {
	LPBITMAPINFOHEADER	lpbi;
	HBITMAP 						hbm;
	HPALETTE						hPalette = NULL;
	HPALETTE						hOldPal;
	HDC 								hDC;	

	if (hDIB == NULL)
		return NULL;

	hDC = ::GetDC( NULL );

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = lpbi->biClrUsed 
								? lpbi->biClrUsed 
								:	1 << lpbi->biBitCount;

	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB;
	LPVOID lpDIBBits;
	if ( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
												  bmInfo.bmiHeader.biClrUsed) + 
													((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	// Create and select a logical palette if needed
	if( nColors <= 256 && ::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		hPalette = ::CreatePalette( pLP );

		delete[] pLP;

		// Select and realize the palette
		hOldPal = ::SelectPalette( hDC, hPalette, FALSE );
		::RealizePalette( hDC );
	}

	
	hbm = ::CreateDIBitmap( hDC,											// handle to device context
													(LPBITMAPINFOHEADER)lpbi,	// pointer to bitmap info header 
													(LONG)CBM_INIT,						// initialization flag
													lpDIBBits,								// pointer to initialization data 
													(LPBITMAPINFO)lpbi,				// pointer to bitmap info
													DIB_RGB_COLORS );					// color-data usage 

	if ( hPalette ) {
		::SelectPalette( hDC, hOldPal, FALSE );
		::DeleteObject( hPalette );
	}

	return hbm;
}
*/


// DDIToDIB				- Creates a DIB from a DDI
// hIcon   			  - Device dependent icon
// dwCompression	- Type of compression - see BITMAPINFOHEADER (It's best to use the BI_RGB)
// hPalette				- Logical palette
HANDLE DDIToDIB( HICON hIcon, HPALETTE hPalette, DWORD *pdwDIBSize ) {
	ICONINFO stIconInfo;
	::GetIconInfo( hIcon, &stIconInfo );

	DWORD dwMaskSize;
	DWORD dwColorSize;

	HANDLE hMaskDIB = ::DDBToDIB( stIconInfo.hbmMask, hPalette, &dwMaskSize );
	HANDLE hColorDIB = ::DDBToDIB( stIconInfo.hbmColor, hPalette, &dwColorSize );

	::DeleteObject( stIconInfo.hbmMask );
	::DeleteObject( stIconInfo.hbmColor );

	if ( pdwDIBSize )
		*pdwDIBSize = 0;

	HANDLE hDIB = NULL;

	if ( hMaskDIB && hColorDIB ) {
		hDIB = ::GlobalAlloc( GPTR, dwMaskSize + dwColorSize + sizeof(dwMaskSize) );	
		if ( hDIB ) {
			PBYTE pDIB = PBYTE(hDIB);
			memcpy( (void *)pDIB, &dwMaskSize, sizeof(dwMaskSize) );

			pDIB += sizeof(dwMaskSize);
			memcpy( (void *)pDIB, hMaskDIB, dwMaskSize );

			pDIB += dwMaskSize;
			memcpy( (void *)pDIB, hColorDIB, dwColorSize );

			if ( pdwDIBSize )
				*pdwDIBSize = dwMaskSize + dwColorSize;
		}
	}

	if ( hMaskDIB )
		::GlobalFree( hMaskDIB );		
	if ( hColorDIB )
		::GlobalFree( hColorDIB );		
		
	return hDIB;
}

// DIBToDDI	- Creates a DDI from a DIB
// hDIB   	- Device independent bitmap
HICON DIBToDDI( HANDLE hDIB ) {
	DWORD dwMaskSize;

	PBYTE pDIB = PBYTE(hDIB);
	memcpy( &dwMaskSize, (void *)pDIB, sizeof(dwMaskSize) );

	pDIB += sizeof(dwMaskSize);
	HBITMAP hMaskDDB = ::DIBToDDB( HANDLE(pDIB) );

	pDIB += dwMaskSize;
	HBITMAP hColorDDB = ::DIBToDDB( HANDLE(pDIB) );

	ICONINFO stIconInfo = {0};
	stIconInfo.fIcon = TRUE;
	stIconInfo.hbmMask = hMaskDDB;
	stIconInfo.hbmColor = hColorDDB;

	HICON hIcon = ::CreateIconIndirect( &stIconInfo );

	::DeleteObject( stIconInfo.hbmMask );
	::DeleteObject( stIconInfo.hbmColor );

	return hIcon;
}


