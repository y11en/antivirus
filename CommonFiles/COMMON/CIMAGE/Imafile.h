/*
 * File:	imafile.h
 * Purpose:	Declaration of the Platform Independent File Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 *
 */
#ifndef IMAFILEH
#define IMAFILEH

#include "cimageb.h"

class CFileImageImpl: public CImageImpl
{
protected:
  CString filename;
public:
  CFileImageImpl(): CImageImpl() {}
  CFileImageImpl(const CFileImageImpl* ima): CImageImpl(ima) { filename = ima->filename; }
  CFileImageImpl(const CString& imageFileName) { filename = imageFileName ; }
};

#endif
