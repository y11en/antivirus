#ifndef LOCALIZECOMPONENTNAME_H_INCLUDED_B70620C3_4477_450e_A100_125A931AF893
#define LOCALIZECOMPONENTNAME_H_INCLUDED_B70620C3_4477_450e_A100_125A931AF893

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>

#include <vector>
#include <algorithm>

#include "../../KeepUp2Date/Client/helper/NoCaseString.h"

namespace KLUPD {
    class Log;
}

namespace PRAGUE_HELPERS {

    // helper structure to convert component list information read from localization file into cRetrFilter structure
    struct ComponentLocalization
    {
        ComponentLocalization(const tDWORD index = 0, const tDWORD parent = 0,
            const cStrObj &componentName = cStrObj(), const cStrObj &componentIdentificator = cStrObj(), const cStrObj &componentComment = cStrObj());

        tDWORD m_index;
        tDWORD m_parent;
        
        cStrObj m_componentName;
        cStrObj m_componentIdentificator;
        cStrObj m_componentComment;
    };
    typedef std::vector<ComponentLocalization> Components;

    // parse string to get localized component information
    extern bool getLocalizedComponentFromString(const cStrObj &, ComponentLocalization &);
    // read file and forms localization information to write into product profile
    extern bool getLocalizedComponentsInformationFromFile(const KLUPD::NoCaseString &componentLocalizationFileName, Components &, KLUPD::Log *);

}   // namespace PRAGUE_HELPERS

#endif  // LOCALIZECOMPONENTNAME_H_INCLUDED_B70620C3_4477_450e_A100_125A931AF893
