/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: interpreter.cpp                                                       *
 * Created: Sat May 11 14:47:29 2002                                           *
 * Desc: Apply profiles to envelope (and generate actions for it.)             *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  interpreter.cpp
 * \brief Apply profiles to envelope (and generate actions for it.)
 *
 * Revision 1.15  2005/09/16 15:28:47  esmirnov
 * KIS_USAGE
 *
 * Revision 1.14  2005/04/12 19:26:42  vtroitsky
 * >= rate
 *
 * Revision 1.13  2005/04/12 19:16:23  vtroitsky
 * Bayes rate condition check transformed to >. Debug output for message Bayes rate added.
 *
 * Revision 1.12  2005/04/12 14:58:36  vtroitsky
 * BayesMatch condition added
 *
 * Revision 1.11  2005/01/22 05:23:13  vtroitsky
 * Skipping formal.xml profile logic added
 *
 * Revision 1.10  2005/01/20 17:37:05  vtroitsky
 * BEGINx macros in normal debug mode fixed
 *
 * Revision 1.9  2005/01/20 17:19:43  vtroitsky
 * *** empty log message ***
 *
 * Revision 1.8  2005/01/20 11:52:48  vtroitsky
 * Additional logging and debugging features
 *
 * Revision 1.7  2005/01/19 16:00:05  vtroitsky
 * The results of warning hunting fixed
 *
 * Revision 1.6  2005/01/17 00:09:01  vtroitsky
 * Debug logging improved (removed unnecessary outputs, added additional info messages)
 *
 * Revision 1.5  2005/01/16 23:00:27  vtroitsky
 * The results of warnings hunting
 *
 * Revision 1.4  2005/01/11 22:21:37  alexiv
 * *** empty log message ***
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
#include <client-filter/SpamTest/CFilterDll.h>

#ifdef _WIN32
#pragma warning (disable : 4102)
#pragma warning (disable : 4018)
#include <malloc.h>
#include <wintypes.h>
#include <_include/nix_types.h>
#endif

#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifndef _WIN32
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/param.h>
#include <netdb.h>
#endif

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif

#ifdef USE_RBL
#ifndef USE_OLD_DNS_RESOLVER
#include "smtp-filter/adc/adc.h"
#endif
#endif

#ifdef CONFIDENTIAL
#include <arc-extractor.h>
#endif /* CONFIDENTIAL */

#include "interpreter.h"
#include "profile.h"
#include <smtp-filter/common/logger.h>

#include <smtp-filter/common/config.h>

#include <smtp-filter/contrib/libpatricia/patricia.h>

//#include "filter-module.h"

#ifdef WITH_SCORES
#include <scores.h>
#endif

#ifdef INFORMER
#include "smtp-filter/libmflt/server.h"
#endif

#ifndef _WIN32
static struct
#else
struct interpreter
#endif //_WIN32
{
    /*
     * XXX At this time memory is allocated, but nobody
     * free it. 
     */

    struct {
	char   *array;
	size_t  alloc;
    } buf;
#ifdef USE_RBL
#ifndef USE_OLD_DNS_RESOLVER
    struct {
        struct adc_object adc;
        int inited;
    } resolver;
#endif
#endif
}
#ifdef _WIN32
;
#else
INTERPRETER;


static unsigned int interpreter_inited = 0;
#endif //_WIN32

struct interpreter_data
{
    interpreter_config        *ic;
    profile_iterate_callbacks  pic;
    action                    *cur_action;

    profile                   *cur_profile;

    unsigned int               rule_number;

    int need_to_finish;

#ifdef _WIN32
	interpreter				   ib;
#endif
};

/*
 * iqmgr_connection functions
 */

#ifdef INFORMER

static iqmgr_connection *iconn = NULL;
static const char *iconn_socket = NULL;

iqmgr_connection *filter_module_iqmgr_connection()
{
    if(!iconn)
	{
	    iconn = iqmgr_connection_open(iconn_socket ? iconn_socket : IQMGR_SOCKET_DEFAULT);
	    
	    if(iconn == NULL)
		logger_printf(LOGGER_ERROR, "Can\'t open iqmgr connection.");
	}

    return iconn;
}

void filter_module_iqmgr_connection_close()
{
    if(iconn)
	{
	    iqmgr_connection_close(iconn);
	    iconn = NULL;
	}
}

#endif    

int interpretator_close()
{

#ifdef INFORMER
    filter_module_iqmgr_connection_close();
#endif 

    return 0;
}


/*
 * Check conditions
 */
#ifndef WITH_LOGDEBUG_LEV2
#define BEGIN(NAME)							\
    interpreter_data *id  = (interpreter_data *)p;			\
    int               res = 0; 						\
    const char       *name = NAME;					

#define BEGIN1(NAME,PAR)							\
    interpreter_data *id  = (interpreter_data *)p;			\
    int               res = 0; 						\
    const char       *name = NAME;					

#define BEGIN2(NAME,P1,P2)							\
    interpreter_data *id  = (interpreter_data *)p;			\
    int               res = 0; 						\
    const char       *name = NAME;					
   
#define FINISH										\
    finish:										\
       return res;
    	
#else

#define BEGIN(NAME)\
    interpreter_data *id  = (interpreter_data *)p;\
    int               res = 0;\
    const char       *name = NAME;\
    const char       *par1 = NULL;\
    const char       *par2 = NULL;         
    
#define BEGIN1(NAME,PAR) \
    interpreter_data *id  = (interpreter_data *)p;\
    int               res = 0; 						\
    const char       *name = NAME; \
    const char       *par1 = PAR; \
    const char       *par2 = NULL;            


#define BEGIN2(NAME,P1,P2) \
    interpreter_data *id  = (interpreter_data *)p;\
    int               res = 0;\
    const char       *name = NAME;\
    const char       *par1 = P1;\
    const char       *par2 = P2;
    
#define FINISH										\
    finish:   if(res || name[0] == 'a' || stricmp(name, "condition_header_match") == 0)           \
    {   \
        logger_printf(LOGGER_DEBUG, (par1 ? (par2 ? "\t%s (\"%s\",\"%s\"): %i" : "\t%s (\"%s\")") : "\t%s"), name, par1, par2, res); \
    }   \
    return res;
#endif //WITH_LOGDEBUG_LEV2


#define ENVELOPE       (id->ic->e)
#define LIST_DB        (id->ic->ldb)
#define ACTION_STORAGE (id->ic->as)
#define ACTION         (id->cur_action)
#define CONTENT_FILTER (id->ic->cf)
#define FILTER_RESULT  (id->ic->fr)
#define PROFILE        (id->cur_profile)
#define IQMGR_CONN     (filter_module_iqmgr_connection())
#define MESSAGE        (id->ic->msg)
#define BAYES_RATE     (id->ic->bayes_rate)

#ifdef _WIN32
#define INTERPRETER    (id->ib)		
#endif //_WIN32

#define RETURN(r)				\
    do {					\
        res = (r);				\
        goto finish;				\
    } while(0)

static int call_filter(void *p);

static int condition_smtp_from(void *p, const char *email)
{
    char *ptr = NULL;
    BEGIN1("condition_smtp_from",email);

    if(ENVELOPE->from.text == NULL || ENVELOPE->from.length == 0)
	{
	    if(email[0] == '\0')
		RETURN(1);
	    else
		RETURN(0);
	}
    else if(email[0] == '@')
	{
	    ptr = (char *)memchr(ENVELOPE->from.text, '@', ENVELOPE->from.length);
	    if(ptr)
		RETURN(strncasecmp(ptr, email, 
				   ENVELOPE->from.length - (ptr - ENVELOPE->from.text)) == 0);
	    else
		RETURN(0);
		
	}
    else
	RETURN(strlen(email) == ENVELOPE->from.length &&
	       strncasecmp(ENVELOPE->from.text,  
				     email, ENVELOPE->from.length) == 0);

    FINISH;
}

static int condition_smtp_from_list(void *p, list_id lid)
{
    BEGIN("condition_smtp_from_list");

#ifndef KIS_USAGE
    RETURN(list_email_check(LIST_DB, lid, ENVELOPE->from.text, ENVELOPE->from.length) == LIST_CHECK_EXIST);
#endif//KIS_USAGE

    FINISH;
}

static int condition_smtp_to(void *p, const char *email)
{
    const char *ptr = NULL;
    BEGIN1("condition_smtp_to",email);


    for(size_t i = 0; i < ACTION_STORAGE->rcpts.used; i++)
	{
	    if(ACTION_RCPT_CHECK(ACTION, i))
		{

		    if(email[0] == '@')
			{
			    ptr = strchr(ACTION_STORAGE->rcpts.array[i].rcpt, '@');
			    if(ptr && strcasecmp(ptr, email) == 0)
				RETURN(1);
			}
		    else
			{
			    if(strcasecmp(ACTION_STORAGE->rcpts.array[i].rcpt, email) == 0)
				RETURN(1);
			}
		}
	}

    FINISH;
}



