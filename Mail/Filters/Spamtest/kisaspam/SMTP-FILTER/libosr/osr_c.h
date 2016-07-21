/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_c.h
  Created: 2006/09/08
  Desc: C-wrapper about class Expert (see osr_expert.h)
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
#ifndef __osr_c_h__
#define __osr_c_h__
/*--------------------------------------------------------------------------------------------------------------------*/
#include <sys/types.h>
#include "osr_error.h"
/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/*--------------------------------------------------------------------------------------------------------------------*/
float          osr_version ();
const char*    osr_version_text ();

typedef struct osr_expert_ osr_expert;

typedef struct
{
    unsigned int
        min_symbol_width,
        max_symbol_width,
        min_symbol_height,
        max_symbol_height,
        max_symbol_fat_percent,
        
        min_symbols_in_word,
        max_symbols_in_word,
        min_words_in_line,
        min_lines_in_text,
        
        min_text_percentage;
} osr_filter;
/*--------------------------------------------------------------------------------------------------------------------*/
/*Initializing interface */

osr_expert*    osr_expert_create        ();
void           osr_expert_destroy       (osr_expert *exp /*IN*/);
void           osr_set_filter           (osr_expert *exp /*IN*/, osr_filter *filter /*IN*/);
void           osr_use_background_white (osr_expert *exp); /* (autodetecting by default) */
void           osr_use_background_black (osr_expert *exp); /* (autodetecting by default) */

/* Processing interface */

osr_err_code   osr_process_file         (osr_expert *exp /*IN*/, const char *file_name /*IN*/,
                                         const unsigned int frame_index /*IN*/, unsigned int *last_frame_index /*OUT*/);
                                         // frame_index - zero-based frame index (of animated image)
osr_err_code   osr_process_gray_array   (osr_expert *exp /*IN*/, const unsigned char *gray_pixels /*IN*/,
                                         const unsigned int width, const unsigned int height);
                                         /* count of pixels must be width*height, pixels(x,y) == pixels(y*width+x) */
/* Resulting interface */

osr_err_code   osr_get_verdict          (osr_expert *exp /*IN*/, char *is_text /*OUT*/);

osr_err_code   osr_get_symbols_count    (osr_expert *exp /*IN*/, unsigned int *count /*OUT*/);
osr_err_code   osr_get_words_count      (osr_expert *exp /*IN*/, unsigned int *count /*OUT*/);
osr_err_code   osr_get_lines_count      (osr_expert *exp /*IN*/, unsigned int *count /*OUT*/);

osr_err_code   osr_get_line_words_count (osr_expert *exp /*IN*/, const unsigned int line_num /*IN*/,
                                         unsigned int* count /*OUT*/);

osr_err_code   osr_get_word_size        (osr_expert *exp /*IN*/, const unsigned int line_num /*IN*/,
                                         const unsigned int word_num /*IN*/, unsigned int* size /*OUT*/);

osr_err_code   osr_get_words_pixels_percentage  
                                        (osr_expert *exp /*IN*/, unsigned int* percents /*OUT*/);

osr_err_code   osr_get_GSG7_signature   (osr_expert *exp /*IN*/, char signature[33] /*OUT*/);
osr_err_code   osr_get_image_size       (osr_expert *exp /*IN*/, unsigned int* image_width /*OUT*/,
                                         unsigned int* image_height /*OUT*/);

osr_err_code   osr_get_default_filter   (osr_filter *filter /*OUT*/);

#ifdef WITH_DUMP

osr_err_code   osr_get_dump             (osr_expert *exp /*IN*/,
                                         const char* dump_name  /*IN*/,
                                         const char dump_image  /*IN*/, const char dump_symbols  /*IN*/,
                                         const char dump_lines  /*IN*/, const char dump_words  /*IN*/);

osr_err_code   osr_get_html_report      (osr_expert *exp /*IN*/, const char *image_file /*IN*/,
                                         const char *dump_file /*IN*/, const char *report_file /*IN*/,
                                         const osr_err_code last_err /*IN*/,
                                         const unsigned int frame_index /*IN*/ );

osr_err_code   osr_separate_html        (const char *report_file /*IN*/, const unsigned int html_separate_step /*IN*/);

#endif /*WITH_DUMP*/

const char* osr_get_error_text          (const osr_err_code err);
/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*--------------------------------------------------------------------------------------------------------------------*/
#endif /*__osr_c_h__*/
/*--------------------------------------------------------------------------------------------------------------------*/
