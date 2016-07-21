#ifndef PRAGUEENVIRONMENTWRAPPER_H_INCLUDED_9A07B71E_6DDC_4ae9_B50F_D1725B77109A
#define PRAGUEENVIRONMENTWRAPPER_H_INCLUDED_9A07B71E_6DDC_4ae9_B50F_D1725B77109A

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../../KeepUp2Date/Client/helper/stdinc.h"

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>


namespace PRAGUE_HELPERS {

class PragueEnvironmentWrapper
{
public:
    virtual bool expandEnvironmentString(const KLUPD::NoCaseString &input, KLUPD::NoCaseString &output, const KLUPD::StringParser::ExpandOrder &);

private:
    // environment variables cache implementation
    struct EnvironmentCacheItem
    {
        EnvironmentCacheItem(const bool result = false, const KLUPD::NoCaseString &output = KLUPD::NoCaseString());

        bool m_result;
        KLUPD::NoCaseString m_output;
    };
    typedef std::map<KLUPD::NoCaseString, EnvironmentCacheItem> EnvironmentCache;
    EnvironmentCache m_environmentCache;
};

}   // namespace PRAGUE_HELPERS

#endif  // PRAGUEENVIRONMENTWRAPPER_H_INCLUDED_9A07B71E_6DDC_4ae9_B50F_D1725B77109A
