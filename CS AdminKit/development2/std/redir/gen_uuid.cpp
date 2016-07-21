/*
 * gen_uuid.c --- generate a DCE-compatible uuid
 *
 * Copyright (C) 1996, 1997, 1998, 1999 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU 
 * Library General Public License.
 * %End-Header%
 */

#ifdef __unix

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

#ifdef N_PLAT_NLM
 #include <time.h>
#else
 #include <sys/time.h>
 #include <sys/file.h>
#endif
#include <sys/stat.h>

#include "uuidP.h"

#include "./macaddr.h"

int KLSTD_Random(int nMin, int nMax);

using namespace KLSTD;


namespace
{
    //global data
    int         g_hRandom = -1;
    vec_macs_t  g_vecMacs;
    bool        g_bRealMac = false;
};

void uuid_pack(const struct uuid *uu, uuid_t& ptr)
{
	__u32	tmp;
	unsigned char	*out = &ptr.Data16[0];

	tmp = uu->time_low;
	out[3] = (unsigned char) tmp;
	tmp >>= 8;
	out[2] = (unsigned char) tmp;
	tmp >>= 8;
	out[1] = (unsigned char) tmp;
	tmp >>= 8;
	out[0] = (unsigned char) tmp;
	
	tmp = uu->time_mid;
	out[5] = (unsigned char) tmp;
	tmp >>= 8;
	out[4] = (unsigned char) tmp;

	tmp = uu->time_hi_and_version;
	out[7] = (unsigned char) tmp;
	tmp >>= 8;
	out[6] = (unsigned char) tmp;

	tmp = uu->clock_seq;
	out[9] = (unsigned char) tmp;
	tmp >>= 8;
	out[8] = (unsigned char) tmp;

	memcpy(out+10, uu->node, 6);
}

namespace
{
    /*
     * Generate a series of random bytes.  Use /dev/urandom if possible,
     * and if not, use srandom/random.
     */
    void get_random_bytes(void *buf, int nbytes)
    {
	    int i, fd = g_hRandom;
	    int lose_counter = 0;
	    char *cp = (char *) buf;

	    if (fd >= 0) {
		    while (nbytes > 0) {
			    i = read(fd, cp, nbytes);
			    if ((i < 0) &&
			        ((errno == EINTR) || (errno == EAGAIN)))
				    continue;
			    if (i <= 0) {
				    if (lose_counter++ == 8)
					    break;
				    continue;
			    }
			    nbytes -= i;
			    cp += i;
			    lose_counter = 0;
		    }
	    }
	    if (nbytes == 0)
		    return;

	    /* XXX put something better here if no /dev/random! */
	    for (i=0; i < nbytes; i++)
		    *cp++ = KLSTD_Random(0, 255) & 0xFF;
	    return;
	    
    }

    /* Assume that the gettimeofday() has microsecond granularity */
    #define MAX_ADJUSTMENT 10

    int get_clock(__u32 *clock_high, __u32 *clock_low, __u16 *ret_clock_seq)
    {
	    static struct timeval		last = {0, 0};

	    struct timeval 			tv;
	    static int			adjustment = 0;
	    static __u16			clock_seq = 0;
	    unsigned long long		clock_reg = 0;
	    
    #ifdef N_PLAT_NLM
	    struct tm 			tv_novell;
	    mktime(&tv_novell);
            tv.tv_sec = tv_novell.tm_sec;
            tv.tv_usec = tv_novell.tm_sec*1000;
    #else
	    gettimeofday(&tv, 0);
    #endif



    try_again:
        //printf("try_again\n");
	    if ((last.tv_sec == 0) && (last.tv_usec == 0)) 
        {
		    get_random_bytes(&clock_seq, sizeof(clock_seq));
		    clock_seq &= 0x1FFF;
		    last = tv;
		    last.tv_sec--;
	    }
            //printf("tv.tv_sec = %u, tv.tv_usec=%u,  last.tv_sec = %u, last.tv_usec=%u\n",
                //tv.tv_sec, tv.tv_usec,  last.tv_sec, last.tv_usec);

	    if ((tv.tv_sec < last.tv_sec) ||
	        ((tv.tv_sec == last.tv_sec) &&
	         (tv.tv_usec < last.tv_usec))) 
        {
		    clock_seq = (clock_seq+1) & 0x1FFF;
		    adjustment = 0;
		    last = tv;
	    }
        else if ((tv.tv_sec == last.tv_sec) &&
	        (tv.tv_usec == last.tv_usec)) 
        {
            //printf("adjustment is %u\n", adjustment);
		    if (adjustment >= MAX_ADJUSTMENT)
            {                
                goto try_again;
            };
		    adjustment++;
	    } 
        else 
        {
            //printf("else:adjustment=0\n");
		    adjustment = 0;
		    last = tv;
	    }
		    
	    clock_reg = tv.tv_usec*10 + adjustment;
	    clock_reg += ((unsigned long long) tv.tv_sec)*10000000;
	    clock_reg += (((unsigned long long) 0x01B21DD2) << 32) + 0x13814000;

	    *clock_high = clock_reg >> 32;
	    *clock_low = clock_reg;
	    *ret_clock_seq = clock_seq;
	    return 0;
    }