static int condition_smtp_to_list(void *p, list_id lid)
{
    BEGIN("condition_smtp_to_list");

#ifndef KIS_USAGE
    for(size_t i = 0; i < ACTION_STORAGE->rcpts.used; i++)
	{
	    if(ACTION_RCPT_CHECK(ACTION, i))
		{
		    if(list_email_check(LIST_DB, lid, ACTION_STORAGE->rcpts.array[i].rcpt, 
					LIST_CHECK_ASCIIZ))
			RETURN(1);
		}
	}
#endif//KIS_USAGE
    FINISH;

}

static int condition_has_header(void *p, const char *header)
{
    BEGIN1("condition_has_header", header);
    
    enum {
	NOT_ASSIGNED,
	DELETED,
	FOUNDED
    } temp_result = NOT_ASSIGNED;

    for(size_t i = 0; i < ACTION->headers.used; i++)
	{
	    if(strcasecmp(header, ACTION->headers.array[i].header) == 0)
		{
		    if(ACTION->headers.array[i].type == HEADER_MODIFY_ADD || 
		       ACTION->headers.array[i].type == HEADER_MODIFY_CHANGE ||
		       ACTION->headers.array[i].type == HEADER_MODIFY_NEW ||
		       ACTION->headers.array[i].type == HEADER_MODIFY_PREPEND)
			{
			    temp_result = FOUNDED;
			}
		    else if(ACTION->headers.array[i].type == HEADER_MODIFY_REMOVE)
			{
			    temp_result = DELETED;
			}
		}
	}

    if(temp_result == DELETED)
	RETURN(0);
    else if(temp_result == FOUNDED)
	RETURN(1);
    else if(envelope_search_header(ENVELOPE, header, NULL, NULL) >= 0)
	RETURN(1);
    else
	RETURN(0);

    FINISH;
}

static int esearch_callback(void *p, const char *value)
{
    return regexec((regex_t *)p, value, (size_t) 0, NULL, 0) == 0;
}

static int condition_header_match(void *p, const char *header, const char *regexp, regex_t *compiled_regexp)
{
    BEGIN2("condition_header_match",header,regexp);
        
    enum {
	DELETED,
	MATCH,
	NOT_MATCH
    } temp_result = NOT_MATCH;

    size_t need_to_check = 0, hdr_len;
    int i;
    int first = -1, last = -1;

    /*
     * Trying to find CHANGE or DELETE operation for this header.
     */

    for(i = 0; i < ACTION->headers.used; i++)
	{
	    if(strcasecmp(header, ACTION->headers.array[i].header) == 0)
		{
		    last = i;

		    switch(ACTION->headers.array[i].type)
			{
			case HEADER_MODIFY_REMOVE:
			    first = -1;
			    temp_result = DELETED;
			    break;
			case HEADER_MODIFY_CHANGE:
			    first = i;
			    temp_result = DELETED;
			    break;
			case HEADER_MODIFY_NONE:
			    break;
			default:
			    if(first < 0) 
				first = i;
			}
		}
	}
    
    /*
     * Short description of this code.
     *
     * All HeaderAdd and HeaderPrepend actions merged into buffers and checked.
     * All HeaderNew and HeaderChange actions checked one-by-one.
     *
     * Note: Add-style actions is not merged with headers from the envelope.
     */


    if(first >= 0)
	{
	    need_to_check = 0;

	    for(i = first; i <= last && temp_result != MATCH; i++)
		{
		    if(strcasecmp(header, ACTION->headers.array[i].header) == 0)
			{
			    switch(ACTION->headers.array[i].type)
				{
				case HEADER_MODIFY_ADD:

				    hdr_len = strlen(ACTION->headers.array[i].value);
				    if(need_to_check + hdr_len + 2 > INTERPRETER.buf.alloc)
					{
					    INTERPRETER.buf.array = (char *)realloc(INTERPRETER.buf.array, 
										    INTERPRETER.buf.alloc = ((need_to_check + hdr_len + 10) / 1024 + 3)*1024);
					}
				    if(need_to_check)
					INTERPRETER.buf.array[need_to_check++] = ' ';

				    memcpy(INTERPRETER.buf.array + need_to_check, ACTION->headers.array[i].value, hdr_len);
				    need_to_check += hdr_len;

				    break;

				case HEADER_MODIFY_NEW:
				case HEADER_MODIFY_CHANGE:
				    temp_result = regexec(compiled_regexp, ACTION->headers.array[i].value, (size_t) 0, NULL, 0) ? temp_result : MATCH;
				    if(temp_result == MATCH)
                    RETURN(1);
          		    break;
	    
				}
			}
		}

	    if(need_to_check > 0)
		{
		    INTERPRETER.buf.array[need_to_check++] = '\0';
		    temp_result = regexec(compiled_regexp, INTERPRETER.buf.array, (size_t) 0, NULL, 0) ? temp_result : MATCH;
		    if(temp_result == MATCH)
            RETURN(1); 
      		
		}

	    need_to_check = 0;

	    for(i = last; i >= first && temp_result != MATCH; i--)
		{
		    if(strcasecmp(header, ACTION->headers.array[i].header) == 0)
			{
			    switch(ACTION->headers.array[i].type)
				{
				case HEADER_MODIFY_PREPEND:

				    hdr_len = strlen(ACTION->headers.array[i].value);
				    if(need_to_check + hdr_len + 2 > INTERPRETER.buf.alloc)
					{
					    INTERPRETER.buf.array = (char *)realloc(INTERPRETER.buf.array, 
										    INTERPRETER.buf.alloc = ((need_to_check + hdr_len + 10) / 1024 + 3)*1024);
					}
				    if(need_to_check)
					INTERPRETER.buf.array[need_to_check++] = ' ';

				    memcpy(INTERPRETER.buf.array + need_to_check, ACTION->headers.array[i].value, hdr_len);
				    need_to_check += hdr_len;

				    break;
				}
			}
		}

	    if(need_to_check > 0)
		{
		    INTERPRETER.buf.array[need_to_check++] = '\0';
		    temp_result = regexec(compiled_regexp, INTERPRETER.buf.array, (size_t) 0, NULL, 0) ? temp_result : MATCH;
		    if(temp_result == MATCH)
                 RETURN(1);
     		
		}


	}

    if(temp_result == MATCH)
         RETURN(1);
    else if(temp_result == DELETED)
        RETURN(0);
    else if(envelope_search_header(ENVELOPE, header, esearch_callback, compiled_regexp) >= 0)
        RETURN(1);
    else
        RETURN(0);
    FINISH;
}

static int condition_attribute_match(void *p, const char *attribute, const char *regexp, regex_t *compiled_regexp)
{
    BEGIN2("condition_attribute_match",attribute,regexp);

    const char *content = envelope_find_attr(ENVELOPE, attribute);
    
    if(!content)
        RETURN(0);

    
    if(regexec(compiled_regexp, content, (size_t) 0, NULL, 0) == 0)
    {
        RETURN(1);
    }
    else
        RETURN(0);

    FINISH;
}

static int condition_content_type_match(void *p, const char *regexp, regex_t *compiled_regexp)
{
    BEGIN1("condition_content_type_match", regexp);
    
#ifdef CONFIDENTIAL

    if(id->ic->message_demimed)
        {
            size_t i;
            char buf[4096];
            size_t length;

            for(i = 0; i < MESSAGE.attachments_count; i++)
                {
                    if(MESSAGE.attachments[i].content_type.ptr == NULL || 
                       MESSAGE.attachments[i].content_type.count == 0)
                        continue;

                    length = MESSAGE.attachments[i].content_type.count;
                    if(length+2 >= sizeof(buf))
                        length = sizeof(buf)-2;

                    memcpy(buf, MESSAGE.attachments[i].content_type.ptr, length);
                    buf[length] = '\0';

                    if(regexec(compiled_regexp, buf, (size_t) 0, NULL, 0) == 0)
                        RETURN(1);
                }

            RETURN(0);
        }
    else
        RETURN(0);

#else /* CONFIDENTIAL */

    RETURN(0);

#endif /* CONFIDENTIAL */

    FINISH;
}

