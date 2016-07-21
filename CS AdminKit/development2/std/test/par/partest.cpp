/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file par/test/test.cpp
 * \author
 * \date 2003
 * \brief 
 *
 */

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: test.cpp,v 1.7 2003/04/04 11:55:45 ptr Exp $"
#  else
#ident "@(#)$Id: test.cpp,v 1.7 2003/04/04 11:55:45 ptr Exp $"
#  endif
#endif

#include "std/base/kldefs.h"

#include "partest.h"
#include "../testing/testing.h"
#ifdef _WIN32
#include <tchar.h>
#endif
#include <sstream>
#include <sys/stat.h>

#include <exception>
#include "std/par/paramsi.h"
#include <common/measurer.h>

#include "std/par/parserialize.h"
#include "std/base/klbase.h"
#include "std/io/klio.h"
#include "std/conv/klconv.h"
#include "std/thr/sync.h"
#include "std/thr/thread.h"

#define KLCS_MODULENAME L"PARTST"

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;

#ifdef WIN32
# pragma comment(lib, "Ws2_32.lib")

/*# ifdef _DEBUG
#  pragma comment(lib, "../../BIN/DLLD/klcsstd.lib")
# else
#  pragma comment(lib, "../../BIN/DLL/klcsstd.lib")
# endif*/

#endif // WIN32
#ifdef N_PLAT_NLM
# include <../conv/_conv.h>
//# include <std/conv/wcharcrt.h>
#endif

static const wchar_t szwFileTml1[]=L"$par1-%d.xml";
static const wchar_t szwFileTml2[]=L"$par2-%d.xml";
static const wchar_t szwFileTml3[]=L"$par3-%d.xml";
static const wchar_t szwFileTml4[]=L"$par4-%d.xml";

#include "testmodule.h"
#include "helpers.h"

// using namespace KLTST2;

void testEnv();
void test_ParamsSubtree();
bool Test_WriteRead(const wchar_t* szwFile1, const wchar_t* szwFile2, bool bBinary);
bool Test_Clone(const wchar_t* szwFile1, const wchar_t* szwFile2, bool bBinary);
bool Test_Duplicate(const wchar_t* szwFile1, const wchar_t* szwFile2, bool bBinary);
void testParWriteTime(int &nSumma, const std::wstring& wstrFileName, bool bBinary);
void testParWriteTime(bool bBinary = false);

namespace
{
    typedef void (*fill_par_func_t)(KLPAR::ParamsPtr& pX);

    void fill_par_func_deser(KLPAR::ParamsPtr& pX)
    {
        KLPAR_DeserializeFromFileName(L"#policy_wks.dat", &pX);
    };

    void fill_par_func_NULL(KLPAR::ParamsPtr& pX)
    {
        ;
    };

    void fill_par_func_empty(KLPAR::ParamsPtr& pX)
    {
        KLPAR_CreateParams(&pX);
    };

    void fill_par_func_1(KLPAR::ParamsPtr& pX)
    {
        KLPAR::param_entry_t par[] = 
        {
            KLPAR::param_entry_t(L"a", true)
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pX);
    };

    void fill_par_func_2(KLPAR::ParamsPtr& pX)
    {
        KLPAR_CreateParams(&pX);
        for(size_t i =0; i < 1000; ++i)
        {
            std::wostringstream os;
            for(size_t i =0; i < 100; ++i)
            {
                char ch = 0;
                while(!ch)
                    ch = (char)rand();
                os << (wchar_t)(char)ch; 
            };
            KLSTD::CAutoPtr<KLPAR::BoolValue> pVal;
            KLPAR::CreateValue(true, &pVal);
            pX->ReplaceValue(os.str().c_str(), pVal);
        };
    };


