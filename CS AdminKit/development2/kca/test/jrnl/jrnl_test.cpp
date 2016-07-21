#include <std/base/klstd.h>
#include <std/trc/trace.h>
#include <std/thr/thread.h>
#include <std/time/kltime.h>
#include <kca/prts/prxsmacros.h>
#include <kca/pres/eventsstorage.h>
#include <kca/prci/componentid.h>
#include <transport/ev/eventsource.h>
#include <transport/tr/transport.h>

#include "../../jrnl/journal.h"

#include <exception>

#include <testmodule.h>
#include <helpers.h>
#include <stdio.h>

#include "jrnl_test.h"

#define KLCS_MODULENAME L"JRNLTST"

using namespace KLTST2;
using namespace KLSTD;
using namespace KLJRNL;
using namespace KLPAR;
using namespace KLPRCI;
using namespace KLPRES;
using namespace KLEV;
using namespace std;

class Cool : public KLBaseImpl<JournalRecord>
{
public:
    int intVal;
    char charVal;
    wstring wstrVal;
    string strVal;
    int* pArr;
    int nArrSize;
	
    void Serialize( std::string & strData ) const
    {
        if( strVal.empty() )
        {
            int arr[] = {0, 1, 2, 3, 4, 5};
            strData = std::string( (const char*)arr, sizeof(arr) );
        } else
        {
            strData = strVal;
        }
    }
	
    void Deserialize( const std::string & strData )
    {
        strVal = strData;
        pArr = (int*)strVal.c_str();
        nArrSize = strVal.size() / sizeof(int);
		
        /* int nSize = strData.size();
        const int* pArr = (const int*)strData.c_str();
        int j = 5; */
    }
};

class Cool2 : public KLBaseImpl<JournalRecord>
{
public:
    int intVal;
    string str;
    wstring wstr; 
	
    void Serialize( std::string & strData ) const
    {
		//#if defined(_WIN32) && defined(_UNICODE)
        /* wchar_t wszBuf[100];
        swprintf( wszBuf, L"wchar_t! val1: %d; val2: %d, val3: %d", intVal, intVal*10, intVal*100 );
		
        strData = string( (const char*)wszBuf, wcslen( wszBuf )* sizeof( wchar_t ) ); */
		//#else        
        char szBuf[100];
        KLSTD_SPRINTF( szBuf, sizeof(szBuf), "val1: %010d; val2: %010d, val3: %010d", intVal, intVal*10, intVal*100 );
		
        strData = szBuf;
        //strData = str;
		//#endif
    }
	
    void Deserialize( const std::string & strData )
    {
        str = strData;
        //wstr = wstring( (wchar_t*)strData.c_str(), strData.size() / 2 );
        //int j = 5;
    }
};

const wchar_t TEST_EVENT_TYPE_1[] = L"TEST_EVENT_TYPE_1";
const wchar_t TEST_EVENT_TYPE_2[] = L"OnDemandScanner_EA_ObjectIdGenericIO";

const wchar_t TST_EVENT_BODY_PATH[] = L"TST_EVENT_BODY_PATH";
const wchar_t TST_EVENT_BODY_DATE1[] = L"TST_EVENT_BODY_DATE1";
const wchar_t TST_EVENT_BODY_STATUS1[] = L"TST_EVENT_BODY_STATUS1";
const wchar_t TST_EVENT_BODY_DATE2[] = L"TST_EVENT_BODY_DATE2";
const wchar_t TST_EVENT_BODY_STATUS2[] = L"TST_EVENT_BODY_STATUS2";


void TestBug()
{
	wstring wstrJrnlPath;
	wstrJrnlPath = L"c:\\test\\jrnl\\SRhuomilN7nkh30O8SNvs0.jrnl";
	
	CAutoPtr<Journal> pJournal;
	KLJRNL_CreateJournal( & pJournal, true );
	pJournal->Open( wstrJrnlPath, CF_OPEN_EXISTING, ACCESS_FLAGS( AF_WRITE | AF_READ ) );
	
	long nId;
	string strData;
	pJournal->ResetIterator();
	while( ! pJournal->IsEOF() )
	{
		pJournal->GetCurrent( nId, strData );
		pJournal->Next();
	}
}

