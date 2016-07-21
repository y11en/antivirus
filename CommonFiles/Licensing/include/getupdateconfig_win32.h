#if !defined (__GETUPDATECONFIG_WIN32_H__)
#define __GETUPDATECONFIG_WIN32_H__

#include "updateconfig.h"

namespace LicensingPolicy
{
    namespace UpdateConfig
    {
        HRESULT Win32GetUpdateConfig (const file_image_t&    xmlImage,
                                      update_config_t        *config
                                     );
    }
}

#endif
