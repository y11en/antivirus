

#include <sstream>

#include "build/general.h"
#include "std/io/klio.h"
#include "std/trc/trace.h"
#include "std/conv/klconv.h"
#include "std/thr/thread.h"
#include "std/par/paramsi.h"
#include "soapH.h"

#if defined(__unix) || defined(N_PLAT_NLM)
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#endif

#ifdef _WIN32
#include <tchar.h>
#include <crtdbg.h>
#include <windows.h>
#include <sys/stat.h>
#endif

#ifdef N_PLAT_NLM
# include <unicode.h>
# include <sys/stat.h>
# include <std/conv/wcharcrt.h>
# include <conv/_conv.h>
#endif

//#ifdef N_PLAT_NLM
//# include <nwthread.h>

//int GetCurrentThreadId()
//{
//	return GetThreadID();
//}
//#endif

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;
using namespace std;


#define KLCS_MODULENAME L"PARTST"

static int random(int x)
{
	int nRand = KLSTD_Random(0, x);
	//printf("random(%u)=%u\n", x, nRand);
    return nRand;
};

int random(int nMin, int nMax)
{
    int nRand = KLSTD_Random(nMin, nMax);
    //printf( "KLSTD_Random(%u, %u) = %u\n", nMin, nMax, nRand);

    return nRand;
	//return random(nMax-nMin+1)+nMin;
};

std::wstring genRandString(int nLen, int& summa)
{
	summa += 2*nLen;
	static wchar_t symbols[]=L"1234567890_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const size_t nChars=KLSTD_COUNTOF(symbols)-1;
	wchar_t* szBuffer=(wchar_t*)alloca(sizeof(wchar_t)*(nLen+1));
	for(int i=0; i < nLen; ++i)
		szBuffer[i]=symbols[random(nChars-1)];
	szBuffer[nLen]=0;

    //printf("genRandString(%u)='%ls'\n", nLen, szBuffer);
	return szBuffer;
};

std::wstring genRandString(int nLen)
{
	static wchar_t symbols[]=L"1234567890_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int nChars=KLSTD_COUNTOF(symbols)-1;
	wchar_t* szBuffer=(wchar_t*)alloca(sizeof(wchar_t)*(nLen+2));
	for(int i=0; i < nLen; ++i)
		szBuffer[i]=symbols[random(nChars-1)];
	szBuffer[nLen]=0;
	//printf("genRandString(%u)='%ls'\n", nLen, szBuffer);
	return szBuffer;
};

void AddValue(
						KLPAR::Params* pParams,
						KLPAR::ValuesFactory* pFactory,
						std::wstring& strName,
						std::wstring& strVal)
{
	CAutoPtr<KLPAR::StringValue> pValue;
	pFactory->CreateStringValue(&pValue);
	pValue->SetValue(strVal.c_str());
	pParams->AddValue(strName, pValue);
};

static void MyFreeBufferCallback(void * buff, void * context)
{
	if(context!=buff){
		Trace1(KLCS_MODULENAME, L"Fatal: context!=buff\n");
		KLSTD_THROW(STDE_GENERAL);
	};
	free(buff);
};

