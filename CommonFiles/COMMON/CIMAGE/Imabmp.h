/*
 * File:	imajpg.h
 * Purpose:	Declaration of the Platform Independent JPEG Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * This software is based in part on the work of the Independent JPEG Group.
 *
 */
#if !defined(__ImaBMP_h)
#define __ImaBMP_h

#include "imafile.h"

class CImageBMP: public CFileImageImpl
{
public:
  CImageBMP::CImageBMP( const CBitmap* bmp);
  CImageBMP(const CImageBMP* ima): CFileImageImpl(ima) {}
  CImageBMP(const CString& imageFileName): CFileImageImpl(imageFileName) {}
  ~CImageBMP() {};

  BOOL ReadFile(const CString& imageFileName="");
  BOOL SaveFile(const CString& imageFileName="");
};

#endif

