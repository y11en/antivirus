/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_expert.h
  Created: 2006/09/08
  Desc: Optical Symbols Recognition library
        Analyze image and recognize the presence of text, it can:
        - return verdict
        - return  text info
        - return MD5 signature
        - build image dump with indicated source image/symbols/words/lines
          (for using dumps define WITH_DUMP)
        - build html-report with dump of image, source image and result info
          (if html-report already exist, new data appends to the end of report)
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifndef __osr_expert_h__
#define __osr_expert_h__
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_common.h"
#include "osr_image_analyzer.h"
#include "osr_text_analyzer.h"
//--------------------------------------------------------------------------------------------------------------------//
#define DEFAULT_MIN_TEXT_PERCENTAGE     18

class ImageAnalyzer;
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
const char* GetOsrVersionText ();
float GetOsrVersion ();

class Expert
{
public:

    struct Filter
    {
        ImageAnalyzer::Filter symbols_filter;
        TextAnalyzer::Filter  text_filter;

        size_t  min_text_percentage;

        Filter ()
        {
            min_text_percentage = DEFAULT_MIN_TEXT_PERCENTAGE;
        }
    };
    enum BckgrDetectingMode
    {
        SET_BACKGROUND_AUTO,
        SET_BACKGROUND_BLACK,
        SET_BACKGROUND_WHITE
    };

    /*Initializing interface */

    Expert ();
    virtual ~Expert ();

    void SetFilter (const Filter& filter_arg /*IN*/) // change default filter
        {filter = filter_arg;}
    void SetBckgrDetectingMode (const BckgrDetectingMode bckgr_detecting_mode_arg ) // (auto-detecting by default)
        {bckgr_detecting_mode = bckgr_detecting_mode_arg;}

    /* Processing interface */

    ErrCode Process (InputBWImage *image /*IN*/); /* available:
                                                     InputRGBImage 
                                                     InputGrayImage (suitable for black-white)
                                                     InputDataOfImageFile (file_name)
                                                     InputDataOfImageFile (file_content)
                                                     see osr_image.h */
    
    /* Resulting interface */

    ErrCode GetVerdict        (bool& is_text /*OUT*/) const;

    ErrCode GetSymbolsCount   (size_t& count /*OUT*/) const;
    ErrCode GetWordsCount     (size_t& count /*OUT*/) const;
    ErrCode GetLinesCount     (size_t& count /*OUT*/) const;

    ErrCode GetWordsCount     (const size_t line_num, size_t& count /*OUT*/) const;

    ErrCode GetWordSize       (const size_t line_num, const size_t word_num, size_t& size /*OUT*/) const;

    ErrCode GetWordsPixelsPercentage  (size_t& percents /*OUT*/) const;

    ErrCode GetGSG7Signature  ( GSG7Signature& signature /*OUT*/) const;
    
    ErrCode GetImageSize      ( size_t& image_width /*OUT*/, size_t& image_height /*OUT*/) const;

#ifdef WITH_DUMP

    ErrCode GetDump  ( const char* file_name /*IN*/,
                       const bool dump_image, const bool dump_symbols,
                       const bool dump_lines, const bool dump_words) const;

    // html get source image, dump image and result info about text
    ErrCode GetHtmlReport ( const char *image_file /*IN*/,
                            const char *dump_file /*IN*/, const char *report_file /*IN*/,
                            const ErrCode last_err,
                            const size_t frame_index = 0,
                            const bool* verdict_arg = NULL /*IN*/,
                            const size_t* good_symbols_arg = NULL /*IN*/,
                            const size_t* good_words_arg = NULL /*IN*/,
                            const size_t* good_lines_arg = NULL /*IN*/,
                            const size_t* text_percentage_arg = NULL /*IN*/,
                            const GSG7Signature* signature_arg = NULL  /*IN*/) const;
    static ErrCode SeparateHtml  ( const char *report_file /*IN*/, const size_t html_separate_step /*IN*/);

#endif /*WITH_DUMP*/

private:
    Expert (const Expert&);
    Expert& operator= (const Expert&);

    Filter filter;
    ImageAnalyzer image_analyzer;
    TextAnalyzer text_analyzer;

    const InputBWImage *src_image;

    Color background;
    BckgrDetectingMode bckgr_detecting_mode;

    long processing_time;

};
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
#endif // __osr_expert_h__
//--------------------------------------------------------------------------------------------------------------------//
