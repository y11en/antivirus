/*
 * File:	cimageb.cc
 * Purpose:	Platform Independent Image Base Class (Windows version)
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995 Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */

#include "stdafx.h"
#include <windowsx.h>
#include "cmap.h"
#include "cimageb.h"
#include "dibutils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void
CImageImpl::Create(int width, int height, int depth, int colortype)
{
  Width = width; Height = height; Depth = depth;
  ColorType = (colortype>=0) ? colortype: ((Depth>8) ? COLORTYPE_COLOR: 0);

  if (lpbi)
    GlobalFreePtr(lpbi);
  RawImage = 0;
  if (imagePalette)
    delete imagePalette;
  imagePalette = 0;
  if (lpbi = DibCreate(Depth, Width, Height))  {
		RawImage = (ImagePointerType)DibPtr(lpbi);
		EffWidth = (long)(((long)Width*Depth + 31) / 32) * 4;
  }
}

CImageImpl::~CImageImpl ( )
{
  if (lpbi)  {
	 GlobalFreePtr(lpbi);
	 delete imagePalette;
  }
}

int CImageImpl::GetIndex(int x, int y)
{
  if (!Inside(x, y) || (Depth>8)) return -1;

  ImagePointerType ImagePointer = RawImage + EffWidth*y + (x*Depth >> 3);
  int index = (int)(*ImagePointer);
  return index;
}

BOOL CImageImpl::SetIndex(int x, int y, int index)
{
  if (!Inside(x, y) || (Depth>8)) return FALSE;

  ImagePointerType ImagePointer = RawImage + EffWidth*y + (x*Depth >> 3);
  *ImagePointer = index;

  return TRUE;
}

/* From "billow@nease.net" <billow@nease.net>
The Function SetIndex() and GetRGB() is wrong.
In you software, It set a int value and get a int value.
When the DIB is mono or 16 color, It's wrong.

JACS notes: this didn't seem to work (the Ellipse
transform in the demo made the image go black).
So going back to old version for now.
*/

#if 0
byte CImageImpl::GetIndex(int x, int y)
{
  if (!Inside(x, y) || (Depth>8)) return -1;

  ImagePointerType ImagePointer = RawImage + EffWidth*y + (x*Depth >> 3);
  byte index = (byte)(*ImagePointer);

  union{
	  byte index8;
	  struct{
		  byte v1:4,
               v0:4;
	  }index4;

	  struct{
		  byte v3:2,
		       v2:2,
		       v1:2,
		       v0:2;
	  }index2;
	  
	  struct{
		  byte v7:1,
		       v6:1,
		       v5:1,
		       v4:1,
		       v3:1,
		       v2:1,
		       v1:1,
		       v0:1;
	  }index1;

  }value;

  value.index8 = index;

  switch(Depth)
  {
   case 8:return index;
   case 4:
	 if(x%2) return value.index4.v1;
	 else return value.index4.v0;
   case 2:
 	switch(x%4)
	{
		case 0:return value.index2.v0;
		case 1:return value.index2.v1;
		case 2:return value.index2.v2;
		case 3:return value.index2.v3;
	};
   case 1:
	switch(x%8)
	{
		case 0:return value.index1.v0;
		case 1:return value.index1.v1;
		case 2:return value.index1.v2;
		case 3:return value.index1.v3;
		case 4:return value.index1.v4;
		case 5:return value.index1.v5;
		case 6:return value.index1.v6;
		case 7:return value.index1.v7;
	}
   default:
     return -1;
 }
}

