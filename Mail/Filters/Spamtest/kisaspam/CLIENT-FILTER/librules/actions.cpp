/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: actions.cpp                                                           *
 * Created: Thu May  9 14:37:58 2002                                           *
 * Desc: Actions on envelope                                                   *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  actions.cpp
 * \brief Actions on envelope
 * $Id: actions.cpp,v 1.5 2005/09/16 15:28:47 esmirnov Exp $
 * $Log: actions.cpp,v $
 * Revision 1.5  2005/09/16 15:28:47  esmirnov
 * KIS_USAGE
 *
 * Revision 1.4  2005/06/07 07:05:27  vtroitsky
 * Global debug buffer was changed into local one
 *
 * Revision 1.3  2005/02/01 11:32:13  vtroitsky
 * Headers macros expansion
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
#include <wintypes.h>
//#include "StdAfx.h"
#endif //_WIN32

/*#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>*/

#ifdef USE_CUSTOM_HEAP
#include "mem_heap.h"
#endif


#ifdef WITH_SCORES
#include <scores.h>
#endif

#ifdef CONFIDENTIAL
#include <arc-extractor.h>
#endif /* CONFIDENTIAL */

#include "actions.h"
#include "interpreter.h"

#include "smtp-filter/filter-config/xmlfiles/keywords.h"

#ifndef _WIN32
#include "smtp-filter/common/mergesort.h"
#else
#include "smtp-filter/common/merge.h"
#endif //_WIN32

#include "smtp-filter/common/logger.h"


/*#ifdef _WIN32
#include "win32-filter/include/exchange_scmif.h"
#include "win32-filter/module/filter_server/thread_logger.h"
#endif*/

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif


/*
 * XXX Note, that macros in actions.h must use this constant too. But
 * they did not use this...
 */
#define BITS_PER_UINT 32

static void action_recycle(action *a)
{
    a->headers.used = 0;
    a->type = 0;
    ACTION_SET_TYPE(a, ACTION_ACCEPT);
    memset(a->rcpts, 0, a->rcpts_size*sizeof(unsigned int));
    memset(a->registers.values, 0, sizeof(a->registers.values));
    
    if(a->headers.alloc)
	memset(a->headers.array, 0, sizeof(header_modify)*a->headers.alloc);
}

static action *action_create(size_t rcpts_count)
{
    size_t delta = 0;

    /*
     * XXX Here (and after this code in this file) we must use some constants 
     * about hardware.
     */
    if(rcpts_count > 4*BITS_PER_UINT)
	delta = (rcpts_count - 4*BITS_PER_UINT) / BITS_PER_UINT + 1;

    action *res = (action *)malloc(sizeof(action) + delta*sizeof(unsigned int));
    memset(res, 0, sizeof(action) + delta*sizeof(unsigned int));

    res->rcpts_size = 4 + delta;

    res->type = 0;
    ACTION_SET_TYPE(res, ACTION_ACCEPT);

    return res;
}

static void action_destroy(action *a)
{
    if(a->headers.array)
	free(a->headers.array);

    free(a);
}

static action *action_realloc(action *a, size_t rcpts_count)
{
    size_t delta = 0;

    if(rcpts_count > 4*BITS_PER_UINT)
	delta = (rcpts_count - 4*BITS_PER_UINT) / BITS_PER_UINT + 1;

    if(4 + delta <= a->rcpts_size)
	return a;
    else
	{
	    action *res = (action *)realloc(a, sizeof(action) + delta*sizeof(unsigned int));
	    memset(res->rcpts + res->rcpts_size, 0, (delta + 4 - res->rcpts_size)*sizeof(unsigned int));
	    res->rcpts_size = 4 + delta;

	    return res;
	}
}

action_storage *action_storage_init(action_storage *as, envelope *e)
{
    memset(as, 0, sizeof(action_storage));
    as->e             = e;
    as->common_action = action_create(e->rcpts.used);

    as->rcpts.alloc = (e->rcpts.used / 10 + 1)*10;
    as->rcpts.array = (action_rcpt *)malloc(as->rcpts.alloc*sizeof(action_rcpt));
    memset(as->rcpts.array, 0, as->rcpts.alloc*sizeof(action_rcpt));

    for(as->rcpts.used = 0; as->rcpts.used < e->rcpts.used; as->rcpts.used++)
	{
	    as->rcpts.array[as->rcpts.used].rcpt = e->rcpts.array[as->rcpts.used].email.text;
	    as->rcpts.array[as->rcpts.used].length = e->rcpts.array[as->rcpts.used].email.length;
	}

    for(size_t i = 0; i < e->rcpts.used; i++)
	ACTION_RCPT_SET(as->common_action, i);
    
    ACTION_SET_TYPE(as->common_action, ACTION_ACCEPT);

    return as;
}

int action_storage_recycle(action_storage *as, envelope *e)
{
    as->e = e;
    as->actions.used = 0;


    if(as->rcpts.alloc < e->rcpts.used)
	{
	    as->rcpts.array = (action_rcpt *)realloc(as->rcpts.array,  (as->rcpts.alloc = (e->rcpts.used / 10 + 1)*10)*sizeof(action_rcpt));
	    memset(as->rcpts.array, 0, as->rcpts.alloc*sizeof(action_rcpt));
	}

    for(as->rcpts.used = 0; as->rcpts.used < e->rcpts.used; as->rcpts.used++)
	{
	    as->rcpts.array[as->rcpts.used].rcpt   = e->rcpts.array[as->rcpts.used].email.text;
	    as->rcpts.array[as->rcpts.used].length = e->rcpts.array[as->rcpts.used].email.length;
	    as->rcpts.array[as->rcpts.used].flags  = 0UL;
	}

    action_recycle(as->common_action);
    as->common_action = action_realloc(as->common_action, e->rcpts.used);

	size_t i;
    for(i = 0; i < as->actions.alloc; i++)
	{
	    if(as->actions.array[i])
		action_recycle(as->actions.array[i]);
	}
    
	//for(size_t i = 0; i < e->rcpts.used; i++)
    for(i = 0; i < e->rcpts.used; i++)
	ACTION_RCPT_SET(as->common_action, i);

    ACTION_SET_TYPE(as->common_action, ACTION_ACCEPT);

    return 0;
}

void action_storage_destroy(action_storage *as)
{
    if(as->actions.array)
	{
	    for(size_t i = 0; i < as->actions.alloc; i++)
	    {
	        if(as->actions.array[i])
		    action_destroy(as->actions.array[i]);
	    }

	    free(as->actions.array);
	}

//vvt: To avoid memory leak!
	if(as->rcpts.array)
		free(as->rcpts.array);
//vvt: End of addition

    action_destroy(as->common_action);
}


action *action_storage_action_clone(action_storage *as, action *a)
{
    action *res = NULL;

    if(as->actions.used == as->actions.alloc)
	{
	    if(as->actions.array)
		as->actions.array = (action **)realloc(as->actions.array, sizeof(action *)*(as->actions.alloc += 10));
	    else
		as->actions.array = (action **)malloc(sizeof(action *)*(as->actions.alloc = 20));

	    memset(as->actions.array + as->actions.used, 0, (as->actions.alloc - as->actions.used)*sizeof(action *));
	}

    if(as->actions.array[as->actions.used])
	as->actions.array[as->actions.used] = action_realloc(as->actions.array[as->actions.used], a->rcpts_size*BITS_PER_UINT);
    else
	as->actions.array[as->actions.used] = action_create(a->rcpts_size*BITS_PER_UINT);
	
    res = as->actions.array[as->actions.used];

    /* memcpy(res->rcpts, a->rcpts, a->rcpts_size*sizeof(unsigned int)); */
    memset(res->rcpts, 0, a->rcpts_size*sizeof(unsigned int));

    if(res->headers.alloc < a->headers.used)
	{
	    if(res->headers.array == NULL)
		res->headers.array = (header_modify *)malloc((res->headers.alloc = (a->headers.used / 10 + 1)*10)*sizeof(header_modify));
	    else
		res->headers.array = (header_modify *)realloc(res->headers.array, 
							      (res->headers.alloc = (a->headers.used / 10 + 1)*10)*sizeof(header_modify));

	    memset(res->headers.array, 0, res->headers.alloc*sizeof(header_modify));
	}

    res->headers.used = a->headers.used;

    if(a->headers.used)
	memcpy(res->headers.array, a->headers.array, a->headers.used*sizeof(header_modify));

    memcpy(res->registers.values, a->registers.values, sizeof(a->registers.values));

    as->actions.used++;

    return res;
}

