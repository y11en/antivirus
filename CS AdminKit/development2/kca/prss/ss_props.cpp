/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_props.cpp
 * \author	Andrew Kazachkov
 * \date	06.10.2003 20:51:05
 * \brief	
 * 
 */


#include "std/err/klerrors.h"
#include "std/conv/klconv.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "kca/prci/simple_params.h"
#include "kca/prss/helpersi.h"
#include <sstream>

#include "std/trc/trace.h"
#include "common/measurer.h"

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

namespace KLPRSS
{
	KLCSKCA_DECL void ForceCreateEntry(
						KLPRSS::Storage*		pStorage,
                        const wchar_t*          szwProduct,
                        const wchar_t*          szwVersion,
                        const wchar_t*          szwSection)
	{		
		const wchar_t* path0[]={NULL};
		KLPRSS_MAKE_PATH1(
			path1, 
			szwProduct);
		KLPRSS_MAKE_PATH2(
			path2, 
			szwProduct,
			szwVersion);

		pStorage->region_load(path0, true);
		pStorage->entry_create(path1[0], false);
		
		pStorage->region_load(path1, true);
		pStorage->entry_create(path2[1], false);
		
		pStorage->region_load(path2, true);
		pStorage->entry_create(szwSection, false);
	}
	
    KLCSKCA_DECL void SS_OnSaving(Storage* pStorage)
    {
        if(pStorage->get_changed())
        {
            WriteStoreID(pStorage, KLSTD_CreateLocallyUniqueString());
            SetStoreChanged(pStorage, true);
        };
    };

    template<class P, class T, KLPAR::Value::Types idtype>
        class CSsPropsT
    {
    public:
        static bool Read_SS_Val(
                    KLPAR::Params*  pVerData,
                    const wchar_t*  name,
                    const T&        valDefault,
                    T&              x);

        static void Write_SS_Val(
                    KLPAR::Params*  pVerData,
                    const wchar_t*  name,
                    const T&        x)
        {
            CAutoPtr<P> p_x;
            CreateValue(x, &p_x);
            pVerData->ReplaceValue(name, p_x);
        };

        static bool Read_SS_Val(
                    KLPRSS::Storage* pStorage,
                    const wchar_t*  name,
                    const T&        valDefault,
                    T&              x);

        static void Write_SS_Val(
                    KLPRSS::Storage* pStorage,
                    const wchar_t*  name,
                    const T&        x);
    };

    template <class P, class T, KLPAR::Value::Types idtype>
        bool CSsPropsT<P, T, idtype>::Read_SS_Val(
                    KLPRSS::Storage* pStorage,
                    const wchar_t*  name,
                    const T&        valDefault,
                    T&              x)
    {
		KLPRSS_MAKE_PATH3(
			path,
			KLPRSS_PRODUCT_CORE,
			KLPRSS_VERSION_INDEPENDENT,
			KLPRSS_VERSION_INFO);

		CAutoPtr<KLPAR::Params> pVerData;        
		KLERR_BEGIN
			pStorage->region_load(path, false);
			pStorage->entry_read(&pVerData);
			return Read_SS_Val(pVerData, name, valDefault, x);
		KLERR_ENDT(3);
        x = valDefault;
        return false;
    };

    template <class P, class T, KLPAR::Value::Types idtype>
        void CSsPropsT<P, T, idtype>::Write_SS_Val(
                    KLPRSS::Storage* pStorage,
                    const wchar_t*  name,
                    const T&        x)
    {
		ForceCreateEntry(
                    pStorage,
                    KLPRSS_PRODUCT_CORE,
                    KLPRSS_VERSION_INDEPENDENT,
                    KLPRSS_VERSION_INFO);
		CAutoPtr<Params> pEntry;
		KLPAR_CreateParams(&pEntry);
        Write_SS_Val(pEntry, name, x);
		KLPRSS_MAKE_PATH3(
			path,
			KLPRSS_PRODUCT_CORE,
			KLPRSS_VERSION_INDEPENDENT,
			KLPRSS_VERSION_INFO);
		pStorage->region_load(path, true);
		pStorage->entry_write(CF_OPEN_ALWAYS, pEntry);
    };

    template <class P, class T, KLPAR::Value::Types idtype>
        bool CSsPropsT<P, T, idtype>::Read_SS_Val(
                    KLPAR::Params*  pVerData,
                    const wchar_t*  name,
                    const T&        valDefault,
                    T&              x)
    {
        bool bResult=false;
        KLSTD::CAutoPtr<P> p_x=
            (P*)pVerData->GetValue2(name, false);
        if(p_x && p_x->GetType() == idtype)
        {
            x=p_x->GetValue();
            bResult=true;
        }
        else
            x=valDefault;
        return bResult;
    };

    void WriteStoreFmtId(KLPRSS::Storage* pStorage, store_fmt_type_t nID)
    {
        CSsPropsT<IntValue, long, Value::INT_T>::
            Write_SS_Val(pStorage, c_wstrSsId, long(nID));
    };
    
/*******************
c_wstrSsId
*******************/
    void WriteStoreID(KLPAR::Params* pVerData, const std::wstring& wstrId)
    {
        CSsPropsT<StringValue, const wchar_t*, Value::STRING_T>::
            Write_SS_Val(pVerData, c_wstrSsId, wstrId.c_str());
    };

    void WriteStoreID(KLPRSS::Storage* pStorage, const std::wstring& wstrId)
    {
        CSsPropsT<StringValue, const wchar_t*, Value::STRING_T>::
            Write_SS_Val(pStorage, c_wstrSsId, wstrId.c_str());
    };