    void OnMyTestCompr()
    {        
        fill_par_func_t p[]=
        {
            fill_par_func_deser,
            fill_par_func_NULL,
            fill_par_func_empty,
            fill_par_func_1,
            fill_par_func_2
        };
        for(size_t i = 0; i < KLSTD_COUNTOF(p); ++i)
        {
            KLPAR::ParamsPtr pData1, pData2;
            KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
            (p[i])(pData1);
            for(size_t k =0; k < 3; ++k)
            {                
                pData2 = NULL;
                pChunk = NULL;
                AVP_dword dwFlags = 0;
                switch(k)
                {
                case 0:
                    dwFlags = KLPAR_SF_BINARY;
                    break;
                case 1:
                    dwFlags = KLPAR_SF_BIN_COMPRESS;
                    break;
                case 2:
                    dwFlags = KLPAR_SF_SOAP;
                    break;
                default:
                    KLSTD_ASSERT(!"Error");
                };                
                KLPAR_SerializeToMemory2(
                        dwFlags, 
                        pData1, 
                        &pChunk);
                KLPAR_DeserializeFromMemory(   pChunk->GetDataPtr(), 
                                                pChunk->GetDataSize(), 
                                                &pData2);
                if(pData1)
                {
                    KLSTD_ASSERT_THROW(pData1->Compare(pData2) == 0);
                }
                else
                {
                    KLSTD_ASSERT_THROW(pData1 == pData2);
                };
                //AVP_dword dwChecksum1 = KLPAR::CalcParamsCheckSum(pData1);
                //AVP_dword dwChecksum2 = KLPAR::CalcParamsCheckSum(pData2);
                //KLSTD_ASSERT_THROW(dwChecksum2 == dwChecksum1);
            };
        };
    };
};



void CSerTest::Initialize(KLTST2::TestData2* pTestData)
{
  ::KLSTD_Initialize();
  ::KLPAR_Initialize();
}

KLSTD_NOTHROW void CSerTest::Deinitialize() throw()
{
  KLERR_BEGIN
    ::KLPAR_Deinitialize();
    ::KLSTD_Deinitialize();
  KLERR_END
}

long CSerTest::Run()
{
  bool bResult=true;
  KLSTD_TRACE0(1, L"\n");
  KLSTD_TRACE0(1, L"Starting test PAR_Ser...\n");
  KLERR_TRY
      for(int i=0; i < 2; ++i)
      {
          const int c_nIterations=10;
          KLSTD::CAutoPtr<KLPAR::Params> pData;
          long lSum=0, lSerSize=0;
          AcquireRandomParams(5, 10, 2, &pData, &lSum);      
          unsigned long delta=0;
          {
              KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
              KLPAR_SerializeToMemory2(i?KLPAR_SF_BINARY:0, pData, &pChunk);
              lSerSize=pChunk->GetDataSize();
              CAutoPtr<Params> results[c_nIterations];
              unsigned long t1=KLSTD::GetSysTickCount();
              for(int i=0; i < c_nIterations; ++i)
              {
                  KLPAR_DeserializeFromMemory(
                                        pChunk->GetDataPtr(),
                                        pChunk->GetDataSize(),
                                        &results[i]);
              };
              delta=KLSTD::GetSysTickCount()-t1;
              for(int j=0; j < c_nIterations; ++j)
              {
                  KLSTD_ASSERT(pData->Compare(results[j]) == 0);
              };
          };
          KLSTD::Trace1(KLCS_MODULENAME,
              L"Deseralizing params %ls memory "
              L"(vars=%u, sersize=%u, servarsize=%lf) "
              L"took %lf ms per iteration\n",
              (i?L"BINARY":L"XML"),
              lSum,
              lSerSize,
              double(lSerSize)/double(lSum),
              double(delta)/double(c_nIterations));
      };
  KLERR_CATCH(pError)
    bResult=false;
    KLERR_SAY_FAILURE(1, pError);			
    KLERR_RETHROW();
  KLERR_ENDTRY

  KLSTD_TRACE1(1, L"...Finishing test PAR_Ser, bResult=%d\n", bResult);
  return bResult?0:(-1);
}
//-----------------------------------------

void CGeneralTest2::Initialize(KLTST2::TestData2* pTestData)
{
  ::KLSTD_Initialize();
  ::KLPAR_Initialize();
}

KLSTD_NOTHROW void CGeneralTest2::Deinitialize() throw()
{
  KLERR_BEGIN
    ::KLPAR_Deinitialize();
    ::KLSTD_Deinitialize();
  KLERR_END
}