int action_storage_rcpt_add(action_storage *as, const char *rcpt, size_t length)
{
    int res = as->rcpts.used;

    if(as->rcpts.used == as->rcpts.alloc)
	{
	    if(as->rcpts.array)
		as->rcpts.array = (action_rcpt *)realloc(as->rcpts.array, (as->rcpts.alloc += 10)*sizeof(action_rcpt));
	    else
		as->rcpts.array = (action_rcpt *)malloc((as->rcpts.alloc = 20)*sizeof(action_rcpt));
		
	    memset(as->rcpts.array + as->rcpts.used, 0, (as->rcpts.alloc - as->rcpts.used)*sizeof(action_rcpt));
	}

    as->rcpts.array[res].rcpt    = rcpt;
    as->rcpts.array[res].length = length;
    as->rcpts.used++;

    return res;
}

int action_header_modify(action *a, unsigned int type, const char *header, const char *value)
{
    if(a->headers.used == a->headers.alloc)
	{
	    if(a->headers.array)
		a->headers.array = (header_modify *)realloc(a->headers.array, (a->headers.alloc += 10)*sizeof(header_modify));
	    else
		a->headers.array = (header_modify *)malloc((a->headers.alloc = 20)*sizeof(header_modify));

	    memset(a->headers.array + a->headers.used, 0, (a->headers.alloc - a->headers.used)*sizeof(header_modify));		
	}

    int pos = a->headers.used;

    a->headers.array[pos].type   = type;
    a->headers.array[pos].header = header;
    a->headers.array[pos].value  = value;

    a->headers.used++;

    return pos;
}

action *action_storage_action_realloc(action_storage *as, action *a, size_t needed_rcpts)
{
    return action_realloc(a, needed_rcpts);
}


static action         *sort_action;
static action_storage *sort_action_storage;

/*
 * This compare function sorts array before normalize it.
 */
static int compare_header_modify(const void *a, const void *b)
{
    const header_modify *hm_a = (const header_modify *)a;
    const header_modify *hm_b = (const header_modify *)b;

    int res = strcasecmp(hm_a->header, hm_b->header);

    return res;
}

/*
 * This compare function sorts already normalized array to make order
 * in change/delete-prepend-add header actions.
 */
static int compare_header_modify_final(const void *a, const void *b)
{
    const header_modify *hm_a = (const header_modify *)a;
    const header_modify *hm_b = (const header_modify *)b;

    int res = strcasecmp(hm_a->header, hm_b->header);

    if(res == 0)
	{
	    if(hm_a->type < hm_b->type)
		res = -1;
	    else if(hm_a->type > hm_b->type)
		res = 1;
	}

    return res;
}

static int headers_reverse(header_modify *array, int first, int last)
{
    header_modify tmp;

    for( ; ; )
	{
	    if(first >= last)
		return 0;

	    memcpy(&tmp, array + first, sizeof(header_modify));
	    memcpy(array + first, array + last, sizeof(header_modify));
	    memcpy(array + last, &tmp, sizeof(header_modify));

	    first++;
	    last--;
	}
}

int action_normalize(action_storage *as, action *a, unsigned int flags)
{
    size_t i;

    if(ACTION_GET_TYPE(a) == ACTION_NONE)
	return 1;

    for(i = 0; i < a->rcpts_size; i++)
	{
	    if(a->rcpts[i] != 0UL)
		break;
	}

    if(i == a->rcpts_size)
	{
	    ACTION_SET_TYPE(a, ACTION_NONE);
	    ACTION_CLEAR_BOUNCE(a);
	    return 0;
	}

    if(ACTION_GET_TYPE(a) == ACTION_REJECT || ACTION_GET_TYPE(a) == ACTION_BLACKHOLE)
	{
	    a->headers.used = 0;
	}
    else 
	{
	    if(a->headers.used)
		{
		    sort_action_storage = as;
		    sort_action         = a;

		    /*
		     * Mergesort choosed from stable property of this sorting function.
		     */
		    local_mergesort(a->headers.array, a->headers.used, sizeof(header_modify),
				    compare_header_modify);

		    size_t cur = 0, next = 1;

		    for(i = 1; i < a->headers.used; i++)
			{
			    if(strcasecmp(a->headers.array[cur].header, a->headers.array[i].header) != 0)
				{
				    cur  = next;
				}
			    else
				{
				    if(a->headers.array[i].type == HEADER_MODIFY_CHANGE || 
				       a->headers.array[i].type == HEADER_MODIFY_REMOVE)
					{
					    next = cur;
					}
				}

			    if(next != i)
				memcpy(a->headers.array + next, a->headers.array + i, sizeof(header_modify));

			    next++;
			}

		    a->headers.used = next;


                    if(!(flags & ACTIONS_STORAGE_NORMALIZE_LIBSPAMTEST))
                        {
                            local_mergesort(a->headers.array, a->headers.used, sizeof(header_modify),
                                            compare_header_modify_final);

		    
                            int first = -1, last = -1;
                            
                            for(i = 0; i < a->headers.used; i++)
                                {
                                    if(first >= 0)
                                        {
                                            if(strcasecmp(a->headers.array[i].header, a->headers.array[first].header) == 0 &&
                                               a->headers.array[i].type == HEADER_MODIFY_PREPEND)
                                                last = i;
                                            else
                                                {
                                                    headers_reverse(a->headers.array, first, last);
                                                    last = -1;
                                                    first = -1;
                                                }
                                        }
                                    else
                                        if(a->headers.array[i].type == HEADER_MODIFY_PREPEND)
                                            {
                                                last = i;
                                                first = i;
                                            }
                                }
                            
                            if(first >= 0 && last >= 0)
                                {
                                    headers_reverse(a->headers.array, first, last);
                                    last = -1;
                                    first = -1;
                                }
                        }

                    for(i = 1; i < a->headers.used; i++)
                        {
                            if(strcasecmp(a->headers.array[i].header, a->headers.array[i-1].header) == 0)
                                {
                                    a->headers.array[i].flags |= HEADER_MODIFY_EQUAL_TO_PREV;
                                }
                            else
                                {
                                    /* nothing */
                                }
			}

		}
	}

    return 0;
}

