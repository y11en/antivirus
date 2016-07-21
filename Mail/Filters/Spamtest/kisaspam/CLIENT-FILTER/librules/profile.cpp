/*******************************************************************************
 * Proj: SMTP Filtration                                                       *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: profile.cpp                                                           *
 * Created: Wed Apr 24 19:24:37 2002                                           *
 * Desc: Profile in/out helpers                                                *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  profile.cpp
 * \brief Profile in/out helpers.
 */
#include "commhdr.h"
#pragma hdrstop

#ifdef _WIN32
#include <_include/nix_types.h>
#endif // _WIN32

#include <string.h>

#ifndef _WIN32
#include <sys/param.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif // _WIN32

#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32
#include <regex.h>
#else 
#include <gnu/regex/regex.h>
#endif // _WIN32

#ifndef _WIN32
#include <sys/param.h>
#include <netinet/in.h>
#endif //_WIN32

#include <stdlib.h>
#include "gnu/_db1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <syslog.h>
#else
#include <winsock.h>
#include <malloc.h>
#endif // WIN32

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif

#include "profile.h"
#include <smtp-filter/common/logger.h>

#include <smtp-filter/common/list_db.h>

#ifdef _MSC_VER
#pragma warning (disable : 4127)
// sgladkov : disable warnings for "conditional expression is constant" for "do { ... } while(0)" cicles
#endif // _MSC_VER

/*******************************************************************************
 *
 * Data structures
 *
 *******************************************************************************/


/*
 * Conditions.
 */

#define CONDITION_NONE                 0
#define CONDITION_SMTP_FROM            1
#define CONDITION_SMTP_FROM_LIST       2
#define CONDITION_SMTP_TO              3
#define CONDITION_SMTP_TO_LIST         4
#define CONDITION_HAS_HEADER           5
#define CONDITION_HEADER_MATCH         6
#define CONDITION_CATEGORY_MATCH       7
#define CONDITION_MSG_SIZE             8
#define CONDITION_IP_FROM              9
#define CONDITION_IP_FROM_LIST         10
#define CONDITION_IP_FROM_RBL          11
#define CONDITION_IP_FROM_NOT_IN_DNS   12
#define CONDITION_REGISTER             13
#define CONDITION_ATTRIBUTE_MATCH      14
#define CONDITION_CONTENT_TYPE_MATCH   15
#define CONDITION_FILE_NAME_MATCH      16
#define CONDITION_HAS_ATTACHMENTS      17
#define CONDITION_UNKNOWN_CONTENT      18
#define CONDITION_VIRUS_MATCH          19
#define CONDITION_DCC_MATCH            20
#define CONDITION_BAYES_MATCH          21 

typedef unsigned int cond_type;

struct condition_struct_header
{
    cond_type    type;
    unsigned int flags;
    size_t       size;  /* Real size of remainder of real object */
};

struct condition 
{
    struct condition_struct_header hdr;

    char      data[1];  /* Can be much longer for different types */
};

struct condition_smtp_from
{
    struct condition_struct_header hdr;

    char      email[1];
};

struct condition_smtp_from_list
{
    struct condition_struct_header hdr;

    list_id   lid;
};

struct condition_smtp_to
{
    struct condition_struct_header hdr;

    char      email[1];
};

struct condition_smtp_to_list
{
    struct condition_struct_header hdr;

    list_id   lid;
};

struct condition_has_header
{
    struct condition_struct_header hdr;

    char      header[1];
};

struct condition_header_match
{
    struct condition_struct_header hdr;

    regex_t   regexp;

    char      header_regexp[2]; /* header zero regexp zero */
};

struct condition_category_match
{
    struct condition_struct_header hdr;

    unsigned int cat_id;
};

struct condition_msg_size
{
    struct condition_struct_header hdr;

    unsigned int limit;
};

struct condition_ip_from
{
    struct condition_struct_header hdr;
    
    in_addr_t addr;
    in_addr_t mask;
};

struct condition_ip_from_list
{
    struct condition_struct_header hdr;

    list_id   lid;
};

struct condition_ip_from_rbl
{
    struct condition_struct_header hdr;
    
    list_id   lid;
};

struct condition_ip_from_not_in_dns
{
    struct condition_struct_header hdr;
};

struct condition_register
{
    struct condition_struct_header hdr;

    unsigned int number;
    unsigned int type;
    int          quantity;
};

struct condition_attribute_match
{
    struct condition_struct_header hdr;

    regex_t   regexp;

    char      attribute_regexp[2]; /* attribute zero regexp zero */
};

struct condition_content_type_match
{
    struct condition_struct_header hdr;

    regex_t   regexp;

    char      str_regexp[1];
};

struct condition_file_name_match
{
    struct condition_struct_header hdr;

    regex_t   regexp;

    char      str_regexp[1];
};

struct condition_has_attachments
{
    struct condition_struct_header hdr;
};

struct condition_unknown_content
{
    struct condition_struct_header hdr;
};

struct condition_virus_match
{
    struct condition_struct_header hdr;

    regex_t regexp;
    char    str_regexp[1];
};

struct condition_dcc_match
{
    struct condition_struct_header hdr;

    char      body_fuz1_fuz2[3]; /* body zero fuz1 zero fuz2 zero */
};

struct condition_bayes_match
{
    struct condition_struct_header hdr;

    unsigned int rate;
};


/*
 * Actions
 */

typedef unsigned int action_type;

#define ACTION_NONE                0
#define ACTION_ACCEPT              1
#define ACTION_REJECT              2
#define ACTION_BLACKHOLE           3
#define ACTION_BOUNCE              4
#define ACTION_SEND                5
#define ACTION_HEADER_CHANGE       6
#define ACTION_HEADER_ADD          7
#define ACTION_HEADER_REMOVE       8
#define ACTION_SKIP                9
#define ACTION_HEADER_NEW         10
#define ACTION_REGISTER_SET       11
#define ACTION_REGISTER_ADD       12
#define ACTION_SEND_NOTIFICATION  13
#define ACTION_SEND_LOG_ENTRY     14
#define ACTION_HEADER_PREPEND     15
#define ACTION_SYSLOG             16
#define ACTION_DCC_REPORT         17

struct action_struct_header
{
    action_type type;
    size_t      size;
};

struct action
{
    struct action_struct_header hdr;

    char data[1];
};

struct action_accept
{
    struct action_struct_header hdr;
};

struct action_reject
{
    struct action_struct_header hdr;
};

struct action_blackhole
{
    struct action_struct_header hdr;
};

struct action_bounce
{
    struct action_struct_header hdr;
};

struct action_send
{
    struct action_struct_header hdr;

    unsigned int type;
    
    unsigned int count;     /* count of recipients (in assciz form in rcpts array  */
    char         rcpts[1];
};

struct action_syslog
{
    struct action_struct_header hdr;

    unsigned int mask;
    char         message[1];
};


struct action_header_change
{
    struct action_struct_header hdr;

    char header_value[2]; /* header zero value zero */
};

struct action_header_add
{
    struct action_struct_header hdr;

    char header_value[2]; /* header zero value zero */
};

struct action_header_prepend
{
    struct action_struct_header hdr;

    char header_value[2]; /* header zero value zero */
};

struct action_header_new
{
    struct action_struct_header hdr;

    char header_value[2]; /* header zero value zero */
};

struct action_header_remove
{
    struct action_struct_header hdr;

    char header[1];
};

struct action_skip
{
    struct action_struct_header hdr;
};

struct action_register_set
{
    struct action_struct_header hdr;

    unsigned int number;
    int          quantity;
};

struct action_register_add
{
    struct action_struct_header hdr;

    unsigned int number;
    int          quantity;
};

struct action_send_notification
{
    struct action_struct_header hdr;

    char entry[1];
};

struct action_send_log_entry
{
    struct action_struct_header hdr;

    char   entry[1];
};

struct action_dcc_report
{
    struct action_struct_header hdr;

    char   count[1];
};

/*
 * Rule representation.
 */
struct __rule
{
    struct {
	size_t             count;
	struct condition **array;
    } conditions;

    struct {
	size_t             count;
	struct action    **array;
    } actions;
};

/*******************************************************************************
 *
 * Static functions
 *
 *******************************************************************************/

/*
 * Destroing rule contents without destroing memory for rule structure itself.
 */
static void rule_destroy(rule *r)
{
    size_t i;
    
    if(r->conditions.array)
	{
	    for(i = 0; i < r->conditions.count; i++)
		{
		    if(r->conditions.array[i])
			{
			    if(r->conditions.array[i]->hdr.type == CONDITION_HEADER_MATCH)
				{
				    regfree(&(((condition_header_match *)(r->conditions.array[i]))->regexp));
				}
			    else if(r->conditions.array[i]->hdr.type == CONDITION_ATTRIBUTE_MATCH)
				{
				    regfree(&(((condition_attribute_match *)(r->conditions.array[i]))->regexp));
				}

			    free(r->conditions.array[i]);
			}
		}

	    free(r->conditions.array);
	}

    if(r->actions.array)
	{
	    for(i = 0; i < r->actions.count; i++)
		if(r->actions.array[i])
		    free(r->actions.array[i]);

	    free(r->actions.array);
	}

    memset(r, 0, sizeof(rule));
}

/*
 * Conditions save.
 */
