/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: dump_c.cpp
  Created: 2006/10/24
  Desc: see dump_c.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef WITH_DUMP
/*--------------------------------------------------------------------------------------------------------------------*/
#include "osr_common.h"
#include "array.h"
#include "gd.h"
#include "dump_c.h"
#include "osr_dump.h"
/*--------------------------------------------------------------------------------------------------------------------*/
using namespace OSR;
/*--------------------------------------------------------------------------------------------------------------------*/
struct dump_
{
    OSR::Dump *p;
};
/*--------------------------------------------------------------------------------------------------------------------*/
dump_t* dump_create (const int width, const int height)
{
    dump_t* ret = (dump_t*)calloc (1, sizeof (dump_t));
    if (ret)
        ret->p = new Dump((Size)width, (Size)height);
    return ret;
}
/*--------------------------------------------------------------------------------------------------------------------*/
void  dump_destroy (dump_t* dump)
{
    if (dump)
    {
        if (dump->p)
            delete dump->p;
        free (dump);
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
int dump_set_pixel (dump_t* dump, const int x, const int y,
                    const unsigned char R, const unsigned char G, const unsigned char B)
{
    if (NULL == dump)
        return -1;
    RGB color = {R, G, B};
    return dump->p->SetPixel((Size)x, (Size)y, color);
}
/*--------------------------------------------------------------------------------------------------------------------*/
int dump_set_gray_pixels (dump_t* dump, const unsigned char *gray, const unsigned int size)
{
    if (NULL == dump)
        return -1;
    return dump->p->SetGrayPixels(gray, size);
}
/*--------------------------------------------------------------------------------------------------------------------*/
int dump_draw_rect (dump_t* dump, const int l, const int t, const int r, const int b,
                    const unsigned char R, const unsigned char G, const unsigned char B)
{
    if (NULL == dump)
        return -1;
    RGB color = {R, G, B};
    return dump->p->DrawRect((Size)l, (Size)t, (Size)r, (Size)b, color);
}
/*--------------------------------------------------------------------------------------------------------------------*/
int dump_draw_line (dump_t* dump, const int start_x, const int start_y, const int end_x, const int end_y,
                    const unsigned char R, const unsigned char G, const unsigned char B)
{
    if (NULL == dump)
        return -1;
    RGB color = {R, G, B};
    return dump->p->DrawLine(start_x, start_y, end_x, end_y, color);
}
/*--------------------------------------------------------------------------------------------------------------------*/
int dump_save (dump_t* dump, const char* file_name)
{
    if (NULL == dump)
        return -1;
    return dump->p->Save(file_name);
}
/*--------------------------------------------------------------------------------------------------------------------*/
#endif /*WITH_DUMP*/
/*--------------------------------------------------------------------------------------------------------------------*/
