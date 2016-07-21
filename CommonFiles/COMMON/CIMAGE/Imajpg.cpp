/*
 * File:	imajpg.cc
 * Purpose:	Platform Independent JPEG Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright: (c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * This software is based in part on the work of the Independent JPEG Group.
 *
 */
 
//#include "stdafx.h"

#include "imajpg.h"

#if CIMAGE_SUPPORT_JPEG

#include <stdio.h>

extern "C" {
#include "jpeglib.h"
}

#include "imaiter.h"
         
#include <setjmp.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

struct ima_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef ima_error_mgr *ima_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

static void
ima_jpeg_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  ima_error_ptr myerr = (ima_error_ptr) cinfo->err;

  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  myerr->pub.format_message (cinfo, buffer);

  /* Send it to stderr, adding a newline */

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

BOOL
CImageJPG::ReadFile(const CString& imageFileName)
{
  if (imageFileName != "")
    filename = imageFileName;
    
  CImageIterator iter(this);
  /* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler. */

  struct ima_error_mgr jerr;
  /* More stuff */
  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

  if ((infile = fopen((const char *)filename, "rb")) == NULL) {
	 //fprintf(stderr, "can't open %s\n", filename);
	 return 0;
  }

  /* Step 1: allocate and initialize JPEG decompression object */
  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = ima_jpeg_error_exit;

  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
	 /* If we get here, the JPEG code has signaled an error.
	  * We need to clean up the JPEG object, close the input file, and return.
	  */
	 jpeg_destroy_decompress(&cinfo);
	 fclose(infile);
	 return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */
  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */
  (void) jpeg_read_header(&cinfo, TRUE);

  /* Step 4: set parameters for decompression */
  if (cinfo.jpeg_color_space!=JCS_GRAYSCALE) {
	cinfo.quantize_colors = TRUE;
	cinfo.desired_number_of_colors = 128;
  }
  /* Step 5: Start decompressor */
  jpeg_start_decompress(&cinfo);

  /* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	*/
  Create(cinfo.image_width, cinfo.image_height, 8*cinfo.output_components);

  if (cinfo.jpeg_color_space==JCS_GRAYSCALE)
	 CreateGrayColourMap(256);
  else
	 SetPalette(cinfo.actual_number_of_colors, cinfo.colormap[0], cinfo.colormap[1], cinfo.colormap[2]);

  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;

  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
  iter.Upset();
  while (cinfo.output_scanline < cinfo.output_height) {
	 (void) jpeg_read_scanlines(&cinfo, buffer, 1);
	 /* Assume put_scanline_someplace wants a pointer and sample count. */
	 iter.SetRow(buffer[0], row_stride);
	 iter.PrevRow();
  }

  /* Step 7: Finish decompression */
  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
  fclose(infile);
  /* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

  /* And we're done! */
  return 1;
}

void CImageJPG::BGR2RGB(unsigned char* pBuffer,int iCount)
{
	int i;
	for(i=0;i<iCount;i=i+3)
	{
		unsigned char cTemp;
		cTemp = *(pBuffer+i);
		*(pBuffer+i) = *(pBuffer+i+2);
		*(pBuffer+i+2) = cTemp;
	}
}

BOOL CImageJPG::SaveFile(const CString& imageFileName)
{
  if (imageFileName != "")
	 filename = imageFileName;

  CImageIterator iter(this);
	byte *buf2=0;

  /* This struct contains the JPEG compression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	* It is possible to have several such structures, representing multiple
	* compression/decompression processes, in existence at once.  We refer
	* to any one struct (and its associated working data) as a "JPEG object".
	*/
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;		/* target file */
  int row_stride;		/* physical row width in image buffer */
  JSAMPARRAY buffer;		/* Output row buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
	* step fails.  (Unlikely, but it could happen if you are out of memory.)
	* This routine fills in the contents of struct jerr, and returns jerr's
	* address which we place into the link field in cinfo.
	*/
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
	* stdio stream.  You can also write your own code to do something else.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to write binary files.
	*/
  if ((outfile = fopen((const char *)filename, "wb")) == NULL) {
	 return FALSE;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
  cinfo.image_width = GetWidth(); 	// image width and height, in pixels
  cinfo.image_height = GetHeight();
  cinfo.input_components = 3; 	// # of color components per pixel
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

  /* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.)
	*/
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:
	*/
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing.
	*/
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though.
	*/
  row_stride = GetWidth()*cinfo.input_components;	/* JSAMPLEs per row in image_buffer */

  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  iter.Upset();
  buf2 = new byte[GetWidth()];
  while (cinfo.next_scanline < cinfo.image_height) 
  {
	 if (GetPalette())
	 {
		iter.GetRow(buf2, GetWidth());
		int k=0;
		for (int i=0; i<GetWidth(); i++, k+=3)
		  GetPalette()->GetRGB(buf2[i], &buffer[0][k], &buffer[0][k+1], &buffer[0][k+2]);
	 } else
     {
		iter.GetRow(buffer[0], row_stride);
        BGR2RGB(buffer[0], row_stride);	 // Lance : 1998/09/01 : Bug ID: EXP-2.1.1-9
     }

	 iter.PrevRow();
	 (void) jpeg_write_scanlines(&cinfo, buffer, 1);

  }
  delete [] buf2;

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */
  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
  return TRUE;
}



void CImageJPG::CreateGrayColourMap(int n)
{
  byte g[256];

  for (int i=0; i<n; i++) g[i] = (byte)(256*i/n);
  SetPalette(n, g);
}

#endif // CIMAGE_SUPPORT_JPEG