BOOL CImageImpl::SetIndex(int x, int y, byte index)
{
  union{
	  byte index8;
	  struct{
		  byte v1:4,
               v0:4;
	  }index4;

	  struct{
		  byte v3:2,
		       v2:2,
		       v1:2,
		       v0:2;
	  }index2;
	  
	  struct{
		  byte v7:1,
		       v6:1,
		       v5:1,
		       v4:1,
		       v3:1,
		       v2:1,
		       v1:1,
		       v0:1;
	  }index1;

  }value;

  if (!Inside(x, y) || (Depth>8)) return FALSE;

  ImagePointerType ImagePointer = RawImage + EffWidth*y + (x*Depth >> 3);
  
  value.index8 = (byte)(*ImagePointer);

  switch(Depth)
  {
   case 8:
        value.index8 = index; // Fix added by Scott Chen <scott_chen@mintsystems.com>
        break;
   case 4:
    switch(x%2)
	{
	    case 0:value.index4.v0=index;break;
	    case 1:value.index4.v1=index;break;
	};
	break;
   case 2:
 	switch(x%4)
	{
		case 0:value.index2.v0=index;break;
		case 1:value.index2.v1=index;break;
		case 2:value.index2.v2=index;break;
		case 3:value.index2.v3=index;break;
	};
	break;
   case 1:
	switch(x%8)
	{
		case 0:value.index1.v0=index;break;
		case 1:value.index1.v1=index;break;
		case 2:value.index1.v2=index;break;
		case 3:value.index1.v3=index;break;
		case 4:value.index1.v4=index;break;
		case 5:value.index1.v5=index;break;
		case 6:value.index1.v6=index;break;
		case 7:value.index1.v7=index;break;
	}
	break;
   default:
     return FALSE;
 }
  *ImagePointer = value.index8;  
  return TRUE;
}
#endif

BOOL CImageImpl::GetRGB(int x, int y, byte* r, byte* g, byte* b)
{
  if (!Inside(x, y)) return FALSE;

  if (imagePalette) {
	 return imagePalette->GetRGB(GetIndex(x, y), r, g, b);
/*	 PALETTEENTRY entry;
	 ::GetPaletteEntries((HPALETTE) (*imagePalette), GetIndex(x, y), 1, &entry);
	 *r = entry.peRed;
	 *g = entry.peGreen;
	 *b = entry.peBlue;  */
  } else {
	 ImagePointerType ImagePointer = RawImage + EffWidth*y + (x*Depth >> 3);
	 *b = ImagePointer[0];
	 *g = ImagePointer[1];
	 *r = ImagePointer[2];
  }
  return TRUE;
}


BOOL CImageImpl::SetRGB(int x, int y, byte r, byte g, byte b)
{
  if (!Inside(x, y)) return FALSE;

  if (ColorType & COLORTYPE_PALETTE)
  {
	 if (!imagePalette) return FALSE;
	 SetIndex(x, y, imagePalette->GetPixel(r, g, b));

  } else {
	 ImagePointerType ImagePointer = RawImage + EffWidth*y + (x*Depth >> 3);
	 ImagePointer[0] = b;
	 ImagePointer[1] = g;
	 ImagePointer[2] = r;
  }

  return TRUE;
}

BOOL CImageImpl::SetPalette(CImagePalette* colourmap)
{
  if (!colourmap)
	 return FALSE;
  ColorType |= (COLORTYPE_PALETTE | COLORTYPE_COLOR);
  imagePalette = colourmap;
  return DibSetUsage(lpbi, (HPALETTE) (*imagePalette), CIMAGE_COLORS );
}

BOOL
CImageImpl::SetPalette(int n, byte *r, byte *g, byte *b)
{
  imagePalette = new CImagePalette();
  if (!imagePalette)
	 return FALSE;

  if (!g) g = r;
  if (!b) b = g;
  imagePalette->Create(n, r, g, b);
  ColorType |= (COLORTYPE_PALETTE | COLORTYPE_COLOR);
  return DibSetUsage(lpbi, (HPALETTE) (*imagePalette), CIMAGE_COLORS );
}

BOOL
CImageImpl::SetPalette(int n, rgb_color_struct *rgb_struct)
{
  if (imagePalette)
    AfxMessageBox("CImageImpl::imagePalette already exists!");
    
  imagePalette = new CImagePalette();
  if (!imagePalette)
	 return FALSE;

  byte r[256], g[256], b[256];

  for(int i=0; i<n; i++)
  {
	 r[i] = rgb_struct[i].red;
	 g[i] = rgb_struct[i].green;
	 b[i] = rgb_struct[i].blue;
  }
  imagePalette->Create(n, r, g, b);
  ColorType |= (COLORTYPE_PALETTE | COLORTYPE_COLOR);
  return DibSetUsage(lpbi, (HPALETTE) (*imagePalette), CIMAGE_COLORS );
}