int action_storage_normalize(action_storage *as, unsigned int flags)
{
    size_t i, j, k;

    action_normalize(as, as->common_action, flags);

    for(i = 0; i < as->actions.used; i++)
	action_normalize(as, as->actions.array[i], flags);

    i = 0;

    action *current = as->common_action;

    for( ; ; )
	{
	    if(ACTION_GET_TYPE(current) != ACTION_NONE)
		{
		    for(j = 0; j < as->actions.used; j++)
			{
			    if(current == as->actions.array[j] || 
			       ACTION_GET_TYPE(as->actions.array[j]) == ACTION_NONE)
				continue;

			    if(current->type == as->actions.array[j]->type && 
			       current->headers.used == as->actions.array[j]->headers.used)
				{
				    for(k = 0; k < current->headers.used; k++)
					{
					    /*
					     * XXX Potential --- strcmp on value not correct in case, then result of macro
					     * expanding depends on user or user group (for example, ${CATEGORY} may depend
					     * on users categories).
					     *
					     * And here we suggest, that count of actions and headers modifies is not
					     * big. 
					     */

					    if(current->headers.array[k].type == as->actions.array[j]->headers.array[k].type &&
					       strcasecmp(current->headers.array[k].header, as->actions.array[j]->headers.array[k].header) == 0 &&
					       strcmp(current->headers.array[k].value, as->actions.array[j]->headers.array[k].value) == 0)
						{
						    /* NONE */
						}
					    else
						goto action_end;
					}

				    if(current->rcpts_size < as->actions.array[j]->rcpts_size)
					{
					    if(current == as->common_action)
						{
						    as->common_action = action_realloc(as->common_action, as->actions.array[j]->rcpts_size);
						    current = as->common_action;
						}
					    else
						goto action_end;
					}

				    for(k = 0; k < as->actions.array[j]->rcpts_size; k++)
					current->rcpts[k] |= as->actions.array[j]->rcpts[k];

				    ACTION_SET_TYPE(as->actions.array[j], ACTION_NONE);
				    ACTION_CLEAR_BOUNCE(as->actions.array[j]);

				}

			  action_end:
			    ;
			}
		}

	    if(i >= as->actions.used)
		break;

	    current = as->actions.array[i++];
	    /* action_normalize(as, current); */
	}

    if(ACTION_GET_TYPE(as->common_action) == ACTION_NONE)
	{
	    size_t counter = 0;
	    j = 0;
	    
	    for(i = 0; i < as->actions.used; i++)
		if(ACTION_GET_TYPE(as->actions.array[i]) != ACTION_NONE)
		    {
			j = i;
			counter++;
		    }

	    if(counter == 1)
		{
		    action *temp         = as->common_action;

		    as->common_action    = as->actions.array[j];
		    as->actions.array[j] = temp;
		}
	}

    return 0;
}

#ifndef KIS_USAGE

#define ENCODE_QUOTED 0x00000001UL
#define ENCODE_HEADER "=?windows-1251?Q?"
#define ENCODE_FOOTER "?="

static size_t encoded_text_preprocess(const char *str, unsigned int &flags)
{
    flags = 0UL;

    const size_t delta = sizeof(ENCODE_HEADER)-1 + sizeof(ENCODE_FOOTER)-1;
    size_t res = 0;

    for(const char *p = str; *p; p++)
	{
	    if((*p >= 33 && *p <= 60) ||
	       (*p >= 62 && *p <= 126))
		res++;
	    else
		{
		    if(!(flags & ENCODE_QUOTED))
			{
			    flags |= ENCODE_QUOTED;
			    res += delta;
			}
		    else
			res += 3;
		}
	}

    return res;
}

static size_t encode_text(const char *str, char *out, unsigned int flags)
{
    char *out_ptr = out;

    if(flags & ENCODE_QUOTED)
	{
	    memcpy(out_ptr, ENCODE_HEADER, sizeof(ENCODE_HEADER)-1);
	    out_ptr += sizeof(ENCODE_HEADER)-1;
	}

    const char *in_ptr = str;
    unsigned int code;

    for( ; *in_ptr; in_ptr++)
	{
	    if((*in_ptr >= 33 && *in_ptr <= 60) ||
	       (*in_ptr >= 62 && *in_ptr <= 126))
		{
		    *(out_ptr++) = *in_ptr;
		}
	    else
		{
		    /* 
		     * ENCODE.
		     */

		    *(out_ptr++) = '=';

		    code = ((unsigned char)(*in_ptr)) >> 4;
		    *(out_ptr++) = code >= 10 ? code - 10 + 'A' : code + '0';

		    code = ((unsigned char)(*in_ptr)) & 0x0FU;
		    *(out_ptr++) = code >= 10 ? code - 10 + 'A' : code + '0';
		}
	}

    if(flags & ENCODE_QUOTED)
	{
	    memcpy(out_ptr, ENCODE_FOOTER, sizeof(ENCODE_FOOTER)-1);
	    out_ptr += sizeof(ENCODE_FOOTER)-1;
	}


    return (out_ptr - out);
}

#undef ENCODE_QUOTED

static char *print_category_path(const category *cat, char *out)
{
    if(cat)
	{
	    char *p = print_category_path(cat->parent, out);
	    if(p != out)
		*(p++) = '/';
	    strcpy(p, cat->textID);
	    return strchr(p, '\0');
	}
    else
	return out;
}


