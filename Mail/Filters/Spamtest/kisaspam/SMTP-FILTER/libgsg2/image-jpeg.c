/* -*- C -*-
 * File: image-jpeg.c
 *
 * Created: Fri Feb 27 14:01:22 2004
 */

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <smtp-filter/libgsg2/gsg.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif
#include <assert.h>

/*#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif*/

#ifndef _WIN32
#include <jpeglib.h>
#else
#include <gnu/jpeg/jpeglib.h>
#endif //_WIN32



//== 
// JPEG decompressors
typedef struct {
    struct jpeg_source_mgr pub;	/* public fields */
    binary_data_t *bd;
    unsigned char eof[2];
} mem_source_mgr;
typedef mem_source_mgr * my_src_ptr;

void init_source(j_decompress_ptr cinfo)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;
    src->bd->readp=0;
}
boolean
fill_input_buffer (j_decompress_ptr cinfo)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;
    if(src->bd->readp >= src->bd->used)
	{
	        src->eof[0] = (JOCTET) 0xFF;
		src->eof[1] = (JOCTET) JPEG_EOI;
		src->pub.next_input_byte = &src->eof[0];
		src->pub.bytes_in_buffer = 2;
		return TRUE; // EOF
	}
    src->pub.next_input_byte = src->bd->data + src->bd->readp;
    src->pub.bytes_in_buffer = 1;
    src->bd->readp++;
    return TRUE;
}

void     
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  if(src->pub.bytes_in_buffer>0)
      num_bytes-=src->pub.bytes_in_buffer;
  src->bd->readp+=num_bytes; // skip
  if(src->bd->readp >= src->bd->used)
      {
	  src->eof[0] = (JOCTET) 0xFF;
	  src->eof[1] = (JOCTET) JPEG_EOI;
	  src->pub.next_input_byte = &src->eof[0];
	  src->pub.bytes_in_buffer = 2;
	  return;
      }
  src->pub.next_input_byte = src->bd->data + src->bd->readp;
  src->pub.bytes_in_buffer = 1;
  src->bd->readp++;
}

//int resync_to_restart

void
term_source (j_decompress_ptr cinfo)
{
    cinfo;
  /* no work necessary here */
}

void
jpeg_set_src (j_decompress_ptr cinfo, binary_data_t *bd)
{
    my_src_ptr src;
    if (cinfo->src == NULL) {	/* first time for this JPEG object? */
	cinfo->src = (struct jpeg_source_mgr *)
	    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(mem_source_mgr));
	    src = (my_src_ptr) cinfo->src;
    }
  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
//  src->pub.resync_to_restart = NULL;//jpeg_resync_to_restart; /* use default method */
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->bd = bd;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;
void my_error_exit (j_common_ptr cinfo)
{
#if 0
    return;
#else
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
//  (*cinfo->err->output_message) (cinfo);
  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
#endif
}
void my_output (j_common_ptr cinfo){cinfo;}

typedef struct jpeg_decompress_struct jpeg_decompress_struct_t;

//==
int unpack_jpeg(binary_data_t *bd)
{
    jpeg_decompress_struct_t cinfo;
    struct my_error_mgr jerr;
    unsigned char *bufp;
    unsigned int line;
    int i;
    int pixels;
    char image_too_large_for_gsg7 = 0;
    int err_code = -1;
    
    if(!bd || !bd->image.gray || !bd->image.hbuf) return -1;

    bzero(&cinfo,sizeof(cinfo));
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jerr.pub.output_message=my_output;
    if (setjmp(jerr.setjmp_buffer)) {
	/* If we get here, the JPEG code has signaled an error.
	 * We need to clean up the JPEG object, close the input file, and return.
	 */
      ERROR:
	jpeg_destroy_decompress(&cinfo);
	return err_code;
  }

    bd->readp=0;
    jpeg_create_decompress(&cinfo);
    cinfo.mem->max_memory_to_use = 512*1024;
    jpeg_set_src(&cinfo, bd);
    (void) jpeg_read_header(&cinfo,TRUE);
    
    image_too_large_for_gsg7 = (char)
        ( cinfo.image_width > MAX_IMAGE_WIDTH || cinfo.image_height > MAX_IMAGE_HEIGHT
        || (pixels = cinfo.image_height*cinfo.image_width) > MAX_IMAGE_PIXELS);

    cinfo.scale_denom=1;

    if (image_too_large_for_gsg7)
    {
        if(cinfo.image_width > 800 && cinfo.image_height > 800)
	        cinfo.scale_denom=8;
        else if(cinfo.image_width > 400 && cinfo.image_height > 400)
	        cinfo.scale_denom=4;
        else if(cinfo.image_width > 200 && cinfo.image_height > 200)
	        cinfo.scale_denom=2;
    }

    cinfo.quantize_colors = 0;
    // always decode into gray ?

#if 0
    if ( cinfo.out_color_space == JCS_GRAYSCALE)
	{
	    bufp = bd->image.gray;
	}
    else 
	{
	     cinfo.out_color_components = 3;
	     cinfo.out_color_space = JCS_RGB;
	     bufp = (unsigned char *)bd->image.hbuf;
	}
#else
	     cinfo.out_color_components = 1;
	     cinfo.output_components = 1;
	     cinfo.out_color_space = JCS_GRAYSCALE;
	     bufp = bd->image.gray;
#endif
    cinfo.global_state = 202;
    jpeg_calc_output_dimensions(&cinfo);

    (void) jpeg_start_decompress(&cinfo);

    if(cinfo.output_width < 2 || cinfo.output_height < 2 )
    {
        err_code = IMAGE_TOO_SMALL;
	    goto ERROR;
    }
    if ( cinfo.output_width > MAX_IMAGE_WIDTH ||cinfo.output_height > MAX_IMAGE_HEIGHT)
    {
        err_code = IMAGE_TOO_LARGE;
	    goto ERROR;
    }

    bd->image.size_x = cinfo.output_width;
    bd->image.size_y = cinfo.output_height;
    bufp = bd->image.gray;
    line = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
#if 1
	if(cinfo.out_color_space == JCS_RGB)
	    {
		(void) jpeg_read_scanlines(&cinfo, &bufp, 1);
		for(i=0;i<bd->image.size_x;i++)
		    bd->image.gray[line*bd->image.size_x+i] = (unsigned char)pvt_rgb_gray_convert(bd,
										   bufp[i*3],
										   bufp[i*3+1],
										   bufp[i*3+2]);
		line++;
	    }
	else 
#endif
	    if(cinfo.out_color_space == JCS_GRAYSCALE)
	    {
		if(bufp - bd->image.gray+bd->image.size_x*cinfo.output_components 
		   > (int) image_pixels(&bd->image))
		    goto ERROR; // overrun
		(void) jpeg_read_scanlines(&cinfo, &bufp, 1);
		bufp+=bd->image.size_x*cinfo.output_components;
	    }
	else
	    goto ERROR;
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    if (image_too_large_for_gsg7)
        return IMAGE_TOO_LARGE_FOR_GSG7;

    return 0;
}
