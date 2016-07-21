#ifndef __KLDBUTILS_H__
#define __KLDBUTILS_H__

#include <server/db/dbconnection.h>
#include <srvp/csp/klcsp.h>

#define KLDB_GET_VALUE(pRecordset, idx) KLDB::DbVal(pRecordset->Fields()->Item(idx))
#define KLDB_GET_VALUE_NULLABLE(pRecordset, idx, dfltVal) (pRecordset->Fields()->Item(idx)->IsEmpty()? dfltVal : KLDB::DbVal(pRecordset->Fields()->Item(idx)))

namespace KLDB
{
    void InsertRecordToTableFromParams(
		    const std::wstring&                 wstrTable,
		    KLSTD::CAutoPtr<KLPAR::Params>      parRecord,
            KLDB::DbConnectionPtr               pCon,
		    const std::vector<std::wstring>*    pvecFields,
            bool                                bTempTable,
			bool								bFieldsUse);

    void WriteRecordToParams(
                    KLDB::DbFieldsPtr   flds, 
                    const wchar_t**     ppFields, 
                    size_t              nFields, 
                    KLPAR::ParamsPtr&   pParams);

    void WriteRecordToParamsByName(
                    KLDB::DbFieldsPtr   flds, 
                    const wchar_t**     ppFields, 
                    size_t              nFields, 
                    KLPAR::ParamsPtr&   pParams);

    void WriteRecordToParams(
                    KLDB::DbFieldsPtr				 flds, 
					const std::vector<std::wstring>& vecFields, 
                    KLPAR::ParamsPtr&				 pParams);

    void WriteRecordToParamsByName(
                    KLDB::DbFieldsPtr				 flds, 
					const std::vector<std::wstring>& vecFields, 
                    KLPAR::ParamsPtr&				 pParams);

    void WriteWholeRecordToParams(
                    KLDB::DbFieldsPtr   flds, 
                    KLPAR::ParamsPtr&   pParams);

    void WriteRecordsetToParamsVector( 
		KLDB::DbRecordsetPtr pRecordset,
		long nCount,
		std::vector< KLSTD::KLAdapt< KLSTD::CAutoPtr< KLPAR::Params > > > & vectParams );

    std::wstring GetStringValue(
                        KLDB::DbRecordsetPtr pRecordset, 
                        const db_field_index_t& ixFieldName, 
                        const wchar_t* pcszDefault = NULL );

    long GetLongValue(
                        KLDB::DbRecordsetPtr pRecordset, 
                        const db_field_index_t& ixFieldName, 
                        long nDefault = 0 );

    time_t GetDateTimeValue(
                        KLDB::DbRecordsetPtr pRecordset, 
                        const db_field_index_t& ixFieldName, 
                        time_t tDefault = 0 );/*Why 0 ?*/

    template<class T>
        T GetDbValue(
                KLDB::DbRecordsetPtr    pRecordset,
                const db_field_index_t& ixFieldName, 
                const T&                valDefault)
    {
        KLDB::DbValuePtr varValue = pRecordset->Fields()->Item(ixFieldName);
        if(varValue->IsEmpty())
            return valDefault;
        return (T)KLDB::DbVal(varValue);
    };

    void MakeOrderList(
        const KLCSP::vec_field_order_t&     vecFieldsToOrder,
        std::wiostream&                     os);
	
	void CreateStringDbValue(KLDB::DbValuePtr varValue, const std::wstring& wstr);
	void CreateStringDbValueTrim(KLDB::DbValuePtr varValue, const std::wstring& wstr, size_t nMaxLen);

	void SerializeParamsToDbValue(KLSTD::CAutoPtr<KLPAR::Params> pParams, int nMaxSize, KLDB::DbValuePtr varValue);
}

#endif //__KLDBUTILS_H__