void DoTestsWithMyPRES()
{
	CAutoPtr<EventsStorage> pEventsStorage;
	
	ComponentId compId( L"Test product", L"Test version", L"Test component", L"Test instance" );
	
	if( true )
	{
		KLPRES_CreateEventsStorageServer(
			L"Test server",
			L"Test product",
			L"Test version",
			L"c:\\test\\jrnl\\pres.pres",
			L"c:\\test\\jrnl\\pres" );
		
		KLPRES_CreateEventsStorageProxy( L"Test server",
			ComponentId(),
			ComponentId(),
			& pEventsStorage,
			true );
	}
	
	wstring wstrTrServerCompName;
	int serverUsedPort;
	wstring trLocalCompName;
	
	if( false )
	{
		// получаем транспортное имя компоненты
		KLTRAP::ConvertComponentIdToTransportName(wstrTrServerCompName, compId);
		KLTR_GetTransport()->SetTimeouts(300000, 300000, 1000);
		// добавляем location сервера для входящих соединений ( порт location'а может быть любым )
		// используется для сохранения порта
		
		KLTR_GetTransport()->AddListenLocation(wstrTrServerCompName.c_str(), L"http://127.0.0.1:17071", serverUsedPort);
		
		KLPRES_CreateEventsStorageServer(
			L"Test server",
			L"Test product",
			L"Test version",
			L"c:\\test\\jrnl\\pres.pres",
			L"c:\\test\\jrnl\\pres" );
		
		KLPRCI::ComponentId cidLocalComponent( L"KLPRES_TEST_PRODUCT", L"1.0", KLSTD_CreateLocallyUniqueString().c_str(), L"1");
		
		KLTRAP::ConvertComponentIdToTransportName(trLocalCompName, cidLocalComponent);
		KLTR_GetTransport()->AddClientConnection(trLocalCompName.c_str(), wstrTrServerCompName.c_str(), L"http://127.0.0.1:17071" );
		
		KLPRES_CreateEventsStorageProxy( L"Test server",
			cidLocalComponent,
			compId,
			& pEventsStorage );
	}
	
	if( true )
	{
		std::wstring sID;
		SubscriptionInfo si;
		si.subscriber = compId;
		si.filter = compId;
		si.maxEvents = KLSTD_INFINITE;
		si.eventTypes.push_back( TEST_EVENT_TYPE_1 );
		
		CAutoPtr<Params> parBody;
		KLPAR_CreateParams(&parBody);
		ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_PATH, StringValue, L"");
		ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_DATE1, DateTimeValue, 0);
		ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_STATUS1, StringValue, L"");
		ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_DATE2, DateTimeValue, 0);
		ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_STATUS2, StringValue, L"");
		
		KLPAR_CreateParams(&si.parEventParametersMask);
		ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_TYPE, IntValue, 0);
		ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_TIME, DateTimeValue, 0);
		ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_PARAMS, ParamsValue, parBody);
		
		si.isSubscriptionPersist = true;
		
		pEventsStorage->StartSubscription(si, sID);
		
		si.eventTypes.push_back( TEST_EVENT_TYPE_2 );
		pEventsStorage->UpdateSubscription(sID, si);
		pEventsStorage->PauseSubscription( sID );
		pEventsStorage->ResumeSubscription( sID );
		
		KLSTD_Sleep(20000);
	}
	
	if( false )
	{
		pEventsStorage->ResetSubscriptionsIterator( ComponentId() );
        
	}
	
	//
	
	/*
	if( false )
	{
		std::wstring sID;
		int nTotalEventCount = 0;
		long dwTotalTickCount = KLSTD::GetSysTickCount();
		do
		{
			CPointer<SubscriptionInfo> pSI;
			if (! pEventsStorage->GetNextSubscription( & pSI, sID) ) break;
			
			printf( "\n...Start to read subscription %S...", sID.c_str() );
			
			unsigned long dwTickCount = KLSTD::GetSysTickCount();
			long nEventCount = 0;
			
			pEventsStorage->ResetEventsIterator( sID );
			
			bool bNextEventOk = false;
			do
			{
				KLPRCI::ComponentId cidPublisher;
				std::wstring sEventType;
				long eventID;
				CAutoPtr<Params> parEventBody;
				time_t timeBirth = 0;
				
				//TestTickCount ttcGetEvent( "GetEvent", 20 );
				bool bEvent =
					pEventsStorage->GetEvent(
					cidPublisher,
					sEventType,
					& parEventBody,
					timeBirth,
					eventID );
				
				
				if( ! bEvent ) break;
				bNextEventOk = pEventsStorage->NextEvent();
			}
			while( bNextEventOk );
			
			printf( "\n.....%d events read, tick count is %d", nEventCount, KLSTD::GetSysTickCount() - dwTickCount );
			nTotalEventCount += nEventCount;
		}
		while(true);
		
		printf( "\nTotal:  %d events read, tick count is %d\n", nTotalEventCount, KLSTD::GetSysTickCount() - dwTotalTickCount );
	}
	*/
	
	pEventsStorage = NULL;
	KLPRES_DeleteEventsStorageServer( L"Test server" );
    }
	
    class TestTickCount
    {
    public:
        TestTickCount( const char * _szInfo, int _nMinTickCountToShow = 0) 
        {
            //printf( "%s: begin\n", szInfo );
            strInfo = _szInfo;
            bInactive = false;
            nMinTickCountToShow = _nMinTickCountToShow;
            dwStartTickCount = KLSTD::GetSysTickCount();
        }
		
        ~TestTickCount() 
        {
            if( ! bInactive )
            {
                Finish();
            }
        } 
		
        bool Finish()
        {
            bool bResult = false;
            
            unsigned long dwTickCount = KLSTD::GetSysTickCount() - dwStartTickCount;
            if( dwTickCount >= (unsigned)nMinTickCountToShow )
            {
//                printf( "\n...%s, %d msec", strInfo.c_str(), dwTickCount );
                bResult = true;
            }
            //printf( "...%s, %03f sec\n", strInfo.c_str(), double(KLSTD::GetSysTickCount() - dwStartTickCount) / 1000.0 );
            bInactive = true;
			
            return bResult;
        }
		
    private:
        unsigned long dwStartTickCount;
        string strInfo;
        bool bInactive;
        int nMinTickCountToShow;
    };
	
    void ReadTestPRES2()
    {
        unsigned long dwTotalTickCount = KLSTD::GetSysTickCount();
        KLPRES_CreateEventsStorageServer(
            L"Test server",
            L"Test product",
            L"Test version",
            L"c:\\test\\jrnl\\slowdata\\events.xml",
            L"c:\\test\\jrnl\\slowdata\\events" );
		
        printf( "\nPRES Server created, %d msec...", KLSTD::GetSysTickCount() - dwTotalTickCount );
		
        //ComponentId compId( L"Test product", L"5.0.0.0", L"", L"Test instance" );
		
        CAutoPtr<EventsStorage> pEventsStorage;
        KLPRES_CreateEventsStorageProxy( L"Test server",
			ComponentId(),
			ComponentId(),
			& pEventsStorage,
			true );
		
        printf( "\nPRES Proxy created, %d msec...", KLSTD::GetSysTickCount() - dwTotalTickCount );
        printf( "\nStart to iterate subscriptions..." );
		
        pEventsStorage->ResetSubscriptionsIterator( ComponentId() );
        
        int nSubscr = 0;
        int nPausedSubscr = 0;
        std::wstring sID;
        vector<wstring> vectPausedSubscr;
        do
		{
			CPointer<SubscriptionInfo> pSI;
			if (! pEventsStorage->GetNextSubscription( & pSI, sID) ) break;
            nSubscr++;
			
            if( pSI->bPaused )
            {
                vectPausedSubscr.push_back( sID );
                nPausedSubscr++;
            }
			
            printf( "\n...%ls, paused: %d", sID.c_str(), int(pSI->bPaused) );
        }
        while(true);
		
        printf( "\nFinish, %d msec, %d subscriptions, %d paused...",
            KLSTD::GetSysTickCount() - dwTotalTickCount,
            nSubscr,
            nPausedSubscr );
		
        //pEventsStorage->ResumeSubscription( vectPausedSubscr[10] );
        /* for( int i = 0; i < vectPausedSubscr.size(); i++ )
        {
		pEventsStorage->ResumeSubscription( vectPausedSubscr[i] );
		pEventsStorage->PauseSubscription( vectPausedSubscr[i] );
	} */
    }
	
    void ReadTestPRES()
    {
        KLPRES_CreateEventsStorageServer(
            L"Test server",
            L"Test product",
            L"Test version",
            L"c:\\test\\jrnl\\pres.pres",
            L"c:\\test\\jrnl\\pres" );
		
        //ComponentId compId( L"Test product", L"5.0.0.0", L"", L"Test instance" );
		
        CAutoPtr<EventsStorage> pEventsStorage;
        KLPRES_CreateEventsStorageProxy( L"Test server",
			ComponentId(),
			ComponentId(),
			& pEventsStorage,
			true );
		
        printf( "\nStart to read events..." );
		
        unsigned long dwTotalTickCount = KLSTD::GetSysTickCount();
        long nTotalEventCount = 0;
		
        pEventsStorage->ResetSubscriptionsIterator( ComponentId() );
        
        std::wstring sID;
        do
		{
			CPointer<SubscriptionInfo> pSI;
			if (! pEventsStorage->GetNextSubscription( & pSI, sID) ) break;
			
            printf( "\n...Start to read subscription %S...", sID.c_str() );
			
            unsigned long dwTickCount = KLSTD::GetSysTickCount();
            long nEventCount = 0;
			
            pEventsStorage->ResetEventsIterator( sID );
			
            bool bNextEventOk = false;
			do
			{
				KLPRCI::ComponentId cidPublisher;
				std::wstring sEventType;
				long eventID;
				CAutoPtr<Params> parEventBody;
				time_t timeBirth = 0;
				
                //TestTickCount ttcGetEvent( "GetEvent", 20 );
                bool bEvent =
                    pEventsStorage->GetEvent(
					cidPublisher,
					sEventType,
					& parEventBody,
					timeBirth,
					eventID );
				
                //if( ttcGetEvent.Finish() )
                {
					//  printf( " (%ds event)", nEventCount );
                }
				
				if( ! bEvent ) break;
				/* CAutoPtr<Params> parParamParam;
				GET_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_PARAM, ParamsValue, PARAMS_T, parParamParam);
				std::wstring sParamText;
				GET_PARAMS_VALUE(parParamParam, EVENT_BODY_PARAM_PARAM_TEXT, StringValue, STRING_T, sParamText);
				
				  int nTest;
                GET_PARAMS_VALUE(parEventBody, TST_EVENT_BODY_PATH, IntValue, INT_T, nTest); */
				
                wstring wstrPath, wstrStatus1, wstrStatus2;
                time_t t1, t2;
                GET_PARAMS_VALUE(parEventBody, TST_EVENT_BODY_PATH, StringValue, STRING_T, wstrPath);
                GET_PARAMS_VALUE(parEventBody, TST_EVENT_BODY_DATE1, DateTimeValue, DATE_TIME_T, t1);
                GET_PARAMS_VALUE(parEventBody, TST_EVENT_BODY_STATUS1, StringValue, STRING_T, wstrStatus1);
                GET_PARAMS_VALUE(parEventBody, TST_EVENT_BODY_DATE2, DateTimeValue, DATE_TIME_T, t2);
                GET_PARAMS_VALUE(parEventBody, TST_EVENT_BODY_STATUS2, StringValue, STRING_T, wstrStatus2);
				
                char * test = ctime(&t2);
				
				nEventCount++;
				
                //TestTickCount ttcNextEvent( "NextEvent", 20 );
                bNextEventOk = pEventsStorage->NextEvent();
                //if( ttcNextEvent.Finish() )
                {
					//  printf( " (%ds event)", nEventCount );
                }
			}
			while( bNextEventOk );
			
            printf( "\n.....%d events read, tick count is %d", nEventCount, KLSTD::GetSysTickCount() - dwTickCount );
            nTotalEventCount += nEventCount;
		}
		while(true);
		
        printf( "\nTotal:  %d events read, tick count is %d\n", nTotalEventCount, KLSTD::GetSysTickCount() - dwTotalTickCount );
		
        KLPRES_DeleteEventsStorageServer( L"Test server" );
    }
	
    void CreateTestPRES()
    {
        unsigned long dwTickCount = KLSTD::GetSysTickCount();
		
        KLPRES_CreateEventsStorageServer(
            L"Test server",
            L"Test product",
            L"Test version",
            L"c:\\test\\jrnl\\pres.pres",
            L"c:\\test\\jrnl\\pres" );
		
        ComponentId compId( L"Test product", L"Test version", L"Test component", L"Test instance" );
		
        {
            std::wstring sID;
			SubscriptionInfo si;
			si.subscriber = compId;
			si.filter = compId;
			si.maxEvents = KLSTD_INFINITE;
			si.eventTypes.push_back( TEST_EVENT_TYPE_2 );
            si.nCloseWriteOpenedJournalTimeout = 10000;
            si.nStoreEventsAs = SubscriptionInfo::SI_STORE_AS_TEXT;
			
			CAutoPtr<Params> parBody;
			KLPAR_CreateParams(&parBody);
			
            ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_PATH, StringValue, L"");
            ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_DATE1, DateTimeValue, 0);
            ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_STATUS1, StringValue, L"");
            ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_DATE2, DateTimeValue, 0);
            ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_STATUS2, StringValue, L"");
			
			KLPAR_CreateParams(&si.parEventParametersMask);
			ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_TYPE, StringValue, L"");
			ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_TIME, DateTimeValue, 0);
			ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_PARAMS, ParamsValue, parBody);
			
			si.isSubscriptionPersist = true;
			
            CAutoPtr<EventsStorage> pEventsStorage;
            KLPRES_CreateEventsStorageProxy( L"Test server",
				ComponentId(),
				ComponentId(),
				& pEventsStorage,
				true );
			
			pEventsStorage->StartSubscription(si, sID);
        }
		
        printf( "Start publish events...\n" );
		
        {
            wchar_t szwPath[1024];
            for( int i = 0; i < 10000; i++)
            {
				CAutoPtr<Params> parBody;
				KLPAR_CreateParams(&parBody);
				
                KLSTD_SWPRINTF( szwPath, sizeof(szwPath), L"E:\\Temp\\%d.exe", i );
				
                ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_PATH, StringValue, szwPath);
                ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_DATE1, DateTimeValue, 0);
                ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_STATUS1, StringValue, L"OnDemandScanner_EA_StatusIdOK");
                ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_DATE2, DateTimeValue, time(0) );
                ADD_PARAMS_VALUE(parBody, TST_EVENT_BODY_STATUS2, StringValue, L"OnDemandScanner_EV_ObjectOK");
				
                CAutoPtr<Event> pEvent;
                KLEV_CreateEvent( & pEvent, compId, TEST_EVENT_TYPE_2, parBody, time(0) );
                KLEV_GetEventSource()->PublishEvent( pEvent );
				
                /* if( div( i, 100 ).rem == 0 )
                {
				Sleep( 1000 );
			} */
            }
        }
		
        KLEV_GetEventSource()->WaitForCleanEventsQueue( L"", true );
		
        printf( "Finished publishing events - %d ms\n", KLSTD::GetSysTickCount() - dwTickCount );
		
        //Sleep( 10000 );
		
        KLPRES_DeleteEventsStorageServer( L"Test server" );
    }
	
    void ReadTestJournal()
    {
        CAutoPtr<Journal> pJournal;
        KLJRNL_CreateJournal( & pJournal, true );
		
        KLJRNL::CreationInfo creationInfo;
		
        creationInfo.bLimitPages = false;
        creationInfo.pageLimitType = KLJRNL::pltRecCount;
        creationInfo.nMaxRecordsInPage = 10;
        creationInfo.bBinaryFormat = true;
		
        pJournal->ChangeLimits( L"c:\\test\\jrnl\\slowdata\\events.xml", creationInfo );
		
        return;
		
		
        wstring wstrJrnlPath;
        //wstrJrnlPath = L"c:\\test\\jrnl\\cool.txt";
        wstrJrnlPath = L"C:\\Test\\Jrnl\\pres\\kI0M1LkehpabvX6Yvo_j91.jrnl";
        pJournal->Open( wstrJrnlPath, CF_OPEN_ALWAYS, ACCESS_FLAGS( AF_READ ) );
        printf( "Will read test journal at \"%ls\".\n", wstrJrnlPath.c_str() );
		
        unsigned long dwTickCount = KLSTD::GetSysTickCount();
		
        long nId = 0, nCount = 0;
		
        pJournal->ResetIterator();
        string strData;
        while( ! pJournal->IsEOF() )
        {
            nCount++;
            pJournal->GetCurrent( nId, strData );
			
            pJournal->Next();
        } 
		
        printf(
            "Done read test journal, last id is %d, record count is %d, tick count is %d.\n",
            nId,
            nCount,
            KLSTD::GetSysTickCount() - dwTickCount );
		
		
			/* pJournal->MoveToRecordN( 9999 );
			long nId;
			string strData;
			pJournal->GetCurrent( nId, strData ); 
			
			  KLSTD_TRACE3(1,
			  L"Done MoveToRecordN, id is %d, record is %hs, tick count is %d.\n",
			  nId,
			  strData.c_str(),
		KLSTD::GetSysTickCount() - dwTickCount ); */
		
        /* string strData;
        unsigned long dwTickCount = KLSTD::GetSysTickCount();
        long nId;
        pJournal->SetIterator( 5000 );
        unsigned long dwEndTickCount = KLSTD::GetSysTickCount() - dwTickCount;
		
		  pJournal->GetCurrent( nId, strData );
		  
			KLSTD_TRACE3(1,
            L"Tick count is %d, id is %d, record is %hs.\n",
            KLSTD::GetSysTickCount() - dwTickCount,
            nId,
		strData.c_str() ); */
		
		
		/*
        int n20Plus = 0;
        int n20 = 0;
        int n0 = 0;
        for( int i = 0; i < 100000; i++ )
        {
		unsigned long dwTickCount = KLSTD::GetSysTickCount();
		long nId;
		pJournal->SetIterator( i );
		unsigned long dwEndTickCount = KLSTD::GetSysTickCount() - dwTickCount;
		if( dwEndTickCount == 0 )
		{
		n0++;
		} else if( dwEndTickCount < 20 )
		{
		n20++;
		} else            
		{
		n20Plus++;
		string strData;
		
		  pJournal->GetCurrent( nId, strData );
		  
			KLSTD_TRACE2(1,
			L"Tick count is %d, id is %d.\n",
			KLSTD::GetSysTickCount() - dwTickCount,
			nId ); 
            }
			}
			
			  KLSTD_TRACE3(1,
			  L"Done set iterator. n0=%d, n20=%d, n20Plus=%d.\n",
			  n0, n20, n20Plus ); 
			  
		*/
        /* unsigned long dwTickCount = KLSTD::GetSysTickCount();
        long nId = 0, nCount = 0;
		
		  pJournal->ResetIterator();
		  Cool2 cool2;
		  while( ! pJournal->IsEOF() )
		  {
		  nCount++;
		  pJournal->GetCurrent( nId, & cool2 );
		  
            pJournal->Next();
			} 
			
			  KLSTD_TRACE3(1,
			  L"Done read test journal, last id is %d, record count is %d, tick count is %d.\n",
			  nId,
			  nCount,
		KLSTD::GetSysTickCount() - dwTickCount ); */
    }
	
    void CreateTestJournal()
    {
        CAutoPtr<Journal> pJournal;
        KLJRNL_CreateJournal( & pJournal, true );
		
        wstring wstrJrnlPath;
        //KLSTD_GetTempFile( wstrJrnlPath );
        wstrJrnlPath = L"c:\\test\\jrnl\\cool.txt";
        KLSTD_TRACE1(1, L"Will create test journal at \"%s\".\n", wstrJrnlPath.c_str() );
		
        CreationInfo creationInfo;
        creationInfo.bLimitPages = false;
        creationInfo.bBinaryFormat = true;
        creationInfo.pageLimitType = pltRecCount;
        creationInfo.nMaxRecordsInPage = 10000;
		
        pJournal->Open( wstrJrnlPath, CF_OPEN_ALWAYS, ACCESS_FLAGS( AF_WRITE | AF_READ ), creationInfo );
		
        char * szLongBuf = new char[1024];
        Cool2 cool2;
        {
            for( int i = 0; i < 1024; i++ )
            {
#ifdef N_PLAT_NLM
                szLongBuf[i] = 30 + stlp_std::div( i, 30 ).rem;
#else
                szLongBuf[i] = 30 + div( i, 30 ).rem;
#endif
            }
        }
        szLongBuf[1023] = 0;
		
        char szBuf[100];
		
        unsigned long dwTickCount = KLSTD::GetSysTickCount();
		
        for( int i = 0; i < 100000; i++ )
        {
            sprintf( szBuf, "%d", i );
            memcpy( szLongBuf, szBuf, strlen(szBuf) );
            cool2.str = szLongBuf;
            long nId = pJournal->Add( & cool2 );
            pJournal->Flush();
        }
		
        KLSTD_TRACE1( 1, L"Time with flush: %d msec\n", KLSTD::GetSysTickCount() - dwTickCount );
    }
	
    void CheckSimpleJournal()
    {
		KLSTD_TRACE0(1, L"Obtaining journal object...\n" );
		
        CAutoPtr<Journal> pJournal;
        KLJRNL_CreateSimpleJournal( & pJournal, true );
		
        wstring wstrJrnlPath;
        //wstrJrnlPath = L"c:\\test\\jrnl\\cool.txt";
        KLSTD_GetTempFile( wstrJrnlPath );
        KLSTD_Unlink( wstrJrnlPath.c_str(), false );
        KLSTD_TRACE1(1, L"Will create journal at \"%s\".\n", wstrJrnlPath.c_str() );
		
        CreationInfo creationInfo;
        creationInfo.bLimitPages = false;
        creationInfo.pageLimitType = pltSize;
        creationInfo.nMaxRecordsInPage = 5;
        creationInfo.nMaxPageSize = 1024;
		
        Cool2 cool2;
		
        pJournal->Open( wstrJrnlPath, CF_OPEN_ALWAYS, ACCESS_FLAGS( AF_READ | AF_WRITE ), creationInfo );
		
        for( int i = 0; i < 20; i++ )
        {
            cool2.intVal = i;
            pJournal->Add( & cool2 );
        }
		
        AVP_longlong llSize = pJournal->GetJournalSize();
        KLSTD_TRACE1( 1, L"Journal size: %d bytes\n", long(llSize) ); 
		
        pJournal->Flush();
		
        bool bFoo = false;
        long nTestCount = 0;
		
        nTestCount = 0;
        pJournal->ResetIterator();
        KLSTD_TRACE0( 1, L"Getting records...\n" );
        while( ! pJournal->IsEOF() )
        {
            long nId;
            Cool2 cool2;
            pJournal->GetCurrent( nId, & cool2 );
			
            bFoo = nId < 5;
			
            KLSTD_TRACE2( 1, L"Id: %d, data: %S\n", nId, cool2.str.c_str() );
			
			
            if( bFoo )
            {
                pJournal->DeleteCurrent();
            } else
            {
                pJournal->Next();
            }
            nTestCount++;
        }
		
        if( nTestCount != 20 )
        {
            KLSTD_THROW(STDE_BADFORMAT);
        }
		
        nTestCount = 0;
        KLSTD_TRACE0( 1, L"Getting records - check MoveToRecordN...\n" );
        pJournal->MoveToRecordN( 30 );
        while( ! pJournal->IsEOF() )
        {
            long nId;
            Cool2 cool2;
            pJournal->GetCurrent( nId, & cool2 );
			
            KLSTD_TRACE2( 1, L"Id: %d, data: %S\n", nId, cool2.str.c_str() );
			
			
            pJournal->Next();
            nTestCount++;
        }
		
        pJournal->Flush();
		
        pJournal->DeleteOlderThanID( 14 );
		
        long nId = 0;
        nTestCount = 0;
        pJournal->ResetIterator();
        while( ! pJournal->IsEOF() )
        {
            Cool2 cool2;
            nTestCount++;
            pJournal->GetCurrent( nId, & cool2 );
            pJournal->Next();
        }
		
        if( ( nId != 20 ) || ( nTestCount != 6 ) )
        {
            KLSTD_THROW(STDE_BADFORMAT);
        }
		
        nTestCount = 0;
        pJournal->SetIterator(15);
        while( ! pJournal->IsEOF() )
        {
            Cool2 cool2;
            nTestCount++;
            pJournal->GetCurrent( nId, & cool2 );
            pJournal->Next();
        }
		
        if( ( nId != 20 ) || ( nTestCount != 6 ) )
        {
            KLSTD_THROW(STDE_BADFORMAT);
        }
		
        pJournal->Flush();
		
        nTestCount = 0;
        pJournal->ResetIterator();
        while( ! pJournal->IsEOF() )
        {
            Cool2 cool2;
            pJournal->GetCurrent( nId, & cool2 );
            if( nTestCount == 5 )
            {
                cool2.intVal = nTestCount * 100;
                pJournal->Update( & cool2 );
            }
            pJournal->Next();
            nTestCount++;
        }
		
        KLSTD_TRACE1( 1, L"Journal record count: %d\n", pJournal->GetRecordCount() );
		
        pJournal->DeleteAll();
		
        pJournal->DeleteOlderThanID( 10 );
		
        //pair<long, long>()
        //pJournal
		
        pJournal->CloseAndRemoveFiles(); 
    }
	
    void DoTest( bool bBinary )
    {
		KLSTD_TRACE0(1, L"Obtaining journal object...\n" );
		
        CAutoPtr<Journal> pJournal;
        KLJRNL_CreateJournal( & pJournal, true );
		
        wstring wstrJrnlPath;
        KLSTD_GetTempFile( wstrJrnlPath );
		
        //wstrJrnlPath = L"c:\\test\\jrnl\\cool.txt";
		
        KLSTD_Unlink( wstrJrnlPath.c_str(), false );
        KLSTD_TRACE1(1, L"Will create journal at \"%s\".\n", wstrJrnlPath.c_str() );
		
        CreationInfo creationInfo;
        creationInfo.bLimitPages = false;
        creationInfo.bBinaryFormat = bBinary;
        creationInfo.pageLimitType = pltRecCount;
        creationInfo.nMaxRecordsInPage = 5;
        creationInfo.bUseRecNumIndex = true;
		
        pJournal->Open( wstrJrnlPath, CF_CREATE_ALWAYS, ACCESS_FLAGS( AF_WRITE | AF_READ ), creationInfo );
		
        CAutoPtr<Params> pParams;
        KLPAR_CreateParams( & pParams );
		
        ADD_PARAMS_VALUE( pParams, L"param0", StringValue, L"agent" );
        ADD_PARAMS_VALUE( pParams, L"param1", StringValue, L"kuku01" );
        ADD_PARAMS_VALUE( pParams, L"param2", BoolValue, false );
		
        Cool cool;
		
        cool.intVal = 1;
        cool.charVal = 'c';
        cool.wstrVal = L"Hello";
        //cool.strVal = "Kuku";
		
        for( int i = 0; i < 22; i++ )
        {
            long nId = pJournal->Add( & cool );
        }
		
        pJournal->Close();
        pJournal->Open( wstrJrnlPath, CF_OPEN_EXISTING, ACCESS_FLAGS( AF_WRITE | AF_READ ), creationInfo );
		
        AVP_longlong llSize = pJournal->GetJournalSize();
        KLSTD_TRACE1( 1, L"Journal size: %d bytes\n", long(llSize) );
		
        pJournal->Flush();
		
        KLSTD_TRACE0( 1, L"Getting records - check MoveToRecordN...\n" );
		
        pJournal->DeleteOlderThanID( 3 );
        pJournal->Flush();
		
        int nRecCount = pJournal->GetRecordCount();
		
        long nTestCount = 0;
        pJournal->MoveToRecordN( 5 );
        while( ! pJournal->IsEOF() )
        {
            long nId;
            Cool cool;
            pJournal->GetCurrent( nId, & cool );
            bool bTest = false;
            if( nId > 11 && nId < 15 )
            {
                cool.strVal = "Hello guys how you doing";
                pJournal->Update( & cool );
            }
            KLSTD_TRACE2( 1, L"Id: %d, data: %S\n", nId, cool.strVal.c_str() );
            nTestCount++;
            pJournal->Next();
        } 
		
        if( nTestCount != 14 )
        {
            KLSTD_TRACE0( 1, L"Error with MoveToRecordN\n" );
            KLSTD_THROW(STDE_BADFORMAT);
        }
		
		
        KLSTD_TRACE0( 1, L"Getting records - check SetIterator...\n" );
        pJournal->SetIterator(3);
		
        long nId;
        nTestCount = 0;
        while( ! pJournal->IsEOF() )
        {
            Cool cool;
            pJournal->GetCurrent( nId, & cool );
            KLSTD_TRACE2( 1, L"Id: %d, data: %S\n", nId, cool.strVal.c_str() );
            pJournal->Next();
            nTestCount++;
        } 
		
        if( nTestCount != 19 )
        {
            KLSTD_TRACE0( 1, L"Error with SetIterator\n" );
            KLSTD_THROW(STDE_BADFORMAT);
        }
        
        KLSTD_TRACE2( 1, L"Journal record count: %d, last id is %d\n", pJournal->GetRecordCount(), nId );
		
        //pJournal->DeleteOlderThanID( 10 );
		
        KLSTD_TRACE1( 1, L"Journal record count: %d\n", pJournal->GetRecordCount() );
		
        pJournal->ResetIterator();
        while( ! pJournal->IsEOF() )
        {
            long nId;
            Cool2 cool2;
            pJournal->GetCurrent( nId, & cool2 );
			
            KLSTD_TRACE2( 1, L"Id: %d, data: %S\n", nId, cool2.str.c_str() );
            pJournal->Next();
        }
		
        pair<long, long> range;
        vector< pair<long, long> > vectRanges;
		
        range.first = 2;
        range.second = 4;
        vectRanges.push_back( range );
		
        range.first = 21;
        range.second = 7;
        vectRanges.push_back( range );
		
		
        pJournal->DeleteRanges( vectRanges );
		
        pJournal->Flush();
		
        pJournal->DeleteAll();
		
        pJournal->DeleteOlderThanID( 10 );
		
        pJournal->CloseAndRemoveFiles();
    }
	
	DECLARE_MODULE_INIT_DEINIT( KLSTD );
	DECLARE_MODULE_INIT_DEINIT( KLPRES );
	DECLARE_MODULE_INIT_DEINIT( KLPRSS );
	DECLARE_MODULE_INIT_DEINIT( KLEV );
	DECLARE_MODULE_INIT_DEINIT( KLPRTS );
	DECLARE_MODULE_INIT_DEINIT( KLTMSG );
	DECLARE_MODULE_INIT_DEINIT( KLPRCI );
	DECLARE_MODULE_INIT_DEINIT( KLPRCP );
	
	
	void CJRNLGeneralTest2::Initialize(KLTST2::TestData2* pTestData)
	{
		::KLSTD_Initialize();
		::KLPAR_Initialize();
		::KLPRSS_Initialize();
		::KLTP_Initialize();
		::KLTR_Initialize();
		::KLEV_Initialize();
		::KLPRTS_Initialize();
		::KLPRES_Initialize();
		::KLTMSG_Initialize();
		::KLPRCI_Initialize();
		::KLPRCP_Initialize();
	};
	KLSTD_NOTHROW void CJRNLGeneralTest2::Deinitialize() throw()
	{
		KLERR_BEGIN
			::KLPRCP_Deinitialize();
		::KLPRCI_Deinitialize();
		::KLTMSG_Deinitialize();
		::KLPRES_Deinitialize();
		::KLPRTS_Deinitialize();
		::KLEV_Deinitialize();
		::KLTR_Deinitialize();
		::KLTP_Deinitialize();
		::KLPRSS_Deinitialize();
		::KLPAR_Deinitialize();
		::KLSTD_Deinitialize();
		KLERR_END
	};
	long CJRNLGeneralTest2::Run()
	{
		bool bResult = false;
		KLSTD_TRACE0(1, L"\n");
		KLSTD_TRACE0(1, L"Starting test JRNL_General...\n");
		KLERR_TRY
			if( true )
            {
				KLSTD_TRACE0(1, L"Checking binary format...\n" );
                DoTest( true );
				
				KLSTD_TRACE0(1, L"Checking text format...\n" );
                DoTest( false );
				
                CheckSimpleJournal();
            }
			
            if( false )
            {
                CreateTestJournal();
            }
			
            if( false )
            {
                ReadTestJournal();
            }
			
            if( false )
            {
                CreateTestPRES();
                
            }
			
            if( false )
            {
                ReadTestPRES2();
            }
			
            if( false )
            {
                ReadTestPRES();
            }
			
            if( false )
            {
                DoTestsWithMyPRES();
            }
			
            if( false )
            {
                TestBug();
            }
			
			KLSTD_TRACE0(1, L"Everything looks OK.\n" );
			bResult = true;
			KLERR_CATCH(pError)
				KLERR_SAY_FAILURE(1, pError);			
			KLERR_ENDTRY
				KLSTD_TRACE1(1, L"...Finishing test JRNL_General, bResult=%d\n", bResult); 
			return bResult?0:(-1);
	};
	
	
