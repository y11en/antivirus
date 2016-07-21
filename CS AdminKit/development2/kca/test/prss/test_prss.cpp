#include "build/general.h"
#include "std/base/klbase.h"
#include "std/err/error.h"
#include <std/thr/thread.h>
#include <std/par/paramslogger.h>
#include "std/par/paramsi.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/helpersi.h"
#include "kca/prss/prssconst.h"
#include "kca/prss/cxreplace.h"
#include "kca/prss/errors.h"
#include "../../prss/nameconvertor.h"
#include <sstream>
#include "common/measurer.h"
#include "kca/prss/ss_bulkmode.h"

#include <kca/prss/prssconst.h>
#include <kca/prss/errors.h>

#ifdef _WIN32
# include <crtdbg.h>
#endif

#ifdef N_PLAT_NLM // Novel Netware
# include <ntypes.h>
#endif

#define KLCS_MODULENAME L"PRSSTS"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;

// static int random(int x)
// {
//   return ((__int64)(rand())*(__int64)(x))/(__int64)(RAND_MAX);
// }

bool KLPRSS_HasArrayItemPolicyFmt(KLPAR::Value* pVal)
{
    if(!pVal || pVal->GetType() != KLPAR::Value::PARAMS_T)
        return false;
    KLPAR::Params* pItem=((KLPAR::ParamsValue*)(KLPAR::Value*)pVal)->GetValue();
    return pItem && pItem->DoesExist(KLPRSS::c_szwSPS_Value);
};

static std::wstring genRandString(int nLen, int& summa)
{
	summa+=2*nLen;
	static wchar_t symbols[]=L"1234567890_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int nChars=KLSTD_COUNTOF(symbols)-1;
	wchar_t* szBuffer=(wchar_t*)alloca(sizeof(wchar_t)*(nLen+1));
	for(int i=0; i < nLen; ++i)
      szBuffer[i]=symbols[ /* random(nChars-1)*/ KLSTD_Random(0, nChars-1) ];
	szBuffer[nLen]=0;
	return szBuffer;
}

static void AddValue(
						KLPAR::Params* pParams,
						KLPAR::ValuesFactory* pFactory,
						std::wstring& strName,
						std::wstring& strVal)
{
	CAutoPtr<KLPAR::StringValue> pValue;
	pFactory->CreateStringValue(&pValue);
	pValue->SetValue(strVal.c_str());
	pParams->AddValue(strName, pValue);
}

void TestComplexSsOps(const wchar_t *szFileName);

void testStorage(const wchar_t *szFileName)
{
    TestComplexSsOps(szFileName);
    KLPRSS_RemoveSettingsStorage(szFileName, KLSTD_INFINITE);
	Trace1(KLCS_MODULENAME, L"Creating new storage with simple contents...\n");
	try{
		CAutoPtr<KLPAR::Params>			pParams;
		CAutoPtr<KLPAR::ValuesFactory>	pFactory;

		KLPAR_CreateParams(&pParams);
		KLPAR_CreateValuesFactory(&pFactory);

		const int	nCycles=1;
		const int	nValues=4;

		int nSumma=0;
		for(int i=0; i < nValues; ++i){
			std::wstring name=genRandString(8, nSumma);
			std::wstring value=genRandString(16, nSumma);
			AddValue(pParams, pFactory, name, value);
		}

		CAutoPtr<KLPRSS::SettingsStorage> pStorage;
	    KLPRSS_CreateSettingsStorageDirect(   szFileName,
										KLSTD::CF_OPEN_ALWAYS,
										KLSTD::AF_READ|KLSTD::AF_WRITE,
										&pStorage);
		try{
			pStorage->CreateSection(L"Product1", L"", L"");
		}catch(KLERR::Error* pError){
			pError->Release();
		}
		try{
			pStorage->CreateSection(L"Product1", L"1.0", L"");
		}catch(KLERR::Error* pError){
			pError->Release();
		}
		try{
			pStorage->CreateSection(L"Product1", L"1.0", L"Section1");
		}catch(KLERR::Error* pError){
			pError->Release();
		}
		pStorage->Clear(L"Product1", L"1.0", L"Section1", pParams);
		Trace1(KLCS_MODULENAME, L"... creating new storage with simple contents OK\n");
		Trace1(KLCS_MODULENAME, L"Reading its contents...\n");
		CAutoPtr<KLPAR::Params>			pParams2;
		pStorage->Read(L"Product1", L"1.0", L"Section1", &pParams2);
		Trace1(KLCS_MODULENAME, L"... reading its contents OK\n");
	}catch(KLERR::Error* pError){
		KLERR_SAY_FAILURE(1, pError);
		pError->Release();
	}
}