static int condition_file_name_match(void *p, const char *regexp, regex_t *compiled_regexp)
{
    BEGIN1("condition_file_name_match",regexp);

#ifdef CONFIDENTIAL

    if(id->ic->message_demimed)
        {
            size_t i;
            char buf[4096];
            size_t length;

            for(i = 0; i < MESSAGE.attachments_count; i++)
                {
                    if(MESSAGE.attachments[i].filename.ptr == NULL || 
                       MESSAGE.attachments[i].filename.count == 0)
                        continue;

                    length = MESSAGE.attachments[i].filename.count;
                    if(length+2 >= sizeof(buf))
                        length = sizeof(buf)-2;

                    memcpy(buf, MESSAGE.attachments[i].filename.ptr, length);
                    buf[length] = '\0';

                    if(regexec(compiled_regexp, buf, (size_t) 0, NULL, 0) == 0)
                        RETURN(1);
                }

            RETURN(0);
        }
    else
        RETURN(0);

#else /* CONFIDENTIAL */

    RETURN(0);

#endif /* CONFIDENTIAL */

    FINISH;
}


#ifdef WITH_SCORES

int tricks_dump(interpreter_config *ic)
{
    char buf[20];
    size_t len;

    if(ic->tricks_score == 0)
        return 0;

    len = snprintf(buf, sizeof(buf), "%i", ic->prev_trick.counter);
    if(len > sizeof(buf)) len = sizeof(buf);

    if(ic->tricks_desc_used + len + 1 <= sizeof(ic->tricks_description))
        {
            memcpy(ic->tricks_description+ic->tricks_desc_used, buf, len);
            ic->tricks_desc_used += len;

            switch(ic->prev_trick.type)
                {
                case CFLIB_TRICK_INVISIBLE_TEXT:
                    ic->tricks_description[ic->tricks_desc_used] = 'I';

                    memcpy(ic->descriptions.invisible.str + ic->descriptions.invisible.used, buf, len);
                    ic->descriptions.invisible.used += len;
                    ic->descriptions.invisible.str[ic->descriptions.invisible.used++] = 'I';
                    break;

                case CFLIB_TRICK_INVISIBLE_CHAR:
                    ic->tricks_description[ic->tricks_desc_used] = 'i';

                    memcpy(ic->descriptions.invisible.str + ic->descriptions.invisible.used, buf, len);
                    ic->descriptions.invisible.used += len;
                    ic->descriptions.invisible.str[ic->descriptions.invisible.used++] = 'i';
                    break;

                case CFLIB_TRICK_RUSLAT_WORD:
                    ic->tricks_description[ic->tricks_desc_used] = 'M';

                    memcpy(ic->descriptions.multi_alphabet.str + ic->descriptions.multi_alphabet.used, buf, len);
                    ic->descriptions.multi_alphabet.used += len;
                    ic->descriptions.multi_alphabet.str[ic->descriptions.multi_alphabet.used++] = 'M';

                    break;

                case CFLIB_TRICK_RUSLAT_CONV_WORD:
                    ic->tricks_description[ic->tricks_desc_used] = 'N';

                    memcpy(ic->descriptions.normalization.str + ic->descriptions.normalization.used, buf, len);
                    ic->descriptions.normalization.used += len;
                    ic->descriptions.normalization.str[ic->descriptions.normalization.used++] = 'N';
                    break;

                case CFLIB_TRICK_RUSLAT_CONV_CHAR:
                    ic->tricks_description[ic->tricks_desc_used] = 'n';

                    memcpy(ic->descriptions.normalization.str + ic->descriptions.normalization.used, buf, len);
                    ic->descriptions.normalization.used += len;
                    ic->descriptions.normalization.str[ic->descriptions.normalization.used++] = 'n';
                    break;

                case CFLIB_TRICK_COLOR_CHANGE:
                    ic->tricks_description[ic->tricks_desc_used] = 'c';

                    memcpy(ic->descriptions.color_change.str + ic->descriptions.color_change.used, buf, len);
                    ic->descriptions.color_change.used += len;
                    ic->descriptions.color_change.str[ic->descriptions.color_change.used++] = 'c';
                    break;

                case CFLIB_TRICK_CSS:
                    ic->tricks_description[ic->tricks_desc_used] = 'D';

                    memcpy(ic->descriptions.css.str + ic->descriptions.css.used, buf, len);
                    ic->descriptions.css.used += len;
                    ic->descriptions.css.str[ic->descriptions.css.used++] = 'D';
                    break;
                    
                default:
                    ic->tricks_description[ic->tricks_desc_used] = 'E';
                    break;

                }

            ic->tricks_desc_used++;
        }
    
    return 0;
}

int tricks_score_callback(void *context, unsigned int score_type, int score, unsigned int type)
{
    interpreter_config *ic = (interpreter_config *)context;

    if(ic->tricks_score)
        {
            if(ic->prev_trick.type == type)
                ic->prev_trick.counter += score;
            else
                {
                    tricks_dump(ic);
                    ic->prev_trick.type = type;
                    ic->prev_trick.counter = score;
                }
        }
    else
        {
            ic->prev_trick.type = type;
            ic->prev_trick.counter = score;
        }

    ic->tricks_score += score;

    return 0;
}
#endif

static int collect_ips(void *p)
{
    BEGIN("collect_ip");
#ifndef KIS_USAGE
    
    size_t  i;
    int last_line = -1, cur_line = -1;
    struct ip_info *ipi;
    in_addr_t addr_1, addr_2;

#define ADD_IP(IP)                                                      \
    do {                                                                \
        if(id->ic->ips.alloc == id->ic->ips.used)                       \
            {                                                           \
                if(id->ic->ips.array)                                   \
                    id->ic->ips.array = (in_addr_t *)realloc(id->ic->ips.array, sizeof(in_addr_t)*(id->ic->ips.alloc += 128)); \
                else                                                    \
                    id->ic->ips.array = (in_addr_t *)malloc(sizeof(in_addr_t)*(id->ic->ips.alloc += 128)); \
            }                                                           \
                                                                        \
        id->ic->ips.array[id->ic->ips.used++] = IP;                     \
        if(cur_line == 0) id->ic->ips.first_line = 1;                   \
                                                                        \
    } while(0)

    if(id->ic->ips.parsed)
        RETURN(0);

    addr_1 = INADDR_NONE;
    addr_2 = INADDR_NONE;
    cur_line = 0;
    
    for(i = 0; i < ip_extractor_ips_count(ENVELOPE->ipe); i++)
        {
            ipi = ip_extractor_get_ip_info(ENVELOPE->ipe, i);

#if 0
            printf("extracted %i : %i.%i.%i.%i from line %i\n", i,
                   ipi->addr & 0xff,
                   (ipi->addr & 0xff00) >> 8,
                   (ipi->addr & 0xff0000) >> 16,
                   (ipi->addr & 0xff000000) >> 24,
                   ipi->line);
#endif

            if(ipi->line == last_line)
                continue;
            else if(cur_line != ipi->line && (addr_1 != INADDR_NONE || addr_2 != INADDR_NONE))
                {
                    if(addr_2 != INADDR_NONE)
                        ADD_IP(addr_2);
                    else 
                        ADD_IP(addr_1);

                    addr_1 = INADDR_NONE;
                    addr_2 = INADDR_NONE;
                }
            
            cur_line = ipi->line;

            if(addr_1 == INADDR_NONE && ipi->next_char != '.')
                addr_1 = ipi->addr;

            if(ipi->prev_char == '[' && ipi->next_char == ']')
                {
                    ADD_IP(ipi->addr);
                    last_line = ipi->line;
                    addr_1 = INADDR_NONE;
                    addr_2 = INADDR_NONE;
                }

            if(addr_2 == INADDR_NONE && ipi->prev_char == '(' && ipi->next_char == ')')
                addr_2 = ipi->addr;
        }

    if(addr_2 != INADDR_NONE)
        ADD_IP(addr_2);
    else if(addr_1 != INADDR_NONE)
        ADD_IP(addr_1);

#undef ADD_IP    

    id->ic->ips.parsed = 1;
#endif//KIS_USAGE

    FINISH;
}


