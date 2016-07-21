#ifndef __KL_DBRECORDSET_H__
#define __KL_DBRECORDSET_H__

#include <server/db/dbvalue.h>

namespace KLDB
{
    typedef AVP_qword   db_row_cnt_t;
    typedef size_t      db_field_cnt_t;

    struct db_field_index_t
    {
        db_field_index_t(db_field_cnt_t nId)
            :   m_bIsNum(true)
            ,   m_nId(nId)
        {;};
        db_field_index_t(const wchar_t* szwId)
            :   m_bIsNum(false)
            ,   m_szwId(szwId)
        {;};
        db_field_index_t(const db_field_index_t& x)
            :   m_bIsNum(x.m_bIsNum)
        {
            if(x.m_bIsNum)
                m_nId = x.m_nId;
            else
                m_szwId = x.m_szwId;
        };
        union
        {
            db_field_cnt_t  m_nId;
            const wchar_t*  m_szwId;
        };
        bool    m_bIsNum;
    };

    class DbFields : public KLSTD::KLBaseQI
    {
    public:        
        /*!
          \brief	Returns dbvalue

          \param	    index - index (string or db_field_cnt_t)
          \return	    DbValuePtr 
          \exception    STDE_BOUND no such index
        */
        virtual DbValuePtr Item(const db_field_index_t& index) = 0;

        /*!
          \brief	Returns count of db filds

          \return	count of fields
        */
        virtual db_field_cnt_t Count() = 0;
        
        /*!
          \brief	Returns column name

          \param	    index - index (string or db_field_cnt_t)
          \return	    column name
          \exception    STDE_BOUND no such index
        */
        virtual std::wstring Name(const db_field_index_t& index) = 0;
    };

    typedef KLSTD::CAutoPtr<DbFields> DbFieldsPtr;

    class DbRecordset;
    typedef KLSTD::CAutoPtr<DbRecordset> DbRecordsetPtr;
    
    const db_row_cnt_t c_nDbPosUnknown  =   db_row_cnt_t(AVP_longlong(-1));    
    const db_row_cnt_t c_nDbPosBOF      =   db_row_cnt_t(AVP_longlong(-2));
    const db_row_cnt_t c_nDbPosEOF      =   db_row_cnt_t(AVP_longlong(-3));

    class DbRecordset : public KLSTD::KLBaseQI
    {
    public:
    // methods
        virtual void Move(db_row_cnt_t offset) = 0;
        virtual void MoveNext() = 0;
        virtual void MovePrevious() = 0;
        virtual void MoveFirst() = 0;
        virtual void MoveLast() = 0;
        virtual void Close() = 0;
        virtual DbRecordsetPtr GetNextRecordset() = 0;
    //properties
        virtual bool DbEOF() = 0;
        virtual bool DbBOF() = 0;
        virtual db_row_cnt_t Position() = 0;
        virtual db_row_cnt_t RecordCount() = 0;
        virtual DbFieldsPtr Fields() = 0;
    };

    //typedef KLSTD::CAutoPtr<DbRecordset> DbRecordsetPtr;
};

#endif //__KL_DBRECORDSET_H__
