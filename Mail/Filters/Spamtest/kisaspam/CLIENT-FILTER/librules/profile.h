/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: profile.h                                                             *
 * Created: Fri Apr 12 22:15:21 2002                                           *
 * Desc: Structures and functions for work with profiles binary data.          *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  profile.h
 * \brief Structures and functions for work with profiles binary data.
 */

#ifndef __profile_h__
#define __profile_h__

#include <stdlib.h>
#include <sys/types.h>

#ifndef _WIN32
#include <regex.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include "gnu/regex/regex.h"
#endif // _WIN32

	       
#include <smtp-filter/common/list_db.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __rule      rule;
    typedef struct __profile   profile;

/*
 * Profile real data structure.
 */
struct __profile 
{
    char *name;
    char *description;
    
    unsigned int type;
    
    struct {
	size_t   count;
	char   **array;

	list_id  lid;
    } rcpts;

    struct {
	size_t  count;
	rule   *array;
    } rules;
};

    /*
     * Conditions
     */

#define CONDITION_NORMAL 0x00000000UL
#define CONDITION_NOT    0x00000001UL

    int rule_add_condition_smtp_from          (rule *r, unsigned int flags, const char *email);
    int rule_add_condition_smtp_from_list     (rule *r, unsigned int flags, list_id lid);
    int rule_add_condition_smtp_to            (rule *r, unsigned int flags, const char *email);
    int rule_add_condition_smtp_to_list       (rule *r, unsigned int flags, list_id lid);
    int rule_add_condition_has_header         (rule *r, unsigned int flags, const char *header);
    int rule_add_condition_header_match       (rule *r, unsigned int flags, const char *header, const char *regexp);
    int rule_add_condition_category_match     (rule *r, unsigned int flags, unsigned int cat_id);
    int rule_add_condition_msg_size           (rule *r, unsigned int flags, unsigned int limit);
    int rule_add_condition_ip_from            (rule *r, unsigned int flags, const char *ip);
    int rule_add_condition_ip_from_list       (rule *r, unsigned int flags, list_id lid);
    int rule_add_condition_ip_from_rbl        (rule *r, unsigned int flags, list_id lid);
    int rule_add_condition_ip_from_not_in_dns (rule *r, unsigned int flags);
    int rule_add_condition_attribute_match    (rule *r, unsigned int flags, const char *attribute, const char *regexp);
    int rule_add_condition_content_type_match (rule *r, unsigned int flags, const char *regexp);
    int rule_add_condition_file_name_match    (rule *r, unsigned int flags, const char *regexp);
    int rule_add_condition_has_attachments    (rule *r, unsigned int flags);
    int rule_add_condition_unknown_content    (rule *r, unsigned int flags);
    int rule_add_condition_virus_match        (rule *r, unsigned int flags, const char *regexp);
    int rule_add_condition_dcc_match          (rule *r, unsigned int flags, const char *body, const char *fuz1, const char *fuz2);
    int rule_add_condition_bayes_match        (rule *r, unsigned int flags, unsigned int rate);

#define CONDITION_REGISTER_EQUAL   0x00000000UL
#define CONDITION_REGISTER_MORE    0x00000001UL
#define CONDITION_REGISTER_LESS    0x00000002UL

    int rule_add_condition_register           (rule *r, unsigned int flags, unsigned int number, unsigned int type, int quantity);

    /*
     * Actions
     */

    int rule_add_action_accept            (rule *r);
    int rule_add_action_blackhole         (rule *r);
    int rule_add_action_reject            (rule *r);
    int rule_add_action_bounce            (rule *r);
    int rule_add_action_skip              (rule *r);

    int rule_add_action_send_notification (rule *r, const char *entry);
    int rule_add_action_send_log_entry    (rule *r, const char *entry);

    /*
     * mask --- from logger.h
     */
    int rule_add_action_syslog            (rule *r, unsigned int mask, const char *message);

    /*
     * XXX Here we must change this constants.
     *
     * Actually, now change and new are equal.
     */
