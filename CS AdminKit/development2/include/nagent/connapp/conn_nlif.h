#ifndef __CONN_NLIF_H__
#define __CONN_NLIF_H__


#include <nagent/conn/networklist.h>

namespace KLCONNAPPINST
{
    typedef KLSTD::CAutoPtr<KLCONN::NetworkListItemsFile> NlifPtr;

    KLSTD_DECLARE_HANDLE(KLNLIF);
}

namespace KLCONNAPP
{
    class KLSTD_NOVTABLE NliFlat
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void GetFileInfo(
                                KLCONNAPPINST::KLNLIF idNlif, 
                                KLPAR::Params** ppFileInfo ) = 0;

        virtual KLCONN::NetworkListItemsFile::FileOperationResult 
                    GetFileChunk( 
                                KLCONNAPPINST::KLNLIF idNlif,
                                AVP_qword startPos, 
			                    void *chunkBuff, 
                                size_t chunkBuffSize, 
                                size_t &receivedChunkSize ) = 0;

		virtual void GetItemFile( 
				                const wchar_t*  szwListName,
				                const wchar_t*  szwListItemId,
			                    KLCONNAPPINST::KLNLIF& idNlif ) = 0;

        virtual void DestroyKLNLIF(KLCONNAPPINST::KLNLIF id) = 0;
    };  
};


#endif //__CONN_NLIF_H__
