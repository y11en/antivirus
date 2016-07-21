/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * --------------------------------------------------------------------------- *
 * File: ip_extractor.h                                                        *
 * Created: Fri Feb 27 15:40:25 2004                                           *
 * Desc: Extractor of IPv4 addresess from the input strings.                   *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  ip_extractor.h
 * \brief Extractor of IPv4 addresess from the input strings.
 */

#ifndef __ip_extractor_h__
#define __ip_extractor_h__

#ifndef _WIN32
#include <sys/param.h>
#include <netinet/in.h>
#else 
#include <winsock.h>
#include <_include/nix_types.h>
#endif 

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __ip_extractor ip_extractor;

    struct ip_info {
        in_addr_t addr;
        int       line;
        char      prev_char, next_char;
    };

#define IP_EXTRACTOR_DEFAULTS     0x00000000U
#define IP_EXTRACTOR_ONE_FOR_LINE 0x00000001U
    ip_extractor *ip_extractor_create(unsigned int flags);
    int           ip_extractor_destroy(ip_extractor *ipe);

    /* returns number of parsed IPs */
    int           ip_extractor_parse(ip_extractor *ipe, const char *text, int length);
    int           ip_extractor_parse_finish(ip_extractor *ipe);

    /* example of "line" is one Received header */
    int           ip_extractor_end_of_line(ip_extractor *ipe);

    int           ip_extractor_recycle(ip_extractor *ipe);
    
    /* returns number of stored IPs */
    int           ip_extractor_ips_count(ip_extractor *ipe);
    in_addr_t     ip_extractor_get_ip(ip_extractor *ipe, unsigned int number);
    int           ip_extractor_ips_drop(ip_extractor *ipe);

    struct ip_info *ip_extractor_get_ip_info(ip_extractor *ipe, unsigned int number);

#ifdef __cplusplus
}
#endif

#endif /* __ip_extractor_h__ */

/*
 * <eof ip_extractor.h>
 */
