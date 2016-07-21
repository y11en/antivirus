/*******************************************************************************
 * Proj: Content Filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: MIMEParser.hpp                                                        *
 * Created: Wed Feb 20 18:28:52 2002                                           *
 * Desc: MIME Parser (string)                                                  *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  MIMEParser.hpp
 * \brief Stringed MIME Parser.
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef __MIMEParser_hpp__
#define __MIMEParser_hpp__

#define MIME_PARSER_WITH_UUE_DECODER
#define MIME_PARSER_CONTENT_TYPE_EXTENSIONS

#define MIME_PARSER_HAS_START_PARTIAL

/**
 * \brief Hidden global constants.
 *
 */
class MIMEParserGlobal 
{

public:

    enum {
	PARSE_OK = 0,
	PARSE_ERROR,
	PARSE_CONT
    };

protected:

    /*
     * XXX. Later --- all *_config needed to unify.
     */

    enum {
	HDR_UNKNOWN = 0,

	HDR_CONTENT_TYPE,
	HDR_MIME_VERSION,
	HDR_CONTENT_TRANSFER_ENCODING
    };
    
    struct header_config
    {
	const char  *name;
	size_t       length;
	unsigned int ID;
    };

    enum {
	CT_UNKNOWN = 0,

	CT_CHARSET,
	CT_BOUNDARY,
        CT_PROTOCOL,
        CT_NAME
    };

    struct ct_parameter_config
    {
	const char*  name;
	size_t       length;
	unsigned int ID;
    };

    enum {
	TE_UNKNOWN,

	TE_7BIT,
	TE_8BIT,
	TE_BINARY,
	TE_QP,
	TE_B64,
	TE_UUE
    };

    struct transfer_encoding_config
    {
	const char  *name;
	size_t       length;
	unsigned int ID;
    };


    static bool is_letter(int ch);
    static bool is_digit(int ch);
    static bool is_printable(int ch);

    static header_config headers[];
    static ct_parameter_config ct_parameters[];
    static transfer_encoding_config transfer_encodings[];
    static unsigned int check_header(const char* str, size_t str_len);
    static unsigned int check_ct_parameter(const char* str, size_t str_len);
    static unsigned int check_transfer_encoding(const char* str, size_t str_len);
    
    static unsigned char pr2six[256];
    static int mime_parser_decode_b64(const char *in, size_t length, char *out);
};

inline unsigned int MIMEParserGlobal::check_header(const char* str, size_t str_len)
{
    size_t i, j;
    
    for(i = 0; headers[i].name; i++)
	if(str_len == headers[i].length)
	    {
		for(j = 0; j < str_len; j++)
		    if(tolower(str[j]) != tolower(headers[i].name[j]))
			break;

		if(j == str_len)
		    return headers[i].ID;
	    }

    return HDR_UNKNOWN;
}

inline unsigned int MIMEParserGlobal::check_ct_parameter(const char* str, size_t str_len)
{
    size_t i, j;
    
    for(i = 0; ct_parameters[i].name; i++)
	if(str_len == ct_parameters[i].length)
	    {
		for(j = 0; j < str_len; j++)
		    if(tolower(str[j]) != tolower(ct_parameters[i].name[j]))
			break;

		if(j == str_len)
		    return ct_parameters[i].ID;
	    }

    return CT_UNKNOWN;
}

inline unsigned int MIMEParserGlobal::check_transfer_encoding(const char* str, size_t str_len)
{
    size_t i, j;
    
    for(i = 0; transfer_encodings[i].name; i++)
	if(str_len == transfer_encodings[i].length)
	    {
		for(j = 0; j < str_len; j++)
		    if(tolower(str[j]) != tolower(transfer_encodings[i].name[j]))
			break;

		if(j == str_len)
		    return transfer_encodings[i].ID;
	    }

    return TE_UNKNOWN;
}

inline bool MIMEParserGlobal::is_printable(int ch)
{
    return (ch >= 33 && ch <= 126);
}

