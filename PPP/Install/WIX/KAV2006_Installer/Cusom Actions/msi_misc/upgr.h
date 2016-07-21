#ifndef __UPGR_H__
#define __UPGR_H__

#include "creg.h"

class IUpgrTracer
{
public:
    virtual ~IUpgrTracer () {};

    virtual void printf(const char * str...);
};

#define UPGR_TRACE(LIST) { if (tracer) tracer->printf(LIST); }


void upgrade_avp6_avp7(CRegKey& srckey, CRegKey& dstkey, IUpgrTracer* tracer);

#endif //__UPGR_H__