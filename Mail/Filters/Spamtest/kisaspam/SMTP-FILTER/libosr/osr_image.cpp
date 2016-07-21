/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_image.cpp
  Created: 2006/09/08
  Desc: see osr_image.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#include <stdio.h>
#include "osr_image.h"
#include "smtp-filter/libgsg2/gsg.h" // for image input
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
#define MAX_FILE_SIZE   (1*1024*1024)

#ifndef __libgsg_h
    #define MAXJSAMPLE 255
    #define SCALEBITS	16
    #define R_Y_OFF		0
    #define G_Y_OFF		(1*(MAXJSAMPLE+1))
    #define B_Y_OFF		(2*(MAXJSAMPLE+1))
    #define R_CB_OFF	(3*(MAXJSAMPLE+1))
    #define G_CB_OFF	(4*(MAXJSAMPLE+1))
    #define B_CB_OFF	(5*(MAXJSAMPLE+1))
    #define R_CR_OFF	B_CB_OFF
    #define G_CR_OFF	(6*(MAXJSAMPLE+1))
    #define B_CR_OFF	(7*(MAXJSAMPLE+1))
#endif // __libgsg_h

#define TABLE_SIZE	(8*(MAXJSAMPLE+1))
#define ONE_HALF	((long) 1 << (SCALEBITS-1))
#define CBCR_OFFSET	((long) 128 << SCALEBITS)
#define FIX(x)		((long) ((x) * (1L<<SCALEBITS) + 0.5))
//--------------------------------------------------------------------------------------------------------------------//
ErrCode OSRCheckImageSizeLimits (const Size width, const Size height)
{
    if ( width < MIN_OSRIMAGE_WIDTH || height < MIN_OSRIMAGE_WIDTH )
        return OSR_IMAGE_TOO_SMALL;
    if ( width > MAX_OSRIMAGE_WIDTH || width > MAX_OSRIMAGE_HEIGHT )
        return OSR_IMAGE_TOO_LARGE;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
InputGrayImage::InputGrayImage ()
{
    width = height = 0;
    bw_level = 127;
    preprocessed = false;
    pixels = NULL;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::Init(const Size width_arg, const Size height_arg)
{
    ErrCode err = OSR_OK;
    err = OSRCheckImageSizeLimits (width_arg, height_arg);
    if ( OSR_OK != err )
        return err;
    width = width_arg;
    height = height_arg;
    pixels = new GrayColor [width*height];
    if (NULL == pixels)
        return OSR_MEMORY_FULL;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
InputGrayImage::~InputGrayImage ()
{
    if (pixels )
        delete [] pixels;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::GetSize (Size& width_arg, Size& height_arg)  const
{
    if (NULL == pixels && !preprocessed)
        return OSR_INVALID_DATA; // call PreProcess() first
    width_arg = width;
    height_arg = height;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::GetPixel (const Size x, const Size y, Color& color) const
{
    if (NULL == pixels && !preprocessed)
        return OSR_INVALID_DATA; // call PreProcess() first
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;

    ErrCode err = OSR_OK;
    err = ConvertColor (pixels [y*width + x], color);

    return err;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::ConvertColor(const GrayColor& in, Color& out) const
{
    if (!preprocessed)
        return OSR_INVALID_DATA; // call PreProcess() first

    out = (in > bw_level) ? WHITE : BLACK;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::SetPixel (const Size x, const Size y, const GrayColor pixel)
{
    if (NULL == pixels)
        return OSR_INVALID_DATA;
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;
    pixels [y*width + x] = pixel;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::SetPixels (const GrayColor *pixels_arg)
{
    if (NULL == pixels)
        return OSR_INVALID_DATA;
    memcpy (pixels, pixels_arg, width*height*sizeof (GrayColor) );
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputGrayImage::PreProcess(const InputBWImage *more_colors)
{
    // PreProcess() prepare discoloring by own data, or by data of "more color".

    if (preprocessed)
        return OSR_INTERNAL_ERROR; // PreProcess() must be called oly once.

    Size w = 0, h = 0, x = 0, y = 0;
    unsigned int i = 0;
    InputRGBImage *rgb_image = NULL;
    ErrCode err = OSR_OK;
    if (NULL != more_colors) /* this object has no own data, but called from InputRGBImage object
                                for discoloring rgb-raster */
    {
        rgb_image = (InputRGBImage*)more_colors;
        err = rgb_image->GetSize(w, h);
        if (OSR_OK != err)
            return err;
    }
    else
    {
        if (NULL == pixels)
            return OSR_INVALID_DATA;
        w = width;
        h = height;
    }

    unsigned int colors_histogram [256] = {0};

    for ( y = 0; y < h; y ++)
        for ( x = 0; x < w; x ++)
        {
            GrayColor gray;
            if (rgb_image)
            {
                err = rgb_image->GetPixel(x, y, gray);
                if (OSR_OK != err)
                    return err;
            }
            else
                gray = pixels[y*w+x];
    	    colors_histogram [ gray % 256 ]++;
        }

    int sum=0, mult=0, i1 = -1;
    for ( i = 0; i < 256; i++ )
	{
        if (-1 == i1 && 0 != colors_histogram[i])
            i1 = i;
	    sum  += colors_histogram[i];
	    mult += colors_histogram[i]*i;
	}
    if (sum <= 0)
        return OSR_INTERNAL_ERROR;
    
    bw_level = (unsigned char)(mult/sum);

    if ( i1 < bw_level )
        bw_level = (unsigned char)(i1 + (bw_level-i1)*3/4);

    preprocessed = true;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
//####################################################################################################################//
//--------------------------------------------------------------------------------------------------------------------//
InputRGBImage::InputRGBImage ()
{
    width = 0;
    height = 0;
    preprocessed = false;

    pixels = NULL;
    rgb_ycc_tab = NULL;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::Init(const Size width_arg, const Size height_arg)
{
    ErrCode err = OSR_OK;
    err = OSRCheckImageSizeLimits (width_arg, height_arg);
    if ( OSR_OK != err )
        return err;
    width = width_arg;
    height = height_arg;
    pixels = new RGB [width*height];
    if (NULL == pixels)
        return OSR_MEMORY_FULL;

    rgb_ycc_tab = new unsigned long [TABLE_SIZE];

    if (NULL == rgb_ycc_tab)
        return OSR_MEMORY_FULL;
    for (size_t i = 0; i <= MAXJSAMPLE; i++) 
    {
        rgb_ycc_tab[i+R_Y_OFF] = FIX(0.29900) * i;
        rgb_ycc_tab[i+G_Y_OFF] = FIX(0.58700) * i;
        rgb_ycc_tab[i+B_Y_OFF] = FIX(0.11400) * i     + ONE_HALF;
        rgb_ycc_tab[i+R_CB_OFF] = (-FIX(0.16874)) * i;
        rgb_ycc_tab[i+G_CB_OFF] = (-FIX(0.33126)) * i;
        rgb_ycc_tab[i+B_CB_OFF] = FIX(0.50000) * i    + CBCR_OFFSET + ONE_HALF-1;
        rgb_ycc_tab[i+G_CR_OFF] = (-FIX(0.41869)) * i;
        rgb_ycc_tab[i+B_CR_OFF] = (-FIX(0.08131)) * i;
    }

    return InputGrayImage::Init(width, height);
}

//--------------------------------------------------------------------------------------------------------------------//
InputRGBImage::~InputRGBImage ()
{
    if (pixels )
        delete [] pixels;
    if (rgb_ycc_tab)
        delete [] rgb_ycc_tab;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::SetPixel (const Size x, const Size y, const RGB pixel)
{
    if (NULL == pixels)
        return OSR_INVALID_DATA;
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;
    pixels [y*width + x] = pixel;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::SetPixels (const RGB *pixels_arg)
{
    if (NULL == pixels)
        return OSR_INVALID_DATA;
    memcpy (pixels, pixels_arg, width*height*sizeof (RGB) );
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::PreProcess(const InputBWImage*)
{
    if (NULL == pixels || NULL == rgb_ycc_tab)
        return OSR_INVALID_DATA;

    if ( preprocessed )
        return OSR_INTERNAL_ERROR;

    preprocessed = true;

    ErrCode err = OSR_OK;
    err = InputGrayImage::PreProcess(this);
    if (OSR_OK != err)
        return err;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::GetSize (Size& width_arg, Size& height_arg) const
{
    if (NULL == pixels || NULL == rgb_ycc_tab || !preprocessed )
        return OSR_INVALID_DATA;
    width_arg = width;
    height_arg = height;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::GetPixel (const Size x, const Size y, Color& color) const      // result BW
{
    if (NULL == pixels || NULL == rgb_ycc_tab || !preprocessed )
        return OSR_INVALID_DATA;
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;
    GrayColor gray;
    ErrCode err = OSR_OK;
    err = GetPixel (x, y, gray);
    if (OSR_OK != err)
        return err;
    return InputGrayImage::ConvertColor(gray, color);
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::GetPixel (const Size x, const Size y, GrayColor& color) const  // discolored color
{
    if (NULL == pixels || NULL == rgb_ycc_tab || !preprocessed )
        return OSR_INVALID_DATA;
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;
    return ConvertColor (pixels[y*width+x], color);
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputRGBImage::ConvertColor (const RGB& in, GrayColor& out) const
{
    out = (GrayColor)((rgb_ycc_tab[in.R+R_Y_OFF] + rgb_ycc_tab[in.G+G_Y_OFF] +
                                  rgb_ycc_tab[in.B+B_Y_OFF]) >> SCALEBITS);
    return OSR_OK;
}

//--------------------------------------------------------------------------------------------------------------------//
//####################################################################################################################//
//--------------------------------------------------------------------------------------------------------------------//
InputDataOfImageFile::InputDataOfImageFile ()
{
    bdata = binary_init (MAX_FILE_SIZE);
    preprocessed = false;
    last_frame_index = -1;
}
//--------------------------------------------------------------------------------------------------------------------//
InputDataOfImageFile::~InputDataOfImageFile ()
{
    if (bdata)
        binary_free (&bdata);
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputDataOfImageFile::Init (const char *file_name, const size_t frame_index)
{
    FILE *f = fopen (file_name, "rb");
    if (NULL == f)
        return OSR_FILE_ERROR;

    const size_t str_size = 64*1024;
    char buf [str_size];
    size_t len = 0, all_len = 0;

    while ( !feof (f) )
    {
        len = fread( buf, 1, str_size, f );
        if (0 == len)
            break;
        all_len += len;
        if (all_len >= MAX_FILE_SIZE)
            return OSR_FILE_TOO_LARGE;
        if ( ferror (f) )
            return OSR_ERROR;
        binary_put(bdata,buf,len);
    }
    fclose(f);
    return Init(bdata->data, bdata->used, frame_index);
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputDataOfImageFile::Init (const unsigned char *file_content, const size_t size, const size_t frame_index)
{
    if (bdata->data != file_content)
        if ( binary_put(bdata,(void*)file_content,size) )
            return OSR_MEMORY_FULL;

    // uniformity of libgsg2
    if ( bdata->used <= 6)
        return OSR_IMAGE_FORMAT_ERROR;
    if(!memcmp("GIF89a",bdata->data,6) || !memcmp("GIF87a",bdata->data,6))
    {
        int i = 0;
        int unpack_res = 0;
        while ( i <= (int)frame_index )
        {
            if ( (unpack_res=unpack_gif(bdata, UNPACKED_GIF_CAN_NOT_BE_SCALED) ) < 0 )
                break;
            if (NULL == bdata->next_frame)
                break;
            i ++;
        }
        last_frame_index = i-1;
        if ( 0 != i && last_frame_index < (int)frame_index )
            return OSR_WRONG_FRAME_INDEX;
        if( unpack_res < 0 )
            return OSR_IMAGE_FORMAT_ERROR;
    }
    else if(bdata->data[0]==255 && bdata->data[1] == 216 && bdata->data[2]==255
            && (!memcmp("JFIF",bdata->data+6,4) ||!memcmp("Exif",bdata->data+6,4)))
    {
        if(unpack_jpeg(bdata)<0)
            return OSR_IMAGE_FORMAT_ERROR;
    }
    else if( !memcmp("\x89PNG",bdata->data,4) )
    {
        if(unpack_png(bdata)<0)
            return OSR_IMAGE_FORMAT_ERROR;
    }
    else
        return OSR_IMAGE_FORMAT_ERROR;

    if ( NULL == bdata->image.gray )
        return OSR_INVALID_DATA;
    ErrCode err = OSR_OK;
    err = OSRCheckImageSizeLimits ((Size)bdata->image.size_x, (Size)bdata->image.size_y);
    if ( OSR_OK != err )
        return err;

    if(bdata->data)
    {
        free(bdata->data); // for optimization
        bdata->data = NULL;
    }

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputDataOfImageFile::PreProcess(const InputBWImage*)
{
    if(NULL == bdata)
        return OSR_INVALID_DATA;

    if (preprocessed)
        return OSR_INTERNAL_ERROR;

    preprocessed = true;

    ErrCode err = OSR_OK;
    err = InputGrayImage::PreProcess(this);
    if (OSR_OK != err)
        return err;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputDataOfImageFile::GetSize (Size& width_arg, Size& height_arg) const
{
    if ( NULL == bdata || NULL == bdata->image.gray || !preprocessed)
        return OSR_INVALID_DATA;
    width_arg = (Size)bdata->image.size_x;
    height_arg = (Size)bdata->image.size_y;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputDataOfImageFile::GetPixel (const Size x, const Size y, Color& color) const      // result BW
{
    if ( NULL == bdata || NULL == bdata->image.gray || !preprocessed)
        return OSR_INVALID_DATA;
    if ( x < 0 || x >= bdata->image.size_x || y < 0 || y >= bdata->image.size_y )
        return OSR_INVALID_ARGUMENT;
    GrayColor gray;
    ErrCode err = OSR_OK;
    err = GetPixel (x, y, gray);
    if (OSR_OK != err)
        return err;
    return InputGrayImage::ConvertColor(gray, color);
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode InputDataOfImageFile::GetPixel (const Size x, const Size y, GrayColor& color) const      // discolored color
{
    if ( NULL == bdata || NULL == bdata->image.gray || !preprocessed)
        return OSR_INVALID_DATA;
    if ( x < 0 || x >= bdata->image.size_x || y < 0 || y >= bdata->image.size_y )
        return OSR_INVALID_ARGUMENT;
    color = (GrayColor)bdata->image.gray[y*bdata->image.size_x+x];
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
