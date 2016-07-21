/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: envelope.cpp                                                          *
 * Created: Mon May  6 21:48:22 2002                                           *
 * Desc: Some helpers for work with mail envelope.                             *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/
/**
 * \file  envelope.cpp
 * \brief Some helpers for work with mail envelope.
 *
 * Revision 1.7  2005/09/16 15:28:47  esmirnov
 * KIS_USAGE
 *
 * Revision 1.6  2005/01/19 11:39:49  alexiv
 * *** empty log message ***
 *
 * Revision 1.5  2005/01/16 23:00:27  vtroitsky
 * The results of warnings hunting
 *
 * Revision 1.4  2005/01/06 14:17:49  vtroitsky
 * HAVE_GSG2 changed to WITH_GSG
 *
 * Revision 1.3  2005/01/06 11:22:07  alexiv
 * *** empty log message ***
 *
 * Revision 1.2  2005/01/04 22:51:22  vtroitsky
 * Some mistakes are fixed in projects
 *
 * Revision 1.1  2004/12/29 05:08:40  vtroitsky
 * *** empty log message ***
 *
 */

#include "commhdr.h"
#pragma hdrstop

#include "LogFile.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif


#ifndef _WIN32
#include <syslog.h>
#else
#pragma warning (disable : 4018)
#endif //_WIN32

#include "Cp2Uni.h"
#include "CodePages.h"

#include "MIMEParser/MIMEParser.hpp"

#include "envelope.h"

#ifndef _WIN32
#include "smtp-filter/common/mergesort.h"
#else
#include "smtp-filter/common/merge.h"
#endif // _WIN32

#ifdef HAVE_MD5CHECK_OLD
extern "C" {

#ifdef OS_LINUX
#define PROTOTYPES 1
#include <md5global.h>
#endif

#include <md5.h>
}


#include "smtp-filter/libsigs/libsigs.h"
#include "smtp-filter/common/logger.h"
int check_md5(md5sum sum);
#endif

#ifdef HAVE_KAVCLIENT
#include <syslog.h>
#include <time.h>
#endif


#ifdef WITH_GSG
extern "C" {
#include <smtp-filter/libgsg2/gsg.h>
}
#include "smtp-filter/common/logger.h"
int check_md5(md5sum sum);
#endif


/*******************************************************************************
 * XXX All this code is cut&pasted in many places. I hope, that alexiv 
 * move this code to pcommon without changes in interface. 
 */

struct encoding_entry
{
    const char *name;
    size_t      length;
    int         ID;
};

#define ENCODING_ENTRY(enc, id) { enc, sizeof(enc)-1, id }

encoding_entry encodings[] = {

    ENCODING_ENTRY("windows-1251",   CP_CYRILLIC),

    ENCODING_ENTRY("iso-8859-5",     CP_ISO8859_5),
    ENCODING_ENTRY("iso-8859_5",     CP_ISO8859_5),
    ENCODING_ENTRY("cyrillic",       CP_ISO8859_5),

    ENCODING_ENTRY("koi8-r",         CP_KOI8R),
    ENCODING_ENTRY("koi8",           CP_KOI8R),
    ENCODING_ENTRY("csKOI8R",        CP_KOI8R),

    ENCODING_ENTRY("x-mac-cyrillic", CP_CYRILLIC_MAC),

    ENCODING_ENTRY("ibm866",         CP_OEM_RUSSIAN),
    ENCODING_ENTRY("cp866",          CP_OEM_RUSSIAN),

    ENCODING_ENTRY("cp819",          CP_LATIN1),
    ENCODING_ENTRY("windows-1252",   CP_LATIN1),
    ENCODING_ENTRY("iso_8859-1",     CP_LATIN1),
    ENCODING_ENTRY("us-ascii",       CP_LATIN1),
    ENCODING_ENTRY("ascii",          CP_LATIN1),
    ENCODING_ENTRY("us",             CP_LATIN1),
    ENCODING_ENTRY("cp367",          CP_LATIN1),
    ENCODING_ENTRY("ibm367",         CP_LATIN1),

    ENCODING_ENTRY("utf-8",          CP_UTF8),

    { NULL, 0, -1 }
};

