#ifndef __KLSTD_MACADDR_H__
#define __KLSTD_MACADDR_H__

#ifdef __unix

#include <vector>

namespace KLSTD
{
    struct macaddr_t
    {
        unsigned char addr[6];
    };

    typedef std::vector<macaddr_t> vec_macs_t;

    int get_macaddresses(vec_macs_t& vecMacs);
};
#endif

#endif //__KLSTD_MACADDR_H__