    void uuid_generate_time(uuid_t& out)
    {
	    static uuid uu;
	    __u32	clock_mid = 0;
	    get_clock(&clock_mid, &uu.time_low, &uu.clock_seq);
	    uu.clock_seq |= 0x8000;
	    uu.time_mid = (__u16) clock_mid;
	    uu.time_hi_and_version = (clock_mid >> 16) | 0x1000;
	    memcpy(uu.node, &g_vecMacs[0].addr[0], 6);
	    uuid_pack(&uu, out);
    }

    void uuid_generate_random(uuid_t& out)
    {
	    uuid_t	buf;
	    struct uuid uu;

	    get_random_bytes(&buf, sizeof(buf));
	    uuid_unpack(buf, &uu);

	    uu.clock_seq = (uu.clock_seq & 0x3FFF) | 0x8000;
	    uu.time_hi_and_version = (uu.time_hi_and_version & 0x0FFF) | 0x4000;
	    uuid_pack(&uu, out);
    }

    int get_random_fd()
    {
	    int fd = -1;
		fd = open("/dev/urandom", O_RDONLY);
		if (fd == -1)
			fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
	    return fd;
    };
};

void uuid_initialize()
{
    uuid_deinitialize();
    g_hRandom = get_random_fd();
    KLSTD::get_macaddresses(g_vecMacs);
    if(g_vecMacs.empty())
    {
        macaddr_t mac;
        get_random_bytes(&mac.addr[0], 6);
		/*
		 * Set multicast bit, to prevent conflicts
		 * with IEEE 802 addresses obtained from
		 * network cards
		 */
		mac.addr[0] |= 0x80;
        g_vecMacs.push_back(mac);

        g_bRealMac = false;
        //printf( "No mac address acquired\n" );
    }
    else
    {
        g_bRealMac = true;
        /*
        printf(  "mac address acquired: %X-%X-%X-%X-%X-%X\n",
                (int)g_vecMacs[0].addr[0],
                (int)g_vecMacs[0].addr[1],
                (int)g_vecMacs[0].addr[2],
                (int)g_vecMacs[0].addr[3],
                (int)g_vecMacs[0].addr[4],
                (int)g_vecMacs[0].addr[5]);
        */
    };
};

void uuid_deinitialize()
{
    if(g_hRandom != -1)
    {
        close(g_hRandom);
        g_hRandom = -1;
    };
    g_vecMacs.clear();
    g_bRealMac = false;
};

void uuid_generate(uuid_t& out)
{
    //pseudo-random generator may be bad, so using MAC is more reliable
    if(g_hRandom >= 0 && !g_bRealMac)
        uuid_generate_random(out);
    else
        uuid_generate_time(out);
    /*
    printf(  "GUID generated: %X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X\n",
            (int)out.Data16[0],
            (int)out.Data16[1],
            (int)out.Data16[2],
            (int)out.Data16[3],
            (int)out.Data16[4],
            (int)out.Data16[5],
            (int)out.Data16[6],
            (int)out.Data16[7],
            (int)out.Data16[8],
            (int)out.Data16[9],
            (int)out.Data16[10],
            (int)out.Data16[11],
            (int)out.Data16[12],
            (int)out.Data16[13],
            (int)out.Data16[14],
            (int)out.Data16[15]);
    */
}

#endif //__unix
