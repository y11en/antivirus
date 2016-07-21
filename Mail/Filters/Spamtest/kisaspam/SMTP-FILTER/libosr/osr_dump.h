/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_dump.h
  Created: 2006/10/24
  Desc: 
      * class Dump - save dump of image (in png-format).

  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifdef WITH_DUMP
//--------------------------------------------------------------------------------------------------------------------//
#ifndef __osr_dump_h__
#define __osr_dump_h__
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_common.h"
#include "array.h"
#include "gd.h"
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
class Dump
{
public:

    /*Initializing interface */
    
    Dump(const Size width, const Size height);
    virtual ~Dump();
    
    /* Processing interface */

    ErrCode SetPixel (const Size x, const Size y, const RGB color);
    ErrCode SetGrayPixels (const unsigned char *gray, const unsigned int);
    ErrCode DrawRect (const Size l, const Size t, const Size r, const Size b, const RGB color);
    ErrCode DrawLine (const int StartX, const int StartY, const int EndX, const int EndY, const RGB color);

    /* Resulting interface */

    ErrCode Save (const char* file_name) const;

private:
    ErrCode GetColorIndex (const RGB color, int& index);

    Size width, height;
    gdImagePtr im;
    MiniContainer <RGB> palette;
    MiniContainer <int> palette_index;
};
//--------------------------------------------------------------------------------------------------------------------//
} // OSR namespace 
//--------------------------------------------------------------------------------------------------------------------//
#endif // __osr_dump_h__
//--------------------------------------------------------------------------------------------------------------------//
#endif // WITH_DUMP
//--------------------------------------------------------------------------------------------------------------------//
