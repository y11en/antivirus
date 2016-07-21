#ifndef __UPGRSETT_H__
#define __UPGRSETT_H__

#include "../common/creg.h"

class IUpgrTracer
{
public:
    virtual ~IUpgrTracer () {};

    virtual void printf(const char * str...) = 0;
};

void upgradesettings_avp6_avp7(AVPUpgradeSettings::CRegKey& srckey, AVPUpgradeSettings::CRegKey& dstkey, IUpgrTracer* tracer);

#endif //__UPGR_H__