static int call_filter(void *p)
{
#ifdef KIS_USAGE
    return 0;
#else
    BEGIN("call_filter");

    if(!id->ic->filter_called)
	{
	    message m;
#ifdef WITH_SCORES
            int i;
#endif /* WITH_SCORES */
	    memset(&m, 0, sizeof(m));

#ifdef WITH_SCORES
            id->ic->tricks_score = 0;
            id->ic->tricks_desc_used = 0;
            
            memset(&(id->ic->descriptions), 0, sizeof(id->ic->descriptions));

            cflib_register_trick_score_callback(tricks_score_callback, id->ic);
#endif /* WITH_SCORES */

#ifndef KASP
			g_CFilterDll.MessageFromMIME(&m, ENVELOPE->data.text, ENVELOPE->data.length);
#endif

#ifdef CONFIDENTIAL
            unsigned int j;
            struct message_extractor_config mec;
            memset(&mec, 0, sizeof(mec));
                              
            mec.depth = 5;
            mec.total_memory_limit = 10*1024*1024;
            mec.one_file_limit = 1*1024*1024;

            if(message_extract_archives(&m, &mec) == MESSAGE_EXTRACT_ERROR)
                logger_printf(LOGGER_ERROR, "Couldn't extract data from archive.");

            id->ic->unknown_parts = 0;
            for(j = 0; j < m.attachments_count; j++)
                {
                    if(m.attachments[j].reserved.r1 || m.attachments[j].reserved.r2)
                        continue;

                    if(m.attachments[j].flags & MSG_ATTACHMENT_EXTRACTED)
                        continue;

                    if(m.attachments[j].count == 0)
                        continue;

                    id->ic->unknown_parts = 1;
                    break;
                }
            
#endif /* CONFIDENTIAL */

#ifdef KASP
		FILTER_RESULT = g_CFilterDll.Check(MESSAGE, FILTER_RESULT, FILTER_DEFAULTS);
#else
	    FILTER_RESULT = g_CFilterDll.Check(&m, FILTER_RESULT, FILTER_DEFAULTS);
#endif

#ifdef WITH_SCORES

            tricks_dump(id->ic);

            cflib_register_trick_score_callback(NULL, NULL);

            ACTION_STORAGE->common_action->registers.values[1] = FILTER_RESULT->scores.tricks;
            for(i = 0; i < ACTION_STORAGE->actions.used; i++)
                ACTION_STORAGE->actions.array[i]->registers.values[1] = FILTER_RESULT->scores.tricks;
#endif /* WITH_SCORES */

	    id->ic->filter_called = 1;

#ifndef KASP
	    g_CFilterDll.MessageDestroy(&m);
#endif

	}

    RETURN(0);
    FINISH;
#endif//KIS_USAGE
}

static int condition_category_match(void *p, unsigned int cat_id)
{
    BEGIN("condition_category_match");
#ifndef KIS_USAGE
    call_filter(p);

    if(FILTER_RESULT && FILTER_RESULT->error_code == FR_OK)
	{
	    for(size_t i = 0; i < FILTER_RESULT->used; i++)
		if(FILTER_RESULT->res[i].CID == cat_id)
		    RETURN(1);
	}

#endif//KIS_USAGE
    RETURN(0);
    FINISH;
}

static int condition_msg_size(void *p, unsigned int limit)
{
    BEGIN("condition_msg_size");

//    if(ENVELOPE->data.length > limit)
	if(ENVELOPE->msg_size > limit)
	RETURN(1);

    FINISH;
}

static int condition_ip_from_list(void *p, list_id lid)
{
    BEGIN("condition_ip_from_list");
#ifndef KIS_USAGE

    size_t i;

    collect_ips(p);
    
    if(ENVELOPE->ip_addr.length > 0 && ENVELOPE->ip_addr.length < 50)
	{
	    if(list_ip_check(LIST_DB, lid, ENVELOPE->ip_addr.text, ENVELOPE->ip_addr.length) == LIST_CHECK_EXIST)
		RETURN(1);
        }

    collect_ips(p);

    for(i = 0; i < id->ic->ips.used; i++)
        {
            if(list_ip_check_ip(LIST_DB, lid, id->ic->ips.array[i]) == LIST_CHECK_EXIST)
                RETURN(1);
        }
#endif//KIS_USAGE
    FINISH;
}

static int condition_ip_from(void *p, in_addr_t addr, in_addr_t mask)
{
    BEGIN("condition_ip_from");

    collect_ips(p);

    if(ENVELOPE->ip_addr.length > 0 && ENVELOPE->ip_addr.length < 50)
	{
	    char *temp = (char *)alloca(ENVELOPE->ip_addr.length+1);
	    memcpy(temp, ENVELOPE->ip_addr.text, ENVELOPE->ip_addr.length);
	    temp[ENVELOPE->ip_addr.length] = '\0';
	    in_addr_t ip = inet_addr(temp);

	    //printf("%08X\n%08X\n%08X\n\n", ip, addr, mask);

	    if((ip & mask) == (addr & mask))
		RETURN(1);
	    else
		RETURN(0);
	}
    else if(id->ic->ips.first_line && id->ic->ips.used)
        {
	    if((id->ic->ips.array[0] & mask) == (addr & mask))
		RETURN(1);
	    else
		RETURN(0);
            
        }
    else if(addr == 0U)
        RETURN(1);
    else
	RETURN(0);

    FINISH;
}

static int condition_ip_from_rbl(void *p, list_id lid)
{
    BEGIN("condition_ip_from_rbl");
    
#define MAX_QUERIES 41

    in_addr_t relay_addr = INADDR_NONE;
    in_addr_t localhost;
    size_t    received_ips_count = 0;
    size_t    i, number;
    struct    rbl_query queries[MAX_QUERIES];
    int rbl_result;

    inet_pton(AF_INET, "127.0.0.1", &localhost);
    memset(queries, 0, sizeof(queries));

    if(ENVELOPE->ip_addr.length > 0 && ENVELOPE->ip_addr.length < 50)
	{
	    char *temp = (char *)alloca(ENVELOPE->ip_addr.length+1);
	    memcpy(temp, ENVELOPE->ip_addr.text, ENVELOPE->ip_addr.length);
	    temp[ENVELOPE->ip_addr.length] = '\0';

            if(inet_pton(AF_INET, temp, &relay_addr) <= 0)
                relay_addr = INADDR_NONE;
        }

    number = 0;
    if(relay_addr != INADDR_NONE && relay_addr != localhost)
        queries[number++].addr   = relay_addr;

    collect_ips(p);

    received_ips_count = id->ic->ips.used;

    if(received_ips_count > MAX_QUERIES-number)
        received_ips_count = MAX_QUERIES-number;

    for(i = 0; i < received_ips_count; i++)
        {
            if(id->ic->ips.array[i] != localhost)
                {
                    queries[number].addr   = id->ic->ips.array[i];
                    queries[number].flags |= RBL_QUERY_RECEIVED;
                    number++;
                }
        }
 //vvt:    
    rbl_result = 0;//list_rbl_check(LIST_DB, lid, queries, number);

    if(rbl_result < 0)
        {
            ACTION->registers.values[0] = 0;
            RETURN(0);
        }
    else
        {
            ACTION->registers.values[0] = rbl_result;
            if(rbl_result >= 100)
                RETURN(1);
            else
                RETURN(0);
        }
    
#undef MAX_QUERIES

    FINISH;
}

static int condition_ip_from_not_in_dns(void *p)
{
    BEGIN("condition_ip_from_not_in_dns");

    collect_ips(p);

    if((!ENVELOPE->ip_addr.text || ENVELOPE->ip_addr.length == 0) && (id->ic->ips.first_line == 0  || id->ic->ips.used == 0))
	RETURN(0);

    if(!ENVELOPE->dns.checked)
	{
#ifdef USE_OLD_DNS_RESOLVER
	    struct hostent *he;
	    in_addr_t addr;
#endif
//	    char temp[32];

	    ENVELOPE->dns.exist = 0;
/*
	    if(ENVELOPE->ip_addr.length < sizeof(temp)-1)
		{
#ifdef USE_OLD_DNS_RESOLVER
#error This code is inconsistent with upper changes. 
		    strncpy(temp, ENVELOPE->ip_addr.text, sizeof(temp));
		    temp[sizeof(temp)-1] = '\0';
		    
		    addr = inet_addr(temp);

		    he = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
		    if(!he)
			  logger_printf(LOGGER_ERROR|LOGGER_VERBOSE_NORMAL, "DNS error: "/*%s.", hstrerror(h_errno));

		    if(he && he->h_name && he->h_name[0])
			ENVELOPE->dns.exist = 1;
#else
                    int status;
                    struct timeval interval;

                    if(INTERPRETER.resolver.inited == 0)
                        {
                            if((status = adc_create(&INTERPRETER.resolver.adc)))
                                {
                                    logger_printf(LOGGER_ERROR|LOGGER_VERBOSE_NORMAL, "Couldn't init resolver: %s.", adc_strerror(status));
                                    INTERPRETER.resolver.inited = -1;
                                }
                            else
                                INTERPRETER.resolver.inited = 1;
                        }
                    
                    if(INTERPRETER.resolver.inited > 0)
                        {
                            struct adc_request request[1];

                            if(ENVELOPE->ip_addr.text)
                                {
                                    strncpy(temp, ENVELOPE->ip_addr.text, sizeof(temp));
                                    temp[sizeof(temp)-1] = '\0';
                                    
                                    inet_pton(AF_INET, temp, &request[0].ip);
                                    request[0].type = ns_t_ptr;
                                }
                            else
                                {
                                    snprintf(temp, sizeof(temp), "%i.%i.%i.%i",
                                             id->ic->ips.array[0] & 0xff,
                                             (id->ic->ips.array[0] & 0xff00) >> 8,
                                             (id->ic->ips.array[0] & 0xff0000) >> 16,
                                             (id->ic->ips.array[0] & 0xff000000) >> 24);
                                    request[0].ip.s_addr = id->ic->ips.array[0];
                                    request[0].type = ns_t_ptr;
                                }
                                    
                            if(list_db_rbl_get_timeout(LIST_DB) > 1)
                                {
                                    interval.tv_sec = list_db_rbl_get_timeout(LIST_DB)/2;
                                    interval.tv_usec = 0;
                                }
                            else
                                {
                                    // half of second 
                                    interval.tv_sec = 0;
                                    interval.tv_usec = 5000000;
                                }

                            adc_resolve(&INTERPRETER.resolver.adc, request, 1, &interval);

                            if(adc_response_num(&INTERPRETER.resolver.adc))
                                {
                                    struct adc_response *resp = adc_get_response(&INTERPRETER.resolver.adc, 0);
                                    if(resp && resp->rq_index == 0 && resp->fqdn)
                                        {
                                            ENVELOPE->dns.exist = 1;
                                            logger_printf(LOGGER_DEBUG | LOGGER_VERBOSE_HIGH, "%s: ip %s resolved as %s.", id->ic->e->queue_id, temp, resp->fqdn);
                                        }
                                    else
                                        {
                                            logger_printf(LOGGER_DEBUG | LOGGER_VERBOSE_HIGH, "%s: ip %s dosen't resolved (illegal response).", id->ic->e->queue_id, temp);
                                        }

                                }
                            else
                                {
                                    logger_printf(LOGGER_DEBUG | LOGGER_VERBOSE_HIGH, "%s: ip %s dosen't resolved.", id->ic->e->queue_id, temp);
                                }
                        }

#endif

		}*/

	    ENVELOPE->dns.checked = 1;
	}

    RETURN(!ENVELOPE->dns.exist);

    FINISH;
}

