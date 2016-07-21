#if !defined(AFX_PRAGUELOG_H__398B853A_E8E0_46C5_9158_A473B31F60F3__INCLUDED_)
#define AFX_PRAGUELOG_H__398B853A_E8E0_46C5_9158_A473B31F60F3__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <Prague/prague.h>

#include "../../KeepUp2Date/Client/include/log_iface.h"

namespace PRAGUE_HELPERS {

class PragueLog : public KLUPD::Log
{
public:
    PragueLog(const char *decorationPrefix, const tTRACE_LEVEL &traceLevel);
    virtual void print(const char *format, ...);

private:
    // disable copy operations
    PragueLog &operator=(const PragueLog &);
    PragueLog(const PragueLog &);


    const std::string m_decorationPrefix;
    const tTRACE_LEVEL m_traceLevel;
};

}   // namespace PRAGUE_HELPERS

#endif // !defined(AFX_PRAGUELOG_H__398B853A_E8E0_46C5_9158_A473B31F60F3__INCLUDED_)
