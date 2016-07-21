#ifndef __AKLWNGT_H__
#define __AKLWNGT_H__

#include <transport/tr/transport.h>

struct soap;
struct aklwngt__sync_info;
struct aklwngt__command;
struct aklwngt__event_info_array;
struct aklwngt__klft_file_info_t;
struct aklwngt__klft_file_chunk_t;

namespace AKLWNGT
{
    class KLSTD_NOVTABLE AkLwNgt
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void SessionBegin(
                        struct soap*                        pSoap,
						const char*                         szDeviceId,
                        const char*                         szPassword,
                        int&                                nProtocolVersion,
                        std::string&                        strSessionId) = 0;

        virtual void SessionEnd(
                        struct soap*                        pSoap,
                        const char*                         szSessionId) = 0;

        virtual void Heartbeat(
                        struct soap*                        pSoap,
                        const char*                         szSessionId,
                        struct aklwngt__sync_info*          pMaySynchronize,
                        struct aklwngt__command&            oCommand) = 0;

        virtual void PutResult(
                        struct soap*                        pSoap,
                        const char*                         szSessionId,
                        struct aklwngt__command*            pResponse,
                        struct aklwngt__command&            oNextCommand) = 0;

        virtual void PutEvents(
                        struct soap*                        pSoap,
                        const char*                         szSessionId,
                        struct aklwngt__event_info_array*   pEventsInfoArray) = 0;

        virtual void FtGetFileInfo(
                        struct soap*                        pSoap,
                        const char*                         szSessionId,
						const wchar_t*                      szwFileName,
                        struct aklwngt__klft_file_info_t&   info) = 0;

        virtual void FtGetNextFileChunk(
                        struct soap*                        pSoap,
                        const char*                         szSessionId,
						const wchar_t*                      szwFileName,
						AVP_qword                           nStartFilePos,
						AVP_qword                           nNeededSize,
                        struct aklwngt__klft_file_chunk_t&  oChunk) = 0;

        virtual void RestoreContextIfNeeded(
                        struct soap*    pSoap,
						const char*     szSessionId) = 0;
    };
};

#endif //__AKLWNGT_H__
