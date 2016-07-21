/*
 * File:	imapng.cc
 * Purpose:	Platform Independent JPEG Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright: (c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 *      logjmp
 */
#include "stdafx.h"
#include <windowsx.h>
#include "imapng.h"
#if CIMAGE_SUPPORT_PNG

#include "imaiter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void
ima_png_error(png_struct *png_ptr, char *message)
{
	longjmp(png_ptr->jmpbuf, 1);
}

//HACK BY OP
void swap_RB(byte buffer[], int length)
{
	int i, temp;
	for (i=0;i<length/3;i++)
	{
		temp = buffer[3*i];
		buffer[3*i] =	buffer[3*i+2];
		buffer[3*i+2] =	temp;
	}
}

void CImagePNG::RGB2BGR(BYTE *buffer, int length)
{
	if (buffer && (GetBits()->biClrUsed==0)){
		BYTE temp;
		length = min(length,EffWidth);
		for (int i=0;i<length;i+=3){
			temp = buffer[i]; buffer[i] = buffer[i+2]; buffer[i+2] = temp;
		}
	}
}

void CImagePNG::ReadResouce(png_structp png_ptr, png_bytep data, png_size_t length)
{
	memcpy(data, png_ptr->io_ptr, length);
	png_ptr->io_ptr = (png_bytep)png_ptr->io_ptr + length;
}

bool CImagePNG::LoadImage(HINSTANCE hInstance, HRSRC hPngRes, COLORREF bkColor)
{
	if (lpbi)  
	{
		GlobalFreePtr(lpbi);
		delete imagePalette;
		lpbi = NULL;
		imagePalette = NULL;
	}

	DWORD rsize = ::SizeofResource ( hInstance, hPngRes );
	HGLOBAL hMem=::LoadResource(hInstance, hPngRes);
	if (hMem){
		LPVOID lpVoid=::LockResource(hMem);
		if (lpVoid)
		{
			SetBackground  (bkColor);
			return Resource2Image  (lpVoid) == TRUE;
		}
	}

	return false;
}

bool CImagePNG::LoadImage(LPCTSTR szFile, COLORREF bkColor)
{
	if (lpbi)  
	{
		GlobalFreePtr(lpbi);
		delete imagePalette;
		lpbi = NULL;
		imagePalette = NULL;
	}
	
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	
	const DWORD FileLength = GetFileSize (hFile, NULL);
	if (FileLength == INVALID_FILE_SIZE)
	{
		VERIFY(CloseHandle (hFile));
		return false;
	}

	LPVOID pMem = malloc(FileLength);
	if (pMem == NULL)
	{
		VERIFY(CloseHandle (hFile));
		return false;
	}

	DWORD dwReaded = 0;
	if ( !::ReadFile(hFile, pMem, FileLength, &dwReaded, NULL) || dwReaded!=FileLength )
	{
		VERIFY(CloseHandle (hFile));
		free (pMem);
		return false;
	}

	VERIFY(CloseHandle (hFile));

	SetBackground  (bkColor);
	BOOL bRet = Resource2Image  (pMem);
	free (pMem);

	return bRet==TRUE;

}

BOOL CImagePNG::Resource2Image(LPVOID res_mem)
{
  int number_passes;

	png_struct *png_ptr;
	png_info *info_ptr;
	CImageIterator iter(this);

	if (!res_mem)
		return FALSE;




	/* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      (void *)NULL, NULL, NULL);

   if (!png_ptr)
		return FALSE;

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
   {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	  return FALSE;
   }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
   if (setjmp(png_ptr->jmpbuf))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	  return FALSE;
   }

	png_set_read_fn(png_ptr, res_mem, (png_rw_ptr)CImagePNG::ReadResouce);

	/* read the file information */
	png_read_info(png_ptr, info_ptr);

	/* allocate the memory to hold the image using the fields
		of png_info. */
	png_color_16 my_background={ 0, (unsigned char)m_crBackGround&0x000000FF, (unsigned char)(m_crBackGround>>8)&0x000000FF, (unsigned char)(m_crBackGround>>16)&0x000000FF,0};//(GetSysColor(COLOR_3DFACE)>>16)&0x000000FF, (GetSysColor(COLOR_3DFACE)>>8)&0x000000FF, GetSysColor(COLOR_3DFACE)&0x000000FF, 0 };
	
	png_set_background(png_ptr, &my_background,
			PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

	/* tell libpng to strip 16 bit depth files down to 8 bits */
	if (info_ptr->bit_depth == 16)
		png_set_strip_16(png_ptr);

	int pixel_depth=(info_ptr->pixel_depth<24) ? info_ptr->pixel_depth: 24;
	Create(info_ptr->width, info_ptr->height, pixel_depth,
		info_ptr->color_type);

	if (info_ptr->num_palette>0)
	  SetPalette((int)info_ptr->num_palette, (rgb_color_struct*)info_ptr->palette);

	int row_stride = info_ptr->width * ((pixel_depth+7)>>3);

	byte *row_pointers = new byte[row_stride];

	/* turn on interlace handling */
	if (info_ptr->interlace_type)
		number_passes = png_set_interlace_handling(png_ptr);
	else
		number_passes = 1;

	for (int pass=0; pass< number_passes; pass++) 
	{
		iter.Upset();
		int y=0;
			do	
			{
			  if (info_ptr->interlace_type)  
			  {
				 if (pass>0)
					iter.GetRow(row_pointers, row_stride);
				 png_read_row(png_ptr, row_pointers, NULL);
			  }
			  else
				png_read_row(png_ptr, row_pointers, NULL);
    			RGB2BGR(row_pointers, row_stride);
				iter.SetRow(row_pointers, row_stride);
			  y++;
			} while(iter.PrevRow());
	}
	delete[] row_pointers;

	/* read the rest of the file, getting any additional chunks
		in info_ptr */
	png_read_end(png_ptr, info_ptr);


   /* clean up after the read, and free any memory allocated - REQUIRED */
   png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);


	/* that's it */
	return TRUE;
}

#endif // CIMAGE_SUPPORT_PNG

