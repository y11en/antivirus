/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_expert.cpp
  Created: 2006/09/08
  Desc: see osr_expert.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_expert.h"
#include "osr_version.h"
#ifdef WITH_DUMP
# include "osr_dump.h"
#endif // WITH_DUMP
#include <smtp-filter/libsigs/libsigs.h>
#if defined (OS_LINUX) || defined (_WIN32)
# include <gnu/md5a/md5a.h>
#else
# include <md5.h>
#endif
#ifndef _WIN32
# include <sys/time.h>
#else
# include <windows.h>
#endif
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
const char* GetOsrVersionText ()
{
    return OSR_VER_TXT;
}
//--------------------------------------------------------------------------------------------------------------------//
float GetOsrVersion ()
{
    return (float)OSR_VER;
}
//--------------------------------------------------------------------------------------------------------------------//
Expert::Expert()
{
    src_image = NULL;
    background = WHITE;
    bckgr_detecting_mode = SET_BACKGROUND_AUTO;
    processing_time = 0;
}
//--------------------------------------------------------------------------------------------------------------------//
Expert::~Expert()
{
}
//--------------------------------------------------------------------------------------------------------------------//
static long GetMilliSeconds()
{
#ifdef _WIN32
    return (long)GetTickCount();
#else
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec*1000 + tp.tv_usec;
#endif // _WIN32
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::Process (InputBWImage* image /*IN*/)
{
    if (NULL == image)
        return OSR_INVALID_ARGUMENT;

    long t_0 = GetMilliSeconds();

    src_image = image;

    ErrCode err = OSR_OK;
    
    Size width = 0, height = 0;

    err = image->PreProcess();
    if ( OSR_OK != err )
        return err;

    err = src_image->GetSize(width, height);
    if ( OSR_OK != err )
        return err;

    err = OSRCheckImageSizeLimits (width, height);
    if ( OSR_OK != err )
        return err;

    image_analyzer.SetFilter (filter.symbols_filter);

    err = image_analyzer.Load(src_image);
    if ( OSR_OK != err )
        return err;

    if ( SET_BACKGROUND_BLACK == bckgr_detecting_mode )
        image_analyzer.SetBackground (BLACK);
    else if ( SET_BACKGROUND_WHITE == bckgr_detecting_mode )
        image_analyzer.SetBackground (WHITE);
    else
        err = image_analyzer.AutodetectBackground(&background);
    if ( OSR_OK != err )
        return err;
    
    err = image_analyzer.RecognizeSymbols();
    if ( OSR_OK != err )
        return err;

    text_analyzer.SetFilter (filter.text_filter);

    err = text_analyzer.AnalyzeText( image_analyzer.GetSymbols() );
    if ( OSR_OK != err )
        return err;

    long t_1 = GetMilliSeconds();

    processing_time = t_1 - t_0;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetVerdict (bool& is_text) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;

    size_t words_pixels_percentage = 0;
    ErrCode err = OSR_OK;
    err = GetWordsPixelsPercentage  (words_pixels_percentage);
    if (OSR_OK != err)
        return err;

    is_text = ( text_analyzer.GetLines()->Size() > 0 &&
                words_pixels_percentage >= filter.min_text_percentage );
    
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetSymbolsCount (size_t& count) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    count = 0;
    const Lines& lines = *text_analyzer.GetLines();
    for (size_t l = 0; l < lines.Size(); l ++)
        for (size_t w = 0; w < lines[l].words.Size(); w ++)
            count += lines[l].words[w].symbols.Size();
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetWordsCount (size_t& count) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    count = 0;
    const Lines& lines = *text_analyzer.GetLines();
    for (size_t l = 0; l < lines.Size(); l ++)
        count += lines[l].words.Size();
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetLinesCount (size_t& count) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    count = text_analyzer.GetLines()->Size();
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetWordsCount (const size_t line_num, size_t& count) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    const Lines& lines = *text_analyzer.GetLines();
    if ( line_num >= lines.Size() )
        return OSR_INVALID_ARGUMENT;
    count = lines[line_num].words.Size();
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetWordSize (const size_t line_num, const size_t word_num, size_t& size) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    const Lines& lines = *text_analyzer.GetLines();
    if ( line_num >= lines.Size() || word_num >= lines[line_num].words.Size() )
        return OSR_INVALID_ARGUMENT;
    size = lines[line_num].words[word_num].symbols.Size();
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetWordsPixelsPercentage  (size_t& percents) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    ErrCode err = OSR_OK;
    Size width = 0, height = 0;
    err = src_image->GetSize(width, height);
    if ( OSR_OK != err )
        return err;
    if (  width <= 0 || height <= 0 )
        return OSR_INVALID_DATA;
    const Lines& lines = *text_analyzer.GetLines();
    Color color;
    size_t symbol_pixels = 0;
    for (size_t l = 0; l < lines.Size(); l ++)
        for (size_t w = 0; w < lines[l].words.Size(); w ++)
            for (size_t s = 0; s < lines[l].words[w].symbols.Size(); s ++)
                symbol_pixels += lines[l].words[w].symbols[s]->GetPixelCount();
    if (0 == symbol_pixels)
    {
        percents = 0;
        return OSR_OK;
    }
    size_t foreground_pixels = 0;
    for(Size y = 0; y < height; y ++)
        for(Size x = 0; x < width; x ++)
        {
            err = src_image->GetPixel(x, y, color);
            if (OSR_OK != err)
                return err;
            if (background != color)
                foreground_pixels ++;
        }
    if ( 0 == foreground_pixels )
        percents = 0;
    else
        percents = symbol_pixels * 100 / foreground_pixels;
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetGSG7Signature ( GSG7Signature& signature ) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;

    ErrCode err = OSR_OK;
    bool is_text = false;
    err = GetVerdict(is_text);
    if (OSR_OK != err)
        return err;
    if (!is_text)
    {
        signature[0] = 0;
        return OSR_OK;
    }
    const Lines& lines = *text_analyzer.GetLines();

    size_t l = 0, w = 0, l2 = 0, w2 = 0, n = 0;
    size_t i = 0, j = 0;

    size_t words_count = 0;
    err = GetWordsCount (words_count);
    if (OSR_OK != err)
        return err;

    size_t summand_count = words_count / 10;
    size_t blur = 2;
    size_t step = summand_count/2;
    size_t rounding = summand_count * blur;

    if ( 0 == step || 0 == summand_count || 0 == rounding)
    {
        summand_count = 4;
        step = 2;
        rounding = 2;
    }

    MD5_CTX ctx;
    md5sum md5;

    bzero(&ctx,sizeof(ctx));
    MD5Init(&ctx);

    for (l = 0; l < lines.Size(); l ++)
        for (w = 0; w < lines[l].words.Size(); w ++)
        {
            if (0 == (i % step))
            {
                j = 0;
                n = 0;
                for (l2 = l;  j < summand_count && l2 < lines.Size(); l2 ++)
                    for (w2 = w; j < summand_count && w2 < lines[l2].words.Size(); w2 ++)
                    {
                        n += lines[l2].words[w2].symbols.Size();
                        j ++;
                    }
                n /= rounding;
                MD5Update(&ctx, (unsigned char*)&n, sizeof (n) );
            }
            i ++;
        }
    
    MD5Final(md5,&ctx);
    md52str(signature, md5);
    signature[GSG7_SIGNATURE_LENGHT-1] = 0;
    
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetImageSize ( size_t& image_width /*OUT*/, size_t& image_height /*OUT*/) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;
    Size w = 0, h = 0;
    ErrCode err = src_image->GetSize(w, h);
    if (OSR_OK == err)
    {
        image_width = w;
        image_height = w;
    }
    return err;
}
//--------------------------------------------------------------------------------------------------------------------//
#ifdef WITH_DUMP
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetDump  (  const char* file_name,
                            const bool dump_image, const bool dump_symbols,
                            const bool dump_lines, const bool dump_words) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;

    if ( NULL == file_name )
        return OSR_INVALID_ARGUMENT;

    Size x= 0, y = 0;
    Size width = 0, height = 0;
    ErrCode err = OSR_OK;
    err = src_image->GetSize(width, height);
    if ( OSR_OK != err )
        return err;

    Dump dump (width, height);

    RGB background_color = {255, 255, 255}, foreground_color = {0, 0, 0},
        symbol_color = {0, 0, 255}, word_color = {255, 0, 0}, line_color = {0, 170, 0},
        symbol_center_color = {0, 255, 0};
    Color color;
    
    const Lines& lines = *text_analyzer.GetLines();
    size_t l = 0, w = 0, s = 0;
    
    for ( y = 0; y < height; y ++ )
        for ( x = 0; x < width; x ++ )
        {
            if (dump_image)
            {
                err = src_image->GetPixel(x, y, color);
                if (OSR_OK != err)
                    return err;

                if (WHITE == color)
                    err = dump.SetPixel( x, y, background_color);
                else if (BLACK == color)
                    err = dump.SetPixel( x, y, foreground_color);
                else
                    return OSR_INTERNAL_ERROR;
            }
            else
                err = dump.SetPixel( x, y, background_color);

            if (OSR_OK != err)
                return err;
        }
    if (dump_symbols )
        for (l = 0; l < lines.Size(); l ++)
            for (w = 0; w < lines[l].words.Size(); w ++)
                for (s = 0; s < lines[l].words[w].symbols.Size(); s ++)
                {
                    const Symbol* symbol = lines[l].words[w].symbols[s];
                    err = dump.DrawRect ( symbol->Left(), symbol->Top(),
                                          symbol->Right(), symbol->Bottom(),
                                          symbol_color);
                    if (OSR_OK != err)
                        return err;
                }
                        
    if (dump_lines)
        for (l = 0; l < lines.Size(); l ++)
        {
            for (w = 0; w < lines[l].words.Size(); w ++)
            {
                for (s = 1; s < lines[l].words[w].symbols.Size(); s ++)
                {
                    const Symbol* symbol0 = lines[l].words[w].symbols[s-1];
                    const Symbol* symbol1 = lines[l].words[w].symbols[s];
                    err = dump.DrawLine ( symbol0->GetCenterX(), symbol0->GetCenterY(),
                                          symbol1->GetCenterX(), symbol1->GetCenterY(),
                                          line_color);
                    if (OSR_OK != err)
                        return err;
                }
                if (w > 0)
                {
                    const Symbol* symbol0 = *lines[l].words[w-1].symbols.GetLast();
                    const Symbol* symbol1 = *lines[l].words[w].symbols.GetFirst();
                    if ( NULL != symbol0 && NULL != symbol1 )
                    {
                        err = dump.DrawLine ( symbol0->GetCenterX(), symbol0->GetCenterY(),
                                              symbol1->GetCenterX(), symbol1->GetCenterY(),
                                              line_color);
                        if (OSR_OK != err)
                            return err;
                    }
                }
            }
            for (w = 0; w < lines[l].words.Size(); w ++)
                for (s = 0; s < lines[l].words[w].symbols.Size(); s ++)
                {
                    const Symbol* symbol = lines[l].words[w].symbols[s];
                    err = dump.SetPixel( symbol->GetCenterX(),
                                         symbol->GetCenterY(), symbol_center_color);
                    if (OSR_OK != err)
                        return err;
                }
        }

        if (dump_words)
            for (l = 0; l < lines.Size(); l ++)
                for (w = 0; w < lines[l].words.Size(); w ++)
                {
                    if (lines[l].words[w].symbols.Size())
                    {
                        const Symbol* symbol = *lines[l].words[w].symbols.GetFirst();
                        err = dump.DrawLine ( symbol->Left(), symbol->Top(),
                                              symbol->Left(), symbol->Bottom(),
                                              word_color);
                        if (OSR_OK != err)
                            return err;
                    }
                    for (s = 0; s < lines[l].words[w].symbols.Size(); s ++)
                    {
                        const Symbol* symbol = lines[l].words[w].symbols[s];
                        err = dump.DrawLine ( symbol->Left(), symbol->Top(),
                                              symbol->Right(), symbol->Top(),
                                              word_color);
                        if (OSR_OK != err)
                            return err;
                        err = dump.DrawLine ( symbol->Left(), symbol->Bottom(),
                                              symbol->Right(), symbol->Bottom(),
                                              word_color);
                        if (OSR_OK != err)
                            return err;

                        if (s > 0)
                        {
                            const Symbol* symbol0 = lines[l].words[w].symbols[s-1];
                            const Symbol* symbol1 = lines[l].words[w].symbols[s];
                            err = dump.DrawLine ( symbol0->Right(), symbol0->Top(),
                                                  symbol1->Left(), symbol1->Top(),
                                                  word_color);
                            if (OSR_OK != err)
                                return err;
                            err = dump.DrawLine ( symbol0->Right(), symbol0->Bottom(),
                                                  symbol1->Left(), symbol1->Bottom(),
                                                  word_color);
                            if (OSR_OK != err)
                                return err;
                        }
                    }
                    if (lines[l].words[w].symbols.Size())
                    {
                        const Symbol* symbol = *lines[l].words[w].symbols.GetLast();
                        err = dump.DrawLine ( symbol->Right(), symbol->Top(),
                                              symbol->Right(), symbol->Bottom(),
                                              word_color);
                        if (OSR_OK != err)
                            return err;
                    }
                }

    err = dump.Save (file_name);
    return err;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::GetHtmlReport ( const char *image_file ,
                                const char *dump_file, const char *report_file,
                                const ErrCode last_err,
                                const size_t frame_index,
                                const bool* verdict_arg,
                                const size_t *good_symbols_arg,
                                const size_t *good_words_arg,
                                const size_t *good_lines_arg,
                                const size_t *text_percentage_arg,
                                const GSG7Signature * signature_arg ) const
{
    if ( NULL == src_image )
        return OSR_INVALID_DATA;

    size_t good_symbols = 0, good_words = 0, good_lines = 0, text_percentage = 0;
    bool verdict = false;
    GSG7Signature signature = "";
    
    ErrCode err = OSR_OK;

    if (OSR_OK == last_err)
    {
        if (verdict_arg && OSR_OK == err)
            verdict = *verdict_arg;
        else
            err = GetVerdict(verdict);

        if (good_symbols_arg && OSR_OK == err)
            good_symbols = *good_symbols_arg;
        else
            err = GetSymbolsCount(good_symbols);

        if (good_words_arg && OSR_OK == err)
            good_words = *good_words_arg;
        else
            err = GetWordsCount(good_words);

        if (good_lines_arg && OSR_OK == err)
            good_lines = *good_lines_arg;
        else
            err = GetLinesCount(good_lines);

        if (text_percentage_arg && OSR_OK == err)
            text_percentage = *text_percentage_arg;
        else
            err = GetWordsPixelsPercentage (text_percentage);

        if (signature_arg && OSR_OK == err)
            memcpy (signature, signature_arg, GSG7_SIGNATURE_LENGHT);
        else
            err = GetGSG7Signature(signature);
    
        if (OSR_OK != err)
            return err;
    }

    FILE *html_report_f = fopen (report_file, "r");

    const char* tmp_name = "html_report.tmp";
    FILE *tmp_f = fopen (tmp_name, "w");
    if (NULL == tmp_f)
    {
        if (html_report_f)
            fclose (html_report_f);
        return OSR_FILE_ERROR;
    }
    const char *stop = "</table></body></html>";
    char line [5048];
    if (html_report_f)
    {
        while ( fgets (line, sizeof(line), html_report_f) &&
            0 != strncmp (stop, line, sizeof (stop) - 1 ) )
            fputs (line, tmp_f);
    }
    else
        fprintf (tmp_f, "<html><body><table border=1>\n");
    
    fprintf (tmp_f, "<tr>");

    if (OSR_OK == last_err)
    {
        fprintf (tmp_f, "<td valign=\"top\"><img src=\"%s\"/>"
                        "<HR>%s<br>",
                            dump_file, dump_file );
        if (frame_index > 0)
            fprintf (tmp_f, 
                         "frame index %d<br>", frame_index );
        fprintf (tmp_f, "%s<br>"
                        "good symbols count = %d<br>"
                        "good words count   = %d<br>"
                        "good lines count   = %d<br>"
                        "good pixels        = %d%%<br>"
                        "GSG7: %s<br>"
                        "processed at %.2f sec.<br>"
                        "</td>",
                            verdict ? "<b>TEXT</b>" : "not text",
                            good_symbols,
                            good_words,
                            good_lines,
                            text_percentage,
                            signature,
                            (float)(processing_time)/1000.0
                );
    }
    else
        fprintf (tmp_f, "<td>Error: %s</td>",ErrorText(last_err));

    fprintf (tmp_f, "<td valign=\"top\"><img src=\"%s\"/><HR>%s</td>", image_file, image_file);
    fprintf (tmp_f, "</tr>\n%s\n", stop);
    
    fclose (tmp_f);
    if (html_report_f)
    {
        fclose (html_report_f);
        remove (report_file);
    }
    
    if ( 0 != rename (tmp_name, report_file) )
        return OSR_FILE_ERROR;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode Expert::SeparateHtml ( const char *report_file , const size_t html_separate_step )
{
    if (html_separate_step <= 0 || NULL == report_file)
        return OSR_INVALID_ARGUMENT;

    const unsigned int max_file_name = 500;
    if (strlen (report_file) > max_file_name - 30)
        return OSR_INVALID_ARGUMENT;
    char file_name [max_file_name];
    strcpy (file_name, report_file);
    char *dot = strrchr(file_name, '.');
    if (NULL == dot)
        return OSR_INVALID_ARGUMENT;

    FILE *html_report_f = fopen (report_file, "r");
    if (NULL == html_report_f )
        return OSR_INVALID_ARGUMENT;

    const char *begin = "<html><body><table border=1>";
    const char *end = "</table></body></html>";
    char line [5048];
    if ( NULL == fgets (line, sizeof(line), html_report_f) || 0 != strcmp (begin, line))
        return OSR_INVALID_ARGUMENT;

    FILE *html_part_f = NULL;
    int n = 0;
    while ( fgets (line, sizeof(line), html_report_f) && 0 != strcmp (end, line) )
    {
        if (0 == n % html_separate_step)
        {
            sprintf (dot, "_%.7d.htm", n / html_separate_step + 1);
            if ( html_part_f )
            {
                fprintf (html_part_f, "</table>\n<br><br><a href=\"%s\">next page %s</a></body></html>\n",
                    file_name, file_name);
                fclose (html_part_f);
            }
            html_part_f = fopen (file_name, "w");
            if (NULL == html_part_f )
                return OSR_FILE_ERROR;
            fprintf (html_part_f, "%s\n", begin);
        }
        fputs (line, html_part_f);
        n ++;
    }
    fprintf (html_part_f, "%s\n", end);
    fclose (html_report_f);

    if (html_part_f)
        fclose (html_part_f);
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
#endif /*WITH_DUMP*/
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