static void *condition_save(condition *cond, void *data, size_t size, size_t *needed_size = NULL)
{
#define SHIFT(x) 				\
    do {					\
	if(p) p += x;				\
        if(needed_size) *needed_size += x;   \
	if(size > x) size -= x;			\
    } while(0)

    unsigned char *p               = (unsigned char *)data;
    u_long        *size_ptr        =  NULL;
    size_t         typed_cond_size = 0;
    size_t         len             = 0;

    if(size < 3*sizeof(u_long))
	p =  NULL;

    if(p) *((u_long *)p) = htonl(cond->hdr.type);
    SHIFT(sizeof(u_long));

    if(p) *((u_long *)p) = htonl(cond->hdr.flags);
    SHIFT(sizeof(u_long));

    size_ptr = (u_long *)p;
    SHIFT(sizeof(u_long));

    switch(cond->hdr.type)
	{
	case CONDITION_SMTP_FROM:
	{
	    len = strlen(((struct condition_smtp_from *)cond)->email) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_smtp_from *)cond)->email, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_SMTP_FROM_LIST:
	{
	    if(sizeof(u_long) > size)
		p = NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_smtp_from_list *)cond)->lid);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);

	    break;
	}
	case CONDITION_SMTP_TO:
	{
	    len = strlen(((struct condition_smtp_to *)cond)->email) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_smtp_to *)cond)->email, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_SMTP_TO_LIST:
	{
	    if(sizeof(u_long) > size)
		p = NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_smtp_to_list *)cond)->lid);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);

	    break;
	}
	case CONDITION_HAS_HEADER:
	{
	    len = strlen(((struct condition_has_header *)cond)->header) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_has_header *)cond)->header, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_HEADER_MATCH:
	{
	    len = strlen(((struct condition_header_match *)cond)->header_regexp) + 1;
	    len += strlen(((struct condition_header_match *)cond)->header_regexp + len) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_header_match *)cond)->header_regexp, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_CATEGORY_MATCH:
	{
	    if(sizeof(u_long) > size)
		p = NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_category_match *)cond)->cat_id);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);

	    break;
	}
	case CONDITION_MSG_SIZE:
	{
	    if(sizeof(u_long) > size)
		p =  NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_msg_size *)cond)->limit);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);

	    break;
	}
	case CONDITION_IP_FROM_LIST:
	{
	    if(sizeof(u_long) > size)
		p =  NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_ip_from_list *)cond)->lid);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);

	    break;
	}
	case CONDITION_IP_FROM:
	{
	    if(2*sizeof(in_addr_t) > size)
		p =  NULL;

	    if(p) 
		{
		    *((in_addr_t *)p) = ((struct condition_ip_from *)cond)->addr;
		    *(((in_addr_t *)p)+1) = ((struct condition_ip_from *)cond)->mask;
		}

	    SHIFT(2*sizeof(in_addr_t));
	    typed_cond_size = 2*sizeof(in_addr_t);

	    break;
	}
	case CONDITION_IP_FROM_RBL:
	{
	    if(sizeof(u_long) > size)
		p =  NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_ip_from_rbl *)cond)->lid);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);

	    break;
	}
	case CONDITION_IP_FROM_NOT_IN_DNS:
	{
	    typed_cond_size = 0;

	    break;
	}
	case CONDITION_REGISTER:
	{
	    if(3*sizeof(u_long) > size)
		p =  NULL;

	    if(p) 
		{
		    *((u_long *)p)     = htonl(((struct condition_register *)cond)->number);
		    *(((u_long *)p)+1) = htonl(((struct condition_register *)cond)->type);
		    *(((u_long *)p)+2) = htonl(((struct condition_register *)cond)->quantity);
		}

	    SHIFT(3*sizeof(u_long));
	    typed_cond_size = 3*sizeof(u_long);

	    break;
	}
	case CONDITION_ATTRIBUTE_MATCH:
	{
	    len = strlen(((struct condition_attribute_match *)cond)->attribute_regexp) + 1;
	    len += strlen(((struct condition_attribute_match *)cond)->attribute_regexp + len) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_attribute_match *)cond)->attribute_regexp, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_CONTENT_TYPE_MATCH:
	{
	    len = strlen(((struct condition_content_type_match *)cond)->str_regexp) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_content_type_match *)cond)->str_regexp, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_FILE_NAME_MATCH:
	{
	    len = strlen(((struct condition_file_name_match *)cond)->str_regexp) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_file_name_match *)cond)->str_regexp, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_HAS_ATTACHMENTS:
	{
	    typed_cond_size = 0;

	    break;
	}
	case CONDITION_UNKNOWN_CONTENT:
	{
	    typed_cond_size = 0;

	    break;
	}
	case CONDITION_VIRUS_MATCH:
	{
	    len = strlen(((struct condition_virus_match *)cond)->str_regexp) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_virus_match *)cond)->str_regexp, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
	case CONDITION_DCC_MATCH:
	{
	    len = strlen(((struct condition_dcc_match *)cond)->body_fuz1_fuz2) + 1;
	    len += strlen(((struct condition_dcc_match *)cond)->body_fuz1_fuz2 + len) + 1;
	    len += strlen(((struct condition_dcc_match *)cond)->body_fuz1_fuz2 + len) + 1;

	    if(len > size)
		p = NULL;

	    if(p) memcpy(p, ((struct condition_dcc_match *)cond)->body_fuz1_fuz2, len);

	    SHIFT(len);
	    typed_cond_size = len;

	    break;
	}
    case CONDITION_BAYES_MATCH:
    {
	    if(sizeof(u_long) > size)
		p =  NULL;

	    if(p) *((u_long *)p) = htonl(((struct condition_bayes_match *)cond)->rate);

	    SHIFT(sizeof(u_long));
	    typed_cond_size = sizeof(u_long);
        
        break;
    }
	default:
	{
	    return NULL;
	}
	}

    if(size_ptr) *size_ptr = htonl(typed_cond_size);

    return p;
}

/*
 * Actions save to buffer.
 */
static void *action_save(action *act, void *data, size_t size, size_t *needed_size = NULL)
{
    unsigned char *p              = (unsigned char *)data;
    u_long        *size_ptr       =  NULL;
    size_t         typed_act_size = 0;
    size_t         len            = 0;

    if(size < 2*sizeof(u_long))
	p = NULL;

    if(p) *((u_long *)p) = htonl(act->hdr.type);
    SHIFT(sizeof(u_long));

    size_ptr = (u_long *)p;
    SHIFT(sizeof(u_long));

    switch(act->hdr.type)
	{
	case ACTION_ACCEPT:
	case ACTION_REJECT:
	case ACTION_BLACKHOLE:
	case ACTION_BOUNCE:
	case ACTION_SKIP:

	    /* here is nothing to do, really */

	    break;

	case ACTION_SEND:
	{
	    size_t i;
	    char *ptr = ((struct action_send *)act)->rcpts;

	    len = 0;
	    for(i = 0; i < ((struct action_send *)act)->count; i++)
		{
		    len += strlen(ptr)+1;
		    ptr = strchr(ptr, '\0') + 1;
		}

	    typed_act_size = len + 2*sizeof(u_long);

	    if(typed_act_size > size)
		p = NULL;

	    if(p) *((u_long *)p) = htonl(((struct action_send *)act)->type);
	    SHIFT(sizeof(u_long));

	    if(p) *((u_long *)p) = htonl(((struct action_send *)act)->count);
	    SHIFT(sizeof(u_long));

	    if(p) memcpy(p, ((struct action_send *)act)->rcpts, len);
	    SHIFT(len);
	    
	    break;
	}
	case ACTION_SYSLOG:
	{
//	    size_t i;
	    char *ptr = ((struct action_syslog *)act)->message;

	    len = strlen(ptr)+1;
	    typed_act_size = len + sizeof(u_long);

	    if(typed_act_size > size)
		p = NULL;

	    if(p) *((u_long *)p) = htonl(((struct action_syslog *)act)->mask);
	    SHIFT(sizeof(u_long));

	    if(p) memcpy(p, ((struct action_syslog *)act)->message, len);
	    SHIFT(len);
	    
	    break;
	}
	case ACTION_HEADER_CHANGE:
	{
	    len = strlen(((struct action_header_change *)act)->header_value) + 1;
	    len += strlen(((struct action_header_change *)act)->header_value + len) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_header_change *)act)->header_value, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}
	case ACTION_HEADER_ADD:
	{
	    len = strlen(((struct action_header_add *)act)->header_value) + 1;
	    len += strlen(((struct action_header_add *)act)->header_value + len) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_header_add *)act)->header_value, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}
	case ACTION_HEADER_PREPEND:
	{
	    len = strlen(((struct action_header_prepend *)act)->header_value) + 1;
	    len += strlen(((struct action_header_prepend *)act)->header_value + len) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_header_prepend *)act)->header_value, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}
	case ACTION_HEADER_REMOVE:
	{
	    len = strlen(((struct action_header_remove *)act)->header) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_header_remove *)act)->header, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}
	case ACTION_HEADER_NEW:
	{
	    len = strlen(((struct action_header_new *)act)->header_value) + 1;
	    len += strlen(((struct action_header_new *)act)->header_value + len) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_header_new *)act)->header_value, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}
	case ACTION_REGISTER_SET:
	{
	    if(2*sizeof(u_long) > size)
		p = NULL;
	    
	    if(p) 
		{
		    *((u_long *)p)     = htonl(((struct action_register_set *)act)->number);
		    *(((u_long *)p)+1) = htonl(((struct action_register_set *)act)->quantity);
		}

	    SHIFT(2*sizeof(u_long));
	    typed_act_size = 2*sizeof(u_long);

	    break;
	}
	case ACTION_REGISTER_ADD:
	{
	    if(2*sizeof(u_long) > size)
		p = NULL;
	    
	    if(p) 
		{
		    *((u_long *)p)     = htonl(((struct action_register_add *)act)->number);
		    *(((u_long *)p)+1) = htonl(((struct action_register_add *)act)->quantity);
		}

	    SHIFT(2*sizeof(u_long));
	    typed_act_size = 2*sizeof(u_long);

	    break;
	}

	case ACTION_SEND_NOTIFICATION:
	{
	    len = strlen(((struct action_send_notification *)act)->entry) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_send_notification *)act)->entry, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}

	case ACTION_SEND_LOG_ENTRY:
	{
	    len = strlen(((struct action_send_log_entry *)act)->entry) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_send_log_entry *)act)->entry, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}

	case ACTION_DCC_REPORT:
	{
	    len = strlen(((struct action_dcc_report *)act)->count) + 1;

	    if(len > size)
		p = NULL;
	    
	    if(p) memcpy(p, ((struct action_dcc_report *)act)->count, len);

	    SHIFT(len);
	    typed_act_size = len;

	    break;
	}

	default:
	    return NULL;
	}

    if(size_ptr) *size_ptr = htonl(typed_act_size);

    return p;

