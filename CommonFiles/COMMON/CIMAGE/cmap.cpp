/*
 * Colour map class
 *
 */
#include "stdafx.h"
#include <windowsx.h>
#include "cmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImagePalette::CImagePalette(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  Create(n, red, green, blue);
}

CImagePalette::CImagePalette(const CImagePalette *pal)
{
  int count = ((CImagePalette *)pal)->GetEntryCount();
  
 LOGPALETTE* logPal = (LOGPALETTE*)
		 new BYTE[sizeof(LOGPALETTE) + count*sizeof(PALETTEENTRY)];
  logPal->palVersion = 0x300;
  logPal->palNumEntries = count;
  pal->GetPaletteEntries(0, count, logPal->palPalEntry);
  
  CPalette::CreatePalette(logPal);
  
  delete logPal;
}

CImagePalette::~CImagePalette(void)
{
  HPALETTE pal = (HPALETTE) (*this);
  Detach();
  ::DeleteObject(pal);
}

BOOL CImagePalette::Create(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue)
{
  if ((HPALETTE) *this)
    return FALSE;
    
  NPLOGPALETTE npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) + 
                        (WORD)n * sizeof(PALETTEENTRY));
  if (!npPal)
    return(FALSE);

  npPal->palVersion = 0x300;
  npPal->palNumEntries = n;

  int i;
  for (i = 0; i < n; i ++)
  {
    npPal->palPalEntry[i].peRed = red[i];
    npPal->palPalEntry[i].peGreen = green[i];
    npPal->palPalEntry[i].peBlue = blue[i];
    npPal->palPalEntry[i].peFlags = 0;
  }
  HPALETTE ms_palette = ::CreatePalette((LPLOGPALETTE)npPal);
  LocalFree((HANDLE)npPal);
  Attach(ms_palette);
  
  return TRUE;
}

int CImagePalette::GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  return ::GetNearestPaletteIndex((HPALETTE) (*this), RGB(red, green, blue));
}

BOOL CImagePalette::GetRGB(const int index, unsigned char *red, unsigned char *green, unsigned char *blue)
{
  if (index < 0 || index > 255)
         return FALSE;

  PALETTEENTRY entry;
  if (::GetPaletteEntries((HPALETTE) (*this), index, 1, &entry))
  {
         *red = entry.peRed;
         *green = entry.peGreen;
         *blue = entry.peBlue;
         return TRUE;
  } else
         return FALSE;
}

