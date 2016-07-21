#include <build/general.h>
#include <std/base/klbase.h>
#include <std/base/kldefs.h>
#include <std/trc/trace.h>
#include <std/conv/klconv.h>
#include <algorithm>
#include <std/par/params.h>
#include <std/par/paramsi.h>
#include <std/par/errors.h>
#include <transport/tr/transport.h>
#include <transport/tr/transportproxy.h>
#include <kca/prci/componentid.h>
#include <kca/prci/componentinstance.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/errors.h>
#include <kca/prcp/componentproxy.h>
#include "soapH.h"
#include <common/policy.h>
#include <common/measurer.h>
#ifdef _WIN32
# include <byteorder.h>
#endif
#include "../jrnl/osdep.h"
#include <fstream>

#include <kca/prts/prxsmacros.h>
#include "presdefs.h"
#include "pressubscriptionlist.h"
#include "prespersistenteventlist.h"

#ifdef N_PLAT_NLM
# include "std/conv/wcharcrt.h"
# include "std/conv/_conv.h"
#endif

#include <sstream>

namespace KLPRES {

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLPOL;
using namespace KLPRCI;
using namespace KLJRNL;
using namespace KLTMSG;
using namespace KLPRCP;

const wchar_t KLCS_MODULENAME[] = L"PRES:EventsStorage";
const wchar_t STORAGE_FILE_EXTENTION[] = L".jrnl";
#define START_PROPERTY_ID		1000

const wchar_t PRES_EVENT_SUBSCRIPTION_ADDED[] = L"KLPRES_EVENT_SUBSCRIPTION_ADDED";			// добавлена подписка
const wchar_t PRES_EVENT_SUBSCRIPTION_DELETED[] = L"KLPRES_EVENT_SUBSCRIPTION_DELETED";		// удалена подписка
const wchar_t PRES_EVENT_SUBSCRIPTION_UPDATED[] = L"KLPRES_EVENT_SUBSCRIPTION_UPDATED";		// обновлена подписка

/* PRES_SUBSCRIPTION_VERSION_ID - GUID, который соответствует подписке. Хранится
   в PRESe в описании подписки, генерируется при добавлении и каждом изменении подписки.
   Т.е. SubscriptionId остается тем же, но по изменению PRES_SUBSCRIPTION_VERSION_ID можно
   отследить Update, произведенный другим PRES-сервером.
*/
const wchar_t PRES_SUBSCRIPTION_VERSION_ID[] = L"PRES_SUBSCRIPTION_VERSION_ID";

/* #define PAUSED_FLAG_CHAR    'p'
#define RUNNING_FLAG_CHAR   'r' */

#define LOCAL_SUBSCR_ITER_ID_PREFIX   L"PRES_LOCAL_"

#define PAUSED_SUBSCR_STORAGE_PREFIX   L"paused_"

#ifndef _WIN32
int LittleEndianToMachine( int x )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  return x;
#else
# error fix here
#endif
}

int MachineToLittleEndian( int x )
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  return x;
#else
# error fix here
#endif
}
#endif

bool GetRawDataFromStrData(
    const std::string strData,
    std::wstring & wstrSubscrId,
    std::string & strSerializedSubscrParams )
{
    bool bStillOk = true;
    bool bDeserializeOk = false;
    AVP_int32 nSize32;

    const char* pCurrData = strData.c_str();
    const char* pEndOfData = pCurrData + strData.size();
        
    if( pCurrData + sizeof(AVP_int32) >= pEndOfData ) goto bad_data;
    nSize32 = LittleEndianToMachine( *((AVP_int32*)(strData.c_str())) );
    if( nSize32 <= 0 ) goto bad_data;
    pCurrData += sizeof( AVP_int32 );

    if( pCurrData + nSize32*sizeof(wchar_t) >= pEndOfData ) goto bad_data;
    wstrSubscrId = wstring( (wchar_t*)pCurrData, nSize32 );
    pCurrData += nSize32*sizeof(wchar_t);

    if( pCurrData >= pEndOfData ) goto bad_data;
    strSerializedSubscrParams = string( pCurrData, pEndOfData - pCurrData );

    return true;

bad_data:
    return false;
}

bool GetFromStrData(
    const std::string strData,
    std::wstring & wstrSubscrId,
    // bool & bPaused,
    KLPAR::Params** pparSubscription )
{
    KLSTD_CHKOUTPTR( pparSubscription );

    string strSerializedSubscrParams;
    if( ! GetRawDataFromStrData( strData, wstrSubscrId, strSerializedSubscrParams ) )
    {
        return false;
    }

    bool bDeserializeOk = false;
    KLERR_BEGIN
        KLPAR_DeserializeFromMemory( strSerializedSubscrParams.c_str(), strSerializedSubscrParams.size(), pparSubscription );
        bDeserializeOk = true;
    KLERR_END;

    return bDeserializeOk;
}

void MoveJrnlPosToSubscription(
    KLSTD::CAutoPtr<KLJRNL::Journal> pJournal,
    const std::wstring & wstrSubscrIdToFind,
    std::string & strSerializedSubscrParams )
{
    pJournal->ResetIterator();
    string strData;
    wstring wstrSubscrIdFromJrnl;
    
    while( ! pJournal->IsEOF() )
    {
        long nId;
        pJournal->GetCurrent( nId, strData );

        if( GetRawDataFromStrData( strData, wstrSubscrIdFromJrnl, strSerializedSubscrParams ) && 
            ( wstrSubscrIdFromJrnl == wstrSubscrIdToFind ) )
        {
            break;
        }

        pJournal->Next();
    }
}

//////////////////////////////////////////////////////////////////////
// CEventInfo

// Creates object that represents new Event
CEventInfo::CEventInfo(CSubscription* pSubscription,
						long nID,
						KLPRCI::ComponentId &cidPublisher,
						std::wstring sEventType,
						KLPAR::Params* parEventBody,
						time_t EventTime,
						int nLifetime)
{
	m_pSubscription = pSubscription;
	m_nID = nID;
	m_cidPublisher = cidPublisher;
	m_sEventType = sEventType;
	m_parEventBody = parEventBody;
	m_EventTime = EventTime;
	m_nLifetime = nLifetime;
}

void CEventInfo::Serialize(std::string & os) const
{
	CAutoPtr<Params> parParams;
	SerializeParams(&parParams);

	CAutoPtr<MemoryChunk> pChunk;
	KLPAR_SerializeToMemory(parParams, &pChunk);

	os = std::string((const char*)pChunk->GetDataPtr(), pChunk->GetDataSize());
}

void CEventInfo::Deserialize(const std::string & is)
{
	CAutoPtr<Params> parParams;
	KLPAR_DeserializeFromMemory(is.c_str(), is.size(), &parParams);
	DeserializeParams(parParams);
}

void CEventInfo::SerializeParams(KLPAR::Params** pparParams) const
{
	KLSTD_CHKOUTPTR(pparParams);
	::KLPAR_CreateParams(pparParams);

	ADD_CID_PARAMS_VALUE(*pparParams, SS_VALUE_NAME_EVENT_CID, m_cidPublisher);
	ADD_PARAMS_VALUE(*pparParams, SS_VALUE_NAME_EVENT_TYPE, StringValue, m_sEventType.c_str());
	ADD_PARAMS_VALUE(*pparParams, SS_VALUE_NAME_EVENT_BODY, ParamsValue, m_parEventBody);
	ADD_PARAMS_VALUE(*pparParams, SS_VALUE_NAME_EVENT_TIME, DateTimeValue, m_EventTime);
	ADD_PARAMS_VALUE(*pparParams, SS_VALUE_NAME_EVENT_ID, IntValue, m_nID);
	ADD_PARAMS_VALUE(*pparParams, SS_VALUE_NAME_EVENT_LIFETIME, IntValue, m_nLifetime);
    ADD_PARAMS_VALUE(*pparParams, ID_ACROSS_PRES_INSTANCE, LongValue, m_llEventIdAcrossInstance );
    ADD_PARAMS_VALUE(*pparParams, PRES_INSTANCE_ID, StringValue, m_wstrInstanceId.c_str() );
}
void CEventInfo::DeserializeParams(const KLPAR::Params* parParams)
{
	KLSTD_CHKINPTR(parParams);

	GET_CID_PARAMS_VALUE(parParams, SS_VALUE_NAME_EVENT_CID, m_cidPublisher);
	GET_PARAMS_VALUE(parParams, SS_VALUE_NAME_EVENT_TYPE, StringValue, STRING_T, m_sEventType);
	GET_PARAMS_VALUE(parParams, SS_VALUE_NAME_EVENT_BODY, ParamsValue, PARAMS_T, m_parEventBody);
	GET_PARAMS_VALUE(parParams, SS_VALUE_NAME_EVENT_TIME, DateTimeValue, DATE_TIME_T, m_EventTime);
	GET_PARAMS_VALUE(parParams, SS_VALUE_NAME_EVENT_LIFETIME, IntValue, INT_T, m_nLifetime);
	GET_PARAMS_VALUE(parParams, SS_VALUE_NAME_EVENT_ID, IntValue, INT_T, m_nID);
    GET_PARAMS_VALUE_NO_THROW(parParams, ID_ACROSS_PRES_INSTANCE, LongValue, LONG_T, m_llEventIdAcrossInstance );
    GET_PARAMS_VALUE_NO_THROW(parParams, PRES_INSTANCE_ID, StringValue, STRING_T, m_wstrInstanceId );
    if( ! m_parEventBody ) KLPAR_CreateParams( & m_parEventBody );
    REPLACE_PARAMS_VALUE(m_parEventBody, ID_ACROSS_PRES_INSTANCE, LongValue, m_llEventIdAcrossInstance );
    REPLACE_PARAMS_VALUE(m_parEventBody, PRES_INSTANCE_ID, StringValue, m_wstrInstanceId.c_str() );
}

//////////////////////////////////////////////////////////////////////
// CEventInfoAsPolicy

void CEventInfoAsPolicy::Serialize(std::string & os) const
{
	LPPOLICY pSerialisedEventInfo = NULL;

	POLICY_BEGIN(m_nID, 0)

	for(unsigned int i = 0;i < m_pSubscription->m_EventInfoMask.Count();i++)
	{
		CEventInfoMaskItem* pMaskItem = m_pSubscription->m_EventInfoMask[i];
		if (pMaskItem)
		{
			long nPropertyID = pMaskItem->m_nID;
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_PUBLISHER_CID) == 0)
			{
				ADD_PROP_STR(nPropertyID, m_cidPublisher.PutToString().c_str());
			}
			else
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TYPE) == 0)
			{
				ADD_PROP_STR(nPropertyID, m_sEventType.c_str());
			}
			else
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TIME) == 0)
			{
				ADD_PROP_DWORD(nPropertyID, (long)m_EventTime);
			}
			else
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_LIFETIME) == 0)
			{
				ADD_PROP_DWORD(nPropertyID, m_nLifetime);
			}
			else
			{
                CAutoPtr<Value> pValue;
                std::vector<std::wstring> vecPath;
                KLSTD_SplitString(pMaskItem->m_sValueName, KLPRES_EVENT_NAME_DIVIDER, vecPath);
                KLSTD_ASSERT_THROW(vecPath.size() > 0);

                std::vector<const wchar_t*> vecPath2;
                vecPath2.resize(vecPath.size() - 1, NULL);
                for(unsigned int jj = 1;jj < vecPath.size()-1;++jj) vecPath2[jj - 1] = vecPath[jj].c_str();                    
                
				KLERR_IGNORE(KLPAR_GetValue(m_parEventBody, &(*(vecPath2.begin())), vecPath[vecPath.size()-1], &pValue));

				if (pValue)
				{
					switch(pValue->GetType())
					{
						case Value::BOOL_T:
							ADD_PROP_DWORD(nPropertyID, (long)((BoolValue*)(Value*)pValue)->GetValue());
						break;
						case Value::INT_T:
							ADD_PROP_DWORD(nPropertyID, (long)((IntValue*)(Value*)pValue)->GetValue());
						break;
						case Value::LONG_T:
						{
							AVP_longlong n = ((LongValue*)(Value*)pValue)->GetValue();
							ADD_PROP_BIN(nPropertyID, (void*)&n, sizeof(AVP_longlong), false);
						}
						break;
						case Value::DATE_TIME_T:
							ADD_PROP_DWORD(nPropertyID, (long)((DateTimeValue*)(Value*)pValue)->GetValue());
						break;
						case Value::FLOAT_T:
							ADD_PROP_DWORD(nPropertyID, (long)((FloatValue*)(Value*)pValue)->GetValue());
						break;
						case Value::STRING_T:
						{
#ifdef _WIN32
								int nSize = WideCharToMultiByte(CP_UTF8, 0, ((StringValue*)(Value*)pValue)->GetValue(), -1, NULL, 0, NULL, NULL);
								if (nSize > 0)
								{
									CArrayPointer<char> psz((char*)malloc(nSize * sizeof(char)));
									WideCharToMultiByte(CP_UTF8, 0, ((StringValue*)(Value*)pValue)->GetValue(), -1, psz, nSize, NULL, NULL);
									ADD_PROP_BIN(nPropertyID, psz, nSize * sizeof(char), FALSE);
								}
#else
								KLSTD_USES_CONVERSION;
								ADD_PROP_STR(nPropertyID, ((StringValue*)(Value*)pValue)->GetValue());
#endif
						}
						break;
						default:
							continue;
					}
				}
			}
		}
	}
	POLICY_END(pSerialisedEventInfo)

	os = std::string((const char*)(POLICY*)pSerialisedEventInfo, pSerialisedEventInfo->Size);

	free(pSerialisedEventInfo);
}

