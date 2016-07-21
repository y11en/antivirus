/********************************************************************
	created:	2005/11/03
	created:	3:11:2005   19:03
	filename: 	smtp-filter\libgsg2\image-png.c
	file path:	smtp-filter\libgsg2
	file base:	image-png
	file ext:	c
	author:		Evgeny Smirnov <Evgeny.Smirnov@kaspersky.com>
	
	purpose:	png-loader for GSG-2
*********************************************************************/


/* -*- C -*-
 * File: image-png.c
 *
 * Created: 01/11/2005
 */

//#include <stdlib.h>
//#include <stdio.h>
//#include <setjmp.h>
//#include <string.h>
#include <stdlib.h>
#include <smtp-filter/libgsg2/gsg.h>


#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

/*#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif*/

#include <gnu/libpng/png.h>

#if 0 // {*******************

static int    InterlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
static int    InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */

static void ResizeLine(unsigned char* LineIn, unsigned char* LineOut,
		       int InLineLen, int OutLineLen,float AScale)
{
    int i, ix, last_ix;
    double x;

    OutLineLen--;

    for (i = InLineLen, x = 0.0, last_ix = -1;
	 i-- > 0;
	 x += AScale, LineIn++)
    {
	ix = (int) x;
    	for (; last_ix < ix && last_ix < OutLineLen; last_ix++)
	    *LineOut++ = *LineIn;
    }

    /* Make sure the line is complete. */
    for (LineIn--; last_ix < OutLineLen; last_ix++)
	*LineOut++ = *LineIn;
}

#endif // ***************}


void png_read_data_fn(png_structp png_ptr,
        png_bytep data, png_size_t sz)
{
    binary_data_t *bd = (binary_data_t *)png_ptr->io_ptr;
    if(bd->used <= bd->readp)
    {
        png_ptr->chunk_name[0] = 0; // force initialize exit from infinite loop,
                                    // when image data has no IDAT-chunk
        return;
    }
    if(sz > bd->used - bd->readp) sz=bd->used-bd->readp;
    if(sz<40)
	{
	    size_t i;
	    for(i=0;i<sz;i++)
		    data[i]=bd->data[bd->readp+i];
	}
    else
	    memmove(data,bd->data+bd->readp,sz);
    bd->readp+=sz;
}