#define ACTION_SEND_NEW    0
#define ACTION_SEND_ADD    1
#define ACTION_SEND_CHANGE 2
#define ACTION_SEND_DELETE 3

    int rule_add_action_send               (rule *r, unsigned int type, size_t count, char **rcpts);

    /*
     * HeaderReplace
     */
    int rule_add_action_header_change      (rule *r, const char *header, const char *value);
    /*
     * HeaderAdd
     */
    int rule_add_action_header_add         (rule *r, const char *header, const char *value);
    /*
     * HeaderPrepend
     */
    int rule_add_action_header_prepend     (rule *r, const char *header, const char *value);
    /*
     * HeaderDelete
     */
    int rule_add_action_header_remove      (rule *r, const char *header);
    /*
     * HeaderNew
     */
    int rule_add_action_header_new         (rule *r, const char *header, const char *value);

    int rule_add_action_register_set       (rule *r, unsigned int number, int qunatity);
    int rule_add_action_register_add       (rule *r, unsigned int number, int qunatity);

    int rule_add_action_dcc_report         (rule *r, const char *count);

    /*
     * Misc profile supporting routines
     */

#define PROFILE_NONE       0
#define PROFILE_COMMON     1
#define PROFILE_PRIVATE    2

    /*
     * Creates profile (COMMON or PRIVATE --- see PROFILE_* constants). 
     *
     * If type != PROFILE_PRIVATE, count and rcpts not using.
     *
     * If count != 0 and rcpts == 0, then count treats as list_id of
     * email list.
     */
    profile *profile_create(const char *name, const char *comment, 
			    unsigned int type, size_t count, char **rcpts);

    void     profile_destroy(profile *p);

    size_t   profile_get_size(profile *p);
    int      profile_check_rcpt(profile *p, list_db *ldb, const char *rcpt, size_t rcpt_len);
    unsigned int profile_get_type(profile *p);
    
    profile *profile_load(void *data, size_t size);
    void    *profile_save(profile *p, void *data, size_t size);

    rule    *profile_create_rule(profile *p);
    int      profile_remove_last_rule(profile *p);

    const char *profile_get_name(profile *p);

    /*
     * Iterate actions and conditions routines
     */

    struct profile_iterate_callbacks
    {
	struct {
	    int (* smtp_from)          (void *p, const char *email);
	    int (* smtp_from_list)     (void *p, list_id lid);
	    int (* smtp_to)            (void *p, const char *email);
	    int (* smtp_to_list)       (void *p, list_id lid);
	    int (* has_header)         (void *p, const char *header);
	    int (* header_match)       (void *p, const char *header, const char *regexp, regex_t *compiled_regex);
	    int (* category_match)     (void *p, unsigned int cat_id);
	    int (* msg_size)           (void *p, unsigned int limit);
	    int (* ip_from_list)       (void *p, list_id lid);
	    int (* ip_from)            (void *p, in_addr_t addr, in_addr_t mask);
	    int (* ip_from_rbl)        (void *p, list_id lid);
	    int (* ip_from_not_in_dns) (void *p);
	    int (* check_register)     (void *p, unsigned int number, unsigned int type, int value);
	    int (* attribute_match)    (void *p, const char *attribute, const char *regexp, regex_t *compiled_regex);
	    int (* content_type_match) (void *p, const char *regexp, regex_t *compiled_regex);
	    int (* file_name_match)    (void *p, const char *regexp, regex_t *compiled_regex);
	    int (* has_attachments)    (void *p);
	    int (* unknown_content)    (void *p);
	    int (* virus_match)        (void *p, const char *regexp, regex_t *compiled_regex);
	    int (* dcc_match)          (void *p, const char *body, const char *fuz1, const char *fuz2);
        int (* bayes_match)        (void *p, unsigned int rate);
	} condition;

	struct {
	    int (* accept)             (void *p);
	    int (* blackhole)          (void *p);
	    int (* reject)             (void *p);
	    int (* bounce)             (void *p);
	    int (* skip)               (void *p);
	    int (* send)               (void *p, unsigned int type, size_t count, const char *rcpts);
	    int (* header_change)      (void *p, const char *header, const char *value);
	    int (* header_add)         (void *p, const char *header, const char *value);
	    int (* header_prepend)     (void *p, const char *header, const char *value);
	    int (* header_new)         (void *p, const char *header, const char *value);
	    int (* header_remove)      (void *p, const char *header);
	    int (* register_set)       (void *p, unsigned int number, int value);
	    int (* register_add)       (void *p, unsigned int number, int value);
	    int (* send_notification)  (void *p, const char *entry);
	    int (* send_log_entry)     (void *p, const char *entry);
	    int (* syslog)             (void *p, unsigned int mask, const char *message);
            int (* dcc_report)         (void *p, const char *count);
	} action;

	int (* rule_begin)(void *p);
    };
    

    int profile_iterate(profile *p, struct profile_iterate_callbacks *pic, void *pdata);

#ifdef __cplusplus
}
#endif

#endif /* __profile_h__ */


/*
 * <eof profile.h>
 */