void CEventInfoAsPolicy::Deserialize(const std::string & is)
{
	if (m_parEventBody) m_parEventBody->Clear();

	LPPOLICY pSerialisedEventInfo = (LPPOLICY)is.c_str();
	if (pSerialisedEventInfo)
	{
		LPPROPERTY p = GetFirstProperty(pSerialisedEventInfo);
		while(p)
		{
			CEventInfoMaskItem* pMaskItem = m_pSubscription->m_EventInfoMask.GetById(p->ID);
			if (pMaskItem)
			{
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_PUBLISHER_CID) == 0)
				{
					m_cidPublisher.GetFromString(std::wstring((wchar_t*)p->Data));
				}
				else
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TYPE) == 0)
				{
					m_sEventType = (wchar_t*)p->Data;
				}
				else
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TIME) == 0)
				{
					m_EventTime = *(time_t*)p->Data;
				}
				else
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_LIFETIME) == 0)
				{
					m_nLifetime = *(long*)p->Data;
				}
				else
				{
					if (!m_parEventBody) KLPAR_CreateParams(&m_parEventBody);

					CAutoPtr<Value> pVal;
					switch(pMaskItem->m_nValueType){
						case Value::BOOL_T:
							KLPAR::CreateValue(*(bool*)p->Data, (BoolValue**)&pVal);
						break;
						case Value::INT_T:
							KLPAR::CreateValue(*(int*)p->Data, (IntValue**)&pVal);
						break;
						case Value::LONG_T:
#if _WIN32
							KLPAR::CreateValue(*(__int64*)p->Data, (LongValue**)&pVal);
#else
							KLPAR::CreateValue(*(long long*)p->Data, (LongValue**)&pVal);
#endif
						break;
						case Value::DATE_TIME_T:
							KLPAR::CreateValue(*(time_t*)p->Data, (DateTimeValue**)&pVal);
						break;
						case Value::STRING_T:
						{
							#ifdef _WIN32
								int nSize = MultiByteToWideChar(CP_UTF8, 0, p->Data, -1, NULL, 0);
								if (nSize > 0)
								{
									CArrayPointer<wchar_t> psz((wchar_t*)malloc(nSize * sizeof(wchar_t)));
									MultiByteToWideChar(CP_UTF8, 0, p->Data, -1, psz, nSize);
									KLPAR::CreateValue((wchar_t*)psz, (StringValue**)&pVal);
								}
							#else
								KLSTD_USES_CONVERSION;
								KLPAR::CreateValue(KLSTD_A2W(p->Data), (StringValue**)&pVal);
							#endif
						}
						break;
					}

					if (pVal)
					{
						std::vector<std::wstring> vecPath;
						KLSTD_SplitString(pMaskItem->m_sValueName, KLPRES_EVENT_NAME_DIVIDER, vecPath);
						KLSTD_ASSERT_THROW(vecPath.size() > 0);

						std::vector<const wchar_t*> vecPath2;
						vecPath2.resize(vecPath.size() - 1, NULL);
						for(unsigned int jj = 1;jj < vecPath.size()-1;++jj) vecPath2[jj - 1] = vecPath[jj].c_str();                    

						KLPAR_ReplaceValue(m_parEventBody, &(*vecPath2.begin()), vecPath[vecPath.size()-1], pVal);
					}
				}
			}

			p = GetNextProperty(p);
		}
	}
	else
		KLSTD_THROW(STDE_BADPARAM);
}

//////////////////////////////////////////////////////////////////////
// CEventInfoAsText

std::wstring TimeToString(time_t tTime)
{
    struct tm gmtmbuffer;
	struct tm *pTm = KLSTD_gmtime(&tTime, &gmtmbuffer);
	wchar_t szBuffer[KLSTD_COUNTOF("CCYY-MM-DDTHH:MM:SSZGXXXX") + 16] = L"";

	if (pTm) wcsftime(szBuffer, KLSTD_COUNTOF(szBuffer), L"%Y-%m-%dT%H:%M:%SZ", pTm);

	return std::wstring(szBuffer);
}

time_t StringToTime(std::wstring sTime)
{
	struct tm T = {0};
	time_t t = -1;

	if (!sTime.empty())
	{
#ifndef N_PLAT_NLM
		swscanf(sTime.c_str(), L"%d-%d-%dT%d:%d:%d", &T.tm_year, &T.tm_mon, &T.tm_mday, &T.tm_hour, &T.tm_min, &T.tm_sec);
#else
        _conv _c;
        std::string tmp = _c.narrow(sTime);
		sscanf(tmp.c_str(), "%d-%d-%dT%d:%d:%d", &T.tm_year, &T.tm_mon, &T.tm_mday, &T.tm_hour, &T.tm_min, &T.tm_sec);
#endif
		if (T.tm_year <= 1901)
			t = INT_MIN;
		else 
		if (T.tm_year >= 2038)
			t = INT_MAX;
		else
		{
			T.tm_year -= 1900;
			T.tm_mon--;
			t = KLSTD_mkgmtime(&T);
		}
	}

	return t;
}

void CEventInfoAsText::Serialize(std::string & os) const
{
	std::wstring sResultString;

	wchar_t szBuffer[256];

	for(unsigned int i = 0;i < m_pSubscription->m_EventInfoMask.Count();i++)
	{
		CEventInfoMaskItem* pMaskItem = m_pSubscription->m_EventInfoMask[i];
		if (pMaskItem)
		{
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_PUBLISHER_CID) == 0)
			{
				sResultString += m_cidPublisher.PutToString().c_str();
				sResultString += m_pSubscription->m_cEventsFieldDelimiter;
			}
			else
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TYPE) == 0)
			{
				sResultString += m_sEventType.c_str();
				sResultString += m_pSubscription->m_cEventsFieldDelimiter;
			}
			else
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TIME) == 0)
			{
				sResultString += TimeToString(m_EventTime);
				sResultString += m_pSubscription->m_cEventsFieldDelimiter;
			}
			else
			if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_LIFETIME) == 0)
			{
				KLSTD_SWPRINTF(szBuffer, KLSTD_COUNTOF(szBuffer), L"%u", m_nLifetime);
                sResultString += szBuffer;
				sResultString += m_pSubscription->m_cEventsFieldDelimiter;
			}
			else
			{
                //<-- Changed by andkaz 29.04.2003 13:11:09
                CAutoPtr<Value> pValue;
                std::vector<std::wstring> vecPath;
                KLSTD_SplitString(pMaskItem->m_sValueName, KLPRES_EVENT_NAME_DIVIDER, vecPath);
                KLSTD_ASSERT_THROW(vecPath.size() > 0);

                std::vector<const wchar_t*> vecPath2;
                vecPath2.resize(vecPath.size() - 1, NULL);
                for(unsigned int jj = 1;jj < vecPath.size()-1;++jj) vecPath2[jj - 1] = vecPath[jj].c_str();                    
                
				KLERR_IGNORE(KLPAR_GetValue(m_parEventBody, &(*vecPath2.begin()), vecPath[vecPath.size()-1], &pValue));

				if (pValue)
				{
					switch(pValue->GetType())
					{
						case Value::BOOL_T:
							KLSTD_SWPRINTF(szBuffer, KLSTD_COUNTOF(szBuffer), L"%u", ((BoolValue*)(Value*)pValue)->GetValue());
						break;
						case Value::INT_T:
							KLSTD_SWPRINTF(szBuffer, KLSTD_COUNTOF(szBuffer), L"%u", ((IntValue*)(Value*)pValue)->GetValue());
						break;
						case Value::LONG_T:
							KLSTD_SWPRINTF(szBuffer, KLSTD_COUNTOF(szBuffer),
#ifdef _WIN32
                                    L"%I64u",
#endif
#ifdef __unix
                                    L"%llu",
#endif
#ifdef N_PLAT_NLM
                                    L"%llu",
#endif
										((LongValue*)(Value*)pValue)->GetValue());
						break;
						case Value::DATE_TIME_T:
							KLSTD_SWPRINTF(szBuffer, KLSTD_COUNTOF(szBuffer), L"%ls", TimeToString(((DateTimeValue*)(Value*)pValue)->GetValue()).data());
						break;
						case Value::STRING_T:
							sResultString += ((StringValue*)(Value*)pValue)->GetValue();
							szBuffer[0] = 0;
						break;
						default:
							szBuffer[0] = 0;
					}
					if (szBuffer[0] != 0) sResultString += szBuffer;
				}
				sResultString += m_pSubscription->m_cEventsFieldDelimiter;
			}
		}
	}

	std::string sConvertedResultString;

#ifdef _WIN32
	int nSize = WideCharToMultiByte(CP_UTF8, 0, sResultString.c_str(), -1, NULL, 0, NULL, NULL);
	if (nSize > 0)
	{
		CArrayPointer<char> psz((char*)malloc(nSize * sizeof(char)));
		WideCharToMultiByte(CP_UTF8, 0, sResultString.c_str(), -1, psz, nSize, NULL, NULL);
		sConvertedResultString = psz;
	}
#else
	KLSTD_USES_CONVERSION;
	sConvertedResultString = KLSTD_W2A(sResultString.c_str());
#endif

	os = sConvertedResultString;
}

