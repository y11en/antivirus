/*
 * File:	ImaIter.h
 * Purpose:	Declaration of the Platform Independent Image Base Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 */
#if !defined(__ImaIter_h)
#define __ImaIter_h

#include "cimage.h"

class CImageIterator
{
friend CImageImpl;
friend CImage;
protected:
  int Itx, Ity;				// Counters
  int Stepx, Stepy;
  ImagePointerType IterImage;  		//  Image pointer
  CImageImpl *ima;
public:
// Constructors
  CImageIterator ( void );
  CImageIterator ( CImage *image );
  CImageIterator ( CImageImpl *imageImpl );
  operator CImageImpl* ();

// Iterators
  BOOL ItOK ();
  void Reset ();
  void Upset ();
  void SetRow(byte *buf, int n);
  void GetRow(byte *buf, int n);
  byte GetByte( ) { return IterImage[Itx]; }
  void SetByte(byte b) { IterImage[Itx] = b; }
  ImagePointerType GetRow(void);
  BOOL NextRow();
  BOOL PrevRow();
  BOOL NextByte();
  BOOL PrevByte();

  void SetSteps(int x, int y=0) {  Stepx = x; Stepy = y; }
  void GetSteps(int *x, int *y) {  *x = Stepx; *y = Stepy; }
  BOOL NextStep();
  BOOL PrevStep();

////////////////////////// AD - for interlace ///////////////////////////////
  void SetY(int y);
/////////////////////////////////////////////////////////////////////////////
};


inline
CImageIterator::CImageIterator(void)
{
  ima = 0;
  IterImage = 0;
  Itx = Ity = 0;
  Stepx = Stepy = 0;
}

inline
CImageIterator::CImageIterator(CImage *image): ima(image->GetImplementation())
{
  if (ima)
	 IterImage = ima->GetRawImage();
  Itx = Ity = 0;
  Stepx = Stepy = 0;
}

inline
CImageIterator::CImageIterator(CImageImpl *imageImpl): ima(imageImpl)
{
  if (ima)
	 IterImage = ima->GetRawImage();
  Itx = Ity = 0;
  Stepx = Stepy = 0;
}

inline
CImageIterator::operator CImageImpl* ()
{
  return ima;
}

inline
BOOL CImageIterator::ItOK ()
{
  if (ima)
	 return ima->Inside(Itx, Ity);
  else
	 return FALSE;
}


inline void CImageIterator::Reset()
{
  IterImage = ima->GetRawImage();
  Itx = Ity = 0;
}

inline void CImageIterator::Upset()
{
  Itx = 0;
  Ity = ima->GetHeight()-1;
  IterImage = ima->GetRawImage() + ima->GetEffWidth()*(ima->GetHeight()-1);
}

inline BOOL CImageIterator::NextRow()
{
  if (++Ity >= ima->GetHeight()) return 0;
  IterImage += ima->GetEffWidth();
  return 1;
}

inline BOOL CImageIterator::PrevRow()
{
  if (--Ity < 0) return 0;
  IterImage -= ima->GetEffWidth();
  return 1;
}

////////////////////////// AD - for interlace ///////////////////////////////
inline void CImageIterator::SetY(int y)
{
  if ((y < 0) || (y > ima->GetHeight())) return;
  Ity = y;
  IterImage = ima->GetRawImage() + ima->GetEffWidth()*y;
}

/////////////////////////////////////////////////////////////////////////////

inline void CImageIterator::SetRow(byte *buf, int n)
{
// Here should be bcopy or memcpy
  //_fmemcpy(IterImage, (void far *)buf, n);
  if (n<0)
	 n = ima->GetWidth();

  for (int i=0; i<n; i++) IterImage[i] = buf[i];
}

inline void CImageIterator::GetRow(byte *buf, int n)
{
  for (int i=0; i<n; i++) buf[i] = IterImage[i];
}

inline ImagePointerType CImageIterator::GetRow()
{
  return IterImage;
}

inline BOOL CImageIterator::NextByte()
{
  if (++Itx < ima->GetEffWidth())
	 return 1;
  else
	 if (++Ity < ima->GetHeight())
	 {
		IterImage += ima->GetEffWidth();
		Itx = 0;
		return 1;
	 } else
		return 0;
}

inline BOOL CImageIterator::PrevByte()
{
  if (--Itx >= 0)
	 return 1;
  else
	 if (--Ity >= 0)
	 {
		IterImage -= ima->GetEffWidth();
		Itx = 0;
		return 1;
	 } else
		return 0;
}

inline BOOL CImageIterator::NextStep()
{
  Itx += Stepx;
  if (Itx < ima->GetEffWidth())
	 return 1;
  else {
	 Ity += Stepy;
	 if (Ity < ima->GetHeight())
	 {
		IterImage += ima->GetEffWidth();
		Itx = 0;
		return 1;
	 } else
		return 0;
  }
}

inline BOOL CImageIterator::PrevStep()
{
  Itx -= Stepx;
  if (Itx >= 0)
	 return 1;
  else {       
	 Ity -= Stepy;
	 if (Ity >= 0 && Ity < ima->GetHeight())
	 {
		IterImage -= ima->GetEffWidth();
		Itx = 0;
		return 1;
	 } else
		return 0;
  }
}

#endif