BOOL CImageImpl::Draw(CDC *cdc, int x, int y, int dx, int dy, int xs, int ys)
{
  if (lpbi)
  {
	 HDC dc = cdc->GetSafeHdc();

	 if (dc)   {
		::RealizePalette(dc);
		if (dx==-1)  dx = GetWidth();
		if (dy==-1)  dy = GetHeight();

//		if (m_bTransparent)
//		{
			SetDIBitsToDevice(dc, x, y, dx, dy, xs, ys, 0, GetHeight(),
				RawImage, (BITMAPINFO *)lpbi, CIMAGE_COLORS );

/*			StretchDIBits(pDC, x, y, cx, cy, 0, 0, head.biWidth, head.biHeight,
				info.pImage,(BITMAPINFO*)hDib, DIB_RGB_COLORS,SRCCOPY);
			
			
/		}else{

			RECT rcDst = {x, y, x+dx, y+dy};
				HDC dcMem = CreateCompatibleDC(dc);
				HBITMAP bm = CreateCompatibleBitmap(dc, head.biWidth, head.biHeight);
				HGDIOBJ oldbm = SelectObject(dcMem, bm);
				SetStretchBltMode(dcMem,COLORONCOLOR);
				StretchDIBits(dcMem, 0, 0, head.biWidth, head.biHeight, 0, 0, head.biWidth, head.biHeight,
					info.pImage,(BITMAPINFO*)hDib,DIB_RGB_COLORS,SRCCOPY);
				
				RGBQUAD rgbBG;
				if (head.biBitCount<24) rgbBG = GetPaletteColor((BYTE)info.nBkgndIndex);
				else rgbBG = info.nBkgndColor;
				COLORREF crBG = RGB(rgbBG.rgbRed, rgbBG.rgbGreen, rgbBG.rgbBlue);
				
				// PREPARE MASK BEGIN
				HDC hdcDst = CreateCompatibleDC(pDC);
				HBITMAP mask = CreateBitmap(head.biWidth, head.biHeight, 1, 1, NULL);
				// Load the bitmaps into memory DC
				HGDIOBJ hbmDstT = SelectObject(hdcDst, mask);
				// Change the background to trans color
				COLORREF clrSaveBk = SetBkColor(dcMem, crBG);
				// This call sets up the mask bitmap.
				BitBlt(hdcDst,0,0,head.biWidth,head.biHeight,dcMem,0,0,SRCCOPY);
				// Now, we need to paint onto the original image, making
				// sure that the "transparent" area is set to black. What
				// we do is AND the monochrome image onto the color Image
				// first. When blitting from mono to color, the monochrome
				// pixel is first transformed as follows:
				// if  1 (black) it is mapped to the color set by SetTextColor().
				// if  0 (white) is is mapped to the color set by SetBkColor().
				// Only then is the raster operation performed.
				COLORREF clrSaveDstText = SetTextColor(dcMem, RGB(255,255,255));
				SetBkColor(dcMem, RGB(0,0,0));
				BitBlt(dcMem,0,0,head.biWidth,head.biHeight,hdcDst,0,0,SRCAND);
				// Clean up by deselecting any objects, and delete the DC's.
				SetTextColor(hdcDst, clrSaveDstText);
				SetBkColor(dcMem, clrSaveBk);
				SelectObject(hdcDst, hbmDstT);
				DeleteDC(hdcDst); hdcDst = NULL;
				// PREPARE MASK END
				DrawTransparentBitmap(pDC, mask,	// The destination DC.
					x, y,		// Where to draw
					cx,cy,		// Width & Height
					dcMem,		// the DC holding the bmp
					head.biWidth,
					head.biHeight);
				SelectObject(dcMem, oldbm); 
				DeleteDC(dcMem);
				DeleteObject(mask);
				DeleteObject(bm);
			}
		}*/

	 }
	 return TRUE;
  }
  else
	 return FALSE;

}