bool cmp_nocase(const char* s1, const char* s2, size_t len)
{
    for(size_t i = 0; i < len; i++)
	if(tolower(s1[i]) != tolower(s2[i])) return false;

    return true;
}

int check_encoding(const char* name, size_t name_len = 0)
{
    if(name_len == 0)
	{
	    if(name) name_len = strlen(name);
	    else return -1;
	}

    for(int i = 0; encodings[i].name; i++)
	if(encodings[i].length == name_len && cmp_nocase(encodings[i].name, name, name_len))
	    return encodings[i].ID;
    
    return -1;
}

/*
 * XXX (end of this code)
 *******************************************************************************/

envelope *envelope_create(envelope *e)
{
    envelope *res = e; /* (envelope *)malloc(sizeof(envelope)); */
    memset(res, 0, sizeof(envelope));

    e->heap.array = (char *)malloc(e->heap.alloc = 20480);
    memset(e->heap.array, 0, e->heap.alloc);

#ifndef KIS_USAGE
    e->ipe = ip_extractor_create(IP_EXTRACTOR_DEFAULTS);
#else
    e->ipe = NULL;
#endif//KIS_USAGE
    e->max_received_headers = 0;

    return res;
}

int envelope_recycle(envelope *e)
{
    memset(&(e->from), 0, sizeof(e->from));
    memset(&(e->data), 0, sizeof(e->data));
    memset(&(e->ip_addr), 0, sizeof(e->ip_addr));

    if(e->rcpts.array)
	memset(e->rcpts.array, 0, sizeof(envelope_rcpt)*e->rcpts.alloc);

    if(e->attrs.array)
        memset(e->attrs.array, 0, sizeof(envelope_attr)*e->attrs.alloc);

    if(e->heap.array)
	memset(e->heap.array, 0, e->heap.alloc);

    if(e->headers.array)
	memset(e->headers.array, 0, e->headers.alloc*sizeof(msg_header));

#ifdef WITH_GSG
    e->binary_processor = 0;
#endif

#ifdef HAVE_MD5CHECK_OLD
    if(e->md5_sums.array)
	memset(e->md5_sums.array, 0, e->md5_sums.alloc*sizeof(md5sum));
    e->md5_sums.used = 0;
#endif

#ifdef HAVE_KAVCLIENT
    if(e->ctypes.array)
	{
	    for(size_t i=0;i<e->ctypes.used && i<e->ctypes.alloc; i++)
		{
		    if(e->ctypes.array[i].type)
			free(e->ctypes.array[i].type);
		    if(e->ctypes.array[i].subtype)
			free(e->ctypes.array[i].subtype);
		}
	    memset(e->ctypes.array, 0, e->ctypes.alloc*sizeof(ctype_data));
	}
    e->ctypes.used = 0;
#endif
    e->rcpts.used   = 0;
    e->heap.used    = 0;
    e->headers.used = 0;
    e->attrs.used = 0;

    memset(&(e->dns), 0, sizeof(e->dns));
    e->queue_id = NULL;

#ifndef KIS_USAGE
    if(e->ipe)
        ip_extractor_recycle(e->ipe);
#endif//KIS_USAGE

	e->msg_size = 0;

    return 0;
}

void envelope_destroy(envelope *e)
{
    if(e->rcpts.array)
	free(e->rcpts.array);

    if(e->heap.array)
	free(e->heap.array);

    if(e->headers.array)
	free(e->headers.array);

    if(e->attrs.array)
        free(e->attrs.array);

#ifdef HAVE_MD5CHECK_OLD
    if(e->md5_sums.array)
	free(e->md5_sums.array);
#endif
#ifndef KIS_USAGE
    if(e->ipe)
        ip_extractor_destroy(e->ipe);
#endif//KIS_USAGE

#ifdef HAVE_KAVCLIENT
    if(e->ctypes.array)
	{
	    for(size_t  i=0;i<e->ctypes.used && i<e->ctypes.alloc; i++)
		{
		    if(e->ctypes.array[i].type)
			free(e->ctypes.array[i].type);
		    if(e->ctypes.array[i].subtype)
			free(e->ctypes.array[i].subtype);
		}
	    free(e->ctypes.array);
	}
#endif

    /* free(e); */
}

