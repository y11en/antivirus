//////////////////////////////////////////////////////////////////////
// Png.cpp
//
// PNG
// 
// Copyright (c) 2005
//
//////////////////////////////////////////////////////////////////////

#include "pngimg.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////////
// CImagePng

CImagePng::CImagePng()
{
	m_pDib = m_pAlpha = NULL;
	memset(&m_head, 0, sizeof(BITMAPINFOHEADER));
	memset(&m_info, 0, sizeof(CIMAGEBINFO));
	m_szSize.cx = m_szSize.cy = 0;
	m_info.nAlphaMax	= 255;
	m_info.nBkgndIndex	= -1;
	m_hTmpDC = NULL;
	m_hIcon = NULL;
	m_bgPix = -1;
	SetXDPI(96);
	SetYDPI(96);
	m_bMosaic = false;
}

CImagePng::~CImagePng()
{
	Clear();
}

void CImagePng::Clear()
{
	if( m_hIcon )
		::DestroyIcon(m_hIcon), m_hIcon = NULL;

	if( m_hTmpDC )
		::DeleteDC(m_hTmpDC), m_hTmpDC = NULL;

	AlphaDelete();
		
	if( m_pDib )
		free(m_pDib), m_pDib = NULL;
}

bool CImagePng::Init(void* pData, long nDataSize, long mImageSize, bool bMozaic)
{
	Clear();

	png_struct * png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if( !png_ptr )
		return false;

	png_info * info_ptr = png_create_info_struct(png_ptr);
	if( !info_ptr )
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	BYTE * row_pointers = NULL;
	
	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/
	if( setjmp(png_ptr->jmpbuf) )
	{
		if( row_pointers ) delete [] row_pointers;
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		Clear();
		return false;
	}

	png_set_read_fn(png_ptr, pData, (png_rw_ptr)user_read_fn);
	
	png_read_info(png_ptr, info_ptr);

	/* allocate the memory to hold the image using the fields of png_info. */
	png_color_16 my_background={ 0, 192, 192, 192, 0 };
	png_color_16 *image_background;

	if( info_ptr->pixel_depth != 32 )
	{
		if( png_get_bKGD(png_ptr, info_ptr, &image_background) )
			png_set_background(png_ptr, image_background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		else
			png_set_background(png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
	}
	else
	{
		if( png_get_bKGD(png_ptr, info_ptr, &image_background) )
		{
			m_info.nBkgndColor.rgbRed   = (BYTE)image_background->red;
			m_info.nBkgndColor.rgbGreen = (BYTE)image_background->green;
			m_info.nBkgndColor.rgbBlue  = (BYTE)image_background->blue;
			m_info.nBkgndColor.rgbReserved = 0;	// <vho>
		}
	}

	/* tell libpng to strip 16 bit depth files down to 8 bits */
	if( info_ptr->bit_depth == 16 )
		png_set_strip_16(png_ptr);

	int pixel_depth=info_ptr->pixel_depth;
	if( pixel_depth >  16 ) pixel_depth = 24;
	if( pixel_depth == 16 ) pixel_depth = 8;

	Create(info_ptr->width, info_ptr->height, pixel_depth);

	/* get metrics */
	switch(info_ptr->phys_unit_type)
	{
	case PNG_RESOLUTION_UNKNOWN:
		SetXDPI(info_ptr->x_pixels_per_unit);
		SetYDPI(info_ptr->y_pixels_per_unit);
		break;
	case PNG_RESOLUTION_METER:
		SetXDPI(MulDiv(info_ptr->x_pixels_per_unit, 254, 10000));
		SetYDPI(MulDiv(info_ptr->y_pixels_per_unit, 254, 10000));
		break;
	}

	if( info_ptr->num_palette > 0 )
		SetPalette((rgb_color*)info_ptr->palette, info_ptr->num_palette);
	else if( info_ptr->bit_depth == 2 )
	{ 
		SetPaletteColor(0, 0, 0, 0);
		SetPaletteColor(1, 85, 85, 85);
		SetPaletteColor(2, 170, 170, 170);
		SetPaletteColor(3, 255, 255, 255);
	} 
	else 
		SetGrayPalette();

	// simple transparency (the real PGN transparency is more complex)
	if( info_ptr->num_trans != 0 )
	{
		//palette transparency
		RGBQUAD * pal = GetPalette();
		if( pal )
		{
			unsigned long ip;
			for(ip = 0; ip < min(m_head.biClrUsed, (unsigned long)info_ptr->num_trans); ip++)
				pal[ip].rgbReserved = info_ptr->trans[ip];

			if( info_ptr->num_trans == 1 && pal[0].rgbReserved == 0 )
			{
				m_info.nBkgndIndex = 0;
			}
			else
			{
				m_info.bAlphaPaletteEnabled = true;
				for(; ip < m_head.biClrUsed; ip++)
					pal[ip].rgbReserved = 0xff;
			}
		}
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA || //Alpha channel
		(info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA && info_ptr->pixel_depth == 32 ) )
	{
		if(info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			png_set_gray_to_rgb(png_ptr);
			png_set_expand(png_ptr);
		}

		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
		AlphaCreate();

//		png_set_strip_alpha(png_ptr);
	}

	// flip the RGB pixels to BGR (or RGBA to BGRA)
	if( info_ptr->color_type & PNG_COLOR_MASK_COLOR ) 
		png_set_bgr(png_ptr);

	//allocate the buffer
	int row_stride = info_ptr->width * ((info_ptr->pixel_depth+7)>>3 );
	row_pointers = new BYTE[10 + row_stride];

	// turn on interlace handling
	int number_passes = png_set_interlace_handling(png_ptr);

	CImageIteratorEx iter(this);
	for(int pass = 0; pass < number_passes; pass++)
	{
		iter.Upset();
		int y = 0;
		do
		{
			if( !AlphaIsValid() )
			{
				//recover data from previous scan
				if( info_ptr->interlace_type && pass > 0 )
					iter.GetRow(row_pointers, m_info.dwEffWidth);
				//read next row
				png_read_row(png_ptr, row_pointers, NULL);

				if( info_ptr->bit_depth == 2 && pass == (number_passes - 1) )
					expand2to4bpp(row_pointers);
				//copy the pixels
				iter.SetRow(row_pointers, m_info.dwEffWidth);
				//go on
				iter.PrevRow();
			}
			else //alpha blend
			{
				//compute the correct position of the line
				long ax, ay;
				ay = m_head.biHeight - 1 - y;
				BYTE* prow = iter.GetRow(ay);

				//recover data from previous scan
				if( info_ptr->interlace_type && pass > 0 && pass != 7 )
				{
					for(ax = m_head.biWidth - 1; ax >=0; ax--)
					{
						row_pointers[ax*4]	= prow[3*ax];
						row_pointers[ax*4+1]= prow[3*ax+1];
						row_pointers[ax*4+2]= prow[3*ax+2];
						row_pointers[ax*4+3]= AlphaGet(ax, ay);
					}
				}
				//read next row
				png_read_row(png_ptr, row_pointers, NULL);
				//RGBA -> RGB + A
				for(ax=0; ax < m_head.biWidth; ax++)
				{
					prow[3*ax]	= row_pointers[ax*4];
					prow[3*ax+1]= row_pointers[ax*4+1];
					prow[3*ax+2]= row_pointers[ax*4+2];
					AlphaSet(ax, ay, row_pointers[ax*4+3]);
				}
			}
			y++;
		}
		while( y < m_head.biHeight);
	}
	delete [] row_pointers, row_pointers = NULL;

	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	m_szSize.cx = m_head.biWidth;
	m_szSize.cy = m_head.biHeight;
	if( mImageSize )
		m_szSize.cx = mImageSize;
	else
		m_bMosaic = bMozaic;

	return true;
}

bool CImagePng::IsValid()
{
	return !!m_pDib;
}

bool CImagePng::IsTransparent()
{
	bool bTransparent = m_info.nBkgndIndex != -1;
	return bTransparent || m_pAlpha || m_info.bAlphaPaletteEnabled;
}

bool CImagePng::Draw(HDC hdc, RECT& rcFrame, CBgContext* pCtx, tDWORD nIdx)
{
	if( !m_pDib || !hdc )
		return false;

	if( m_bMosaic )
	{
		if( nIdx == -1 )
			nIdx = 0;
		else
		{
			RECT rcNew = rcFrame;
			while( rcNew.left < rcFrame.right )
			{
				rcNew.right = rcNew.left + m_head.biWidth;
				if( rcNew.right > rcFrame.right )
					rcNew.right = rcFrame.right;
				Draw(hdc, rcNew, pCtx, -1);
				rcNew.left = rcNew.right;
			}
			return true;
		}
	}

	if( pCtx && !nIdx )
		nIdx = pCtx->m_nCurFrame;

	int nCount = Count();
	if( !nCount || nIdx && nIdx >= Count() )
		return false;

	RECT rc = rcFrame, rcDraw = rcFrame;
	DWORD dwRes = ::GetClipBox(hdc, &rcDraw);
	if( dwRes == SIMPLEREGION || dwRes == COMPLEXREGION )
		IntersectRect(&rc, &rcFrame, &rcDraw);

	long x = rc.left, cx = rc.right - rc.left, y = rc.top, cy = rc.bottom - rc.top;
	if( !cx || !cy )
		return false;

	RECT rcSrc;
	rcSrc.left = rc.left - rcFrame.left;
	rcSrc.top = rc.top - rcFrame.top;
	rcSrc.right = m_head.biWidth - rc.right;
	rcSrc.bottom = m_head.biHeight - rc.bottom;

	::OffsetRect(&rcSrc, m_szSize.cx*nIdx, 0);

	long maxWidth = m_head.biWidth*(nIdx+1)/nCount;
	if( cx + rcSrc.left > maxWidth )  cx = maxWidth - rcSrc.left;
	if( cy + rcSrc.top > m_head.biHeight ) cy = m_head.biHeight - rcSrc.top;

	if( 0/*!!!!!!!!!!!!! !IsTransparent() temporary commented !!!!!!!!!!!!!*/ )
	{
		::SetStretchBltMode(hdc, COLORONCOLOR);

		BYTE *ppix = m_info.pImage + (m_head.biHeight - rc.bottom)*m_info.dwEffWidth;
		::SetDIBitsToDevice(hdc, x, y, cx, cy,
			rcSrc.left, 0, 0, cy, ppix, (BITMAPINFO *)m_pDib, DIB_RGB_COLORS);
		return true;
	}
	
	// draw image with transparent/alpha blending
	RGBQUAD c = {0, 0, 0, 0 };
	long cit = GetTransIndex();
	long ct= *(long*)&GetTransColor();
	long ci;

	//Preparing Bitmap Info
	BITMAPINFO bmInfo; memset(&bmInfo.bmiHeader, 0, sizeof(bmInfo.bmiHeader));
	bmInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth    = cx;
	bmInfo.bmiHeader.biHeight   = cy;
	bmInfo.bmiHeader.biPlanes   = 1;
	bmInfo.bmiHeader.biBitCount = 24;

	BYTE *pbase;	//points to the final dib
	BYTE *pdst;		//current pixel from pbase
	BYTE *ppix;		//current pixel from image

	//get the background
	GdiFlush();
	HBITMAP hTmpBmp = ::CreateDIBSection(hdc, &bmInfo, DIB_RGB_COLORS, (void**)&pbase, 0, 0);
	if( !hTmpBmp )
		return false;

	if( !m_hTmpDC )
	{
		HDC gDC = ::GetDC(NULL);
		m_hTmpDC = ::CreateCompatibleDC(gDC);
		::ReleaseDC(NULL, gDC);
	}

	HGDIOBJ hTmpObj = ::SelectObject(m_hTmpDC, hTmpBmp);
	::BitBlt(m_hTmpDC, 0, 0, cx, cy, hdc, x, y, SRCCOPY);

	long xx, yy, alphaoffset, ix, iy;
	BYTE a, a1;
	long ew = ((((24 * cx) + 31) / 32) * 4);

	iy = m_head.biHeight - cy - rcSrc.top;
	for(yy = 0; yy < cy; yy++, iy++)
	{
		alphaoffset = iy*m_head.biWidth;
		ix = rcSrc.left;
		pdst = pbase + yy*ew;
		ppix = m_info.pImage + iy*m_info.dwEffWidth + ix*3;
		
		for(xx = 0; xx < cx; xx++, ix++)
		{
			a = ( m_pAlpha ) ? m_pAlpha[alphaoffset+ix] : 255;
			a = (BYTE)((a*(1 + m_info.nAlphaMax))>>8);

			if( m_head.biClrUsed )
			{
				ci = GetPixelIndex(ix, iy);
				c = GetPaletteColor((BYTE)ci);
				if( m_info.bAlphaPaletteEnabled )
					a = (BYTE)((a*(1+c.rgbReserved))>>8);
			}
			else
			{
				c.rgbBlue = *ppix++;
				c.rgbGreen= *ppix++;
				c.rgbRed  = *ppix++;
			}

			if( pCtx && pCtx->m_bGrayed )
			{
				BYTE col = (BYTE)(((DWORD)c.rgbBlue + c.rgbGreen + c.rgbRed)/3);
				col = (col < 255 - 20) ? col + 20 : 255;
				c.rgbBlue = c.rgbGreen = c.rgbRed = col;
			}

			if( (m_head.biClrUsed && ci!=cit) ||
					(!m_head.biClrUsed && *(long*)&c!=ct) || (m_info.nBkgndIndex == -1) )
			{
				if( a == 0 ) // Transparent, retain dest 
					pdst += 3; 
				else if (a == 255) // opaque, ignore dest 
				{	
					if( m_bgPix == -1 )
					{
						m_bgPix = RGB(c.rgbRed, c.rgbGreen, c.rgbBlue);
						m_bgPt.x = ix;
						m_bgPt.y = iy;
					}

					*pdst++= c.rgbBlue; 
					*pdst++= c.rgbGreen; 
					*pdst++= c.rgbRed; 
				}
				else
				{				// semi transparent 
					a1=(BYTE)~a;
					*pdst++=(BYTE)((*pdst * a1 + a * c.rgbBlue)>>8); 
					*pdst++=(BYTE)((*pdst * a1 + a * c.rgbGreen)>>8); 
					*pdst++=(BYTE)((*pdst * a1 + a * c.rgbRed)>>8); 
				} 
			}
			else
				pdst += 3;
		}
	}
	
	//paint the image & cleanup
	::SetDIBitsToDevice(hdc, x, y, cx, cy,
		0, 0, 0, cy, pbase, &bmInfo, 0);
	::DeleteObject(::SelectObject(m_hTmpDC, hTmpObj));
	return true;
}

HICON CImagePng::IconHnd()
{
	if( m_hIcon )
		return m_hIcon;

	// маска прозрачности, кол-во байт на одну строку (word aligned for CreateBitmap)
	long mw = (((m_head.biWidth + 15) / 16) * 2);
	// 24-битный цвет, кол-во байт на одну строку (dword aligned for CreateBitmap)
	long ew = ((((24 * m_head.biWidth) + 31) / 32) * 4);

	long size_line = mw * m_head.biHeight;
	BYTE *pmask = new BYTE[size_line], *pcolor = NULL;
	memset(pmask, 0x00, size_line);

	BITMAPINFO bmInfo;
	memset(&bmInfo.bmiHeader, 0, sizeof(bmInfo.bmiHeader));
	bmInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth    = m_head.biWidth;
	bmInfo.bmiHeader.biHeight   = m_head.biHeight;
	bmInfo.bmiHeader.biPlanes   = 1;
	bmInfo.bmiHeader.biBitCount = 24;

	ICONINFO ii;
	ii.fIcon = 1;
	ii.xHotspot = m_head.biWidth/2;
	ii.yHotspot = m_head.biHeight/2;
	ii.hbmColor = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void **)&pcolor, NULL, 0); 
	if( !pcolor )
		return NULL;

	memset(pcolor, 0x0, ew*m_head.biHeight);

	RGBQUAD c = {0, 0, 0, 0 };
	RGBQUAD ct = GetTransColor();
	long* pc = (long*)&c;
	long* pct= (long*)&ct;
	long cit = GetTransIndex();
	long ci, bit, yy, xx;

	for(yy = 0; yy < m_head.biHeight; yy++)
	{
		BYTE* pbyte = pmask + (m_head.biHeight-yy-1)*mw;
		BYTE* pdst = pcolor + yy*ew;
		BYTE* ppix = m_info.pImage + yy*m_info.dwEffWidth;

		for(xx = 0, bit = 0; xx < m_head.biWidth; xx++)
		{
			BYTE a = ( m_pAlpha ) ? m_pAlpha[yy*m_head.biWidth+xx] : 255;
			a = (BYTE)((a*(1 + m_info.nAlphaMax))>>8);

			if( m_head.biClrUsed )
			{
				ci = GetPixelIndex(xx, yy);
				c = GetPaletteColor((BYTE)ci);
				if( m_info.bAlphaPaletteEnabled )
					a = (BYTE)((a*(1+c.rgbReserved))>>8);
			}
			else
			{
				c.rgbBlue = *ppix++;
				c.rgbGreen= *ppix++;
				c.rgbRed  = *ppix++;
			}

			if( (m_head.biClrUsed && ci!=cit) || (!m_head.biClrUsed && *pc!=*pct) || (m_info.nBkgndIndex == -1) )
			{
				if (a == 255) // opaque, ignore dest 
				{	
					*pdst++= c.rgbBlue; 
					*pdst++= c.rgbGreen; 
					*pdst++= c.rgbRed; 
				}
				else if( a )
				{				// semi transparent 
					BYTE a1=(BYTE)~a;
					*pdst++=(BYTE)((255 * a1 + a * c.rgbBlue)>>8); 
					*pdst++=(BYTE)((255 * a1 + a * c.rgbGreen)>>8); 
					*pdst++=(BYTE)((255 * a1 + a * c.rgbRed)>>8); 
				} 
			}
			else
				a = 0;


			if( !a )
			{
				pdst+=3;
				*pbyte |= (1 << (7-bit));
			}

			if( ++bit == 8 )
			{
				pbyte++;
				bit = 0;
			}
		}
	}

	ii.hbmMask = CreateBitmap(m_head.biWidth, m_head.biHeight, 1, 1, pmask);
	delete[] pmask;

	m_hIcon = CreateIconIndirect(&ii);
	DeleteObject(ii.hbmColor);
	DeleteObject(ii.hbmMask);
	return m_hIcon;
}

