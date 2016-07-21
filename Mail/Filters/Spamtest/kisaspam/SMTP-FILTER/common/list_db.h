/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: list_db.h                                                             *
 * Created: Wed Apr 17 14:09:34 2002                                           *
 * Desc: Common database of lists (low level of API)                           *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  list_db.h
 * \brief Common database of lists (low level of API)
 *
 * Can add list to berkeley db file with structure:
 *
 *    key  := < list_type > < list_id > < list_key > |
 *            < list_type > < list_id >
 *
 * Last form is used then search by list_key does not needed.
 *
 *    data := < some additional data, for example desctiption >
 */

#ifndef __list_db_h__
#define __list_db_h__

#ifndef _WIN32
#include <sys/param.h>
#include <netinet/in.h>
#else
#include <_include/nix_types.h>

#include "smtp-filter/adc/adc.h"


#endif


#ifdef __cplusplus
extern "C"
{
#endif 

    typedef unsigned char list_type;

#define LIST_TYPE_IP      'I'
#define LIST_TYPE_EMAIL   'E'
#define LIST_TYPE_RBL     'R'

    
    typedef unsigned long int list_id;

#define LIST_ID_NONE 0UL

    typedef struct __list_db list_db;

    /*
     * mode, flags and name same to open routine.
     */
    list_db *list_db_open(const char *name, int flags, int mode);
    void     list_db_close(list_db *ldb);

    list_id  list_create(list_db *ldb, list_type type, 
			 const char *name, const char *description);
    list_id  list_find(list_db *ldb, list_type type, const char *name);

    struct list_email_entry {
	const char *address;
	const char *description;
    };

    struct list_rbl_entry {
	char *domain;
	char *description;
        unsigned short weight; /* by default --- threshold (100, look into .cpp) */
    };

#ifdef _WIN32
	struct adc_structure {
        struct adc_object object;

        struct {
            adc_request *array;
            size_t       number;
        } requests;

        struct {
            char   *buf;
            size_t  size;
        } pool;

        int timeout;

        int error;

		int inited;
    };
#endif
    /***************************************************************************
     *
     * Manage lists --- add elements.
     *
     */

    int      list_email_add (list_db *ldb, list_id lid, const struct list_email_entry *lee);
    int      list_ip_add    (list_db *ldb, list_id lid, const char *ip_address);
    int      list_rbl_add   (list_db *ldb, list_id lid, const struct list_rbl_entry *lre);

    
    /*
     * Check elements in lists.
     */

#define LIST_CHECK_EXIST        1
#define LIST_CHECK_NOT_EXIST    0
#define LIST_CHECK_ERROR       -1

#define LIST_CHECK_ASCIIZ      -1

    int      list_email_check(list_db *ldb, list_id lid, const char *email, int length);
    int      list_ip_check(list_db *ldb, list_id lid, const char *ip, int length);
    int      list_ip_check_ip(list_db *ldb, list_id lid, in_addr_t addr);


#define RBL_QUERY_RECEIVED 0x00000001U

    struct rbl_query {
        in_addr_t    addr;
        unsigned int flags;
    };

    /*
     * Returns total weight.
     */
#ifndef _WIN32
    int      list_rbl_check(list_db *ldb, list_id lid, 
                            struct rbl_query *queries, 
                            size_t queries_number);
#else
	int      list_rbl_check2(adc_structure &adc, list_db *ldb, list_id lid, 
                            struct rbl_query *queries, 
                            size_t queries_number);
#endif	


    /* if second == 0, than timeout sets to default value */
    int      list_db_rbl_set_timeout(list_db *ldb, int seconds);
    int      list_db_rbl_get_timeout(list_db *ldb);

#ifdef __cplusplus
}
#endif

#endif /* __list_db_h__ */

/*
 * <eof list_db.h>
 */