void CEventInfoAsText::Deserialize(const std::string & is)
{
#define FIELD_DATA				std::wstring(pszFieldBegin, pszFieldEnd - pszFieldBegin)
#define FIELD_DATA_AS_NUMBER	wcstol(FIELD_DATA.c_str(), NULL, 10)

	if (m_parEventBody) m_parEventBody->Clear();

	std::wstring sSourceString;
#ifdef _WIN32
	int nSize = MultiByteToWideChar(CP_UTF8, 0, is.c_str(), -1, NULL, 0);
	if (nSize > 0)
	{
		CArrayPointer<wchar_t> psz((wchar_t*)malloc(nSize * sizeof(wchar_t)));
		MultiByteToWideChar(CP_UTF8, 0, is.c_str(), -1, psz, nSize);
		sSourceString = psz;
	}
#else
	KLSTD_USES_CONVERSION;
	sSourceString = KLSTD_A2W(is.c_str());
#endif

	const wchar_t* pszFieldBegin = sSourceString.c_str();
	wchar_t* pszFieldEnd;
	if (!pszFieldBegin) return;

	unsigned long n = 0;
	while(n < m_pSubscription->m_EventInfoMask.Count() &&
		 (pszFieldEnd = wcschr((wchar_t*)pszFieldBegin, m_pSubscription->m_cEventsFieldDelimiter)) != NULL)
	{
		if (pszFieldEnd - pszFieldBegin > 0)
		{
			CEventInfoMaskItem* pMaskItem = m_pSubscription->m_EventInfoMask[n];
			if (pMaskItem)
			{
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_PUBLISHER_CID) == 0)
				{
					m_cidPublisher.GetFromString(FIELD_DATA);
				}
				else
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TYPE) == 0)
				{
					m_sEventType = FIELD_DATA;
				}
				else
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_TIME) == 0)
				{
					m_EventTime = StringToTime(FIELD_DATA);
				}
				else
				if (wcscmp(pMaskItem->m_sValueName.c_str(), EVENT_CLASS_MASK_EVENT_LIFETIME) == 0)
				{
					m_nLifetime = FIELD_DATA_AS_NUMBER;
				}
				else
				{
					if (!m_parEventBody) KLPAR_CreateParams(&m_parEventBody);

					KLSTD::CAutoPtr<KLPAR::Value> pVal;

					switch(pMaskItem->m_nValueType){
						case Value::BOOL_T:
                            KLPAR::CreateValue(FIELD_DATA_AS_NUMBER ? true : false, (BoolValue**)&pVal);
						break;
						case Value::INT_T:
							KLPAR::CreateValue((int)FIELD_DATA_AS_NUMBER, (IntValue**)&pVal);
						break;
						case Value::LONG_T:
						{
#if _WIN32
							KLPAR::CreateValue(_wtoi64(FIELD_DATA.c_str()), (LongValue**)&pVal);
#else
                            std::wstringstream s;
                            s << FIELD_DATA.c_str();
                            long l;
                            s >> l;
							KLPAR::CreateValue(l, (LongValue**)&pVal);
#endif
						}
						break;
						case Value::DATE_TIME_T:
							KLPAR::CreateValue(StringToTime(FIELD_DATA), (DateTimeValue**)&pVal);
						break;
						case Value::STRING_T:
							KLPAR::CreateValue(FIELD_DATA.c_str(), (StringValue**)&pVal);
						break;
					}

					if (pVal)
					{
						std::vector<std::wstring> vecPath;
						KLSTD_SplitString(pMaskItem->m_sValueName, KLPRES_EVENT_NAME_DIVIDER, vecPath);
						KLSTD_ASSERT_THROW(vecPath.size() > 0);

						std::vector<const wchar_t*> vecPath2;
						vecPath2.resize(vecPath.size() - 1, NULL);
						for(unsigned int jj = 1;jj < vecPath.size()-1;++jj) vecPath2[jj - 1] = vecPath[jj].c_str();                    

						KLPAR_ReplaceValue(m_parEventBody, &(*vecPath2.begin()), vecPath[vecPath.size()-1], pVal);
					}
				}
			}
		}
		n++;
		pszFieldBegin = pszFieldEnd + 1;
	}
}

//////////////////////////////////////////////////////////////////////
// CSubscription

// CPublishAddEventEventWorker

const long CHECK_EVENTS_PERIOD = 5000;

CSubscription::CPublishAddEventEventWorker::CPublishAddEventEventWorker(volatile bool& bEventsAdded, KLSTD::CriticalSection* pCS, const CSubscription* pSubscription):
	m_bEventsAdded(bEventsAdded),
	m_pCS(pCS),
	m_pSubscription(pSubscription)
{
	KLSTD_ASSERT(m_pSubscription);
	KLSTD_ASSERT(m_pCS);
}

CSubscription::CPublishAddEventEventWorker::~CPublishAddEventEventWorker()
{
	RunWorker(0);	// Force to send EVENT_ADDED event on subscription deinitialization
}

int CSubscription::CPublishAddEventEventWorker::RunWorker(KLTP::ThreadsPool::WorkerId WID)
{
	{
		KLSTD::AutoCriticalSection cs(m_pCS);
		if (!m_bEventsAdded) return 0;
		m_bEventsAdded = false;
	}

	if (m_pSubscription) m_pSubscription->_PublishEvent(PRES_EVENT_ADDED);

	return 0;
}

// Creates object that represents existing persistent subscription in storage file
CSubscription::CSubscription(KLEVQ::HSUBSQUEUE hQueue)
    :   m_hQueue(hQueue)
{
	m_bInitialized = false;
	m_bPaused = false;
	m_bUseRecNumIndex = false;
	m_WIDPublishAddEventEventWorker = -1;
	m_bEventsAdded = false;
	m_bKeepEventsWhenSubscriptionIsDeleted = false;
	m_nMaxEvents = 10;
	m_nMaxPageSize = JRNL_PAGE_SIZE_INFINITE;

	KLSTD_CreateCriticalSection(&m_pCS);

	KLTP::ThreadsPool* pGlobalTreadsPool = KLTP_GetThreadsPool();
	KLSTD_ASSERT(pGlobalTreadsPool);
	if (pGlobalTreadsPool)
	{
		KLSTD::CAutoPtr<CPublishAddEventEventWorker> pWorker;
		pWorker.Attach(new CPublishAddEventEventWorker(m_bEventsAdded, m_pCS, this));
		if (pWorker) pGlobalTreadsPool->AddWorker(&m_WIDPublishAddEventEventWorker, KLSTD_CreateGUIDString().c_str(), pWorker, KLTP::ThreadsPool::RunNormal, true, CHECK_EVENTS_PERIOD);
	}
}

CSubscription::~CSubscription()
{
	KLERR_BEGIN
	{
		if( m_bInitialized )
		{
			KLSTD_TRACE1(4, L"[KLPRES] Removing CSubscription '%ls'\n", m_sID.c_str());
			_UnsubscribeAll();
			m_pEvents->Deinitialize();
		}
	}
	KLERR_ENDT(1);

	KLERR_BEGIN
	{
		if (m_WIDPublishAddEventEventWorker != -1)
		{
			KLTP::ThreadsPool* pGlobalTreadsPool = KLTP_GetThreadsPool();
			KLSTD_ASSERT(pGlobalTreadsPool);
			if (pGlobalTreadsPool) pGlobalTreadsPool->DeleteWorker(m_WIDPublishAddEventEventWorker);
		}
	}
	KLERR_ENDT(1);
}

void CSubscription::InitializeExistingInStorage(
	const KLPRCI::ComponentId&	idAgent,
	CSubscriptionList*	pMaster, 
	const std::wstring			sID, 
	const std::wstring			sEventStoragesFolder,
	const Params*				parSubscription )
{
	KLSTD_CHKINPTR(pMaster);

	m_sID = sID;
	m_pMaster = pMaster;

    GET_PARAMS_VALUE( parSubscription, PRES_SUBSCRIPTION_VERSION_ID, StringValue, STRING_T, m_wstrVersionId );
	_Initialize(idAgent, sEventStoragesFolder, parSubscription);
}


// Creates object that represents new subscription
void CSubscription::InitializeNew(
	const KLPRCI::ComponentId& idAgent,
	CSubscriptionList* pMaster, 
	const std::wstring sID, 
	const bool bPersistent, 
	Params* parSubscriptionInfo, 
	const std::wstring sEventStoragesFolder)
{
	KLSTD_CHKINPTR(pMaster);

	m_sID = sID;
	m_pMaster = pMaster;
    m_wstrVersionId = KLSTD_CreateGUIDString();

    _Initialize(idAgent, sEventStoragesFolder, parSubscriptionInfo, true);

	if (bPersistent)	// Create persistent in file
    {
        string strData;
        REPLACE_PARAMS_VALUE( parSubscriptionInfo, PRES_SUBSCRIPTION_VERSION_ID, StringValue, m_wstrVersionId.c_str() );
        SerializeToString( m_sID, parSubscriptionInfo, strData );

        CAutoPtr<KLJRNL::Journal> pJournal = 
            m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), m_bPaused );
        long nId = pJournal->Add( strData );
        pJournal->Flush();
        pJournal->Close();

        _PublishEvent( PRES_EVENT_SUBSCRIPTION_ADDED );

        // link to IDs: nId and m_sID
        
		/* CAutoPtr<ParamsValue> pValue;
		CreateValue((Params*)parSubscription, &pValue);
		CAutoPtr<KLPAR::Params> parStorageParams;
		KLPAR_CreateParams(&parStorageParams);
		parStorageParams->AddValue(m_sID, pValue); */
		//!!! m_pMaster->m_pStorage->Replace(SS_SUBSCRIPTIONS_NAME, SS_SUBSCRIPTIONS_VERSION, SS_SUBSCRIPTIONS_SECTION_NAME, parStorageParams);

    } else				 // Create in memory
		m_parSubscriptionInfo = parSubscriptionInfo;
}

void CSubscription::Reinit(const KLPAR::Params* parSubscription)
{
    _Initialize(m_cidEventsStorageComponentId, L"", parSubscription);
}

void CSubscription::Update(KLPAR::Params* parSubscription)
{
	KLSTD_CHKINPTR(parSubscription);

    KLSTD_TRACE1(4, L"[KLPRES] Subscription '%ls' (persistent) is updating now...\n", m_sID.c_str());

    _Initialize(m_cidEventsStorageComponentId, L"", parSubscription);
	
	KLSTD_ASSERT(m_pEvents);
	m_pEvents->Update();

    if( m_bPersistent )
    {
        CAutoPtr<Journal> pJournal = m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), false );
        CAutoPtr<Journal> pJournalWithPausedSubscr = m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), true );

        bool bFound = false;
        string strSerializedSubscrParams;
        MoveJrnlPosToSubscription( pJournal, m_sID, strSerializedSubscrParams );
        if( ! pJournal->IsEOF() )
        {
            bFound = true;
            KLSTD_TRACE1( 1, L"[KLPRES] [CSubscription::Update] Update of '%ls'. Now deleting old data found in active storage...\n", m_sID.c_str() );
            pJournal->DeleteCurrent();
        }

        if( ! bFound )
        {
            MoveJrnlPosToSubscription( pJournalWithPausedSubscr, m_sID, strSerializedSubscrParams );
            if( ! pJournalWithPausedSubscr->IsEOF() )
            {
                bFound = true;
                KLSTD_TRACE1( 1, L"[KLPRES] [CSubscription::Update] Update of '%ls'. Now deleting old data found in paused storage...\n", m_sID.c_str() );
                pJournalWithPausedSubscr->DeleteCurrent();
            }
        }

        if( bFound )
        {
            m_wstrVersionId = KLSTD_CreateGUIDString();

            string strData;
            REPLACE_PARAMS_VALUE( parSubscription, PRES_SUBSCRIPTION_VERSION_ID, StringValue, m_wstrVersionId.c_str() );
            SerializeToString( m_sID, parSubscription, strData );
            if( m_bPaused )
            {
                KLSTD_TRACE1( 1, L"[KLPRES] Update of '%ls'. Now add new data to paused storage...\n", m_sID.c_str() );
                pJournalWithPausedSubscr->Add( strData );
            } else
            {
                KLSTD_TRACE1( 1, L"[KLPRES] Update of '%ls'. Now add new data to active storage...\n", m_sID.c_str() );
                pJournal->Add( strData );
            }

			_PublishEvent( PRES_EVENT_SUBSCRIPTION_UPDATED );
        }
    }
}