int action_add_header_value_ex(interpreter_config *ic, unsigned int flags, action *a, 
			    const char *value, unsigned int cur_letter, 
			    char ** buf, size_t *buf_size, size_t *buf_used,
			    int (* callback)(void *p, unsigned int flags, const char *data, size_t length, size_t &buf_alloc, size_t &buf_used, char **buffer), void *p)
{
    size_t header_length = 0;

#define REALLOC_BUF(x)							\
do									\
    {									\
	if(*buf_size < out_pos + (x))					\
	    {								\
                if(flags & ACTION_HEADER_VALUE_NOTREALLOC) { *buf_size = out_pos; return 1; }	\
		*buf_size += x;						\
		*buf_size = (*buf_size / 1024 + 1)*1024;			\
		*buf = (char *)realloc(*buf, *buf_size);			\
	    }								\
    } while(0)


    size_t out_pos = 0;
    const char *var_begin = NULL;
		
    enum {
	NORMAL,
	DOLLAR,
	VARIABLE
    } value_state = NORMAL;

    for(const char *ptr = value; *ptr; ptr++)
	{
	    switch(value_state)
		{
		case NORMAL:
		    
		    if(*ptr != '$')
			{
			    REALLOC_BUF(1);
			    (*buf)[out_pos++] = *ptr;
			}
		    else
			value_state = DOLLAR;
				
		    break;
		case DOLLAR:
		    
		    if(*ptr == '{')
			{
			    value_state = VARIABLE;
			    var_begin = ptr+1;
			}
		    else if(*ptr == '$')
			{
			    REALLOC_BUF(1);
			    (*buf)[out_pos++] = '$';
			}
		    else
			{
			    REALLOC_BUF(2);
			    (*buf)[out_pos++] = '$';
			    (*buf)[out_pos++] = *ptr;
			    value_state = NORMAL;
			}
		    
		    break;
		case VARIABLE:

#if defined(KASP)

#define CALL_FILTER                                                     \
		    do {                                                \
			if(!ic->filter_called)                          \
			    {								\
											\
				ic->fr = g_CFilterDll.Check(ic->msg, ic->fr, FILTER_DEFAULTS); \
				ic->filter_called = 1;                  \
                                                                        \
				}                                           \
		    } while(0)


#else
#define CALL_FILTER                                                     \
		    do {                                                \
			if(!ic->filter_called)                          \
			    {                                           \
				message m;                              \
				memset(&m, 0, sizeof(m));               \
                                                                        \
				g_CFilterDll.MessageFromMIME(&m, ic->e->data.text, ic->e->data.length); \
                                                                        \
				ic->fr = g_CFilterDll.Check(&m, ic->fr, FILTER_DEFAULTS); \
				ic->filter_called = 1;                  \
                                                                        \
				g_CFilterDll.MessageDestroy(&m);                    \
			    }                                           \
		    } while(0)

#endif

		    if(*ptr == '}')
			{
			    unsigned int reg_number = 0;

			    enum {
				VAR_NONE, 
				VAR_CAT_ID,
				VAR_CATEGORY,
				VAR_REG,
                                VAR_TRICKS,
                                VAR_TRICKS_EXPANDED,
                                VAR_ENVELOPE_FROM,
                                VAR_ENVELOPE_ALL_RCPTS,
                                VAR_ENVELOPE_CUR_RCPTS,
                                VAR_ENVELOPE_ORIG_RCPTS
			    } variable = VAR_NONE;

			    if(ptr - var_begin == (header_length = sizeof(VARIABLE_CAT_ID)-1) &&
			       strncasecmp(var_begin, VARIABLE_CAT_ID, header_length) == 0)
				variable = VAR_CAT_ID;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_CATEGORY)-1) &&
				    strncasecmp(var_begin, VARIABLE_CATEGORY, header_length) == 0)
				variable = VAR_CATEGORY;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_TRICKS_DETECTED)-1) &&
				    strncasecmp(var_begin, VARIABLE_TRICKS_DETECTED, header_length) == 0)
				variable = VAR_TRICKS;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_TRICKS_DETECTED_EXPANDED)-1) &&
				    strncasecmp(var_begin, VARIABLE_TRICKS_DETECTED_EXPANDED, header_length) == 0)
				variable = VAR_TRICKS_EXPANDED;
			    else if(ptr - var_begin >= sizeof(VARIABLE_REGISTER)-1 &&
				    strncasecmp(var_begin, VARIABLE_REGISTER, sizeof(VARIABLE_REGISTER)-1) == 0)
				{
				    variable = VAR_REG;
				    if(ptr - var_begin > sizeof(VARIABLE_REGISTER)-1)
					{
					    if(var_begin[sizeof(VARIABLE_REGISTER)-1] >= '0' && var_begin[sizeof(VARIABLE_REGISTER)-1] <= '9')
						reg_number = var_begin[sizeof(VARIABLE_REGISTER)-1] - '0';
					}
				}
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_FROM)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_FROM, header_length) == 0)
				variable = VAR_ENVELOPE_FROM;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_ALL_RCPTS)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_ALL_RCPTS, header_length) == 0)
				variable = VAR_ENVELOPE_ALL_RCPTS;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_CUR_RCPTS)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_CUR_RCPTS, header_length) == 0)
				variable = VAR_ENVELOPE_CUR_RCPTS;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_ORIG_RCPTS)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_ORIG_RCPTS, header_length) == 0)
				variable = VAR_ENVELOPE_ORIG_RCPTS;

			    if(variable == VAR_CATEGORY || variable == VAR_CAT_ID)
				{

				    CALL_FILTER;
				    
				    /*
				     * Suggesting that there is not so much relevant categories.
				     */
				    
				    bool first_category = true;
				    
				    if(ic->fr->error_code == FR_OK)
					for(size_t i = 0; i < ic->fr->used; i++)
					    {
						
#define SKIP_CATEGORY do { goto skip_category; } while(0)
						const category *cur_cat = g_CFilterDll.RubSearch(ic->fr->res[i].CID);
						const category *iter_cat; 
						
						for(size_t k = 0; k < cur_cat->sub.count; k++)
						    {
							for(size_t j = 0; j < ic->fr->used; j++)
							    {
								if(cur_cat->sub.categories[k]->CID == ic->fr->res[j].CID)
								    SKIP_CATEGORY;
							    }
						    }
						
						if(!cur_cat)
						    {

							logger_printf(LOGGER_ERROR, "%u: Content Filtration library returned not existing category id %u.", 
								      cur_letter, ic->fr->res[i].CID);
							
							SKIP_CATEGORY;
						    }
						
						
						header_length = first_category ? 0 : 1;

						unsigned int encode_flags;
						encode_flags = 0UL;
						
						
						if(variable == VAR_CAT_ID)
						    {
							iter_cat = cur_cat;
							for(iter_cat = cur_cat; iter_cat; iter_cat = iter_cat->parent)
							    header_length += strlen(iter_cat->textID) + 1;
						    }
						else
						    {
#ifdef FILTER_ENCODE_CATEGORY
							header_length += encoded_text_preprocess(cur_cat->title, encode_flags);
#else
							header_length += strlen(cur_cat->title);
#endif
						    }
						
						REALLOC_BUF(header_length);
						
						if(!first_category)
						    (*buf)[out_pos++] = ';';
						
						if(variable == VAR_CAT_ID)
						    out_pos += print_category_path(cur_cat, (*buf)+out_pos) - ((*buf)+out_pos);
						else
						    {
#ifdef FILTER_ENCODE_CATEGORY
							out_pos += encode_text(cur_cat->title, (*buf)+out_pos, encode_flags);
#else
							memcpy((*buf)+out_pos, cur_cat->title, strlen(cur_cat->title));
							out_pos += strlen(cur_cat->title);
#endif
						    }
						
						first_category = false;
						
#undef SKIP_CATEGORY

					      skip_category:
						;
					    }
				}
                            else if(variable == VAR_ENVELOPE_FROM)
                                {
                                    REALLOC_BUF(ic->e->from.length);
                                    memcpy((*buf)+out_pos, ic->e->from.text, ic->e->from.length);
                                    out_pos += ic->e->from.length;
                                }
                            else if(variable == VAR_ENVELOPE_ALL_RCPTS)
                                {
                                    int    delimiter = 0;
                                    size_t i;

                                    for(i = 0; i < ic->as->rcpts.used; i++)
                                        {
                                            if(delimiter)
                                                {
                                                    REALLOC_BUF(1);
                                                    *((*buf)+out_pos) = ';';
                                                    out_pos ++;
                                                }

                                            REALLOC_BUF(ic->as->rcpts.array[i].length);
                                            memcpy((*buf)+out_pos, 
                                                   ic->as->rcpts.array[i].rcpt, 
                                                   ic->as->rcpts.array[i].length);

                                            out_pos += ic->as->rcpts.array[i].length;

                                            delimiter = 1;
                                        }
                                }
                            else if(variable == VAR_ENVELOPE_CUR_RCPTS)
                                {
                                    int    delimiter = 0;
                                    size_t i;

                                    for(i = 0; i < ic->as->rcpts.used; i++)
                                        {
                                            if(ACTION_RCPT_CHECK(a, i))
                                                {
                                                    if(delimiter)
                                                        {
                                                            REALLOC_BUF(1);
                                                            *((*buf)+out_pos) = ';';
                                                            out_pos ++;
                                                        }

                                                    REALLOC_BUF(ic->as->rcpts.array[i].length);
                                                    memcpy((*buf)+out_pos, 
                                                           ic->as->rcpts.array[i].rcpt, 
                                                           ic->as->rcpts.array[i].length);
                                                    
                                                    out_pos += ic->as->rcpts.array[i].length;
                                                    
                                                    delimiter = 1;
                                                }
                                        }
                                }
                            else if(variable == VAR_ENVELOPE_ORIG_RCPTS)
                                {
                                    int    delimiter = 0;
                                    size_t i;

                                    for(i = 0; i < ic->e->rcpts.used; i++)
                                        {
                                            if(delimiter)
                                                {
                                                    REALLOC_BUF(1);
                                                    *((*buf)+out_pos) = ';';
                                                    out_pos ++;
                                                }

                                            REALLOC_BUF(ic->e->rcpts.array[i].email.length);
                                            memcpy((*buf)+out_pos, 
                                                   ic->e->rcpts.array[i].email.text, 
                                                   ic->e->rcpts.array[i].email.length);

                                            out_pos += ic->e->rcpts.array[i].email.length;

                                            delimiter = 1;
                                        }
                                }
