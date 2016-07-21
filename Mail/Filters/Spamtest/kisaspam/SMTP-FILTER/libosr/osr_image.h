/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_image.h
  Created: 2006/09/08
  Desc: 
      * class InputBWImage - abstract class, it give size of image and
        image pixels to the ImageAnalyzer

      * class InputGrayImage can:
        - accumulate gray pixels
        - choose color level to do image black-white
        - convert gray color to black-white
      * class InputRGBImage can:
        - accumulate RGB pixels
        - choose color level to do image gray
        - convert RGB to gray
        - delegate InputGrayImage to convert gray to black-white
      * class InputDataOfImageFile can:
        - get data from the image file
        - get data from the array of image file content
        - delegate InputRGBImage to choose color level to do image gray
        - delegate InputRGBImage to convert RGB to gray

  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifndef __osr_image_h__
#define __osr_image_h__
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_common.h"
#include "array.h"
struct __binary_data;
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
ErrCode OSRCheckImageSizeLimits (const Size width, const Size height);
//--------------------------------------------------------------------------------------------------------------------//
class InputBWImage
{
public:
    virtual ~InputBWImage(){};
    /*Initializing interface */
    virtual ErrCode PreProcess (const InputBWImage* = NULL) {return OSR_OK;}
    /* Resulting interface */
    virtual ErrCode GetSize (Size& width /*OUT*/, Size& height /*OUT*/) const = 0;
    virtual ErrCode GetPixel (const Size x, const Size y, Color& color /*OUT*/) const = 0;
};
//--------------------------------------------------------------------------------------------------------------------//
typedef unsigned char GrayColor; /* gray palette [0;255], black-white image can be represented 
                                    as 0 (black) and 255 (white) */

class InputGrayImage : public InputBWImage
{
public:

    /*Initializing interface */
    
    InputGrayImage ();
    virtual ~InputGrayImage ();

    ErrCode Init (const Size width /*IN*/, const Size height /*IN*/);

    ErrCode SetPixel (const Size x, const Size y, const GrayColor pixel);
    ErrCode SetPixels (const GrayColor *pixel); // pixels count must be width*height, pixels(x,y) == pixels(y*width+x)
    
    virtual ErrCode PreProcess (const InputBWImage *more_colors = NULL);

    /* Resulting interface */

    virtual ErrCode GetSize (Size& width, Size& height) const;
    virtual ErrCode GetPixel (const Size x, const Size y, Color& color) const; // result BW, discolored color

    virtual ErrCode ConvertColor (const GrayColor& in, Color& out) const;


private:

    Size width, height;
    unsigned char bw_level;
    GrayColor *pixels;
    bool preprocessed;
};
//--------------------------------------------------------------------------------------------------------------------//
class InputRGBImage : public InputGrayImage
{
public:

    /*Initializing interface */
    
    InputRGBImage ();
    virtual ~InputRGBImage ();

    ErrCode Init (const Size width /*IN*/, const Size height /*IN*/);

    ErrCode SetPixel (const Size x, const Size y, const RGB pixel);
    ErrCode SetPixels (const RGB *pixel); // pixels count must be width*height, pixels(x,y) == pixels(y*width+x)

    virtual ErrCode PreProcess(const InputBWImage *more_colors = NULL);

    /* Resulting interface */

    virtual ErrCode GetSize (Size& width, Size& height) const;
    virtual ErrCode GetPixel (const Size x, const Size y, Color& color) const;      // result BW
    virtual ErrCode GetPixel (const Size x, const Size y, GrayColor& color) const;  // discolored color

    virtual ErrCode ConvertColor (const RGB& in, GrayColor& out) const;

private:

    Size width, height;
    RGB *pixels;
    unsigned long *rgb_ycc_tab;
    bool preprocessed;
};
//--------------------------------------------------------------------------------------------------------------------//
class InputDataOfImageFile: public InputRGBImage
{
public:

    /*Initializing interface */
    
    InputDataOfImageFile ();
    virtual ~InputDataOfImageFile();

    ErrCode Init (const char *file_name, const size_t frame_index = 0);
    // or         // frame_index - zero-based frame index (of animated image)
    ErrCode Init (const unsigned char *file_content, const size_t size, const size_t frame_index = 0);

    virtual ErrCode PreProcess(const InputBWImage *more_colors = NULL);

    /* Resulting interface */

    virtual ErrCode GetSize (Size& width, Size& height) const;
    virtual ErrCode GetPixel (const Size x, const Size y, Color& color) const;
    virtual ErrCode GetPixel (const Size x, const Size y, GrayColor& color) const;  // discolored color

    int GetLastFrameIndex() { return last_frame_index; } /*zero-based, -1 if no frames*/

private:

    bool preprocessed;
    struct __binary_data *bdata;
    int last_frame_index;

};
//--------------------------------------------------------------------------------------------------------------------//
} // OSR namespace 
//--------------------------------------------------------------------------------------------------------------------//
#endif // __osr_image_h__
//--------------------------------------------------------------------------------------------------------------------//
