#ifndef __KLSRV_CHUNKACCESSOR_H__
#define __KLSRV_CHUNKACCESSOR_H__

namespace KLSRV
{
    class KLSTD_NOVTABLE SrvChunkAccessorWithRs
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void SetRecordset(KLDB::DbRecordset * pRs) = 0;
    };

    class KLSTD_NOVTABLE SrvChunkAccessor
        :   public KLTMSG::TimeoutObject
    {
    public:
		virtual size_t GetTotalCount() = 0;

        virtual size_t GetChunk(
						size_t          nStart,
						size_t          nCount,
						KLPAR::Params** ppInfo) = 0;

        virtual size_t GetNextChunk(
                        size_t              nCount,
						KLPAR::ArrayValue** ppInfo,
                        bool&               bEof) = 0;
    /*!
      \brief	creates a tie between item and connection and puts item to the timeoutstorage

      \param	pHelper - helper to tie with connection
      \param	pTs - timeoutstorage
      \param	pObject - item to store
      \param	wstrKey key (the same both for pHelper and pTs)
      \param	wstrLocalConnectionName
      \param	wstrRemoteConnectionName
      \param	lTimeout timeout, ms
    */
        virtual void CreateTie(
            CONTIE::ConnectionTiedObjectsHelper*    pHelper,
            KLTMSG::TimeoutStore2*                  pTs,
            const std::wstring&                     wstrKey,
            const std::wstring&                     wstrLocalConnectionName,
            const std::wstring&                     wstrRemoteConnectionName,
            long                                    lTimeout) = 0;

        virtual void DestroyTie(const std::wstring& wstrKey) = 0;
        
        virtual void PutData(KLSTD::KLBaseQI* pObject) = 0;

        virtual void GetData(KLSTD::KLBaseQI** ppObject) = 0;

        virtual void GetQI(KLSTD::KLBaseQI** pp) = 0;
    };

    typedef KLSTD::CAutoPtr<SrvChunkAccessor> SrvChunkAccessorPtr;

    SrvChunkAccessorPtr Create(
                KLDB::DbRecordsetPtr                pRs,
				const std::vector<std::wstring>&    vect_fields);
    
    SrvChunkAccessorPtr Create(
                KLDB::DbRecordsetPtr    pRs,
				const wchar_t**         pFileds,
                size_t                  nFileds);

};

#endif //__KLSRV_CHUNKACCESSOR_H__