//==
int unpack_png(binary_data_t *bd)
{
    int nStep = 1;

    png_struct    *png_ptr = NULL;
    png_info	  *info_ptr = NULL;
    png_byte      *png_pixels = NULL;
    png_byte     **row_pointers = NULL;
    png_uint_32    row_bytes;
    
    png_uint_32   width;
    png_uint_32   height;
    png_uint_32   new_width;
    png_uint_32   new_height;
    int           bit_depth;
    int           color_type;
    unsigned int  row, col;
    int           ret;
    unsigned int  i;
    int           pixels = 0;
    char          image_too_large_for_gsg7 = 0;

    if(!bd ||!bd->data || bd->used < 8) return -1;
    if(!bd->image.gray) return -1;

    bd->readp = 0;
    
    /* read and check signature in PNG file */
   
    ret = png_check_sig (bd->data, 8);
    if (!ret)
        return -1;
    
    /* create png and info structures */
    
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
    if (!png_ptr)
        return -1;   /* out of memory */

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct (&png_ptr, NULL, NULL);
        return -1;   /* out of memory */
    }
    
    if (setjmp (png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        return -1;
    }
    
    /* set up the input control for C streams */
    png_set_read_fn (png_ptr, bd, png_read_data_fn);
    
    /* read the file information */
    png_read_info (png_ptr, info_ptr);
    
    /* get size and bit-depth of the PNG-image */
    png_get_IHDR (png_ptr, info_ptr,
        &width, &height, &bit_depth, &color_type,
        NULL, NULL, NULL);
    
    /* set-up the transformations */

    /* alpha channel doesn't mean for GSG-2*/
    png_set_strip_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png_ptr);
        png_set_gamma(png_ptr, 2.2, 0.45455);
    }

    
    if (color_type & PNG_COLOR_MASK_COLOR)
        png_set_rgb_to_gray (png_ptr, 1, -1, -1);
    /* expand images to bit-depth 8 (only applicable for grayscale images) */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_gray_1_2_4_to_8 (png_ptr);
    
    /* downgrade 16-bit images to 8 bit */
    if (bit_depth == 16)
        png_set_strip_16 (png_ptr);
    
   /* all transformations have been registered; now update info_ptr data,
    * get rowbytes and channels, and allocate image memory */
    
    png_read_update_info (png_ptr, info_ptr);
    
    /* get the new color-type and bit-depth (after expansion/stripping) */
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
        NULL, NULL, NULL);
    
    if (PNG_COLOR_TYPE_GRAY != color_type)
    {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        png_debug(1, "GSG-2 png decoloration failed\n");
	    return -1;
    }

    if ( width < 1 || height < 1 )
    {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        return IMAGE_TOO_SMALL;
    }

    image_too_large_for_gsg7 = (char)
            ( width > MAX_IMAGE_WIDTH || height > MAX_IMAGE_HEIGHT
              || (pixels = height*width) > MAX_IMAGE_PIXELS);

    nStep = 1;

    if (image_too_large_for_gsg7)
    {
        // source image is too large for allocated buffer GSG7 (need scaling)
        if(width > 800 && height > 800)
	        nStep = 8;
        else if(width > 400 && height > 400)
	        nStep = 4;
        else if(width > 200 && height > 200)
	        nStep = 2;
    }

    new_width  = width  / nStep;
    if (0 == new_width)
        new_width = 1;
    new_height = height / nStep;
    if (0 == new_height)
        new_height = 1;

    if(new_width < 1 ||new_height < 1 )
	{
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
	    return IMAGE_TOO_SMALL;
	}

    if(new_width > MAX_IMAGE_WIDTH || new_height > MAX_IMAGE_HEIGHT
       || (new_height*new_width) > MAX_IMAGE_PIXELS)
	{
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
	    return IMAGE_TOO_LARGE;
	}
    bd->image.size_x = new_width;
    bd->image.size_y = new_height;
    
    /* row_bytes is the width x number of channels x (bit-depth / 8) */
    row_bytes = png_get_rowbytes (png_ptr, info_ptr);

    if ( width != row_bytes ) {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        png_debug(1, "GSG-2 png preparing failed\n");
        return -1;
    }
    
    if ((png_pixels = (png_bytep) png_malloc (png_ptr, width * height * sizeof (png_byte))) == NULL) {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        return -1;
    }
    
    if ((row_pointers = (png_bytepp) png_malloc (png_ptr, height * sizeof (png_bytep))) == NULL)
    {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        free (png_pixels);
        png_pixels = NULL;
        return -1;
    }
    
    /* set the individual row_pointers to point at the correct offsets */
    for (i = 0; i < height; i++)
        row_pointers[i] = png_pixels + i * row_bytes;
    
    /* now we can go ahead and just read the whole image */
    png_read_image (png_ptr, row_pointers);
    
    /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
    png_read_end (png_ptr, info_ptr);

    /* clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
    
    i = 0;
    for (row = 0; row < new_height; row ++)
        for (col = 0; col < new_width; col ++)
            if (row*nStep < height && col*nStep < width)
    		    bd->image.gray[i++] = row_pointers [row*nStep][col*nStep];
            else // impossible, because new_height*nStep < height, becuse new_height == height / nStep
                 // but...
                bd->image.gray[i++] = 0;
    
    if (row_pointers != (unsigned char**) NULL)
        free (row_pointers);
    if (png_pixels != (unsigned char*) NULL)
        free (png_pixels);

    if (image_too_large_for_gsg7)
        return IMAGE_TOO_LARGE_FOR_GSG7;

    return 0;
}
