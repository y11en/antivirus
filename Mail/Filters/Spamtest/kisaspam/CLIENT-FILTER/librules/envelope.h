/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: envelope.h                                                            *
 * Created: Mon May  6 21:02:38 2002                                           *
 * Desc: SMTP envelope data structures.                                        *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  envelope.h
 * \brief SMTP envelope data structures.
 * $Id: envelope.h,v 1.4 2005/01/16 23:00:27 vtroitsky Exp $ 
 * $Log: envelope.h,v $
 * Revision 1.4  2005/01/16 23:00:27  vtroitsky
 * The results of warnings hunting
 *
 * Revision 1.3  2005/01/06 14:17:49  vtroitsky
 * HAVE_GSG2 changed to WITH_GSG
 *
 * Revision 1.2  2005/01/04 22:51:22  vtroitsky
 * Some mistakes are fixed in projects
 *
 * Revision 1.1  2004/12/29 05:08:40  vtroitsky
 * *** empty log message ***
 *
 * Revision 1.16  2004/08/25 18:56:06  vtroitsky
 * Сервер с подключенным GSG
 *
 * Revision 1.15  2004/07/28 23:18:02  vtroitsky
 * Корректно перенесенный actions_send.
 * Рабочая версия с отсылкой действий и нулевой функциональностью.
 * Базы не загружаются.
 * Вставлены комментарии с версиями.
 * 
 */

#ifndef __envelope_h__
#define __envelope_h__

#include "smtp-filter/common/ip_extractor.h"

#include "smtp-filter/libsigs/libsigs.h"
#include "smtp-filter/libgsg2/gsg.h"

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef _WIN32
	typedef struct __common common_state;
#endif //_WIN32
    typedef struct __envelope      envelope;
    typedef struct __envelope_text envelope_text;
    typedef struct __envelope_rcpt envelope_rcpt;
    typedef struct __msg_header    msg_header;
    typedef struct __envelope_attr envelope_attr;
    typedef size_t                 heap_offset;

#ifdef HAVE_MD5CHECK
    typedef struct 
    {
	md5sum   sum;
	size_t   size;
    }md5data;
#endif
#ifdef HAVE_KAVCLIENT
    typedef struct
    {
        char *type;
        char *subtype;
        size_t size;
    }ctype_data;
#endif

    struct __envelope_text
    {
	const char *text;
	size_t      length;
    };

    struct __envelope_attr {

        const char    *name;
        const char    *content;

    };

    struct __msg_header
    {
	heap_offset header;
	heap_offset value;
    };

#define ERCPT_USED 0x00000001U

    struct __envelope_rcpt
    {
	envelope_text email;
	unsigned int  flags;
    };

    struct __envelope 
    {
	envelope_text from;
	envelope_text ip_addr;
	
	struct {
	    envelope_rcpt *array;
	    size_t         used;
	    size_t         alloc;
	} rcpts;

        struct {
            envelope_attr *array;
            size_t         used;
            size_t         alloc;
        } attrs;
	
	envelope_text data;

	struct {
	    msg_header *array;
	    size_t      used;
	    size_t      alloc;
	} headers;

	struct {
	    char *array;
	    size_t used;
	    size_t alloc;
	} heap;

	struct {
	    int checked;
	    int exist;
	} dns;

#ifdef HAVE_MD5CHECK_OLD
	struct {
	    md5data *array;
	    size_t   used;
	    size_t   alloc;
	} md5_sums;
#endif
#ifdef HAVE_KAVCLIENT
	struct {
	    ctype_data *array;
	    size_t	used;
	    size_t	alloc;
	} ctypes;
#endif
#ifdef WITH_GSG
	binary_data_t*  binary_processor;
#endif
		const char *queue_id;

        ip_extractor *ipe;
        size_t max_received_headers;

		unsigned int msg_size;
    };
    
	bool cmp_nocase(const char* s1, const char* s2, size_t len);
	int check_encoding(const char* name, size_t name_len);


    envelope   *envelope_create(envelope *e);
    int         envelope_recycle(envelope *e);
    void        envelope_destroy(envelope *e);

#define ETEXT_ASCIIZ -1
    int         envelope_set_from(envelope *e, const char *email, int length);
    int         envelope_set_data(envelope *e, const char *data, int length);
    int         envelope_add_rcpt(envelope *e, const char *email, int length);
    int         envelope_set_ip_addr(envelope *e, const char *ip_addr, int length);

#ifdef WITH_GSG
    int         envelope_prepare_headers(envelope *e,binary_data_t *);
#else
    int         envelope_prepare_headers(envelope *e);
#endif
	int			envelope_new_header(envelope *e);

    int         envelope_search_header(envelope *e, const char *header, 
				       int (* callback)(void *p, const char *value),
				       void *p);
    int         envelope_search_header_iterate(envelope *e, const char *header, 
					       int (* callback)(void *p, const char *value),
					       void *p);

    size_t      envelope_heap_get_string(envelope *e, size_t length);
    int         envelope_heap_resize(envelope *e, size_t length);

    int         envelope_add_attr(envelope *e, const char *attr_name, const char *content);
    const char *envelope_find_attr(envelope *e, const char *attr_name);

#define ENVELOPE_HEAP_STRING(e, offset) ((e)->heap.array + (offset))

#ifdef __cplusplus
}
#endif


#endif /* __envelope_h__ */

/*
 * <eof envelope.h>
 */