long CGeneralTest2::Run()
{    
KL_TMEASURE_BEGIN(L"PAR_General", 1)

	KLERR_TRY    
    
    std::wstring wstrFile1=MakePerThreadFileName(szwFileTml1);
    std::wstring wstrFile2=MakePerThreadFileName(szwFileTml2);
    testEnv();
    test_ParamsSubtree();
    if( Test_WriteRead(wstrFile1.c_str(), wstrFile2.c_str(), false) &&
        Test_WriteRead(wstrFile1.c_str(), wstrFile2.c_str(), true))
    {
      KLSTD_Unlink(wstrFile1.c_str(), false);
      KLSTD_Unlink(wstrFile2.c_str(), false);
    }

    OnMyTestCompr();
	
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(1, pError);			
        KLERR_RETHROW();
    KLERR_ENDTRY
KL_TMEASURE_END()
    return 0;
}


void CCloningTest2::Initialize(KLTST2::TestData2* pTestData)
{
  ::KLSTD_Initialize();
  ::KLPAR_Initialize();
}

KLSTD_NOTHROW void CCloningTest2::Deinitialize() throw()
{
  KLERR_BEGIN
    ::KLPAR_Deinitialize();;
    ::KLSTD_Deinitialize();;
  KLERR_END
}

long CCloningTest2::Run()
{
KL_TMEASURE_BEGIN(L"PAR_Cloning", 1)
    KLERR_TRY
    std::wstring wstrFile1=MakePerThreadFileName(szwFileTml3);
    std::wstring wstrFile2=MakePerThreadFileName(szwFileTml4);
    if( Test_Clone(wstrFile1.c_str(), wstrFile2.c_str(), false) &&
        Test_Clone(wstrFile1.c_str(), wstrFile2.c_str(), true))
    {
      KLSTD_Unlink(wstrFile1.c_str(), false);
      KLSTD_Unlink(wstrFile2.c_str(), false);
    };
    KLERR_CATCH(pError)
    KLERR_SAY_FAILURE(1, pError);			
    KLERR_RETHROW();
    KLERR_ENDTRY
KL_TMEASURE_END()
  return 0;
}

void CDuplicatingTest2::Initialize(KLTST2::TestData2* pTestData)
{
  ::KLSTD_Initialize();
  ::KLPAR_Initialize();
}

KLSTD_NOTHROW void CDuplicatingTest2::Deinitialize() throw()
{
  KLERR_BEGIN
    ::KLPAR_Deinitialize();;
    ::KLSTD_Deinitialize();;
  KLERR_END
}

long CDuplicatingTest2::Run()
{
KL_TMEASURE_BEGIN(L"PAR_Duplicating", 1)
  KLERR_TRY
    std::wstring wstrFile1=MakePerThreadFileName(szwFileTml3);
    std::wstring wstrFile2=MakePerThreadFileName(szwFileTml4);
    if( Test_Duplicate(wstrFile1.c_str(), wstrFile2.c_str(), false) && 
        Test_Duplicate(wstrFile1.c_str(), wstrFile2.c_str(), true))
    {
      KLSTD_Unlink(wstrFile1.c_str(), false);
      KLSTD_Unlink(wstrFile2.c_str(), false);
    }
  KLERR_CATCH(pError)
    KLERR_SAY_FAILURE(1, pError);			
  KLERR_ENDTRY
KL_TMEASURE_END()
  return 0;
}

void CTimingTest2::Initialize(KLTST2::TestData2* pTestData)
{
  ::KLSTD_Initialize();
  ::KLPAR_Initialize();
}

KLSTD_NOTHROW void CTimingTest2::Deinitialize() throw()
{
  KLERR_BEGIN
    ::KLPAR_Deinitialize();;
    ::KLSTD_Deinitialize();;
  KLERR_END
}

long CTimingTest2::Run()
{
KL_TMEASURE_BEGIN(L"PAR_Timing", 1);
    KLERR_TRY
        testParWriteTime(false);
        testParWriteTime(true);
    KLERR_CATCH(pError)        
        KLERR_SAY_FAILURE(1, pError);    
        KLERR_RETHROW();
    KLERR_ENDTRY
KL_TMEASURE_END();
  return 0;
}

//functions

static void WriteToFile(const wchar_t* szwFileName, Params* pParams, bool bBinary)
{
	CAutoPtr<File> pFile;
	KLSTD_CreateFile(szwFileName, 0, CF_CREATE_ALWAYS, AF_WRITE, EF_SEQUENTIAL_SCAN, &pFile, 0);
    ::KLPAR_SerializeToFileID2(pFile->GetFileID(), bBinary? KLPAR_SF_BINARY: 0, pParams);
};

