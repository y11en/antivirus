 /*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file stubrouter.cpp
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Реализация функция для работы с stub функциями gSOAP
 *
 */
#include <string>
#include <map>

#include "std/base/kldefs.h"
#include "std/base/klbase.h"
#include "std/thr/sync.h"
#include "std/trc/trace.h"

#include "std/gsoap/stubrouter.h"
#include "stdsoap2.h"
#include "soapStub.h"

SOAPComponentId::SOAPComponentId()
{
}

namespace KLSTRT{

	class StubRouter
	{
	public:
		StubRouter()
		{
			KLSTD_CreateCriticalSection( &m_pCricSec );
		}

		~StubRouter() {}

		void AddFunctionsArray( SOAPStubFunc *array, int array_size )
		{
			KLSTD::AutoCriticalSection unlocker( m_pCricSec );

			for( int i = 0; i < array_size; i++ )
			{
				if ( array[i].funcName==NULL ||
					array[i].funcPtr==NULL ) break;
				m_funcMap[array[i].funcName]=array[i].funcPtr;
			}
		}

		void DeleteFunctionsArray( SOAPStubFunc *array, int array_size )
		{
			KLSTD::AutoCriticalSection unlocker( m_pCricSec );

			for( int i = 0; i < array_size; i++ )
			{
				if ( array[i].funcName==NULL ) break;
				FunctionMap::iterator fIt = m_funcMap.find(array[i].funcName);
				if ( fIt!=m_funcMap.end() )
				{
					m_funcMap.erase( fIt );
				}				
			}
		}

		void *GetFunctionPtrByName( const wchar_t *funcName )
		{
			void *res = NULL;			

			KLSTD::AutoCriticalSection unlocker( m_pCricSec );

			FunctionMap::iterator fIt = m_funcMap.find( funcName );
			if ( fIt!=m_funcMap.end() ) res = fIt->second;
			else res = NULL;

			KLTRACE5( L"KLSTD", L"GetFunctionPtrByName funcName '%ls' res - %d\n", funcName, res );

			return res;
		}

	private:
		
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCricSec;

		typedef std::map<std::wstring, void *> FunctionMap;

		FunctionMap			m_funcMap;
	};

	StubRouter *g_pStubRouter = NULL;

	//!< Функции де/инициализации подсистемы
	void InitStubRouter()
	{
		if ( g_pStubRouter==NULL )
			g_pStubRouter = new StubRouter();
	}

	void DeinitStubRouter()
	{
		if ( g_pStubRouter!=NULL )
		{
			StubRouter *saveP = g_pStubRouter;
			g_pStubRouter = NULL;
			delete saveP;
		}
	}

	//!\brief Регестрирует gSOAP функции модуля
	void RegisterGSOAPStubFunctions( SOAPStubFunc *array, int array_size )
	{
		if ( g_pStubRouter!=NULL ) 
			g_pStubRouter->AddFunctionsArray( array, array_size );
	}

	//!\brief Удаляет функции из списка зарегестрированных gSOAP функции модуля
	void UnregisterGSOAPStubFunctions( SOAPStubFunc *array, int array_size )
	{
		if ( g_pStubRouter!=NULL ) 
			g_pStubRouter->DeleteFunctionsArray( array, array_size );
	}

	//!\brief Возвращает указатель зарегестрированной функции
	void *GetFunctionPtrByName( const wchar_t *funcName )
	{
		if ( g_pStubRouter!=NULL ) 
		{
			return g_pStubRouter->GetFunctionPtrByName( funcName );
		}
		else return NULL;
	}


}; // end namespace KLSTRT

KLCSC_DECL int klstd_soap_serve(struct soap *soap)
{
	soap_begin(soap);		
	if (soap_begin_recv(soap))
	{	
		if (soap->error < SOAP_STOP)
			return soap_send_fault(soap);
		soap_closesock(soap);
		return soap_send_fault( soap );
	}
	if (soap_envelope_begin_in(soap)
		|| soap_recv_header(soap)
		|| soap_body_begin_in(soap)
		|| soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap)))
		return soap_send_fault(soap);		
	
	return SOAP_OK;
}