void CSubscription::_PublishEvent(const std::wstring sEventType) const
{
	CAutoPtr<Params> parEventBody;
	KLPAR_CreateParams(&parEventBody);
	ADD_PARAMS_VALUE(parEventBody, PRES_EVENT_BODY_PARAM_SUBSCR_NAME, StringValue, (wchar_t*)m_sID.c_str());

	CAutoPtr<KLEV::Event> pEvent;
	KLEV_CreateEvent(&pEvent, m_cidEventsStorageComponentId, sEventType, parEventBody, time(0));
	KLSTD_CHKMEM(pEvent);

	KLSTD_CHKMEM(KLEV_GetEventSource());
	KLEV_GetEventSource()->PublishEvent(pEvent);
}

void CSubscription::Get(const Params* parStorageParams, Params** pparSubscriptionInfo)
{
	KLSTD_CHKINPTR(parStorageParams);
	KLSTD_CHKOUTPTR(pparSubscriptionInfo);

	if (!IsPersistent())	// Non persistent
	{
		*pparSubscriptionInfo = m_parSubscriptionInfo;
		(*pparSubscriptionInfo)->AddRef();
	}
	else						// Persistent: from file
	{
		CAutoPtr<ParamsValue> val =	(ParamsValue*)parStorageParams->GetValue2(m_sID, true);
		*pparSubscriptionInfo = val->GetValue();
		(*pparSubscriptionInfo)->AddRef();
	}
}

void CSubscription::PauseResume(const bool bPause)
{
	if (m_bPaused == bPause) return;

    string strSerializedSubscrParams;
    CAutoPtr<Journal> pJournal =
        m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), false );

    CAutoPtr<Journal> pJournalWithPausedSubscr =
        m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), true );

    bool bFound = false;

    MoveJrnlPosToSubscription( pJournal, m_sID, strSerializedSubscrParams );
    if( ! pJournal->IsEOF() )
    {
        bFound = true;
		KLSTD_TRACE1( 1, L"[KLPRES] [CSubscription::PauseResume] Deleting subscription %ls from active storage...\n", m_sID.c_str() );
        pJournal->DeleteCurrent();
		KLSTD_TRACE0( 1, L"[KLPRES] [CSubscription::PauseResume] Deletion successfull\n" );
    }

    if( ! bFound )
    {
        MoveJrnlPosToSubscription( pJournalWithPausedSubscr, m_sID, strSerializedSubscrParams );
        if( ! pJournalWithPausedSubscr->IsEOF() )
        {
            bFound = true;
			KLSTD_TRACE1( 1, L"[KLPRES] Deleting subscription %ls from paused storage...\n", m_sID.c_str() );
            pJournalWithPausedSubscr->DeleteCurrent();
			KLSTD_TRACE0( 1, L"[KLPRES] Deletion successfull\n" );
        }
    }

    if( bFound )
    {
        CAutoPtr<Params> parSubscription;
        KLPAR_DeserializeFromMemory( strSerializedSubscrParams.c_str(), strSerializedSubscrParams.size(), & parSubscription );

	    CPointer<SubscriptionInfo> pSubscriptionInfo;
	    Deserialize(parSubscription, &pSubscriptionInfo);

	    m_bPaused = bPause;
        pSubscriptionInfo->bPaused = m_bPaused;

        parSubscription = NULL;
	    Serialize(pSubscriptionInfo, &parSubscription);

        m_wstrVersionId = KLSTD_CreateGUIDString();

        string strData;
        REPLACE_PARAMS_VALUE( parSubscription, PRES_SUBSCRIPTION_VERSION_ID, StringValue, m_wstrVersionId.c_str() );
        SerializeToString( m_sID, parSubscription, strData );

        if( m_bPaused )
        {
            pJournalWithPausedSubscr->Add( strData );
        } else
        {
            pJournal->Add( strData );
        }

        pJournal->Close();
        pJournalWithPausedSubscr->Close();

    	_Resubscribe(pSubscriptionInfo);
    }
}

void CSubscription::Remove()
{
	_UnsubscribeAll();

	if (IsPersistent())	// Persistent: from file
	{
        string strSerializedSubscrParams;
        CAutoPtr<Journal> pJournal =
            m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), false );

        CAutoPtr<Journal> pJournalWithPausedSubscr =
            m_pMaster->_GetCentralJournal( ACCESS_FLAGS( AF_READ | AF_WRITE ), true );

        bool bFound = false;

        MoveJrnlPosToSubscription( pJournal, m_sID, strSerializedSubscrParams );
        if( ! pJournal->IsEOF() )
        {
            bFound = true;
			KLSTD_TRACE1( 1, L"[KLPRES] [CSubscription::Remove] Deleting subscription %ls from active storage...\n", m_sID.c_str() );
			pJournal->DeleteCurrent();
			KLSTD_TRACE0( 1, L"[KLPRES] [CSubscription::Remove] Deletion successfull\n" );
        }

        if( ! bFound )
        {
            MoveJrnlPosToSubscription( pJournalWithPausedSubscr, m_sID, strSerializedSubscrParams );
            if( ! pJournalWithPausedSubscr->IsEOF() )
            {
                bFound = true;
				KLSTD_TRACE1( 1, L"[KLPRES] [CSubscription::Remove] Deleting subscription %ls from paused storage...\n", m_sID.c_str() );
                pJournalWithPausedSubscr->DeleteCurrent();
				KLSTD_TRACE0( 1, L"[KLPRES] [CSubscription::Remove] Deletion successfull\n" );
            }
        }

        KLSTD_TRACE1(4, L"[KLPRES] Subscription '%ls' (persistent) unsubscribes from (Removing from storage)\n", m_sID.c_str());

        _PublishEvent( PRES_EVENT_SUBSCRIPTION_DELETED );

		/* CAutoPtr<ParamsValue> val;
		CAutoPtr<Params> parStorageParams;

		
		KLPAR_CreateParams(&parStorageParams);
		CreateValue(NULL, &val);
		parStorageParams->AddValue(m_sID, val); */
		//!!!m_pMaster->m_pStorage->Delete(SS_SUBSCRIPTIONS_NAME, SS_SUBSCRIPTIONS_VERSION, SS_SUBSCRIPTIONS_SECTION_NAME, parStorageParams);
	}

	// Deletes events storage
	KLERR_BEGIN
		KLSTD_TRACE1( 1, L"Trying to remove events storage for subscription %ls...\n", m_sID.c_str() );
		m_pEvents->RemoveStorage(m_bKeepEventsWhenSubscriptionIsDeleted);
	KLERR_ENDT(1)
}

bool CSubscription::IsPersistent()
{
	return m_bPersistent;
}

void CSubscription::Serialize(const SubscriptionInfo* pSubscriptionInfo, Params** pparSubscriptionInfo)
{
	KLSTD_CHKINPTR(pSubscriptionInfo);
	KLSTD_CHKOUTPTR(pparSubscriptionInfo);

	CAutoPtr<Params> pParams;
	KLPAR_CreateParams(&pParams);

	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_BODY_FILTER, ParamsValue, pSubscriptionInfo->bodyFilter);
	ADD_CID_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_FILTER, pSubscriptionInfo->filter);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_PERSIST, BoolValue, pSubscriptionInfo->isSubscriptionPersist);
	ADD_ARRAY_STR_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENT_TYPE, pSubscriptionInfo->eventTypes);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_MAX_EVENTS, IntValue, pSubscriptionInfo->maxEvents);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_PAGE_SIZE, IntValue, pSubscriptionInfo->maxPageSize);
	ADD_CID_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_SUBSCRIBER, pSubscriptionInfo->subscriber);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK, ParamsValue, pSubscriptionInfo->parEventParametersMask);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_CLOSE_EVENT_STORE_TIMEOUT, IntValue, pSubscriptionInfo->nCloseWriteOpenedJournalTimeout);
    ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_PAUSED, IntValue, pSubscriptionInfo->bPaused ? long(1) : long(0) );
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_STORE_EVENTS_AS, IntValue, pSubscriptionInfo->nStoreEventsAs);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_STORE_EVENTS_AS_TEXT_DELIMITER, IntValue, pSubscriptionInfo->cFieldDelimiter);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENTS_FILE_NAME, StringValue, pSubscriptionInfo->sEventsListFileName.c_str());
//	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_USE_REC_NUM_INDEX, BoolValue, pSubscriptionInfo->bUseRecNumIndex);
	ADD_ARRAY_STR_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENTS_PARAMS_SORT_MASK, pSubscriptionInfo->arEventParametersSortMask);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_KEEP_EVENTS, BoolValue, pSubscriptionInfo->bKeepEventsWhenSubscriptionIsDeleted);

	*pparSubscriptionInfo = pParams.Detach();
}

void CSubscription::Deserialize(const Params* parSubscriptionInfo, SubscriptionInfo** ppSubscriptionInfo)
{
	KLSTD_CHKINPTR(parSubscriptionInfo);
	KLSTD_CHKOUTPTR(ppSubscriptionInfo);

	CPointer<SubscriptionInfo> pSubscriptionInfo = new SubscriptionInfo;
    KLSTD_CHKMEM( pSubscriptionInfo );

	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_BODY_FILTER, ParamsValue, PARAMS_T, pSubscriptionInfo->bodyFilter);
	GET_CID_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_FILTER, pSubscriptionInfo->filter);
	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_PERSIST, BoolValue, BOOL_T, pSubscriptionInfo->isSubscriptionPersist);
	GET_ARRAY_PARAMS_VALUE_NO_THROW(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_EVENT_TYPE, StringValue, pSubscriptionInfo->eventTypes);

	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_MAX_EVENTS, IntValue, INT_T, pSubscriptionInfo->maxEvents);
	pSubscriptionInfo->maxPageSize = JRNL_PAGE_SIZE_INFINITE;
	GET_PARAMS_VALUE_NO_THROW(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_PAGE_SIZE, IntValue, INT_T, pSubscriptionInfo->maxPageSize);

	GET_CID_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_SUBSCRIBER, pSubscriptionInfo->subscriber);
	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK, ParamsValue, PARAMS_T, pSubscriptionInfo->parEventParametersMask);
	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_CLOSE_EVENT_STORE_TIMEOUT, IntValue, INT_T, pSubscriptionInfo->nCloseWriteOpenedJournalTimeout);

    {
        int nTemp;
	    GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_PAUSED, IntValue, INT_T, nTemp);
        pSubscriptionInfo->bPaused = nTemp != 0;
    }

	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_STORE_EVENTS_AS, IntValue, INT_T, pSubscriptionInfo->nStoreEventsAs);
	long delimer = 0;
	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_STORE_EVENTS_AS_TEXT_DELIMITER, IntValue, INT_T, delimer );
	pSubscriptionInfo->cFieldDelimiter = (wchar_t)delimer;
	GET_PARAMS_VALUE(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_EVENTS_FILE_NAME, StringValue, STRING_T, pSubscriptionInfo->sEventsListFileName);
	GET_ARRAY_PARAMS_VALUE_NO_THROW(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_EVENTS_PARAMS_SORT_MASK, StringValue, pSubscriptionInfo->arEventParametersSortMask);
	GET_PARAMS_VALUE_NO_THROW(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_USE_REC_NUM_INDEX, BoolValue, BOOL_T, pSubscriptionInfo->bUseRecNumIndex);
	GET_PARAMS_VALUE_NO_THROW(parSubscriptionInfo, SS_VALUE_NAME_SUBSCRIPTION_KEEP_EVENTS, BoolValue, BOOL_T, pSubscriptionInfo->bKeepEventsWhenSubscriptionIsDeleted);

	*ppSubscriptionInfo = pSubscriptionInfo.Relinquish();
}

