/*******************************************************************************
 * Proj: Content Filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: test_parser.cpp                                                       *
 * Created: Fri Feb 22 20:51:28 2002                                           *
 * Desc: Sample application of MIMEParser.                                     *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  test_parser.cpp
 * \brief Sample application of MIMEParser.
 */

#include "MIMEParser.hpp"

#include <ctype.h>
#include <stdio.h>

#define DUMP_HEADERS

struct my_ParserConfig
{
    void header_continue();
    void header_name(const char* p, size_t count);
    void header_body(const char* p, size_t count, const char* codepage, size_t cp_count);
    void part_begin(unsigned int level);
#ifdef MIME_PARSER_WITH_UUE_DECODER
    void uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length);
#endif
    void epilogue(unsigned int level);
    void body(const char* p, size_t count, bool eol);

    void content_type(const char* p, size_t count);
    void content_subtype(const char* p, size_t count);
    void content_charset(const char* p, size_t count);
    void content_full_type(const char* p, size_t count);

#ifdef MIME_PARSER_CONTENT_TYPE_EXTENSIONS
    void content_name(const char* p, size_t count);
    void content_protocol(const char* p, size_t count);
#endif

    bool awaiting_lines;
    bool awaiting_disposition;

    size_t lines;
    size_t cur_lines;

    bool body_processing;

    enum {
	HDR_NONE,
	HDR_LINES,
	HDR_CONTENT_DISPOSITION
    } cur_header;
};

inline void my_ParserConfig::content_full_type(const char* p, size_t count)
{
}

void print_string(const char* p, size_t count)
{
    for(size_t i = 0; i < count; i++)
	putc(p[i], stdout);
}

bool compare_mem(const char* s1, const char* s2, size_t count)
{
    for(size_t i = 0; i < count; i++)
	if(tolower(s1[i]) != tolower(s2[i]))
	    return false;

    return true;
}

void my_ParserConfig::content_type(const char* p, size_t count)
{
}

void my_ParserConfig::content_subtype(const char* p, size_t count)
{
}

void my_ParserConfig::content_charset(const char* p, size_t count)
{
}

#ifdef MIME_PARSER_CONTENT_TYPE_EXTENSIONS

void my_ParserConfig::content_name(const char* p, size_t count)
{
}

void my_ParserConfig::content_protocol(const char* p, size_t count)
{
}

#endif

void my_ParserConfig::header_continue()
{
}

void my_ParserConfig::header_name(const char* p, size_t count)
{
    cur_header = HDR_NONE;

    if(!body_processing)
	{
	    if(count == sizeof("Lines")-1 && memcmp(p, "Lines", count) == 0)
		cur_header = HDR_LINES;
	}

    if(count == sizeof("Content-Disposition")-1 && compare_mem(p, "Content-Disposition", count) == 0)
	cur_header = HDR_CONTENT_DISPOSITION;

#ifdef DUMP_HEADERS
    print_string(p, count);
    printf("\n");
#endif
}

void my_ParserConfig::header_body(const char* p, size_t count, const char* codepage, size_t cp_count)
{
    switch(cur_header)
	{
	case HDR_LINES:

	    size_t i;

	    for(i = 0; i < count && isspace(p[i]); i++)
		;

	    for( ; i < count && isdigit(p[i]); i++)
		lines = lines*10 + p[i] - '0';
	    cur_lines = 0;

	    break;

	case HDR_CONTENT_DISPOSITION:

	    break;
	}


#ifdef DUMP_HEADERS
    printf("\t");

    if(codepage)
	{
	    printf("[");
	    print_string(codepage, cp_count);
	    printf("] ");
	}
    
    print_string(p, count);
    printf("\n");
#endif
}

void my_ParserConfig::part_begin(unsigned int level)
{
    if(!body_processing)
	body_processing = true;

    for(size_t i = 0; i < level; i++)
	printf("  ");

    printf("part at level %i\n", level);
}

#ifdef MIME_PARSER_WITH_UUE_DECODER
void my_ParserConfig::uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length)
{
    if(!body_processing)
	body_processing = true;

    for(size_t i = 0; i < level; i++)
	printf("  ");

    printf("uue part at level %i\n", level);
}
#endif

void my_ParserConfig::epilogue(unsigned int level)
{
    if(!body_processing)
	body_processing = true;

    for(size_t i = 0; i < level; i++)
	printf("  ");
    printf("epilogue at level %i\n", level);
}

void my_ParserConfig::body(const char* p, size_t count, bool eol)
{
    if(!body_processing)
	body_processing = true;
}

int main(int argc, char* argv[])
{
    char buf[40960];

    my_ParserConfig pc;
    MIMEParser<my_ParserConfig> parser(&pc);

    memset(&pc, 0, sizeof(pc));
    
    FILE* mailbox = fopen((argc < 2) ? "test_mailbox" : argv[1], "rb");

    if(!mailbox)
	{
	    printf("Can\'t open mailbox.\n");
	    return 1;
	}

    unsigned int count = 0;
    char *p;

    bool skip_line = true;

    for( ; ; )
	{
	    if(fgets(buf, sizeof(buf), mailbox) == NULL)
		break;

	    if(skip_line) 
		{
		    skip_line = false;
		    continue;
		}

	    p = strchr(buf, '\n'); if(p) *p = 0;
	    p = strchr(buf, '\r'); if(p) *p = 0;

	    parser.parse(buf, strlen(buf));

	    if(pc.body_processing)
		{
		    pc.cur_lines++;
		    if(pc.cur_lines > pc.lines)
			{
			    count++;
			    printf("%i\n", count);

			    memset(&pc, 0, sizeof(pc));
			    parser.recycle();

			    skip_line = true;
			    pc.cur_lines = 0;
			}
		}
	}

    fclose(mailbox);

    return 0;
}


/*
 * <eof test_parser.cpp>
 */
