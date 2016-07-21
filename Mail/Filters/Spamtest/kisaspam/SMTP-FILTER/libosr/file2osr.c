/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: file2osr.c
  Created: 2006/09/08
  Desc: tool for testing libosr library (see osr_expert.h).
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef _MSC_VER
# pragma warning( disable : 4514 4505 4127 4702) 
#endif // _MSC_VER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osr_c.h"

#define HTML_SEPARATE_STEP  20
#define LEARN_USAGE_ERR_CODE  10

/*--------------------------------------------------------------------------------------------------------------------*/
static void print_usage (const char *program_name)
{
    printf ("Usage:\n"
        "%s [-fr i] [-i] [-v] [-s] [-di] [-ds] [-dl] [-dw] [-da] [-html report_table.html] [-ne] [-ver] [-h] image_file\n"
        "or\n"
        "tes_osr -separate_html report_table.html [N=%d]\n"
        "\n"
        "options:\n"
        "-fr i  - process frame with zero-based index \"i\" of animated image\n"
        "-i     - print all info about text\n"
        "-v     - print verdict (\"VERDICT: text\" or \"VERDICT: not text\")\n"
        "-s     - print GSG7 signature of result text, if text has been detected\n"
        "-di    - indicate black-white source image in the dump\n"
        "-ds    - indicate symbols in the dump\n"
        "-dl    - indicate lines in the dump\n"
        "-dw    - indicate words in the dump\n"
        "-da    - indicate all\n"
        "       \\ image_file_osr_dump.png created when one of dumps has been queried\n"
        "-html  - create or add to html-report of results\n"
        "-ne    - no print errors\n"
        "-ver   - print version of libosr\n"
        "-h     - print usage\n"
        "\n"
        "-separate_html - separate html-report per N lines\n"
        ,
        program_name,
        HTML_SEPARATE_STEP
        );
}
/*--------------------------------------------------------------------------------------------------------------------*/
typedef struct
{
    const char* file_name;

    char print_info;
    char print_verdict;
    char print_signature;
    char dump_image, dump_symbols, dump_lines, dump_words;
    const char* html_report;
    const char* separate_html;
    size_t html_separate_step;
    char no_print_err;
    // hided options:
    char win_group;     // hided option (need only for some tests)
    char ignore_filter; // hided option (need only for some tests)
    char print_ver;
    char print_usage;
    unsigned int frame_index;
}params_t;
static params_t* params_create ();
static void params_destroy (params_t* par);
static int  parse_command_line (int argc, char **argv, params_t *par);
static void get_dump_name(char* new_file_name, int limit, const char* file_name);
static const char* get_program_name(char* argv0);
/*--------------------------------------------------------------------------------------------------------------------*/
int main (int argc, char **argv)
{
    params_t *par = 0;
    osr_err_code err = OSR_OK;
    osr_expert *expert = 0;
    int parse_result = 0;

    par = params_create();
    parse_result = parse_command_line (argc, argv, par);
    if (parse_result)
    {
        params_destroy(par);
        if (LEARN_USAGE_ERR_CODE == parse_result)
        {
            print_usage(get_program_name(argv[0]));
            return 0;
        }
        printf ("Error: can't parse command line.\n");
        return 1;
    }

    if (par->print_usage)
        printf ("libosr version %s\n", osr_version_text() );
    if (par->print_ver)
        printf ("libosr version %s\n", osr_version_text() );
	
    if (par->separate_html)
    {
        err =  osr_separate_html (par->separate_html, par->html_separate_step);
        params_destroy(par);
        if (OSR_OK == err)
            return 0;
        else
            return 1;
    }

    if ( OSR_OK == err )
    {
        expert = osr_expert_create();
        if (0 == expert)
        {
            if (!par->no_print_err)
                printf ("Error: OSR initializing error, %s.\n", osr_get_error_text(err));
            err = OSR_INVALID_DATA;
        }
        if (par->ignore_filter)
        {
            osr_filter unlim;
            osr_get_default_filter(&unlim);
            unlim.min_symbols_in_word=1,
            unlim.max_symbols_in_word=1000,
            unlim.min_words_in_line=1,
            unlim.min_lines_in_text=1,
            unlim.min_text_percentage=1;

            osr_set_filter(expert, &unlim);
        }
    }

    if ( OSR_OK == err )
    {
        unsigned int last_frame_index = 0;
        err = osr_process_file(expert, par->file_name, par->frame_index, &last_frame_index);
        if ( OSR_OK != err )
            if (!par->no_print_err)
            {
                if ( OSR_WRONG_FRAME_INDEX == err )
                    printf ("Error: OSR can't get frame %d of animated image."
                            " It must be [0;%d]. File %s.",
                            par->frame_index, last_frame_index, par->file_name);
                else
                    printf ("Error: OSR can't process image (file %s), %s.\n",
                            par->file_name, osr_get_error_text(err));
            }
    }

    if ( OSR_OK == err )
    {
        if (par->print_verdict)
        {
            char verdict = 0;
            err = osr_get_verdict(expert, &verdict);
            if (OSR_OK == err)
                printf ("VERDICT: %s\n", verdict ? "text" : "not text");
            else
                if (!par->no_print_err)
                    printf ("Error: OSR can't get verdict (file %s), %s.\n",
                            par->file_name, osr_get_error_text(err));
        }
    }

    if ( OSR_OK == err )
    {
        if (par->print_info)
        {
            size_t symbols_count = 0, words_count = 0, lines_count = 0, words_pixels_percentage = 0;
            if (OSR_OK == err)
                err = osr_get_symbols_count(expert, &symbols_count);
            if (OSR_OK == err)
                err = osr_get_words_count (expert, &words_count);
            if (OSR_OK == err)
                err = osr_get_lines_count (expert, &lines_count);
            if (OSR_OK == err)
                err = osr_get_words_pixels_percentage (expert, &words_pixels_percentage);
            if (OSR_OK == err)
            {
                printf ("TEXT INFO:\n");
                if (par->frame_index > 0)
                    printf (
                        "frame index            = %d\n", par->frame_index);
                printf ("good symbols count     = %d\n"
                        "good words count       = %d\n"
                        "good lines count       = %d\n"
                        "good pixels percentage = %d%%\n",
                        symbols_count, words_count, lines_count, words_pixels_percentage);
            }
            else
                if (!par->no_print_err)
                    printf ("Error: OSR can't get text properties (file %s), %s.\n",
                            par->file_name, osr_get_error_text(err));
        }
    }

    if ( OSR_OK == err )
    {
        if (par->print_signature)
        {
            char gsg7[33];
            err = osr_get_GSG7_signature(expert, gsg7);
            if (OSR_OK == err)
            {
                if ( 0 != gsg7[0] )
                    printf ("GSG7 %.*s\n", 32, gsg7);
            }
            else
                if (!par->no_print_err)
                    printf ("Error: OSR can't get GSG7 (file %s), %s.\n",
                            par->file_name, osr_get_error_text(err));
        }
    }

    if ( OSR_OK == err )
    {
        if (par->dump_image || par->dump_lines || par->dump_symbols || par->dump_words)
        {
            char dump_name [500];
            get_dump_name (dump_name, 500, par->file_name);
            err = osr_get_dump (expert, dump_name, par->dump_image, par->dump_symbols, par->dump_lines, par->dump_words);
            
            if (OSR_OK != err)
                if (!par->no_print_err)
                    printf ("Error: OSR can't get dump of file %s, %s.\n",
                            par->file_name, osr_get_error_text(err));
        }
    }
    if (par->html_report)
    {
        char dump_name [500];
        get_dump_name (dump_name, 500, par->file_name);

        err = osr_get_html_report(expert, par->file_name, dump_name, par->html_report, err, par->frame_index);
        if (OSR_OK != err)
            if (!par->no_print_err)
                printf ("Error: OSR can't get html-report (image file %s), %s.\n",
                        par->file_name, osr_get_error_text(err));
    }

    if (OSR_OK == err )
        if (par->win_group)
        {
            char gsg7[33];
            err = osr_get_GSG7_signature(expert, gsg7);
            if (OSR_OK == err)
            {
                if (0 == gsg7[0])
                    strncpy (gsg7, "_not_text", 32);
                printf ("@ if not exist %s mkdir %s\n"
                        "@ move \"%s\" %s\n",
                            gsg7, gsg7, par->file_name, gsg7);
            }
            else
                if (!par->no_print_err)
                    printf ("Error: OSR can't get GSG7 (file %s), %s.\n",
                            par->file_name, osr_get_error_text(err));
        }
    
    if (par)
        params_destroy(par);
    if (expert)
        osr_expert_destroy(expert);

    if (OSR_OK == err)
        return 0;
    return 1;
}
/*--------------------------------------------------------------------------------------------------------------------*/
params_t* params_create ()
{
    params_t *ret = (params_t*)calloc(1, sizeof (params_t));
    if (ret)
        ret->html_separate_step = HTML_SEPARATE_STEP;
    return ret;
}
/*--------------------------------------------------------------------------------------------------------------------*/
void params_destroy (params_t* par)
{
    if (par)
        free (par);
}
/*--------------------------------------------------------------------------------------------------------------------*/
const char* get_program_name(char* argv0)
{
    char *program_name = 0;
    
    program_name = strrchr (argv0, '/');
    if (program_name)
        program_name ++;
    else
    {
        program_name = strrchr (argv0, '\\');
        if (program_name)
            program_name ++;
        else
            program_name = argv0;
    }
    return program_name;
}
/*--------------------------------------------------------------------------------------------------------------------*/
static int parse_command_line (int argc, char **argv, params_t *par)
{
    int i = 0;

    if ( argc <= 1 )
        return LEARN_USAGE_ERR_CODE;

    for (i = 1; i < argc; i ++ )
    {
        if ( 0 == strcmp("-i", argv[i] ) )
            par->print_info = 1;
        else if ( 0 == strcmp("-v", argv[i] ) )
            par->print_verdict = 1;
        else if ( 0 == strcmp("-s", argv[i] ) )
            par->print_signature = 1;
        else if ( 0 == strcmp("-di", argv[i] ) )
            par->dump_image = 1;
        else if ( 0 == strcmp("-ds", argv[i] ) )
            par->dump_symbols = 1;
        else if ( 0 == strcmp("-dl", argv[i] ) )
            par->dump_lines = 1;
        else if ( 0 == strcmp("-dw", argv[i] ) )
            par->dump_words = 1;
        else if ( 0 == strcmp("-da", argv[i] ) )
            par->dump_image = par->dump_symbols = par->dump_lines = par->dump_words = 1;
        else if ( 0 == strcmp("-html", argv[i] ) && i+1 < argc )
            par->html_report = argv[++i];
        else if ( 0 == strcmp("-separate_html", argv[i] ) && i+1 < argc )
        {
            par->separate_html = argv[++i];
            if ( i+1 < argc )
                par->html_separate_step = atoi (argv[++i]);
        }
        else if ( 0 == strcmp("-ne", argv[i] ) )
            par->no_print_err = 1;
        else if ( 0 == strcmp("-win_group", argv[i] ) )
            par->win_group = 1;
        else if ( 0 == strcmp("-ignore_filter", argv[i] ) )
            par->ignore_filter = 1;
        else if ( 0 == strcmp("-ver", argv[i] ) )
            par->print_ver = 1;
        else if ( 0 == strcmp("-h", argv[i] ) )
            return LEARN_USAGE_ERR_CODE;
        else if ( 0 == strcmp("-fr", argv[i] ) && i+1 < argc )
            par->frame_index = atoi (argv[++i]);
        else if ( '-' == argv[i][0])
            printf ("Warning: invalid option %s has been ignored.\n", argv[i]);
        else
            if (argc - 1 == i)
                par->file_name = argv[i];

    }

    if(par->html_report && !( par->dump_image || par->dump_symbols || par->dump_lines || par->dump_words ) )
    {
        printf ("Warning: -html option available with option -dump only.\n");
        return 1;
    }

    if (0 == par->file_name)
        return 1;

    return 0;
}
/*--------------------------------------------------------------------------------------------------------------------*/
static void get_dump_name (char* new_file_name, int limit, const char* file_name)
{
    char *dot = 0;
    strncpy (new_file_name, file_name, limit);
    dot = strrchr (new_file_name, '.');
    if (dot)
        *dot = 0;
    else
        dot = new_file_name + strlen (new_file_name);
    strncat (dot, "_osr_dump.png", limit-(dot-new_file_name));
}
/*--------------------------------------------------------------------------------------------------------------------*/
