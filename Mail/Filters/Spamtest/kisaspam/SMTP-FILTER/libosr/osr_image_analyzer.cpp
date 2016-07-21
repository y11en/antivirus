/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_image_analyzer.cpp
  Created: 2006/09/08
  Desc: osr_image_analyzer.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_image_analyzer.h"
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
ImageAnalyzer::ImageAnalyzer ()
{
    data = NULL;
    background = WHITE;
    width = 0;
    height = 0;
}
//--------------------------------------------------------------------------------------------------------------------//
ImageAnalyzer::~ImageAnalyzer()
{
    if (data)
        delete [] data;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode ImageAnalyzer::Load (const InputBWImage *image)
{
    ErrCode err = OSR_OK;
    err = image->GetSize(width, height);
    if (OSR_OK != err)
        return err;
    if ( width <= 0 || height <= 0 )
        return OSR_INVALID_ARGUMENT;

    Size x = 0, y = 0;

    data = new Color [width * height];
    if (NULL == data)
        return OSR_MEMORY_FULL;
    for (y = 0; y < height; y ++)
        for (x = 0; x < width; x ++)
            if ( OSR_OK != image->GetPixel(x, y, Pixel(x, y)) )
                return OSR_INVALID_ARGUMENT;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode ImageAnalyzer::AutodetectBackground (Color *result, unsigned int *percent)
{
    /* background is color that occupy maximum place */

    if ( width <= 0 || height <= 0 || NULL == data)
        return OSR_INVALID_DATA;

    size_t black_count = 0, white_count = 0, max = 0;

    Size x = 0, y = 0;

    for ( y = 0; y < height; y ++ )
        for ( x = 0; x < width; x ++ )
        {
            if ( BLACK == Pixel(x, y) )
                black_count ++;
            else if ( WHITE == Pixel(x, y) )
                white_count ++;
            else
                return OSR_INTERNAL_ERROR;
        }
    if ( black_count > white_count )
    {
        background = BLACK;
        max = black_count;
    }
    else
    {
        background = WHITE;
        max = white_count;
    }

    if (result)
        *result = background;
    if (percent)
        *percent = (100*max) / (width*height);

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode ImageAnalyzer::RecognizeSymbols ()
{
    Size x = 0, y = 0;
    ErrCode err = OSR_OK;

    Size symbol_width = 0, symbol_height = 0;

    for ( y = 0; y < height; y ++ )
        for ( x = 0; x < width; x ++ )
            if ( Pixel(x, y) != background )
            {
                Symbol symbol;
                err = FillSymbol (x, y, symbol);
                if ( OSR_OK != err )
                    return err;
                symbol_width  = symbol.GetWidth();
                symbol_height = symbol.GetHeight();
                if ( symbol_width  >= filter.min_symbol_width  &&
                     symbol_width  <= filter.max_symbol_width  &&
                     symbol_height >= filter.min_symbol_height &&
                     symbol_height <= filter.max_symbol_height &&
                     symbol_width <= symbol_height * (Size)(filter.max_symbol_fat_percent / 100))
                {
                    if ( NULL == symbols.Add() )
                        return OSR_MEMORY_FULL;
                    *symbols.GetLast() = symbol;
                }
            }
    return err;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode ImageAnalyzer::FillSymbol(const Size x, const Size y, Symbol& symbol)
{
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return OSR_INVALID_ARGUMENT;

    MiniContainer <Point> deffered_pixels;

    Point pt, pt2;
    pt.x = x;
    pt.y = y;

    Pixel(pt) = background;

    if ( !deffered_pixels.Push(pt) )
        return OSR_MEMORY_FULL;

    symbol.Set (pt.x, pt.y, pt.x, pt.y);
    
    bool add_ok = true;
    int limit = width * height;

    while( deffered_pixels.Size() > 0 && --limit >= 0)
    {
        if ( !deffered_pixels.Pop(pt) )
            return OSR_INTERNAL_ERROR;
        symbol.IncrementPixelsCount();

        if ( pt.x < symbol.Left() )
            symbol.Left()   = pt.x;
        if ( pt.y < symbol.Top() )
            symbol.Top()    = pt.y;
        if ( pt.x > symbol.Right() )
            symbol.Right()  = pt.x;
        if ( pt.y > symbol.Bottom() )
            symbol.Bottom() = pt.y;

        pt2 = pt;
        for ( size_t i = 0; i < 8; i ++ )
        {
            switch(i)
            {
            case 0:
                pt2.x --; pt2.y --;
            	break;
            case 1:
            case 2:
                pt2.x ++;
            	break;
            case 3:
            case 4:
                pt2.y ++;
            	break;
            case 5:
            case 6:
                pt2.x --;
            	break;
            case 7:
                pt2.y --;
            	break;
            default:
                return OSR_INTERNAL_ERROR;
            }

            if ( pt2.x >= 0 && pt2.x < width && pt2.y >= 0 && pt2.y < height )
                if ( background != Pixel(pt2.x, pt2.y) )
                {
                    Pixel(pt2.x, pt2.y) = background;
                    add_ok = deffered_pixels.Push(pt2);
                    if (!add_ok)
                        return OSR_MEMORY_FULL;
                }
        }
    }
    if ( limit <= 0 )
        return OSR_INTERNAL_ERROR;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
