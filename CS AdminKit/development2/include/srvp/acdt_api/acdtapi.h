#include <std/base/klstd.h>
#include <kca/acdt/acdt_const.h>

#ifndef __ACDTAPI_H__
#define __ACDTAPI_H__

namespace KLADMSRV
{
    class AdmServer;
};

namespace KLACDTAPI
{
    class KLSTD_NOVTABLE ACDTFile
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual AVP_dword   Read(void* pBuffer, AVP_dword dwBuffer) = 0;
        virtual AVP_qword	Seek(AVP_longlong llOffset, KLSTD::SEEK_TYPE type) = 0;
        virtual AVP_qword	GetSize() = 0;
    };


    class KLSTD_NOVTABLE AcdtActionCallback
    {
    public:
        virtual void SetExecutionPercentAndDescription(
                        int             nPercent, 
                        const wchar_t*  szwDescription) = 0;
    };

    class KLSTD_NOVTABLE AcdtHost
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void ExecuteAction(
                        const wchar_t*      szwAction,
                        AcdtActionCallback* pCallback,
                        KLSTD::KLBaseQI**   ppResult,
                        const wchar_t*      szwPar1 = NULL,
                        const wchar_t*      szwPar2 = NULL,
                        const wchar_t*      szwPar3 = NULL,
                        const wchar_t*      szwPar4 = NULL,
                        const wchar_t*      szwPar5 = NULL,
                        const wchar_t*      szwPar6 = NULL,
                        const wchar_t*      szwPar7 = NULL,
                        const wchar_t*      szwPar8 = NULL,
                        const wchar_t*      szwPar9 = NULL) = 0;

        virtual void CancelExecution() = 0;
    };
    
    typedef KLSTD::CAutoPtr<AcdtHost> AcdtHostPtr;
};


void KLCSSRVP_DECL KLACDTAPI_ConnectThroughAdmServer(
                KLADMSRV::AdmServer*            pAdmServer,
                const wchar_t*                  szwHostId,
                KLACDTAPI::AcdtActionCallback*  pCallback,
                KLACDTAPI::AcdtHost**           ppAcdtHost);

void KLCSSRVP_DECL KLACDTAPI_ConnectThroughCifs(
                const wchar_t*                  szwAddress,
                const wchar_t*                  szwUserName,
                const wchar_t*                  szwPassword,
                KLACDTAPI::AcdtActionCallback*  pCallback,
                KLACDTAPI::AcdtHost**           ppAcdtHost);

#endif //__ACDTAPI_H__