#ifdef WITH_SCORES
                            else if(variable == VAR_TRICKS)
                                {
				    CALL_FILTER;

                                    if(ic->tricks_score)
                                        {
                                            REALLOC_BUF(ic->tricks_desc_used);
                                            memcpy((*buf)+out_pos, ic->tricks_description, ic->tricks_desc_used);
                                            out_pos += ic->tricks_desc_used;
                                        }
                                    
                                }
                            else if(variable == VAR_TRICKS_EXPANDED)
                                {
				    CALL_FILTER;

                                    if(ic->tricks_score)
                                        {

#define ADD_CONST(x)                                                    \
                                            do {                        \
                                                REALLOC_BUF(sizeof(x)-1); \
                                                memcpy((*buf)+out_pos, x, sizeof(x)-1); \
                                                out_pos += sizeof(x)-1;   \
                                            } while(0)

#define ADD_DESC(name)                                                  \
                                            do {                        \
                                                    REALLOC_BUF(ic->descriptions.name.used); \
                                                    memcpy((*buf)+out_pos, ic->descriptions.name.str, ic->descriptions.name.used); \
                                                    out_pos += ic->descriptions.name.used; \
                                            } while(0)
                                           
                                            if(ic->descriptions.invisible.used)
                                                {
                                                    ADD_CONST("Invisible: ");
                                                    ADD_DESC(invisible);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.multi_alphabet.used)
                                                {
                                                    ADD_CONST("Multi-Alphabet: ");
                                                    ADD_DESC(multi_alphabet);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.normalization.used)
                                                {
                                                    ADD_CONST("Normalization: ");
                                                    ADD_DESC(normalization);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.color_change.used)
                                                {
                                                    ADD_CONST("ColorChange: ");
                                                    ADD_DESC(color_change);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.css.used)
                                                {
                                                    ADD_CONST("CSS: ");
                                                    ADD_DESC(css);
                                                    ADD_CONST(" ");
                                                }

#undef ADD_CONST
#undef ADD_DESC

                                        }
                                    
                                }
#endif
			    else if(variable == VAR_REG)
				{
				    if(reg_number < 10)
					{
					    REALLOC_BUF(10);
					    out_pos += sprintf((*buf)+out_pos, "%i", a->registers.values[reg_number]);
					}
				    else
					{
					    REALLOC_BUF(50);
					    out_pos += sprintf((*buf)+out_pos, "[ incorrect register number: %u ]", reg_number);
					}
					
				}
				
					
			    value_state = NORMAL;
			}
		    
		    break;
		}
	}
		

    (*buf_size) = out_pos;
    if(callback && callback(p, flags & ACTION_HEADER_VALUE_NOTCRLF ? ACTION_EXEC_NONE : ACTION_EXEC_CRLF, *buf, out_pos, *buf_size, *buf_used, buf ))
	return 1;

#undef REALLOC_BUF
#undef CALL_FILTER

    return 0;
}



int action_add_header_value(interpreter_config *ic, unsigned int flags, action *a, 
			    const char *value, unsigned int cur_letter, 
			    char ** buf, size_t *buf_size,
			    int (* callback)(void *p, unsigned int flags, const char *data, size_t length), void *p)
{
    size_t header_length = 0;

#define REALLOC_BUF(x)							\
do									\
    {									\
	if(*buf_size < out_pos + (x))					\
	    {								\
                if(flags & ACTION_HEADER_VALUE_NOTREALLOC) { *buf_size = out_pos; return 1; }	\
		*buf_size += x;						\
		*buf_size = (*buf_size / 1024 + 1)*1024;			\
		*buf = (char *)realloc(*buf, *buf_size);			\
	    }								\
    } while(0)


    size_t out_pos = 0;
    const char *var_begin = NULL;
		
    enum {
	NORMAL,
	DOLLAR,
	VARIABLE
    } value_state = NORMAL;

    for(const char *ptr = value; *ptr; ptr++)
	{
	    switch(value_state)
		{
		case NORMAL:
		    
		    if(*ptr != '$')
			{
			    REALLOC_BUF(1);
			    (*buf)[out_pos++] = *ptr;
			}
		    else
			value_state = DOLLAR;
				
		    break;
		case DOLLAR:
		    
		    if(*ptr == '{')
			{
			    value_state = VARIABLE;
			    var_begin = ptr+1;
			}
		    else if(*ptr == '$')
			{
			    REALLOC_BUF(1);
			    (*buf)[out_pos++] = '$';
			}
		    else
			{
			    REALLOC_BUF(2);
			    (*buf)[out_pos++] = '$';
			    (*buf)[out_pos++] = *ptr;
			    value_state = NORMAL;
			}
		    
		    break;
		case VARIABLE:

#if defined(KASP)

#define CALL_FILTER                                                     \
		    do {                                                \
			if(!ic->filter_called)                          \
			    {								\
											\
				ic->fr = g_CFilterDll.Check(ic->msg, ic->fr, FILTER_DEFAULTS); \
				ic->filter_called = 1;                  \
                                                                        \
				}                                           \
		    } while(0)


#else
#define CALL_FILTER                                                     \
		    do {                                                \
			if(!ic->filter_called)                          \
			    {                                           \
				message m;                              \
				memset(&m, 0, sizeof(m));               \
                                                                        \
				g_CFilterDll.MessageFromMIME(&m, ic->e->data.text, ic->e->data.length); \
                                                                        \
				ic->fr = g_CFilterDll.Check(&m, ic->fr, FILTER_DEFAULTS); \
				ic->filter_called = 1;                  \
                                                                        \
				g_CFilterDll.MessageDestroy(&m);                    \
			    }                                           \
		    } while(0)

#endif

		    if(*ptr == '}')
			{
			    unsigned int reg_number = 0;

			    enum {
				VAR_NONE, 
				VAR_CAT_ID,
				VAR_CATEGORY,
				VAR_REG,
                                VAR_TRICKS,
                                VAR_TRICKS_EXPANDED,
                                VAR_ENVELOPE_FROM,
                                VAR_ENVELOPE_ALL_RCPTS,
                                VAR_ENVELOPE_CUR_RCPTS,
                                VAR_ENVELOPE_ORIG_RCPTS
			    } variable = VAR_NONE;

			    if(ptr - var_begin == (header_length = sizeof(VARIABLE_CAT_ID)-1) &&
			       strncasecmp(var_begin, VARIABLE_CAT_ID, header_length) == 0)
				variable = VAR_CAT_ID;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_CATEGORY)-1) &&
				    strncasecmp(var_begin, VARIABLE_CATEGORY, header_length) == 0)
				variable = VAR_CATEGORY;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_TRICKS_DETECTED)-1) &&
				    strncasecmp(var_begin, VARIABLE_TRICKS_DETECTED, header_length) == 0)
				variable = VAR_TRICKS;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_TRICKS_DETECTED_EXPANDED)-1) &&
				    strncasecmp(var_begin, VARIABLE_TRICKS_DETECTED_EXPANDED, header_length) == 0)
				variable = VAR_TRICKS_EXPANDED;
			    else if(ptr - var_begin >= sizeof(VARIABLE_REGISTER)-1 &&
				    strncasecmp(var_begin, VARIABLE_REGISTER, sizeof(VARIABLE_REGISTER)-1) == 0)
				{
				    variable = VAR_REG;
				    if(ptr - var_begin > sizeof(VARIABLE_REGISTER)-1)
					{
					    if(var_begin[sizeof(VARIABLE_REGISTER)-1] >= '0' && var_begin[sizeof(VARIABLE_REGISTER)-1] <= '9')
						reg_number = var_begin[sizeof(VARIABLE_REGISTER)-1] - '0';
					}
				}
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_FROM)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_FROM, header_length) == 0)
				variable = VAR_ENVELOPE_FROM;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_ALL_RCPTS)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_ALL_RCPTS, header_length) == 0)
				variable = VAR_ENVELOPE_ALL_RCPTS;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_CUR_RCPTS)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_CUR_RCPTS, header_length) == 0)
				variable = VAR_ENVELOPE_CUR_RCPTS;
			    else if(ptr - var_begin == (header_length = sizeof(VARIABLE_ENVELOPE_ORIG_RCPTS)-1) &&
				    strncasecmp(var_begin, VARIABLE_ENVELOPE_ORIG_RCPTS, header_length) == 0)
				variable = VAR_ENVELOPE_ORIG_RCPTS;

			    if(variable == VAR_CATEGORY || variable == VAR_CAT_ID)
				{

				    CALL_FILTER;
				    
				    /*
				     * Suggesting that there is not so much relevant categories.
				     */
				    
				    bool first_category = true;
				    
				    if(ic->fr->error_code == FR_OK)
					for(size_t i = 0; i < ic->fr->used; i++)
					    {
						
#define SKIP_CATEGORY do { goto skip_category; } while(0)
						const category *cur_cat = g_CFilterDll.RubSearch(ic->fr->res[i].CID);
						
						const category *iter_cat; 
						
						for(size_t k = 0; k < cur_cat->sub.count; k++)
						    {
							for(size_t j = 0; j < ic->fr->used; j++)
							    {
								if(cur_cat->sub.categories[k]->CID == ic->fr->res[j].CID)
								    SKIP_CATEGORY;
							    }
						    }
						
						if(!cur_cat)
						    {

							logger_printf(LOGGER_ERROR, "%u: Content Filtration library returned not existing category id %u.", 
								      cur_letter, ic->fr->res[i].CID);
							
							SKIP_CATEGORY;
						    }
						
						
						header_length = first_category ? 0 : 1;

						unsigned int encode_flags;
						encode_flags = 0UL;
						
						
						if(variable == VAR_CAT_ID)
						    {
							iter_cat = cur_cat;
							for(iter_cat = cur_cat; iter_cat; iter_cat = iter_cat->parent)
							    header_length += strlen(iter_cat->textID) + 1;
						    }
						else
						    {
#ifdef FILTER_ENCODE_CATEGORY
							header_length += encoded_text_preprocess(cur_cat->title, encode_flags);
#else
							header_length += strlen(cur_cat->title);
#endif
						    }
						
						REALLOC_BUF(header_length);
						
						if(!first_category)
						    (*buf)[out_pos++] = ';';
						
						if(variable == VAR_CAT_ID)
						    out_pos += print_category_path(cur_cat, (*buf)+out_pos) - ((*buf)+out_pos);
						else
						    {
#ifdef FILTER_ENCODE_CATEGORY
							out_pos += encode_text(cur_cat->title, (*buf)+out_pos, encode_flags);
#else
							memcpy((*buf)+out_pos, cur_cat->title, strlen(cur_cat->title));
							out_pos += strlen(cur_cat->title);
#endif
						    }
						
						first_category = false;
						
#undef SKIP_CATEGORY

					      skip_category:
						;
					    }
				}
                            else if(variable == VAR_ENVELOPE_FROM)
                                {
                                    REALLOC_BUF(ic->e->from.length);
                                    memcpy((*buf)+out_pos, ic->e->from.text, ic->e->from.length);
                                    out_pos += ic->e->from.length;
                                }
                            else if(variable == VAR_ENVELOPE_ALL_RCPTS)
                                {
                                    int    delimiter = 0;
                                    size_t i;

                                    for(i = 0; i < ic->as->rcpts.used; i++)
                                        {
                                            if(delimiter)
                                                {
                                                    REALLOC_BUF(1);
                                                    *((*buf)+out_pos) = ';';
                                                    out_pos ++;
                                                }

                                            REALLOC_BUF(ic->as->rcpts.array[i].length);
                                            memcpy((*buf)+out_pos, 
                                                   ic->as->rcpts.array[i].rcpt, 
                                                   ic->as->rcpts.array[i].length);

                                            out_pos += ic->as->rcpts.array[i].length;

                                            delimiter = 1;
                                        }
                                }
                            else if(variable == VAR_ENVELOPE_CUR_RCPTS)
                                {
                                    int    delimiter = 0;
                                    size_t i;

                                    for(i = 0; i < ic->as->rcpts.used; i++)
                                        {
                                            if(ACTION_RCPT_CHECK(a, i))
                                                {
                                                    if(delimiter)
                                                        {
                                                            REALLOC_BUF(1);
                                                            *((*buf)+out_pos) = ';';
                                                            out_pos ++;
                                                        }

                                                    REALLOC_BUF(ic->as->rcpts.array[i].length);
                                                    memcpy((*buf)+out_pos, 
                                                           ic->as->rcpts.array[i].rcpt, 
                                                           ic->as->rcpts.array[i].length);
                                                    
                                                    out_pos += ic->as->rcpts.array[i].length;
                                                    
                                                    delimiter = 1;
                                                }
                                        }
                                }
                            else if(variable == VAR_ENVELOPE_ORIG_RCPTS)
                                {
                                    int    delimiter = 0;
                                    size_t i;

                                    for(i = 0; i < ic->e->rcpts.used; i++)
                                        {
                                            if(delimiter)
                                                {
                                                    REALLOC_BUF(1);
                                                    *((*buf)+out_pos) = ';';
                                                    out_pos ++;
                                                }

                                            REALLOC_BUF(ic->e->rcpts.array[i].email.length);
                                            memcpy((*buf)+out_pos, 
                                                   ic->e->rcpts.array[i].email.text, 
                                                   ic->e->rcpts.array[i].email.length);

                                            out_pos += ic->e->rcpts.array[i].email.length;

                                            delimiter = 1;
                                        }
                                }