static void ReadFromFile(const wchar_t* szwFileName, Params** ppParams)
{
	CAutoPtr<File> pFile;
	KLSTD_CreateFile(szwFileName, 0, CF_OPEN_EXISTING, AF_READ, EF_SEQUENTIAL_SCAN, &pFile, 0);
	::KLPAR_DeserializeFromFileID(pFile->GetFileID(), ppParams);
};

///////////////////////////////////////////////////////
//
//		Проверка эквивалентности записии чтения
//
///////////////////////////////////////////////////////


bool Test_WriteRead(const wchar_t* szwFile1, const wchar_t* szwFile2, bool bBinary)
{	
    {
        std::vector<const wchar_t*> vecStrings1;
        vecStrings1.push_back( L"1" );
        vecStrings1.push_back( L"2" );
        vecStrings1.push_back( L"3" );
        vecStrings1.push_back( L"4" );

        std::vector<const wchar_t*> vecStrings2;
        vecStrings2.push_back( L"4" );
        vecStrings2.push_back( L"3" );
        vecStrings2.push_back( L"2" );
        vecStrings2.push_back( L"1" );
        vecStrings2.push_back( L"0" );

        KLSTD::CAutoPtr<KLPAR::ArrayValue> pArray1, pArray2;
        KLPAR::CreateStringValueArray(vecStrings1, &pArray1);
        KLPAR::CreateStringValueArray(vecStrings2, &pArray2);
        KLSTD_ASSERT( pArray1->Compare (pArray2) != 0 );
    };
	{
		Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"\n", szwFile1);
		CAutoPtr<Params> pParams1, pParams2;	
		long lSum=0;
		AcquireRandomParams(3, 3, 3, &pParams1, &lSum);
        {
            std::vector<const wchar_t*> vecStrings;
            vecStrings.push_back( L"xawsassa" );
            KLSTD::CAutoPtr<KLPAR::ArrayValue> pArray;
            KLPAR::CreateStringValueArray(vecStrings, &pArray);
            pArray->SetSize(pArray->GetSize() + 2);
            pParams1->AddValue(L"TheArray", pArray);
        };
        {
            CAutoPtr<ParamsValue> pValue;
            CAutoPtr<ValuesFactory> pFactory;
            KLPAR_CreateValuesFactory(&pFactory);
            pFactory->CreateParamsValue(&pValue);
            pValue->SetValue(NULL);
            pParams1->AddValue(L"NULL_PARAMS", pValue);
        };    
		Trace1(KLCS_MODULENAME, L"Values generated: %d\n", lSum);
		//AcquireRandomParams(3, 1, 2, &pParams1, NULL);
        AVP_dword dwCheckSum1=KLPAR::CalcParamsCheckSum(pParams1);
		WriteToFile(szwFile1, pParams1, bBinary);
		ReadFromFile(szwFile1, &pParams2);
        AVP_dword dwCheckSum2=KLPAR::CalcParamsCheckSum(pParams2);
        KLSTD_ASSERT_THROW(dwCheckSum1 == dwCheckSum2);
		Trace1(KLCS_MODULENAME, L"Comparing...\n");
		int result=pParams1->Compare(pParams2);
		if(result==0)
			Trace1(KLCS_MODULENAME, L"...Comparing SUCCESS\n");
		else{
			Trace1(KLCS_MODULENAME, L"...Comparing FAILURE\n");
			return false;
		};
	};

/*	{
		Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"\n", szwFile1);
		CAutoPtr<Params> pParams;	
		AcquireRandomParams(3, 5, 3, &pParams, NULL);
		WriteToFile(szwFile1, pParams);
	};
*/
	{
		Trace1(KLCS_MODULENAME, L"Reading from file \"%ls\"\n", szwFile1);
		CAutoPtr<Params> pParams;			
		ReadFromFile(szwFile1, &pParams);
		Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"\n", szwFile2);
		WriteToFile(szwFile2, pParams, bBinary);
	};

	Trace1(KLCS_MODULENAME, L"Comparing files \"%ls\" and \"%ls\"\n", szwFile1, szwFile2);
	return KLPAR_CompareFiles(szwFile1, szwFile2)==0;
};