void CSubscription::AddEvent(const KLPRCI::ComponentId& publisher, const KLEV::Event *event)
{
	KLERR_TRY
	{
		KLSTD_CHKINPTR(event);

		CAutoPtr<CEventInfo> pEventInfo;
		m_pEvents->GetEventInfoClassInstance(&pEventInfo);

		pEventInfo->m_cidPublisher = publisher;
		pEventInfo->m_sEventType = event->GetType();
		pEventInfo->m_parEventBody = event->GetParams();
		pEventInfo->m_EventTime = event->GetBirthTime();
		pEventInfo->m_pSubscription = this;
		pEventInfo->m_nLifetime = event->GetLifeTime();
		pEventInfo->m_llEventIdAcrossInstance = m_pMaster->IncrementEventCounter();
		pEventInfo->m_wstrInstanceId = m_pMaster->m_wstrInstanceId;

		m_pEvents->AddEvent(pEventInfo);

		KLSTD::AutoCriticalSection cs(m_pCS);
		m_bEventsAdded = true;
	}
	KLERR_CATCH(pError)
	{
        KLERR_SAY_FAILURE( 1, pError );
		_PublishEvent(PRES_ERR_ADD_EVENT);
	}
	KLERR_ENDTRY;
}

std::wstring CSubscription::GetStorageFilePath()
{
	return m_sEventsListFilePath;
}

//////////////////////////////////////////////////////////////////////

void CSubscription::_Initialize(const KLPRCI::ComponentId& idAgent, const std::wstring sEventStoragesFolder, const Params* parSubscription, bool bNewSubscription)
{
	CPointer<SubscriptionInfo> pSubscriptionInfo;
	Deserialize(parSubscription, &pSubscriptionInfo);

	m_bPaused = pSubscriptionInfo->bPaused;
    m_bPersistent = pSubscriptionInfo->isSubscriptionPersist;
	m_nMaxEvents = pSubscriptionInfo->maxEvents;
	m_nMaxPageSize = pSubscriptionInfo->maxPageSize;

	if (!m_bInitialized)
	{
		KLSTD_TRACE1(1, L"[KLPRES] Subscription added: %ls\n", m_sID.c_str());
		m_cidEventsStorageComponentId = idAgent;

		// Cash frequently required data in memory
		m_bKeepEventsWhenSubscriptionIsDeleted = pSubscriptionInfo->bKeepEventsWhenSubscriptionIsDeleted;
		m_nCloseWriteOpenedJournalTimeout = pSubscriptionInfo->nCloseWriteOpenedJournalTimeout;
		m_cEventsFieldDelimiter = pSubscriptionInfo->cFieldDelimiter;
		if (pSubscriptionInfo->sEventsListFileName.empty())
			m_sEventsListFilePath = sEventStoragesFolder + SLASH + m_sID + STORAGE_FILE_EXTENTION;
		else
			m_sEventsListFilePath = pSubscriptionInfo->sEventsListFileName;
		m_arEventParametersSortMask = pSubscriptionInfo->arEventParametersSortMask;
		m_bUseRecNumIndex = pSubscriptionInfo->bUseRecNumIndex;

		_ParseMask(pSubscriptionInfo->parEventParametersMask);
		if (m_EventInfoMask.Count() != 0)
		{
			switch (pSubscriptionInfo->nStoreEventsAs) {
				case SubscriptionInfo::SI_STORE_AS_BINARY:
					m_pEvents.Attach( new KLBaseImpl<CPersistentEventListAsPolicy> );
                    KLSTD_CHKMEM( m_pEvents );
				break;
				case SubscriptionInfo::SI_STORE_AS_TEXT:
					m_pEvents.Attach( new KLBaseImpl<CPersistentEventListAsText> );
                    KLSTD_CHKMEM( m_pEvents );
				break;
				default:
					KLSTD_THROW(STDE_NOTFOUND);
			}
		}
		else
        {
			m_pEvents.Attach( new KLBaseImpl<CPersistentEventList> );
            KLSTD_CHKMEM( m_pEvents );
        }
		
		m_pEvents->Initialize(this, bNewSubscription);

		m_bInitialized = true;
	}
	else
	{
		KLSTD_TRACE1(1, L"[KLPRES] Subscription updated: %ls\n", m_sID.c_str());
	}

	_Resubscribe(pSubscriptionInfo);
}

void CSubscription::_Resubscribe(KLPRES::SubscriptionInfo* pSubscriptionInfo)
{
	_UnsubscribeAll();

    if( m_bPaused || ! m_pMaster->m_bSubscribeToEventsToStore )
    {
        return;
    }

	KLEV::SubscriptionOptions so;
	so.local = true;
	if (pSubscriptionInfo->eventTypes.size() > 0)
	{
		for(std::vector<std::wstring>::iterator i = pSubscriptionInfo->eventTypes.begin();i != pSubscriptionInfo->eventTypes.end();i++)
		{
			m_arEventSourceSubscriptionSIDs.push_back(
                KLEVQ::GetEventSourceQueued()->Subscribe(
                                                 m_hQueue,
                                                 m_cidEventsStorageComponentId,
												 pSubscriptionInfo->filter, 
												 *i, 
												 pSubscriptionInfo->bodyFilter,
												 SubscriptionsStorageEventCallback,
												 (void*)this,
												 so)            
			);
			KLSTD_TRACE2(4, L"[KLPRES] Subscription '%ls' subscribes on events of type '%ls'\n", m_sID.c_str(), (*i).c_str());
		}
	}
	else
	{
		m_arEventSourceSubscriptionSIDs.push_back(
			KLEVQ::GetEventSourceQueued()->Subscribe(
                                             m_hQueue,
                                             m_cidEventsStorageComponentId,
											 pSubscriptionInfo->filter, 
											 L"", 
											 pSubscriptionInfo->bodyFilter,
											 SubscriptionsStorageEventCallback,
											 (void*)this,
											 so)
		);

		KLSTD_TRACE1(4, L"[KLPRES] Subscription '%ls' subscribes on events of all types\n", m_sID.c_str());
	}
}

void CSubscription::_ParseMask(const KLPAR::Params* parEventParametersMask)
{
	m_EventInfoMask.ParseMask(parEventParametersMask, m_arEventParametersSortMask);
}

void CSubscription::_UnsubscribeAll()
{
	for(std::vector<KLEVQ::HSUBSCRIPTION>::iterator i = m_arEventSourceSubscriptionSIDs.begin();i != m_arEventSourceSubscriptionSIDs.end();i++)
	{
		KLSTD_TRACE2(4, L"[KLPRES] Subscription '%ls' unsubscribes from EvSrcSubscrID '%X'\n", m_sID.c_str(), *i);
		KLEVQ::GetEventSourceQueued()->Unsubscribe(m_hQueue, *i);
	}

	m_arEventSourceSubscriptionSIDs.clear();
}

/* inline int GetSizeOfStoredComponentId( const KLPRCI::ComponentId & compId )
{
    return 
        sizeof(wchar_t) * compId.productName.size() + 1 + 
        sizeof(wchar_t) * compId.version.size() + 1 + 
        sizeof(wchar_t) * compId.componentName.size() + 1 + 
        sizeof(wchar_t) * compId.instanceId.size() + 1;
}

inline int GetSizeOfStringArr( const vector<wstring> & vect )
{
    int nSize = 0;
    for( int i = 0; i < vect.size(); i++ )
    {
        if( ! vect[i].empty() )
        {
            nSize += vect[i].size() + 1;
        }
    }
    return nSize + 1;
}

inline void AddComponentIdToString( string & strData, const KLPRCI::ComponentId & compId )
{
    strData += string( (const char*)compId.productName.c_str(), compId.productName.size() * sizeof(wchar_t ) );
    strData += '\x0';
    strData += compId.version;
    strData += '\x0';
    strData += compId.componentName;
    strData += '\x0';
    strData += compId.instanceId;
    strData += '\x0';
}

        CPointer<SubscriptionInfo> pSubscriptionInfo;
        Deserialize( parSubscription, & pSubscriptionInfo );

        CAutoPtr<MemoryChunk> pMCBodyFilter;
        KLPAR_SerializeToMemory( pSubscriptionInfo->bodyFilter, & pMCBodyFilter );

        CAutoPtr<MemoryChunk> pMCEventParametersMask;
        KLPAR_SerializeToMemory( pSubscriptionInfo->parEventParametersMask, & pMCEventParametersMask );

        int nSize = GetSizeOfStoredComponentId( pSubscriptionInfo->filter );
        nSize += GetSizeOfStringArr( pSubscriptionInfo->eventTypes );
        nSize += sizeof( AVP_int32 ) + pMCBodyFilter->GetDataSize();
        nSize += GetSizeOfStoredComponentId( pSubscriptionInfo->subscriber );
        nSize += 1; // bool isSubscriptionPersist
        nSize += sizeof( AVP_int32 ); // int maxEvents
        nSize += sizeof( AVP_int32 ) + pMCEventParametersMask->GetDataSize();
        nSize += GetSizeOfStringArr( pSubscriptionInfo->arEventParametersSortMask );
        nSize += sizeof( AVP_int32 ); // int nCloseWriteOpenedJournalTimeout
        nSize += sizeof( AVP_int32 ); // bool bPaused
        nSize += sizeof( AVP_int32 ); // int nStoreEventsAs
        nSize += sizeof( wchar_t ); // considered sizeof(wchar_t) is the constant across all platforms
        nSize += pSubscriptionInfo->sEventsListFileName.size() + 1;

        string strSubscription;
        strSubscription.reserve( nSize );
        AddComponentIdToString( strSubscription, pSubscriptionInfo->filter );
*/

void CSubscription::SerializeToString(
    const std::wstring & wstrID,
    const KLPAR::Params* parSubscription,
    std::string & strData )
{
    // int nPaused;
    //GET_PARAMS_VALUE(parSubscription, SS_VALUE_NAME_SUBSCRIPTION_PAUSED, IntValue, INT_T, nPaused);

    strData = "";
    {
        AVP_int32 nSize32 = MachineToLittleEndian( wstrID.size() );
        strData += string( (const char*) & nSize32, sizeof( AVP_int32 ) );
        strData += string( (const char*) wstrID.c_str(), sizeof(wstrID[0])*wstrID.size() );
    }
    // strData += nPaused ? PAUSED_FLAG_CHAR : RUNNING_FLAG_CHAR;
    
    CAutoPtr<KLSTD::MemoryChunk> pChunk;
    KLPAR_SerializeToMemory( parSubscription, & pChunk );
    strData += string( (const char *)pChunk->GetDataPtr(),  pChunk->GetDataSize() );
}

//////////////////////////////////////////////////////////////////////

void CSubscription::SubscriptionsStorageEventCallback(const KLPRCI::ComponentId& subscriber,
													  const KLPRCI::ComponentId& publisher,
													  const KLEV::Event *event,
													  void *context)
{
	KLSTD_CHKINPTR(context);

	KLSTD_TRACE1(4, L"[KLPRES] Subscription '%ls': new event callback called\n", ((CSubscription*)context)->m_sID.c_str());
	((CSubscription*)context)->AddEvent(publisher, event);
}

//////////////////////////////////////////////////////////////////////
// CSubscriptionList

#define MAKE_SURE_THAT_INITIALIZED	\
	if (!m_bInitialized) KLSTD_NOINIT(L"KLPRES::CEventList"); \
	AutoCriticalSection acs(m_pcsLock);


//////////////////////////////////////////////////////////////////////

CSubscriptionList::CSubscriptionList()
    :   m_hQueue(NULL)
{
	m_bInitialized = false;
	::KLSTD_CreateCriticalSection(&m_pcsLock);
    ::KLSTD_CreateCriticalSection(&m_pcsLostCounterpartList);
    KLTMSG_CreateTimeoutStore( & m_pTSSubscrIterators );

    m_wstrInstanceId = KLSTD_CreateGUIDString();
    m_llEventCounter = 0;
    ::KLSTD_CreateCriticalSection(&m_pcsEventCounter);
    m_hQueue = KLEVQ::GetEventSourceQueued()->CreateQueue();
}

CSubscriptionList::~CSubscriptionList()
{
	_Unsubscribe();
    KLEVQ::GetEventSourceQueued()->DestroyQueue(m_hQueue);
}

