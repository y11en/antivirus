/*
* File:	ImaPNG.h
* Purpose:	Declaration of the PNG Image Class
* Author:	Alejandro Aguilar Sierra
* Created:	1995
* Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
*
*
*/
#if !defined(__ImaPNG_h)
#define __ImaPNG_h

#include "imafile.h"
#include "..\png\png.h"
#include "..\defs.h"

#if CIMAGE_SUPPORT_PNG
class COMMONEXPORT CImagePNG: public CImageImpl
{
protected:
	COLORREF	m_crBackGround;
	void RGB2BGR(BYTE *buffer, int length);
	BOOL Resource2Image(LPVOID res_mem);
	static void ReadResouce(png_structp png_ptr, png_bytep data, png_size_t length);
	
public:
	void SetBackground(COLORREF cr){m_crBackGround = cr;}
	bool LoadImage(HINSTANCE hInstance, HRSRC hPngRes, COLORREF bkColor = ::GetSysColor(COLOR_3DFACE));
	bool LoadImage(LPCTSTR szFile, COLORREF bkColor = ::GetSysColor(COLOR_3DFACE));

};

#endif

#endif