static int set_envelope_text(envelope_text *et, const char *text, int length)
{
    et->text = text;

    if(!text)
	et->length = 0;
    else
	{
	    if(length == ETEXT_ASCIIZ)
		et->length = strlen(text);
	    else
		et->length = length;
	}

    return 0;
}

int envelope_set_from(envelope *e, const char *email, int length)
{
    return set_envelope_text(&(e->from), email, length);
}

int envelope_set_data(envelope *e, const char *data, int length)
{
    return set_envelope_text(&(e->data), data, length);
}

int envelope_set_ip_addr(envelope *e, const char *ip_addr, int length)
{
    return set_envelope_text(&(e->ip_addr), ip_addr, length);
}

int envelope_add_rcpt(envelope *e, const char *email, int length)
{
    if(e->rcpts.used == e->rcpts.alloc)
	{
	    if(e->rcpts.array)
		e->rcpts.array = (envelope_rcpt *)realloc(e->rcpts.array, sizeof(envelope_rcpt)*(e->rcpts.alloc += 16));
	    else
		e->rcpts.array = (envelope_rcpt *)malloc(sizeof(envelope_rcpt)*(e->rcpts.alloc = 16));

	    memset(e->rcpts.array + e->rcpts.used, 0, (e->rcpts.alloc - e->rcpts.used)*sizeof(envelope_rcpt));
	}

    return set_envelope_text(&((e->rcpts.array + e->rcpts.used++)->email), email, length);
}

#ifdef HAVE_MD5CHECK_OLD

static md5data *envelope_new_md5(envelope *e)
{
    if(e->md5_sums.used == e->md5_sums.alloc)
	{
	    if(e->md5_sums.array)
		e->md5_sums.array = (md5data *)realloc(e->md5_sums.array, 
						       sizeof(md5data)*(e->md5_sums.alloc += 16));
	    else
		e->md5_sums.array = (md5data *)malloc(sizeof(md5data)*(e->md5_sums.alloc = 16));

	    memset(e->md5_sums.array + e->md5_sums.used, 0, 
		   (e->md5_sums.alloc - e->md5_sums.used)*sizeof(md5data));
	}
    return &(e->md5_sums.array[e->md5_sums.used++]);
    
}
#endif 
#ifdef HAVE_KAVCLIENT

static ctype_data *envelope_new_ctype(envelope *e)
{
    if(e->ctypes.used == e->ctypes.alloc)
	{
	    if(e->ctypes.array)
		e->ctypes.array = (ctype_data *)realloc(e->ctypes.array, 
						       sizeof(ctype_data)*(e->ctypes.alloc += 16));
	    else
		e->ctypes.array = (ctype_data *)malloc(sizeof(ctype_data)*(e->ctypes.alloc = 16));

	    memset(e->ctypes.array + e->ctypes.used, 0, 
		   (e->ctypes.alloc - e->ctypes.used)*sizeof(ctype_data));
	}
    return &(e->ctypes.array[e->ctypes.used++]);
    
}
static ctype_data *envelope_first_ctype(envelope *e)
{
    if(e->ctypes.used)
	return &e->ctypes.array[0];
    else
	return NULL;
}
static ctype_data *envelope_current_ctype(envelope *e)
{
    if(e->ctypes.used)
	return &e->ctypes.array[e->ctypes.used-1];
    else
	return NULL;
}
#endif 

int envelope_new_header(envelope *e)
{
    if(e->headers.used == e->headers.alloc)
	{
	    if(e->headers.array)
		e->headers.array = (msg_header *)realloc(e->headers.array, (e->headers.alloc += 10)*sizeof(msg_header));
	    else 
		e->headers.array = (msg_header *)malloc((e->headers.alloc = 10)*sizeof(msg_header));

	    memset(e->headers.array + e->headers.used, 0, (e->headers.alloc - e->headers.used)*sizeof(msg_header));
	}

    return e->headers.used++;
}



/*******************************************************************************
 *
 * Parse headers of letter.
 *
 *******************************************************************************/

