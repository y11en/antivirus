/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	wat/common.h
 * \author	Mikhail Karmazine
 * \date	06.02.2003 11:44:00
 * \brief	Декларации, общие для всего namespace'а KLWAT
 * 
 */

#ifndef __KLWAT_COMMON_H__
#define __KLWAT_COMMON_H__

#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <transport/spl/common.h>
#include <string>

namespace KLWAT
{
	/*!
		\brief ClientContext - класс, котролирующий время жизни клиентского контекста,
            полученного с помощью класса AuthServer. Объекты, реализующий этот интерфейс,
            могут быть получены с помощью AuthServer::GenClientContext(). 
            Когда такой объект удаляется, то у сервера (который произвел этот объект) вызывается
            метод ForgetClient.
	*/
    class KLSTD_NOVTABLE ClientContext : public KLSTD::KLBase
    {
    public:
        virtual ~ClientContext () {};
		
		/*!
          \brief	Копирует контекст
		*/
		virtual void CopyTo( ClientContext **ppCntx ) = 0;

        /*!
          \brief	GetClientIdentity - возвращает клиентский ID, который
          можно использовать при вызовая методов AuthServer.

          \return	std::wstring - клиентский ID
        */
        virtual std::wstring GetClientIdentity() = 0;

		/*!
          \brief	Сохраняет список доступа
        */
		virtual void SetPermissions( KLSPL::Permissions *perms ) = 0;

		/*!
          \brief	Возвращает список доступа ( NULL если не опеределен )
        */
		virtual KLSPL::Permissions *GetPermissions() = 0;
		
		/*!
          \brief	Сохраняет идентификационную информацию пользователя
        */
		virtual void SetUserInfo( const std::wstring &wstrUserSystemId, long nUserAccessMapId,
				const std::wstring &wstrAccessMapHash ) = 0;

		/*!
          \brief	Возвращает идентификационную информацию пользователя
        */
		virtual void GetUserInfo( std::wstring &wstrUserSystemId, long &nUserAccessMapId,
				std::wstring &wstrAccessMapHash ) = 0;

		/*!
          \brief	Возвращает идентификационную информацию пользователя
        */
		virtual void GetUserInfo( long &nUserAccessMapId ) = 0;
    };
	
    /*!
	    \brief AuthenticationType - тип аутентификации. 
            atOS - аутентификация производится средствами ОС
            atCustom - самодельная аутентификация через имя/пароль
    */
    enum AuthenticationType { atOS, atCustom };
}

#if defined(__unix) || defined(_MBCS) || defined(N_PLAT_NLM)
# define KLWAT_OFF
#endif // _MBCS

#endif //__KLWAT_COMMON_H__

// Local Variables:
// mode: C++
// End:
