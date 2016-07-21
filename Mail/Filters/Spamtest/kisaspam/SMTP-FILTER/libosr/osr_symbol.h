/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_symbol.h
  Created: 2006/09/08
  Desc: class Symbol - describe one symbol of image
        class Symbols - is the sotrage of symbols
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifndef __osr_symbol_h__
#define __osr_symbol_h__
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_common.h"
#include "array.h"
//--------------------------------------------------------------------------------------------------------------------//
#define     SYMBOL_USED_IN_LINE     0x00000001
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
class Symbol
{
public:
    struct Point
    {
        Size x, y;
    };

    Symbol() : left (0), right (0), top (0), bottom (0), 
               pixels_count (0), flags(0)
    {}
    virtual ~Symbol() {};
    inline Size& Left()   {return left;}
    inline Size& Right()  {return right;}
    inline Size& Top()    {return top;}
    inline Size& Bottom() {return bottom;}

    inline const Size& Left()   const {return left;}
    inline const Size& Right()  const {return right;}
    inline const Size& Top()    const {return top;}
    inline const Size& Bottom() const {return bottom;}
    
    inline void Set (Size l, Size t, Size r, Size b)
    {
        left = l;
        right = r;
        top = t;
        bottom = b;
    }

    inline Point GetCenter()    const { Point pt = {GetCenterX(), GetCenterY()}; return pt; }
    inline Size GetCenterX()    const { return (Size)((left + right) / 2); }
    inline Size GetCenterY()    const { return (Size)((top + bottom) / 2); }
    inline Size GetWidth()      const { return (Size)(right - left + 1); }
    inline Size GetHeight()     const { return (Size)(bottom - top + 1); }
    inline Size GetPixelCount() const { return pixels_count; }

    inline void IncrementPixelsCount() { pixels_count ++; }

    inline unsigned int& Flags() { return flags;}

private:

    Size left, right, top, bottom; // coordinats of bounds
    Size pixels_count;
    unsigned int flags;
};
typedef Container <Symbol> Symbols;
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
#endif // __osr_symbol_h__
//--------------------------------------------------------------------------------------------------------------------//