#undef SHIFT
}


static void *rule_save(rule *r, void *data, size_t size)
{
    size_t  i;
    void   *p;

    if(size < 2*sizeof(u_long))
	return NULL;

    *((u_long *)data) = htonl(r->conditions.count);
    data = (u_long *)data + 1;

    *((u_long *)data) = htonl(r->actions.count);
    data = (u_long *)data + 1;

    size -= 2*sizeof(u_long);

    for(i = 0; i < r->conditions.count; i++)
	{
	    p = condition_save(r->conditions.array[i], data, size);
	    if(!p) return NULL;

	    size -= (char *)p - (char *)data;
	    data = p;
	}

    for(i = 0; i < r->actions.count; i++)
	{
	    p = action_save(r->actions.array[i], data, size);
	    if(!p) return NULL;

	    size -= (char *)p - (char *)data;
	    data = p;
	}

    return data;
}

/*
 * Return size.
 */
static size_t action_size_of(action *act)
{
    size_t res = 0;
    action_save(act, NULL, 0, &res);
    return res;
}

static size_t condition_size_of(condition *cond)
{
    size_t res = 0;
    condition_save(cond, NULL, 0, &res);
    return res;
}

/*
 * XXX This function is better fit as call to rule_save...
 */
static size_t rule_size_of(rule *r)
{
    size_t i;
    size_t res = 2*sizeof(u_long);

    for(i = 0; i < r->conditions.count; i++)
	res += condition_size_of(r->conditions.array[i]);

    for(i = 0; i < r->actions.count; i++)
	res += action_size_of(r->actions.array[i]);

    return res;
}


/*
 * Conditions load (perfoms network to host conversions and so on).
 */