void testStorage2(const wchar_t *szFileName)
{
	try{
		CAutoPtr<SettingsStorage>		pStorage;
		CAutoPtr<KLPAR::Params>			pParams0, pParams1;
		CAutoPtr<KLPAR::ValuesFactory>	pFactory;

		KLPAR_CreateParams(&pParams0);
		KLPAR_CreateValuesFactory(&pFactory);
		{
			CAutoPtr<StringValue>			pValue;
			CAutoPtr<ParamsValue>			pParamsValue;
			CAutoPtr<KLPAR::Params>			pSubParams;
			
			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value1");
			pParams0->AddValue(L"Name1", pValue);

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value2");
			pParams0->AddValue(L"Name2", pValue);

			pFactory->CreateParamsValue(&pParamsValue);
			pSubParams=pParamsValue->GetValue();
			pParams0->AddValue(L"Node1", pParamsValue);

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value3");
			pSubParams->AddValue(L"Name3", pValue);

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value4");
			pSubParams->AddValue(L"Name4", pValue);			

			pParamsValue=NULL;
			pSubParams=NULL;
			pFactory->CreateParamsValue(&pParamsValue);
			pSubParams=pParamsValue->GetValue();
			pParams0->AddValue(L"Empty Node", pParamsValue);
		};

    	KLPRSS_CreateSettingsStorageDirect(
										szFileName,
										KLSTD::CF_OPEN_ALWAYS,
										KLSTD::AF_READ|KLSTD::AF_WRITE,
										&pStorage);

		try{
			pStorage->CreateSection(L"Product1", L"", L"");
		}catch(KLERR::Error* pError){
			pError->Release();
		}
		try{
			pStorage->CreateSection(L"Product1", L"1.0", L"");
		}catch(KLERR::Error* pError){
			pError->Release();
		}
		try{
			pStorage->CreateSection(L"Product1", L"1.0", L"Section2");
		}catch(KLERR::Error* pError){
			pError->Release();
		}

		Trace1(KLCS_MODULENAME, L"Adding intersecting on node contents...\n");			
			pStorage->Add(L"Product1", L"1.0", L"Section2", pParams0);
		Trace1(KLCS_MODULENAME, L"...Adding intersecting on node contents OK\n");

		{
			CAutoPtr<KLPAR::Params>			pParams;
			CAutoPtr<StringValue>			pValue;
			CAutoPtr<ParamsValue>			pParamsValue;
			CAutoPtr<KLPAR::Params>			pSubParams;
			
			KLPAR_CreateParams(&pParams);
			pFactory->CreateParamsValue(&pParamsValue);
			pSubParams=pParamsValue->GetValue();
			pParams->AddValue(L"Empty Node", pParamsValue);

			Trace1(KLCS_MODULENAME, L"Deleting intersecting on empty node contents...\n");
				pStorage->Delete(L"Product1", L"1.0", L"Section2", pParams);			
			Trace1(KLCS_MODULENAME, L"...Deleting intersecting on empty node contents OK\n");

			Trace1(KLCS_MODULENAME, L"Adding intersecting on empty node contents...\n");
				pStorage->Add(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Adding intersecting on empty node contents OK\n");

			Trace1(KLCS_MODULENAME, L"Updating intersecting on empty node contents...\n");
				pStorage->Update(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Updating intersecting on empty node contents OK\n");

			Trace1(KLCS_MODULENAME, L"Replacing intersecting on empty node contents...\n");
				pStorage->Replace(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Replacing intersecting on empty node contents OK\n");

			// Из дерева будет удален только EmptyNode
			pParamsValue->SetValue(NULL);
			Trace1(KLCS_MODULENAME, L"Deleting node EmptyNode...\n");
				pStorage->Delete(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Deleting node EmptyNode OK\n");
			

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value1");
			pParams->AddValue(L"Name1", pValue);

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value2");
			pParams->AddValue(L"Name2", pValue);

			pParamsValue=NULL;
			pFactory->CreateParamsValue(&pParamsValue);
			pSubParams=pParamsValue->GetValue();

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value3");
			pSubParams->AddValue(L"Name3", pValue);

			pValue=NULL;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(L"Value4");
			pSubParams->AddValue(L"Name4", pValue);
			pParams->AddValue(L"Node1", pParamsValue);

			Trace1(KLCS_MODULENAME, L"Adding intersecting contents...\n");
			{
				bool bOK=false;			
				try{
					pStorage->Add(L"Product1", L"1.0", L"Section2", pParams);
				}
				catch(KLERR::Error* pError)
				{
					bOK=true;
					pError->Release();
				};
				if(!bOK)
					KLSTD_THROW(STDE_GENERAL);			
			}
			Trace1(KLCS_MODULENAME, L"...Adding intersecting contents OK\n");

			Trace1(KLCS_MODULENAME, L"Updating intersecting contents...\n");
				pStorage->Update(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Updating intersecting contents OK\n");

			Trace1(KLCS_MODULENAME, L"Updating intersecting contents...\n");
				pParamsValue=NULL;
				pFactory->CreateParamsValue(&pParamsValue);
				pParamsValue->SetValue(NULL);
				pParams->SetValue(L"Node1", pParamsValue);
				pStorage->Update(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Updating intersecting contents OK\n");

			// Из дерева будет удален Node1
			pParams=NULL;
			pParamsValue=NULL;
			KLPAR_CreateParams(&pParams);			
			pFactory->CreateParamsValue(&pParamsValue);
			pParamsValue->SetValue(NULL);
			pParams->AddValue(L"Node1", pParamsValue);

			Trace1(KLCS_MODULENAME, L"Deleting node Node1...\n");
				pStorage->Delete(L"Product1", L"1.0", L"Section2", pParams);
			Trace1(KLCS_MODULENAME, L"...Deleting node Node1 OK\n");

			Trace1(KLCS_MODULENAME, L"Re-reading the result...\n");
				CAutoPtr<KLPAR::Params>			pParams2;
				pStorage->Read(L"Product1", L"1.0", L"Section2", &pParams2);
			Trace1(KLCS_MODULENAME, L"...re-reading the result OK\n");
		}
	}catch(KLERR::Error* pError){
		KLERR_SAY_FAILURE(1, pError);
		pError->Release();
	}
}

void AcquireRandomParams(int nLevels, int nVars, int nNodes, Params** ppParams, long* pSum);

void ReadFileTiming(const wchar_t* c_szwSS1)
{
    KL_TMEASURE_BEGIN(L"Reading large stores", 1);
        KLSTD_TRACE1(3, L"Using file %ls\n", c_szwSS1);
        const int c_nCycles=10;
        //const wchar_t c_szwSS1[]=L"#subs_1.xml";
        int nSize=0;
        {
            CAutoPtr<File> pFile;
            KLSTD_CreateFile(c_szwSS1, SF_READ, CF_OPEN_EXISTING, AF_READ, EF_SEQUENTIAL_SCAN, &pFile, KLSTD_INFINITE);
            nSize=(int)pFile->GetSize();
        }
        CAutoPtr<SettingsStorage> pSS;
        KLPRSS_CreateSettingsStorageDirect(c_szwSS1, CF_OPEN_EXISTING, AF_READ, &pSS);
        clock_t t1=clock();
        for(int i=0; i < c_nCycles; ++i)
        {
            ParamsNames names;
            pSS->GetNames(L"", L"", names);
        };
        clock_t t2=clock();
        KLSTD_TRACE2(
                    1,
                    L"Reading took %lf ms per storage (%lf b/sec)\n",
                    double(t2-t1)/double(c_nCycles),
                    double(nSize)/(double(t2-t1)/double(c_nCycles))*1000.0);
    KL_TMEASURE_END();
};

void testStorage3(const wchar_t *szFileName)
{	
	CAutoPtr<SettingsStorage> pStorage;
	KLPRSS_CreateSettingsStorageDirect(
										szFileName,
										KLSTD::CF_OPEN_ALWAYS,
										KLSTD::AF_READ|KLSTD::AF_WRITE,
										&pStorage);

    struct prodver_t
    {
        const wchar_t* product;
        const wchar_t* version;
        const wchar_t* section;
    };

	const prodver_t sections[]=
	{
        {L"Product 1", L"1.0.0.0", L"Section 1"},
        {L"Product 1", L"2.0.0.0", L"Section 1"},
        {L"Product 2", L"3.0.0.0", L"Section 1"},
        {L"Product 2", L"2.0.0.0", L"Section 1"},

        {L"Product 1", L"1.0.0.0", L"Section 2"},
        {L"Product 1", L"5.0.0.0", L"Section 2"},
        {L"Product 2", L"2.0.0.0", L"Section 2"},        

		{L"Product 2", L"1.0.0.0", L"Section 3"},
        {L"Product 1", L"1.0.0.0", L"Section 3"},

        {KLPRSS_PRODUCT_NAME, KLPRSS_VERSION_CURRENT, L"Section 4"},		
        {KLPRSS_PRODUCT_NAME, KLPRSS_VERSION_CURRENT, L"Section 3"},
        {L"Product 1", L"1.2.0.0", L"Section 4"},
        {L"Product 1", L"3.0.0.0", L"Section 4"},
        {L"Product 2", L"1.1.0.0", L"Section 4"},
        {L"Product 2", L"1.0.0.0", L"Section 4"},

        {L"Product 1", L"1.0.0.0", L"Section 5"},
        {L"Product 1", L"1.1.0.0", L"Section 5"},

        {L"Product 1", L"1.0.0.1", L"Section 6"},
        {L"Product 1", L"1.1.0.0", L"Section 6"},
        {L"Product 2", L"1.0.0.0", L"Section 6"},
        {L"Product 3", L"1.0.0.0", L"Section 6"},

        {L"Product 1", L"1.0.0.0", L"Section 7"},
        {L"Product 2", L"1.0.0.0", L"Section 7"},

		{L"Product 1", L"1.0.0.0", L"Section 8"},
		{L"Product 1", L"2.0.0.0", L"Section 9"},
        {L"Product 2", L"1.1.0.0", L"Section 10"}
	};

    KLPRSS_MAYEXIST(pStorage->CreateSection(KLPRSS_PRODUCT_CORE, L"", L""));
    KLPRSS_MAYEXIST(pStorage->CreateSection(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L""));
    KLPRSS_MAYEXIST(pStorage->CreateSection(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, KLPRSS_VERSION_INFO));

	const int nLevels=2, nVars=20, nNodes=1;
    //KLPRSS_MAYEXIST(pStorage->CreateSection(L"xxx", L"", L""));
    //KLPRSS_MAYEXIST(pStorage->CreateSection(L"xxxx", L"", L""));
    //KLPRSS_MAYEXIST(pStorage->CreateSection(L"xxxx", L"yyy", L""));
	for(int i=0; i < KLSTD_COUNTOF(sections); ++i)
	{
	    KLPRSS_MAYEXIST(pStorage->CreateSection(sections[i].product, L"", L""));
	    KLPRSS_MAYEXIST(pStorage->CreateSection(sections[i].product, sections[i].version, L""));
        KLPRSS_MAYEXIST(pStorage->CreateSection(sections[i].product, sections[i].version, sections[i].section));
		
		CAutoPtr<Params> pParams;
		long lVars=0;
		AcquireRandomParams(nLevels, nVars, nNodes, &pParams, &lVars);
		clock_t t1=clock();
		pStorage->Replace(sections[i].product, sections[i].version, sections[i].section, pParams);
		clock_t t2=clock();
		Trace1(KLCS_MODULENAME, L"Writing %d vars took %d ms\n", lVars, t2-t1);
	};
    {
        KLPRSS::sections_t Sections;
        CAutoPtr<KLPRSS::SsBulkMode> pBulkMode;
        pStorage->QueryInterface(KLSTD_IIDOF(KLPRSS::SsBulkMode), (void**)&pBulkMode);
        KLSTD_ASSERT_THROW(pBulkMode != NULL);
        pBulkMode->EnumAllWSections(Sections);
        KLSTD_ASSERT_THROW(Sections.m_vecNames.size() == KLSTD_COUNTOF(sections) + 1);
    }
    ;
#ifdef N_PLAT_NLM
    ReadFileTiming(L"sys:/klab/adminkit/test/#subs_1.xml");
    ReadFileTiming(L"sys:/klab/adminkit/test/#SS_SETTINGS.xml");
#else
    ReadFileTiming(L"#subs_1.xml");
    ReadFileTiming(L"#SS_SETTINGS.xml");
#endif
}

bool CompareSyncSSs(const std::wstring& wstrSS1, const std::wstring& wstrSS2)
{
	KLSTD_TRACE2(
		1,
		L"Comparing stores \"%ls\" and \"%ls\"...\n",
		wstrSS1.c_str(),
		wstrSS2.c_str());
	;
	CAutoPtr<SettingsStorage> pStore1, pStore2;	
	KLPRSS_CreateSettingsStorageDirect(
							wstrSS1,
							KLSTD::CF_OPEN_EXISTING,
							KLSTD::AF_READ,
							&pStore1);

	KLPRSS_CreateSettingsStorageDirect(
							wstrSS2,
							KLSTD::CF_OPEN_EXISTING,
							KLSTD::AF_READ,
							&pStore2);

	std::vector<std::wstring> products1, products2;
	pStore1->GetNames(L"", L"", products1);
	pStore2->GetNames(L"", L"", products2);
	if(products1!=products2)
    {
        KLSTD_TRACE0(1, L"Number of products differs");
        return false;
    };
	for(unsigned int i=0; i < products1.size(); ++i)
	{
		std::vector<std::wstring> versions1, versions2;
		pStore1->GetNames(products1[i], L"", versions1);
		pStore2->GetNames(products2[i], L"", versions2);
		if(versions1!=versions2)
        {
            KLSTD_TRACE0(1, L"Number of versions differs");
            return false;
        };
		for(unsigned int j=0; j < versions1.size(); ++j)
		{
			std::vector<std::wstring> sections1, sections2;
			pStore1->GetNames(products1[i], versions1[j], sections1);
			pStore2->GetNames(products2[i], versions2[j], sections2);
			if(sections1!=sections2)
            {
                KLSTD_TRACE0(1, L"Number of sections differs");
                return false;
            };
			for(unsigned int k=0; k < sections1.size(); ++k)
			{
				CAutoPtr<Params> pSettings1, pSettings2;
				pStore1->Read(products1[i], versions1[j], sections1[k], &pSettings1);
				pStore2->Read(products2[i], versions2[j], sections2[k], &pSettings2);
				if(0!=pSettings1->Compare(pSettings2))
                {
                    KLSTD_TRACE3(
                                1,
                                L"Section '%ls'-'%ls'-'%ls' differs\n",
                                products1[i].c_str(),
                                versions1[j].c_str(),
                                sections1[k].c_str());
                    return false;
                };
				CAutoPtr<BoolValue> pChanged1, pChanged2;
				pStore1->AttrRead(products1[i], versions1[j], sections1[k], L"chg", (Value**)&pChanged1);
				KLPAR_CHKTYPE(pChanged1, BOOL_T, L"chg");
				pStore1->AttrRead(products2[i], versions2[j], sections2[k], L"chg", (Value**)&pChanged2);
				KLPAR_CHKTYPE(pChanged2, BOOL_T, L"chg");
				if(pChanged1->GetValue()!=pChanged2->GetValue() || pChanged1->GetValue()!=false)
                {
                    KLSTD_TRACE4(
                            1,
                            L"Attribute '%ls' for section '%ls'-'%ls'-'%ls' differs\n",
                            L"chg",
                            products1[i].c_str(),
                            versions1[j].c_str(),
                            sections1[k].c_str());
                    return false;
                };
            };
		}
	}
	KLSTD_TRACE2(
		1,
		L"...Comparing stores \"%ls\" and \"%ls\" OK\n",
		wstrSS1.c_str(),
		wstrSS2.c_str());

	return true;
}

static void CreateAndInitSS(const std::wstring& wstrLocation, SettingsStorage** ppSS)
{
	KLSTD_TRACE1(3, L"Creating storage '%ls'...\n", wstrLocation.c_str());
	CAutoPtr<SettingsStorage> pSS;
	KLPRSS_CreateSettingsStorageDirect(       wstrLocation,
										KLSTD::CF_OPEN_ALWAYS,
										KLSTD::AF_READ|KLSTD::AF_WRITE,
										&pSS);
	KLPRSS_MAYEXIST(pSS->CreateSection(KLPRSS_PRODUCT_CORE, L"", L""));
	KLPRSS_MAYEXIST(pSS->CreateSection(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L""));
	KLPRSS_MAYEXIST(pSS->CreateSection(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, KLCS_MODULENAME));
	KLSTD_TRACE1(3, L"...OK creating storage '%ls'\n", wstrLocation.c_str());
	pSS.CopyTo(ppSS);
};

static void AddValue(
						SettingsStorage* pSS,
						const wchar_t* szwName,
						const wchar_t* szwValue)
{
	KLSTD_ASSERT_THROW(pSS && szwName && szwValue);
	CAutoPtr<Params> pData;
	param_entry_t pars[]=
	{
		param_entry_t(szwName, szwValue)
	};
	CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pData);
	pSS->Add(
				KLPRSS_PRODUCT_CORE,
				KLPRSS_VERSION_INDEPENDENT,
				KLCS_MODULENAME,
				pData);
};

static std::wstring GetValue(
						SettingsStorage* pSS,
						const wchar_t* szwName)
{
	KLSTD_ASSERT_THROW(pSS && szwName);
	CAutoPtr<Params> pData;
	pSS->Read(
				KLPRSS_PRODUCT_CORE,
				KLPRSS_VERSION_INDEPENDENT,
				KLCS_MODULENAME,
				&pData);
	return GetStringValue(pData, szwName);
};

KLCSC_DECL void KLPRSS_ResolveNames(KLPAR::Params* pInData, KLPAR::Params** ppOutData);

void TestNames(const std::wstring& wstrData)
{
        KLSTD_TRACE1(1, L"TestNames... %ls'\n", wstrData.c_str());
	
    CNameConvertor conv;
    CAutoPtr<Params> pDataTmp, pData;
    conv.convert(wstrData, &pDataTmp);
    KLPRSS_ResolveNames(pDataTmp, &pData);
    ParamsNames vecNames;
    pData->GetNames(vecNames);
    for(ParamsNames::iterator it=vecNames.begin(); it != vecNames.end(); ++it)
        KLSTD_TRACE2(1, L"TestNames: %ls='%ls'\n", it->c_str(), GetStringValue(pData, *it).c_str());
};


CAutoPtr<Params> MakeValue(const std::wstring& wstrValue, bool bMandatory)
{    
    CAutoPtr<Params>        pResult;   
    {
        CAutoPtr<StringValue>   pValue;
        KLPAR::CreateValue(wstrValue.c_str(), &pValue);
        param_entry_t  par[]=
        {
            param_entry_t(c_szwSPS_Value, pValue),
            param_entry_t(c_szwSPS_Mandatory, (bool)bMandatory)
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pResult);
    };
    return pResult;
};

CAutoPtr<Params> MakeValue(KLPAR::Value* pValue, bool bMandatory)
{    
    CAutoPtr<Params>        pResult;   
    {
        param_entry_t  par[]=
        {
            param_entry_t(c_szwSPS_Value, pValue),
            param_entry_t(c_szwSPS_Mandatory, (bool)bMandatory)
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pResult);
    };
    return pResult;
};


CAutoPtr<Params> MakeValue(CAutoPtr<Params>& pX, bool bMandatory)
{    
    CAutoPtr<Params>        pResult;   
    {
        CAutoPtr<ParamsValue>   pValue;
        KLPAR::CreateValue(pX, &pValue);
        param_entry_t  par[]=
        {
            param_entry_t(c_szwSPS_Value, pValue),
            param_entry_t(c_szwSPS_Mandatory, (bool)bMandatory)
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pResult);
    };
    return pResult;
};

bool CheckExistence(
                        KLPAR::Params*      pRoot,
                        const wchar_t**     path,
                        const std::wstring& wstrName)
{
    bool bResult=false;
    KLERR_TRY
        CAutoPtr<Value> pTmp;
        KLPAR_GetValue(pRoot, path, wstrName, &pTmp);
        bResult=true;
    KLERR_CATCH(p)
        if(p->GetId() != KLPAR::NOT_EXIST)
            KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult;
};

#define DECLARE_PATH0(_name)    const wchar_t* _name[]={NULL};
#define DECLARE_PATH1(_name, x1)    const wchar_t* _name[]={x1, NULL};
#define DECLARE_PATH2(_name, x1, x2)    const wchar_t* _name[]={x1, x2, NULL};
#define DECLARE_PATH3(_name, x1, x2, x3)    const wchar_t* _name[]={x1, x2, x3, NULL};
#define DECLARE_PATH4(_name, x1, x2, x3, x4)    const wchar_t* _name[]={x1, x2, x3, x4, NULL};

bool CheckInexistence(
                        KLPAR::Params*      pRoot,
                        const wchar_t**     path,
                        const std::wstring& wstrName)
{
    return !CheckExistence(pRoot, path, wstrName);
};

void TestPolicy()
{
    {
        CAutoPtr<Params> pPolicy, pMandatory, pDefault;
        {
            CAutoPtr<ArrayValue> pPolicyMandArray, pPolicyDefArray;
            CAutoPtr<ValuesFactory> pFactory;
            KLPAR_CreateValuesFactory(&pFactory);
            pFactory->CreateArrayValue(&pPolicyMandArray);
            pFactory->CreateArrayValue(&pPolicyDefArray);
            const int c_nValues=50;
            pPolicyMandArray->SetSize(c_nValues);
            pPolicyDefArray->SetSize(c_nValues);
            int i;
            for(i=0; i < c_nValues; ++i)
            {
                if(rand() & 1)
                {
                    CAutoPtr<ParamsValue> pVal;
                    CreateValue(MakeValue(L"Test", rand() & 1), &pVal);
                    pPolicyMandArray->SetAt(i, pVal);
                    pVal=NULL;
                    CreateValue(MakeValue(L"Test", rand() & 1), &pVal);
                    pPolicyDefArray->SetAt(i, pVal);
                }
                else
                {
                    CAutoPtr<Params> pEmptyParams;
                    if(rand() & 1)
                        KLPAR_CreateParams(&pEmptyParams);
                    else
                    {
                        CAutoPtr<Params> pTmpVal=MakeValue(L"!!!", false);
                        param_entry_t px[]=
                        {
                            param_entry_t(L"x", pTmpVal)
                        };
                        KLPAR::CreateParamsBody(px, KLSTD_COUNTOF(px), &pEmptyParams);
                    }
                    CAutoPtr<ParamsValue> pVal;
                    CreateValue(MakeValue(pEmptyParams, rand() & 1), &pVal);
                    pPolicyMandArray->SetAt(i, pVal);
                    pVal=NULL;
                    CreateValue(MakeValue(pEmptyParams, rand() & 1), &pVal);
                    pPolicyDefArray->SetAt(i, pVal);
                };
            };
            CAutoPtr<Params> parMandArray=MakeValue(pPolicyMandArray, true);
            CAutoPtr<Params> parDefArray=MakeValue(pPolicyDefArray, false);

            param_entry_t pars[]=
            {
                param_entry_t(L"mandarray",parMandArray),
                param_entry_t(L"defarray", parDefArray)
            };
            CAutoPtr<Params> pX;
            KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pX);
            pPolicy=MakeValue(pX, false);
        };
        //KLPAR_SerializeToFileName(L"###Test.xml", pPolicy);
        KLPRSS_GetMandatoryAndDefault(pPolicy, &pMandatory, &pDefault);
        KLSTD_ASSERT_THROW(pMandatory!=NULL);
        KLSTD_ASSERT_THROW(pDefault!=NULL);
        CAutoPtr<ArrayValue> pMandArray=GetArrayValue(pMandatory, L"mandarray");
        CAutoPtr<ArrayValue> pDefArray=GetArrayValue(pDefault, L"defarray");
        KLSTD_ASSERT_THROW(pMandArray!=NULL);
        KLSTD_ASSERT_THROW(pDefArray!=NULL);
        KLSTD_ASSERT_THROW(pDefArray->Compare(pMandArray) == 0);        
    }
    ;
    {
        CAutoPtr<Params> pHighest, pHighestEntry, pRoot, pRootEntry;
        {//pHighest
            CAutoPtr<Params> pVal1=MakeValue(L"Hello!", false);
            CAutoPtr<Params> pVal2=MakeValue(L"Test!", true);
            param_entry_t  par[]=
            {
                param_entry_t(L"wstrX1", pVal1),
                param_entry_t(L"wstrX2", pVal2)
            };
            KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pHighest);
        };
        pHighestEntry=MakeValue(pHighest, true);
        {//pRoot
            CAutoPtr<Params> pVal1=MakeValue(L"Hello!", false);
            CAutoPtr<Params> pVal2=MakeValue(L"Test!", true);
            param_entry_t  par[]=
            {
                param_entry_t(L"Highest", pHighestEntry),
                param_entry_t(L"wstrX1", pVal1),
                param_entry_t(L"wstrX2", pVal2)
            };
            KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pRoot);
        };
        pRootEntry=MakeValue(pRoot, true);
        CAutoPtr<Params> pMandatory, pDefault;
        KLPRSS_GetMandatoryAndDefault(pRootEntry, &pMandatory, &pDefault);

        //KLPAR_SerializeToFileName(L"$Mandatory.xml", pMandatory);
        //KLPAR_SerializeToFileName(L"$Default.xml", pDefault);
    
        DECLARE_PATH0(pathRoot);
        DECLARE_PATH1(pathHighest, L"Highest");

        KLSTD_ASSERT_THROW(CheckExistence(pDefault, pathRoot, L"wstrX1"));
        //KLSTD_ASSERT_THROW(CheckExistence(pDefault, pathRoot, L"wstrX2"));
        KLSTD_ASSERT_THROW(CheckExistence(pDefault, pathRoot, L"Highest"));
        KLSTD_ASSERT_THROW(CheckExistence(pDefault, pathHighest, L"wstrX1"));
        //KLSTD_ASSERT_THROW(CheckExistence(pDefault, pathHighest, L"wstrX2"));

        //KLSTD_ASSERT_THROW(CheckInexistence(pMandatory, pathRoot, L"wstrX1"));
        KLSTD_ASSERT_THROW(CheckExistence(pMandatory, pathRoot, L"wstrX2"));
        KLSTD_ASSERT_THROW(CheckExistence(pMandatory, pathRoot, L"Highest"));
        //KLSTD_ASSERT_THROW(CheckInexistence(pMandatory, pathHighest, L"wstrX1"));
        KLSTD_ASSERT_THROW(CheckExistence(pMandatory, pathHighest, L"wstrX2"));
    };
};

static void Convert(const wchar_t* szwPath, std::vector<const wchar_t*>& vecPath, std::vector<std::wstring>& vecValues)
{
    vecPath.resize(0);
    if(!szwPath)
    {
        vecPath.resize(1);
        vecPath[0]=NULL;
    }
    else
    {
        vecValues.resize(0);
        KLSTD_SplitString(szwPath, L"/", vecValues);
        vecPath.reserve(vecValues.size() +1);
        for(std::vector<std::wstring>::iterator it=vecValues.begin(); it != vecValues.end(); ++it)
            vecPath.push_back((*it).c_str());
        vecPath.push_back(NULL);
    };
};

void AddByPath(Params* pRoot, const wchar_t* szwPath, const std::wstring& wstrName, Value* pValue)
{
    std::vector<std::wstring> vecValues;
    std::vector<const wchar_t*> vecPath;
    Convert(szwPath, vecPath, vecValues);
    KLPAR_ReplaceValue(pRoot, &(*vecPath.begin()), wstrName, pValue);
}

void GetByPath(Params* pRoot, const wchar_t* szwPath, const std::wstring& wstrName, Value** ppValue)
{
    std::vector<std::wstring> vecValues;
    std::vector<const wchar_t*> vecPath;
    Convert(szwPath, vecPath, vecValues);
    KLPAR_GetValue(pRoot, &(*vecPath.begin()), wstrName, ppValue);
};

void TestPolicy2()
{
    std::wstring wstrGuid=KLSTD_CreateGUIDString();
    std::wstring wstrSsSrc=L"$prss-src-" + wstrGuid + L".xml";
    std::wstring wstrSsDst1=L"$prss-dst-" + wstrGuid + L".xml";
#ifdef N_PLAT_NLM
    std::wstring nw_path=L"sys:/klab/adminkit/test/";
    std::wstring wstrSsSrcNW = nw_path+wstrSsSrc;
    std::wstring wstrSsDst1NW = nw_path+wstrSsDst1;
#endif    

    const wchar_t* section1[]={KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1"};
    const wchar_t** sections[]=
    {
        section1,
        NULL
    };
    
    
    const wchar_t* paths_dst[]=
    {
#ifdef N_PLAT_NLM
        wstrSsDst1NW.c_str(),
#else
        wstrSsDst1.c_str(),
#endif
        NULL
    };

    CAutoPtr<IntValue> p_nUnlocked, p_nLocked;
    CAutoPtr<BoolValue> p_bBoolTrue, p_bBoolFalse;
    CAutoPtr<StringValue> p_wstrPolicyName, p_wstrGroupName;
    CreateValue(long(10), &p_nUnlocked);
    CreateValue(long(20), &p_nLocked);    
    CreateValue((bool)true, &p_bBoolTrue);
    CreateValue((bool)false, &p_bBoolFalse);    
    CreateValue(L"policy name", &p_wstrPolicyName);
    CreateValue(L"group name", &p_wstrGroupName);

    CAutoPtr<SettingsStorage> pSsSrc, pSsDst1;
    {
        CAutoPtr<Params> pSection1;
        KLPAR_CreateParams(&pSection1);
        ;    
        ;
        /*
            KLPRSS_Mnd=false
           \KLPRSS_Val=PARAMS_T
               \unlocked
                    KLPRSS_Mnd=false
                   \KLPRSS_Val=PARAMS_T
                       \x=INT_T
                            KLPRSS_Mnd=false
                            KLPRSS_Val=10
               \locked
                    KLPRSS_ValLckPolicy=L"policy name"
                    KLPRSS_ValLckGroup=L"group name"
                    KLPRSS_Mnd=true
                   \KLPRSS_Val=PARAMS_T
                       \x=INT_T
                            KLPRSS_ValLckPolicy=L"policy name"
                            KLPRSS_ValLckGroup=L"group name"
                            KLPRSS_Mnd=true
                            KLPRSS_Val=10

        */
        AddByPath(pSection1, NULL, L"KLPRSS_Mnd",   p_bBoolFalse);

        AddByPath(pSection1, L"KLPRSS_Val/unlocked", L"KLPRSS_Mnd",     p_bBoolFalse);
        AddByPath(pSection1, L"KLPRSS_Val/unlocked/KLPRSS_Val/x", L"KLPRSS_Val", p_nUnlocked);
        AddByPath(pSection1, L"KLPRSS_Val/unlocked/KLPRSS_Val/x", L"KLPRSS_Mnd", p_bBoolFalse);    

        AddByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckPolicy", p_wstrPolicyName);
        AddByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckGroup", p_wstrGroupName);
        AddByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_Mnd",       p_bBoolTrue);
        AddByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_Val", p_nLocked);
        AddByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_Mnd", p_bBoolFalse);

        
#ifdef N_PLAT_NLM
        KLPRSS_CreateSettingsStorageDirect(wstrSsSrcNW, CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pSsSrc);
        KLPRSS_CreateSettingsStorageDirect(wstrSsDst1NW, CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pSsDst1);
#else
        KLPRSS_CreateSettingsStorageDirect(wstrSsSrc, CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pSsSrc);
        KLPRSS_CreateSettingsStorageDirect(wstrSsDst1, CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pSsDst1);
#endif
        KLPRSS_MAYEXIST(pSsSrc->CreateSection(KLPRSS_PRODUCT_CORE, L"", L""));
        KLPRSS_MAYEXIST(pSsSrc->CreateSection(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L""));
        KLPRSS_MAYEXIST(pSsSrc->CreateSection(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1"));
        pSsSrc->Add(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1", pSection1);
    };

    for(int i_test1=0; i_test1 < 2; ++i_test1)
    {
        KLPRSS_DistributePolicy(
#ifdef N_PLAT_NLM
                    wstrSsSrcNW.c_str(),
#else
                    wstrSsSrc.c_str(),
#endif
                    KLSTD_INFINITE,
                    sections,
                    paths_dst,
                    KLSTD_INFINITE,
                    NULL,
                    NULL,
                    true);
        
        {
            CAutoPtr<Params> pSection1;
            pSsSrc->Read(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1", &pSection1);
    
            {
                CAutoPtr<Params> pMandatory, pDefault;
                KLPRSS_GetMandatoryAndDefault(pSection1, &pMandatory, &pDefault);
                KLSTD_ASSERT_THROW(pMandatory!=NULL && pDefault != NULL);
#ifdef N_PLAT_NLM
                KLPAR_SerializeToFileName(nw_path+L"$mnd-" + wstrSsSrc, pMandatory);
                KLPAR_SerializeToFileName(nw_path+L"$def-" + wstrSsSrc, pDefault);
#else
                KLPAR_SerializeToFileName(L"$mnd-" + wstrSsSrc, pMandatory);
                KLPAR_SerializeToFileName(L"$def-" + wstrSsSrc, pDefault);
#endif
            };

            CAutoPtr<BoolValue> p_bNewMandatory1, p_bNewMandatory2, p_bLocked1, p_bLocked2;
            CAutoPtr<StringValue> p_wstrNewPolicy1, p_wstrNewPolicy2, p_wstrNewGroup1, p_wstrNewGroup2;
    
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_Mnd", (Value**)&p_bNewMandatory1);
            //GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLck", (Value**)&p_bLocked1);
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckPolicy", (Value**)&p_wstrNewPolicy1);
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckGroup", (Value**)&p_wstrNewGroup1);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_Mnd", (Value**)&p_bNewMandatory2);
            //GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLck", (Value**)&p_bLocked2);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLckPolicy", (Value**)&p_wstrNewPolicy2);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLckGroup", (Value**)&p_wstrNewGroup2);

            KLSTD_ASSERT_THROW(p_bNewMandatory1->Compare(p_bBoolTrue) == 0);
            KLSTD_ASSERT_THROW(p_bNewMandatory2->Compare(p_bBoolTrue) == 0);
            //KLSTD_ASSERT_THROW(p_bLocked1->Compare(p_bBoolTrue) == 0);
            //KLSTD_ASSERT_THROW(p_bLocked2->Compare(p_bBoolTrue) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewPolicy1->Compare(p_wstrPolicyName) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewPolicy2->Compare(p_wstrPolicyName) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewGroup1->Compare(p_wstrGroupName) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewGroup2->Compare(p_wstrGroupName) == 0);
        };
        {
            CAutoPtr<Params> pSection1;
            
            pSsDst1->Read(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1", &pSection1);
    
            CAutoPtr<BoolValue> p_bNewMandatory1, p_bNewMandatory2, p_bLocked1, p_bLocked2;
            CAutoPtr<StringValue> p_wstrNewPolicy1, p_wstrNewPolicy2, p_wstrNewGroup1, p_wstrNewGroup2;
    
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_Mnd", (Value**)&p_bNewMandatory1);
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLck", (Value**)&p_bLocked1);
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckPolicy", (Value**)&p_wstrNewPolicy1);
            GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckGroup", (Value**)&p_wstrNewGroup1);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_Mnd", (Value**)&p_bNewMandatory2);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLck", (Value**)&p_bLocked2);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLckPolicy", (Value**)&p_wstrNewPolicy2);
            GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLckGroup", (Value**)&p_wstrNewGroup2);

            KLSTD_ASSERT_THROW(p_bNewMandatory1->Compare(p_bBoolTrue) == 0);
            KLSTD_ASSERT_THROW(p_bNewMandatory2->Compare(p_bBoolTrue) == 0);
            KLSTD_ASSERT_THROW(p_bLocked1->Compare(p_bBoolTrue) == 0);
            KLSTD_ASSERT_THROW(p_bLocked2->Compare(p_bBoolTrue) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewPolicy1->Compare(p_wstrPolicyName) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewPolicy2->Compare(p_wstrPolicyName) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewGroup1->Compare(p_wstrGroupName) == 0);
            KLSTD_ASSERT_THROW(p_wstrNewGroup2->Compare(p_wstrGroupName) == 0);
        };
    };
    {
        std::wstring wstrCopyPath;
        {
            std::wstring wstrDir, wstrName, wstrExt;
#ifdef N_PLAT_NLM
            KLSTD_SplitPath(wstrSsDst1NW, wstrDir, wstrName, wstrExt);
#else
            KLSTD_SplitPath(wstrSsDst1, wstrDir, wstrName, wstrExt);
#endif            
            KLSTD_MakePath(wstrDir, L"$Copy of " + wstrName, wstrExt, wstrCopyPath);
        };

#ifdef N_PLAT_NLM
            KLPRSS_CopySettingsStorage(wstrSsDst1NW, wstrCopyPath, false, KLSTD_INFINITE);
            KLSTD_ASSERT_THROW(CompareSyncSSs(wstrSsDst1NW, wstrCopyPath) == 0);
#else
            KLPRSS_CopySettingsStorage(wstrSsDst1, wstrCopyPath, false, KLSTD_INFINITE);
            KLSTD_ASSERT_THROW(CompareSyncSSs(wstrSsDst1, wstrCopyPath) == 0);
#endif
            KLPRSS_ResetParentalLocks(wstrCopyPath.c_str(), KLSTD_INFINITE);
    };
    ;
    {
        CAutoPtr<Params> pSection1;
        KLPAR_CreateParams(&pSection1);
        AddByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_Mnd",       p_bBoolFalse);
        pSsSrc->Replace(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1", pSection1);    
    };

    for(int i_test2=0; i_test2 < 2; ++i_test2)
    {
        KLPRSS_DistributePolicy(
#ifdef N_PLAT_NLM
                    wstrSsSrcNW.c_str(),
#else
                    wstrSsSrc.c_str(),
#endif
                    KLSTD_INFINITE,
                    sections,
                    paths_dst,
                    KLSTD_INFINITE,
                    NULL,
                    NULL,
                    true);

        bool bNotExist=false;
        CAutoPtr<BoolValue> p_bNewMandatory1, p_bNewMandatory2;

    #define CHKNOTEXIST(_sec, _path, _name) \
        {   \
            CAutoPtr<Value> pTempVal;   \
            KLERR_IGNORE(GetByPath(_sec, _path, _name, &pTempVal)); \
            KLSTD_ASSERT_THROW(pTempVal == NULL);   \
        }

        CAutoPtr<Params> pSection1;
        pSsSrc->Read(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, L"Section1", &pSection1);
    
        GetByPath(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_Mnd", (Value**)&p_bNewMandatory1);
        CHKNOTEXIST(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLck");
        CHKNOTEXIST(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckPolicy");
        CHKNOTEXIST(pSection1, L"KLPRSS_Val/locked", L"KLPRSS_ValLckGroup");
        GetByPath(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_Mnd", (Value**)&p_bNewMandatory2);
        CHKNOTEXIST(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLck");
        CHKNOTEXIST(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLckPolicy");
        CHKNOTEXIST(pSection1, L"KLPRSS_Val/locked/KLPRSS_Val/x", L"KLPRSS_ValLckGroup");

        KLSTD_ASSERT_THROW(p_bNewMandatory1->Compare(p_bBoolFalse) == 0);
        KLSTD_ASSERT_THROW(p_bNewMandatory2->Compare(p_bBoolFalse) == 0);
    #undef CHKNOTEXIST
    };
};

void adjustTasksParams(
                    const std::wstring&     wstrName,
                    KLPAR::Params*          pData,
                    KLPAR::Params**         ppResutl)
{
    CAutoPtr<Params> pResult;
    bool bWasHandled=false;    
    {
        KLERR_BEGIN
            CAutoPtr<Params> pRoot;
            param_entry_t pars[]=
            {
                param_entry_t(wstrName.c_str(), pData)
            };
            KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pRoot);
            CAutoPtr<SettingsStorage> pSS;
            KLPRSS_CreateSettingsStorage(
                            KLPRSS_GetSettingsStorageLocation(),
                            CF_OPEN_EXISTING,
                            AF_READ,
                            &pSS);
            pSS->SetTimeout(KLSTD_INFINITE);
            pSS->Read2(
                L"Workstation",
                L"5.0.0.0",
                KLPRSS_TASKS_POLICY,
                pRoot,
                &pResult);
            pResult=KLPAR::GetParamsValue(pResult, wstrName);
            bWasHandled=true;
        KLERR_END
    };
    if(!bWasHandled)
        pResult=pData;
    pResult.CopyTo(ppResutl);
};


// High Bits<  PMU  >Low Bits
void TestStoragePolicy(
					const std::wstring& wstrUser,
					const std::wstring& wstrMachine,
					const std::wstring& wstrPolicy)
{
    /*
    {
        CAutoPtr<Params> pInitial, pResult;
        KLPAR_DeserializeFromFileName(L"#KLOAS_TA_MONITOR_TASK_ini.xml", &pInitial);
        adjustTasksParams(L"KLOAS_TA_MONITOR_TASK", pInitial, &pResult);
        KLPAR_SerializeToFileName(L"$xxx.xml", pResult);
    };
    */
    KLSTD_TRACE0(1,L"TestStoragePolicy...\n");    
    TestPolicy2();
    KLSTD_TRACE0(1,L"...TestStoragePolicy\n");
    std::basic_ostringstream<wchar_t> os;    
    const wchar_t c_wchQuote[]=L"\"";
    os  <<  c_szwSSP_User << L"=" << c_wchQuote << c_szwSSP_Marker << c_szwSST_CUserSS << c_wchQuote  << L"; "
        <<  L"u= " << c_wchQuote << c_szwSSP_Marker << c_szwSST_CUserSS << c_wchQuote  << L"; "
        <<  c_szwSSP_Host << L"=" << c_wchQuote << c_szwSSP_Marker << c_szwSST_HostSS<< c_wchQuote  << L"; ";
    TestNames(os.str());
    TestNames(L"u=\"c:\\user.cfg\"; m = \"c:\\machine.cfg\";p=\"c:\\policy.cfg\"");
    TestNames(L"test_value.org=\"c:\\user.cfg\"");
    TestPolicy();
    /*
	CAutoPtr<SettingsStorage> pUser, pMachine, pPolicy, pMain;
	const wchar_t c_szwUser[]=L"user";
	const wchar_t c_szwMachine[]=L"machine";
	const wchar_t c_szwPolicy[]=L"policy";
	KLSTD_TRACE0(3, L"Creating storages...\n");
	CreateAndInitSS(wstrUser, &pUser);
	CreateAndInitSS(wstrMachine, &pMachine);
	CreateAndInitSS(wstrPolicy, &pPolicy);
	KLSTD_TRACE0(3, L"...OK creating storages\n");
	int i;
	for(i=1; i < 8; ++i)
	{
		wchar_t szwName[32];
		KLSTD_SWPRINTF(szwName, KLSTD_COUNTOF(szwName), L"val-%d", i);
		if(i & 0x1)
			AddValue(pUser, szwName, c_szwUser);
		if(i & 0x2)
			AddValue(pMachine, szwName, c_szwMachine);
		if(i & 0x4)
			AddValue(pPolicy, szwName, c_szwPolicy);
	};
	wchar_t szMainLoc[1024];
	KLSTD_SWPRINTF(
				szMainLoc,
				KLSTD_COUNTOF(szMainLoc),
				L"%ls|%ls|%ls|<1>",
				wstrMachine.c_str(),
				wstrUser.c_str(),
				wstrPolicy.c_str());
	KLPRSS_OpenSettingsStorageRW(szMainLoc, &pMain);
	const wchar_t* c_szwCorrectValues[]=
	{
		NULL,			//0
		c_szwUser,		//1
		c_szwMachine,	//2
		c_szwUser,		//3
		c_szwPolicy,	//4
		c_szwPolicy,	//5
		c_szwPolicy,	//6
		c_szwPolicy		//7
	};
	for(i=1; i < 8; ++i)
	{
		wchar_t szwName[32];
		KLSTD_SWPRINTF(szwName, KLSTD_COUNTOF(szwName), L"val-%d", i);
		std::wstring wstrValue=GetValue(pMain, szwName);
		KLSTD_TRACE2(3, L"\t'%ls'='%ls'\n", szwName, wstrValue.c_str());
		KLSTD_ASSERT_THROW(wstrValue == c_szwCorrectValues[i]);
	};
	pPolicy->DeleteSection(
				KLPRSS_PRODUCT_CORE,
				KLPRSS_VERSION_INDEPENDENT,
				KLCS_MODULENAME);

	const wchar_t* c_szwCorrectValues2[]=
	{
		NULL,			//0
		c_szwUser,		//1
		c_szwMachine,	//2
		c_szwUser,		//3
		NULL,			//4
		c_szwUser,		//5
		c_szwMachine,	//6
		c_szwUser		//7
	};

	for(i=1; i < 8; ++i)
	{
		if(i == 4)
			continue;
		wchar_t szwName[32];
		KLSTD_SWPRINTF(szwName, KLSTD_COUNTOF(szwName), L"val-%d", i);
		std::wstring wstrValue=GetValue(pMain, szwName);
		KLSTD_TRACE2(3, L"\t'%ls'='%ls'\n", szwName, wstrValue.c_str());
		KLSTD_ASSERT_THROW(wstrValue == c_szwCorrectValues2[i]);
	};
    */
};

/*
С = {p1=1, p2=2, p3=3, p4=4}    src
П = {p1=7, p4=0, p6=6}          mnd
Ц = {p1=8, p2=5, p9=9}          dst
*/

static void AddParams(
                      KLPAR::Params*        pRoot,
                      const std::wstring&   wstrName,
                      KLPAR::Params*        pVar)
{
    KLSTD_ASSERT_THROW(pVar != NULL);
    CAutoPtr<ParamsValue> pVal;
    CAutoPtr<Params> pVarCopy;
    pVar->Clone(&pVarCopy);
    KLPAR::CreateValue(pVarCopy, &pVal);
    pVar->AddValue(wstrName, pVal);
}

KLCSKCA_DECL bool KLPRSS_IfMustModifySettings(KLPAR::Params* pOldPolSection, KLPAR::Params* pNewPolSection);

namespace
{
    KLPAR::ParamsPtr CreatePolicySection(bool bMnd, bool bYVal)
    {
        KLPAR::ParamsPtr pResult;
    KL_TMEASURE_BEGIN(L"CreatePolicySection", 4)
        KLPAR_CreateParams(&pResult);
        ;
        const wchar_t* path1[] = 
        {
            c_szwSPS_Value,
            L"X",
            NULL
        };

        const wchar_t* path2[] = 
        {
            c_szwSPS_Value,
            L"Y",
            NULL
        };
        
        KLPAR::IntValuePtr pX, pY;
        KLPAR::BoolValuePtr p_boolTrue, p_boolFalse;
        KLPAR::CreateValue(1L,      &pX);
        KLPAR::CreateValue(2L,      &pY);        
        KLPAR::CreateValue(true,    &p_boolTrue);
        KLPAR::CreateValue(false,   &p_boolFalse);
        
        pResult->AddValue(c_szwSPS_Mandatory,               p_boolFalse);
        ;
        KLPAR_AddValue(pResult, path1, c_szwSPS_Value,      pX);
        KLPAR_AddValue(pResult, path1, c_szwSPS_Mandatory,  bMnd?p_boolTrue:p_boolFalse);

        if(bYVal)
        {
            KLPAR_AddValue(pResult, path2, c_szwSPS_Value,      pY);
            KLPAR_AddValue(pResult, path2, c_szwSPS_Mandatory,  p_boolTrue);
        };
        ;
        KLPARLOG_LogParams(4, pResult);
    KL_TMEASURE_END()
        return pResult;
    };

    typedef bool (*fill_par_func_t)(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew);

    bool fill_par_func_1(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        //NULL, NULL
        return false;
    };

    bool fill_par_func_2(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        pOld = CreatePolicySection(false, false);
        //non-NULL, NULL (deleted) but no locks
        return false;
    };
    
    bool fill_par_func_2_2(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        pOld = CreatePolicySection(true, false);
        //non-NULL, NULL (deleted) and has a lock
        return true;
    };

    bool fill_par_func_3(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        pNew = CreatePolicySection(false, false);
        //NULL, non-NULL (created)
        return false;
    };

    bool fill_par_func_4(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        //mnd=false, mnd=true
        pOld = CreatePolicySection(false, false);
        pNew = CreatePolicySection(true, false);
        return false;
    };

    bool fill_par_func_5(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        //mnd=true, mnd=false
        pOld = CreatePolicySection(true, false);
        pNew = CreatePolicySection(false, false);
        return true;
    };

    bool fill_par_func_6(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        //val exists, val not exists (deleted)
        pOld = CreatePolicySection(true, true);
        pNew = CreatePolicySection(true, false);
        return true;
    };

    bool fill_par_func_7(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        //val not exists, val exists (created)
        pOld = CreatePolicySection(true, false);
        pNew = CreatePolicySection(true, true);
        return false;
    };

    bool fill_par_func_8(KLPAR::ParamsPtr& pOld, KLPAR::ParamsPtr& pNew)
    {
        //equal
        pOld = CreatePolicySection(true, true);
        pNew = CreatePolicySection(true, true);
        return false;
    };

    void OnMyTestCompr()
    {        
        fill_par_func_t p[] =
        {
            fill_par_func_1,
            fill_par_func_2,
            fill_par_func_2_2,
            fill_par_func_3,
            fill_par_func_4,
            fill_par_func_5,
            fill_par_func_6,
            fill_par_func_7,
            fill_par_func_8
        };
        
        for(size_t i = 0; i < KLSTD_COUNTOF(p); ++i)
        {
            KLPAR::ParamsPtr pOld, pNew;
            const bool bStandardResult = (p[i])(pOld, pNew);
            KLSTD_ASSERT_THROW(KLPRSS_IfMustModifySettings(pOld, pNew) == bStandardResult);
        };
    };
    
    void TestSsModify()
    {
        OnMyTestCompr();
    };
};

void TestSmartWrite()
{
    TestSsModify();
    ;
    CAutoPtr<Params> pSrc, pMnd, pDef, pDst, pExpected, pResult;
    {
        KLPAR::param_entry_t parsSrc[]=
        {
            KLPAR::param_entry_t(L"p1", 1L),
            KLPAR::param_entry_t(L"p2", 2L),
            KLPAR::param_entry_t(L"p3", 3L),
            KLPAR::param_entry_t(L"p4", 4L)
        };
        KLPAR::CreateParamsBody(parsSrc, KLSTD_COUNTOF(parsSrc), &pSrc);
        AddParams(pSrc, L"x", pSrc);
    };
    {
        KLPAR::param_entry_t parsMnd[]=
        {
            KLPAR::param_entry_t(L"p1", 7L),
            KLPAR::param_entry_t(L"p4", 0L),
            KLPAR::param_entry_t(L"p6", 6L)
        };
        KLPAR::CreateParamsBody(parsMnd, KLSTD_COUNTOF(parsMnd), &pMnd);
        AddParams(pMnd, L"x", pMnd);
    };
    {
        KLPAR::param_entry_t parsDst[]=
        {
            KLPAR::param_entry_t(L"p1", 8L),
            KLPAR::param_entry_t(L"p2", 5L),
            KLPAR::param_entry_t(L"p9", 9L)
        };
        KLPAR::CreateParamsBody(parsDst, KLSTD_COUNTOF(parsDst), &pDst);
        AddParams(pDst, L"x", pDst);
    };
    {
        KLPAR::param_entry_t parsRes[]=
        {
            KLPAR::param_entry_t(L"p1", 8L),
            KLPAR::param_entry_t(L"p2", 2L),
            KLPAR::param_entry_t(L"p3", 3L),
            KLPAR::param_entry_t(L"p4", 0L),
            KLPAR::param_entry_t(L"p6", 6L),
            KLPAR::param_entry_t(L"p9", 9L)
        };
        KLPAR::CreateParamsBody(parsRes, KLSTD_COUNTOF(parsRes), &pExpected);
        AddParams(pExpected, L"x", pExpected);
    };
    KLPRSS::PrepareForSmartWrite(pDst, NULL, pMnd, pSrc, &pResult);
    ;
    if(!pResult || pResult->Compare(pExpected)!=0)
    {
        std::wstringstream os;
        os  <<  L"TestSmartWrite-"
            <<  std::hex
            <<  KLSTD_GetCurrentThreadId()
            << L"-"
            <<  KLSTD::GetSysTickCount()
            << L".xml";

        KLPAR_SerializeToFileName(os.str(), pResult);
        KLSTD_THROW(STDE_GENERAL);
    };
};

namespace
{
    const wchar_t c_szwProduct[] = L"prod";
    const wchar_t c_szwVersion[] = L"ver";
    const wchar_t c_szwSection[] = L"sec";
};

void TestComplexSsOps(const wchar_t *szFileName)
{
    ParamsPtr pNode1;
    {
        ParamsPtr pNode1_1;
        {
            KLPAR::param_entry_t par[]=
            {
                KLPAR::param_entry_t(L"a", 100L),
                KLPAR::param_entry_t(L"b", 10L),
                KLPAR::param_entry_t(L"c", 2L),
                KLPAR::param_entry_t(L"d", 4L),
                KLPAR::param_entry_t(L"e", 5L)
            };
            KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par),  &pNode1_1);
        };
        ;
        ParamsPtr pNode1_2;
        {
            KLPAR::param_entry_t par[]=
            {
                KLPAR::param_entry_t(L"a", 100L),
                KLPAR::param_entry_t(L"b", 10L),
                KLPAR::param_entry_t(L"c", 2L),
                KLPAR::param_entry_t(L"d", 4L),
                KLPAR::param_entry_t(L"e", 5L)
            };
            KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pNode1_2);
        };
        ;
    
        {
            KLPAR::param_entry_t par[]=
            {
                KLPAR::param_entry_t(L"n1", pNode1_1),
                KLPAR::param_entry_t(L"n2", pNode1_2),
                KLPAR::param_entry_t(L"1", 2L),
                KLPAR::param_entry_t(L"2", 4L),
                KLPAR::param_entry_t(L"3", 5L)
            };
            KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pNode1);
        };
    };
    ;
    KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSs;
    KLPRSS_OpenSettingsStorageRW(szFileName, &pSs);
    KLPRSS_MAYEXIST(pSs->CreateSection(c_szwProduct, L"", L""));
    KLPRSS_MAYEXIST(pSs->CreateSection(c_szwProduct, c_szwVersion, L""));
    KLPRSS_MAYEXIST(pSs->CreateSection(c_szwProduct, c_szwVersion, c_szwSection));
    pSs->Replace(c_szwProduct, c_szwVersion, c_szwSection, pNode1);
    KLPRSS_SsMerge_ReplaceLeavesUpdateNodes(
                szFileName, 
                c_szwProduct, 
                c_szwVersion, 
                c_szwSection,
                pNode1,
                false,
                KLSTD_INFINITE);
    ParamsPtr pNode2;
    pNode1->Clone(&pNode2);
    KLSTD::CAutoPtr<KLPAR::Value> pFoo;
    pNode2->GetValue(L"n2", &pFoo);
    pNode2->DeleteValue(L"n2", true);
    pNode2->AddValue(L"n3", pFoo);
    bool bOK = false;
    KLERR_TRY
        KLPRSS_SsMerge_ReplaceLeavesUpdateNodes(
                    szFileName, 
                    c_szwProduct, 
                    c_szwVersion, 
                    c_szwSection,
                    pNode2,
                    false,
                    KLSTD_INFINITE);
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(1, pError);
        if(pError->GetId() != KLPRSS::NOT_EXIST)
        {
            KLERR_RETHROW();
        };
        bOK = true;
    KLERR_ENDTRY
    if(!bOK)
    {
        KLSTD_THROW(KLSTD::STDE_GENERAL);
    };
    KLPRSS_SsMerge_ReplaceLeavesUpdateNodes(
                szFileName, 
                c_szwProduct, 
                c_szwVersion, 
                c_szwSection,
                pNode1,
                true,
                KLSTD_INFINITE);
    ;
};
