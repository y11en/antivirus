#ifndef __SRVDEFERREDACTIONS_CONST_H__
#define __SRVDEFERREDACTIONS_CONST_H__

namespace KLSRV
{
    typedef enum 
    {
        DFAM_HST = 1,
        DFAM_GSYN = 2,
        DFAM_POLICY = 3,        
    }
    dfr_module_t;


    typedef enum 
    {
        DFEO_LEVEL1 = 10,
        DFEO_LEVEL2 = 20,
        DFEO_LEVEL3 = 30,
        DFEO_CUSTOM = 200,
        DFEO_HST = DFEO_CUSTOM + 1,
        DFEO_GSYN = DFEO_CUSTOM + 2,
        DFEO_POLICY = DFEO_CUSTOM + 3
    }
    dfr_execorder_t;
};

#endif //__SRVDEFERREDACTIONS_CONST_H__