static condition *condition_load(void **data, size_t *size)
{
    u_long *cond_hdr = (u_long *)(*data);
    size_t  len = 0;

    condition *result = NULL;

#define HDR_SIZE (3*sizeof(u_long))
#define SHIFT(x)				\
    do {					\
	*size -= x;				\
	*data = (char *)(*data) + x;	\
    } while(0)

    switch(ntohl(cond_hdr[0]))
	{
	case CONDITION_SMTP_FROM:
	{
	    len = sizeof(condition_smtp_from);
	    len += strlen((char *)(*data) + HDR_SIZE);

	    struct condition_smtp_from *real_result = 
		(struct condition_smtp_from *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->email, (char *)(*data) + HDR_SIZE, len - sizeof(condition_smtp_from) + 1);

	    SHIFT(HDR_SIZE + (len - sizeof(condition_smtp_from) + 1));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_SMTP_FROM_LIST:
	{
	    len = sizeof(condition_smtp_from_list);

	    struct condition_smtp_from_list *real_result = 
		(struct condition_smtp_from_list *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->lid = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_SMTP_TO:
	{
	    len = sizeof(condition_smtp_to);
	    len += strlen((char *)(*data) + HDR_SIZE);

	    struct condition_smtp_to *real_result = 
		(struct condition_smtp_to *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->email, (char *)(*data) + HDR_SIZE, len - sizeof(condition_smtp_to) + 1);
	    SHIFT(HDR_SIZE + (len - sizeof(condition_smtp_to) + 1));
	    
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_SMTP_TO_LIST:
	{
	    len = sizeof(condition_smtp_to_list);

	    struct condition_smtp_to_list *real_result = 
		(struct condition_smtp_to_list *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->lid = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_HAS_HEADER:
	{
	    len = sizeof(condition_has_header);
	    len += strlen((char *)(*data) + HDR_SIZE);

	    struct condition_has_header *real_result = 
		(struct condition_has_header *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header, (char *)(*data) + HDR_SIZE, len - sizeof(condition_has_header) + 1);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_has_header) + 1));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_HEADER_MATCH:
	{
	    size_t hdr_len;
	    int rc_res;

	    len = sizeof(condition_header_match);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += strlen((char *)(*data) + HDR_SIZE + hdr_len + 1);

	    struct condition_header_match *real_result = 
		(struct condition_header_match *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header_regexp, (char *)(*data) + HDR_SIZE, len - sizeof(condition_header_match) + 2);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_header_match) + 2));

	    rc_res = regcomp(&(real_result->regexp), strchr(real_result->header_regexp, '\0')+1, REG_EXTENDED | REG_NOSUB);

	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_CATEGORY_MATCH:
	{
	    len = sizeof(condition_category_match);

	    struct condition_category_match *real_result = 
		(struct condition_category_match *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->cat_id = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_MSG_SIZE:
	{
	    len = sizeof(condition_msg_size);

	    struct condition_msg_size *real_result = 
		(struct condition_msg_size *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->limit = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_IP_FROM_LIST:
	{
	    len = sizeof(condition_ip_from_list);

	    struct condition_ip_from_list *real_result = 
		(struct condition_ip_from_list *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->lid = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_IP_FROM:
	{
	    len = sizeof(condition_ip_from);

	    struct condition_ip_from *real_result = 
		(struct condition_ip_from *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->addr = ((in_addr_t *)(cond_hdr + 3))[0];
	    real_result->mask = ((in_addr_t *)(cond_hdr + 3))[1];

	    SHIFT(HDR_SIZE + 2*sizeof(in_addr_t));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_IP_FROM_RBL:
	{
	    len = sizeof(condition_ip_from_rbl);

	    struct condition_ip_from_rbl *real_result = 
		(struct condition_ip_from_rbl *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->lid = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_IP_FROM_NOT_IN_DNS:
	{
	    len = sizeof(condition_ip_from_not_in_dns);

	    struct condition_ip_from_not_in_dns *real_result = 
		(struct condition_ip_from_not_in_dns *)malloc(len);
	    memset(real_result, 0, len);

	    SHIFT(HDR_SIZE);
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_REGISTER:
	{
	    len = sizeof(condition_register);

	    struct condition_register *real_result = 
		(struct condition_register *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->number   = ntohl(((u_long *)(cond_hdr + 3))[0]);
	    real_result->type     = ntohl(((u_long *)(cond_hdr + 3))[1]);
	    real_result->quantity = ntohl(((u_long *)(cond_hdr + 3))[2]);

	    SHIFT(HDR_SIZE + 3*sizeof(u_long));
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_ATTRIBUTE_MATCH:
	{
	    size_t hdr_len;
	    int rc_res;

	    len = sizeof(condition_attribute_match);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += strlen((char *)(*data) + HDR_SIZE + hdr_len + 1);

	    struct condition_attribute_match *real_result = 
		(struct condition_attribute_match *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->attribute_regexp, (char *)(*data) + HDR_SIZE, len - sizeof(condition_attribute_match) + 2);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_attribute_match) + 2));

	    rc_res = regcomp(&(real_result->regexp), strchr(real_result->attribute_regexp, '\0')+1, REG_EXTENDED | REG_NOSUB);

	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_CONTENT_TYPE_MATCH:
	{
	    int rc_res;

	    len = sizeof(condition_content_type_match);
	    len += strlen((char *)(*data) + HDR_SIZE);

	    struct condition_content_type_match *real_result = 
		(struct condition_content_type_match *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->str_regexp, (char *)(*data) + HDR_SIZE, len - sizeof(condition_content_type_match) + 1);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_content_type_match) + 1));

	    rc_res = regcomp(&(real_result->regexp), real_result->str_regexp, REG_EXTENDED | REG_NOSUB);

	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_FILE_NAME_MATCH:
	{
	    int rc_res;

	    len = sizeof(condition_file_name_match);
	    len += strlen((char *)(*data) + HDR_SIZE);

	    struct condition_file_name_match *real_result = 
		(struct condition_file_name_match *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->str_regexp, (char *)(*data) + HDR_SIZE, len - sizeof(condition_file_name_match) + 1);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_file_name_match) + 1));

	    rc_res = regcomp(&(real_result->regexp), real_result->str_regexp, REG_EXTENDED | REG_NOSUB);

	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_HAS_ATTACHMENTS:
	{
	    len = sizeof(condition_has_attachments);

	    struct condition_has_attachments *real_result = 
		(struct condition_has_attachments *)malloc(len);
	    memset(real_result, 0, len);

	    SHIFT(HDR_SIZE);
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_UNKNOWN_CONTENT:
	{
	    len = sizeof(condition_unknown_content);

	    struct condition_unknown_content *real_result = 
		(struct condition_unknown_content *)malloc(len);
	    memset(real_result, 0, len);

	    SHIFT(HDR_SIZE);
	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_VIRUS_MATCH:
	{
	    int rc_res;

	    len = sizeof(condition_virus_match);
	    len += strlen((char *)(*data) + HDR_SIZE);

	    struct condition_virus_match *real_result = 
		(struct condition_virus_match *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->str_regexp, (char *)(*data) + HDR_SIZE, len - sizeof(condition_virus_match) + 1);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_virus_match) + 1));

	    rc_res = regcomp(&(real_result->regexp), real_result->str_regexp, REG_EXTENDED | REG_NOSUB);

	    result = (condition *)real_result;

	    break;
	}
	case CONDITION_DCC_MATCH:
	{
	    size_t hdr_len;
            size_t len1, len2;
//	    int rc_res;

	    len = sizeof(condition_dcc_match);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += (len1 = strlen((char *)(*data) + HDR_SIZE + hdr_len + 1));
            len += (len2 = strlen((char *)(*data) + HDR_SIZE + hdr_len + 1 + len1 + 1));

	    struct condition_dcc_match *real_result = 
		(struct condition_dcc_match *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->body_fuz1_fuz2, (char *)(*data) + HDR_SIZE, len - sizeof(condition_dcc_match) + 3);
	    
	    SHIFT(HDR_SIZE + (len - sizeof(condition_dcc_match) + 3));

	    result = (condition *)real_result;

	    break;
	}
    case CONDITION_BAYES_MATCH:
    {
	    len = sizeof(condition_bayes_match);

	    struct condition_bayes_match *real_result = 
		(struct condition_bayes_match *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->rate = ntohl(cond_hdr[3]);

	    SHIFT(HDR_SIZE + sizeof(u_long));
	    result = (condition *)real_result;

        break;
    }
	default:
	    logger_printf(LOGGER_ERROR, "condition_load: bad condition type: %u", ntohl(cond_hdr[0]));
	    return NULL;
	}

    result->hdr.type  = htonl(cond_hdr[0]);
    result->hdr.flags = htonl(cond_hdr[1]);
    result->hdr.size  = len - sizeof(condition_struct_header);

#undef HDR_SIZE

    return result;
}

static action *action_load(void **data, size_t *size)
{
    u_long *act_hdr = (u_long *)(*data);
    size_t  len = 0;

    action *result = NULL;

#define HDR_SIZE (2*sizeof(u_long))

    switch(htonl(act_hdr[0]))
	{
	case ACTION_ACCEPT:
	{
	    len = sizeof(action_accept);

	    struct action_accept *real_result = (action_accept *)malloc(len);
	    memset(real_result, 0, len);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE);

	    break;
	}
	case ACTION_REJECT:
	{
	    len = sizeof(action_reject);

	    struct action_reject *real_result = (action_reject *)malloc(len);
	    memset(real_result, 0, len);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE);

	    break;
	}
	case ACTION_BLACKHOLE:
	{
	    len = sizeof(action_blackhole);

	    struct action_blackhole *real_result = (action_blackhole *)malloc(len);
	    memset(real_result, 0, len);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE);

	    break;
	}
	case ACTION_BOUNCE:
	{
	    len = sizeof(action_bounce);

	    struct action_bounce *real_result = (action_bounce *)malloc(len);
	    memset(real_result, 0, len);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE);

	    break;
	}
	case ACTION_SKIP:
	{
	    len = sizeof(action_skip);

	    struct action_skip *real_result = (action_skip *)malloc(len);
	    memset(real_result, 0, len);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE);

	    break;
	}

	case ACTION_SEND:
	{
	    size_t i;
	    char *p = (char *)(*data) + HDR_SIZE + 2*sizeof(u_long);
	    len = 0;

	    for(i = 0; i < ntohl(act_hdr[3]); i++)
		{
		    len += strlen(p)+1;
		    p = strchr(p, '\0') + 1;
		}

	    struct action_send *real_result = (action_send *)malloc(len + sizeof(action_send) - 1);
	    memset(real_result, 0, len);
	    
	    real_result->type  = ntohl(act_hdr[2]);
	    real_result->count = ntohl(act_hdr[3]);

	    memcpy(real_result->rcpts, (char *)(*data) + HDR_SIZE + 2*sizeof(u_long), len);
	    
	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + 2*sizeof(u_long) + len);
	    
	    break;
	}
	case ACTION_SYSLOG:
	{
//	    size_t i;
	    char *p = (char *)(*data) + HDR_SIZE + 1*sizeof(u_long);
	    len = 0;

	    len = strlen(p)+1;

	    struct action_syslog *real_result = (action_syslog *)malloc(len + sizeof(action_syslog) - 1);
	    memset(real_result, 0, len);
	    
	    real_result->mask  = ntohl(act_hdr[2]);
	    memcpy(real_result->message, (char *)(*data) + HDR_SIZE + sizeof(u_long), len);
	    
	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + sizeof(u_long) + len);
	    
	    break;
	}
	case ACTION_HEADER_CHANGE:
	{
	    size_t hdr_len;

	    len = sizeof(action_header_change);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += strlen((char *)(*data) + HDR_SIZE + hdr_len + 1);

	    struct action_header_change *real_result = (action_header_change *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header_value, (char *)(*data) + HDR_SIZE, len - sizeof(action_header_change) + 2);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_header_change) + 2);

	    break;
	}
	case ACTION_HEADER_ADD:
	{
	    size_t hdr_len;

	    len = sizeof(action_header_add);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += strlen((char *)(*data) + HDR_SIZE + hdr_len + 1);

	    struct action_header_add *real_result = (action_header_add *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header_value, (char *)(*data) + HDR_SIZE, len - sizeof(action_header_add) + 2);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_header_add) + 2);

	    break;
	}
	case ACTION_HEADER_PREPEND:
	{
	    size_t hdr_len;

	    len = sizeof(action_header_prepend);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += strlen((char *)(*data) + HDR_SIZE + hdr_len + 1);

	    struct action_header_prepend *real_result = (action_header_prepend *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header_value, (char *)(*data) + HDR_SIZE, len - sizeof(action_header_prepend) + 2);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_header_prepend) + 2);

	    break;
	}
	case ACTION_HEADER_REMOVE:
	{
	    size_t hdr_len;

	    len = sizeof(action_header_remove);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));

	    struct action_header_remove *real_result = (action_header_remove *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header, (char *)(*data) + HDR_SIZE, len - sizeof(action_header_remove) + 1);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_header_remove) + 1);

	    break;
	}
	case ACTION_HEADER_NEW:
	{
	    size_t hdr_len;

	    len = sizeof(action_header_new);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));
	    len += strlen((char *)(*data) + HDR_SIZE + hdr_len + 1);

	    struct action_header_new *real_result = (action_header_new *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->header_value, (char *)(*data) + HDR_SIZE, len - sizeof(action_header_new) + 2);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_header_new) + 2);

	    break;
	}
	case ACTION_REGISTER_SET:
	{
//	    size_t hdr_len;

	    len = sizeof(action_register_set);
	    struct action_register_set *real_result = (action_register_set *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->number   = ntohl(((u_long*)((char *)(*data)+HDR_SIZE))[0]);
	    real_result->quantity = ntohl(((u_long*)((char *)(*data)+HDR_SIZE))[1]);
	    
	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + 2*sizeof(u_long));

	    break;
	}
	case ACTION_REGISTER_ADD:
	{
//	    size_t hdr_len;

	    len = sizeof(action_register_add);
	    struct action_register_add *real_result = (action_register_add *)malloc(len);
	    memset(real_result, 0, len);

	    real_result->number   = ntohl(((u_long*)((char *)(*data)+HDR_SIZE))[0]);
	    real_result->quantity = ntohl(((u_long*)((char *)(*data)+HDR_SIZE))[1]);
	    
	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + 2*sizeof(u_long));

	    break;
	}
	case ACTION_SEND_NOTIFICATION:
	{
	    size_t hdr_len;

	    len = sizeof(action_send_notification);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));

	    struct action_send_notification *real_result = (action_send_notification *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->entry, (char *)(*data) + HDR_SIZE, len - sizeof(action_send_notification) + 1);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_send_notification) + 1);

	    break;
	}
	case ACTION_SEND_LOG_ENTRY:
	{
	    size_t hdr_len;

	    len = sizeof(action_send_log_entry);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));

	    struct action_send_log_entry *real_result = (action_send_log_entry *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->entry, (char *)(*data) + HDR_SIZE, len - sizeof(action_send_log_entry) + 1);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_send_log_entry) + 1);

	    break;
	}
	case ACTION_DCC_REPORT:
	{
	    size_t hdr_len;

	    len = sizeof(action_dcc_report);
	    len += (hdr_len = strlen((char *)(*data) + HDR_SIZE));

	    struct action_dcc_report *real_result = (action_dcc_report *)malloc(len);
	    memset(real_result, 0, len);

	    memcpy(real_result->count, (char *)(*data) + HDR_SIZE, len - sizeof(action_dcc_report) + 1);

	    result = (action *)real_result;
	    SHIFT(HDR_SIZE + len - sizeof(action_dcc_report) + 1);

	    break;
	}
	default:
	    logger_printf(LOGGER_ERROR, "action_load: bad action type: %u", ntohl(act_hdr[0]));
	    return NULL;
	}

    result->hdr.type  = htonl(act_hdr[0]);
    result->hdr.size  = len - sizeof(action_struct_header);

