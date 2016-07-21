/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_dump.h
  Created: 2006/10/24
  Desc: see osr_dump.h

  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef WITH_DUMP
/*--------------------------------------------------------------------------------------------------------------------*/
#ifndef __osr_dump_c_h__
#define __osr_dump_c_h__
/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------------------------------------------------------------------------------------*/
typedef struct dump_ dump_t;

dump_t* dump_create (const int width, const int height);
void    dump_destroy (dump_t*);

int dump_set_pixel (dump_t* dump, const int x, const int y,
                    const unsigned char R, const unsigned char G, const unsigned char B);
int dump_set_gray_pixels
                   (dump_t* dump, const unsigned char *gray, const unsigned int size);
int dump_draw_rect (dump_t* dump, const int l, const int t, const int r, const int b,
                    const unsigned char R, const unsigned char G, const unsigned char B);
int dump_draw_line (dump_t* dump, const int start_x, const int start_y, const int end_x, const int end_y,
                    const unsigned char R, const unsigned char G, const unsigned char B);
int dump_save      (dump_t* dump, const char* file_name);

/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*--------------------------------------------------------------------------------------------------------------------*/
#endif // __osr_dump_c_h__
/*--------------------------------------------------------------------------------------------------------------------*/
#endif /*WITH_DUMP*/
/*--------------------------------------------------------------------------------------------------------------------*/