///////////////////////////////////////////////////////
//
//			Проверка скорости добавления
//
///////////////////////////////////////////////////////
void testParAddTime()
{
	try{		
		CAutoPtr<KLPAR::ValuesFactory>	pFactory;		
		KLPAR_CreateValuesFactory(&pFactory);

		const int	nCycles=20;		
		const int	nValues=1000;

		int nSumma=0;
		long t1, t2, dt=0;
		
		static CAutoPtr<KLPAR::Params>	pParamsArray[nCycles];
		static std::wstring names[nValues];
		static CAutoPtr<KLPAR::StringValue> pValuesArray[nValues];

		for(int j=0; j < nValues; ++j){			
			pFactory->CreateStringValue(&pValuesArray[j]);
			pValuesArray[j]->SetValue(genRandString(16, nSumma).c_str());
			names[j]=genRandString(8, nSumma);
			
		};
		int i;
		for( i=0; i < nCycles; ++i)
			KLPAR_CreateParams(&pParamsArray[i]);

		{
			t1=GetSysTickCount();
			for(i=0; i < nCycles; ++i)
				for(int j=0; j < nValues; ++j)
					pParamsArray[i]->AddValue(names[j], pValuesArray[j]);
			t2=GetSysTickCount();
			double writes_per_second=double(nValues)*double(nCycles)*1000.0l/double(t2-t1);
			Trace1(KLCS_MODULENAME,
				L"%d values were written,it took %d ms\n"
				L"Writes per second: %lf\n",
				nValues*nCycles,
				t2-t1,
				writes_per_second);
		};

	}catch(KLERR::Error* pError){
		Trace1(KLCS_MODULENAME, L"%ls\n", pError->GetMsg());		
		pError->Release();
	};
};

///////////////////////////////////////////////////////
//
//			Проверка скорости поиска
//
///////////////////////////////////////////////////////
void testParFindTime()
{
	try{		
		CAutoPtr<KLPAR::ValuesFactory>	pFactory;		
		KLPAR_CreateValuesFactory(&pFactory);

		const bool	bCheckAll=true;
		const int	nCycles=2000;		
		const int	nValues=100;

		int nSumma=0;
		long t1, t2, dt=0;
		
		CAutoPtr<KLPAR::Params>	pParams;
		static std::wstring names[nValues];
		static CAutoPtr<KLPAR::StringValue> pValuesArray[nValues];
//		static CAutoPtr<KLPAR::Value> pValuesArray2[nValues][nCycles];

		KLPAR_CreateParams(&pParams);

		for(int j=0; j < nValues; ++j){			
			pFactory->CreateStringValue(&pValuesArray[j]);
			pValuesArray[j]->SetValue(genRandString(16, nSumma).c_str());
			names[j]=genRandString(8, nSumma);
			pParams->AddValue(names[j], pValuesArray[j]);			
		};
		// Проверка скорости поиска
		{
			CAutoPtr<KLPAR::Value> pValue;
			t1=GetSysTickCount();
			for(int i=0; i < nCycles; ++i)
				for(int j=0; j < nValues; ++j){					
//					pParams->GetValue(names[j], &pValuesArray2[j][i]);
					pParams->GetValue(names[j], &pValue);
					pValue=NULL;
				};
			t2=GetSysTickCount();
			double writes_per_second=double(nValues)*double(nCycles)*1000.0l/double(t2-t1);
			Trace1(KLCS_MODULENAME,
				L"%d values were found,it took %d ms\n"
				L"Writes per second: %lf\n",
				nValues*nCycles,
				t2-t1,
				writes_per_second);
		};

	}catch(KLERR::Error* pError){
		Trace1(KLCS_MODULENAME, L"%ls\n", pError->GetMsg());		
		pError->Release();
	};
};

///////////////////////////////////////////////////////
//
//			Проверка работы с окружением
//
///////////////////////////////////////////////////////

void testEnv()
{
	CAutoPtr<Params> pParams, pParams2;
	CAutoPtr<MemoryChunk> pChunk;
	SplitEnvironment(NULL, &pParams);
	MakeEnvironment(pParams, &pChunk);
	SplitEnvironment(pChunk->GetDataPtr(), &pParams2);	
	KLSTD_ASSERT_THROW(pParams->Compare(pParams2) == 0);
};

///////////////////////////////////////////////////////
//
//			Проверка скорости записи
//
///////////////////////////////////////////////////////

