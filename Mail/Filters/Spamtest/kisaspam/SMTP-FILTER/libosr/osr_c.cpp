/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_c.cpp
  Created: 2006/09/08
  Desc: see osr_c.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
#include <assert.h>
#include "osr_c.h"
#include "osr_expert.h"
using namespace OSR;
/*--------------------------------------------------------------------------------------------------------------------*/
const char* osr_version_text ()
{
    return (const char*)GetOsrVersionText();
}
/*--------------------------------------------------------------------------------------------------------------------*/
float osr_version ()
{
    return (float)GetOsrVersion();
}
/*--------------------------------------------------------------------------------------------------------------------*/
struct osr_expert_
{
    Expert *expert;
    InputBWImage *image;
};
/*--------------------------------------------------------------------------------------------------------------------*/
/*Initializing interface */
/*--------------------------------------------------------------------------------------------------------------------*/
osr_expert* osr_expert_create ()
{
    osr_expert* ret = (osr_expert*)calloc (1, sizeof (osr_expert));
    if (ret)
        ret->expert = new Expert;
    ret->image = 0;
    return ret;
}
/*--------------------------------------------------------------------------------------------------------------------*/
void osr_expert_destroy (osr_expert *exp /*IN*/)
{
    if (0 == exp)
        return;
    if (exp->expert)
        delete exp->expert;
    if (exp->image)
        delete exp->image;
    free (exp);
}
/*--------------------------------------------------------------------------------------------------------------------*/
void osr_set_filter (osr_expert *exp /*IN*/, osr_filter *filter_arg /*IN*/)
{
    if (0 == exp)
        return;

    Expert::Filter filter;

    filter.symbols_filter.min_symbol_width          = (Size)filter_arg->min_symbol_width;
    filter.symbols_filter.max_symbol_width          = (Size)filter_arg->max_symbol_width;
    filter.symbols_filter.min_symbol_height         = (Size)filter_arg->min_symbol_height;
    filter.symbols_filter.max_symbol_height         = (Size)filter_arg->max_symbol_height;
    filter.symbols_filter.max_symbol_fat_percent    = filter_arg->max_symbol_fat_percent;
    
    filter.text_filter.min_symbols_in_word  = filter_arg->min_symbols_in_word;
    filter.text_filter.max_symbols_in_word  = filter_arg->max_symbols_in_word;
    filter.text_filter.min_words_in_line    = filter_arg->min_words_in_line;
    filter.text_filter.min_lines_in_text    = filter_arg->min_lines_in_text;
    
    filter.min_text_percentage = filter_arg->min_text_percentage;

    exp->expert->SetFilter(filter);
}
/*--------------------------------------------------------------------------------------------------------------------*/
void osr_use_background_white (osr_expert *exp)
{
    if (0 == exp)
        return;
    exp->expert->SetBckgrDetectingMode(Expert::SET_BACKGROUND_WHITE);
}
/*--------------------------------------------------------------------------------------------------------------------*/
void osr_use_background_black (osr_expert *exp)
{
    if (0 == exp)
        return;
    exp->expert->SetBckgrDetectingMode(Expert::SET_BACKGROUND_BLACK);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_default_filter (osr_filter *filter /*OUT*/)
{
    if (0 == filter)
        return OSR_INVALID_ARGUMENT;

    filter->min_symbol_width        = DEFAULT_MIN_SYMBOL_WIDTH;
    filter->max_symbol_width        = DEFAULT_MAX_SYMBOL_WIDTH;
    filter->min_symbol_height       = DEFAULT_MIN_SYMBOL_HEIGHT;
    filter->max_symbol_height       = DEFAULT_MAX_SYMBOL_HEIGHT;
    filter->max_symbol_fat_percent  = DEFAULT_MAX_SYMBOL_FAT_PERCENT;
    
    filter->min_symbols_in_word     = DEFAULT_MIN_SYMBOLS_IN_WORD;
    filter->max_symbols_in_word     = DEFAULT_MAX_SYMBOLS_IN_WORD;
    filter->min_words_in_line       = DEFAULT_MIN_WORDS_IN_LINE;
    filter->min_lines_in_text       = DEFAULT_MIN_LINES_IN_TEXT;
    
    filter->min_text_percentage = DEFAULT_MIN_TEXT_PERCENTAGE;

    return OSR_OK;
}
/*--------------------------------------------------------------------------------------------------------------------*/
/* Processing interface */
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_process_file (osr_expert *exp /*IN*/, const char *file_name /*IN*/,
                               const unsigned int frame_index /*IN*/, unsigned int *last_frame_index /*OUT*/)
                               // frame_index - zero-based frame index (of animated image)
{
    osr_err_code err = OSR_OK;

    if (0 == exp)
        return OSR_INVALID_DATA;
    
    InputDataOfImageFile* image_file = new InputDataOfImageFile;
    exp->image = image_file;
    
    err = image_file->Init(file_name, frame_index);
    if ( OSR_WRONG_FRAME_INDEX == err )
        *last_frame_index = image_file->GetLastFrameIndex();
    if (OSR_OK != err)
        return err;

    err = exp->expert->Process(image_file);

    return err;
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_process_gray_array (osr_expert *exp /*IN*/, const unsigned char *gray_pixels /*IN*/,
                                     const unsigned int width, const unsigned int height)
                                     // count of pixels must be width*height, pixels(x,y) == pixels(y*width+x)
{
    osr_err_code err = OSR_OK;

    if (0 == exp)
        return OSR_INVALID_DATA;
    InputGrayImage *gray_image = new InputGrayImage;
    exp->image = gray_image;
    err = gray_image->Init((Size)width, (Size)height);
    if (OSR_OK != err)
        return err;
    err = gray_image->SetPixels(gray_pixels);
    if (OSR_OK != err)
        return err;
    err = exp->expert->Process(gray_image);
    return err;
}
/*--------------------------------------------------------------------------------------------------------------------*/
/* Resulting interface */
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_verdict (osr_expert *exp /*IN*/, char *is_text /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    bool is_text_bool;
    ErrCode err = exp->expert->GetVerdict(is_text_bool);
    if (OSR_OK == err)
        *is_text = (char) (is_text_bool ? 1 : 0);
    return err;
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_symbols_count (osr_expert *exp /*IN*/, unsigned int *count /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetSymbolsCount(*count);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_words_count (osr_expert *exp /*IN*/, unsigned int *count /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetWordsCount(*count);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_lines_count (osr_expert *exp /*IN*/, unsigned int *count /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetLinesCount(*count);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_line_words_count (osr_expert *exp /*IN*/, const unsigned int line_num /*IN*/,
                                  unsigned int* count /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetWordsCount(line_num, *count);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_word_size (osr_expert *exp /*IN*/, const unsigned int line_num /*IN*/,
 const unsigned int word_num /*IN*/, unsigned int* size /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetWordSize(line_num, word_num, *size);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_words_pixels_percentage 
 (osr_expert *exp /*IN*/, unsigned int* percents /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetWordsPixelsPercentage(*percents);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_GSG7_signature ( osr_expert *exp /*IN*/, char signature[33] /*OUT*/)
{
    assert (sizeof (GSG7Signature) == sizeof (char[33]));
    if (0 == exp)
        return OSR_INVALID_DATA;
    GSG7Signature s = "";
    ErrCode err = OSR_OK;
    err = exp->expert->GetGSG7Signature(s);
    if (OSR_OK != err)
        return err;
    memcpy (signature, s, sizeof(GSG7Signature));
    return OSR_OK;
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_image_size (osr_expert *exp /*IN*/, unsigned int* image_width /*OUT*/,
                                         unsigned int* image_height /*OUT*/)
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetImageSize(*image_width, *image_height);
}
/*--------------------------------------------------------------------------------------------------------------------*/
#ifdef WITH_DUMP
osr_err_code osr_get_dump (osr_expert *exp /*IN*/,
                           const char* dump_name  /*IN*/,
                           const char dump_image  /*IN*/, const char dump_symbols  /*IN*/,
                           const char dump_lines  /*IN*/, const char dump_words  /*IN*/ )
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetDump( dump_name, 0 != dump_image, 0 != dump_symbols, 0 != dump_lines, 0 != dump_words);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_get_html_report (osr_expert *exp /*IN*/, const char *image_file /*IN*/,
                                  const char *dump_file /*IN*/, const char *report_file /*IN*/,
                                  const osr_err_code last_err /*IN*/,
                                  const unsigned int frame_index /*IN*/ )
{
    if (0 == exp)
        return OSR_INVALID_DATA;
    return exp->expert->GetHtmlReport(image_file, dump_file, report_file, last_err, frame_index);
}
/*--------------------------------------------------------------------------------------------------------------------*/
osr_err_code osr_separate_html (const char *report_file /*IN*/, const unsigned int html_separate_step /*IN*/)
{
    return Expert::SeparateHtml (report_file, html_separate_step);
}
#endif /*WITH_DUMP*/
/*--------------------------------------------------------------------------------------------------------------------*/
const char* osr_get_error_text (const osr_err_code err)
{
    return ErrorText(err);
}
/*--------------------------------------------------------------------------------------------------------------------*/