#undef SHIFT
#undef HDR_SIZE

    return result;
}

static rule *rule_load(rule *r, void **data, size_t *size)
{
#define RETURN_NULL 				\
    do {					\
	goto error;				\
    } while(0)

    size_t  i;
    rule   *result = r;

    memset(result, 0, sizeof(rule));

    result->conditions.count = ntohl(((u_long *)(*data))[0]);
    result->actions.count    = ntohl(((u_long *)(*data))[1]);

    result->conditions.array = (condition **)malloc(result->conditions.count*sizeof(condition *));
    result->actions.array    = (action **)malloc(result->actions.count*sizeof(action *));

    memset(result->conditions.array, 0, result->conditions.count*sizeof(condition *));
    memset(result->actions.array, 0, result->actions.count*sizeof(action *));

    *data = (u_long *)(*data) + 2;
    *size -= 2*sizeof(u_long);

    for(i = 0; i < result->conditions.count; i++)
	{
	    result->conditions.array[i] = condition_load(data, size);
	    if(!result->conditions.array[i])
		RETURN_NULL;
	}

    for(i = 0; i < result->actions.count; i++)
	{
	    result->actions.array[i] = action_load(data, size);
	    if(!result->actions.array[i])
		RETURN_NULL;
	}

#undef RETURN_NULL

    return result;

  error:
    
    if(result)
	{
	    rule_destroy(result);
	}

    return NULL;
}

/*******************************************************************************
 *
 * Exported functions
 *
 *******************************************************************************/

/*
 * Profile
 */

profile *profile_create(const char *name, const char *description,
			unsigned int type, size_t count, char **rcpts)
{
    profile *res = (profile *)malloc(sizeof(profile));
    memset(res, 0, sizeof(profile));

    if(name && *name) 
	res->name = _strdup(name);

    if(description && *description)
	res->description = _strdup(description);

    res->type = type;

    if(type == PROFILE_PRIVATE && count)
	{
	    if(rcpts)
		{
		    res->rcpts.array = (char **)malloc(sizeof(char *)*count);
		    memset(res->rcpts.array, 0, sizeof(char *)*count);

		    for(res->rcpts.count = 0; res->rcpts.count < count; res->rcpts.count++)
			res->rcpts.array[res->rcpts.count] = _strdup(rcpts[res->rcpts.count]);
		}
	    else
		res->rcpts.lid = count;
	}

    return res;
}

void profile_destroy(profile *p)
{
    size_t i;

    if(p->name)
	free(p->name);

    if(p->description)
	free(p->description);


    if(p->rules.array)
	{
	    for(i = 0; i < p->rules.count; i++)
		{
		    if(p->rules.array + i)
			rule_destroy(p->rules.array + i);
		}
	    
	    free(p->rules.array);
	}

    if(p->rcpts.array)
	{
	    for(i = 0; i < p->rcpts.count; i++)
		free(p->rcpts.array[i]);

	    free(p->rcpts.array);
	}

    free(p);
}

size_t profile_get_size(profile *p)
{
    size_t i;
    size_t res = 
	(p->name ? strlen(p->name) + 1 : 1) + 
	(p->description ? strlen(p->description) + 1 : 1) + 
	4*sizeof(u_long);

    for(i = 0; i < p->rcpts.count; i++)
	res += strlen(p->rcpts.array[i]) + 1;
       
    for(i = 0; i < p->rules.count; i++)
	res += rule_size_of(p->rules.array + i);

    return res;
}

profile *profile_load(void *data, size_t size)
{
#define RETURN_NULL				\
    do {					\
	goto error; 				\
    } while(0)

    size_t len, i;
    profile *result = (profile *)malloc(sizeof(profile));
    memset(result, 0, sizeof(profile));

    len = strlen((char*)(data));

    if(len == 0)
	{
	    data = (char*)data + 1;
	    size -= 1;
	}
    else
	{
	    result->name = (char *)malloc(len + 1);
	    memcpy(result->name, data, len+1);

	    data = (char*)data + len + 1;
	    size -= len + 1;
	}

    len = strlen((char*)(data));

    if(len == 0)
	{
	    data = (char*)data + 1;
	    size -= 1;
	}
    else
	{
	    result->description = (char *)malloc(len + 1);
	    memcpy(result->description, data, len+1);

	    data = (char*)data + len + 1;
	    size -= len + 1;
	}

    result->type = ntohl(*((u_long *)data));
    data = (u_long *)data + 1;
    size -= sizeof(u_long);

    result->rcpts.count = ntohl(*((u_long *)data));
    data = (u_long *)data + 1;
    size -= sizeof(u_long);

    result->rcpts.lid = ntohl(*((u_long *)data));
    data = (u_long *)data + 1;
    size -= sizeof(u_long);

    if(result->type == PROFILE_PRIVATE)
	{
	    if(result->rcpts.count)
		{
		    result->rcpts.array = (char **)malloc(result->rcpts.count * sizeof(char *));
		    memset(result->rcpts.array, 0, result->rcpts.count * sizeof(char *));

		    for(i = 0; i < result->rcpts.count; i++)
			{
			    len = strlen((char *)data);
			    
			    result->rcpts.array[i] = _strdup((char *)data);
			    
			    data = (char *)data + len + 1;
			    size -= len + 1;
			}
		}
	}
    
    result->rules.count = ntohl(*((u_long *)data));
    data = (u_long *)data + 1;
    size -= sizeof(u_long);
    
    if(result->rules.count)
	{
	    result->rules.array = (rule *)malloc(result->rules.count*sizeof(rule));
	    memset(result->rules.array, 0, result->rules.count*sizeof(rule));
    
	    for(i = 0; i < result->rules.count; i++)
		{
		    if(!rule_load(result->rules.array + i, &data, &size))
			RETURN_NULL;
		}
	}


    return result;

  error:

    if(result)
	profile_destroy(result);

    return NULL;
}

void *profile_save(profile *p, void *data, size_t size)
{

    size_t  i;
    size_t  name_len = p->name ? strlen(p->name) : 0;
    size_t  desc_len = p->description ? strlen(p->description) : 0;
    size_t  str_len = 0;
    void   *ptr;

    if(name_len + 1 + desc_len + 1 > size)
	return NULL;

    if(p->name)
	memcpy(data, p->name, name_len + 1);
    else
	*((char *)data) = '\0';
    data = (char *)data + name_len + 1;

    if(p->description)
	memcpy(data, p->description, desc_len + 1);
    else
	*((char *)data) = '\0';
    data = (char *)data + desc_len + 1;

    size -= name_len + desc_len + 2;

    *((u_long *)data) = htonl(p->type);
    data = (char *)data + sizeof(u_long);

    *((u_long *)data) = htonl(p->rcpts.count);
    data = (char *)data + sizeof(u_long);

    *((u_long *)data) = htonl(p->rcpts.lid);
    data = (char *)data + sizeof(u_long);

    size -= 3*sizeof(u_long);

    for(i = 0; i < p->rcpts.count; i++)
	{
	    str_len = strlen(p->rcpts.array[i]) + 1;
	    
	    memcpy(data, p->rcpts.array[i], str_len);
	    data = (char *)data + str_len;
	    size -= str_len;
	}

    *((u_long *)data) = htonl(p->rules.count);
    data = (char *)data + sizeof(u_long);
    size -= sizeof(u_long);

    for(i = 0; i < p->rules.count; i++)
	{
	    ptr = rule_save(p->rules.array+i, data, size);
	    if(!ptr)
		return NULL;

	    size -= (char *)ptr - (char *)data;
	    data = ptr;
	}

    return data;
}

rule *profile_create_rule(profile *p)
{
    if(!p) return NULL;

    if(p->rules.array)
	{
	    p->rules.array = (rule *)realloc(p->rules.array, (p->rules.count+1)*sizeof(rule));
	    memset(p->rules.array + p->rules.count, 0, sizeof(rule));
	    p->rules.count++;
	}
    else
	{
	    p->rules.array = (rule *)malloc(sizeof(rule));
	    memset(p->rules.array, 0, sizeof(rule));
	    p->rules.count++;
	}

    return p->rules.array + (p->rules.count-1);
}

int profile_remove_last_rule(profile *p)
{
    if(p == NULL || p->rules.count == 0)  return 1;

    rule_destroy(p->rules.array + (p->rules.count-1));

    p->rules.count--;

    if(p->rules.count)
	{
	    p->rules.array = (rule *)realloc(p->rules.array, p->rules.count*sizeof(rule));
	}
    else
	{
	    free(p->rules.array);
	    p->rules.array = NULL;
	}

    return 0;
}


/*******************************************************************************
 *
 * Rules (conditions)
 *
 *******************************************************************************/

#define ALLOC_CONDITION(x, s, id)				\
   struct x *cond = (x *)malloc(s);				\
   if(cond == NULL) return -1;					\
   memset(cond, 0, s);						\
   cond->hdr.flags = flags;					\
   cond->hdr.size = s - sizeof(condition_struct_header); 	\
   cond->hdr.type = id; 
   

#define ADD_CONDITION 											\
do {													\
    if(r->conditions.array)										\
	{												\
	    r->conditions.array = (condition **)realloc(r->conditions.array, 				\
							 sizeof(condition *)*(r->conditions.count+1));	\
	    r->conditions.array[r->conditions.count] = (condition *)cond;				\
	    r->conditions.count++;									\
	}												\
    else												\
	{												\
	    r->conditions.array = (condition **)malloc(sizeof(condition *));				\
	    r->conditions.array[0] = (condition *)cond;						\
	    r->conditions.count = 1;									\
	}												\
} while(0)