AVP_longlong CSubscriptionList::IncrementEventCounter()
{
    AVP_longlong llTemp;

    {
        AutoCriticalSection acs( m_pcsEventCounter );
        llTemp = ++m_llEventCounter;
    }

    return llTemp;
}

KLSTD::CAutoPtr<KLJRNL::Journal> CSubscriptionList::_GetCentralJournal(
    KLSTD::ACCESS_FLAGS accessFlags, 
    bool bPausedSubscriptions )
{
    KLSTD::CAutoPtr<KLJRNL::Journal> pJournal;

    KLJRNL::CreationInfo creationInfo;

    creationInfo.bLimitPages = false;
    creationInfo.pageLimitType = KLJRNL::pltRecCount;
    creationInfo.nMaxRecordsInPage = 10;
    creationInfo.bBinaryFormat = true;

    wstring wstrJournalPath;
    if( ! bPausedSubscriptions )
    {
        wstrJournalPath = m_sStorageFilePath;
    } else
    {
        wstrJournalPath = m_sPausedStorageFilePath;
    }

    KLJRNL_CreateJournal( & pJournal, true );
   KL_TMEASURE_BEGIN(L"Journal::Open", 4)
    KLERR_TRY
        KLSTD_TRACE1(4, L"Journal::Open(%ls)\n", wstrJournalPath.c_str());
        pJournal->Open( wstrJournalPath, CF_OPEN_ALWAYS, accessFlags, creationInfo );
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE( 1, pError );
        if( pError->GetId() == KLJRNL::ERR_CORRUPTED )
        {
            KLSTD_TRACE1( 1, L"Journal \"%s\" is corrupted, trying to repair and then reopen...", wstrJournalPath.c_str() )
			_ReleaseAllIterators();
            pJournal->Repair( wstrJournalPath, creationInfo );
            pJournal->Open( wstrJournalPath, CF_OPEN_ALWAYS, accessFlags, creationInfo );
        } else
        {
            KLERR_RETHROW();
        }
    KLERR_ENDTRY
    KL_TMEASURE_END()

    return pJournal;
}

void CSubscriptionList::ReadSubscriptionsFromCentralJournal( bool bPausedSubscriptions, bool bRefresh )
{
	KLSTD_TRACE3( 1, L"Reading subscriptions for %ls. Read parameters: paused=%d, refresh=%d\n", m_sStorageFilePath.c_str(), int(bPausedSubscriptions), int(bRefresh) )

    CAutoPtr<Journal> pJournalForRead = _GetCentralJournal( AF_READ, bPausedSubscriptions );

    std::set<wstring> setRefreshedSubscrId;

    string strSerializedSubscrParams;
    pJournalForRead->ResetIterator();
    while( ! pJournalForRead->IsEOF() )
    {
        long nId;
        string strData;
        pJournalForRead->GetCurrent( nId, strData );

        wstring wstrSubscrId;
        // bool bPaused;
        CAutoPtr<Params> parSubscription;

        if( GetRawDataFromStrData( strData, wstrSubscrId, strSerializedSubscrParams ) )
        {
            MapSubscriptions::iterator itSubscr = m_mapSubscriptions.find(wstrSubscrId);
            if( ( itSubscr == m_mapSubscriptions.end() ) || bRefresh )
            {
                CAutoPtr<Params> parSubscription;
                bool bDeserializeOk = false;

                KLERR_BEGIN
                    KLPAR_DeserializeFromMemory( strSerializedSubscrParams.c_str(), strSerializedSubscrParams.size(), & parSubscription );
                    bDeserializeOk = true;
                KLERR_END;
                if( bDeserializeOk )
                {
                    bool bAddSubscr = false;
                    if( itSubscr == m_mapSubscriptions.end() )
                    {
                        bAddSubscr = true;
                    } else if( bRefresh )
                    {
                        wstring wstrVersionId;
                        GET_PARAMS_VALUE( parSubscription, PRES_SUBSCRIPTION_VERSION_ID, StringValue, STRING_T, wstrVersionId );
                        if( itSubscr->second.m_T->m_wstrVersionId != wstrVersionId )
                        {
                            RemoveSubscription(wstrSubscrId, false);                            
                            bAddSubscr = true;
                        }
                        setRefreshedSubscrId.insert( wstrSubscrId );
                    }

                    if (bAddSubscr)
                    {
						KLERR_TRY
						{
							CAutoPtr<CSubscription> pSubscription;
							pSubscription.Attach( new CSubscription(m_hQueue) );
							KLSTD_CHKMEM(pSubscription);
							pSubscription->InitializeExistingInStorage(m_idAgent, this, wstrSubscrId, m_sEventStoragesFolder, parSubscription );
							m_mapSubscriptions[wstrSubscrId] = pSubscription;
						}
						KLERR_CATCH(pError)
						{
							KLSTD_TRACE1( 1, L"PRES ERROR: cannot read subscription %ls. Description followed:\n", wstrSubscrId.c_str() );
							KLERR_SAY_FAILURE( 1, pError );
						}
						KLERR_ENDTRY;
                    }
                }
            }
        }

        pJournalForRead->Next();
    }

    if (bRefresh)
    {
        MapSubscriptions::iterator itMapSubscr = m_mapSubscriptions.begin();
        while(itMapSubscr != m_mapSubscriptions.end())
        {
            if ((setRefreshedSubscrId.find( itMapSubscr->first) == setRefreshedSubscrId.end()) &&
                (itMapSubscr->second.m_T->IsPersistent()))
            {
                m_mapSubscriptions.erase(itMapSubscr);
				itMapSubscr = m_mapSubscriptions.begin();
            } 
			else
                itMapSubscr++;
        }
    }

    if (bPausedSubscriptions) m_bPausedSubscriptionRead = true;

	KLSTD_TRACE1( 1, L"Subscriptions read from %ls\n", m_sStorageFilePath.c_str() );
}

void CSubscriptionList::Initialize(const KLPRCI::ComponentId& idAgent,
								   const std::wstring sStorageFilePath,
								   const std::wstring sEventStoragesFolder,
								   bool  bSubscribeToEventsToStore)
{
	if (m_bInitialized) return;

	KLSTD_TRACE0(1, L"[KLPRES] Initializing subscription list\n");

	m_idAgent = idAgent;
    m_bSubscribeToEventsToStore = bSubscribeToEventsToStore;

	m_sEventStoragesFolder = sEventStoragesFolder;

	// Initialize settings storage
	m_sStorageFilePath = sStorageFilePath;

    {
        wstring wstrDir, wstrName, wstrExt;
        KLSTD_SplitPath( m_sStorageFilePath, wstrDir, wstrName, wstrExt );
        wstrName = PAUSED_SUBSCR_STORAGE_PREFIX + wstrName;
        KLSTD_MakePath( wstrDir, wstrName, wstrExt, m_sPausedStorageFilePath );
    }

	m_mapSubscriptions.clear();

    ReadSubscriptionsFromCentralJournal( false, false );
    m_bPausedSubscriptionRead = false;

	_Unsubscribe();

	KLSTD_TRACE0(1, L"[KLPRES] Subscribe on subscription list modification notification events\n");

    KLEV::SubscriptionOptions soForEventsFromOtherPRESs;
    soForEventsFromOtherPRESs.local = false;
    soForEventsFromOtherPRESs.sendToAgent = true;

    ComponentId cidPublisher( m_idAgent.productName, m_idAgent.version, L"", L"" );

	m_arNotificationEventsSubscriptionID.push_back(
        KLEVQ::GetEventSourceQueued()->Subscribe(
            m_hQueue,
            m_idAgent,
			cidPublisher, 
			PRES_EVENT_SUBSCRIPTION_ADDED,
			NULL,
			SubscriptionListStorageEventCallback,
			(void*)this,
            soForEventsFromOtherPRESs
        )
    );

	m_arNotificationEventsSubscriptionID.push_back(
        KLEVQ::GetEventSourceQueued()->Subscribe(
            m_hQueue,
            m_idAgent,
			cidPublisher, 
			PRES_EVENT_SUBSCRIPTION_UPDATED,
			NULL,
			SubscriptionListStorageEventCallback,
			(void*)this,
            soForEventsFromOtherPRESs
        )
    );

	m_arNotificationEventsSubscriptionID.push_back(
        KLEVQ::GetEventSourceQueued()->Subscribe(
            m_hQueue,
            m_idAgent,
			cidPublisher, 
			PRES_EVENT_SUBSCRIPTION_DELETED,
			NULL,
			SubscriptionListStorageEventCallback,
			(void*)this,
            soForEventsFromOtherPRESs
        )
    );

	KLSTD_TRACE0(1, L"[KLPRES] Subscription list initialized\n");

	m_bInitialized = true;
}

wstring CSubscriptionList::ResetIterator(const KLPRCI::ComponentId &cidFilter, int nTimeout)
{
	KLSTD_TRACE0(4, L"[KLPRES] ResetIterator...\n");
	MAKE_SURE_THAT_INITIALIZED;

    // persistent subscriptions:
    CAutoPtr<SubscrIterator> pSubscrIterator;
    pSubscrIterator.Attach( new SubscrIterator );
    KLSTD_CHKMEM(pSubscrIterator);
    
    pSubscrIterator->pJournal = _GetCentralJournal( AF_READ, false );
    pSubscrIterator->pJournal->ResetIterator();
    pSubscrIterator->cidFilter = cidFilter;


    // non-persistent subscriptions:
	KLPAR_CreateParams( & (pSubscrIterator->parNonPersSubscriptions) );
    pSubscrIterator->nNonPersIdx = 0;

    ComponentId cidSubscriber;
    CAutoPtr<Params> parSubscription;
    MapSubscriptions::iterator it;
    it = m_mapSubscriptions.begin();
	for(; it != m_mapSubscriptions.end(); it++ )
	{
		KLSTD_TRACE2(4, L"[KLPRES] ResetIterator subsId - '%ls' persistant - %d\n", 
			it->second.m_T->m_sID.c_str(), it->second.m_T->IsPersistent() );
		if( ! it->second.m_T->IsPersistent() )
        {
            parSubscription = it->second.m_T->m_parSubscriptionInfo;
            GET_CID_PARAMS_VALUE(parSubscription, SS_VALUE_NAME_SUBSCRIPTION_SUBSCRIBER, cidSubscriber);

		    if (CompareComponentID(cidFilter, cidSubscriber))
		    {
			    CAutoPtr<ParamsValue> val;
			    CreateValue(parSubscription, &val);
			    pSubscrIterator->parNonPersSubscriptions->AddValue(
                    it->second.m_T->m_sID, val);
                pSubscrIterator->vectNonPersSubscrIds.push_back( it->second.m_T->m_sID );
		    }
        }
	}

    // storing iterator:
    wstring wstrSubscrIteratorId;
    if( nTimeout > 0 )
    {
        wstrSubscrIteratorId = m_pTSSubscrIterators->Insert( pSubscrIterator, nTimeout * 1000 );
    } else
    {
        wstrSubscrIteratorId = LOCAL_SUBSCR_ITER_ID_PREFIX + KLSTD_CreateLocallyUniqueString();
        m_mapSubscrIterators[wstrSubscrIteratorId] = pSubscrIterator;
    }
    
	m_listOpenedSubscriptionsIteratorIds.push_back(wstrSubscrIteratorId);

	KLSTD_TRACE0(4, L"[KLPRES] ResetIterator...OK\n");

    return wstrSubscrIteratorId;
}

bool IsLocalSubscrIteratorId( const std::wstring & wstrSubscrIteratorId )
{
    const wchar_t * pszStr = wstrSubscrIteratorId.c_str();
    return pszStr == wcsstr( pszStr, LOCAL_SUBSCR_ITER_ID_PREFIX );
}

