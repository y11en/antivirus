#ifndef __KLCONNECTIONTIEDOBJECTSHELPER_H__
#define __KLCONNECTIONTIEDOBJECTSHELPER_H__

#include <std/base/klstd.h>

namespace CONTIE
{
	class KLSTD_NOVTABLE DisconnectSink : public KLSTD::KLBase
	{
	public:
		virtual void OnDisconnectObjectID(
                            const std::wstring& wstrObject) = 0;
	};

    class KLSTD_NOVTABLE ConnectionTiedObjectsHelper
        :   public KLSTD::KLBaseQI
	{
    public:
        KLSTD_NOTHROW virtual void Close() throw() = 0;

		virtual void AddObject(
                    const std::wstring& wstrObjID,
                    DisconnectSink*     pSink,
			        const std::wstring& wstrLocalConnectionName,
			        const std::wstring& wstrRemoteConnectionName) = 0;

		KLSTD_NOTHROW virtual void RemoveObject(
                    const std::wstring& wstrObjID) throw() = 0;
	};
};

KLCSKCA_DECL void CONTIE_CreateConnectionTiedObjectsHelper(
            CONTIE::ConnectionTiedObjectsHelper** ppHelper);

#endif //__KLCONNECTIONTIEDOBJECTSHELPER_H__
