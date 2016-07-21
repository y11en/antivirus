/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: actions.h                                                             *
 * Created: Mon May  6 21:06:30 2002                                           *
 * Desc: Actions, that needed to execute on the envelope (result of            *
 *       profile interpretation.)                                              *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  actions.h
 * \brief Actions, that needed to execute on the envelope (result of 
 *        profile interpretation.)
 * $Id: actions.h,v 1.2 2005/01/16 23:00:27 vtroitsky Exp $
 * $Log: actions.h,v $
 * Revision 1.2  2005/01/16 23:00:27  vtroitsky
 * The results of warnings hunting
 *
 * Revision 1.1  2004/12/29 05:08:40  vtroitsky
 * *** empty log message ***
 *
 * Revision 1.14  2004/07/28 23:18:02  vtroitsky
 * Корректно перенесенный actions_send.
 * Рабочая версия с отсылкой действий и нулевой функциональностью.
 * Базы не загружаются.
 * Вставлены комментарии с версиями.
 *
 */

#include <string.h>

#include "envelope.h"

#ifdef _WIN32
#include "Filtration/API/filter.h"
#else
#include "filter.h"
#endif

#include "interpreter.h"
#include "smtp-filter/common/config.h"

#ifndef __actions_h__
#define __actions_h__

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __action_storage action_storage;
    typedef struct __action         action;
    typedef struct __header_modify  header_modify;
    typedef struct __action_rcpt    action_rcpt;
    typedef struct __action_execute_state action_execute_state;


/*
 * Order of HEADER_MODIFY_* constants (not bitmasks)  have matter 
 * (see sort function at post-normalize stage in actions.cpp)
 */
#define HEADER_MODIFY_NONE               0
#define HEADER_MODIFY_REMOVE             1
#define HEADER_MODIFY_CHANGE             2
#define HEADER_MODIFY_PREPEND            3
#define HEADER_MODIFY_NEW                4
#define HEADER_MODIFY_ADD                5

#define HEADER_MODIFY_FLAG_USED     0x00000001UL
#define HEADER_MODIFY_EQUAL_TO_PREV 0x00000002UL

    struct __header_modify
    {
	unsigned int  type;
	unsigned int  flags;
	const char   *header;
	const char   *value;
    };

#define ACTION_RCPT_USED   0x00000001UL
#define ACTION_RCPT_BOUNCE 0x00000002UL
    struct __action_rcpt
    {
	unsigned int  flags;
	const char   *rcpt;
	size_t        length;
    };

#define ACTION_NONE         0
#define ACTION_ACCEPT       1
#define ACTION_REJECT       2
#define ACTION_BLACKHOLE    3

#define ACTION_TYPE_MASK    0x000000FF

#define ACTION_BOUNCE       0x00000100

    struct __action
    {
	struct {
	    header_modify *array;
	    size_t         used;
	    size_t         alloc;
	} headers;

	struct {
	    int values[MAX_REGISTERS];
	} registers;

	unsigned int type;

	size_t        rcpts_size;
	unsigned int  rcpts[4];    /* actualy can be bigger (rcpts_size) */
    };

    struct __action_storage
    {
	envelope *e;

	struct {
	    action_rcpt *array;
	    size_t       used;
	    size_t       alloc;
	} rcpts;

	action *common_action;

	struct {
	    action **array;
	    size_t   used;
	    size_t   alloc;
	} actions;
    };

    /*
     * Note, that all char data, passed to this functions,
     * not copied into structures (used pointers to original data). 
     */


    action_storage *action_storage_init(action_storage *as, envelope *e);
    int             action_storage_recycle(action_storage *as, envelope *e);
    void            action_storage_destroy(action_storage *as);

#define ACTIONS_STORAGE_NORMALIZE_DEFAULTS    0x00000000U
#define ACTIONS_STORAGE_NORMALIZE_LIBSPAMTEST 0x00000001U
    int             action_storage_normalize(action_storage *as, unsigned int flags);

    /*
     * This function is very ugly...
     */
    action         *action_storage_action_realloc(action_storage *as, action *a, size_t needed_rcpts);

    action         *action_storage_action_clone(action_storage *as, action *a);
    int             action_storage_rcpt_add(action_storage *as, const char *rcpt, size_t length);

    int             action_header_modify(action *a, unsigned int type, const char *headers, const char *value);
    int             action_normalize(action_storage *as, action *a);

/*
 * XXX It is problem with this macros --- we need to realloc memory 
 * if i bigger than size of rcpts array.
 */
#define ACTION_RCPT_SET(a, i)				\
    do {						\
	(a)->rcpts[(i) >> 5] |= (1 << ((i) & 0x1F));	\
    } while(0)

#define ACTION_RCPT_CLEAR(a, i)				\
    do {						\
	(a)->rcpts[(i) >> 5] &= ~(1 << ((i) & 0x1F));	\
    } while(0)

#define ACTION_RCPT_MAX(a) ((a)->rcpts_size*32)

#define ACTION_RCPT_CHECK(a, i)  ((a)->rcpts[(i) >> 5] & (1 << ((i) & 0x1F)))

#define ACTION_RCPT_ZERO(a)					\
    do {							\
	memset((a)->rcpts, 0, (a)->rcpts_size*sizeof(unsigned int));	\
    } while(0)
    

#define ACTION_GET_TYPE(a) ((a)->type & ACTION_TYPE_MASK)

#define ACTION_SET_BOUNCE(as, a)					\
    do {								\
	size_t i = 0;							\
	(a)->type |= ACTION_BOUNCE; 					\
									\
	for(i = 0; i < (as)->rcpts.used; i++)				\
	    if(ACTION_RCPT_CHECK((a), i))				\
		(as)->rcpts.array[i].flags |= ACTION_RCPT_BOUNCE;	\
    } while(0)

#define ACTION_IS_BOUNCE(a) ((a)->type & ACTION_BOUNCE)

#define ACTION_CLEAR_BOUNCE(a)			\
    do {					\
	(a)->type &= ~ACTION_BOUNCE;		\
    } while(0)

#define ACTION_SET_TYPE(a, t)			\
    do {					\
	(a)->type &= ~ACTION_TYPE_MASK;		\
	(a)->type |= (t);			\
    } while(0)

#define ACTION_STORAGE_INITED(as) ((as)->rcpts.array)

#ifdef __cplusplus
}
#endif


#endif /* __actions_h__ */


/*
 * <eof actions.h>
 */