static int condition_has_attachments(void *p)
{
    BEGIN("condition_has_attachments");

#ifdef CONFIDENTIAL

    unsigned long i;

    for(i = 0; i < MESSAGE.attachments_count; i++)
        {
            if(!(MESSAGE.attachments[i].flags & MSG_ALTERNATIVE))
                RETURN(1);
        }

#endif /* CONFIDENTIAL */

    RETURN(0);

    FINISH;
}

static int condition_unknown_content(void *p)
{
    BEGIN("condition_unknown_content");

#ifdef CONFIDENTIAL


    if(id->ic->filter_called)
        {
            RETURN(id->ic->unknown_parts);
        }
    else
        {
            unsigned long i;

            for(i = 0; i < MESSAGE.attachments_count; i++)
                {
                    if(!MESSAGE.attachments[i].reserved.r1 && 
                       !(MESSAGE.attachments[i].flags & MSG_ATTACHMENT_EXTRACTED) &&
                        MESSAGE.attachments[i].count)
                        RETURN(1);
                }
        }

#endif /* CONFIDENTIAL */

    RETURN(0);

    FINISH;
}

static int condition_check_register(void *p, unsigned int number, unsigned int type, int value)
{
    BEGIN("condition_check_register");

    logger_printf(LOGGER_INFO, "check_register: number=%u, type=%u, value=%i, rvalue=%i", number, type, value, ACTION->registers.values[number]); 

    if(number >= MAX_REGISTERS)
	RETURN(0);

    switch(type)
	{
	case CONDITION_REGISTER_EQUAL:

	    if(ACTION->registers.values[number] == value)
		RETURN(1);

	    break;

	case CONDITION_REGISTER_LESS:

	    if(ACTION->registers.values[number] < value)
		RETURN(1);

	    break;

	case CONDITION_REGISTER_MORE:

	    if(ACTION->registers.values[number] > value)
		RETURN(1);

	    break;
	}

    RETURN(0);

    FINISH;
}

static int call_bayes(void *p)
{
    BEGIN("call_bayes");

    //At the startup BAYES_RATE == -1
    if(BAYES_RATE == -1)
    {
        //TODO: Add call to Bayes classification routine here (should return value 0..100 into BAYES_RATE - cache it!)
        BAYES_RATE = 50;
        logger_printf(LOGGER_DEBUG,"Bayes rate: %u", BAYES_RATE);
    }
    RETURN(0);
    FINISH;
}

static int condition_bayes_match(void *p, unsigned int rate)
{
    BEGIN("condition_bayes_match");

    call_bayes(p);

	if(BAYES_RATE >= rate)
	    RETURN(1);

    RETURN(0);
    
    FINISH;
}

/*
 * Make actions
 */

#define NEED_TO_FINISH				\
do {						\
    id->need_to_finish = 1;			\
    RETURN(1);					\
} while(0);

static int action_accept(void *p)
{
    BEGIN("action_accept");
    ACTION_SET_TYPE(ACTION, ACTION_ACCEPT);
    NEED_TO_FINISH;
    FINISH;
}

static int action_blackhole(void *p)
{
    BEGIN("action_blackhole");
    ACTION_SET_TYPE(ACTION, ACTION_BLACKHOLE);
    NEED_TO_FINISH;
    FINISH;
}

static int action_reject(void *p)
{
    BEGIN("action_reject");

    if(profile_get_type(PROFILE) == PROFILE_COMMON)
	{
	    ACTION_SET_TYPE(ACTION, ACTION_REJECT);
	}
    else
	{
	    ACTION_SET_TYPE(ACTION, ACTION_BLACKHOLE);
	    ACTION_SET_BOUNCE(ACTION_STORAGE, ACTION);
	}

    NEED_TO_FINISH;
    FINISH;
}

static int action_skip(void *p)
{
    BEGIN("action_skip");
    RETURN(1);
    FINISH;
}

static int action_bounce(void *p)
{
    BEGIN("action_bounce");
    ACTION_SET_BOUNCE(ACTION_STORAGE, ACTION);
    RETURN(0);
    FINISH;
}

static int action_send(void *p, unsigned int type, size_t count, const char *rcpts)
{
    const char *ptr;
    const char *dog_ptr;
    size_t      i, k;
    int         r = -1;
    BEGIN("action_send");

    if(type == ACTION_SEND_NEW || type == ACTION_SEND_CHANGE)
	ACTION_RCPT_ZERO(ACTION);

    if(type == ACTION_SEND_DELETE)
	{
	    for(i = 0; i < ACTION_STORAGE->rcpts.used; i++)
		{
		    if(ACTION_RCPT_CHECK(ACTION, i))
			{
			    ptr = rcpts;
			    for(k = 0; k < count; k++)
				{
				    if(*ptr == '@')
					{
					    dog_ptr = (const char *)memchr(ACTION_STORAGE->rcpts.array[i].rcpt, '@', 
									   ACTION_STORAGE->rcpts.array[i].length);

					    if(dog_ptr && strlen(ptr) == ACTION_STORAGE->rcpts.array[i].length - 
					       (dog_ptr - ACTION_STORAGE->rcpts.array[i].rcpt))
						{
						    if(strncasecmp(dog_ptr+1, ptr+1, 
								   ACTION_STORAGE->rcpts.array[i].length - 
								   (dog_ptr - ACTION_STORAGE->rcpts.array[i].rcpt)) == 0)
							ACTION_RCPT_CLEAR(ACTION, i);
						}
					}
				    else
					{
					    if(ACTION_STORAGE->rcpts.array[i].length == strlen(ptr))
						{
						    if(strncasecmp(ACTION_STORAGE->rcpts.array[i].rcpt, 
								   ptr, ACTION_STORAGE->rcpts.array[i].length) == 0)
							ACTION_RCPT_CLEAR(ACTION, i);
						}
					}

				    ptr = strchr(ptr, '\0') + 1;
				}

			}
		}

	}
    else
	{
	    ptr = rcpts;
	    for(i = 0; i < count; i++)
		{
		    if(strcmp(ptr, "${SMTP_FROM}") == 0)
			{
			    if(ENVELOPE->from.length)
				r = action_storage_rcpt_add(ACTION_STORAGE, ENVELOPE->from.text, ENVELOPE->from.length);
			}
		    else
			r = action_storage_rcpt_add(ACTION_STORAGE, ptr, strlen(ptr));
			
		    ptr = strchr(ptr, '\0') + 1;
		    
		    if(r > 0)
			{
			    if(ACTION_RCPT_MAX(ACTION) > r)
				{
				    ACTION = action_storage_action_realloc(ACTION_STORAGE, ACTION, r);
				}
			    
			    ACTION_RCPT_SET(ACTION, r);
			}
		}
	}

    RETURN(0);

    FINISH;
}

static int action_header_change(void *p, const char *header, const char *value)
{
    BEGIN2("action_header_change",header,value);
    action_header_modify(ACTION, HEADER_MODIFY_REMOVE, header, "");
    action_header_modify(ACTION, HEADER_MODIFY_NEW, header, value);

    /* action_header_modify(ACTION, HEADER_MODIFY_CHANGE, header, value); */
    RETURN(0);
    FINISH;
}