#ifdef WITH_SCORES
                            else if(variable == VAR_TRICKS)
                                {
				    CALL_FILTER;

                                    if(ic->tricks_score)
                                        {
                                            REALLOC_BUF(ic->tricks_desc_used);
                                            memcpy((*buf)+out_pos, ic->tricks_description, ic->tricks_desc_used);
                                            out_pos += ic->tricks_desc_used;
                                        }
                                    
                                }
                            else if(variable == VAR_TRICKS_EXPANDED)
                                {
				    CALL_FILTER;

                                    if(ic->tricks_score)
                                        {

#define ADD_CONST(x)                                                    \
                                            do {                        \
                                                REALLOC_BUF(sizeof(x)-1); \
                                                memcpy((*buf)+out_pos, x, sizeof(x)-1); \
                                                out_pos += sizeof(x)-1;   \
                                            } while(0)

#define ADD_DESC(name)                                                  \
                                            do {                        \
                                                    REALLOC_BUF(ic->descriptions.name.used); \
                                                    memcpy((*buf)+out_pos, ic->descriptions.name.str, ic->descriptions.name.used); \
                                                    out_pos += ic->descriptions.name.used; \
                                            } while(0)
                                           
                                            if(ic->descriptions.invisible.used)
                                                {
                                                    ADD_CONST("Invisible: ");
                                                    ADD_DESC(invisible);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.multi_alphabet.used)
                                                {
                                                    ADD_CONST("Multi-Alphabet: ");
                                                    ADD_DESC(multi_alphabet);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.normalization.used)
                                                {
                                                    ADD_CONST("Normalization: ");
                                                    ADD_DESC(normalization);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.color_change.used)
                                                {
                                                    ADD_CONST("ColorChange: ");
                                                    ADD_DESC(color_change);
                                                    ADD_CONST(" ");
                                                }

                                            if(ic->descriptions.css.used)
                                                {
                                                    ADD_CONST("CSS: ");
                                                    ADD_DESC(css);
                                                    ADD_CONST(" ");
                                                }

#undef ADD_CONST
#undef ADD_DESC

                                        }
                                    
                                }
#endif
			    else if(variable == VAR_REG)
				{
				    if(reg_number < 10)
					{
					    REALLOC_BUF(10);
					    out_pos += sprintf((*buf)+out_pos, "%i", a->registers.values[reg_number]);
					}
				    else
					{
					    REALLOC_BUF(50);
					    out_pos += sprintf((*buf)+out_pos, "[ incorrect register number: %u ]", reg_number);
					}
					
				}
				
					
			    value_state = NORMAL;
			}
		    
		    break;
		}
	}
		

    (*buf_size) = out_pos;
    if(callback && callback(p, flags & ACTION_HEADER_VALUE_NOTCRLF ? ACTION_EXEC_NONE : ACTION_EXEC_CRLF, *buf, out_pos))
	return 1;

