/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TransportProxy.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Интерфейс модуля proxy для работы с транспортной библиотекой.
 *
 *  Данный класс предназначен для построения proxy объектов в клиентском приложении.
 *  Клиентский класс должен быть наследован от это класса и должен осуществлять вызовы
 *  серверных методов при помощи предоставленного данным классом интерфейсом.
 */


#ifndef KLTR_TRANSPORTPROXY_H
#define KLTR_TRANSPORTPROXY_H

#include <string>

#include "std/base/klbase.h"

#include "transport/tr/common.h"
#include "transport/tr/errors.h"

struct soap;

namespace KLTRAP {

	class Transport;
	
	class KLCSTR_DECL TransportProxy 
	{
	public:		

		TransportProxy()
		{
			localComponentName = connectionName = NULL;
		}

		virtual ~TransportProxy() 
		{
			if ( localComponentName!=NULL ) delete[] localComponentName;
			if ( connectionName!=NULL ) delete[] connectionName;
		}

	/*!
      \brief Функция инициализации.

      \param localComponentName [in] Наименование клиента, от лица которого будет производиться вызов с 
						удаленные вызовы
      \param remoteComponentName [in] Наименование удаленного клиента
      \exception TRERR_INVALID_PARAMETER, TRERR_CANT_ESTABLISH_CONNECTION 
    */
        void Initialize( const wchar_t *localComponentName, const wchar_t *remoteComponentName );
		
	/*!
      \brief Функция возвращает описателя соединения.
		
	  Функция возвращает описатель соединения который представлен в качестве
	  структуру soap. Указатель на данную структуру можеть использоваться для осуществления 
	  вызова сервера. После работы с указателем необходимо вызвать функцию CheckResult с
	  параметром releasePtr = true.
	  Пример типичной работы:
	  int GetMaxSize()	//!< Функция класса наследованного от TransportProxy
	  {
	      int res = 0;

		  struct soap *soap = GetConnectionDesc(); //!< получаем описатель соединения

		  // Вызываем метод сервера ( функция pinf_getmaxsize была сгенерированна компилятором gSOAP)
		  pinf_getmaxsize( soap, NULL, NULL, &res ); 
		  
		  // Проверяем результат выполнения последней операции и освобождаем память
		  CheckResult( &soap, true ); 
	  }
      
      \exception TRERR_NOT_INITIALIZED,  
	  \return	 Указатель на структуру soap. 
    */
        struct soap *GetConnectionDesc( bool lockConnection = true );

	/*!
      \brief Проверяет статус выполнения последней операции.

      Данная callback функция предназначена для оповешения клиента о изменении статуса
	  контролируемого соединения.

      \param soap [in] Описатель соединения, при помощи которого осуществлялся вызов
	  

	  \exception TRERR_TRANSPORT_ERROR, TRERR_WRONG_METHOD, TRERR_WRONG_METHOD_PARAMETERS  
    */
        void CheckResult( struct soap **soap );

	/*!
	  \brief Освобождает описатель соединения

	  Функция освобождения описателя соединения полученного функцией GetConnectionDesc
	  После вызова данной функции указатель на структуру будет равен NULL.
	*/
		void ReleaseConnectionDesc( struct soap **soap );


	private:
		// Внутренние переменные класса
		
		wchar_t *localComponentName;	//!< Имя локальной компоненты
		wchar_t *connectionName;		//!< Имя соединения				
		bool lockConnection;				//!< Признак блокировки данного соединения

	protected:
		void ReleaseConnectionDescAndFillLocation( struct soap **soap, std::wstring *locConn );

	};

	/*
		\brief Вспомогательный класа для работы с TransportProxy

		Данный класс упрощает работу с TransportProxy и автоматически заводит и особождает
		описатель соединения.
	*/
	class TransportConnectionLocker 
	{
	public:
		TransportConnectionLocker( TransportProxy* pProxy )
		{
			ptrProxy = pProxy;
			soap=ptrProxy->GetConnectionDesc(true);
		}

		~TransportConnectionLocker( )
		{
			 ptrProxy->ReleaseConnectionDesc( &soap );
		}
		
		void Release()
		{
			if ( soap!=NULL ) ptrProxy->ReleaseConnectionDesc( &soap );
		}

		struct soap* Get()
		{
			return soap;
		}

		struct soap* CheckResult()
		{
			ptrProxy->CheckResult( &soap );
			return soap;
		}

	protected:
		struct soap*				soap;
		TransportProxy*			ptrProxy;
	};

} // namespace KLTRAP



#endif // KLTR_TRANSPORTPROXY_H

// Local Variables:
// mode: C++
// End:

