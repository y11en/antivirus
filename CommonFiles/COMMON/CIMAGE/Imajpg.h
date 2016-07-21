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
#if !defined(__ImaJPEG_h)
#define __ImaJPEG_h

#include "imafile.h"

#if CIMAGE_SUPPORT_JPEG
class CImageJPG: public CFileImageImpl
{
protected:
  int quality;
  void CreateGrayColourMap(int n);
public:
  CImageJPG(const CImageJPG* ima): CFileImageImpl(ima) { quality = 75; }
  CImageJPG(const CString& imageFileName ): CFileImageImpl(imageFileName) { quality = 75; }
  ~CImageJPG() {};

  BOOL ReadFile(const CString& imageFileName="");
  BOOL SaveFile(const CString& imageFileName="");

  void BGR2RGB(unsigned char* pBuffer,int iCount);
};

#endif

#endif

