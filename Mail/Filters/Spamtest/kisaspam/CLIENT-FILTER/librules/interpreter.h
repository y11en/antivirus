/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: interpreter.h                                                         *
 * Created: Mon May  6 20:46:53 2002                                           *
 * Desc: Interpreter of rules                                                  *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  interpreter.h
 * \brief Interpreter of rules
 * $Id: interpreter.h,v 1.6 2005/06/07 07:05:27 vtroitsky Exp $ 
 * $Log: interpreter.h,v $
 * Revision 1.6  2005/06/07 07:05:27  vtroitsky
 * Global debug buffer was changed into local one
 *
 * Revision 1.5  2005/04/12 14:58:36  vtroitsky
 * BayesMatch condition added
 *
 * Revision 1.4  2005/01/22 05:23:13  vtroitsky
 * Skipping formal.xml profile logic added
 *
 * Revision 1.3  2005/01/20 11:52:48  vtroitsky
 * Additional logging and debugging features
 *
 * Revision 1.2  2005/01/04 22:51:22  vtroitsky
 * Some mistakes are fixed in projects
 *
 * Revision 1.1  2004/12/29 05:08:40  vtroitsky
 * *** empty log message ***
 *
 * Revision 1.16  2004/07/28 23:18:02  vtroitsky
 * Корректно перенесенный actions_send.
 * Рабочая версия с отсылкой действий и нулевой функциональностью.
 * Базы не загружаются.
 * Вставлены комментарии с версиями.
 * 
 */

#ifndef __interpreter_h__
#define __interpreter_h__

#include "profile_db.h"
#include "envelope.h"
#include "actions.h"
#include "filter.h"

#ifdef INFORMER
#include "smtp-filter/iqmgr/iqmgr.h"
#include "message.h"
#endif


#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __interpreter_config interpreter_config;

    struct __interpreter_config
    {
	envelope         *e;
	action_storage   *as;
	profile_db       *pdb;
	list_db          *ldb;

#ifdef INFORMER
	iqmgr_connection *iconn;
#endif

#ifdef WITH_SCORES
        int tricks_score;
        char tricks_description[256];
        size_t tricks_desc_used;

        struct {
            
            struct {
                char   str[256];
                size_t used;
            } invisible;

            struct {
                char   str[256];
                size_t used;
            } multi_alphabet;

            struct {
                char   str[256];
                size_t used;
            } normalization;

            struct {
                char   str[256];
                size_t used;
            } color_change;

            struct {
                char   str[256];
                size_t used;
            } css;

        } descriptions;

        struct {
            unsigned int type;
            unsigned int counter;
        } prev_trick;
#endif

	content_filter   *cf;
	
	/*
	 * Filled inside interpreter, but
	 * must free outside.
	 */
	
	filter_result   *fr;
	int filter_called;

    //BayesMatch cached value
    int bayes_rate;


#if defined(INFORMER) || defined(CONFIDENTIAL) || defined(KASP)
	int message_demimed;
        int unknown_parts;
	message *msg;
#endif

        struct {

            in_addr_t *array;
            size_t     used;
            size_t     alloc;

            int        parsed;
            int        first_line;
        } ips;

    };
    
    int profile_interpretate(interpreter_config *ic, bool skipFormal);
    int interpretator_close();

    /*
     * From actions.cpp
     */
#define ACTION_EXEC_NONE 0x00000000UL
#define ACTION_EXEC_CRLF 0x00000001UL
    int action_execute(interpreter_config *ic, action *a, unsigned int cur_letter, 
		       int (* callback)(void *p, unsigned int flags, const char *data, size_t length), void *p);


    /* Very strange internal function --- used in actins_send.c too */

#define ACTION_HEADER_VALUE_DEFAULTS   0x00000000UL
#define ACTION_HEADER_VALUE_NOTREALLOC 0x00000001UL
#define ACTION_HEADER_VALUE_NOTCRLF    0x00000002UL

    int action_add_header_value(interpreter_config *ic, unsigned int flags, action *a, const char *value, unsigned int cur_letter, char **buf, size_t *buf_size,
				int (* callback)(void *p, unsigned int flags, const char *data, size_t length), void *p);

    int action_add_header_value_ex(interpreter_config *ic, unsigned int flags, action *a, 
			    const char *value, unsigned int cur_letter, 
			    char ** buf, size_t *buf_size, size_t *buf_alloc,
			    int (* callback)(void *p, unsigned int flags, const char *data, size_t length, size_t &buf_alloc, size_t &buf_used, char **buffer), void *p);


    int tricks_score_callback(void *context, unsigned int score_type, int score, unsigned int type);
    int tricks_dump(interpreter_config *ic);

    
#ifdef __cplusplus
}
#endif

#endif /* __interpreter_h__ */

/*
 * <eof interpreter.h>
 */
