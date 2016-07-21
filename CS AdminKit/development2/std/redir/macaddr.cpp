#ifdef __unix
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
//#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <errno.h>

#include <stdio.h>

#include <vector>
#include <algorithm>

#include "./macaddr.h"

namespace KLSTD
{
    int get_macaddresses(vec_macs_t& vecMacs)
    {
	    int 		sd = -1;
	    struct ifreq 	ifr, *ifrp;
	    struct ifconf 	ifc;
	    char buf[1024];
	    int		n, i;
	    unsigned char 	*a;
	    
    /*
     * BSD 4.4 defines the size of an ifreq to be
     * max(sizeof(ifreq), sizeof(ifreq.ifr_name)+ifreq.ifr_addr.sa_len
     * However, under earlier systems, sa_len isn't present, so the size is 
     * just sizeof(struct ifreq)
     */
    #ifdef HAVE_SA_LEN
        //printf("HAVE_SA_LEN\n");
        #define ifreq_size(i)   (std::max( sizeof(struct ifreq), sizeof((i).ifr_name)+(i).ifr_addr.sa_len ))
    #else
        //printf("no HAVE_SA_LEN\n");
        #define ifreq_size(i) (sizeof(struct ifreq))
    #endif /* HAVE_SA_LEN*/
	    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        //printf("sd=%u\n", sd);
	    if (sd < 0)
        {
		    return -1;
	    }
	    memset(buf, 0, sizeof(buf));
	    ifc.ifc_len = sizeof(buf);
	    ifc.ifc_buf = buf;
	    if (ioctl (sd, SIOCGIFCONF, (char *)&ifc) < 0)
        {
            //printf("ioctl (sd, SIOCGIFCONF, (char *)&ifc) < 0\n");
		    close(sd);
		    return -1;
	    }
	    n = ifc.ifc_len;
        //printf("n=%u\n", n);
    
        vecMacs.reserve(n);

	    for (i = 0; i < n; i+= ifreq_size(*ifr) )
        {
		    ifrp = (struct ifreq *)((char *) ifc.ifc_buf+i);
		    strncpy(ifr.ifr_name, ifrp->ifr_name, IFNAMSIZ);
            //printf("ifrp->ifr_name=%s\n", ifrp->ifr_name);
    #ifdef SIOCGIFHWADDR
		    if (ioctl(sd, SIOCGIFHWADDR, &ifr) < 0)
            {
                //printf("ioctl(sd, SIOCGIFHWADDR, &ifr) < 0\n");
                continue;
            }
		    a = (unsigned char *) &ifr.ifr_hwaddr.sa_data;
    #else
        #ifdef SIOCGENADDR
		    if (ioctl(sd, SIOCGENADDR, &ifr) < 0)
			    continue;
		    a = (unsigned char *) ifr.ifr_enaddr;
        #else
		    /*
		     * XXX we don't have a way of getting the hardware
		     * address
		     */
		    close(sd);
		    return 0;
        #endif /* SIOCGENADDR */
    #endif /* SIOCGIFHWADDR */
		    if (!a[0] && !a[1] && !a[2] && !a[3] && !a[4] && !a[5])
			    continue;
            macaddr_t mac;
		    memcpy(mac.addr, a, sizeof(mac.addr));
            vecMacs.push_back(mac);
            /*
            printf(
                    "a=%X-%X-%X-%X-%X-%X\n", 
                    (int)a[0],
                    (int)a[1],
                    (int)a[2],
                    (int)a[3],
                    (int)a[4],
                    (int)a[5]);
            */
        };
	    close(sd);
	    return 0;
    };
};
#endif // __unix
