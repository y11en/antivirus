

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include "cimage.h"

#include "imapng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct FORMAT_LIST {
  char *ext;
  int Type;
} Image_Format_List[] = {
	"BMP", CIMAGE_FORMAT_BMP,
	"GIF", CIMAGE_FORMAT_GIF,
	"JPG", CIMAGE_FORMAT_JPEG,
	"PNG", CIMAGE_FORMAT_PNG,
	"XPM", CIMAGE_FORMAT_XPM
};

char *GetExtension( char* FileName )
{
  char* p = &FileName[strlen(FileName)-1];

  if (*p=='.') return 0;

  for (int i=strlen(FileName); i > 0 && *(p-1)!='.'; i--) p--;

  return p;
}


int search_formats(char *format)
{
	int l = 0, r = CMAX_IMAGE_FORMATS, m, k;
//	printf("{%s}", format);
	while (l<r) {
	  m = (l+r)/2;
	  k = strcmpi(Image_Format_List[m].ext, format);
	  if (k==0)
		 return Image_Format_List[m].Type;
	  else
		 if (k<0) l = m+1; else r = m;
	}

	return -1;
}

// New implementation
CImage::CImage(void)
{
  filename = "";
  filetype = -1;
  implementation = NULL;
}

 
CImage::CImage(const CString& fileName, int imageType)
{
	implementation = NULL;
	ReadFile(fileName, imageType);
}

CImage::CImage(const CImage *img)
{
  filename = ((CImage *)img)->GetFilename();
  filetype = ((CImage *)img)->GetFileType();
  switch (filetype)
  {
#if CIMAGE_SUPPORT_GIF
    case CIMAGE_FORMAT_GIF:
        implementation = new CImageGIF((const CImageGIF *)img->implementation);
        break;
#endif
#if CIMAGE_SUPPORT_JPEG
    case CIMAGE_FORMAT_JPEG:
        implementation = new CImageJPG((const CImageJPG *)img->implementation);
        break;
#endif
#if CIMAGE_SUPPORT_BMP
    case CIMAGE_FORMAT_BMP:
        implementation = new CImageBMP((const CImageBMP *)img->implementation);
        break;
#endif
#if CIMAGE_SUPPORT_PNG
    case CIMAGE_FORMAT_PNG:
        implementation = new CImagePNG((const CImagePNG *)img->implementation);
        break;
#endif
    default:
        implementation = NULL;
        break;
  }
}
  
CImage::~CImage()
{
  delete implementation;
}

BOOL CImage::ReadFile(const CString& fileName, int imageType)
{
        int oldImageType = filetype;
	filename = fileName;

	if (imageType==-1) {
	  imageType = search_formats(GetExtension((char *)(const char *)filename));
	}
        filetype = imageType;

        if (!implementation || (imageType != oldImageType))
        {
          if (!CreateImplementation(filename, imageType))
            return FALSE;
        }
          
        return implementation->ReadFile(filename);
}

BOOL CImage::SaveFile(const CString& fileName, int imageType)
{
        if (!implementation)
          return FALSE;

        int oldImageType = filetype;
	filename = fileName;

	if (imageType==-1) {
	  imageType = search_formats(GetExtension((char *)(const char *)filename));
	}
        filetype = imageType;
        
        if (oldImageType == imageType)
        {
          // Don't need to create a new implementation
        }
        else
        {
            
          CImageImpl *oldImage = implementation;
          implementation = NULL;
          if (!CreateImplementation(filename, imageType))
          {
            implementation = oldImage;
            return FALSE;
          }
          
          implementation->TransferBits(oldImage);
          delete oldImage;
        }
          
        return implementation->SaveFile(filename);
}

void CImage::Create(int width, int height, int deep, int colortype)
{
  if (!implementation)
  {
    int typ = CIMAGE_FORMAT_BMP;
    CreateImplementation("", typ);
    filetype = typ;
    filename = "";
  }
    

  implementation->Create(width, height, deep, colortype);
}

BOOL CImage::CreateImplementation(const CString& imageFileName, int& imageType)
{
  if (implementation)
    delete implementation;
  implementation = NULL;
  
	switch (imageType) {
	
#if CIMAGE_SUPPORT_PNG
	case CIMAGE_FORMAT_PNG:
	{
	  implementation = new CImagePNG(imageFileName);
	  break;
	}
#endif

#if CIMAGE_SUPPORT_JPEG
	case CIMAGE_FORMAT_JPEG:
	{
	  implementation = new CImageJPG(imageFileName);
	  break;
	}
#endif

#if CIMAGE_SUPPORT_BMP
	case CIMAGE_FORMAT_BMP:
	{
	  implementation =  new CImageBMP(imageFileName);
	  break;
	}
#endif

#if CIMAGE_SUPPORT_GIF
	case CIMAGE_FORMAT_GIF:
	{
	  implementation = new CImageGIF(imageFileName);
	  break;
	}
#endif

	default:
//	  AfxMessageBox("Unrecognized CImage format");
          return FALSE;
	}

	return TRUE;
}


CBitmap *CImage::MakeBitmap()
{
	HDC dc = ::CreateCompatibleDC(NULL);

	if (dc)
	{
        // tmpBitmap is a dummy, to satisfy ::CreateCompatibleDC (it
        // is a memory dc that must have a bitmap selected into it)
        HDC dc2 = GetDC(NULL);
        HBITMAP tmpBitmap = ::CreateCompatibleBitmap(dc2, GetWidth(), GetHeight());
        ReleaseDC(NULL, dc2);
        HBITMAP oldBitmap = (HBITMAP) ::SelectObject(dc, tmpBitmap);

        if ( GetPalette() )
        {
            HPALETTE oldPal = ::SelectPalette(dc, (HPALETTE) GetPalette()->m_hObject, FALSE);
		    ::RealizePalette(dc);
        }

		HBITMAP hBitmap = ::CreateDIBitmap(dc, GetImplementation()->GetBits(),
		    CBM_INIT, GetRawImage(), (LPBITMAPINFO) GetImplementation()->GetBits(),
			DIB_PAL_COLORS);

        ::SelectPalette(dc, NULL, TRUE);
        ::SelectObject(dc, oldBitmap);
        ::DeleteObject(tmpBitmap);
        ::DeleteDC(dc);

        if ( hBitmap )
        {
          CBitmap *bitmap = new CBitmap;
          bitmap->Attach(hBitmap);

		  return bitmap;
        }
        else
        {
          return NULL;
        }
	}
	else
    {
		return NULL;
    }
}