void * CImagePng::Create(long dwWidth, long dwHeight, long wBpp)
{
	if( !dwWidth || !dwHeight )
		return NULL;

    // Make sure bits per pixel is valid
    if		(wBpp <= 1)	wBpp = 1;
    else if (wBpp <= 4)	wBpp = 4;
    else if (wBpp <= 8)	wBpp = 8;
    else				wBpp = 24;

	// set the correct bpp value
    switch(wBpp)
	{
    case 1:  m_head.biClrUsed = 2;	 break;
    case 4:  m_head.biClrUsed = 16;  break;
    case 8:  m_head.biClrUsed = 256; break;
    default: m_head.biClrUsed = 0;
    }

	//set the common image informations
    m_info.dwEffWidth = ((((wBpp * dwWidth) + 31) / 32) * 4);

    // initialize BITMAPINFOHEADER
	m_head.biSize = sizeof(BITMAPINFOHEADER); 
    m_head.biWidth			= dwWidth;			// fill in width from parameter
    m_head.biHeight			= dwHeight;			// fill in height from parameter
    m_head.biPlanes			= 1;				// must be 1
    m_head.biBitCount		= (WORD)wBpp;		// from parameter
    m_head.biCompression	= BI_RGB;    
    m_head.biSizeImage		= m_info.dwEffWidth * dwHeight;
    m_head.biClrImportant	= 0;

	m_pDib = malloc(GetSize()); // alloc memory block to store our bitmap
    if( !m_pDib )
		return NULL;

	//clear the palette
	RGBQUAD* pal=GetPalette();
	if( pal )
		memset(pal, 0, GetPaletteSize());

    // use our bitmap info structure to fill in first part of
    // our DIB with the BITMAPINFOHEADER
    BITMAPINFOHEADER* lpbi = (BITMAPINFOHEADER*)(m_pDib);
    *lpbi = m_head;

	m_info.pImage = GetBits();
    return m_pDib;
}