#undef REALLOC_BUF
#undef CALL_FILTER

    return 0;
}

inline int  action_add_headers(interpreter_config *ic, action *a, unsigned int cur_letter, char *& buf, size_t &buf_size,
			       int (* callback)(void *p, unsigned int flags, const char *data, size_t length), void *p)
{
    size_t header_length = 0;
    size_t j;
    bool was_prepend = false;
    bool header_written = false;

    bool make_additions = false;
    bool was_new_header = false;


    for(size_t k = 0; k < a->headers.used; k++)
	{
            if(!(a->headers.array[k].flags & HEADER_MODIFY_EQUAL_TO_PREV))
                {
                    make_additions = false;
                    was_new_header = false;
                }

            if(!was_new_header && a->headers.array[k].type == HEADER_MODIFY_NEW)
                {
                    was_new_header = true;
                    make_additions = true;
                }

	    if(a->headers.array[k].flags & HEADER_MODIFY_EQUAL_TO_PREV)
		{
		    if((a->headers.array[k].type == HEADER_MODIFY_ADD ||
			a->headers.array[k].type == HEADER_MODIFY_PREPEND) &&
		       (a->headers.array[k].flags & HEADER_MODIFY_FLAG_USED))
			continue;

		    if(a->headers.array[k].type == HEADER_MODIFY_REMOVE)
			continue;

#define SKIP do { goto skip; } while(0)

		    if(buf_size < (header_length = strlen(a->headers.array[k].value)))
			{
			    if(buf) free(buf);
			    buf = (char *)malloc(buf_size = (header_length / 1024 + 1)*1024);
			}

		    if(header_written && a->headers.array[k].type == HEADER_MODIFY_ADD)
			{
			    a->headers.array[k].flags |= HEADER_MODIFY_FLAG_USED;
			    if(callback(p, ACTION_EXEC_NONE, "    ", 2))
				return 1;
			    if(action_add_header_value(ic, ACTION_HEADER_VALUE_DEFAULTS, a, a->headers.array[k].value, cur_letter, &buf, &buf_size, callback, p))
				return 1;
			    
			    SKIP;
			}

		    if(header_written && a->headers.array[k].type == HEADER_MODIFY_PREPEND)
			{
                            if(was_prepend)
                                {
                                    if(callback(p, ACTION_EXEC_NONE, "  ", 1))
                                        return 1;
                                }

			    a->headers.array[k].flags |= HEADER_MODIFY_FLAG_USED;
			    if(action_add_header_value(ic, ACTION_HEADER_VALUE_NOTCRLF, a, a->headers.array[k].value, cur_letter, &buf, &buf_size, callback, p))
				return 1;

			    SKIP;
			}

		    if(was_prepend && a->headers.array[k].type == HEADER_MODIFY_NEW)
			{
                            if(was_prepend)
                                {
                                    if(callback(p, ACTION_EXEC_NONE, "  ", 1))
                                        return 1;
                                }

			    a->headers.array[k].flags |= HEADER_MODIFY_FLAG_USED;
			    if(action_add_header_value(ic, ACTION_HEADER_VALUE_DEFAULTS, a, a->headers.array[k].value, cur_letter, &buf, &buf_size, callback, p))
				return 1;

			    SKIP;
			}

		//	logger_printf(LOGGER_INFO | LOGGER_VERBOSE_HIGH, "%s: %u: NEW HEADER \"%s\".", ic->e->queue_id, cur_letter, a->headers.array[k].header);


		    if(callback(p, ACTION_EXEC_NONE, a->headers.array[k].header, strlen(a->headers.array[k].header)))
			return 1;
		    if(callback(p, ACTION_EXEC_NONE, ":  ", 2))
			return 1;
		    
		    if(action_add_header_value(ic, 
                                               (a->headers.array[k].type == HEADER_MODIFY_PREPEND) ? 
                                               ACTION_HEADER_VALUE_NOTCRLF : ACTION_HEADER_VALUE_DEFAULTS,
                                               a, a->headers.array[k].value, cur_letter, &buf, &buf_size, callback, p))
			return 1;

#undef SKIP
		  skip:

		    
                    was_prepend = (a->headers.array[k].type == HEADER_MODIFY_PREPEND);
		    header_written = true;

		}
	    else
		{
		    if(was_prepend)
			{
			    if(callback(p, ACTION_EXEC_CRLF," ", 0))
				return 1;
			}

		    was_prepend = false;
		    header_written = false;
		    
		    if((a->headers.array[k].type == HEADER_MODIFY_ADD ||
			a->headers.array[k].type == HEADER_MODIFY_PREPEND) &&
		       a->headers.array[k].flags & HEADER_MODIFY_FLAG_USED)
			continue;

		    if(a->headers.array[k].type == HEADER_MODIFY_REMOVE)
			continue;

		    if(buf_size < (header_length = strlen(a->headers.array[k].value)))
			{
			    if(buf) free(buf);
			    buf = (char *)malloc(buf_size = (header_length / 1024 + 1)*1024);
			}

			//logger_printf(LOGGER_INFO | LOGGER_VERBOSE_HIGH, "%s: %u: NEW HEADER \"%s\".", ic->e->queue_id, cur_letter, a->headers.array[k].header);

		    if(callback(p, ACTION_EXEC_NONE, a->headers.array[k].header, strlen(a->headers.array[k].header)))
			return 1;
		    if(callback(p, ACTION_EXEC_NONE, ":  ", 2))
			return 1;
		    
		    if(action_add_header_value(ic, (a->headers.array[k].type == HEADER_MODIFY_PREPEND) ? ACTION_HEADER_VALUE_NOTCRLF : ACTION_HEADER_VALUE_DEFAULTS, 
					       a, a->headers.array[k].value, cur_letter, &buf, &buf_size, callback, p))
			return 1;

                    was_prepend = (a->headers.array[k].type == HEADER_MODIFY_PREPEND);
		    header_written = true;
		    
		}

            if(make_additions)
                {
                    for(j = k+1; j < a->headers.used; j++)
                        {
                            if(!(a->headers.array[j].flags & HEADER_MODIFY_EQUAL_TO_PREV))
                                break;
                            
                            if(a->headers.array[j].type != HEADER_MODIFY_ADD)
                                continue;

                            /* all additions was made before */
                            if(a->headers.array[j].flags & HEADER_MODIFY_FLAG_USED)
                                break;
                            
			    a->headers.array[j].flags |= HEADER_MODIFY_FLAG_USED;
			    if(callback(p, ACTION_EXEC_NONE, "    ", 2))
				return 1;
			    if(action_add_header_value(ic, ACTION_HEADER_VALUE_DEFAULTS, a, a->headers.array[j].value, 
                                                       cur_letter, &buf, &buf_size, callback, p))
				return 1;
                            
                        }

                    make_additions = false;
                }
	}

    if(was_prepend)
	{
	    if(callback(p, ACTION_EXEC_CRLF," ", 0))
		return 1;
	}

    return 0;
}