struct EnvelopeParserConfig
{
    void header_continue();
    void header_name(const char* p, size_t count);
    void header_body(const char* p, size_t count, const char* codepage, size_t cp_count);
    void part_begin(unsigned int level);
    void uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length);
    void epilogue(unsigned int level);
    void body(const char* p, size_t count, bool eol);

    void content_type(const char* p, size_t count);
    void content_subtype(const char* p, size_t count);
    void content_charset(const char* p, size_t count);
    void content_full_type(const char* p, size_t count);
    void content_name(const char* p, size_t count);
    void content_protocol(const char* p, size_t count);

    envelope *e;
    int       cur_header;
    bool      was_header_body;

    bool received_header;
    unsigned int received_counter;
    bool in_body;

#ifdef HAVE_MD5CHECK_OLD
    MD5_CTX  *md5_ctx;
    size_t   md5_size;
#endif

    EnvelopeParserConfig(envelope *env);
};

inline EnvelopeParserConfig::EnvelopeParserConfig(envelope *env) : e(env), cur_header(-1), was_header_body(false),
                                                                   received_header(false), received_counter(0), in_body(false)
#ifdef HAVE_MD5CHECK_OLD
, md5_ctx(NULL)
#endif
{
}

inline void EnvelopeParserConfig::header_continue()
{
}

inline void EnvelopeParserConfig::header_name(const char* p, size_t count)
{
    if(in_body)
        return;

    size_t offset = envelope_heap_get_string(e, count+1);
    cur_header = envelope_new_header(e);

    if(received_header)
        {
#ifndef KIS_USAGE
            ip_extractor_end_of_line(e->ipe);
#endif//KIS_USAGE
            received_header = false;
        }
    
    memcpy(e->heap.array + offset, p, count);
    *(e->heap.array + offset + count) = 0;
    
    e->headers.array[cur_header].header = offset;
    
    was_header_body = false;
    
    if(count == sizeof("Received")-1 && cmp_nocase(p, "Received", count))
        {
            received_header = true;
            received_counter++;
        }
}

inline void EnvelopeParserConfig::header_body(const char* p, size_t count, const char* codepage, size_t cp_count)
{
    if(!p || !count)
	return;

    if(in_body)
        return;

    int cp_id = CP_KOI8R;

#ifndef KIS_USAGE
    if(received_header && received_counter <= e->max_received_headers)
        ip_extractor_parse(e->ipe, p, count);
#endif//KIS_USAGE

    if(codepage)
	cp_id = check_encoding(codepage, cp_count);
    if(cp_id == CP_UNKNOWN)
	cp_id = CP_KOI8R;
    

    if(!was_header_body)
	{
	    e->headers.array[cur_header].value = envelope_heap_get_string(e, count+1);

	    memcpy(ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), p, count);
	 
	    if(cp_id != CP_KOI8R)
		ConvertData((unsigned char *)ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), count, cp_id, CP_KOI8R);		    

	    *ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value + count) = '\0';
	    was_header_body = true;
	}
    else
	{
	    envelope_heap_resize(e, count);

	    char *offset = strchr(ENVELOPE_HEAP_STRING(e, e->headers.array[cur_header].value), '\0');

	    memcpy(offset, p, count);
	    
	    if(cp_id != CP_KOI8R)
		ConvertData((unsigned char *)offset, count, cp_id, CP_KOI8R);

	    *(offset + count) = '\0';
	}
}

inline void EnvelopeParserConfig::part_begin(unsigned int level)
{
    in_body = true;

#ifdef HAVE_KAVCLIENT
    envelope_new_ctype(e);
#endif
#ifdef WITH_GSG
    if(e->binary_processor)
	binary_endpart(e->binary_processor);
#endif
#ifdef HAVE_MD5CHECK_OLD
    if(md5_ctx)
	{
	    md5data *mdd = envelope_new_md5(e);
	    if(mdd)
		{
		    MD5Final(mdd->sum,md5_ctx);
		    mdd->size = md5_size;
		}
	    bzero(md5_ctx,sizeof(MD5_CTX));
	    MD5Init(md5_ctx);
	    md5_size = 0;
	}
    else
	{
	    // no prologue, start of message
	    if((md5_ctx = (MD5_CTX*)malloc(sizeof(MD5_CTX))))
		{
		    bzero(md5_ctx,sizeof(MD5_CTX));
		    MD5Init(md5_ctx);
		    md5_size = 0;
		}
	}
#endif
}