void CImagePng::SetXDPI(long dpi)
{
	if ( dpi <= 0 ) dpi = 96;
	m_info.xDPI = dpi;
	m_head.biXPelsPerMeter = MulDiv(dpi, 10000, 254);
}

void CImagePng::SetYDPI(long dpi)
{
	if ( dpi <= 0) dpi = 96;
	m_info.yDPI = dpi;
	m_head.biYPelsPerMeter = MulDiv(dpi, 10000, 254);
}

void CImagePng::SetPalette(rgb_color *rgb,long nColors)
{
	if( !rgb || NULL == m_pDib || 0== m_head.biClrUsed )
		return;
	RGBQUAD* ppal = GetPalette();
	unsigned long m = min((unsigned long)nColors, m_head.biClrUsed);
	for (unsigned long i = 0; i < m; i++)
	{
		ppal[i].rgbRed		= rgb[i].r;
		ppal[i].rgbGreen	= rgb[i].g;
		ppal[i].rgbBlue		= rgb[i].b;
	}
	m_info.last_c_isvalid = false;
}

void CImagePng::CopyPalette(CImagePng* pFrom)
{
	if (!pFrom)
		return;

	if( !m_pDib || !m_head.biClrUsed )
		return;

	unsigned long m = min(pFrom->m_head.biClrUsed, m_head.biClrUsed);
	memcpy(GetPalette(), pFrom->GetPalette(), m*sizeof(RGBQUAD));
	m_info.last_c_isvalid = false;
}