static int action_header_add(void *p, const char *header, const char *value)
{
    BEGIN2("action_header_add",header,value);
    action_header_modify(ACTION, HEADER_MODIFY_ADD, header, value);
    RETURN(0);
    FINISH;
}

static int action_header_prepend(void *p, const char *header, const char *value)
{
    BEGIN2("action_header_prepend",header,value);
    action_header_modify(ACTION, HEADER_MODIFY_PREPEND, header, value);
    RETURN(0);
    FINISH;
}

static int action_header_new(void *p, const char *header, const char *value)
{
    BEGIN2("action_header_new",header,value);
    action_header_modify(ACTION, HEADER_MODIFY_NEW, header, value);
    RETURN(0);
    FINISH;
}

static int action_header_remove(void *p, const char *header)
{
    BEGIN1("action_header_remove",header);
    action_header_modify(ACTION, HEADER_MODIFY_REMOVE, header, "");
    RETURN(0);
    FINISH;
}

static int action_register_set(void *p, unsigned int number, int value)
{
    
    BEGIN("action_register_set");

    if(number < MAX_REGISTERS)
	{
	    ACTION->registers.values[number] = value;
	}

    
    RETURN(0);
    FINISH;
}

static int action_register_add(void *p, unsigned int number, int value)
{
    BEGIN("action_register_add");

    if(number < MAX_REGISTERS)
	{
	    ACTION->registers.values[number] += value;
	}
    
    RETURN(0);
    FINISH;
}

#ifdef INFORMER
static int prepare_message(void *p, iqmgr_message *imsg, unsigned int type, const char *notification = NULL)
{
    BEGIN("prepare_message");

    libmflt_msg_header hdr;
    
    memset(&hdr, 0, sizeof(hdr));

    hdr.type = type; 
    hdr.rule_number = id->rule_number;

    libmflt_message_add_header(imsg, &hdr);
    libmflt_message_add_profile_name(imsg, profile_get_name(PROFILE), -1);

    libmflt_message_add_sender(imsg, ENVELOPE->from.text, ENVELOPE->from.length);

    for(int i = 0; i < ENVELOPE->rcpts.used; i++)
	libmflt_message_add_rcpt(imsg, ENVELOPE->rcpts.array[i].email.text, ENVELOPE->rcpts.array[i].email.length);

    if(!id->ic->message_demimed)
	{
	    memset(&(id->ic->msg), 0, sizeof(id->ic->msg));
	    
	    message_create_mime(&(id->ic->msg), ENVELOPE->data.text, ENVELOPE->data.length);
	    id->ic->message_demimed = 1;
	}

    if(id->ic->msg.subject.count > 0)
	libmflt_message_add_subject(imsg, id->ic->msg.subject.ptr, id->ic->msg.subject.count);

    for(int i = 0; i < id->ic->msg.attachments_count; i++)
	{
	    if(id->ic->msg.attachments[i].filename.count)
		libmflt_message_add_attachment(imsg, id->ic->msg.attachments[i].filename.ptr, 
					       id->ic->msg.attachments[i].filename.count);
	}


    if(notification)
	libmflt_message_add_notification(imsg, notification, -1);

    RETURN(0);
    FINISH;
}
#endif

static int action_send_notification(void *p, const char *entry)
{
    BEGIN1("action_send_notification", entry);

#ifdef INFORMER

    if(IQMGR_CONN)
	{
	    iqmgr_message  imsg;
	    iqmgr_request  req;
	    iqmgr_response resp;

	    libmflt_message_init(&imsg, 10);
	    prepare_message(p, &imsg, 2, entry);
	    libmflt_message_close(&imsg);

	    req.type = REQUEST_ADD;

	    if(iqmgr_connection_send_request(IQMGR_CONN, &req))
		{
		    filter_module_iqmgr_connection_close();
		    RETURN(0);
		}

	    if(iqmgr_connection_read_response(IQMGR_CONN, &resp))
		{
		    filter_module_iqmgr_connection_close();
		    RETURN(0);
		}

	    if(resp.type == RESPONSE_OK)
		{
		    if(iqmgr_connection_send_message(IQMGR_CONN, &imsg))
			{
			    filter_module_iqmgr_connection_close();
			    RETURN(0);
			}
		}
	}

#endif
    
    RETURN(0);
    FINISH;
}

static int action_send_log_entry(void *p, const char *entry)
{
    BEGIN1("action_send_log_entry",entry);

#ifdef INFORMER

    if(IQMGR_CONN)
	{
	    iqmgr_message  imsg;
	    iqmgr_request  req;
	    iqmgr_response resp;

	    libmflt_message_init(&imsg, 10);
	    prepare_message(p, &imsg, 1, entry);
	    libmflt_message_close(&imsg);

	    req.type = REQUEST_ADD;

	    if(iqmgr_connection_send_request(IQMGR_CONN, &req))
		{
		    filter_module_iqmgr_connection_close();
		    RETURN(0);
		}

	    if(iqmgr_connection_read_response(IQMGR_CONN, &resp))
		{
		    filter_module_iqmgr_connection_close();
		    RETURN(0);
		}

	    if(resp.type == RESPONSE_OK)
		{
		    if(iqmgr_connection_send_message(IQMGR_CONN, &imsg))
			{
			    filter_module_iqmgr_connection_close();
			    RETURN(0);
			}
		}
	}

#endif

    RETURN(0);
    FINISH;
}

typedef struct iterate_data {
    size_t used;
    size_t width;
#ifdef _WIN32
	interpreter *ib;
#endif //_WIN32
} iterate_data;

#define REALLOC_BUF(sz)															\
    do {																\
	if((sz) > INTERPRETER.buf.alloc)												\
	    {																\
		if(INTERPRETER.buf.array)												\
		    {															\
			INTERPRETER.buf.array = (char *)realloc(INTERPRETER.buf.array, INTERPRETER.buf.alloc = ((sz)/1024 + 5)*1024 );	\
		    }															\
		else															\
		    {															\
			INTERPRETER.buf.array = (char *)malloc(INTERPRETER.buf.alloc = ((sz)/1024 + 5)*1024);				\
		    }															\
	    }																\
    } while(0)


static int envelope_headers_iterate(void *p, const char *value)
{
    int len = strlen(value);
    iterate_data *id = (iterate_data *)p;
#ifdef _WIN32
    #undef INTERPRETER
	#define INTERPRETER    (*(id->ib))
#endif //_WIN32
    if(len > id->width) 
	len = id->width;

    if(len)
	{
	    REALLOC_BUF(id->used + len);

	    memcpy(INTERPRETER.buf.array + id->used, value, len);
	    id->used += len;
	    id->width -= len;

	    return 0;
	}
    else
	return 1;
}

#ifdef _WIN32
#undef INTERPRETER
#define INTERPRETER    (id->ib)
#endif // _WIN32