void testParWriteTime(int &nSumma, const std::wstring& wstrFileName, bool bBinary)
{
	KLSTD_USES_CONVERSION;
	try{
		CAutoPtr<KLPAR::Params>			pParams;
		CAutoPtr<KLPAR::ValuesFactory>	pFactory;

		KLPAR_CreateParams(&pParams);
		KLPAR_CreateValuesFactory(&pFactory);

		const int	nCycles=10;
		const bool	bUseBuffers=true;
		const int	nValues=1000;

		nSumma=0;
		for(int i=0; i < nValues; ++i){
			std::wstring name=genRandString(8, nSumma);
			std::wstring value=genRandString(16, nSumma);
			AddValue(pParams, pFactory, name, value);
		};
#ifdef N_PLAT_NLM
//		int nId=access(
//					KLSTD_W2A(wstrFileName.c_str()),
//					O_BINARY| O_CREAT| O_TRUNC| O_RDWR |
//					S_IREAD | S_IWRITE);
#endif // N_PLAT_NLM
#ifdef WIN32
		int nId=_topen(
					KLSTD_W2CT(wstrFileName.c_str()),
					_O_BINARY|_O_CREAT|_O_TRUNC|_O_RDWR,
					_S_IREAD | _S_IWRITE);
#endif // WIN32
#ifdef __unix
		int nId = open(KLSTD_W2A(wstrFileName.c_str()), O_CREAT| O_TRUNC| O_RDWR, 0666 );
#endif
#ifdef N_PLAT_NLM
        _conv c;
		int nId = open( c.narrow(wstrFileName).data(), O_CREAT| O_TRUNC| O_RDWR | O_BINARY, S_IWRITE | S_IREAD );
#endif // N_PLAT_NLM


		if(nId==0 || nId==-1)
			KLSTD_THROW(STDE_NOACCESS);
		//	... 
		long t1, t2, dt=0;
        t1=GetSysTickCount();
		for(int j=0; j < nCycles; ++j)
            ::KLPAR_SerializeToFileID2(nId, bBinary?KLPAR_SF_BINARY:0, pParams);
		t2=GetSysTickCount();
		dt=t2-t1;
		close(nId);
		double ms_per_cycle=double(dt)/(double(nCycles));
		double mbyte_per_sec=(double(nSumma)*double(1000))/(double(ms_per_cycle)*double(1024*1024));
		Trace1(
			KLCS_MODULENAME,
			L"%ls processed %d bytes\n"
			L"Made %d cycles\n"
			L"It took %d ms\n"
			L"Is %lf ms per cycle or %lf Mb/sec\n",
            (bBinary?L"Binary":L"XML"),
			nSumma,
			nCycles,
			dt,
			ms_per_cycle,
			mbyte_per_sec);

		pParams=NULL;
		pFactory=NULL;
	}catch(KLERR::Error* pError){
		Trace1(KLCS_MODULENAME, L"%ls\n", pError->GetMsg());		
		pError->Release();
	};
};

///////////////////////////////////////////////////////
//
//			Проверка скорости Десериилизации
//
///////////////////////////////////////////////////////

void testParReadTime(bool bBinary)
{
	KLERR_TRY
		int nSumma=0;
//		wchar_t szwBuff[16]=L"";
//		_ltow(GetCurrentThreadId(), szwBuff, 16);

        std::wstringstream ss;
        ss << "__file" << KLSTD_GetCurrentThreadId();

//		std::wstring wtrFileName=std::wstring(L"__file") + szwBuff;
//		testParWriteTime(nSumma, wtrFileName.c_str());		
		testParWriteTime(nSumma, ss.str().c_str(), bBinary);
		CAutoPtr<KLPAR::Params>			pParams;

		const int nCycles=10;
		long t1, t2, dt=0;
		for(int j=0; j < nCycles; ++j){
			CAutoPtr<File> pFile;
			::KLSTD_CreateFile( ss.str() /*wtrFileName */, SF_READ, CF_OPEN_EXISTING, AF_READ, EF_SEQUENTIAL_SCAN, &pFile);
			t1=GetSysTickCount();
			::KLPAR_DeserializeFromFileID(pFile->GetFileID(), &pParams);
			t2=GetSysTickCount();
			int tmp=t2-t1;
			dt+=tmp;
			pParams=NULL;
		};

		double ms_per_cycle=double(dt)/(double(nCycles));
		double kbyte_per_sec=(double(nSumma)*double(1000))/(double(dt)*double(1024));
		Trace1(
			KLCS_MODULENAME,
			L"Processed %d bytes\n"
			L"Made %d cycles\n"
			L"It took %d ms\n"
			L"Is %f ms per cycle or %f Mb/sec\n",
			nSumma,
			nCycles,
			dt,
			ms_per_cycle,
			kbyte_per_sec/1024.0l);

		pParams=NULL;
	KLERR_CATCH(pError)
		Trace1(KLCS_MODULENAME, L"%ls\n", pError->GetMsg());		
	KLERR_ENDTRY
};