inline void EnvelopeParserConfig::uue_part_begin(unsigned int level, int mode, const char *filename, size_t filename_length)
{
    in_body = true;

#ifdef WITH_GSG
    if(e->binary_processor)
	binary_endpart(e->binary_processor);
#endif
#ifdef HAVE_MD5CHECK_OLD
    // закрываем md5, если он был, сохраняем контрольную сумму,
    // начинаем заново
    if(md5_ctx)
	{
	    md5data *mdd = envelope_new_md5(e);
	    if(mdd)
		{
		    MD5Final(mdd->sum,md5_ctx);
		    mdd->size = md5_size;
		}
	    bzero(md5_ctx,sizeof(MD5_CTX));
	    MD5Init(md5_ctx);
	    md5_size = 0;
	}
    else
	{
	    // no prologue, start of message
	    if((md5_ctx = (MD5_CTX*)malloc(sizeof(MD5_CTX))))
		{
		    bzero(md5_ctx,sizeof(MD5_CTX));
		    MD5Init(md5_ctx);
		    md5_size = 0;
		}
	}
#endif
}


inline void EnvelopeParserConfig::epilogue(unsigned int level)
{
    in_body = true;
#ifdef HAVE_KAVCLIENT
    // nothing to do in epilogue
#endif
#ifdef WITH_GSG
    if(e->binary_processor)
	binary_endpart(e->binary_processor);
#endif
#ifdef HAVE_MD5CHECK_OLD
    // закрываем md5 если он был, сохраняем контрольную сумму,
    // если мы не на нулевом уровне, то part_begin все переоткроет
    if(md5_ctx)
	{
	    md5data *mdd = envelope_new_md5(e);
	    if(mdd)
		{
		    MD5Final(mdd->sum,md5_ctx);
		    mdd->size = md5_size;
		}
	    free(md5_ctx);
	    md5_ctx=NULL;
	    md5_size = 0;
	}
#endif
}

inline void EnvelopeParserConfig::body(const char* p, size_t count, bool eol)
{
#ifndef KIS_USAGE
    if(received_header)
        ip_extractor_parse_finish(e->ipe);
#endif//KIS_USAGE
    in_body = true;

#ifdef HAVE_KAVCLIENT
    if(!e->ctypes.used)
	{
	    // message without Content-Type and multipart
	    ctype_data *ctd = envelope_new_ctype(e);
	    ctd->size = 0; 
// empty type/subtype, will be handled in libkavclient
	}
    if(e->ctypes.used > 1)
	{
	    // one ctype for entire message and one for last part of multipart
	    ctype_data *ctd = envelope_first_ctype(e);
	    ctd->size+=count;
	    ctd = envelope_current_ctype(e);
	    ctd->size+=count;
	}
    else
	{
	    // only one part
	    ctype_data *ctd = envelope_first_ctype(e);
	    ctd->size+=count;
	}
#endif
#ifdef WITH_GSG
    if(e->binary_processor)
	binary_put(e->binary_processor,(void*)p,count);
#endif
#ifdef HAVE_MD5CHECK_OLD
    // если мы ничего не считали еще - аллоцируем MD5 context
    if(!md5_ctx)
	{
#if 0
	    // we dont want to analyze text-only messages,
	    // only attachments
	    if((md5_ctx = (MD5_CTX*)malloc(sizeof(MD5_CTX))))
		{
		    bzero(md5_ctx,sizeof(MD5_CTX));
		    MD5Init(md5_ctx);
		}
#endif
	}
    if(md5_ctx)
	{
	    MD5Update(md5_ctx,(unsigned char*)p,count);
	    md5_size += count;
	}
#endif
}