int rule_add_condition_smtp_from(rule *r, unsigned int flags, const char *email)
{
    if(!r || !email) 
	return -1;

    ALLOC_CONDITION(condition_smtp_from, 
		    sizeof(condition_smtp_from)+strlen(email), 
		    CONDITION_SMTP_FROM);

    strcpy(cond->email, email);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_smtp_from_list(rule *r, unsigned int flags, list_id lid)
{
    if(!r || lid == LIST_ID_NONE) 
	return -1;

    ALLOC_CONDITION(condition_smtp_from_list, 
		    sizeof(condition_smtp_from_list), 
		    CONDITION_SMTP_FROM_LIST);

    cond->lid = lid;

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_smtp_to(rule *r, unsigned int flags, const char *email)
{
    if(!r || !email) 
	return -1;

    ALLOC_CONDITION(condition_smtp_to, 
		    sizeof(condition_smtp_to)+strlen(email), 
		    CONDITION_SMTP_TO);

    strcpy(cond->email, email);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_smtp_to_list(rule *r, unsigned int flags, list_id lid)
{
    if(!r || lid == LIST_ID_NONE) 
	return -1;

    ALLOC_CONDITION(condition_smtp_to_list, 
		    sizeof(condition_smtp_to_list), 
		    CONDITION_SMTP_TO_LIST);

    cond->lid = lid;

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_has_header(rule *r, unsigned int flags, const char *header)
{
    if(!r || !header) 
	return -1;

    ALLOC_CONDITION(condition_has_header, 
		    sizeof(condition_has_header)+strlen(header), 
		    CONDITION_HAS_HEADER);

    strcpy(cond->header, header);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_header_match(rule *r, unsigned int flags, const char *header, const char *regexp)
{
    if(!r || !header || !regexp) 
	return -1;

    regex_t temp;

    if(regcomp(&temp, regexp, REG_EXTENDED | REG_NOSUB))
	return -1;

    regfree(&temp);

    ALLOC_CONDITION(condition_header_match, 
		    sizeof(condition_header_match)+strlen(header)+strlen(regexp), 
		    CONDITION_HEADER_MATCH);

    strcpy(cond->header_regexp, header);
    strcpy(strchr(cond->header_regexp, '\0')+1, regexp);

    regcomp(&(cond->regexp), regexp, REG_EXTENDED | REG_NOSUB);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_category_match(rule *r, unsigned int flags, unsigned int cat_id)
{
    if(!r || cat_id == 0)
	return -1;

    ALLOC_CONDITION(condition_category_match, 
		    sizeof(condition_category_match), 
		    CONDITION_CATEGORY_MATCH);

    cond->cat_id = cat_id;
    
    ADD_CONDITION;

    return 0;
}

int rule_add_condition_msg_size(rule *r, unsigned int flags, unsigned int limit)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_msg_size, 
		    sizeof(condition_msg_size), 
		    CONDITION_MSG_SIZE);

    cond->limit = limit;

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_ip_from(rule *r, unsigned int flags, const char *ip)
{
    if(!r)
	return -1;

    size_t bitlen = 32;
    const char *ptr = strchr(ip, '/');

    if(ptr && ptr-ip+1 > 50)
	return -1;

    ALLOC_CONDITION(condition_ip_from, 
		    sizeof(condition_ip_from), 
		    CONDITION_IP_FROM);

    if(ptr)
	{
	    char *tmp = (char *)alloca(ptr-ip+1);
	    memcpy(tmp, ip, ptr-ip);
	    tmp[ptr-ip] = 0;

	    cond->addr = inet_addr(tmp);
	    sscanf(ptr+1, "%u", &bitlen);
	}
    else
	{
	    cond->addr = inet_addr(ip);
	}
	
    /* XXX 31 const here is not very good solution. */

    for(size_t i = 0; i < bitlen; i++)
	cond->mask |= (1 << (31-i));

    cond->mask = htonl(cond->mask);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_ip_from_list(rule *r, unsigned int flags, list_id lid)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_ip_from_list, 
		    sizeof(condition_ip_from_list), 
		    CONDITION_IP_FROM_LIST);

    cond->lid = lid;

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_ip_from_rbl(rule *r, unsigned int flags, list_id lid)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_ip_from_rbl,
		    sizeof(condition_ip_from_rbl),
		    CONDITION_IP_FROM_RBL);

    cond->lid = lid;

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_ip_from_not_in_dns(rule *r, unsigned int flags)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_ip_from_not_in_dns,
		    sizeof(condition_ip_from_not_in_dns),
		    CONDITION_IP_FROM_NOT_IN_DNS);

    ADD_CONDITION;

    return 0;
}


int rule_add_condition_register(rule *r, unsigned int flags, unsigned int number, unsigned int type, int quantity)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_register,
		    sizeof(condition_register),
		    CONDITION_REGISTER);

    cond->number   = number;
    cond->type     = type;
    cond->quantity = quantity;

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_attribute_match(rule *r, unsigned int flags, const char *attribute, const char *regexp)
{
    if(!r || !attribute || !regexp) 
	return -1;

    regex_t temp;

    if(regcomp(&temp, regexp, REG_EXTENDED | REG_NOSUB))
	return -1;

    regfree(&temp);

    ALLOC_CONDITION(condition_attribute_match, 
		    sizeof(condition_attribute_match)+strlen(attribute)+strlen(regexp), 
		    CONDITION_ATTRIBUTE_MATCH);

    strcpy(cond->attribute_regexp, attribute);
    strcpy(strchr(cond->attribute_regexp, '\0')+1, regexp);

    regcomp(&(cond->regexp), regexp, REG_EXTENDED | REG_NOSUB);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_content_type_match (rule *r, unsigned int flags, const char *regexp)
{
    if(!r || !regexp) 
	return -1;

    regex_t temp;

    if(regcomp(&temp, regexp, REG_EXTENDED | REG_NOSUB))
	return -1;

    regfree(&temp);

    ALLOC_CONDITION(condition_content_type_match, 
		    sizeof(condition_content_type_match)+strlen(regexp), 
		    CONDITION_CONTENT_TYPE_MATCH);

    strcpy(cond->str_regexp, regexp);

    regcomp(&(cond->regexp), regexp, REG_EXTENDED | REG_NOSUB);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_file_name_match    (rule *r, unsigned int flags, const char *regexp)
{
    if(!r || !regexp) 
	return -1;

    regex_t temp;

    if(regcomp(&temp, regexp, REG_EXTENDED | REG_NOSUB))
	return -1;

    regfree(&temp);

    ALLOC_CONDITION(condition_file_name_match, 
		    sizeof(condition_file_name_match)+strlen(regexp), 
		    CONDITION_FILE_NAME_MATCH);

    strcpy(cond->str_regexp, regexp);

    regcomp(&(cond->regexp), regexp, REG_EXTENDED | REG_NOSUB);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_has_attachments(rule *r, unsigned int flags)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_has_attachments,
		    sizeof(condition_has_attachments),
		    CONDITION_HAS_ATTACHMENTS);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_unknown_content(rule *r, unsigned int flags)
{
    if(!r)
	return -1;

    ALLOC_CONDITION(condition_unknown_content,
		    sizeof(condition_unknown_content),
		    CONDITION_UNKNOWN_CONTENT);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_virus_match(rule *r, unsigned int flags, const char *regexp)
{
    if(!r || !regexp) 
	return -1;

    regex_t temp;

    if(regcomp(&temp, regexp, REG_EXTENDED | REG_NOSUB))
	return -1;

    regfree(&temp);

    ALLOC_CONDITION(condition_virus_match, 
		    sizeof(condition_virus_match)+strlen(regexp), 
		    CONDITION_VIRUS_MATCH);

    strcpy(cond->str_regexp, regexp);

    regcomp(&(cond->regexp), regexp, REG_EXTENDED | REG_NOSUB);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_dcc_match(rule *r, unsigned int flags, const char *body, const char *fuz1, const char *fuz2)
{
    if(!r) 
	return -1;

    if(!fuz1)
        fuz1 = "";
    if(!fuz2)
        fuz2 = "";
    if(!body)
        body = "";
    
    ALLOC_CONDITION(condition_dcc_match, 
		    sizeof(condition_dcc_match)+strlen(body)+strlen(fuz1)+strlen(fuz2), 
		    CONDITION_DCC_MATCH);

    strcpy(cond->body_fuz1_fuz2, body);
    strcpy(strchr(cond->body_fuz1_fuz2, '\0')+1, fuz1);
    strcpy(strchr(strchr(cond->body_fuz1_fuz2, '\0')+1, '\0')+1, fuz2);

    ADD_CONDITION;

    return 0;
}

int rule_add_condition_bayes_match(rule *r, unsigned int flags, unsigned int rate)
{
    if(!r || (rate > 100))
	return -1;

    ALLOC_CONDITION(condition_bayes_match, 
		    sizeof(condition_bayes_match), 
		    CONDITION_BAYES_MATCH);

    cond->rate = rate;

    ADD_CONDITION;

    return 0;
}

/*******************************************************************************
 *
 * Rules (actions)
 *
 *******************************************************************************/

#define ALLOC_ACTION(x, s, id)				\
   struct x *act = (x *)malloc(s);			\
   const char *ACTION_NAME = #id;                       \
   if(act == NULL) return -1;				\
   memset(act, 0, (s));					\
   act->hdr.size = (s) - sizeof(action_struct_header);	\
   act->hdr.type = id; 
   

#define ADD_ACTION										\
do {												\
    if(r->actions.array)									\
	{											\
	    r->actions.array = (action **)realloc(r->actions.array,				\
							 sizeof(action *)*(r->actions.count+1));	\
	    r->actions.array[r->actions.count] = (action *)act;				\
	    r->actions.count++;								\
	}											\
    else											\
	{											\
	    r->actions.array = (action **)malloc(sizeof(action *));				\
	    r->actions.array[0] = (action *)act;						\
	    r->actions.count = 1;								\
	}											\
} while(0)

int rule_add_action_accept(rule *r)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_accept,
		 sizeof(action_accept),
		 ACTION_ACCEPT);

    ADD_ACTION;

    return 0;
}

int rule_add_action_blackhole(rule *r)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_blackhole,
		 sizeof(action_blackhole),
		 ACTION_BLACKHOLE);

    ADD_ACTION;

    return 0;
}

int rule_add_action_reject(rule *r)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_reject,
		 sizeof(action_reject),
		 ACTION_REJECT);

    ADD_ACTION;

    return 0;
}