void testParWriteTime(bool bBinary)
{
	int nSumma=0;

#if defined(_WIN32)
	std::wstring wstrFileName=L"NUL";

#elif defined(__unix)

    std::wstring wstrFileName=L"/dev/null";

#else

    #error "Not implemented"

#endif
	testParWriteTime(nSumma, wstrFileName, bBinary);
};

///////////////////////////////////////////////////////
//
//		Проверка метода Clone
//
///////////////////////////////////////////////////////
bool Test_Clone(const wchar_t* szwFile1, const wchar_t* szwFile2, bool bBinary)
{
	Trace1(KLCS_MODULENAME, L"Cloning...\n");	
	CAutoPtr<Params> pParams, pClonedParams;
	AcquireRandomParams(random(2, 4), random(3, 10), random(2, 4), &pParams, NULL);
    {
        CAutoPtr<ParamsValue> pValue;
        CAutoPtr<ValuesFactory> pFactory;
        KLPAR_CreateValuesFactory(&pFactory);
        pFactory->CreateParamsValue(&pValue);
        pValue->SetValue(NULL);
        pParams->AddValue(L"NULL_PARAMS", pValue);
    };    
	pParams->Clone(&pClonedParams);
	Trace1(KLCS_MODULENAME, L"...Cloning OK\n");

	Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"...\n", szwFile1);
	WriteToFile(szwFile1, pParams, bBinary);
	Trace1(KLCS_MODULENAME, L"...Writing to file \"%ls\" OK\n", szwFile1);
	pParams=NULL;

	Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"...\n", szwFile2);
	WriteToFile(szwFile2, pClonedParams, bBinary);
	Trace1(KLCS_MODULENAME, L"...Writing to file \"%ls\" OK\n", szwFile2);

	Trace1(KLCS_MODULENAME, L"Comparing files \"%ls\" and \"%ls\"...\n", szwFile1, szwFile2);
	int nResult=KLPAR_CompareFiles(szwFile1, szwFile2);
	if(nResult==0)
		Trace1(KLCS_MODULENAME, L"...Comparing files OK\n");
	else
		Trace1(KLCS_MODULENAME, L"...FAILED: files are different OK\n");
	;

	{
		CAutoPtr<Params> pData1, pData2;
		KLPAR_CreateParams(&pData1);
		CAutoPtr<StringValue> pString;
		KLPAR::CreateValue(L"Test", &pString);
		pData1->AddValue(L"x", pString);
		pData1->Clone(&pData2);
		KLSTD_ASSERT_THROW(pData1->Compare(pData2) == 0);
		pData2->DeleteValue(L"x", true);
		KLSTD_ASSERT_THROW(pData2->Compare(pData1) != 0);
	};
	{
		KLSTD::CAutoPtr<KLPAR::Params> pParams, pCloneParams;
		KLPAR_CreateParams(&pParams);
		
		KLSTD::CAutoPtr<KLPAR::StringValue> pString;
		KLPAR::CreateValue(L"Test", &pString);
		pParams->AddValue(L"StringValue", pString);

		KLSTD::CAutoPtr<KLPAR::DoubleValue> pDoubleValue;
		KLPAR::CreateValue(1.0, &pDoubleValue);
		pParams->AddValue(L"DoubleValue", pDoubleValue);

		pParams->Clone(&pCloneParams);
		KLSTD_ASSERT_THROW(pCloneParams->Compare( pParams ) == 0);
		pCloneParams->DeleteValue(L"StringValue", true);
		KLSTD_ASSERT_THROW( pCloneParams->Compare(pParams) != 0 );
	};	
	return nResult==0;
};