inline void EnvelopeParserConfig::content_type(const char* p, size_t count)
{
#ifdef HAVE_KAVCLIENT
    ctype_data *ctd;
    if(e->ctypes.used) // already have something set from part_begin
	ctd = envelope_current_ctype(e);
    else
	ctd = envelope_new_ctype(e); // nothing had, so create new --
				     // 1st CT-header in message
    ctd->type = (char*)malloc(count+1);
    strncpy(ctd->type,p,count);
    ctd->type[count]=0;
#endif
}

inline void EnvelopeParserConfig::content_subtype(const char* p, size_t count)
{
#ifdef HAVE_KAVCLIENT
    ctype_data *ctd;
    ctd = envelope_current_ctype(e);
    if(!ctd) return;
    ctd->subtype = (char*)malloc(count+1);
    strncpy(ctd->subtype,p,count);
    ctd->subtype[count]=0;
#endif
    
}

inline void EnvelopeParserConfig::content_charset(const char* p, size_t count)
{
}

inline void EnvelopeParserConfig::content_full_type(const char* p, size_t count)
{
}

inline void EnvelopeParserConfig::content_name(const char* p, size_t count)
{
}

inline void EnvelopeParserConfig::content_protocol(const char* p, size_t count)
{
}

#ifndef _WIN32 // !!!
static envelope *envelope_for_sort;

static int compare_headers(const void *a, const void *b)
{
    const msg_header *mh_a = (const msg_header *)a;
    const msg_header *mh_b = (const msg_header *)b;

    return strcasecmp(ENVELOPE_HEAP_STRING(envelope_for_sort, mh_a->header),
		      ENVELOPE_HEAP_STRING(envelope_for_sort, mh_b->header));
}
#else
class CmpFunctor
{
public:
	CmpFunctor(envelope* env) : envelope_for_sort(env) {};
	CmpFunctor(const CmpFunctor &src) : envelope_for_sort(src.envelope_for_sort) {};
	CmpFunctor& operator=(const CmpFunctor &rhs)
	{
		if(this != &rhs)
		{
			envelope_for_sort = rhs.envelope_for_sort;
		}
		return *this;
	}
	
	CmpFunctor& operator*(){ return *this; };

	int operator()(const void *a, const void *b)
	{
		const msg_header *mh_a = (const msg_header *)a;
		const msg_header *mh_b = (const msg_header *)b;

		return strcasecmp(ENVELOPE_HEAP_STRING(envelope_for_sort, mh_a->header),
		      ENVELOPE_HEAP_STRING(envelope_for_sort, mh_b->header));
	}
private:
	envelope *envelope_for_sort;
};
#endif //_WIN32