void CImagePng::SetPaletteColor(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha)
{
	if ( m_pDib && m_head.biClrUsed )
	{
		BYTE* iDst = (BYTE*)(m_pDib) + sizeof(BITMAPINFOHEADER);
		if ( idx < m_head.biClrUsed ){
			long ldx=idx*sizeof(RGBQUAD);
			iDst[ldx++] = (BYTE) b;
			iDst[ldx++] = (BYTE) g;
			iDst[ldx++] = (BYTE) r;
			iDst[ldx] = (BYTE) alpha;
			m_info.last_c_isvalid = false;
		}
	}
}

void CImagePng::SetGrayPalette()
{
	if( NULL == m_pDib || 0 == m_head.biClrUsed )
		return;
	RGBQUAD* pal = GetPalette();
	for (unsigned long ni = 0; ni < m_head.biClrUsed; ni++)
		pal[ni].rgbBlue = pal[ni].rgbGreen = pal[ni].rgbRed = (BYTE)(ni*(255/(m_head.biClrUsed-1)));
}

RGBQUAD * CImagePng::GetPalette() const
{
	if ( m_pDib && m_head.biClrUsed )
		return (RGBQUAD*)((BYTE*)m_pDib + sizeof(BITMAPINFOHEADER));
	return NULL;
}