static int action_syslog(void *p, unsigned int mask, const char *message)
{
    BEGIN1("action_syslog",message);

    const char *ptr = message;
    const char *var_begin = NULL;
    int var_chr = 0, all_headers = 0, new_headers = 0;
    size_t used = 0, i, j;
    size_t width = 0;

    enum {
	NORMAL,
	VAR_BEGIN,
	VAR_HDR_CHAR,
	VAR_HDR
    } state = NORMAL;

    for( ; *ptr; ptr++)
	{
	    switch(state)
		{
		case NORMAL:

		    if(*ptr == '%')
			{
			    state = VAR_BEGIN;
			    width = 0;
			}
		    else
			{
			    REALLOC_BUF(used+1);
			    INTERPRETER.buf.array[used++] = *ptr;
			    
			}

		    break;

		case VAR_BEGIN:

		    switch(*ptr)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':

			    width = width*10 + (*ptr - '0');
			    
			    break;

			case '%':

			    /* 
			     * % symbol
			     */

			    REALLOC_BUF(used+1);
			    INTERPRETER.buf.array[used++] = *ptr;
			    state = NORMAL;
			    break;


			case 'i':
			    
			    /*
			     * IP address
			     */
			    
			    if(ENVELOPE->ip_addr.text && ENVELOPE->ip_addr.length)
				{
				    if(width)
					width = width > ENVELOPE->ip_addr.length ? ENVELOPE->ip_addr.length : width;
				    else
					width = ENVELOPE->ip_addr.length;

				    REALLOC_BUF(used + width);

				    memcpy(INTERPRETER.buf.array + used, ENVELOPE->ip_addr.text, width);
				    used += width;
				}

			    state = NORMAL;
			    break;

			case 'f':

			    /*
			     * Envelope from
			     */

			    if(ENVELOPE->from.text && ENVELOPE->from.length)
				{
				    if(width)
					width = width > ENVELOPE->from.length ? ENVELOPE->from.length : width;
				    else
					width = ENVELOPE->from.length;

				    REALLOC_BUF(used + width);

				    memcpy(INTERPRETER.buf.array + used, ENVELOPE->from.text, width);
				    used += width;
				}

			    state = NORMAL;
			    break;

			case 'r':
			case 'R':

			    /*
			     * Recipients; 'r' --- matched to this profile;
			     * 'R' --- all recipients of envelope.
			     */

			    j = 0;

			    if(width == 0)
				width = 20480; /* 20KB is enough */

			    for(i = 0; i < ENVELOPE->rcpts.used; i++)
				{
				    if(*ptr =='R' || ACTION_RCPT_CHECK(ACTION, i))
					{
					    REALLOC_BUF(used + ENVELOPE->rcpts.array[i].email.length + 1);
					    if(j && width)
						{
						    INTERPRETER.buf.array[used++] = ';';
						    width--;
						}

					    int l = width > ENVELOPE->rcpts.array[i].email.length ? ENVELOPE->rcpts.array[i].email.length : width;
					    memcpy(INTERPRETER.buf.array + used, ENVELOPE->rcpts.array[i].email.text, l);
					    used += l;
					    width -= l;

					    if(width == 0)
						break;

					    j++;
					}
				}

			    state = NORMAL;
			    break;

			case 'c':
                        {
#ifdef KIS_USAGE
                            break;
#else
                            int first_category = 1;
                            int length = 0;

                            call_filter(p);

                            if(FILTER_RESULT && FILTER_RESULT->error_code == FR_OK)
                                {
                                    for(size_t i = 0; i < FILTER_RESULT->used; i++)
                                        {
                                            
                                            const category *cur_cat = g_CFilterDll.RubSearch(FILTER_RESULT->res[i].CID);
						
                                            if(!cur_cat)
                                                continue;

                                            for(size_t k = 0; k < cur_cat->sub.count; k++)
                                                {
                                                    for(size_t j = 0; j < FILTER_RESULT->used; j++)
                                                        {
                                                            if(cur_cat->sub.categories[k]->CID == FILTER_RESULT->res[j].CID)
                                                                goto skip_category;
                                                        }
                                                }

                                            length = strlen(cur_cat->title);
                                            
                                            REALLOC_BUF(used + 1 + length);
						
                                            if(!first_category)
                                                INTERPRETER.buf.array[used++] = ';';
                                            
                                            memcpy(INTERPRETER.buf.array+used, cur_cat->title, length);
                                            
                                            used += length;
                                            first_category = 0;
                                            
                                          skip_category: ;
                                        }
                                }

			    state = NORMAL;
			    break;
#endif//KIS_USAGE
                        }

			case 'h':
			case 'H':

			    /*
			     * Headers.
			     * 'h' --- first header
			     * 'H' --- all headers
			     * For example, 
			     * %h|subject|
			     */

			    all_headers = (*ptr == 'H');
                            new_headers = 0;

			    state = VAR_HDR_CHAR;

			    break;

                        case 'n':

                            /* New headers */

                            new_headers = 1;
			    state = VAR_HDR_CHAR;

                            break;

#ifdef CONFIDENTIAL
                        case 'a':

                            /* Attachment names */

			    j = 0;

			    if(width == 0)
				width = 20480; /* 20KB is enough */

			    for(i = 0; i < MESSAGE.attachments_count; i++)
				{
                                    if(MESSAGE.attachments[i].filename.ptr == NULL)
                                        continue;

                                    REALLOC_BUF(used + MESSAGE.attachments[i].filename.count + 1);

                                    if(j && width)
                                        {
                                            INTERPRETER.buf.array[used++] = ';';
                                            width--;
                                        }

                                    int l = width > MESSAGE.attachments[i].filename.count ? 
                                        MESSAGE.attachments[i].filename.count : width;
                                    memcpy(INTERPRETER.buf.array + used, MESSAGE.attachments[i].filename.ptr, l);
                                    used += l;
                                    width -= l;
                                    
                                    if(width == 0)
                                        break;

                                    j++;
				}

			    state = NORMAL;
			    break;

                        case 'A':

                            /* Attachments types */

			    j = 0;

			    if(width == 0)
				width = 20480; /* 20KB is enough */

			    for(i = 0; i < MESSAGE.attachments_count; i++)
				{
                                    if(MESSAGE.attachments[i].content_type.ptr == NULL)
                                        continue;

                                    REALLOC_BUF(used + MESSAGE.attachments[i].content_type.count + 1);

                                    if(j && width)
                                        {
                                            INTERPRETER.buf.array[used++] = ';';
                                            width--;
                                        }

                                    int l = width > MESSAGE.attachments[i].content_type.count ? 
                                        MESSAGE.attachments[i].content_type.count : width;
                                    memcpy(INTERPRETER.buf.array + used, MESSAGE.attachments[i].content_type.ptr, l);
                                    used += l;
                                    width -= l;
                                    
                                    if(width == 0)
                                        break;

                                    j++;
				}

			    state = NORMAL;
			    break;

                        case 's':
                        {
                            char ch = (MESSAGE.flags & MESSAGE_FLAG_CRYPTED) ? '+' : '-';
                            REALLOC_BUF(used+1);

                            memcpy(INTERPRETER.buf.array + used, &ch, 1);

                            used++;
                            state = NORMAL;
                            break;
                        }

                        case 't':
                        {
                            if(MESSAGE.subject.count)
                                {
                                    REALLOC_BUF(used+MESSAGE.subject.count);

                                    memcpy(INTERPRETER.buf.array + used, MESSAGE.subject.ptr, MESSAGE.subject.count);

                                    used += MESSAGE.subject.count;
                                }

                            state = NORMAL;
                            break;
                        }

#endif /* CONFIDENTIAL */

			default:
			    logger_printf(LOGGER_WARNING, "Unknown char \'%c\' in message template \"%s\".", *ptr, message);

			}

		    break;

		case VAR_HDR_CHAR:

		    var_chr = *ptr;
		    state = VAR_HDR;
		    var_begin = NULL; 
		    
		    break;

		case VAR_HDR:
		    
		    if(*ptr == var_chr)
			{
			    char *hdr_name = (char *)malloc(ptr - var_begin + 1);

			    memcpy(hdr_name, var_begin, ptr - var_begin);
			    hdr_name[ptr - var_begin] = '\0';

			    if(!width)
				width = 20480;

                            if(new_headers)
                                {
                                    size_t i, first = ACTION->headers.used+1, last = 0;

                                    for(i = 0; i < ACTION->headers.used; i++)
                                        {
                                            if(strcasecmp(hdr_name, ACTION->headers.array[i].header) == 0)
                                                {
                                                    if(ACTION->headers.array[i].type == HEADER_MODIFY_ADD || 
                                                       ACTION->headers.array[i].type == HEADER_MODIFY_CHANGE ||
                                                       ACTION->headers.array[i].type == HEADER_MODIFY_PREPEND || 
                                                       ACTION->headers.array[i].type == HEADER_MODIFY_NEW)
                                                        {
                                                            if(first >= ACTION->headers.used)
                                                                first = i;

                                                            last = i;
                                                        }
                                                    else if(ACTION->headers.array[i].type == HEADER_MODIFY_REMOVE)
                                                        {
                                                            first = ACTION->headers.used+1;
                                                        }
                                                }
                                        }

                                    for(i = first; i < ACTION->headers.used; i++)
                                        {
                                            if(strcasecmp(hdr_name, ACTION->headers.array[i].header) == 0)
                                                {
                                                    unsigned int sz = width;
                                                    if(sz > strlen(ACTION->headers.array[i].value))
                                                        sz = strlen(ACTION->headers.array[i].value);

                                                    REALLOC_BUF(used + sz);

#ifdef CONFIDENTIAL
                                                    unsigned int j;

                                                    for(j = 0; j < sz; j++)
                                                        {
                                                            if(ACTION->headers.array[i].value[j] == '\"')
                                                                {
                                                                    if(width > 0)
                                                                        width--;
                                                                    REALLOC_BUF(used+1);

                                                                    INTERPRETER.buf.array[used++] = '\\';
                                                                }

                                                            if(width > 0)
                                                                width--;
                                                            REALLOC_BUF(used+1);

                                                            INTERPRETER.buf.array[used++] = ACTION->headers.array[i].value[j];
                                                        }
                                                        
#else
                                                    memcpy(INTERPRETER.buf.array + used, ACTION->headers.array[i].value, sz);
                                                    used += sz;
                                                    width -= sz;
#endif

                                                    if(width == 0)
                                                        break;
                                                }

                                            if(i >= last)
                                                break;
                                        }
                                    
                                }
                            else
                                {
                                    if(all_headers)
                                        {
                                            iterate_data itd;
                                            
                                            memset(&itd, 0, sizeof(id));
                                            
                                            itd.width = width;
                                            itd.used = used;
#ifdef _WIN32
											itd.ib = &INTERPRETER;
#endif //_WIN32
                                            envelope_search_header_iterate(ENVELOPE, hdr_name, envelope_headers_iterate, &itd);
                                            
                                            used = itd.used;
                                        }
                                    else
                                        {
                                            int hdr_index = envelope_search_header_iterate(ENVELOPE, hdr_name, NULL, NULL);
                                            
                                            if(hdr_index >= 0)
                                                {
                                                    int len = strlen(ENVELOPE->heap.array + ENVELOPE->headers.array[hdr_index].value);
                                                    width = width > len ? len : width;
                                                    
#ifdef CONFIDENTIAL
                                                    unsigned int j;
                                                    const char *str = ENVELOPE->heap.array + ENVELOPE->headers.array[hdr_index].value;

                                                    for(j = 0; j < width; j++)
                                                        {
                                                            if(str[j] == '\"')
                                                                {
                                                                    REALLOC_BUF(used+1);

                                                                    INTERPRETER.buf.array[used++] = '\\';
                                                                }

                                                            REALLOC_BUF(used+1);

                                                            INTERPRETER.buf.array[used++] = str[j];
                                                        }
                                                        
#else
                                                    memcpy(INTERPRETER.buf.array + used, ENVELOPE->heap.array + ENVELOPE->headers.array[hdr_index].value, width);
                                                    used += width;
#endif
                                                }
                                        }
                                }

			    free(hdr_name);

                            state = NORMAL;
			}
		    else
			{
			    if(!var_begin)
				var_begin = ptr;
			}

		    break;
		}
	}

    REALLOC_BUF(used+1);
    INTERPRETER.buf.array[used++] = '\0';

    logger_printf(mask | LOGGER_USER, "%s", INTERPRETER.buf.array);

    RETURN(0);
    FINISH;