int action_execute(interpreter_config *ic, action *a, unsigned int cur_letter, 
		   int (* callback)(void *p, unsigned int flags, const char *data, size_t length), void *p)
{
    int res = 0;
#define RETURN(x) 				\
    do {					\
	res = x;				\
	goto finish;				\
    } while(0)


    envelope       *e  = ic->e;

    const char *line_begin = e->data.text;
    size_t      line_length = 0;
    size_t      i;

    enum {
	NORMAL,
	CR
    } state = NORMAL;

    enum {
	HEADERS,
	BODY
    } message_state = HEADERS;

    bool header_delete = false;
    int  header_to_add = -1;
    bool header_written = false;

    char *buf = NULL;
    size_t buf_size = 0;
    size_t k;

    for(k = 0; k < a->headers.used; k++)
	a->headers.array[k].flags &= ~HEADER_MODIFY_FLAG_USED;


    if(ACTION_GET_TYPE(a) == ACTION_REJECT || ACTION_GET_TYPE(a) == ACTION_BLACKHOLE)
	RETURN(-2);

#define IF_HEADER_TO_ADD																	\
    do {																			\
	if(header_to_add >= 0)																	\
	    {																			\
		if(callback(p, ACTION_EXEC_NONE, "    ", 2))													\
		    RETURN(1);																	\
		if(action_add_header_value(ic, ACTION_HEADER_VALUE_DEFAULTS, a, a->headers.array[header_to_add].value, cur_letter,				\
					   &buf, &buf_size, callback, p))											\
		    RETURN(-1);																	\
																				\
		header_to_add++;																\
		for( ; header_to_add < a->headers.used; header_to_add++)											\
		    {																		\
			if(a->headers.array[header_to_add].flags & HEADER_MODIFY_EQUAL_TO_PREV)									\
			    {																	\
				if(a->headers.array[header_to_add].type == HEADER_MODIFY_ADD)									\
				    {																\
					if(callback(p, ACTION_EXEC_NONE, "    ", 2))										\
					    RETURN(-1);														\
					if(action_add_header_value(ic, ACTION_HEADER_VALUE_DEFAULTS, a, a->headers.array[header_to_add].value, cur_letter,	\
								   &buf, &buf_size, callback, p))								\
					    RETURN(-1);														\
					a->headers.array[header_to_add].flags |= HEADER_MODIFY_FLAG_USED;							\
				    }																\
			    }																	\
			else																	\
			    break;																\
		    }																		\
	    }																			\
    } while(0)


    for(i = 0; i < e->data.length; i++)
	{
            if(e->data.text[i] == 10)
                {
                    if(message_state == HEADERS)
                        {
                            if(line_length == 0)
                                {
                                    line_begin = e->data.text + i;
                                    
                                    /*
                                     * Adding all headers and values in message.
                                     */
                                    
                                    IF_HEADER_TO_ADD;
                                    
                                    header_delete = false;
                                    header_to_add = -1;
                                    
                                    if(action_add_headers(ic, a, cur_letter, buf, buf_size, callback, p))
                                        RETURN(-1);
                                    
                                    message_state = BODY;
                                    
                                    if(callback(p, ACTION_EXEC_CRLF, line_begin, line_length))
                                        RETURN(-1);
                                    
                                    line_begin = NULL;
                                }
                            else
                                {
                                    if(!isspace(*line_begin))
                                        {
                                            IF_HEADER_TO_ADD;
                                            
                                            header_delete = false;
                                            header_to_add = -1;
                                            header_written = false;
                                            
                                            const char *ptr = line_begin;
                                            
                                            for( ; ptr - e->data.text < e->data.length && *ptr != ':'; ptr++)
                                                ;
                                            
                                            if(ptr - e->data.text < e->data.length)
                                                {
                                                    for(k = 0; k < a->headers.used; k++)
                                                        {
                                                            if(ptr - line_begin == strlen(a->headers.array[k].header) &&
                                                               strncasecmp(a->headers.array[k].header, line_begin, ptr-line_begin) == 0)
                                                                break;
                                                        }
                                                    
                                                    if(k < a->headers.used && !(a->headers.array[k].flags & HEADER_MODIFY_FLAG_USED))
                                                        {
                                                            
                                                            if(a->headers.array[k].type == HEADER_MODIFY_CHANGE || 
                                                               a->headers.array[k].type == HEADER_MODIFY_REMOVE)
                                                                {
                                                                    header_delete = true;
                                                                    
                                                                    /*
                                                                     * All add/prepend actions will be done after headers
                                                                     * processing.
                                                                     */
                                                                }
                                                            else
                                                                {
                                                                    for( ; ; )
                                                                        {
                                                                            switch(a->headers.array[k].type)
                                                                                {
                                                                                case HEADER_MODIFY_PREPEND:
                                                                                    
                                                                                    if(!header_written)
                                                                                        {
                                                                                            if(callback(p, ACTION_EXEC_NONE, 
                                                                                                        line_begin, 
                                                                                                        ptr-line_begin))
                                                                                                RETURN(-1);
                                                                                            
                                                                                            if(callback(p, ACTION_EXEC_NONE, 
                                                                                                        ": ", 2))
                                                                                                RETURN(-1);
                                                                                            
                                                                                            header_written = true;
                                                                                        }
                                                                                    
                                                                                    if(action_add_header_value(ic, 
                                                                                                               ACTION_HEADER_VALUE_NOTCRLF, 
                                                                                                               a, 
                                                                                                               a->headers.array[k].value, 
                                                                                                               cur_letter,
                                                                                                               &buf, &buf_size, callback, p))
                                                                                        RETURN(-1);
                                                                                    
                                                                                    if(callback(p, ACTION_EXEC_NONE, 
                                                                                                " ", 1))
                                                                                        RETURN(-1);
                                                                                    
                                                                                    
                                                                                    a->headers.array[k].flags |= HEADER_MODIFY_FLAG_USED;
											    
                                                                                case HEADER_MODIFY_ADD:
                                                                                    
                                                                                    if(a->headers.array[k].flags & HEADER_MODIFY_FLAG_USED)
                                                                                        break;
                                                                                    
                                                                                    header_to_add = k;
                                                                                    a->headers.array[k].flags |= HEADER_MODIFY_FLAG_USED;
                                                                                    break;
                                                                                }

                                                                            k++;
                                                                            if(k >= a->headers.used)
                                                                                break;
                                                                            
                                                                            if(!(a->headers.array[k].flags & HEADER_MODIFY_EQUAL_TO_PREV))
                                                                                break;
                                                                            
                                                                            if(header_to_add >= 0)
                                                                                break;
                                                                            
                                                                        }
                                                                }
                                                        }

                                                }
						    
                                            if(!header_delete)
                                                {
                                                    if(header_written)
                                                        {
                                                            if(callback(p, ACTION_EXEC_CRLF, ptr+1, line_length - (ptr - line_begin + 1)))
                                                                RETURN(-1);
                                                        }
                                                    else
                                                        {
                                                            if(callback(p, ACTION_EXEC_CRLF, line_begin, line_length))
                                                                RETURN(-1);
                                                        }
                                                }
                                        }
                                    else
                                        {
                                            if(!header_delete)
                                                {
                                                    if(callback(p, ACTION_EXEC_CRLF, line_begin, line_length))
                                                        RETURN(-1);
                                                }
                                        }
                                }
                        }
                    else
                        {
                            if(callback(p, ACTION_EXEC_CRLF, line_begin, line_length))
                                RETURN(-1);
                        }
                    
                    line_begin = e->data.text+i+1;
                    line_length = 0;
                }
            else 
                {
                    if(line_length == 0 && *line_begin == 13)
                        {
                            line_begin++;
                        }
                    else
                        {
                            line_length++;
                        }
                }
        }

    if(message_state == HEADERS)
        {
            /*
             * Adding all headers and values in message.
             */
            
            IF_HEADER_TO_ADD;
                                    
            header_delete = false;
            header_to_add = -1;
                                    
            if(action_add_headers(ic, a, cur_letter, buf, buf_size, callback, p))
                RETURN(-1);
        }

    if(line_begin && line_length)
        {
            /*
             * It is only one possibility of this: 
             * last string didn't have last CRLF. 
             * Such situation very strange; so we pass 
             * message by SMTP, we have to add CRLF to last line.
             */

            callback(p, ACTION_EXEC_CRLF, line_begin, line_length);
        }

#undef RETURN
#undef HEADER_TO_ADD
  finish:

    if(buf)
	free(buf);
    
    return res;
}
#endif//KIS_USAGE
/*
 * <eof actions.cpp>
 */
