#ifndef __KL_CONN_NLIF_WRAPPER_H__
#define __KL_CONN_NLIF_WRAPPER_H__


#include <nagent/conn/networklist.h>
#include <nagent/connapp/conn_nlif.h>

#ifndef KLCS_MODULENAME
    #define KLCS_MODULENAME L"KlAkHelper"
    #define __KL_CONN_NLIF_WRAPPER_H__UNDEF_MOD
#endif

namespace KLCONNAPPINST
{
    class CNetworkListItemsFileWrapper
        :   public KLSTD::KLBaseImpl<KLCONN::NetworkListItemsFile>
    {
    public:
        CNetworkListItemsFileWrapper(
                    KLCONNAPP::NliFlat*     pNliFlat,
                    KLNLIF                  nlifID)
            :   m_pNliFlat(pNliFlat)
            ,   m_nlifID(nlifID)
        {
            KLSTD_CHKINPTR(pNliFlat);
            KLSTD_CHKINPTR(nlifID);
        };
        virtual ~CNetworkListItemsFileWrapper()
        {
            if(m_pNliFlat && m_nlifID)
            {
                KLERR_IGNORE(m_pNliFlat->DestroyKLNLIF(m_nlifID));
            };
        };
        KLSTD_INTERAFCE_MAP_BEGIN(KLCONN::NetworkListItemsFile)
        KLSTD_INTERAFCE_MAP_END()
    protected:
        virtual void GetFileInfo( KLPAR::Params** ppFileInfo )
        {
            m_pNliFlat->GetFileInfo(m_nlifID, ppFileInfo);
        };

        virtual FileOperationResult GetFileChunk( 
                                        AVP_qword startPos, 
			                            void *chunkBuff, 
                                        size_t chunkBuffSize, 
                                        size_t &receivedChunkSize )
        {
            return m_pNliFlat->GetFileChunk(
                        m_nlifID, 
                        startPos,
                        chunkBuff,
                        chunkBuffSize,
                        receivedChunkSize);
        };
    protected:
        KLCONNAPP::NliFlat*     m_pNliFlat;
        KLNLIF                  m_nlifID;
    };
};

#ifdef __KL_CONN_NLIF_WRAPPER_H__UNDEF_MOD
    #undef KLCS_MODULENAME
#endif


#endif //__KL_CONN_NLIF_WRAPPER_H__