int rule_add_action_bounce(rule *r)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_bounce,
		 sizeof(action_bounce),
		 ACTION_BOUNCE);

    ADD_ACTION;

    return 0;
}

int rule_add_action_skip(rule *r)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_skip,
		 sizeof(action_skip),
		 ACTION_SKIP);

    ADD_ACTION;

    return 0;
}

int rule_add_action_send(rule *r, unsigned int type, size_t count, char **rcpts)
{
    if(!r || !rcpts)
	return -1;

    size_t all_size = 0, i;
    char *p;

    for(i = 0; i < count; i++)
	all_size += strlen(rcpts[i])+1;

    ALLOC_ACTION(action_send,
		 sizeof(action_send) + all_size - 1,
		 ACTION_SEND);

    act->type = type;
    act->count = count;

    p = act->rcpts;

    for(i = 0; i < count; i++)
	{
	    strcpy(p, rcpts[i]);
	    p = strchr(p, '\0')+1;
	}

    ADD_ACTION;

    return 0;
}

int rule_add_action_syslog(rule *r, unsigned int mask, const char *message)
{
    if(!r || !message)
	return -1;

    size_t all_size = 0;

    all_size += strlen(message)+1;

    ALLOC_ACTION(action_syslog,
		 sizeof(action_syslog) + all_size - 1,
		 ACTION_SYSLOG);

    act->mask = mask;
    strcpy(act->message, message);

    ADD_ACTION;

    return 0;
}

int rule_add_action_header_change(rule *r, const char *header, const char *value)
{
    if(!r || !header || !value)
	return -1;

    ALLOC_ACTION(action_header_change,
		 sizeof(action_header_change) + strlen(header) + strlen(value),
		 ACTION_HEADER_CHANGE);

    strcpy(act->header_value, header);
    strcpy(act->header_value + strlen(header) + 1, value);

    ADD_ACTION;

    return 0;
}

int rule_add_action_header_add(rule *r, const char *header, const char *value)
{
    if(!r || !header || !value)
	return -1;

    ALLOC_ACTION(action_header_add,
		 sizeof(action_header_add) + strlen(header) + strlen(value),
		 ACTION_HEADER_ADD);

    strcpy(act->header_value, header);
    strcpy(act->header_value + strlen(header) + 1, value);

    ADD_ACTION;

    return 0;
}

int rule_add_action_header_prepend(rule *r, const char *header, const char *value)
{
    if(!r || !header || !value)
	return -1;

    ALLOC_ACTION(action_header_prepend,
		 sizeof(action_header_prepend) + strlen(header) + strlen(value),
		 ACTION_HEADER_PREPEND);

    strcpy(act->header_value, header);
    strcpy(act->header_value + strlen(header) + 1, value);

    ADD_ACTION;

    return 0;
}

int rule_add_action_header_new(rule *r, const char *header, const char *value)
{
    if(!r || !header || !value)
	return -1;

    ALLOC_ACTION(action_header_new,
		 sizeof(action_header_new) + strlen(header) + strlen(value),
		 ACTION_HEADER_NEW);

    strcpy(act->header_value, header);
    strcpy(act->header_value + strlen(header) + 1, value);

    ADD_ACTION;

    return 0;
}

int rule_add_action_header_remove(rule *r, const char *header)
{
    if(!r || !header)
	return -1;

    ALLOC_ACTION(action_header_remove,
		 sizeof(action_header_remove) + strlen(header),
		 ACTION_HEADER_REMOVE);

    strcpy(act->header, header);

    ADD_ACTION;

    return 0;
}

int rule_add_action_register_set(rule *r, unsigned int number, int quantity)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_register_set,
		 sizeof(action_register_set),
		 ACTION_REGISTER_SET);

    act->number   = number;
    act->quantity = quantity;

    ADD_ACTION;

    return 0;
}

int rule_add_action_register_add(rule *r, unsigned int number, int quantity)
{
    if(!r)
	return -1;

    ALLOC_ACTION(action_register_add,
		 sizeof(action_register_add),
		 ACTION_REGISTER_ADD);

    act->number   = number;
    act->quantity = quantity;

    ADD_ACTION;

    return 0;
}

int rule_add_action_send_notification(rule *r, const char *entry)
{
    if(!r || !entry)
	return -1;

    ALLOC_ACTION(action_send_notification,
		 sizeof(action_send_notification) + strlen(entry),
		 ACTION_SEND_NOTIFICATION);

    strcpy(act->entry, entry);

    ADD_ACTION;

    return 0;
}

int rule_add_action_send_log_entry(rule *r, const char *entry)
{
    if(!r || !entry)
	return -1;

    ALLOC_ACTION(action_send_log_entry,
		 sizeof(action_send_log_entry) + strlen(entry),
		 ACTION_SEND_LOG_ENTRY);

    strcpy(act->entry, entry);

    ADD_ACTION;

    return 0;
}

int rule_add_action_dcc_report(rule *r, const char *count)
{
    if(!r)
	return -1;

    if(!count)
        count = "";

    ALLOC_ACTION(action_dcc_report,
		 sizeof(action_dcc_report) + strlen(count),
		 ACTION_DCC_REPORT);

    strcpy(act->count, count);

    ADD_ACTION;

    return 0;
}

const char *profile_get_name(profile *p)
{
    if(!p)
	return NULL;

    return p->name;
}