BOOL CImageImpl::Stretch(CDC *cdc, int xd, int yd, int dxd, int dyd,
	 int xs, int ys, int dxs, int dys)
{
  if (lpbi)
  {
	HDC dc =  cdc->GetSafeHdc();

	 if (dc)   {
		if (dxd==-1) dxd = GetWidth();
		if (dyd==-1) dyd = GetHeight();
		if (dxs==-1) dxs = Width - xs;
		if (dys==-1) dys = Height - ys;
		::RealizePalette(dc);

#ifdef WIN32       // Paul Shirley's patch
		SetStretchBltMode(dc, COLORONCOLOR);
#else
		SetStretchBltMode(dc, STRETCH_DELETESCANS);
#endif

	  if (bgindex == -1)
	  {
			StretchDIBits(dc, xd, yd, dxd, dyd, xs, ys, dxs, dys,
				RawImage, (BITMAPINFO *)lpbi, CIMAGE_COLORS, SRCCOPY);
		}
		else
		{
			//HDC tmpdc = CreateCompatibleDC(dc);
			//HBITMAP tmpbmp = CreateCompatibleBitmap(dc, dxd, dyd);
			//SelectObject(tmpdc, tmpbmp);
			//BitBlt(tmpdc, 0, 0, dxd, dyd, dc, xd, yd, SRCCOPY);

			unsigned char* save_index = new unsigned char[Height*Width];
			int black = imagePalette->GetPixel(0, 0, 0);
			int white = imagePalette->GetPixel(255, 255, 255);

			// Make background white and foreground black to use as mask...

			int x, y;

			for (y = 0; y < Height; y++)
				for (x = 0; x < Width; x++)
				{
					long i = ((long)y*Width)+x;
					save_index[i] = (unsigned char)GetIndex(x, y);

					if (GetIndex(x, y) == bgindex)
						SetIndex(x, y, white);
					else
						SetIndex(x, y, black);
				}

			// AND into the canvas, this will preserve the background but
			// turn the foreground white...

			//StretchDIBits(tmpdc, 0, 0, dxd, dyd, xs, ys, dxs, dys,
			StretchDIBits(dc, xd, yd, dxd, dyd, xs, ys, dxs, dys,
				RawImage, (BITMAPINFO *)lpbi, CIMAGE_COLORS, SRCAND);

			// Restore foreground & make background black...

			for (y = 0; y < Height; y++)
				for (x = 0; x < Width; x++)
				{
					long i = ((long)y*Width)+x;
					SetIndex(x, y, save_index[i]);

					if (GetIndex(x, y) == bgindex)
						SetIndex(x, y, black);
				}

			// OR into the canvas, this will preserve the background but
			// copy the foreground...

			//StretchDIBits(tmpdc, 0, 0, dxd, dyd, xs, ys, dxs, dys,
			StretchDIBits(dc, xd, yd, dxd, dyd, xs, ys, dxs, dys,
				RawImage, (BITMAPINFO *)lpbi, CIMAGE_COLORS, SRCPAINT);

			for (y = 0; y < Height; y++)
				for (x = 0; x < Width; x++)
				{
					long i = ((long)y*Width)+x;
					SetIndex(x, y, save_index[i]);
				}

			delete [] save_index;

			//BitBlt(dc, xd, yd, dxd, dyd, tmpdc, 0, 0, SRCCOPY);
			//DeleteDC(tmpdc);
			//DeleteObject(tmpbmp);
		}
	 }
	 return TRUE;
  }
  else
	 return FALSE;
}

void CImageImpl::TransferBits(CImageImpl *from)
{
  if (lpbi)
  {
    GlobalFreePtr(lpbi);
    lpbi = NULL;
  }
  if (imagePalette)
  {
    delete imagePalette;
    imagePalette = NULL;
  }

  lpbi = from->lpbi;
  bgindex = from->bgindex;
  imagePalette = from->imagePalette;
  RawImage = from->RawImage;
  Width = from->Width;
  Height = from->Height;
  Depth = from->Depth;
  ColorType = from->ColorType;
  EffWidth = from->EffWidth;
  
  from->RawImage = NULL;
  from->lpbi = NULL;
  from->imagePalette = NULL;
}
