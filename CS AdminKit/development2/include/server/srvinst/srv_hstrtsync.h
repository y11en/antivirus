#ifndef __KLSRV_HSTRTSYNC_H__
#define __KLSRV_HSTRTSYNC_H__

#include <server/evp/transeventsource.h>

namespace KLSRV
{
    class KLSTD_NOVTABLE SrvHstRtSync
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void Ping_UpdateProductStates(
                    KLDB::DbConnectionPtr   pCon,
                    long                    lHost, 
                    KLPAR::Params*          pData,
					bool*					pbProductVersionChanged) = 0;
        
        virtual void UpdateHostHasRtp(
                    KLDB::DbConnectionPtr   pCon,
                    const std::wstring&     wstrHost,
					long				    idHost) = 0;
    };
};

#endif //__KLSRV_HSTRTSYNC_H__