int profile_iterate(profile *p, profile_iterate_callbacks *pic, void *pdata)
{
    if(!p || !pic)
	return 1;

    size_t i, j;
    condition *cur_condition;
    action *cur_action;
    int check_result, cond_result, exec_result;

#define END_OF_PROCESSING 			\
    do {					\
	return 1;				\
    } while(0)

    for(i = 0; i < p->rules.count; i++)
	{
	    if(pic->rule_begin)
		pic->rule_begin(pdata);

	    cond_result = 1;

	    for(j = 0; j < p->rules.array[i].conditions.count && cond_result; j++)
		{
		    cur_condition = p->rules.array[i].conditions.array[j];

		    check_result = 0;

		    switch(cur_condition->hdr.type)
			{
			case CONDITION_SMTP_FROM:
			    
			    if(pic->condition.smtp_from)
				check_result = pic->condition.smtp_from(pdata, ((condition_smtp_from *)cur_condition)->email);

			    break;
			case CONDITION_SMTP_FROM_LIST:

			    if(pic->condition.smtp_from_list)
				check_result = pic->condition.smtp_from_list(pdata, ((condition_smtp_from_list *)cur_condition)->lid);

			    break;
			case CONDITION_SMTP_TO:

			    if(pic->condition.smtp_to)
				check_result = pic->condition.smtp_to(pdata, ((condition_smtp_to *)cur_condition)->email);

			    break;
			case CONDITION_SMTP_TO_LIST:

			    if(pic->condition.smtp_to_list)
				check_result = pic->condition.smtp_to_list(pdata, ((condition_smtp_to_list *)cur_condition)->lid);

			    break;
			case CONDITION_HAS_HEADER:

			    if(pic->condition.has_header)
				check_result = pic->condition.has_header(pdata, ((condition_has_header *)cur_condition)->header);

			    break;
			case CONDITION_HEADER_MATCH:

			    if(pic->condition.header_match)
				check_result = pic->condition.header_match(pdata, ((condition_header_match *)cur_condition)->header_regexp, 
									   strchr( ((condition_header_match *)cur_condition)->header_regexp, '\0')+1,
									   &(((condition_header_match *)cur_condition)->regexp));

			    break;
			case CONDITION_CATEGORY_MATCH:

			    if(pic->condition.category_match)
				check_result = pic->condition.category_match(pdata, ((condition_category_match *)cur_condition)->cat_id);

			    break;
			case CONDITION_MSG_SIZE:

			    if(pic->condition.msg_size)
				check_result = pic->condition.msg_size(pdata, ((condition_msg_size *)cur_condition)->limit);

			    break;
			case CONDITION_IP_FROM_LIST:

			    if(pic->condition.ip_from_list)
				check_result = pic->condition.ip_from_list(pdata, ((condition_ip_from_list *)cur_condition)->lid);

			    break;
			case CONDITION_IP_FROM:

			    if(pic->condition.ip_from)
				check_result = pic->condition.ip_from(pdata, ((condition_ip_from *)cur_condition)->addr, 
								      ((condition_ip_from *)cur_condition)->mask);

			    break;
			case CONDITION_IP_FROM_RBL:

			    if(pic->condition.ip_from_rbl)
				check_result = pic->condition.ip_from_rbl(pdata, ((condition_ip_from_rbl *)cur_condition)->lid);

			    break;
			case CONDITION_IP_FROM_NOT_IN_DNS:

			    if(pic->condition.ip_from_not_in_dns)
				check_result = pic->condition.ip_from_not_in_dns(pdata);

			    break;
			case CONDITION_REGISTER:

			    if(pic->condition.check_register)
				check_result = pic->condition.check_register(pdata, ((condition_register *)cur_condition)->number, 
									     ((condition_register *)cur_condition)->type,
									     ((condition_register *)cur_condition)->quantity);
                            
                            break;

			case CONDITION_ATTRIBUTE_MATCH:

			    if(pic->condition.attribute_match)
				check_result = pic->condition.attribute_match(pdata, ((condition_attribute_match *)cur_condition)->attribute_regexp, 
                                                                              strchr( ((condition_attribute_match *)cur_condition)->attribute_regexp, '\0')+1,
                                                                              &(((condition_attribute_match *)cur_condition)->regexp));

			    break;

			case CONDITION_CONTENT_TYPE_MATCH:

			    if(pic->condition.content_type_match)
				check_result = pic->condition.content_type_match(pdata, ((condition_content_type_match *)cur_condition)->str_regexp, 
                                                                                 &(((condition_content_type_match *)cur_condition)->regexp));

			    break;

			case CONDITION_FILE_NAME_MATCH:

			    if(pic->condition.file_name_match)
				check_result = pic->condition.file_name_match(pdata, ((condition_file_name_match *)cur_condition)->str_regexp, 
                                                                              &(((condition_file_name_match *)cur_condition)->regexp));

			    break;

			case CONDITION_HAS_ATTACHMENTS:

			    if(pic->condition.has_attachments)
				check_result = pic->condition.has_attachments(pdata);

			    break;

			case CONDITION_UNKNOWN_CONTENT:

			    if(pic->condition.unknown_content)
				check_result = pic->condition.unknown_content(pdata);

			    break;

			case CONDITION_VIRUS_MATCH:

			    if(pic->condition.virus_match)
				check_result = pic->condition.virus_match(pdata, ((condition_virus_match *)cur_condition)->str_regexp, 
                                                                          &(((condition_virus_match *)cur_condition)->regexp));

			    break;

			case CONDITION_DCC_MATCH:
                        {
                            char *body = NULL;
                            char *fuz1 = NULL;
                            char *fuz2 = NULL;

                            body = ((condition_dcc_match *)cur_condition)->body_fuz1_fuz2;
                            fuz1 = strchr(body, '\0') + 1;
                            fuz2 = strchr(fuz1, '\0') + 1;

                            if(!*body) body = NULL;
                            if(!*fuz1) fuz1 = NULL;
                            if(!*fuz2) fuz2 = NULL;

			    if(pic->condition.dcc_match)
				check_result = pic->condition.dcc_match(pdata, body, fuz1, fuz2);

			    break;
                        }

            case CONDITION_BAYES_MATCH:
                
                if(pic->condition.bayes_match)
				check_result = pic->condition.bayes_match(pdata, ((condition_bayes_match *)cur_condition)->rate);

                break;

			default:
			    /* XXX assert? */
			    cond_result = 0;
			}

		   // logger_printf(LOGGER_DEBUG, "Check of condition: %s.", cond_result ? "yes" : "no");

		    cond_result = cond_result && (cur_condition->hdr.flags & CONDITION_NOT ? !check_result : check_result);
		}

	    if(cond_result)
		{
#ifdef WITH_LOGDEBUG_LEV2
            logger_printf(LOGGER_DEBUG,"  then");
#endif //WITH_LOGDEBUG_LEV2
		    for(j = 0; j < p->rules.array[i].actions.count; j++)
			{
			    cur_action = p->rules.array[i].actions.array[j];

			    exec_result = 1;

			    switch(cur_action->hdr.type)
				{
				case ACTION_ACCEPT:

				    if(pic->action.accept)
					exec_result = pic->action.accept(pdata);
				    
				    break;
				case ACTION_REJECT:

				    if(pic->action.reject)
					exec_result = pic->action.reject(pdata);

				    break;
				case ACTION_BLACKHOLE:

				    if(pic->action.blackhole)
					exec_result = pic->action.blackhole(pdata);

				    break;
				case ACTION_BOUNCE:

				    if(pic->action.bounce)
					exec_result = pic->action.bounce(pdata);

				    break;
				case ACTION_SKIP:

				    if(pic->action.skip)
					exec_result = pic->action.skip(pdata);

				    break;
				case ACTION_SEND:

				    if(pic->action.send)
					exec_result = pic->action.send(pdata, ((action_send *)cur_action)->type, ((action_send *)cur_action)->count,
								       ((action_send *)cur_action)->rcpts);

				    break;
				case ACTION_SYSLOG:

				    if(pic->action.syslog)
					exec_result = pic->action.syslog(pdata, ((action_syslog *)cur_action)->mask, ((action_syslog *)cur_action)->message);

				    break;
				case ACTION_HEADER_CHANGE:

				    if(pic->action.header_change)
					exec_result = pic->action.header_change(pdata, ((action_header_change *)cur_action)->header_value, 
										strchr(((action_header_change *)cur_action)->header_value, '\0') + 1);

				    break;
				case ACTION_HEADER_ADD:

				    if(pic->action.header_add)
					exec_result = pic->action.header_add(pdata, ((action_header_add *)cur_action)->header_value, 
									     strchr(((action_header_add *)cur_action)->header_value, '\0') + 1);

				    break;
				case ACTION_HEADER_PREPEND:

				    if(pic->action.header_prepend)
					exec_result = pic->action.header_prepend(pdata, ((action_header_prepend *)cur_action)->header_value, 
										 strchr(((action_header_prepend *)cur_action)->header_value, '\0') + 1);

				    break;
				case ACTION_HEADER_NEW:

				    if(pic->action.header_new)
					exec_result = pic->action.header_new(pdata, ((action_header_new *)cur_action)->header_value, 
									     strchr(((action_header_new *)cur_action)->header_value, '\0') + 1);

				    break;
				case ACTION_HEADER_REMOVE:

				    if(pic->action.header_remove)
					exec_result = pic->action.header_remove(pdata, ((action_header_remove *)cur_action)->header);

				    break;

				case ACTION_REGISTER_SET:

				    if(pic->action.register_set)
					exec_result = pic->action.register_set(pdata, ((action_register_set *)cur_action)->number,
									       ((action_register_set *)cur_action)->quantity);

				    break;

				case ACTION_REGISTER_ADD:

				    if(pic->action.register_add)
					exec_result = pic->action.register_add(pdata, ((action_register_add *)cur_action)->number,
									       ((action_register_add *)cur_action)->quantity);

				    break;

				case ACTION_SEND_NOTIFICATION:

				    if(pic->action.send_notification)
					exec_result = pic->action.send_notification(pdata, ((action_send_notification *)cur_action)->entry);
				    break;

				case ACTION_SEND_LOG_ENTRY:

				    if(pic->action.send_log_entry)
					exec_result = pic->action.send_log_entry(pdata, ((action_send_log_entry *)cur_action)->entry);

				    break;

				case ACTION_DCC_REPORT:
                                {
                                    char *count = NULL;

                                    count = ((action_dcc_report *)cur_action)->count;
                                    if(!*count) count = NULL;


				    if(pic->action.dcc_report)
					exec_result = pic->action.dcc_report(pdata, count);

				    break;
                                }

				default:
				    /* XXX assert? */

				    ;
				    
				}

			    if(exec_result)
				END_OF_PROCESSING;
			}
		}
	}

#undef END_OF_PROCESSING

    return 0;
}

unsigned int profile_get_type(profile *p)
{
    if(!p)
	return PROFILE_NONE;

    return p->type;
}

int profile_check_rcpt(profile *p, list_db *ldb, const char *rcpt, size_t rcpt_len)
{
    if(!p || !rcpt || rcpt_len == 0)
	return 0;

    if(p->rcpts.count == 0 && p->rcpts.lid == LIST_ID_NONE)
	{
	    return 1;
	}
#ifndef KIS_USAGE
    else if(p->rcpts.lid)
	{
	    return (list_email_check(ldb, p->rcpts.lid, rcpt, rcpt_len) == LIST_CHECK_EXIST);
	}
    else
	{
	    for(size_t i = 0; i < p->rcpts.count; i++)
		{
		    if(p->rcpts.array[i][0] == '@')
			{
			    const char *ptr = (const char *)memchr(rcpt, '@', rcpt_len);
			    if(!ptr)
				continue;

			    if(strlen(p->rcpts.array[i]) != rcpt_len - (ptr-rcpt))
				continue;

			    if(strncasecmp(p->rcpts.array[i], ptr, rcpt_len - (ptr-rcpt)) == 0)
				return 1;
			}
		    else
			{
			    if(strlen(p->rcpts.array[i]) == rcpt_len && strncasecmp(p->rcpts.array[i], rcpt, rcpt_len) == 0)
				return 1;
			}
		}
	}
#endif//KIS_USAGE

    return 0;
}


/*
 * <eof profile.cpp>
 */