CAutoPtr<SubscrIterator> CSubscriptionList::GetSubscrIterator( const std::wstring & wstrSubscrIteratorId )
{
    CAutoPtr<SubscrIterator> pSubscrIterator;

    if( IsLocalSubscrIteratorId( wstrSubscrIteratorId ) )
    {
        MapSubscrIterators::iterator it = m_mapSubscrIterators.find( wstrSubscrIteratorId );
        if( it == m_mapSubscrIterators.end() )
        {
            KLSTD_TRACE1( 1,
                L"Cannot find subscription iterator %s.", wstrSubscrIteratorId.c_str() );
            KLSTD_THROW( STDE_NOTFOUND );
        }
        pSubscrIterator = it->second;
    } else
    {
        m_pTSSubscrIterators->GetAt( wstrSubscrIteratorId, (KLBase**)(& pSubscrIterator) );
        m_pTSSubscrIterators->Prolong( wstrSubscrIteratorId );
    }

    return pSubscrIterator;
}

void CSubscriptionList::_ReleaseAllIterators()
{
	std::list<std::wstring>::iterator it = m_listOpenedSubscriptionsIteratorIds.begin();
	while(it != m_listOpenedSubscriptionsIteratorIds.end())
	{
		ReleaseIterator(*it);
		it++;
	}
}

void CSubscriptionList::ReleaseIterator(const std::wstring & wstrSubscrIteratorId)
{
    // Держим CAutoPtr на удаляемый объект, дабы он освободился только вместе с 
    // освобождением этого CAutoPtr. Тем самым мы избегаем освобождения объекта
    // в контейнере, что может привести к deadlock'у. Избегаем также удаления объекта
    // их детсруктора итератора - по тем же причинам. 
    CAutoPtr<SubscrIterator> pTemp;
    if( IsLocalSubscrIteratorId( wstrSubscrIteratorId ) )
    {
        MapSubscrIterators::iterator it =
            m_mapSubscrIterators.find( wstrSubscrIteratorId );
        if( it != m_mapSubscrIterators.end() )
        {
            pTemp = it->second;
            m_mapSubscrIterators.erase( it );
        }
    } 
	else
        m_pTSSubscrIterators->Remove( wstrSubscrIteratorId, (KLBase**)(& pTemp ) );
	
	std::list<std::wstring>::iterator it = std::find(
		m_listOpenedSubscriptionsIteratorIds.begin(), 
		m_listOpenedSubscriptionsIteratorIds.end(), 
		wstrSubscrIteratorId);
	if (it != m_listOpenedSubscriptionsIteratorIds.end()) m_listOpenedSubscriptionsIteratorIds.erase(it);
}

bool CSubscriptionList::GetNextSubscriptionInJournal(Journal* pJournal,
													 SubscrIterator* pSubscrIterator,
													 std::wstring& wstrSubscrId,
													 Params** pparSubscription)
{
	KLSTD_TRACE0( 1, L"[GetNextSubscriptionInJournal] begin\n"  );
    bool bResult = false;

    while( ! pJournal->IsEOF() && ! bResult )
    {
        long nId;
        string strData;
    
		KLSTD_TRACE0( 1, L"[GetNextSubscriptionInJournal] Getting current record...\n" );
        pJournal->GetCurrent( nId, strData );
		KLSTD_TRACE1( 1, L"[GetNextSubscriptionInJournal] Got current record, id is %d\n", nId );
        
        // bool bPaused;
        CAutoPtr<Params> parSubscription;
        if( GetFromStrData( strData, wstrSubscrId, /*bPaused,*/ & parSubscription ) )
        {
            pair< SetSubscriptions::iterator, bool > pr =
                pSubscrIterator->setPassedSubscriptions.insert( wstrSubscrId );
            if( pr.second == true )
            {
                ComponentId cidSubscriber;
                GET_CID_PARAMS_VALUE(parSubscription, SS_VALUE_NAME_SUBSCRIPTION_SUBSCRIBER, cidSubscriber);
            
                if( CompareComponentID( pSubscrIterator->cidFilter, cidSubscriber ) )
                {
                    bResult = true;
                    *pparSubscription = parSubscription.Detach();
                }
            }
        }
		
		KLSTD_TRACE1( 1, L"[GetNextSubscriptionInJournal] Got subscription %ls, going to next record...\n", wstrSubscrId.c_str() );

        pJournal->Next();

		KLSTD_TRACE1( 1, L"[GetNextSubscriptionInJournal] successful move to next record.\n", wstrSubscrId.c_str() );
    }

	KLSTD_TRACE1( 1, L"[GetNextSubscriptionInJournal] end, result is %d\n", long(bResult) );

    return bResult;
}

bool CSubscriptionList::GetNextSubscription(const std::wstring& wstrSubscrIteratorId,
											std::wstring& wstrSubscrId,
											Params** pparSubscription)
{
    KLSTD_CHKOUTPTR(pparSubscription);
    CAutoPtr<SubscrIterator> pSubscrIterator = GetSubscrIterator(wstrSubscrIteratorId);

    bool bResult = GetNextSubscriptionInJournal(pSubscrIterator->pJournal,
												pSubscrIterator,
												wstrSubscrId,
												pparSubscription);
    if (!bResult)
    {
        if (!pSubscrIterator->pJournalWithPausedSubscr)
        {
            pSubscrIterator->pJournalWithPausedSubscr = _GetCentralJournal( AF_READ, true );
            pSubscrIterator->pJournalWithPausedSubscr->ResetIterator();
        }

        bResult = GetNextSubscriptionInJournal(pSubscrIterator->pJournalWithPausedSubscr,
											   pSubscrIterator,
											   wstrSubscrId,
											   pparSubscription);
    }

    if (!bResult)
    {
        if ((unsigned)pSubscrIterator->nNonPersIdx < pSubscrIterator->vectNonPersSubscrIds.size())
        {
			KLPAR::ParamsPtr parSubscription;
            wstring wstrSubscrId = pSubscrIterator->vectNonPersSubscrIds[pSubscrIterator->nNonPersIdx];

            GET_PARAMS_VALUE(pSubscrIterator->parNonPersSubscriptions, wstrSubscrId.c_str(), ParamsValue, PARAMS_T, parSubscription);
            *pparSubscription = parSubscription.Detach();
            pSubscrIterator->nNonPersIdx++;
            bResult = true;
        }
    }

    if( ! bResult )
    {
        ReleaseIterator( wstrSubscrIteratorId );
    }

    return bResult;
}


void CSubscriptionList::AddSubscription(const std::wstring sSubscriptionID, const bool bPersistent, KLPAR::Params* parSubscription, std::wstring& sID)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(parSubscription);

	if (sSubscriptionID.empty())
		sID = KLSTD_CreateGUIDString();
	else
		sID = sSubscriptionID;

    KLSTD_TRACE1(4, L"[KLPRES] Add new subscription, id is '%ls'...\n", sID.c_str());

	CAutoPtr<CSubscription> pSubscription;
    pSubscription.Attach( new CSubscription(m_hQueue) );
    KLSTD_CHKMEM( pSubscription );
    pSubscription->InitializeNew(m_idAgent, this, sID, bPersistent, parSubscription, m_sEventStoragesFolder);
	KLSTD_CHKMEM(pSubscription);

	m_mapSubscriptions[sID] = pSubscription;
}

void CSubscriptionList::RemoveSubscription(const std::wstring sID, const bool bFromStorage)
{
	MAKE_SURE_THAT_INITIALIZED;

    // Держим CAutoPtr на удаляемый объект, дабы он освободился только вместе с 
    // освобождением этого CAutoPtr. Тем самым мы избегаем освобождения объекта
    // в контейнере, что может привести к deadlock'у. Избегаем также удаления объекта
    // их деструктора итератора - по тем же причинам.
    CAutoPtr<CSubscription> pTemp;
    {
        MapSubscriptions::iterator it = m_mapSubscriptions.begin();
	    for(; it != m_mapSubscriptions.end(); it++)
	    {
		    if (it->second.m_T->m_sID == sID)
		    {
                pTemp = it->second;
			    m_mapSubscriptions.erase(it);
			    if (bFromStorage) pTemp->Remove();

			    break;
		    }
	    }
    }
}

// Gets pointer to subscription from list (not copy!!!)
KLSTD::CAutoPtr<CSubscription> CSubscriptionList::GetSubscriptionByID(const std::wstring& sID)
{
	MAKE_SURE_THAT_INITIALIZED;

    MapSubscriptions::iterator it = m_mapSubscriptions.find(sID);
	if (it == m_mapSubscriptions.end())
	{	// Подписки нет в map'e. Перечитываем подписки из хранилища...
		ReadSubscriptionsFromCentralJournal(false, false);	// ...Читаем активные подписки...

		it = m_mapSubscriptions.find(sID);
		if (it != m_mapSubscriptions.end())
		{
			return it->second.m_T;
		}
		else
		{	
			if (!m_bPausedSubscriptionRead)
			{	// ...Читаем неактивные подписки
				ReadSubscriptionsFromCentralJournal(true, false);
				it = m_mapSubscriptions.find(sID);
				if (it != m_mapSubscriptions.end()) return it->second.m_T;
			}
		}
	}
	else
		return it->second.m_T;

	return NULL;
}

void CSubscriptionList::OnAddSubscription(const std::wstring sSubscriptionID, const KLPRCI::ComponentId& publisher)
{
	MAKE_SURE_THAT_INITIALIZED;

    if (publisher == m_idAgent) return;

	if (m_mapSubscriptions.find(sSubscriptionID) == m_mapSubscriptions.end())
    {
        string strSerializedSubscrParams;
        CAutoPtr<Journal> pJournal = _GetCentralJournal(ACCESS_FLAGS(AF_READ), false);
        CAutoPtr<Journal> pJournalWithPausedSubscr = _GetCentralJournal(ACCESS_FLAGS(AF_READ), true);

        MoveJrnlPosToSubscription(pJournal, sSubscriptionID, strSerializedSubscrParams);

        bool bFound = !pJournal->IsEOF();
        if (!bFound)
        {
            MoveJrnlPosToSubscription( pJournalWithPausedSubscr, sSubscriptionID, strSerializedSubscrParams );
            bFound = !pJournalWithPausedSubscr->IsEOF();
        }

        if (bFound)
        {
			KLPAR::ParamsPtr parSubscription;
            KLPAR_DeserializeFromMemory( strSerializedSubscrParams.c_str(), strSerializedSubscrParams.size(), & parSubscription );

		    CAutoPtr<CSubscription> pSubscription;
            
			pSubscription.Attach(new CSubscription(m_hQueue));
            KLSTD_CHKMEM(pSubscription);

            pSubscription->InitializeExistingInStorage(m_idAgent, this, sSubscriptionID, m_sEventStoragesFolder, parSubscription );
		    KLSTD_CHKMEM(pSubscription);

		    m_mapSubscriptions[sSubscriptionID] = pSubscription;
        }
    }
}

void CSubscriptionList::OnUpdateSubscription(const std::wstring sSubscriptionID, const KLPRCI::ComponentId& publisher)
{
	MAKE_SURE_THAT_INITIALIZED;

    if (publisher == m_idAgent) return;

    MapSubscriptions::iterator it = m_mapSubscriptions.find(sSubscriptionID);
	if (it == m_mapSubscriptions.end())
    {
        OnAddSubscription(sSubscriptionID, publisher);
    }
	else
    {
        string strSerializedSubscrParams;
        CAutoPtr<Journal> pJournal = _GetCentralJournal(ACCESS_FLAGS(AF_READ), false);
        CAutoPtr<Journal> pJournalWithPausedSubscr = _GetCentralJournal(ACCESS_FLAGS(AF_READ), true);

        MoveJrnlPosToSubscription( pJournal, sSubscriptionID, strSerializedSubscrParams );
        bool bFound = !pJournal->IsEOF();
        if (!bFound)
        {
            MoveJrnlPosToSubscription(pJournalWithPausedSubscr, sSubscriptionID, strSerializedSubscrParams);
            bFound = !pJournalWithPausedSubscr->IsEOF();
        }

        if (bFound)
        {
			KLPAR::ParamsPtr parSubscription;
            KLPAR_DeserializeFromMemory(strSerializedSubscrParams.c_str(), strSerializedSubscrParams.size(), &parSubscription);

            it->second.m_T->Reinit(parSubscription);
        }
		else
            OnDeleteSubscription(sSubscriptionID, publisher);
    }
}