static void CreateRandomValue(ValuesFactory* pFactory, Value** ppValue, long* pSum)
{
	int x=random(1, 10);
	switch(x){
	case 1:
		{
			CAutoPtr<StringValue> pValue;
			pFactory->CreateStringValue(&pValue);
			pValue->SetValue(genRandString(random(0, 1)).c_str());
			pValue.CopyTo((StringValue**)ppValue);			
		}
		break;
	case 2:
		{
			CAutoPtr<BoolValue> pValue;
			pFactory->CreateBoolValue(&pValue);
			pValue->SetValue(random(0, 1)!=0);
			pValue.CopyTo((BoolValue**)ppValue);
		}
		break;
	case 3:
		{
			CAutoPtr<IntValue> pValue;
			pFactory->CreateIntValue(&pValue);
			pValue->SetValue(rand());
			pValue.CopyTo((IntValue**)ppValue);
		}
		break;
	case 4:
		{
			CAutoPtr<LongValue> pValue;
			pFactory->CreateLongValue(&pValue);
			pValue->SetValue(rand());
			pValue.CopyTo((LongValue **)ppValue);
		}
		break;
	case 5:
		{
			CAutoPtr<DateTimeValue> pValue;
			pFactory->CreateDateTimeValue(&pValue);
			if(random(0, 1) == 0)
				pValue->SetValue(time_t(random(0, SHRT_MAX)));
			else
				pValue->SetValue(-1);
			pValue.CopyTo((DateTimeValue **)ppValue);
		}
		break;
	case 6:
		{
			CAutoPtr<DateValue> pValue;
			pFactory->CreateDateValue(&pValue);
			pValue->SetValue("01-01-2001");
			pValue.CopyTo((DateValue **)ppValue);
		}
		break;
	case 7:
		{
			CAutoPtr<BinaryValue> pValue;
			pFactory->CreateBinaryValue(&pValue);
			if(random(0, 1) == 0){
				const int nSize=random(0, 32);
				if(nSize)
					pValue->SetValue(memset(alloca(nSize), 0, nSize), nSize);
				else
					pValue->SetValue(NULL, 0);
			}
			else{
				const int nSize=random(1, 32);
				void* p=malloc(nSize);
				pValue->SetValue(memset(p, 0, nSize), nSize, MyFreeBufferCallback, p);
			};
			pValue.CopyTo((BinaryValue **)ppValue);
		}
		break;
	case 8:
		{
			CAutoPtr<FloatValue> pValue;
			pFactory->CreateFloatValue(&pValue);
			pValue->SetValue((float)random(8191));
			pValue.CopyTo((FloatValue **)ppValue);
		}
		break;
	case 9:
		{
			CAutoPtr<DoubleValue> pValue;
			pFactory->CreateDoubleValue(&pValue);
			pValue->SetValue(random(8191));
			pValue.CopyTo((DoubleValue **)ppValue);
		}
		break;
	case 10:
		{
			CAutoPtr<ArrayValue> pValue;
			pFactory->CreateArrayValue(&pValue);
			const int nSize=random(0, 4);
			pValue->SetSize(nSize);
			for(int i=0; i < nSize; ++i)
			{
				CAutoPtr<Value> pValue2;
                CreateRandomValue(pFactory, &pValue2, pSum);
				pValue->SetAt(i, pValue2);
			};
			pValue.CopyTo((ArrayValue **)ppValue);
		}
		break;
	default:
		KLSTD_ASSERT(false);
	};
	if(pSum)
		++(*pSum);
};

wstring makePrefixByType(Value* pValue)
{
	switch(pValue->GetType())
	{
	case Value::EMPTY_T:
		return L"emp-";
	case Value::STRING_T:
		return L"str-";
	case Value::BOOL_T:
		return L"b-";
	case Value::INT_T:
		return L"l-";
	case Value::LONG_T:
		return L"ll-";
	case Value::DATE_TIME_T:
		return L"tm-";
	case Value::DATE_T:
		return L"date-";
	case Value::BINARY_T:
		return L"bin-";
	case Value::FLOAT_T:
		return L"f-";
	case Value::DOUBLE_T:
		return L"lf-";
	case Value::PARAMS_T:
		return L"par-";
	case Value::ARRAY_T:
		return L"arr-";
	default:
		KLSTD_ASSERT(false);
	};
	return L"";
};

static void AddRandomValue(Params* pParams, ValuesFactory* pFactory, long *pSum)
{
	CAutoPtr<Value> pValue;
	CreateRandomValue(pFactory, &pValue, pSum);
	pParams->AddValue(makePrefixByType(pValue) + genRandString(random(8, 10)), pValue);
};

static void AddVars(Params* pParams, ValuesFactory* pFactory, int nCount, long *pSum)
{
	for(int i=0; i < nCount; ++i){
		AddRandomValue(pParams, pFactory, pSum);
	};
};

