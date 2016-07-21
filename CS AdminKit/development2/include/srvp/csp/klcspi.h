/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspi.h
 * \author	Andrew Kazachkov
 * \date	16.06.2003 20:21:32
 * \brief	internal
 * 
 */

#ifndef __KL__CSPI_H__
#define __KL__CSPI_H__

#include "./klcsp.h"
#include <transport/tr/transportproxy.h>

namespace KLCSP
{
    class KLSTD_NOVTABLE IteratorHost : public KLSTD::KLBase
    {
    public:
        virtual bool OnGetNextItems(
                        const wchar_t*  szwIterator,
                        long&           lCount,
                        KLPAR::Params** ppData) = 0;
        
        KLSTD_NOTHROW virtual void OnReleaseIterator(
                        const wchar_t* szwIterator) throw() = 0;
    };

	class KLSTD_NOVTABLE ChunkAccessorImp : public KLSTD::KLBase
	{
	public:
		virtual long GetRecordsCount(const wchar_t* szwChunkAccessor)=0;
		virtual long GetChunk(const wchar_t* szwChunkAccessor, long lStart, long lCount, KLPAR::Params** ppData)=0;
		virtual void ReleaseChunkAccessor(const wchar_t* szwChunkAccessor)=0;
	};
};

void KLCSP_CreateForwardIterator(
                                    const wchar_t*              szwIterator,
                                    KLCSP::IteratorHost*        pHost,
                                    KLCSP::ForwardIterator**    ppIterator);

void KLCSP_CreateChunkAccessor(
                                    const wchar_t*              szwAccessor,
                                    KLCSP::ChunkAccessorImp*    pChunkAccessorImp,
                                    KLCSP::ChunkAccessor**		ppAccessor);

bool KLCSP_OnGetNextItems(
                          KLTRAP::TransportProxy*   pProxy,
                          const wchar_t*            szwIterator,
                          long&                     lCount,
                          KLPAR::Params**           ppData);

KLSTD_NOTHROW void KLCSP_OnReleaseIterator(
                          KLTRAP::TransportProxy*   pProxy,
                          const wchar_t*            szwIterator) throw();

long KLCSP_OnGetRecordsCount(
						KLTRAP::TransportProxy*   pProxy,
                        const wchar_t*            szwChunkAccessor);

long KLCSP_OnGetChunk(
					KLTRAP::TransportProxy*   pProxy,
					const wchar_t*            szwChunkAccessor,
					long						lStart,
					long						lCount,
					KLPAR::Params**           ppData);

KLSTD_NOTHROW void KLCSP_OnReleaseChunkAccessor(
                          KLTRAP::TransportProxy*   pProxy,
                          const wchar_t*            szwChunkAccessor) throw();

#endif //__KL__CSPI_H__