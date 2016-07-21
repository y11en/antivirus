/*
 * File:	wbimage.cc
 * Purpose:	Platform Independent Image Base Class (Windows version)
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995 Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
#include "imabmp.h"

#if CIMAGE_SUPPORT_BMP
#include "dibutils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImageBMP::CImageBMP( const CBitmap* bmp)
{
    bgindex = -1;
    lpbi = 0;
    RawImage = 0;
    imagePalette = 0;
    if (bmp) {     
        BITMAP bm;
        HBITMAP hBitmap = (HBITMAP)(bmp->GetSafeHandle());
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);
        int width = bm.bmWidth;
        int height = bm.bmHeight;
        //      int depth = bm.bmPlanes;
        int depth = bm.bmBitsPixel;
        Create(width, height, depth);
        HDC dc = ::GetDC(NULL);
        if (GetDIBits(dc, hBitmap, 0, GetHeight(),
            RawImage, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS) == 0)
        {
            TRACE0("GetDIBits failed");       
        }
        // Create a palette - PJO
        HPALETTE palette;
        if (palette = MakePalette((const BITMAPINFO FAR*)lpbi, 0))
        {
            imagePalette = new CImagePalette;
            imagePalette->Attach(palette);
            DibSetUsage(lpbi, (HPALETTE) (*imagePalette), CIMAGE_COLORS);
            ColorType = (COLORTYPE_PALETTE | COLORTYPE_COLOR);
        } 
        ::ReleaseDC(NULL, dc);
    }
}

BOOL CImageBMP::ReadFile(const CString& imageFileName)
{
  if (imageFileName != "")
	 filename = imageFileName;

  if (lpbi = DibOpenFile((char *)(const char *)filename))  {
		Width =  DibWidth(lpbi);
		Height = DibHeight(lpbi);
		Depth = DibBitCount(lpbi);
		RawImage = (ImagePointerType)DibPtr(lpbi);
		EffWidth = (long)(((long)Width*Depth + 31) / 32) * 4;
		HPALETTE palette;
		if (palette = MakePalette((const BITMAPINFO FAR*)lpbi, 0))
		{
		  imagePalette = new CImagePalette;
		  imagePalette->Attach(palette);
		  DibSetUsage(lpbi, (HPALETTE) (*imagePalette), CIMAGE_COLORS);
		  ColorType = (COLORTYPE_PALETTE | COLORTYPE_COLOR);
		} 

		return TRUE;
  }
  return FALSE;
}


BOOL CImageBMP::SaveFile(const CString& imageFileName)
{                         
  if (imageFileName != "")
	 filename = imageFileName;

  HPALETTE hPal = 0;
  if (imagePalette)
    hPal = (HPALETTE) (*imagePalette);
    
  DibSetUsage(lpbi, hPal, DIB_RGB_COLORS);

  BOOL retValue = WriteDIB((const char *)imageFileName, lpbi);

  DibSetUsage(lpbi, hPal, DIB_PAL_COLORS);

  return retValue;

/*
//  return WriteDIB(ImageFileName, HandleFromDib(lpbi));
  // This was the original code used in wxImage, but the resulting
  // file isn't a valid BMP file.
  DibSetUsage(lpbi, (HPALETTE) (*imagePalette), DIB_RGB_COLORS);
  if (!DibWriteFile((char *)(const char *)filename, lpbi))
	return FALSE;
	else return TRUE;
*/
}

#endif 	// CIMAGE_SUPPORT_BMP