#ifdef WITH_GSG
int envelope_prepare_headers(envelope *e,binary_data_t *dt)
#else
int envelope_prepare_headers(envelope *e)
#endif
{
    EnvelopeParserConfig pc(e);

#ifndef _WIN32 //!!!!
    envelope_for_sort = e;
#endif //_WIN32

#ifdef WITH_GSG
    e->binary_processor = dt;
#endif


    MIMEParser<EnvelopeParserConfig> parser(&pc);
#ifdef HAVE_MD5CHECK_OLD
    int md5count = 0;
#endif
    
    int res = 0;
#define RETURN(x) 				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0)

    const char *line_begin = e->data.text;
    size_t      line_length = 0;
    size_t      i;
    

    for(i = 0; i < e->data.length; i++)
	{
            if(e->data.text[i] == 10)
                {
                    if(parser.parse(line_begin, line_length) == MIMEParserGlobal::PARSE_ERROR)
                        RETURN(-1);
                    
                    line_begin = e->data.text+i+1;
                    line_length = 0;
                }
            else 
                line_length++;
        }

    if(line_begin && line_length)
	parser.parse(line_begin, line_length);

    pc.in_body = 0;
#ifdef HAVE_MD5CHECK_OLD
    // dump MD5 sums
    // last part without epilogue
    if(pc.md5_size>0 && pc.md5_ctx)
	{
	    md5data *mdd = envelope_new_md5(e);
	    if(mdd)
		{
		    MD5Final(mdd->sum,pc.md5_ctx);
		    mdd->size = pc.md5_size;
		}
	    free(pc.md5_ctx);
	    pc.md5_ctx=NULL;
	    pc.md5_size = 0;
	}
    if(e->md5_sums.array && e->md5_sums.used)
	{
	    for(i=0;i<e->md5_sums.used; i++)
		{
		    if(e->md5_sums.array[i].size > 0 && check_md5(e->md5_sums.array[i].sum))
			{
			    md5count++;
			    char mdbuf[33];
			    static const char hex[]="0123456789abcdef";
			    
			    for(int j=0;j<16;j++)
				{
				    mdbuf[j+j] = hex[e->md5_sums.array[i].sum[j]>>4];
				    mdbuf[j+j+1] = hex[e->md5_sums.array[i].sum[j]&0xf];
				}
			    mdbuf[32]='\0';
			    logger_printf(LOGGER_INFO|LOGGER_VERBOSE_INFORMATIVE, "MD5 Found (idx=%d, size=%d): %s",
					  i,e->md5_sums.array[i].size,mdbuf);
			}
		}
	}
	    //
    if(pc.md5_ctx)
	{
	    free(pc.md5_ctx);
	    pc.md5_ctx = NULL;
	}
    if(md5count>0)
	{
	    pc.header_name("X-SpamTest-Checksum",19);
	    pc.header_body("MATCH",5,NULL,0);
	}
#endif
#ifdef WITH_GSG
    if(e->binary_processor)
	binary_endpart(e->binary_processor);
    if(e->binary_processor && e->binary_processor->md5list)
	{
	    unsigned int match[NGSG];
	    char header[64], value[32];
	    unsigned int i;
	    bzero(match,sizeof(match));
	    for(i=0;i<e->binary_processor->md5list->used;i++)
		{
		    char str[33];
		    md52str(str,e->binary_processor->md5list->array[i].md5);
		    if(check_md5(e->binary_processor->md5list->array[i].md5))
			{
			    match[e->binary_processor->md5list->array[i].type%NGSG]++;
			}
		}
	    for(i=0;i<NGSG;i++)
		if(match[i])
		    {
			if( i==1 )
			    { // compatibility
				pc.header_name("X-SpamTest-Checksum",19);
				pc.header_body("MATCH",5,NULL,0);
			    }
#if 1
			snprintf(header,64,"X-SpamTest-Internal-GSG-%d",i);
			snprintf(value,32,"MATCH %u",match[i]);
			pc.header_name(header,strlen(header));
			pc.header_body(value,strlen(value),NULL,0);
			logger_printf(LOGGER_INFO|LOGGER_VERBOSE_HIGH, "GSG%d match (%d times)",
				      i,match[i]);
#endif
		    }
	}
#endif

#ifndef _WIN32 //!!!
    local_mergesort(e->headers.array, e->headers.used, sizeof(msg_header), compare_headers);
#else
	local_mergesort(e->headers.array, e->headers.used, sizeof(msg_header), CmpFunctor(e));
#endif //_WIN32

#undef RETURN
  finish:
    
    return res;
}

int envelope_search_header(envelope *e, const char *header, int (* callback)(void *p, const char *value), void *p)
{
    int l = 0, r = e->headers.used-1, t, res, founded = -1;

    for( ; r >= l; )
	{
	    t = (l + r) >> 1;

	    res = strcasecmp(header, ENVELOPE_HEAP_STRING(e, e->headers.array[t].header));

	    if(res < 0)
		r = t-1;
	    else if(res > 0)
		l = t+1;
	    else
		{
		    founded = t;
		    break;
		}
	}

    if(founded >= 0 && callback)
	{
	    int i;
	    
	    for(i = founded; i >= 0; i--)
		{
		    if(strcasecmp(header, ENVELOPE_HEAP_STRING(e, e->headers.array[i].header)) == 0)
			{
			    if(callback(p, ENVELOPE_HEAP_STRING(e, e->headers.array[i].value)))
				{
				    founded = i;
				    goto finish;
				}
			}
		    else
			break;
		}

	    for(i = founded+1; i < e->headers.used; i++)
		{
		    if(strcasecmp(header, ENVELOPE_HEAP_STRING(e, e->headers.array[i].header)) == 0)
			{
			    if(callback(p, ENVELOPE_HEAP_STRING(e, e->headers.array[i].value)))
				{
				    founded = i;
				    goto finish;
				}
			}
		    else
			break;
		}

	    founded = -1;
	}

  finish:

    return founded;
}