    bool RetrieveStoreID(KLPRSS::Storage* pStorage, std::wstring& wstrId)
    {
        return CSsPropsT<StringValue, std::wstring, Value::STRING_T>::
            Read_SS_Val(pStorage, c_wstrSsId, L"", wstrId);
    };

    bool RetrieveStoreID(KLPAR::Params* pVerData, std::wstring& wstrId)
    {
        return CSsPropsT<StringValue, std::wstring, Value::STRING_T>::
            Read_SS_Val(pVerData, c_wstrSsId, L"", wstrId);
    };

/*******************
c_wstrSsClientId
*******************/
    
    void SetStoreClientId(KLPRSS::Storage* pStorage, const std::wstring& wstrId)
    {
        CSsPropsT<StringValue, const wchar_t*, Value::STRING_T>::
            Write_SS_Val(pStorage, c_wstrSsClientId, wstrId.c_str());
    };

    bool GetStoreClientId(KLPRSS::Storage* pStorage, std::wstring& wstrId)
    {
        return CSsPropsT<StringValue, std::wstring, Value::STRING_T>::
            Read_SS_Val(pStorage, c_wstrSsClientId, L"", wstrId);
    };

/*******************
c_wstrSsServerId
*******************/

    void SetStoreServerId(KLPRSS::Storage* pStorage, const std::wstring& wstrId)
    {
        CSsPropsT<StringValue, const wchar_t*, Value::STRING_T>::
            Write_SS_Val(pStorage, c_wstrSsServerId, wstrId.c_str());
    };

    void SetStoreServerId(KLPAR::Params* pVerData, const std::wstring& wstrId)
    {
        CSsPropsT<StringValue, const wchar_t*, Value::STRING_T>::
            Write_SS_Val(pVerData, c_wstrSsServerId, wstrId.c_str());
    };

    bool GetStoreServerId(KLPRSS::Storage* pStorage, std::wstring& wstrId)
    {
        return CSsPropsT<StringValue, std::wstring, Value::STRING_T>::
            Read_SS_Val(pStorage, c_wstrSsServerId, L"", wstrId);
    };

    bool GetStoreServerId(KLPAR::Params* pVerData, std::wstring& wstrId)
    {
        return CSsPropsT<StringValue, std::wstring, Value::STRING_T>::
            Read_SS_Val(pVerData, c_wstrSsServerId, L"", wstrId);
    };

/*******************
c_wstrSsChanged
*******************/

    void SetStoreChanged(KLPRSS::Storage* pStorage, bool bChanged)
    {
        CSsPropsT<BoolValue, bool, Value::BOOL_T>::
            Write_SS_Val(pStorage, c_wstrSsChanged, bChanged);
    }

    KLCSKCA_DECL bool GetStoreChanged(KLPRSS::Storage* pStorage)
    {
        bool bChanged;
        CSsPropsT<BoolValue, bool, Value::BOOL_T>::
            Read_SS_Val(pStorage, c_wstrSsChanged, false, bChanged);
        return bChanged;
    }
    
/*******************
c_wstrSsIsNew
*******************/

    void SetStoreNew(KLPRSS::Storage* pStorage, bool bNew)
    {
        CSsPropsT<BoolValue, bool, Value::BOOL_T>::
            Write_SS_Val(pStorage, c_wstrSsIsNew, bNew);
    };

    void SetStoreNew(KLPAR::Params* pVerData, bool bNew)
    {
        CSsPropsT<BoolValue, bool, Value::BOOL_T>::
            Write_SS_Val(pVerData, c_wstrSsIsNew, bNew);
    };

    bool GetStoreNew(KLPRSS::Storage* pStorage)
    {
        bool bNew;
        CSsPropsT<BoolValue, bool, Value::BOOL_T>::
            Read_SS_Val(pStorage, c_wstrSsIsNew, true, bNew);
        return bNew;
    };

    bool GetStoreNew(KLPAR::Params* pVerData)
    {
        bool bNew;
        CSsPropsT<BoolValue, bool, Value::BOOL_T>::
            Read_SS_Val(pVerData, c_wstrSsIsNew, true, bNew);
        return bNew;
    };

/*******************
c_wstrSyncVersion
*******************/
    /*
    void WriteStoreVersion(KLPRSS::Storage* pStorage, AVP_qword qwVersion)
    {
        CSsPropsT<LongValue, AVP_qword, Value::LONG_T>::
            Write_SS_Val(pStorage, c_wstrSyncVersion, qwVersion);
    };

    void WriteStoreVersion(KLPAR::Params* pVerData, AVP_qword qwVersion)
    {
        CSsPropsT<LongValue, AVP_qword, Value::LONG_T>::
            Write_SS_Val(pVerData, c_wstrSyncVersion, qwVersion);
    };

    AVP_qword RetrieveStoreVersion(KLPRSS::Storage* pStorage)
    {
        AVP_qword qwVersion;
        CSsPropsT<LongValue, AVP_qword, Value::LONG_T>::
            Read_SS_Val(pStorage, c_wstrSyncVersion, true, qwVersion);
        return qwVersion;
    };

    AVP_qword RetrieveStoreVersion(KLPAR::Params* pVerData)
    {
        AVP_qword qwVersion;
        CSsPropsT<LongValue, AVP_qword, Value::LONG_T>::
            Read_SS_Val(pVerData, c_wstrSyncVersion, true, qwVersion);
        return qwVersion;
    };

    AVP_qword IncrementVersion(KLPRSS::Storage* pStorage)
    {
        AVP_qword qwResult=RetrieveStoreVersion(pStorage)+1;
        WriteStoreVersion(pStorage, qwResult);
        return qwResult;
    };
    */
};