long CImagePng::GetPaletteSize()
{
	return (m_head.biClrUsed * sizeof(RGBQUAD));
}

void CImagePng::AlphaCreate()
{
	if( !m_pAlpha )
	{
		m_pAlpha = (BYTE*)malloc(m_head.biWidth * m_head.biHeight);
		if( m_pAlpha ) 
			memset(m_pAlpha, 255, m_head.biWidth * m_head.biHeight);
	}
}


void CImagePng::AlphaDelete()
{
	if( m_pAlpha ) 
		free(m_pAlpha), m_pAlpha = NULL; 
}

bool CImagePng::AlphaIsValid()
{
	return m_pAlpha != 0;
}

BYTE CImagePng::AlphaGet(const long x, const long y)
{
	return ( m_pAlpha && IsInside(x,y) ) ? m_pAlpha[x + y*m_head.biWidth] : 0;
}

void CImagePng::AlphaSet(const long x, const long y, const BYTE level)
{
	if( m_pAlpha && IsInside(x,y) )
		m_pAlpha[x + y*m_head.biWidth] = level;
}

long CImagePng::GetSize()
{
	return m_head.biSize + m_head.biSizeImage + GetPaletteSize();
}

long CImagePng::GetHeight() const
{
	return m_head.biHeight;
}