static void AddNodes(Params* pParams, ValuesFactory* pFactory, int nLevels, int nVars, int nNodes, long *pSum)
{
	AddVars(pParams, pFactory, nVars, pSum);
	if(nLevels > 0){
		for(int i=0; i < nNodes; ++i){
			CAutoPtr<ParamsValue> pParamsValue;
			pFactory->CreateParamsValue(&pParamsValue);
			pParams->AddValue(genRandString(8), pParamsValue);
			if(pSum)++(*pSum);
			AddNodes(pParamsValue->GetValue(), pFactory, nLevels-1, nVars, nNodes, pSum);
		};
	};
};

void AcquireRandomParams(int nLevels, int nVars, int nNodes, Params** ppParams, long* pSum)
{
	Trace1(KLCS_MODULENAME, L"Creating Params (%d levels, %d vars, %d nodes) ...\n", nLevels, nVars, nNodes);
	try{
		if(pSum)
			(*pSum)=0;
		if(nLevels <=0)
			nLevels=1;
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateParams(ppParams);
		KLPAR_CreateValuesFactory(&pFactory);
		AddVars(*ppParams, pFactory, nVars, pSum);
		for(int i=0; i < nNodes; ++i)
			AddNodes(*ppParams, pFactory, nLevels, nVars, nNodes, pSum);
	}catch(...)
	{
		Trace1(KLCS_MODULENAME, L"...creating Params FAILED !!!\n");
		throw;
	};
	Trace1(KLCS_MODULENAME, L"...creating Params OK\n");
};

int KLPAR_CompareFiles(const wchar_t* szwFile1, const wchar_t* szwFile2)
{
#if defined(_WIN32) || defined(__unix)
	CAutoPtr<File> pFile1, pFile2;
	CAutoPtr<FileMapping> pMapping1, pMapping2;

	KLSTD_CreateFile(szwFile1, 0, CF_OPEN_EXISTING, AF_READ, EF_SEQUENTIAL_SCAN|EF_TEMPORARY, &pFile1, 0);
	KLSTD_CreateFile(szwFile2, 0, CF_OPEN_EXISTING, AF_READ, EF_SEQUENTIAL_SCAN|EF_TEMPORARY, &pFile2, 0);
	if(pFile1->GetSize()!=pFile2->GetSize())
		return pFile2->GetSize() - pFile1->GetSize();
	if(!pFile1->GetSize())
		return 0;
	pFile1->Map(AF_READ, 0, pFile1->GetSize(), &pMapping1);
	pFile2->Map(AF_READ, 0, pFile2->GetSize(), &pMapping2);
	return memcmp(pMapping1->GetPointer(), pMapping2->GetPointer(), pMapping1->GetSize());
#endif
#ifdef N_PLAT_NLM
    _conv _c;

    string fn1 = _c.narrow( szwFile1 );
    string fn2 = _c.narrow( szwFile2 );

    ifstream f1( fn1.data() );
    ifstream f2( fn2.data() );
    istream_iterator<char> i1( f1 );
    istream_iterator<char> i2( f2 );
    return lexicographical_compare_3way( i1, istream_iterator<char>(), i2, istream_iterator<char>() );
   
#endif
};


int __inline sign(int x)
{
	return (x==0) ? 0: ((x < 0) ? -1 : 1);
};

std::wstring MakePerThreadFileName(const wchar_t* szwTemplate)
{
    std::wstring wstrPath;
    {
#ifdef _WIN32
        TCHAR szDir[MAX_PATH] = _T("");
        GetModuleFileName(NULL, szDir, KLSTD_COUNTOF(szDir)-1);    
#elif defined(N_PLAT_NLM)
		char szDir[] = "sys:/klab/adminkit/test/std/stdtest.mdm";
#else 
		char szDir[] = "stdtest";
#endif
        std::wstring wstrName, wstrExt;
        KLSTD_SplitPath((const wchar_t*)KLSTD_T2CW2(szDir), wstrPath, wstrName, wstrExt);
    };
    ;
	int len=wcslen(szwTemplate)+16;
	wchar_t* szwBuffer=(wchar_t*)alloca(len*sizeof(wchar_t));
	KLSTD_SWPRINTF(szwBuffer, len, szwTemplate, KLSTD_GetCurrentThreadId());
    ;
    std::wstring wstrResult;
    KLSTD_MakePath(wstrPath, szwBuffer, L"", wstrResult);
    return wstrResult;
};