void CSubscriptionList::OnDeleteSubscription(const std::wstring sSubscriptionID, const KLPRCI::ComponentId& publisher)
{
	MAKE_SURE_THAT_INITIALIZED;

	if (publisher == m_idAgent) return;

    KLSTD_TRACE1(1, L"[KLPRES] OnDeleteSubscription '%ls'\n", sSubscriptionID.c_str());
	if (m_mapSubscriptions.find(sSubscriptionID) != m_mapSubscriptions.end())
    {
	    KLSTD_TRACE1(3, L"[KLPRES] OnDeleteSubscription:RemoveSubscription '%ls'\n", sSubscriptionID.c_str());
		RemoveSubscription(sSubscriptionID, false);
    }
}

void CSubscriptionList::RefreshFromDisk()
{
	MAKE_SURE_THAT_INITIALIZED;
    
    ReadSubscriptionsFromCentralJournal(false, true);
    m_bPausedSubscriptionRead = false;
}

/////////////////////////////////////////////////////////////////////////////////

void CSubscriptionList::_Unsubscribe()
{
	for(std::vector<KLEVQ::HSUBSCRIPTION>::iterator i = m_arNotificationEventsSubscriptionID.begin();i != m_arNotificationEventsSubscriptionID.end();i++)
	{
		if (*i != KLEVQ::UndefinedSubId)
		{
            KLERR_BEGIN
			    KLSTD_TRACE1(4, L"[KLPRES] Subscription List unsubscribes from Subscription ID '%X'\n", *i);
			    KLEVQ::GetEventSourceQueued()->Unsubscribe(m_hQueue, *i);
    			*i = KLEVQ::UndefinedSubId;
            KLERR_ENDT(1)
		}
	}

	m_arNotificationEventsSubscriptionID.clear();
}

bool CSubscriptionList::CompareComponentID(const KLPRCI::ComponentId& CIDFilter, const KLPRCI::ComponentId& CID2)
{
#define CHECK_FIELD(field) (CIDFilter.field.size() == 0?true:(CIDFilter.field == CID2.field))

	return (CHECK_FIELD(productName) &&
			CHECK_FIELD(version) &&
			CHECK_FIELD(componentName) &&
			CHECK_FIELD(instanceId));
}

void CSubscriptionList::SubscriptionListStorageEventCallback(const KLPRCI::ComponentId& subscriber,
														     const KLPRCI::ComponentId& publisher,
														     const KLEV::Event *event,
														     void *context)
{
    KLSTD_TRACE1(1, L"[KLPRES] Subscription list: got event \"%ls\"\n", event->GetType().c_str() );

	KLERR_BEGIN
	{
		KLSTD_CHKINPTR(context);
		KLSTD_CHKINPTR(event);

		KLTMSG::AsyncCall2T<CSubscriptionList, const KLPRCI::ComponentId, CAutoPtr<KLEV::Event> >::Start(
			reinterpret_cast<CSubscriptionList*>(context), 
			&CSubscriptionList::OnInternalPRESEvent, 
			publisher, 
			CAutoPtr<KLEV::Event>(const_cast<KLEV::Event*>(event)));
	}
	KLERR_END

	KLSTD_TRACE1(1, L"[KLPRES] Subscription list: event \"%ls\" processed.\n", event->GetType().c_str() );
}

void CSubscriptionList::OnInternalPRESEvent(const KLPRCI::ComponentId publisher, KLSTD::CAutoPtr<KLEV::Event> event)
{
	KLERR_BEGIN
	{
	    if (event)
	    {
			KLPAR::ParamsPtr pEventBody = event->GetParams();
		    if (!pEventBody) return;

			std::wstring wstrEventType = event->GetType();
            if ((wstrEventType == PRES_EVENT_SUBSCRIPTION_ADDED) ||
                (wstrEventType == PRES_EVENT_SUBSCRIPTION_UPDATED) ||
                (wstrEventType == PRES_EVENT_SUBSCRIPTION_DELETED))
            {
			    std::wstring sSubscriptionID;
			    GET_PARAMS_VALUE(pEventBody, PRES_EVENT_BODY_PARAM_SUBSCR_NAME, StringValue, STRING_T, sSubscriptionID);

			    if (wstrEventType == PRES_EVENT_SUBSCRIPTION_ADDED)
				    OnAddSubscription(sSubscriptionID, publisher);
			    else
			    if (wstrEventType == PRES_EVENT_SUBSCRIPTION_UPDATED)
				    OnUpdateSubscription(sSubscriptionID, publisher);
			    else
			    if (wstrEventType == PRES_EVENT_SUBSCRIPTION_DELETED)
				    OnDeleteSubscription(sSubscriptionID, publisher);
            }
	    }
	}
    KLERR_ENDT(1)
}

////////////////////////////////////////////////////////////////////////////////
// CEventInfoMaskItem

CEventInfoMaskItem::CEventInfoMaskItem(const KLPAR::Params* parEventInfoMaskItem)
{
	KLSTD_CHKINPTR(parEventInfoMaskItem);

	GET_PARAMS_VALUE(parEventInfoMaskItem, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK_VALUE_NAME, StringValue, STRING_T, m_sValueName);
	GET_PARAMS_VALUE(parEventInfoMaskItem, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK_VALUE_TYPE, IntValue, INT_T, m_nValueType);
	GET_PARAMS_VALUE(parEventInfoMaskItem, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK_VALUE_ID, IntValue, INT_T, m_nID);
}

void CEventInfoMaskItem::Serialize(KLPAR::Params** pparEventInfoMaskItem)
{
	KLSTD_CHKOUTPTR(pparEventInfoMaskItem);

	CAutoPtr<Params> pParams;
	KLPAR_CreateParams(&pParams);

	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK_VALUE_NAME, StringValue, m_sValueName.c_str());
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK_VALUE_TYPE, IntValue, m_nValueType);
	ADD_PARAMS_VALUE(pParams, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK_VALUE_ID, IntValue, m_nID);

	*pparEventInfoMaskItem = pParams.Detach();
}

////////////////////////////////////////////////////////////////////////////////
// CEventInfoMask

CEventInfoMask::CEventInfoMask()
	:m_nEventInfoMaskCurrentUniqueID(START_PROPERTY_ID)	
{
}

void CEventInfoMask::Serialize(KLPAR::Params* parEventInfoMask)
{
	KLSTD_CHKINPTR(parEventInfoMask);

	if (Count() == 0) return;

	CAutoPtr<ValuesFactory> pValuesFactory;
	KLPAR_CreateValuesFactory(&pValuesFactory);
	CAutoPtr<ArrayValue> valArray;
	pValuesFactory->CreateArrayValue(&valArray);
	valArray->SetSize(Count());
	
	for(unsigned int i = 0;i < Count();i++) 
	{
		CAutoPtr<Params> pItemParams;
		(*this)[i]->Serialize(&pItemParams);

		CAutoPtr<ParamsValue> pVal;
		CreateValue(pItemParams, &pVal);
		valArray->SetAt(i, pVal);
	}

	parEventInfoMask->AddValue(SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK, valArray);
}

void CEventInfoMask::Deserialize(const KLPAR::Params* parEventInfoMask)
{
	KLSTD_CHKINPTR(parEventInfoMask);

	CAutoPtr<ArrayValue> valArray = (ArrayValue*)parEventInfoMask->GetValue2(SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK, false);
	if (valArray)
	{
		KLPAR_CHKTYPE(valArray, ARRAY_T, SS_VALUE_NAME_SUBSCRIPTION_EVENT_INFO_MASK);
		for(int i = 0;i < (int)valArray->GetSize();i++) 
		{
			CAutoPtr<ParamsValue> pVal = (ParamsValue*)valArray->GetAt(i);
			_NewItemFromParams(pVal->GetValue());
		}
	}
}

bool CEventInfoMask::Check(const wchar_t* pszValueName, long& nPropertyID)
{
	KLSTD_CHKINPTR(pszValueName);

	for(unsigned int i = 0;i < Count();i++)
	{
		if (wcscmp((*this)[i]->m_sValueName.c_str(), pszValueName) == 0) 
		{
			nPropertyID = (*this)[i]->m_nID;
			return true;
		}
	}

	return false;
}

CEventInfoMaskItem* CEventInfoMask::GetById(long nID)
{
	for(unsigned int i = 0;i < Count();i++)
	{
		if ((*this)[i]->m_nID == nID) return (*this)[i];
	}

	return NULL;
}

void CEventInfoMask::ParseMask(const KLPAR::Params* parEventParametersMask, std::vector<std::wstring>& arEventParametersSortMask)
{
	std::wstring wstrPrefix;
	_ParseMask(parEventParametersMask, wstrPrefix);

	// Sort fields by sort mask
	if (arEventParametersSortMask.size() > 0)
	{
		for (unsigned int i = 0;i < arEventParametersSortMask.size();i++)
		{
			for (unsigned int j = 0;j < Count();j++)
			{
				if ((*this)[j]->m_sValueName == arEventParametersSortMask[i])
				{
					if (i != j) Insert(i, RemoveInd(j, noDelete));
					break;
				}
			}
		}
	}
}

void CEventInfoMask::_ParseMask(const KLPAR::Params* parEventParametersMask, std::wstring wstrPrefix)
{
	if (parEventParametersMask)
	{
		ParamsNames arParamsNames;
		parEventParametersMask->GetNames(arParamsNames);
		std::sort(arParamsNames.begin(), arParamsNames.end());

		for(ParamsNames::iterator i = arParamsNames.begin();i != arParamsNames.end();i++)
		{
          CEventInfoMaskItem* pEventInfoMaskItem = NULL;
          std::wstring tmp;
			CAutoPtr<Value> pValue;
			((KLPAR::Params*)parEventParametersMask)->GetValue(*i, &pValue);
			switch (pValue->GetType()) {
			case Value::PARAMS_T:                
			{
                //<-- Changed by andkaz 29.04.2003 13:26:07
                //ParseMask(((ParamsValue*)(Value*)pValue)->GetValue());
				_ParseMask(((ParamsValue*)(Value*)pValue)->GetValue(), wstrPrefix.empty()?(*i):(wstrPrefix + KLPRES_EVENT_NAME_DIVIDER + *i));
                // -->
			}
			break;
			default:
                //<-- Changed by andkaz 29.04.2003 13:26:19
                //Add(new CEventInfoMaskItem(*i, pValue->GetType(), m_nEventInfoMaskCurrentUniqueID++));
              tmp = wstrPrefix.empty() ? L"": (wstrPrefix + KLPRES_EVENT_NAME_DIVIDER);
              tmp += *i;
              pEventInfoMaskItem =
                  new CEventInfoMaskItem( tmp,
											pValue->GetType(),
											m_nEventInfoMaskCurrentUniqueID++);
              KLSTD_CHKMEM( pEventInfoMaskItem );
				Add( pEventInfoMaskItem );
                // -->
			}
		}
	}
}

void CEventInfoMask::_NewItemFromParams(const KLPAR::Params* parEventInfoMaskItem)
{
	KLSTD_CHKINPTR(parEventInfoMaskItem);

	CPointer<CEventInfoMaskItem> pNewItem = new CEventInfoMaskItem(parEventInfoMaskItem);
    KLSTD_CHKMEM( pNewItem );
	Add(pNewItem.Relinquish());
}

}