///////////////////////////////////////////////////////
//
//		Проверка метода Duplicate
//
///////////////////////////////////////////////////////
bool Test_Duplicate(const wchar_t* szwFile1, const wchar_t* szwFile2, bool bBinary)
{
	Trace1(KLCS_MODULENAME, L"Duplicating...\n");	
	CAutoPtr<Params> pParams, pDuplicatedParams;
	AcquireRandomParams(random(2, 4), random(3, 10), random(2, 4), &pParams, NULL);
	pParams->Duplicate(&pDuplicatedParams);
	Trace1(KLCS_MODULENAME, L"...Duplicating OK\n");

	Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"...\n", szwFile1);
	WriteToFile(szwFile1, pParams, bBinary);
	Trace1(KLCS_MODULENAME, L"...Writing to file \"%ls\" OK\n", szwFile1);
	pParams=NULL;

	Trace1(KLCS_MODULENAME, L"Writing to file \"%ls\"...\n", szwFile2);
	WriteToFile(szwFile2, pDuplicatedParams, bBinary);
	Trace1(KLCS_MODULENAME, L"...Writing to file \"%ls\" OK\n", szwFile2);

	Trace1(KLCS_MODULENAME, L"Comparing files \"%ls\" and \"%ls\"...\n", szwFile1, szwFile2);
	int nResult=KLPAR_CompareFiles(szwFile1, szwFile2);
	if(nResult==0)
		Trace1(KLCS_MODULENAME, L"...Comparing files OK\n");
	else
		Trace1(KLCS_MODULENAME, L"...FAILED: files are different OK\n");
	;

	{
		CAutoPtr<Params> pData1, pData2;
		KLPAR_CreateParams(&pData1);
		CAutoPtr<StringValue> pString;
		KLPAR::CreateValue(L"Test", &pString);
		pData1->AddValue(L"x", pString);
		pData1->Duplicate(&pData2);
		KLSTD_ASSERT_THROW(pData1->Compare(pData2) == 0);
		pData2->DeleteValue(L"x", true);
		KLSTD_ASSERT_THROW(pData2->Compare(pData1) != 0);
	};
	{
		KLSTD::CAutoPtr<KLPAR::Params> pParams, pDuplicateParams;
		KLPAR_CreateParams(&pParams);
		
		KLSTD::CAutoPtr<KLPAR::StringValue> pString;
		KLPAR::CreateValue(L"Test", &pString);
		pParams->AddValue(L"StringValue", pString);

		KLSTD::CAutoPtr<KLPAR::DoubleValue> pDoubleValue;
		KLPAR::CreateValue(1.0, &pDoubleValue);
		pParams->AddValue(L"DoubleValue", pDoubleValue);

		pParams->Duplicate(&pDuplicateParams);
		KLSTD_ASSERT_THROW(pDuplicateParams->Compare( pParams ) == 0);
		pDuplicateParams->DeleteValue(L"StringValue", true);
		KLSTD_ASSERT_THROW( pDuplicateParams->Compare(pParams) != 0 );
	};	
	return nResult==0;
};

void test_ParamsSubtree()
{
    const wchar_t c_szwVal1[]=L"Value 1";
    const wchar_t c_szwVal2[]=L"Value 2";
    const wchar_t c_szwVal3[]=L"Value 3";
    const wchar_t c_szwVal4[]=L"Value 4";
    param_entry_t pars[]=
    {
        param_entry_t(c_szwVal1, L"aaa"),
        param_entry_t(c_szwVal2, L"bbb"),
        param_entry_t(c_szwVal3, L"ccc"),
        param_entry_t(c_szwVal4, L"ddd")
    };
    CAutoPtr<Params> pData;
    CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pData);

    {
        CAutoPtr<Params> pFilter, pDst;
        pData->Clone(&pFilter);
        KLPAR_GetParamsSubtree(pData, pFilter, &pDst);
        KLSTD_ASSERT_THROW(pData->Compare(pDst) == 0);
    };

    {
        CAutoPtr<Params> pFilter, pDst;
        param_entry_t pars[]=
        {
            param_entry_t(c_szwVal1, L"aaa"),
            param_entry_t(c_szwVal3, L"ccc"),
        };
        CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pFilter);
        KLPAR_GetParamsSubtree(pData, pFilter, &pDst);
        KLSTD_ASSERT_THROW(pFilter->Compare(pDst) == 0);
    };

};