inline bool MIMEParserGlobal::is_letter(int ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

inline bool MIMEParserGlobal::is_digit(int ch)
{
    return (ch >= '0' && ch <= '9');
}

/**
 * \brief MIME parser itself.
 *
 * ParserConfig is struct, that describe programmer's interface of 
 * the parser.
 * 
 * It must have next member functions:
 *
 *  void header_name(const char* p, size_t count);
 *
 *    --- Called at header name in text. Here and anywhere later in 
 *        parser description, pointers can point to external (for parser) storage (memory,
 *        that have been passed to parse method) or internal. You can check
 *        pointers and use that information.
 * 
 *  void header_body(const char* p, size_t count, const char* codepage, size_t cp_count);
 *
 *    --- Called for header body (for blocks of text). If codepage non-null, 
 *        p points to internal storage of parser. Header_body can be called more than one
 *        time for one header name. And, otherwise, header_body called at once one time
 *        for each header_body.
 *
 *  void header_continue();
 *   
 *    --- Non-used method: called if passed string to parser contain data for 
 *        previous header.
 * 
 *  void part_begin(unsigned int level);
 *
 *   --- Called before part begin of multi-part messages. Note, that
 *       if message containe prolog, that prolog will be passed before
 *       first part_begin. level --- level of hierarchy.
 *  
 *  void uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length);
 *  
 *  --- Called before each uue-encoded data. mode and filename extracted from
 *      "begin 644 filename" string. After uue-encoded data part_begin will be called.
 *      MIMEParser header with this feature includes definition of 
 *      MIME_PARSER_WITH_UUE_DECODER macro.
 *
 *  void epilogue(unsigned int level);
 *
 *   --- Called after last boundary of message, all data after this
 *       call is epilogue and can be ignored.
 *
 *  void body(const char* p, size_t count, bool eol);
 *
 *   --- Called for strings of body. eol is true, than string must be ended by '\n'.
 *       You can use it if p pointed into internal storage, and after p+count 
 *       already exist newline character.
 *
 *  void content_full_type(const char* p, size_t count);
 *  
 *   --- Called for full MIME-type on content-type: "text/html", for example.
 *
 *  void content_type(const char* p, size_t count);
 *
 *   --- Called fo MIME global type: "text"
 *
 *  void content_subtype(const char* p, size_t count);
 *
 *   --- Called for subtype, "html" in previouse examples.
 *
 *  void content_charset(const char* p, size_t count);
 *
 *   --- Called for charset parameter of content-type.
 * 
 * MIMEParser header, with parser which requests next callbacks, defines
 * MIME_PARSER_CONTENT_TYPE_EXTENSIONS:
 *
 *  void content_name(const char* p, size_t count);
 *
 *   --- Called for the name parameter of content-type.
 * 
 *  void content_protocol(const char* p, size_t count);
 *
 *   --- Called for the protocol parameter of content-type.
 * 
 * \note This comment is actual for 21.12.03. 
 *       At this moment test_parser aplication is unsupported (but it is possible
 *       to build this application with MIMEParser.hpp).
 *
 */
template<class ParserConfig>
class MIMEParser : public MIMEParserGlobal
{
public:
    
    MIMEParser(ParserConfig* parser_config);
    ~MIMEParser();

    int parse(const char* line, size_t length);
    void recycle();

    int start_partial() 
        { 
            if(partial_parser) 
                return partial_parser->start_partial(); 
            else 
                {
                    start_partial_parser = true;
                    return 0;
                } 
        }

protected:

    size_t level;

    void set_level(size_t l);


    ParserConfig* pc;

    int parse_headers(const char* line, size_t length);
    int parse_body(const char* line, size_t length);
    int parse_body_simple(const char* line, size_t length);

    int parse_content_type(const char* ctype, size_t length);
    int parse_content_transfer_encoding(const char* tenc, size_t length);

    int process_header_body(const char* p, size_t count);

    int decode_b64(const char* in, size_t length, char* out);
    int decode_qp(const char* in, size_t length, char* out);
    int decode_uue(const char *in, size_t length, char *out);

    void alloc_decode_buffer(size_t s);

    enum {
	GS_HEADERS,
	GS_BODY, 
	GS_BODY_EPILOGUE,
	GS_ERROR
    } global_state;

    enum {
	PCT_START,
	PCT_TYPE,
	PCT_SUBTYPE,
	PCT_SKIP_SPACES,
	PCT_SKIP_COMMENT,
	PCT_COMMENT_ESCAPED,
	PCT_PARAMETER_SEMICOLON,
	PCT_PARAMETER,
	PCT_PARAMETER_NAME,
	PCT_PARAMETER_QUANT_BEGIN,
	PCT_PARAMETER_QUANT_QUOTED,
	PCT_PARAMETER_QUANT,
	PCT_PARAMETER_QUANT_END,
	PCT_PARAMETER_QUANT_ESCAPED,
        PCT_PARAMETER_EQUAL_SIGN,
	PCT_ERROR
    } pct_state, backuped_state;

    int transfer_encoding;
    int old_transfer_encoding;
    

    size_t headers_count; 
    int bad_header;
    size_t parts_count;
    unsigned int cur_header;

    struct 
    {
	char*  type;
	char*  subtype;

	char*  boundary;
	char*  charset;

	size_t boundary_len;
    } content_type;

    unsigned int headers_seen;
    unsigned int nested_comments;

    char  *decode_buffer;
    size_t decode_buffer_size;

    char get_base64_digit(char ch);

    MIMEParser<ParserConfig>* partial_parser;

    size_t body_counter;
    bool last_atom_encoded;
    bool start_partial_parser;
};

/**
 * \brief Constructor.
 */
template<class ParserConfig>
inline MIMEParser<ParserConfig>::MIMEParser(ParserConfig* parser_config) : 
    level(0), 
    pc(parser_config),
    global_state(GS_HEADERS), 
    pct_state(PCT_START),
    transfer_encoding(MIMEParserGlobal::TE_7BIT),
    old_transfer_encoding(MIMEParserGlobal::TE_7BIT),
    headers_count(0),
    bad_header(0),
    parts_count(0), 
    cur_header(MIMEParserGlobal::HDR_UNKNOWN),
    headers_seen(0), 
    nested_comments(0), 
    decode_buffer(NULL),
    decode_buffer_size(0),
    partial_parser(NULL),
    body_counter(0),
    last_atom_encoded(false),
    start_partial_parser(false)
{
    memset(&content_type, 0, sizeof(content_type));
}

/**
 * \brief Destructor.
 */
template<class ParserConfig>
inline MIMEParser<ParserConfig>::~MIMEParser()
{
    if(content_type.type)     free(content_type.type);
    if(content_type.subtype)  free(content_type.subtype);
    if(content_type.boundary) free(content_type.boundary);
    if(content_type.charset)  free(content_type.charset);
    if(decode_buffer)  free(decode_buffer);

    if(partial_parser) delete partial_parser;
}

template<class ParserConfig>
inline void MIMEParser<ParserConfig>::recycle()
{
    global_state = GS_HEADERS;
    /* pc not needed to rotate */
    cur_header = MIMEParserGlobal::HDR_UNKNOWN;
    headers_seen = 0;
    headers_count = 0;
    bad_header = 0;
    pct_state = PCT_START;
    nested_comments = 0;
    parts_count = 0;
    transfer_encoding = TE_7BIT;
    /* level not needed to change */

    if(content_type.type)     free(content_type.type);
    if(content_type.subtype)  free(content_type.subtype);
    if(content_type.boundary) free(content_type.boundary);
    if(content_type.charset)  free(content_type.charset);

    if(partial_parser)
	partial_parser->recycle();

    memset(&content_type, 0, sizeof(content_type));

    body_counter = 0;
}

template<class ParserConfig>
inline void MIMEParser<ParserConfig>::set_level(size_t l)
{
    level = l;
}

/**
 * \brief Line parsing function.
 *
 * User must call this method for each line of the message.
 */
template<class ParserConfig>
inline int MIMEParser<ParserConfig>::parse(const char* line, size_t length)
{
    bool simple_body = false;
    
    if(!line) 
	{
	    global_state = GS_ERROR;
	    return PARSE_ERROR;
	}

    int ret_code = PARSE_OK;
    
    simple_body = !(content_type.boundary || start_partial_parser || partial_parser);

    if(global_state == GS_HEADERS) 
	{
	    if(parse_headers(line, length) == PARSE_ERROR)
		{
		    global_state = GS_BODY;
                    
                    if(simple_body)
                        parse_body_simple(line, length);
                    else
			parse_body(line, length);
		}
	}
    else if(global_state == GS_BODY && !simple_body) ret_code = parse_body(line, length);
    else if(global_state == GS_BODY_EPILOGUE || simple_body) ret_code = parse_body_simple(line, length);
    else
	ret_code = PARSE_ERROR;

    return ret_code;
}

/**
 * \brief Headers processing function.
 */
template<class ParserConfig>
inline int MIMEParser<ParserConfig>::parse_headers(const char* line, size_t l)
{
    int ret_code = PARSE_OK;
    const char* p = line, *p2;
    size_t count, l_newlines;
    int length = l, length2;

#define RETURN(x) 				\
    do {					\
	if(x == PARSE_ERROR)			\
	    global_state = GS_ERROR;		\
	ret_code = x;				\
	goto finish;				\
    } while(0)


    if(l == 0)
	{
	    global_state = GS_BODY;
	    RETURN(PARSE_OK);
	}

    for(l_newlines = 0; l_newlines < l && line[l_newlines] == '\r'; l_newlines++)
        ;

    if(l_newlines == l)
        {
            global_state = GS_BODY;
            RETURN(PARSE_OK);
        }

    if(isspace(*p))
	{
	    if(headers_count == 0)
		RETURN(PARSE_ERROR);

            if(!bad_header)
                pc->header_continue();

	}
    else
	{
            bad_header = 0;
            last_atom_encoded = false;

            /* 
               if(cur_header == HDR_CONTENT_TYPE && parse_content_type("", 0) != PARSE_OK)
               RETURN(PARSE_ERROR);
            */

            if(cur_header == HDR_CONTENT_TYPE)
                parse_content_type("", 0);

	    if(!is_printable(*line) || *line == ':')
                {
                    bad_header = 1;
                    RETURN(PARSE_OK);
                    /* RETURN(PARSE_ERROR); */
                }


	    p = line;
	    count = 0;

	    for( ; length >= 0 && is_printable(p[count]) && p[count] != ':' ; count++, length-- )
		;

	    if(count && length >= 0 && (isspace(p[count]) || p[count] == ':'))
		{
		    pc->header_name(p, count);
		    cur_header = check_header(p, count);

		    if(cur_header != HDR_UNKNOWN)
			headers_seen |= 1 >> cur_header;
		}
	    else
                {
                    /* RETURN(PARSE_ERROR); */
                    bad_header = 1;
                    RETURN(PARSE_OK);
                }

	    p += count;

	    for( ; length >= 0 && isspace(*p) ; p++, length--)
		;
	    
	    if(length < 0 || *p != ':')
                {
                    /* RETURN(PARSE_ERROR); */
                    bad_header = 1;
                    RETURN(PARSE_OK);
                }
	    
	    headers_count++;
	    p++;
	    length--;
	}

    if(bad_header)
        RETURN(PARSE_OK);

    /*
     * All spaces from continuation included in header_body
     */

    p2 = p;
    length2 = length;
    for( ; length2 > 0 && isspace(*p2) ; p2++, length2--)
	;

    if(length < 0)
	pc->header_body(NULL, 0, NULL, 0);
    else
	{
	    size_t last_non_space = 0;
	    bool was_non_space = false;
	    count = 0;

	    for( ; length2 > 0; count++, length2--)
		if(!isspace(p2[count]))
		    {
			last_non_space = count;
			was_non_space = true;
		    }

	    // Must be prooved.
	    process_header_body(p, length);

	    int pct_return = PARSE_OK;;

	    if(was_non_space)
		last_non_space++;

	    if(last_non_space > 0)
		{
		    switch(cur_header)
			{
			case HDR_CONTENT_TYPE:
			    pct_return = parse_content_type(p2, last_non_space);
			    break;
			    
			case HDR_CONTENT_TRANSFER_ENCODING:
			    pct_return = parse_content_transfer_encoding(p2, last_non_space);
			    break;
			}
		}

#if 0
	    if(pct_return == PARSE_ERROR)
		RETURN(PARSE_ERROR);
#endif

	}
    
#undef RETURN

  finish:
    return ret_code;
}

template<class ParserConfig>
inline int MIMEParser<ParserConfig>::parse_content_transfer_encoding(const char* tenc, size_t length)
{
    if(!tenc) return PARSE_ERROR;

    const char* ptr = tenc;

    if(*tenc == '\"')
	{
	    ptr++;
	    length--;

	    if(!length) 
		return PARSE_ERROR;

	    for( ; length > 0 && ptr[length-1] != '\"'; length--)
		;

	    if(length == 0) return PARSE_ERROR;
	}

    transfer_encoding = check_transfer_encoding(tenc, length);
    if(transfer_encoding == TE_UNKNOWN)
	return PARSE_ERROR;

    return PARSE_OK;
}

/**
 * \brief Content-type field body parsing.
 *
 * \note XXX. This function solves problem, which is subset of more common
 *       task: parsing of RFC822 (STD11) structured fields. But full parsiong such fields
 *       is more complex and not needed now.
 */
template<class ParserConfig>
inline int MIMEParser<ParserConfig>::parse_content_type(const char* ctype, size_t length)
{
    int ret_code = PARSE_OK;

    const char* cur = ctype;
    bool read_next = true;

    const char* full_ctype = NULL;
    size_t      full_ctype_size = 0;

    const char* p = NULL;
    size_t count = 0;

    unsigned int cur_parameter = CT_UNKNOWN;

#define RETURN(x)				\
    do {                                        \
	ret_code = x;				\
	goto finish;				\
    } while(0)

#define NOT_READ_NEXT					\
    do {						\
	if(cur && size_t(cur - ctype) < length-1)	\
	    read_next = false;				\
    } while(0)

    if(!length && *ctype == '\0')
	{
	    /* XXX */

	    pct_state = PCT_START;

	    if(content_type.type && strcmp(content_type.type, "multipart") == 0 && !content_type.boundary)
		RETURN(PARSE_ERROR);

	    RETURN(PARSE_OK);
	}

    for( ; pct_state != PCT_ERROR ; )
	{
	    switch(pct_state)
		{
		case PCT_ERROR:

		    /* nothing --- skip all */

		    break;

		case PCT_START:
		    pct_state = PCT_TYPE;
		    p = cur;
		    count = 0;

		    full_ctype = cur;
		    full_ctype_size = 0;

		case PCT_TYPE:
		    if(!cur)
			RETURN(PARSE_ERROR);

		    if(*cur == '/')
			{
			    if(count == 0)
				RETURN(PARSE_ERROR);

			    if(p)
				{
				    content_type.type = (char*)realloc(content_type.type, count+1);
				    memcpy(content_type.type, p, count);
				    content_type.type[count] = 0;

				    pc->content_type(p, count);
				}

			    pct_state = PCT_SUBTYPE;
			    p = cur+1;
			    count = 0;
			    
			}
		    else if(isspace(*cur))
			RETURN(PARSE_ERROR);
		    else
			count++;

		    full_ctype_size++;

		    break;

		case PCT_SUBTYPE:

		    if(!cur || isspace(*cur) || *cur == ';')
			{
			    if(count == 0)
				RETURN(PARSE_ERROR);
			    
                            if(p)
				{
				    content_type.subtype = (char*)realloc(content_type.subtype, count+1);
				    memcpy(content_type.subtype, p, count);
				    content_type.subtype[count] = 0;

				    pc->content_subtype(p, count);
				}

			    if(full_ctype)
				pc->content_full_type(full_ctype, full_ctype_size);

			    read_next = false;
			    pct_state = PCT_PARAMETER_SEMICOLON;
			}
		    else
			{
			    count++;
			    full_ctype_size++;
			}
		    
		    break;

		case PCT_PARAMETER_SEMICOLON:

		    if(!cur)
			RETURN(PARSE_OK);

		    if(isspace(*cur))
			{
			    backuped_state = PCT_PARAMETER_SEMICOLON;
			    pct_state = PCT_SKIP_SPACES;
			}
		    else if(*cur == ';')
			{
			    pct_state = PCT_PARAMETER;
			}
		    else if(*cur == '(')
			{
			    backuped_state = PCT_PARAMETER_SEMICOLON;
			    pct_state = PCT_SKIP_COMMENT;
			}
		    else
			RETURN(PARSE_ERROR);

		    
		    break;
		    
		case PCT_PARAMETER:

		    if(!cur)
			RETURN(PARSE_OK);

		    if(isspace(*cur))
			{
			    backuped_state = PCT_PARAMETER;
			    pct_state = PCT_SKIP_SPACES;
			}
		    else if(*cur == '(')
			{
			    backuped_state = PCT_PARAMETER;
			    pct_state = PCT_SKIP_COMMENT;
			}
		    else
			{
			    pct_state = PCT_PARAMETER_NAME;
			    p = cur;
			    count = 0;
			    read_next = false;
			}

		    break;

		case PCT_PARAMETER_NAME:

		    if(!cur)
			RETURN(PARSE_ERROR);
		    
		    if(isspace(*cur) || *cur == '=')
			{
			    if(count == 0)
				RETURN(PARSE_ERROR);
			    
			    if(p)
				cur_parameter = check_ct_parameter(p, count);

			    if(*cur == '=')
				pct_state = PCT_PARAMETER_QUANT_BEGIN;
			    else
				{
				    backuped_state = PCT_PARAMETER_EQUAL_SIGN;
				    pct_state = PCT_SKIP_SPACES;
				}
				
			}
		    else
			count++;
		    
		    break;

		case PCT_PARAMETER_EQUAL_SIGN:

			if(!cur || *cur != '=')
				RETURN(PARSE_ERROR);

			pct_state = PCT_PARAMETER_QUANT_BEGIN;

			break;

		case PCT_PARAMETER_QUANT_BEGIN:

		    count = 0;

		    if(!cur) RETURN(PARSE_ERROR);

		    if(*cur == '\"') 
			{
			    pct_state = PCT_PARAMETER_QUANT_QUOTED;
			    p = cur+1;
			    break;
			}
		    else if(*cur == ' ')
			{
			    /*
			     * Just skip...
			     */
			}
		    else
			{
			    read_next = false;
			    pct_state = PCT_PARAMETER_QUANT;
			    p = cur;
			}
			
		    break;
		    
		case PCT_PARAMETER_QUANT:


		    if(!cur || isspace(*cur) || *cur == ';')
			{
			    read_next = false;
			    pct_state = PCT_PARAMETER_QUANT_END;
			}
		    else
			count++;

		    break;

		case PCT_PARAMETER_QUANT_QUOTED:

		    /* XXX -- здесь потенциальная дыра: понимаются escape-символы,
		     * но вот разэскейпливания не делается, то есть они так и 
		     * отдаются парными символами: \\ 
		     */

		    if(!cur)
                        RETURN(PARSE_ERROR);

		    if(*cur == '\"')
			{
			    pct_state = PCT_PARAMETER_QUANT_END;
                            read_next = false;
			}
		    else
			{
			    if(*cur == '\\')
				pct_state = PCT_PARAMETER_QUANT_ESCAPED;
			    
			    count++;
			}

		    break;

		case PCT_PARAMETER_QUANT_ESCAPED:

		    count++;
		    pct_state = PCT_PARAMETER_QUANT_QUOTED;

		    break;

		case PCT_PARAMETER_QUANT_END:

                    if(cur && *cur != '\"')
                        read_next = false;
                    
		    switch(cur_parameter)
			{
			case CT_BOUNDARY:

			    if(p)
				{
				    content_type.boundary = (char*)realloc(content_type.boundary, count+1);
				    memcpy(content_type.boundary, p, count);
				    content_type.boundary[count] = 0;
				    content_type.boundary_len = count;
				}

			    break;

			case CT_CHARSET:

			    if(p)
				{
				    content_type.charset = (char*)realloc(content_type.charset, count+1);
				    memcpy(content_type.charset, p, count);
				    content_type.charset[count] = 0;
				    
				    pc->content_charset(p, count);
				}

			    break;

                        case CT_PROTOCOL:

                            if(p)
                                {
                                    pc->content_protocol(p, count);
                                }

                            break;

                        case CT_NAME:

                            if(p)
                                {
                                    pc->content_name(p, count);
                                }

                            break;


			case CT_UNKNOWN:
			    break;
			}
		    

		    cur_parameter = CT_UNKNOWN;

		    pct_state = PCT_PARAMETER_SEMICOLON;

		    break;
		    
		case PCT_SKIP_SPACES:

		    if(!cur || !isspace(*cur))
			{
			    NOT_READ_NEXT;
			    pct_state = backuped_state;
			}

		    break;

		case PCT_SKIP_COMMENT:

		    if(!cur)
			RETURN(PARSE_CONT);

		    switch(*cur)
			{
			case '(':
			    nested_comments++;
			    break;

			case ')':
			    if(nested_comments == 0)
				pct_state = backuped_state;
			    else
				nested_comments--;
			    break;

			case '\\':
			    pct_state = PCT_COMMENT_ESCAPED;
			    break;
			}
		    
		    break;

		case PCT_COMMENT_ESCAPED:
		    if(!cur)
			RETURN(PARSE_ERROR);

		    pct_state = PCT_SKIP_COMMENT;
		    
		    break;
		}

	    if(read_next && !cur)
                break;

	    if(read_next)
		{
		    if(size_t(cur - ctype) < length-1)
			cur++;
		    else
			cur = NULL;
		}
	    else
		read_next = true;

	}

#undef RETURN
#undef NOT_READ_NEXT

  finish:

    if(ret_code != PARSE_OK)
	pct_state = PCT_ERROR;

    return ret_code;
}


template<class ParserConfig>
inline int MIMEParser<ParserConfig>::parse_body(const char* line, size_t l)
{
    int ret_code = PARSE_OK;

#define RETURN(x) 				\
    do {					\
	if(x == PARSE_ERROR)			\
	    global_state = GS_ERROR;		\
	ret_code = x;				\
	goto finish;				\
    } while(0)

#if 0
    if(!body_counter)
	{
	    printf(" --- > content_type: \"%s\" / \"%s\" charset=\"%s\" boundary=\"%s\"\n", 
		   content_type.type, content_type.subtype,
		   content_type.charset, content_type.boundary
		);
	}

    body_counter++;
#endif

    bool was_boundary = false;
    bool epilogue = false;
    
    if(line[0] == '-' && line[1] == '-')
	{
	    size_t last_non_space = l-1;
	    for( ; isspace(line[last_non_space]); last_non_space--)
		;
	    
	    if(last_non_space > 1)
		{
		    if(last_non_space == content_type.boundary_len + 3 && 
                       memcmp(line+2, content_type.boundary, content_type.boundary_len) == 0 &&
                       line[last_non_space] == '-' && line[last_non_space-1] == '-')
			{
			    epilogue = true;
			    last_non_space -= 2;
			}
		    
		    if(content_type.boundary_len == last_non_space - 1 &&
		       memcmp(line+2, content_type.boundary, content_type.boundary_len) == 0)
			was_boundary = true;
		}
	}
    
    if(start_partial_parser)
        {
            if(!partial_parser) 
                {
                    pc->body(NULL, 0, true);

                    partial_parser = new MIMEParser<ParserConfig>(pc);
                    partial_parser->set_level(level+1);
                }
            else
                partial_parser->recycle();

            pc->part_begin(level);

            start_partial_parser = false;
        }

    if(was_boundary)
	if(epilogue)
	    {
		pc->epilogue(level);
		global_state = GS_BODY_EPILOGUE;
	    }
	else
	    {
		if(!partial_parser) 
		    {
			partial_parser = new MIMEParser<ParserConfig>(pc);
			partial_parser->set_level(level+1);
		    }
		else
		    partial_parser->recycle();
		
		pc->part_begin(level);
		
		parts_count++;
	    }
    else
	{
	    if(partial_parser)
		{
		    if(partial_parser->parse(line, l) == PARSE_ERROR)
			RETURN(PARSE_ERROR);
		}
	    else
		parse_body_simple(line, l);
	}

#undef RETURN

  finish:
    return ret_code;
}

template<class ParserConfig>
inline char MIMEParser<ParserConfig>::get_base64_digit(char ch)
{
    if(ch >= 'A' && ch <= 'Z') return ch - 'A';
    else if(ch >= 'a' && ch <= 'z') return ch - 'a' + 26;
    else if(ch >= '0' && ch <= '9') return ch - '0' + 52;
    else if(ch == '+') return 62;
    else if(ch == '/') return 63;
    else return 0;
}

template<class ParserConfig>
inline int MIMEParser<ParserConfig>::parse_body_simple(const char* line, size_t l)
{
    if(transfer_encoding == TE_QP || 
       transfer_encoding == TE_B64 || 
       transfer_encoding == TE_UUE)
	{
	    int out_length;
	    bool eol = false;

	    alloc_decode_buffer(l);
	    
	    switch(transfer_encoding)
		{
		case TE_QP:
		{
		    size_t last_non_space = l ? l-1 : 0;
		    for( ; last_non_space > 0 && isspace(line[last_non_space]); last_non_space--)
			;

		    out_length = decode_qp(line, l, decode_buffer);
                    if(out_length < 0)
                        out_length = 0;

		    if(l && line[last_non_space] != '=')
			eol = true;

                    pc->body(decode_buffer, out_length, eol);

		    break;
		}

		case TE_B64:

		    out_length = decode_b64(line, l, decode_buffer);
		    if(out_length < 0) out_length = 0;
                    pc->body(decode_buffer, out_length, eol);

		    break;

		case TE_UUE:

                {
                    int uue_end = 0;

                    if(l >= 3 && line[0] == 'e' && line[1] == 'n' && line[2] == 'd')
                        uue_end = 1;

                    if(uue_end == 0)
                        {
                            out_length = decode_uue(line, l, decode_buffer);

                            if(out_length != 0)
                                {
                                    if(out_length < 0)
                                        {
                                            uue_end = 2;
                                            out_length = -1*out_length;
                                        }

                                    pc->body(decode_buffer, out_length, eol);
                                }
                            else
                                {
                                    if(l <= 1)
                                        {
                                            /* nothing to do, just waiting for 'end' directive */
                                        }
                                    else
                                        {
                                            uue_end = 2;
                                        }
                                }
                        }

                    if(uue_end > 0)
                        {
                            pc->part_begin(level);
                            transfer_encoding = old_transfer_encoding;;
                        }

                    if(uue_end > 1)
                        {
                            /* line with error */
                            pc->body(line, l, true);
                        }

                    break;
                }

		default:
		    break;
		}
	}
    else
	{
	    /* TE_7BIT, TE_8BIT, TE_BINARY, TE_UNKNOWN */

            if(l > sizeof("begin"))
                {
                    if(memcmp(line, "begin", 5) == 0 && line[5] == ' ')
                        {
                            size_t pos = 6;
                            int mode = 0;
                            int i;
                            const char *filename;
                            size_t filename_length = 0;

#define SKIP do { goto skip; } while(0)

                            for( ; pos < l && line[pos] == ' '; pos++)
                                ;
                            if(pos >= l)
                                SKIP;
                            
                            /* access mode */

                            for(i = 0; i < 3; i++)
                                {
                                    if(line[pos] >= '0' && line[pos] <= '7')
                                        mode = mode*8+line[pos]-'0';
                                    else
                                        SKIP;

                                    pos++;
                                    if(pos >= l)
                                        SKIP;
                                }

                            if(line[pos] != ' ')
                                SKIP;

                            for( ; pos < l && line[pos] == ' '; pos++)
                                ;
                            if(pos >= l)
                                SKIP;

                            /* filename */

                            filename = line+pos;
                            filename_length = l - (pos);

                            pc->uue_part_begin(level, mode, filename, filename_length);
                            old_transfer_encoding = transfer_encoding;
                            transfer_encoding = TE_UUE;

#undef SKIP
                            
                          skip:
                            ;
                        }
                }

            if(transfer_encoding != TE_UUE)
                pc->body(line, l, true);
	}

    return PARSE_OK;
}


template<class ParserConfig>
inline int MIMEParser<ParserConfig>::decode_b64(const char* in, size_t length, char* out)
#if 1  /* LEXA - inserted B64 decoder from Apache 1.3.29 */
{
    return mime_parser_decode_b64(in,length,out);

}

#else
{
    size_t filled = 0;
    char symbols[4];
#if 0
    if(length % 4)
	return 0;
#endif
    while(length && isspace(in[length-1])) length--; // LEXA
    for(size_t i = 0; i < length; i += 4)
	{
	    symbols[0] = get_base64_digit(in[i]);
	    symbols[1] = i+1 < length ? get_base64_digit(in[i+1]) : 0;
	    symbols[2] = i+2 < length ? get_base64_digit(in[i+2]) : 0;
	    symbols[3] = i+3 < length ? get_base64_digit(in[i+3]) : 0;

	    out[filled++] = symbols[0] << 2 | symbols[1] >> 4;

            if(i+2 < length)
                {
                    if(in[i+2] == '=') continue;
                }
	    out[filled++] = symbols[1] << 4 | symbols[2] >> 2;

            if(i+3 < length)
                {
                    if(in[i+3] == '=') continue;
                }
	    out[filled++] = symbols[2] << 6 | symbols[3];
	}

    return filled;
}
#endif
template<class ParserConfig>
inline int MIMEParser<ParserConfig>::decode_qp(const char* in, size_t length, char* out)
{
    unsigned int num = 0;
    int code;
    size_t filled = 0;
    size_t last_equal_sign;

    if(length == 0)
        return 0;

    for(last_equal_sign = length-1; last_equal_sign > 0; last_equal_sign--)
        {
            if(in[last_equal_sign] == '=')
                break;

            if(in[last_equal_sign] == ' ' || 
               in[last_equal_sign] == '\t' ||
               in[last_equal_sign] == '\r' ||
               in[last_equal_sign] == '\n')
                continue;

            break;
        }

    if(in[last_equal_sign] == '=')
        length = last_equal_sign;

    for(size_t i = 0; i < length; i++)
	{
	    if(num > 0)
		{
		    if(in[i] >= '0' && in[i] <= '9')
			code = (code << 4) + in[i] - '0';
		    else if(in[i] >= 'A' && in[i] <= 'F')
			code = (code << 4) + in[i] - 'A' + 10;
		    else if(in[i] >= 'a' && in[i] <= 'f')
			code = (code << 4) + in[i] - 'a' + 10;
		    else
			{
			    if(num == 2) out[filled++] = in[i-1];
			    out[filled++] = in[i];
			    num = 0;
			}

		    if(num)
			{
			    num++;
			    if(num == 3)
				{
				    out[filled++] = code;
				    num = 0;
				}
			}
		}
	    else
		{
		    if(in[i] == '=')
			{
			    num = 1;
			    code = 0;
			}
		    else if(in[i] == '_')
			out[filled++] = ' ';
		    else
			out[filled++] = in[i];
		}
	}

    return filled;
}

template<class ParserConfig>
inline int MIMEParser<ParserConfig>::decode_uue(const char* in, size_t length, char* out)
{
    int i, ch;
    int filled = 0; 

#define	DECODE(c) (((c) - ' ') & 077)
#define IS_DECODABLE(c) ( (((c) - ' ') >= 0) && (((c) - ' ') <= 077 + 1) )
#define OUT_OF_RANGE do { return -1*filled; } while(0)
#define ADD(CH) do { out[filled++] = (CH); } while(0)
    
    if(*in == '\0')
        return 0;
    
    if((i = DECODE(*in)) <= 0)
        return 0;
    
    in++;

    for ( ; i > 0; in += 4, i -= 3)
        {
            if(i >= 3) 
                {
                    if(!(IS_DECODABLE(in[0]) && IS_DECODABLE(in[1]) &&
                         IS_DECODABLE(in[2]) && IS_DECODABLE(in[3])))
                        OUT_OF_RANGE;
                    
                    ch = DECODE(in[0]) << 2 | DECODE(in[1]) >> 4;
                    ADD(ch);

                    ch = DECODE(in[1]) << 4 | DECODE(in[2]) >> 2;
                    ADD(ch);

                    ch = DECODE(in[2]) << 6 | DECODE(in[3]);
                    ADD(ch);
                }
            else 
                {
                    if(i >= 1) 
                        {
                            if(!(IS_DECODABLE(in[0]) && IS_DECODABLE(in[1])))
                                OUT_OF_RANGE;

                            ch = DECODE(in[0]) << 2 | DECODE(in[1]) >> 4;
                            ADD(ch);
                        }

                    if(i >= 2) 
                        {
                            if(!(IS_DECODABLE(in[1]) && IS_DECODABLE(in[2])))
                                OUT_OF_RANGE;
                    
                            ch = DECODE(in[1]) << 4 | DECODE(in[2]) >> 2;
                            ADD(ch);
                        }

                    
                    if(i >= 3) 
                        {
                            if(!(IS_DECODABLE(in[2]) && IS_DECODABLE(in[3])))
                                OUT_OF_RANGE;

                            ch = DECODE(in[2]) << 6 | DECODE(in[3]);
                            ADD(ch);
                        }
                }
        }

#undef DECODE
#undef IS_DECODABLE
#undef OUT_OF_RANGE
#undef ADD
    
    return filled;
}


template<class ParserConfig>
inline int MIMEParser<ParserConfig>::process_header_body(const char* p, size_t count)
{
    enum {
	LS_NORMAL,
	LS_ENCODED,
	LS_ENCODED_CODEPAGE_BEGIN,
	LS_ENCODED_CODEPAGE,
	LS_ENCODED_METHOD,
	LS_ENCODED_PRE_DATA,
	LS_ENCODED_DATA,
	LS_ENCODED_DATA_END
    } local_state = LS_NORMAL;

    const char* last_p = p;
    size_t distance = 0, skipped = 0;

    const char* codepage;
    size_t cp_size;

    const char* data;
    size_t data_size, decoded_length;

    int encoding = TE_UNKNOWN;
    
    for(size_t i = 0; i < count; i++)
	{
	    switch(local_state)
		{
		case LS_NORMAL:

		    if(p[i] == '=')
			{
			    skipped = 1;
			    local_state = LS_ENCODED;
			}
		    else
			distance++;

		    break;

		case LS_ENCODED:

		    if(p[i] == '?')
			{
			    skipped++;
			    local_state = LS_ENCODED_CODEPAGE_BEGIN;
			}
		    else if(p[i] == '=')
			{
			    distance += skipped;
			    local_state = LS_ENCODED;
			    skipped = 1;
			}
		    else
			{
			    distance += skipped+1;
			    local_state = LS_NORMAL;
			    skipped = 0;
			}

		    break;

		case LS_ENCODED_CODEPAGE_BEGIN:

		    if(is_printable(p[i]) && p[i] != '?')
			{
			    skipped++;
			    local_state = LS_ENCODED_CODEPAGE;

			    codepage = p+i;
			    cp_size = 0;
			}
		    else
			{
			    distance += skipped+1;
			    local_state = LS_NORMAL;
			    skipped = 0;
			}

		    break;

		case LS_ENCODED_CODEPAGE:

		    if(p[i] == '?')
			{
			    skipped++;
			    local_state = LS_ENCODED_METHOD;
			}
		    if(is_printable(p[i]))
			{
			    cp_size++;
			    skipped++;
			}
		    else
			{
			    distance += skipped+1;
			    local_state = LS_NORMAL;
			    skipped = 0;
			}

		    break;

		case LS_ENCODED_METHOD:

		    switch(p[i])
			{
			case 'q':
			case 'Q':

			    encoding = TE_QP;
			    skipped++;
			    local_state = LS_ENCODED_PRE_DATA;

			    break;

			case 'b':
			case 'B':

			    encoding = TE_B64;
			    skipped++;
			    local_state = LS_ENCODED_PRE_DATA;

			    break;

			default:

			    distance += skipped+1;
			    skipped = 0;
			    local_state = LS_NORMAL;
			}

		    break;

		case LS_ENCODED_PRE_DATA:

		    if(p[i] == '?')
			{
			    local_state = LS_ENCODED_DATA;
			    skipped++;

			    data = p+i+1;
			    data_size = 0;
			}
		    else
			{
			    distance += skipped+1;
			    skipped = 0;
			    local_state = LS_NORMAL;
			}

		    break;

		case LS_ENCODED_DATA:

		    skipped++;

		    if(p[i] == '?')
			local_state = LS_ENCODED_DATA_END;
		    else
			data_size++;
		    
		    break;

		case LS_ENCODED_DATA_END:

		    skipped++;

		    if(p[i] == '=')
			{
			    alloc_decode_buffer(data_size);

			    if(encoding == TE_QP)
				decoded_length = decode_qp(data, data_size, decode_buffer);
			    else
				decoded_length = decode_b64(data, data_size, decode_buffer);

			    if(decoded_length)
				{
				    if(distance)
					{
                                            size_t k;
                                            bool all_spaces = true;

                                            if(last_atom_encoded)
                                                {
                                                    for(k = 0; k < distance; k++)
                                                        {
                                                            if(!isspace(last_p[k]))
                                                                {
                                                                    all_spaces = false;
                                                                    break;
                                                                }
                                                        }
                                                }

                                            if(last_atom_encoded && all_spaces)
                                                {
                                                    /* nothing: ignore spaces beetween two adjacent encoded atoms */
                                                }
                                            else
                                                pc->header_body(last_p, distance, NULL, 0);

					    distance = 0;
					}

				    last_p = p+i+1;

				    pc->header_body(decode_buffer, decoded_length, codepage, cp_size);

                                    last_atom_encoded = true;
				}
			    else
				distance += skipped;

			    skipped = 0;
			    local_state = LS_NORMAL;
			}
		    else
			data_size += 2;

		    break;
		}
	}

    if(distance || skipped)
	{
	    distance += skipped;

            if(last_atom_encoded)
                {
                    size_t k;
                    bool all_spaces = true;
                    
                    for(k = 0; k < distance; k++)
                        {
                            if(!isspace(last_p[k]))
                                {
                                    all_spaces = false;
                                    break;
                                }
                        }

                    if(all_spaces)
                        {
                            /* nothing */
                        }
                    else
                        {
                            pc->header_body(last_p, distance, NULL, 0);
                            last_atom_encoded = false;
                        }
                }
            else
                pc->header_body(last_p, distance, NULL, 0);
	}

    return PARSE_OK;
}


template<class ParserConfig>
inline void MIMEParser<ParserConfig>::alloc_decode_buffer(size_t s)
{
    if(s > decode_buffer_size)
	{
	    decode_buffer_size = (s/1024 + 1)*1024;
	    if(decode_buffer) free(decode_buffer);
	    decode_buffer = (char*)malloc(decode_buffer_size);
	}
}

#endif // __MIMEParser_hpp__

/*
 * <eof MIMEParser.hpp>
 */