int envelope_search_header_iterate(envelope *e, const char *header, int (* callback)(void *p, const char *value), void *p)
{
    int l = 0, r = e->headers.used-1, t, res, founded = -1;
    int i, from, to;

    for( ; r >= l; )
	{
	    t = (l + r) >> 1;

	    res = strcasecmp(header, ENVELOPE_HEAP_STRING(e, e->headers.array[t].header));

	    if(res < 0)
		r = t-1;
	    else if(res > 0)
		l = t+1;
	    else
		{
		    founded = t;
		    break;
		}
	}

    from = founded;
    to   = founded;

    for(i = founded; i >= 0; i--)
	{
	    if(strcasecmp(header, ENVELOPE_HEAP_STRING(e, e->headers.array[i].header)) == 0)
		from = i;
	    else
		break;
	}
    
    for(i = founded+1; i < e->headers.used; i++)
	{
	    if(strcasecmp(header, ENVELOPE_HEAP_STRING(e, e->headers.array[i].header)) == 0)
		to  = i;
	    else
		break;
	}
    
    if(founded >= 0 && callback)
	{

	    for(i = from; i <= to; i++)
		{
		    if(callback(p, ENVELOPE_HEAP_STRING(e, e->headers.array[i].value)))
			{
			    founded = i;
			    goto finish;
			}
		}

	    founded = -1;
	}

  finish:

    return founded;
}


size_t envelope_heap_get_string(envelope *e, size_t length)
{
    size_t res = 0;

    if(e->heap.used + length >= e->heap.alloc)
	{
	    e->heap.array = (char *)realloc(e->heap.array, e->heap.alloc += (length / 1024 + 2)*1024);

	    memset(ENVELOPE_HEAP_STRING(e, e->heap.used), 0, e->heap.alloc - e->heap.used);
	}

    res = e->heap.used;
    e->heap.used += length;

    return res;
}

int envelope_heap_resize(envelope *e, size_t length)
{
    if(e->heap.used + length >= e->heap.alloc)
	{
	    if(e->heap.array)
		e->heap.array = (char *)realloc(e->heap.array, e->heap.alloc += (length / 1024 + 2)*1024);

	    memset(ENVELOPE_HEAP_STRING(e, e->heap.used), 0, e->heap.alloc - e->heap.used);
	}

    e->heap.used += length;
    
    return 0;
}

/*
 * Attributes storage.
 *
 * Right now it is 3 possibly attributes here, so we can search
 * in storage with linear search thru whole storage.
 *
 * NOTE. All attribute data (name and content) supposed to be allocated
 * by calling side and have to be deallocated after this envelope
 * recycling.
 */

static int envelope_find_attr_index(envelope *e, const char *attr_name)
{
    size_t i = 0;

    for(i = 0; i < e->attrs.used; i++)
        if(strcmp(attr_name, e->attrs.array[i].name) == 0)
            return i;

    return -1;
}


int envelope_add_attr(envelope *e, const char *attr_name, const char *content)
{
    int index = -1;

    index = envelope_find_attr_index(e, attr_name);

    if(index < 0)
        {
            if(e->attrs.used == e->attrs.alloc)
                {
                    if(e->attrs.array)
                        e->attrs.array = (envelope_attr *)realloc(e->attrs.array, (e->attrs.alloc += 16)*sizeof(envelope_attr));
                    else
                        e->attrs.array = (envelope_attr *)malloc((e->attrs.alloc = 16)*sizeof(envelope_attr));
                }

            index = e->attrs.used++;
        }

    if(index >= 0)
        {
            e->attrs.array[index].name = attr_name;
            e->attrs.array[index].content = content;
        }

    return index;
}

const char *envelope_find_attr(envelope *e, const char *attr_name)
{
    int index = envelope_find_attr_index(e, attr_name);

    if(index >= 0)
        return e->attrs.array[index].content;
    else
        return NULL;
}


/*
 * <eof envelope.cpp>
 */