long CImagePng::GetEffWidth() const
{
	return m_info.dwEffWidth;
}

BYTE * CImagePng::GetBits(long row)
{
	if( !m_pDib )
		return NULL;

	if( !row )
		return ((BYTE*)m_pDib + *(long*)m_pDib + GetPaletteSize());

	if( row < (long)m_head.biHeight )
		return ((BYTE*)m_pDib + *(long*)m_pDib + GetPaletteSize() + (m_info.dwEffWidth * row));
	return NULL;
}

bool CImagePng::IsInside(long x, long y)
{
	return (0 <= y && y < m_head.biHeight && 0 <= x && x < m_head.biWidth);
}

RGBQUAD	CImagePng::GetTransColor()
{
	if ( m_head.biBitCount < 24 && m_info.nBkgndIndex != -1 ) 
		return GetPaletteColor((BYTE)m_info.nBkgndIndex);
	return m_info.nBkgndColor;
}

long CImagePng::GetTransIndex() const
{
	return m_info.nBkgndIndex;
}

BYTE CImagePng::GetPixelIndex(long x,long y)
{
	if (( m_pDib == NULL )|| ( m_head.biClrUsed == 0 )) 
		return 0;

	if( ( x < 0 )||( y < 0 )||( x >= m_head.biWidth )||( y >= m_head.biHeight ) )
	{
		if( m_info.nBkgndIndex != -1 )
			return (BYTE)m_info.nBkgndIndex;
		else 
			return *m_info.pImage;
	}
	if ( m_head.biBitCount == 8 ){
		return m_info.pImage[y*m_info.dwEffWidth + x];
	}
	else
	{
		BYTE pos;
		BYTE iDst = m_info.pImage[y*m_info.dwEffWidth + (x*m_head.biBitCount >> 3)];
		if ( m_head.biBitCount == 4 ){
			pos = (BYTE)(4*(1-x%2));
			iDst &= (0x0F<<pos);
			return (BYTE)(iDst >> pos);
		} else if ( m_head.biBitCount == 1 ){
			pos = (BYTE)(7-x%8);
			iDst &= (0x01<<pos);
			return (BYTE)(iDst >> pos);
		}
	}
	
	return 0;
}

