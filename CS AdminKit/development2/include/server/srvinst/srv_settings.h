#ifndef __SRV_SETTINGS_H__
#define __SRV_SETTINGS_H__

#include<std/par/par_conv.h>

namespace KLSRV
{
    class SectionsStore
        :   public  KLSTD::KLBaseQI
    {
    public:
        virtual void Destroy() = 0;
        ;
        //! returns specified parameter. One shouldn't change returned data.
        virtual KLSTD::CAutoPtr<KLPAR::Value> GetParameter(
                            const wchar_t* szwName,
                            const wchar_t* szwSection,
                            const wchar_t* szwProduct, 
                            const wchar_t* szwVersion) = 0;
        
        //! returns specified parameters. One shouldn't change returned data.
        virtual KLPAR::ParamsPtr GetParameters(
                            const wchar_t** szwNames,
                            size_t          nNames,
                            const wchar_t*  szwSection,
                            const wchar_t*  szwProduct, 
                            const wchar_t*  szwVersion) = 0;
        
        //! adding 
        virtual KLSRV::HCHANGESINK Advise(
                            KLSRV::SrvSettingsChangeSink *pSink,
                            const wchar_t* szwSection,
                            const wchar_t* szwProduct, 
                            const wchar_t* szwVersion) = 0;

        virtual void Unadvise(KLSRV::HCHANGESINK hSink) = 0;
    };
};

void KLSRV_CreateSectionsStore(
                        KLSTD::CriticalSection*     pCS,
                        KLPRCI::ComponentInstance*  pInst,
                        KLSRV::SectionsStore**      pp);

#endif //__SRV_SETTINGS_H__
