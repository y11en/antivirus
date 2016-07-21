//////////////////////////////////////////////////////////////////////
// Png.cpp

#ifndef _PNG_H_
#define _PNG_H_

#include <ProductCore/basedefs.h>
#include <vector>

extern "C"
{
//#define PNG_SETJMP_NOT_SUPPORTED
#include "..\png\png.h"
}

//////////////////////////////////////////////////////////////////////
// CImagePng

class CImagePng : public CImageBase
{
public:
	CImagePng();
	~CImagePng();
	
	void     Clear();
	bool     Init(void* pData, long nDataSize, long mImageSize = 0, bool bMozaic = false);
	bool     IsValid();
	bool     IsTransparent();
	bool     Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx);
	SIZE     Size() { return m_szSize; }
	tDWORD   Count(){ return m_szSize.cx ? m_head.biWidth / m_szSize.cx : 0; }
	HICON    IconHnd();
	bool	 CreateCompatibleImage(CImagePng* pFrom, long dwWidth, long dwHeight, long nWidth = 0);

	BYTE     AlphaGet(const long x, const long y);
	void     AlphaSet(const long x, const long y, const BYTE level);


private:
	typedef struct
	{
		long	dwEffWidth;			///< long aligned scan line width
		unsigned char*	pImage;				///< THE IMAGE BITS
		long	nBkgndIndex;		///< used for GIF, PNG, MNG
		RGBQUAD nBkgndColor;		///< used for RGB transparency
		long	xDPI;				///< horizontal resolution
		long	yDPI;				///< vertical resolution
		BYTE	nAlphaMax;			///< max opacity (fade)
		bool	bAlphaPaletteEnabled; ///< true if alpha values in the palette are enabled.
		bool	last_c_isvalid;
	} CIMAGEBINFO;

	struct rgb_color { BYTE r,g,b; };

	class CImageIteratorEx
	{
	public:
		int         Itx, Ity;		// Counters
		BYTE *      m_IterImage;	//  Image pointer
		CImagePng * m_pima;
	
	public:
		CImageIteratorEx(CImagePng *image) : m_pima(image)
		{
			if( m_pima )
				m_IterImage = m_pima->GetBits();
			Itx = Ity = 0;
		}

		void Upset()
		{
			Itx = 0;
			Ity = m_pima->GetHeight()-1;
			m_IterImage = m_pima->GetBits() + m_pima->GetEffWidth()*(m_pima->GetHeight()-1);
		}
		
		void SetRow(BYTE *buf, int n)
		{
			if (n<0) n = (int)m_pima->GetEffWidth();
			else n = min(n,(int)m_pima->GetEffWidth());

			if( m_IterImage != NULL && buf != NULL && n > 0 )
				memcpy(m_IterImage, buf, n);
		}
		
		void GetRow(BYTE *buf, int n)
		{
			if( m_IterImage != NULL && buf != NULL && n > 0 )
				memcpy(buf, m_IterImage, n);
		}
		
		BYTE* GetRow(int n)
		{
			SetY(n);
			return m_IterImage;
		}
		
		BOOL NextRow()
		{
			if (++Ity >= (int)m_pima->GetHeight()) return 0;
			m_IterImage += m_pima->GetEffWidth();
			return 1;
		}
		
		BOOL PrevRow()
		{
			if (--Ity < 0) return 0;
			m_IterImage -= m_pima->GetEffWidth();
			return 1;
		}

		void SetY(int y)
		{
			if ((y < 0) || (y >= (int)m_pima->GetHeight())) return;
			Ity = y;
			m_IterImage = m_pima->GetBits() + m_pima->GetEffWidth()*y;
		}
	}; friend class CImageIteratorEx;

private:
	void *   Create(long dwWidth, long dwHeight, long wBpp);
	void     SetXDPI(long dpi);
	void     SetYDPI(long dpi);
	void     SetPalette(rgb_color *rgb, long nColors=256);
	void     SetPaletteColor(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha=0);
	void     SetGrayPalette();
	RGBQUAD* GetPalette() const;
	long	 GetPaletteSize();
	void     AlphaCreate();
	void     AlphaDelete();
	bool     AlphaIsValid();
	long	 GetSize();
	long     GetHeight() const;
	long     GetEffWidth() const;
	BYTE *	 GetBits(long row = 0);
	bool     IsInside(long x, long y);
	RGBQUAD  GetTransColor();
	long     GetTransIndex() const;
	BYTE     GetPixelIndex(long x,long y);
	RGBQUAD  GetPaletteColor(BYTE idx);
	void	 CopyPalette(CImagePng* pFrom);
	
	void     expand2to4bpp(BYTE* prow);

protected:
	static void user_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		memcpy(data, png_ptr->io_ptr, length);
		png_ptr->io_ptr = (png_bytep)png_ptr->io_ptr + length;
	}
public:
	void*				m_pDib;		// contains the header, the palette, the pixels
    BITMAPINFOHEADER    m_head;		// standard header
	CIMAGEBINFO			m_info;		// extended information

protected:
	BYTE*				m_pAlpha;	// alpha channel
	SIZE                m_szSize;
	HDC                 m_hTmpDC;
	HICON               m_hIcon;
	COLORREF            m_bgPix;
	POINT               m_bgPt;
	bool                m_bMosaic;
};

//////////////////////////////////////////////////////////////////////

#endif // _PNG_H_