#undef REALLOC_BUF

}


/*
 * Begin of rule
 */
static int rule_begin(void *p)
{
    BEGIN("rule_begin");

    id->rule_number++;
#ifdef WITH_LOGDEBUG_LEV2
    logger_printf(LOGGER_DEBUG, "Rule (%d):", id->rule_number);
#endif //WITH_LOGDEBUG_LEV2
    RETURN(0);
    FINISH;
}


/*
 * Interpreter (callbacks to interate functions of corresponding modules)
 */

static int profile_db_callback(void *user_data, profile *p)
{
    interpreter_data *id = (interpreter_data *)user_data;

    id->cur_profile = p;

    id->rule_number = 0;

    if(profile_get_type(p) == PROFILE_COMMON)
	{
	    logger_printf(LOGGER_DEBUG , "Processing common profile: %s", p->name);

	    profile_iterate(p, &(id->pic), id);
	    return id->need_to_finish;
	}
    else 
	{
	    action *new_action = NULL;

	    logger_printf(LOGGER_DEBUG, "Processing personal profile: %s", p->name);

	    for(size_t i = 0; i < ACTION_STORAGE->rcpts.used; i++)
		{
		    if(ACTION_RCPT_CHECK(ACTION_STORAGE->common_action, i))
			{
			    
			    if(profile_check_rcpt(p, LIST_DB, ACTION_STORAGE->rcpts.array[i].rcpt, 
						  ACTION_STORAGE->rcpts.array[i].length))
				{
				    logger_printf(LOGGER_INFO | LOGGER_VERBOSE_HIGH, "User %s fit to this profile.", ACTION_STORAGE->rcpts.array[i].rcpt);

				    if(!new_action)
					new_action = action_storage_action_clone(ACTION_STORAGE, ACTION_STORAGE->common_action);

				    ACTION_RCPT_CLEAR(ACTION_STORAGE->common_action, i);
				    ACTION_RCPT_SET(new_action, i);
				}
			}
		}

	    if(new_action)
		{
		    id->cur_action = new_action;
		    profile_iterate(p, &(id->pic), id);
		}

	    return 0;
	}
}
#ifndef _WIN32
static int interpreter_init()
{
#else
static int interpreter_init(void *p)
{
	 interpreter_data *id  = (interpreter_data *)p;
#endif //_WIN32
    memset(&INTERPRETER, 0, sizeof(INTERPRETER));

    INTERPRETER.buf.array = (char *)malloc(INTERPRETER.buf.alloc = 4096);
#ifndef _WIN32
    interpreter_inited = 1;
#endif //_WIN32
    return 0;
}

int profile_interpretate(interpreter_config *ic, bool skipFormal)
{
    int res;
#ifndef _WIN32
    if(!interpreter_inited)
	{
	    interpreter_init();
	}
#endif //_WIN32
    interpreter_data id;
    memset(&id, 0, sizeof(id));
#ifdef _WIN32
	interpreter_init(&id);
#endif //_WIN32

    id.ic              = ic;
    ic->filter_called  = 0;
    ic->ips.used       = 0;
    ic->ips.parsed     = 0;
    ic->ips.first_line = 0;
    ic->bayes_rate     = -1;

    id.pic.condition.smtp_from          = condition_smtp_from;
    id.pic.condition.smtp_from_list     = condition_smtp_from_list;
    id.pic.condition.smtp_to            = condition_smtp_to;
    id.pic.condition.smtp_to_list       = condition_smtp_to_list;
    id.pic.condition.smtp_from          = condition_smtp_from;
    id.pic.condition.has_header         = condition_has_header;
    id.pic.condition.header_match       = condition_header_match;
    id.pic.condition.category_match     = condition_category_match;
    id.pic.condition.msg_size           = condition_msg_size;
    id.pic.condition.ip_from_list       = condition_ip_from_list;
    id.pic.condition.ip_from            = condition_ip_from;
    id.pic.condition.ip_from_rbl        = condition_ip_from_rbl;
    id.pic.condition.ip_from_not_in_dns = condition_ip_from_not_in_dns;
    id.pic.condition.check_register     = condition_check_register;
    id.pic.condition.attribute_match    = condition_attribute_match;
    id.pic.condition.content_type_match = condition_content_type_match;
    id.pic.condition.file_name_match    = condition_file_name_match;
    id.pic.condition.has_attachments    = condition_has_attachments;
    id.pic.condition.unknown_content    = condition_unknown_content;
    id.pic.condition.bayes_match        = condition_bayes_match;

    id.pic.action.accept                = action_accept;
    id.pic.action.reject                = action_reject;
    id.pic.action.bounce                = action_bounce;
    id.pic.action.blackhole             = action_blackhole;
    id.pic.action.send                  = action_send;
    id.pic.action.header_change         = action_header_change;
    id.pic.action.header_add            = action_header_add;
    id.pic.action.header_prepend        = action_header_prepend;
    id.pic.action.header_new            = action_header_new;
    id.pic.action.header_remove         = action_header_remove;
    id.pic.action.skip                  = action_skip;
    id.pic.action.register_set          = action_register_set;
    id.pic.action.register_add          = action_register_add;
    id.pic.action.send_notification     = action_send_notification;
    id.pic.action.send_log_entry        = action_send_log_entry;
    id.pic.action.syslog                = action_syslog;

    id.pic.rule_begin                   = rule_begin;

    id.cur_action                       = ic->as->common_action;

#if defined(INFORMER) || defined(CONFIDENTIAL)

    ic->message_demimed = 0;

    if(!ic->message_demimed)
	{
            /* XXX */

	    memset(&(ic->msg), 0, sizeof(ic->msg));
	    
	    message_create_mime(&(ic->msg), ic->e->data.text, ic->e->data.length);
	    ic->message_demimed = 1;

#ifdef MESSAGE_FLAG_CRYPTED

            if(ic->msg.flags & MESSAGE_FLAG_CRYPTED)
                envelope_add_attr(ic->e, "crypted", "true");

#endif /* MESSAGE_FLAG_CRYPTED */
	}

#endif

#ifdef WITH_SCORES
    ic->tricks_score = 0;
#endif

    res = profile_db_iterate(ic->pdb, profile_db_callback, &id, skipFormal);

#if defined(INFORMER) || defined(CONFIDENTIAL)
    if(ic->message_demimed)
        {
            g_CFilterDll.MessageDestroy(&(ic->msg));
            ic->message_demimed = 0;
        }
#endif

    // Free ADC if need
#ifdef USE_RBL
#ifdef _WIN32
    if( id.ib.resolver.inited )
    {
        adc_destroy(&id.ib.resolver.adc);
        id.ib.resolver.inited = 0;
    }
#endif
#endif

	
	//Free buffers
	if(id.ib.buf.array)
	{
		free(id.ib.buf.array);
		id.ib.buf.alloc = 0;
	}

    return res;
}

/*
 * <eof interpreter.cpp>
 */
