/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_image.cpp
  Created: 2006/10/24
  Desc: see osr_dump.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifdef WITH_DUMP
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_dump.h"
#include "gd.h" // for image output
#ifdef _MSC_VER
# pragma comment(lib, "../gdwin32/bgd.lib" )
#endif /*_MSC_VER*/
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
Dump::Dump(const Size width_arg, const Size height_arg)
{
    width = width_arg;
    height = height_arg;

    im = gdImageCreate(width, height);
}
//--------------------------------------------------------------------------------------------------------------------//
Dump::~Dump()
{
    if (im)
         gdImageDestroy(im);
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Dump::GetColorIndex (const RGB color, int& index)
{
    unsigned int i = 0;
    if ( !palette.Find (color, i) )
    {
        if (NULL == palette.Add() || NULL == palette_index.Add() )
            return OSR_MEMORY_FULL;
        *palette.GetLast() = color;
        i = gdImageColorAllocate (im, color.R, color.G, color.B);
        *palette_index.GetLast() = i;
        if (-1 == i)
            return OSR_MEMORY_FULL;
    }
    if (palette_index.Size() <= i)
        return OSR_INTERNAL_ERROR;
    index = palette_index[i];
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Dump::SetPixel (const Size x, const Size y, const RGB color)
{
    if ( NULL == im)
        return OSR_INVALID_DATA;
    if (x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;

    ErrCode err = OSR_OK;
    int i = 0;
    err = GetColorIndex (color, i);
    if (OSR_OK != err)
        return err;

    gdImageSetPixel (im, x, y, i);

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Dump::SetGrayPixels (const unsigned char *gray, const unsigned int size)
{
    if ( ((unsigned int)width)*((unsigned int)height) != size )
        return OSR_INVALID_ARGUMENT;
    ErrCode err = OSR_OK;
    int i = 0;
    Size x = 0, y = 0;
    for ( y = 0; y < height; y ++ )
        for ( x = 0; x < width; x ++ )
        {
            RGB rgb = {gray[y*width+x], gray[y*width+x], gray[y*width+x]};
            err = GetColorIndex (rgb, i);
            if (OSR_OK != err)
                return err;
            gdImageSetPixel (im, x, y, i);
        }
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Dump::DrawRect (const Size l, const Size t, const Size r, const Size b, const RGB color)
{
    if ( NULL == im)
        return OSR_INVALID_DATA;
    if (l < 0 || l >= width || t < 0 || t >= height ||
        r < 0 || r >= width || b < 0 || b >= height )
        return OSR_INVALID_ARGUMENT;

    ErrCode err = OSR_OK;
    int i = 0;
    err = GetColorIndex (color, i);
    if (OSR_OK != err)
        return err;

    gdImageRectangle (im, l, t, r, b, i);

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Dump::DrawLine(const int x1, const int y1, const int x2, const int y2, const RGB color)
{
    if ( NULL == im)
        return OSR_INVALID_DATA;
    if (x1 < 0 || x1 >= width || y1 < 0 || y1 >= height ||
        x2 < 0 || x2 >= width || y2 < 0 || y2 >= height )
        return OSR_INVALID_ARGUMENT;

    ErrCode err = OSR_OK;
    int i = 0;
    err = GetColorIndex (color, i);
    if (OSR_OK != err)
        return err;

    gdImageLine (im, x1, y1, x2, y2, i);

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Dump::Save (const char* file_name) const
{
    FILE *out = NULL;
    int size = 0;
    char *data = NULL;
    out = fopen(file_name, "wb");
    if (!out)
        return OSR_INVALID_ARGUMENT;
    data = (char *) gdImagePngPtr(im, &size);
    if (NULL == data)
        return OSR_MEMORY_FULL;
    if (fwrite(data, 1, size, out) != (unsigned int)size)
        return OSR_FILE_ERROR;
    if (fclose(out) != 0)
        return OSR_FILE_ERROR;
    gdFree(data);  
    
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
#endif // WITH_DUMP
//--------------------------------------------------------------------------------------------------------------------//
