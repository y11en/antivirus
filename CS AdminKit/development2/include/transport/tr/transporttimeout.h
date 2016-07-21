/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file TransportTimeout.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Класс для расчета timeout'ов при работе с транспортом
 * 
 */


#ifndef KLTR_TRANSPORT_TIMEOUT_H
#define KLTR_TRANSPORT_TIMEOUT_H

#include "std/err/error.h"
#include "errors.h"

namespace KLTRAP {

	const int c_DefaultTimeout				= 10000;				// 10 сек
	const int c_DefaultMaxTransportTimeout	= 24 * 60 * 60 * 1000;	// 24 часа

	class TransportTimeout
	{
        //<-- Changed by andkaz 07.05.2004 18:33:07
    public:
        // -->    
		TransportTimeout( int connectTimeout = c_DefaultTimeout, int maxTimeout = c_DefaultMaxTransportTimeout )
		{        
            Create(connectTimeout,maxTimeout);
		}
        
        void Create( int connectTimeout = c_DefaultTimeout, int maxTimeout = c_DefaultMaxTransportTimeout )
		{
			m_timeout = connectTimeout;
			m_timeoutDelta = connectTimeout;
			m_initiateTimeout = m_timeout;
			m_maxTimeout = maxTimeout;			
		}
        

		/*!
		    \brief Увеличивает timeout
		*/
		int Increment()
		{
			if ( m_timeout < m_maxTimeout )
				m_timeout += m_timeoutDelta;

			return m_timeout;
		}

		/*!
		    \brief Уменьшает timeout
		*/
		int Decrement()
		{
			if ( (m_timeout - m_timeoutDelta) > 0 )
				m_timeout -= m_timeoutDelta;

			return m_timeout;
		}

		/*!
		    \brief Анализирует код ошибку и в случае необходимости увеличивает timeout
		*/
		void CheckFault( KLERR::Error *pErr )
		{
			if ( std::wstring(pErr->GetModuleName())==KLTRAP_ModuleName )
			{
				if ( pErr->GetId()==TRERR_REMOTE_SERVER_IS_BUSY ||
					pErr->GetId()==TRERR_TIMEOUT ) 
				{
					Increment();
				}
				else
				{
					// в случае всех остальных ошибок сбрасываем на default таймаут
					Create( m_initiateTimeout, m_maxTimeout );
				}
			}
		}

		/*!
		    \brief Сигнал об успешной операции соединения 
					( в частном случае функция уменьшает timeout )
		*/
		void GoodAttempt()
		{
			Decrement();
		}

		/*!
			\brief Возвращает текущее значение timeout'а
		*/
		int Get()
		{
			return m_timeout;
		}

	private:
		
		int m_timeout;
		int m_timeoutDelta;
		int m_initiateTimeout;
		int m_maxTimeout;
	};

};

#endif // KLTR_TRANSPORT_TIMEOUT_H
