#ifndef __HST_NAGENT_H__
#define __HST_NAGENT_H__

#include <kca/prcp/componentproxy.h>

namespace KLHST
{
    class KLSTD_NOVTABLE HostNagent
        :   public KLSTD::KLBaseQI
    {
    public:        
        virtual void GetNagentProxy(
                        const wchar_t* szwHost, 
                        KLPRCP::ComponentProxy** ppNagent) = 0;
    };
};

#endif //__HST_NAGENT_H__
