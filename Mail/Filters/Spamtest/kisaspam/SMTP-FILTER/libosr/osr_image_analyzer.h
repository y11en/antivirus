/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_image_analyzer.h
  Created: 2006/09/08
  Desc: class OSRImageAnalyzer:
        - get pixels of image
        - recognize symbols
        - store symbols in the own storage (see Symbols, osr_symbol.h)
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifndef __osr_image_analyzer_h__
#define __osr_image_analyzer_h__
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_common.h"
#include "osr_symbol.h"
#include "osr_image.h"
//--------------------------------------------------------------------------------------------------------------------//
#define DEFAULT_MIN_SYMBOL_WIDTH        1
#define DEFAULT_MAX_SYMBOL_WIDTH        70
#define DEFAULT_MIN_SYMBOL_HEIGHT       5
#define DEFAULT_MAX_SYMBOL_HEIGHT       40
#define DEFAULT_MAX_SYMBOL_FAT_PERCENT  1000
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
class ImageAnalyzer
{
public:
    struct Filter
    {
        Size    min_symbol_width,
                max_symbol_width,
                min_symbol_height,
                max_symbol_height;
        size_t
                max_symbol_fat_percent;
        Filter()
        {
            min_symbol_width       = DEFAULT_MIN_SYMBOL_WIDTH;
            max_symbol_width       = DEFAULT_MAX_SYMBOL_WIDTH;
            min_symbol_height      = DEFAULT_MIN_SYMBOL_HEIGHT;
            max_symbol_height      = DEFAULT_MAX_SYMBOL_HEIGHT;
            max_symbol_fat_percent = DEFAULT_MAX_SYMBOL_FAT_PERCENT;
        }
    };

    /* Initializing interface */

    ImageAnalyzer ();
    virtual ~ImageAnalyzer ();

    ErrCode Load (const InputBWImage *image);

    void SetBackground  (const Color color) { background = color; }
    ErrCode AutodetectBackground (Color *result = NULL /*OUT*/, 
                                  unsigned int *percent = NULL /*OUT*/);

    void SetFilter (const Filter& filter_arg) {filter = filter_arg;}

    /* Processing interface */

    ErrCode RecognizeSymbols ();

    /* Resulting interface */

    inline Symbols* GetSymbols () {return &symbols;};

private:

    ImageAnalyzer(const ImageAnalyzer&);
    ImageAnalyzer& operator= (const ImageAnalyzer&);

    struct Point
    {
        Size x, y;
    };
    
    inline Color& Pixel (const Size x, const Size y) {return data[y*width+x];}
    inline Color& Pixel (const Point pt) {return data[pt.y*width+pt.x];}
    
    ErrCode FillSymbol (const Size x, const Size y, Symbol& symbol /*IN,OUT*/);

    Size     width, height;
    Color    *data;
    Color    background;

    Filter filter;
    Symbols symbols;
};
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
#endif // __osr_image_analyzer_h__
//--------------------------------------------------------------------------------------------------------------------//