RGBQUAD CImagePng::GetPaletteColor(BYTE idx)
{
	RGBQUAD rgb = {0,0,0,0};
	if ( m_pDib && m_head.biClrUsed ){
		BYTE* iDst = (BYTE*)(m_pDib) + sizeof(BITMAPINFOHEADER);
		if ( idx < m_head.biClrUsed ){
			long ldx = idx*sizeof(RGBQUAD);
			rgb.rgbBlue		= iDst[ldx++];
			rgb.rgbGreen	= iDst[ldx++];
			rgb.rgbRed		= iDst[ldx++];
			rgb.rgbReserved = iDst[ldx];
		}
	}
	return rgb;
}

void CImagePng::expand2to4bpp(BYTE* prow)
{
	BYTE *psrc,*pdst;
	BYTE pos,idx;
	for(long x = m_head.biWidth - 1; x >= 0; x--)
	{
		psrc = prow + ((2*x)>>3);
		pdst = prow + ((4*x)>>3);
		pos = (BYTE)(2*(3-x%4));
		idx = (BYTE)((*psrc & (0x03<<pos))>>pos);
		pos = (BYTE)(4*(1-x%2));
		*pdst &= ~(0x0F<<pos);
		*pdst |= (idx & 0x0F)<<pos;
	}
}

bool CImagePng::CreateCompatibleImage(CImagePng* pFrom, long dwWidth, long dwHeight, long nWidth)
{
	if (!pFrom)
		return false;

	if (IsValid())
		return false;

	m_info = pFrom->m_info;

	Create(dwWidth, dwHeight, pFrom->m_head.biBitCount);

	SetXDPI(pFrom->m_info.xDPI);
	SetYDPI(pFrom->m_info.yDPI);

	if( pFrom->m_head.biClrUsed > 0 )
		CopyPalette(pFrom);
	else 
		SetGrayPalette();

	//empty 255 alpha
	if (pFrom->AlphaIsValid())
		AlphaCreate();

	m_szSize.cx = m_head.biWidth;
	m_szSize.cy = m_head.biHeight;

	if (nWidth)
		m_szSize.cx = nWidth;

	m_bMosaic = pFrom->m_bMosaic;
	return true;
}
//////////////////////////////////////////////////////////////////////
